// DebugExprLexAn.cpp
// Debug expression lexical analyser (hand made with 
// state functions, multiple automata).
// ScriptFighter Project.
// A. Savidis, February 2002.
// Revised (some mistakes in the rules resolved), May 2006, July 2008.
//

#include <ctype.h>
#include <assert.h>

#include "DebugExprLexAn.h"
#include "DeltaVirtualMachine.h"
#include "DebugEvaluatedExpr.h"
#include "DebugExprParser.h"
#include "DDebug.h"
#include "ustrings.h"
#include "ufunctors.h"
#include "ulexutil.h"

#include <list>

////////////////////////////////////////////////////////

static char*	buffer			= (char*) 0;
static char*	currPos			= (char*) 0;
static bool		eofWasForced	= false;

////////////////////////////////////////////////////////

bool DebugExprLexAn::LastEOFWasForced (void) 
	{ return eofWasForced; }

////////////////////////////////////////////////////////

void DebugExprLexAn::Initialise (const char* inputBuffer) {
	if (buffer)
		DDELARR(buffer);
	buffer			= ucopystr(inputBuffer);
	currPos			= buffer;
	eofWasForced	= false;
}

////////////////////////////////////////////////////////

void DebugExprLexAn::CleanUp (void) {

	if (buffer) {
		DDELARR(buffer);
		buffer = currPos = (char*) 0;
	}
}

////////////////////////////////////////////////////////
// Token state information for automatically
// progressing the lexical analysis phase.
//

struct TokenStateInformation;
typedef bool (*TokenProcessFunc)(char&, TokenStateInformation*);

struct TokenStateInformation {

	util_ui32			parseState;			// The exact state of processing for this token.
	bool				recognised;			// Has the token been recognised.
	TokenProcessFunc	engagementCheck;	// Check if parsing can be entered. Returns true if yes.
	TokenProcessFunc	parseProgress;		// Progresses minor state. Returns false on failure.
	util_ui16			tokenId;			// BISON specific token id to be returned on recongition.

	void	Reset (void) { parseState = 0; recognised = false; }
	bool	Succeeded (void) const { return recognised; }
};

////////////////////////////////////////////////////////

class IdentifierHandler {

	private:
	char	prefix;
	char	suffix;
	bool	underscoreAllowed;
	bool	hasContent;

	bool	HasSuffix (void) const { return suffix != '\0'; }
	bool	HasPrefix (void) const { return prefix != '\0'; }

	public:
	bool MajorCheck (char& token, TokenStateInformation* info) const
		{ return HasPrefix()? token == prefix : isalpha(token); }

	bool MinorProgress (char& token, TokenStateInformation* info) {

		switch (info->parseState) {

			case 0 : {  
				if (!HasPrefix())		// first token part of the id
					hasContent = true; 
				info->parseState = 1;
				break;
			}

			case 1 :
				if (isalnum(token) || (underscoreAllowed && token == '_'))	// part of id?
					{ hasContent = true; break; }
				else
				if (HasSuffix()) {							// only the suffix is eligible
					if (suffix == token && hasContent)		// found and has content
						return info->recognised = true;		// success
					token = info->parseState = 0;			// failure
					return false;
				}
				else {
					token = 0;								// don't consume token
					if (hasContent)							// ok, we have an acceptable id
						return info->recognised = true;		// success
					else {									// empty content, failure
						info->parseState = 0;
						return false;
					}
				}

			default : DASSERT(false);
		}
		return true;
	}

	IdentifierHandler (void) : 
		prefix('\0'), 
		suffix('\0'), 
		underscoreAllowed(true),
		hasContent(false){}

	IdentifierHandler (char _prefix, char _suffix, bool _underscoreAllowed) : 
		prefix(_prefix), 
		suffix(_suffix), 
		underscoreAllowed(_underscoreAllowed),
		hasContent(false){}
};

////////////////////////////////////////////////////////

#define	NO_ARGS

#define	PRODUCE_IDENTIFIER_INFORMATION(name, args)							\
	static IdentifierHandler name##Handler args;							\
static bool name##MajorCheck (char& token, TokenStateInformation* info)		\
	{ return name##Handler.MajorCheck(token, info); }						\
static bool name##MinorProgress (char& token, TokenStateInformation* info)	\
	{  return name##Handler.MinorProgress(token, info); }					\
static TokenStateInformation name##_Information = {							\
	0,																		\
	false,																	\
	name##MajorCheck,														\
	name##MinorProgress,													\
	name																	\
};

////////////////////////////////////////////////////////

PRODUCE_IDENTIFIER_INFORMATION(IDENT,			NO_ARGS)
PRODUCE_IDENTIFIER_INFORMATION(RESERVEDATTR,	('_', '_', false))
PRODUCE_IDENTIFIER_INFORMATION(MONITOR,			('$', '\0', true))
PRODUCE_IDENTIFIER_INFORMATION(ATTRIBUTE,		('@', '\0', true))

////////////////////////////////////////////////////////

class KeywordHandler {
	private:
	std::string kwd;

	public:
	bool MajorCheck (char& token, TokenStateInformation* info) const
		{ return token == kwd[0]; }

	bool MinorProgress (char& token, TokenStateInformation* info) const {
		if (kwd.length() == info->parseState) { // Keyword fully recognised.
			token = 0;
			info->recognised = true;
		} 
		else
		if (token == kwd[info->parseState])		// Character part of keyword.
			++info->parseState;
		else {									// Failure.
			token = info->parseState = 0;					
			return false; 
		}
		return true;
	}

	KeywordHandler (const char* _kwd) : kwd(_kwd){}
};

////////////////////////////////////////////////////////

#define	PRODUCE_KEYWORD_INFORMATION(name, kwd)								\
	static KeywordHandler name##Handler(kwd);								\
static bool name##MajorCheck (char& token, TokenStateInformation* info)		\
	{ return name##Handler.MajorCheck(token, info); }						\
static bool name##MinorProgress (char& token, TokenStateInformation* info)	\
	{  return name##Handler.MinorProgress(token, info); }					\
static TokenStateInformation name##_Information = {							\
	0,																		\
	false,																	\
	name##MajorCheck,														\
	name##MinorProgress,													\
	name																	\
};

////////////////////////////////////////////////////////

PRODUCE_KEYWORD_INFORMATION(TRUE, "true")
PRODUCE_KEYWORD_INFORMATION(FALSE, "false")
PRODUCE_KEYWORD_INFORMATION(AND, "and")
PRODUCE_KEYWORD_INFORMATION(OR, "or")
PRODUCE_KEYWORD_INFORMATION(NOT, "not")
PRODUCE_KEYWORD_INFORMATION(NIL, "nil")
PRODUCE_KEYWORD_INFORMATION(LAMBDA, "@lambda")

PRODUCE_KEYWORD_INFORMATION(LOCAL, "local")
PRODUCE_KEYWORD_INFORMATION(FORMAL, "function")
PRODUCE_KEYWORD_INFORMATION(FUNCRES, "::")
PRODUCE_KEYWORD_INFORMATION(STATIC, "static")
PRODUCE_KEYWORD_INFORMATION(NEW, "$new")
PRODUCE_KEYWORD_INFORMATION(DELETE, "$delete")

PRODUCE_KEYWORD_INFORMATION(GE, ">=")
PRODUCE_KEYWORD_INFORMATION(LE, "<=")
PRODUCE_KEYWORD_INFORMATION(EQ, "==")
PRODUCE_KEYWORD_INFORMATION(NE, "!=")
PRODUCE_KEYWORD_INFORMATION(GT, ">")
PRODUCE_KEYWORD_INFORMATION(LT, "<")

PRODUCE_KEYWORD_INFORMATION(DOUBLE_DOT, "..")
PRODUCE_KEYWORD_INFORMATION(DOT, ".")
PRODUCE_KEYWORD_INFORMATION(DOUBLE_LEFT_SB, "[[")
PRODUCE_KEYWORD_INFORMATION(DOUBLE_RIGHT_SB, "]]")
PRODUCE_KEYWORD_INFORMATION(SINGLE_LEFT_SB, "[")
PRODUCE_KEYWORD_INFORMATION(SINGLE_RIGHT_SB, "]")

////////////////////////////////////////////////////////

static bool REALCONST_MajorCheck (char& token, TokenStateInformation* info) 
	{ return isdigit(token); }

static bool REALCONST_MinorProgress (char& token, TokenStateInformation* info) {

	switch (info->parseState) {
		case 0 : { // Start.
			if (!isdigit(token)) {			// Digits are ok, but anything else gets checked.
				if (token == '.')			// Start directly fractional part.
					info->parseState = 1;	// Got to fractional part state
				else {
					token = 0;				// Do not consume token.
					info->recognised = true;
				}
			}
			break;
		}
		case 1 : {							// Fractional part.
			if (!isdigit(token)) {			// No digits, end of number constant.
				token = 0;					// Do not consume.
				info->recognised = true;
			}
			break;
		}
		default: DASSERT(false);
	}
	return true;
}

static TokenStateInformation REALCONST_Information = {
	0,
	false,
	REALCONST_MajorCheck,
	REALCONST_MinorProgress,
	REAL_CONST
};

////////////////////////////////////////////////////////

static bool INTCONST_MajorCheck (char& token, TokenStateInformation* info) 
	{ return isdigit(token); }

static bool INTCONST_MinorProgress (char& token, TokenStateInformation* info) {
	switch (info->parseState) {
		case 0 : {
			if (!isdigit(token)) {			// Digits are only acceptable.
				token = 0;					// Do not consume, not part of this.
				info->recognised = true;
			}
			break;
		}
		default: DASSERT(false);
	}
	return true;
}

static TokenStateInformation INTCONST_Information = {
	0,
	false,
	INTCONST_MajorCheck,
	INTCONST_MinorProgress,
	INT_CONST
};

////////////////////////////////////////////////////////

static const char* hexChars = "abcdefABCDEF";

static bool HEXINTCONST_MajorCheck (char& token, TokenStateInformation* info) 
	{ return token == '0'; }

static bool HEXINTCONST_MinorProgress (char& token, TokenStateInformation* info) {

	switch (info->parseState) {

		case 0 : {  info->parseState = 1; break; }

		case 1:
			if (token == 'x' || token == 'X') 
				{ info->parseState = 2; break; }
			else {
				token = info->parseState = 0;
				return false; 
			}

		case 2:
			if (!isdigit(token) && !strchr(hexChars, token)) {
				token = info->parseState = 0;
				return false; 
			}
			else 
				{ info->parseState = 3; break; }

		case 3: {
			if (!isdigit(token) && !strchr(hexChars, token)) {
				token = 0;
				info->recognised = true;
			}
			break;
		}

		default: DASSERT(false);
	}

	return true;
}

static TokenStateInformation HEXINTCONST_Information = {
	0,
	false,
	HEXINTCONST_MajorCheck,
	HEXINTCONST_MinorProgress,
	HEXINT_CONST
};

////////////////////////////////////////////////////////

static bool STRCONST_MajorCheck (char& token, TokenStateInformation* info) 
	{ return token == '"'; }

static bool STRCONST_MinorProgress (char& token, TokenStateInformation* info) {
	if (info->parseState==0)		// State 0 is the first. Have to assert ".
		if (token == '"')
			info->parseState = 1;	// Start string, preceeding quote.
		else {
			token = 0;				// Token not consumed.
			return false;			// Failed to recognise.
		}
	else
	if (token == '"')				// Trailing quote.
		info->recognised = true;

	return true;
}

static TokenStateInformation STRCONST_Information = {
	0,
	false,
	STRCONST_MajorCheck,
	STRCONST_MinorProgress,
	STRING_CONST
};

////////////////////////////////////////////////////////
// In case of rules with sub-domains over other, like
// REAL_CONST and INT_CONST, we put those by increasing
// priority.
//
static TokenStateInformation* tokenClasses[]={

	// Keywords have higher priority than ids and attributes.
	//
	&IDENT_Information,
	&RESERVEDATTR_Information,
	&MONITOR_Information,
	&ATTRIBUTE_Information,
		
	&TRUE_Information,			
	&FALSE_Information,			
	&AND_Information,			
	&OR_Information,	
	&NOT_Information,			
	&NIL_Information,			
	&LOCAL_Information,
	&LAMBDA_Information,
	
	&FORMAL_Information,
	&STATIC_Information,
	&FUNCRES_Information,

	&NEW_Information,
	&DELETE_Information,

	&GE_Information,			
	&LE_Information,			
	&GT_Information,			
	&LT_Information,			
	&EQ_Information,	
	&NE_Information,

	&DOUBLE_DOT_Information,
	&DOT_Information,

	&DOUBLE_LEFT_SB_Information,
	&DOUBLE_RIGHT_SB_Information,
	&SINGLE_LEFT_SB_Information,
	&SINGLE_RIGHT_SB_Information,
	
	&REALCONST_Information,		
	&HEXINTCONST_Information,
	&INTCONST_Information,
	&STRCONST_Information
};

////////////////////////////////////////////////////////

int DebugExprLexAn::GetToken (void* yylval) {

	if (LastEOFWasForced())
		return 0;

	while (currPos && *currPos && isspace(*currPos))	// Skip blanks.
		++currPos;

	if (!*currPos)	// Check EOF.
		return 0;

	//*************************************
	// I. Identify the initially active rules.
	//
	std::list<TokenStateInformation*> activeRules;
	for (util_ui8 i = 0; i < uarraysize(tokenClasses); ++i)
		if ((*tokenClasses[i]->engagementCheck)(*currPos, tokenClasses[i])) {
			tokenClasses[i]->Reset();
			activeRules.push_back(tokenClasses[i]);
		}

	//*************************************
	// II. If no active rules, return the current character.
	// There is no way that EOF is reached, as no input is consumed.
	//
	if (!activeRules.size()) {
		DASSERT(currPos);
		return *currPos++;
	}

	//*************************************
	// III. Iterate through active rules and process the current character.
	// If a rule fails, reset it and remove it directly. If a rule succeeds,
	// we record its data. Upon processing, rules may consume a token.
	//
	char*					tokenStart	= currPos;
	TokenStateInformation*	matchedRule	= (TokenStateInformation*) 0;

	while (1) {

		//*************************************
		// III.1 Process all active rules for the current
		// input character.
		//
		bool consumeInput = false;
		std::list<TokenStateInformation*>::iterator ruleIter = activeRules.begin();

		while (ruleIter != activeRules.end()) {

			char token = *currPos;								// Store, since it may be affected.
			TokenStateInformation* currRule = *ruleIter;
			++ruleIter;

			if (!(*currRule->parseProgress)(token, currRule)) {	// Failed.
				DASSERT(!token);								// In case of failuer there is no way input is consumed.
				activeRules.remove(currRule);
			}
			else
			if (currRule->Succeeded())	{						// Recongised token ?
				activeRules.remove(currRule);					// Removed it.
				matchedRule = currRule;							// Set it as the matching rule.
			}
			else												// Normal progress, a character should have
				DASSERT(!!token);								// been consumed necessarily.

			if (token)											// If zerod, it was not consumed (i.e. look ahead).
				consumeInput = true;
		}
		
		bool eofReached = !*currPos;
		if (consumeInput)										// Check to if we have to move to the next.
			{ DASSERT(!eofReached); ++currPos; }

		//*************************************
		// III.2 If we have no active rules or EOF, we are at
		// a terminal point. Either we have a matched rule, so
		// a token has been recognized, or else we failed, meaning
		// we either resume input or check for EOF.
		//
		if (!activeRules.size() || eofReached) {

			if (matchedRule) {									// Have a matching rule ?

				matchedRule->Reset();							// Reset it.

				SetTokenContent(								// Record the token.
					yylval, 
					matchedRule->tokenId,
					tokenStart, 
					currPos
				);

				return matchedRule->tokenId;					// Return the token id.
			}
			else
			if (*(currPos = tokenStart))						// Can't identify a token, resume input.
				return *currPos++;
			else 
				{ ForceEOF(); return 0; }
		}

	}	// while (1)
}

////////////////////////////////////////////////////////

void DebugExprLexAn::ForceEOF (void) {
	CleanUp();
	eofWasForced = true;
}

////////////////////////////////////////////////////////

void DebugExprLexAn::SetTokenContent (
		void*			yylval, 
		util_ui16		tokenId,
		char*			tokenStart,
		char*			tokenEnd
	) {

	char endChar = *tokenEnd;								// Save char to put null terminator.
	*tokenEnd = '\0';										// Temporarily null terminate.

	YYSTYPE* tokenVal = (YYSTYPE*) yylval;
	if (tokenId == IDENT || tokenId == RESERVEDATTR || tokenId == MONITOR || tokenId == ATTRIBUTE)
		DebugEvaluatedExpr::StringLateDestroy(tokenVal->id = ucopystr(tokenStart + (tokenId == ATTRIBUTE ? 1 : 0)));
	else
	if (tokenId == REAL_CONST)
		sscanf(tokenStart,"%lf", &tokenVal->realConst);
	else
	if (tokenId == INT_CONST)
		sscanf(tokenStart,"%u", &tokenVal->intConst);
	else
	if (tokenId == HEXINT_CONST)
		tokenVal->intConst = ustrtohex(std::string(tokenStart + 2, tokenEnd).c_str());
	else
	if (tokenId == STRING_CONST)
		DebugEvaluatedExpr::StringLateDestroy(
			tokenVal->strConst = ucopystr(std::string(tokenStart + 1, tokenEnd - 1))
		);

	*tokenEnd = endChar;		// Restore string.
}

////////////////////////////////////////////////////////
