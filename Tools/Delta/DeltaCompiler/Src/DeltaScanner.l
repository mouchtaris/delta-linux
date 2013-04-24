%{
// Lexical analyser of the Delta language.
// ScriptFighter Project.
// A. Savidis, Start October 1999.
// Updates for reading quoted strings and some refactoring, February 2009.
//

#include <cstring>
#include <cstdio>
#include <ctype.h>
#include <cstdarg>

#include "DDebug.h"
#include "Expr.h"
#include "Symbol.h"
#include "DeltaByteCodeTypes.h"
#include "ulexutil.h"
#include "DeltaCompErrorMsg.h"
#include "ASTNode.h"	//must be included before DeltaSyntax.h
#include "DeltaSyntax.h"
#include "ParseParms.h"
#include "CompilerStringHolder.h"
#include "ufunctors.h"

#define	YY_SKIP_YYWRAP
#define YY_USE_PROTOS
#define	YY_NEVER_INTERACTIVE 1

#define YY_DECL	int yyFlexLexer::yylex (YYSTYPE* yylval, YYLTYPE* yylloc)

#ifndef	_MSC_VER
extern "C" {
static int isatty YY_PROTO(( int )) { return 0; }
}
#endif

#define SET_END_LOCATION							\
	yylloc->last_line = GET_PARSEPARMS.GetLine();	\
	yylloc->last_column = yypos

#define YY_USER_ACTION								\
	yylloc->first_line = GET_PARSEPARMS.GetLine();	\
	yylloc->first_column = yypos;					\
	yypos += yyleng;								\
	SET_END_LOCATION;

///////////////////////////////////////////////////////////
// Helper macros to get specific components from the lexer
// component directory.

#define GET_COMPMESSENGER	COMPMESSENGER_EX(GetDirectory())
#define GET_PARSEPARMS		PARSEPARMS_EX(GetDirectory())
#define GET_STRINGHOLDER	STRINGHOLDER_EX(GetDirectory())

%}

line			[\n]
ws				[ \t\r]
letter			[a-zA-Z]
decdigit		[0-9]
underscore		[_]
hexdigit		[0-9a-fA-F]
ident			{letter}({letter}|{decdigit}|{underscore})*
real			{decdigit}+"."{decdigit}+
hex				0x{hexdigit}+
flag			{ws}+{decdigit}
cpp				#{ws}+{decdigit}+{ws}+\"[^"]+{flag}?
attr			[@]

%option c++
%option prefix="DeltaScanner"
%option outfile="DeltaScanner.cpp"
%option verbose
%option noyywrap
%%

function		return  FUNCTION;
return			return	RETURN;
assert			return	ASSERT;
lambda			return	LAMBDA;
@lambda			return	LAMBDA_REF;
onevent			return	ONEVENT;
method			return	METHOD;
self			return	SELF;
arguments		return	ARGUMENTS;
static			return	STATIC;
const			return	CONST;
if				return	IF;
else			return	ELSE;
while			return	WHILE;
for				return	FOR;
foreach			return	FOREACH;
nil				return	NIL;
and				return	AND;
not				return	NOT;
or				return	OR;
true			return	TRUE;
false			return	FALSE;
break			return	BREAK;
continue		return	CONTINUE;
local			return	LOCAL;
try				return	TRY;
trap			return	TRAP;
throw			return	THROW;
using			return	USING;
@set			return	SET;
@get			return	GET;
@operator		return	OPERATOR;
@self			return	NEWSELF;

"@"				return	ATTRIBUTE;
"#"				return	STRINGIFY;
"+"				return	ADD;
"-"				return	SUB;
"*"				return	MUL;
"/"				return	DIV;
"%"				return	MOD;
">"				return	GT;
"<"				return	LT;
"!="			return	NE;
"=="			return	EQ;
">="			return	GE;
"<="			return	LE;
"="				return	ASSIGN;
"()"			return	CALL;
"=()"			return	CAST;
"+_"			return	ADD_POSTFIX;
"-_"				return	SUB_POSTFIX;
"*_"			return	MUL_POSTFIX;
"/_"			return	DIV_POSTFIX;
"%_"			return	MOD_POSTFIX;
">_"			return	GT_POSTFIX;
"<_"			return	LT_POSTFIX;
"!=_"			return	NE_POSTFIX;
"==_"			return	EQ_POSTFIX;
">=_"			return	GE_POSTFIX;
"<=_"			return	LE_POSTFIX;

"::"			return	GLOBAL_SCOPE;
"++"			return	PLUSPLUS;
"--"				return	MINUSMINUS;
"."				return	DOT;
".."			return	DOUBLE_DOT;
"..."			return	TRIPLE_DOT;
".="			return	DOT_ASSIGN;
".=="			return	DOT_EQUAL;
".==_"			return	DOT_EQUAL_RHS;
".=()"			return	DOT_CAST;
"[["			return	DOUBLE_LB;
"]]"			return	DOUBLE_RB;
"+="			return	ADD_A;
"-="				return	SUB_A;
"*="			return	MUL_A;
"/="			return	DIV_A;
"%="			return	MOD_A;
";"				return	SEMI;

"<<"			return META_LSHIFT;
">>"			return META_RSHIFT;
"~"				return META_ESCAPE;
"!"				return META_INLINE;
"&"				return META_EXECUTE;
"$"				return META_RENAME;

{ws}*			{}

{line}			{	GET_PARSEPARMS.NextLine(); SET_END_LOCATION; }

{decdigit}+		{	ucastassign(yylval->numberConst, ustrdectodouble(yytext));
					return NUMBER_CONST;
				}

{ident}			{	yylval->constStr = usaveidstr(yytext, &yy_id_buffer); return IDENT; }

{attr}{ident}	{	yylval->constStr = usaveidstr(yytext, &yy_id_buffer); return ATTRIBUTE_IDENT; }
				
{real}			{	ucastassign(yylval->numberConst, ustrfracttodouble(yytext));
					return NUMBER_CONST;
				}

{hex}			{	ucastassign(yylval->numberConst, ustrtohex(yytext + 2));
					return NUMBER_CONST;
				}

{cpp}			{	util_ui32 line;
					sscanf(yytext + 1, "%u", &line);
					GET_PARSEPARMS.SetLine(line);
					GET_COMPMESSENGER.SetCurrentFile(DNULLCHECK(strchr(yytext, '\"')) + 1);
					while (yyinput() != '\n'){} // Read until end of line.
					//TODO: what about location information?
				}

\"				{	yylval->dynamicStr = GET_STRINGHOLDER.StringWithLateDestruction(ucopystr(ReadQuotedString()));
					SET_END_LOCATION;
					return STRING_CONST;
				}

\/\* 			{	IgnoreCStyleComments();		SET_END_LOCATION; }
\/\/			{	IgnoreCPPStyleComments();	SET_END_LOCATION; }

.				return yytext[0];

%%

//------------------------------------------------------------------

int DeltaSyntax_yylex (YYSTYPE* yylval, YYLTYPE* yylloc, yyFlexLexer& lexer)
	{ return lexer.yylex(yylval, yylloc); }

char yyFlexLexer::yyinput_wrapper(void* closure) {
	yyFlexLexer* lexer = (yyFlexLexer*) closure;
	++lexer->yypos;
	return lexer->yyinput();
}

void yyFlexLexer::yyunput_wrapper(char c, void *closure) {
	yyFlexLexer* lexer = (yyFlexLexer*) closure;
	--lexer->yypos;
	lexer->yyunput(c, lexer->yytext_ptr);
}

void yyFlexLexer::IgnoreCStyleComments (void)  {
	util_ui32 line = GET_PARSEPARMS.GetLine();
	std::string error;
	bool result = uignoreCcomments(
		umakecallback(yyinput_wrapper, this),
		umakecallback(yyunput_wrapper, this),
		&line,
		&error
	);
	if (!result)
		GET_COMPMESSENGER.Error(error.c_str());
	GET_PARSEPARMS.SetLine(line);
} 

void yyFlexLexer::IgnoreCPPStyleComments (void) {
	if (uignoreCPPcomments(umakecallback(yyinput_wrapper, this)))
		GET_PARSEPARMS.NextLine();
}

const std::string yyFlexLexer::ReadQuotedString (void) {
	util_ui32 line = GET_PARSEPARMS.GetLine();
	std::string s;
	std::string error;
	if (!ureadquotedstring(s, umakecallback(yyinput_wrapper, this), &line, &error))
		GET_COMPMESSENGER.Error(error.c_str());
	GET_PARSEPARMS.SetLine(line);
	return uextendescapesequences(s.c_str());
}

//------------------------------------------------------------------
