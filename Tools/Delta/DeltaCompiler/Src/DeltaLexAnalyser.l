%{
// Lexical analyser of the Delta language.
// ScriptFighter Project.
// A. Savidis, Start October 1999.
// Updates for reading quoted strings and some refactoring, February 2009.
//

#include <cstring>
#include <cstdio>
#include <ctype.h>

#include "DDebug.h"
#include "Expr.h"
#include "Symbol.h"
#include "ParseActions.h"
#include "DeltaByteCodeTypes.h"
#include "ulexutil.h"
#include "DeltaCompErrorMsg.h"
#include "DeltaParser.h"
#include "CompilerAPI.h"
#include "ParseActions.h"

#define	YY_SKIP_YYWRAP
#define YY_USE_PROTOS
#define	YY_NEVER_INTERACTIVE 1

// Must be static local since every lexer
// has its own scan buffer.

static void* lexBufferState = (void*) 0;

bool DeltaCompiler::InitialiseForText (const char* text) {
	DASSERT(!lexBufferState);
	lexBufferState		= DeltaCompiler_yy_scan_string(text);
	*DPTR(srcFile)		= DBG_DYNAMIC_SOURCE;
	*DPTR(dynamicCode)	= text;
	isDynamicCode		= true;
	return true;
}

void DeltaCompiler::CleanUpForText (void) {
	if (lexBufferState) {
		DeltaCompiler_yy_flush_buffer((YY_BUFFER_STATE) lexBufferState);
		DeltaCompiler_yy_delete_buffer((YY_BUFFER_STATE) lexBufferState);
		unullify(lexBufferState);
	}
}

// The yylex() must be re-entrant, so it takes
// yylval as a formal parameter.
//
#define YY_DECL		int DeltaCompiler_yylex (void* yylval)
#define	STACKVAL	((YYSTYPE*) yylval)
#define	yywrap		DeltaCompiler_yywrap

// Lexical analysis helpers.
//
static void					IgnoreCStyleComments (void);
static void					IgnoreCPPStyleComments (void);
static const std::string	ReadQuotedString (void);

extern int DeltaCompiler_yywrap(void);

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
"-_"			return	SUB_POSTFIX;
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
"--"			return	MINUSMINUS;
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
"-="			return	SUB_A;
"*="			return	MUL_A;
"/="			return	DIV_A;
"%="			return	MOD_A;
";"				return	SEMI;

{ws}*			{}

{line}			{	DeltaCompiler::NextLine(); }

{decdigit}+		{	ucastassign(STACKVAL->numberConst, ustrdectodouble(yytext));
					return NUMBER_CONST;
				}

{ident}			{	STACKVAL->id = usaveidstr(yytext);
					return IDENT;
				}

{attr}{ident}	{	STACKVAL->id = usaveidstr(yytext);
					return ATTRIBUTE_IDENT;
				}
				
{real}			{	ucastassign(STACKVAL->numberConst, ustrfracttodouble(yytext));
					return NUMBER_CONST;
				}

{hex}			{	ucastassign(STACKVAL->numberConst, ustrtohex(yytext + 2));
					return NUMBER_CONST;
				}

{cpp}			{	util_ui32 line;
					sscanf(yytext + 1, "%u", &line);
					DeltaCompiler::SetLine(line);
					DeltaCompSetCurrentFile(DNULLCHECK(strchr(yytext, '\"')) + 1);
					while (yyinput() != '\n'){} // Read until end of line.
				}

\"				{	STACKVAL->strConst = Translate_StringWithLateDestruction(ucopystr(ReadQuotedString()));
					return STRING_CONST;
				}

\/\* 			{	IgnoreCStyleComments();		}
\/\/			{	IgnoreCPPStyleComments();	}

.				return yytext[0];

%%

//------------------------------------------------------------------

int					DeltaCompiler_yywrap (void)			{ return 1; }
void				DeltaCompiler_ResetLex (FILE* fp)	{ yyrestart(fp); }
const std::string	DeltaCompiler_GetText (void)		{ return yytext; }

static char input_wrapper (void)	{ return yyinput(); }
static void unput_wrapper (char c)	{ unput(c); }

static void IgnoreCStyleComments (void)  {
	util_ui32 line = DeltaCompiler::GetLine();
	uignoreCcomments(input_wrapper, unput_wrapper, &line, &DeltaCompError);
	DeltaCompiler::SetLine(line);
} 

static void IgnoreCPPStyleComments (void) {
	util_ui32 line = DeltaCompiler::GetLine();
	uignoreCPPcomments(input_wrapper, &line);
	DeltaCompiler::SetLine(line);
}

static const std::string ReadQuotedString (void) {
	util_ui32 line = DeltaCompiler::GetLine();
	std::string s;
	ureadquotedstring(s, input_wrapper, &line, &DeltaCompError);
	DeltaCompiler::SetLine(line);
	return uextendescapesequences(s.c_str());
}

//------------------------------------------------------------------
