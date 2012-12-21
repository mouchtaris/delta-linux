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
#include "InterCode.h"					//must be included before DeltaParser.h
#include "ParseTableConstruction.h"		//must be included before DeltaParser.h
#include "DeltaParser.h"
#include "ParseParms.h"
#include "CompilerStringHolder.h"
#include "ParsingContext.h"
#include "ufunctors.h"

#define	YY_SKIP_YYWRAP
#define YY_USE_PROTOS
#define	YY_NEVER_INTERACTIVE 1

// The yylex() must be re-entrant, so it takes
// yylval as a formal parameter.
//
#define YY_DECL		int yyFlexLexer::yylex (YYSTYPE* yylval, YYLTYPE* yylloc)
#define	STACKVAL	yylval

///////////////////////////////////////////////////////////

#undef COMPMESSENGER
#define COMPMESSENGER	COMPMESSENGER_EX(context())

#undef PARSEPARMS
#define PARSEPARMS		PARSEPARMS_EX(context())

#undef STRINGHOLDER
#define STRINGHOLDER	STRINGHOLDER_EX(context())

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
%option prefix="DeltaLexAnalyser"
%option outfile="DeltaLexAnalyser.cpp"
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

{line}			{	PARSEPARMS.NextLine(); }

{decdigit}+		{	ucastassign(STACKVAL->numberConst, ustrdectodouble(yytext));
					return NUMBER_CONST;
				}

{ident}			{	STACKVAL->id = usaveidstr(yytext, &yy_id_buffer);
					return IDENT;
				}

{attr}{ident}	{	STACKVAL->id = usaveidstr(yytext, &yy_id_buffer);
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
					PARSEPARMS.SetLine(line);
					COMPMESSENGER.SetCurrentFile(DNULLCHECK(strchr(yytext, '\"')) + 1);
					while (yyinput() != '\n'){} // Read until end of line.
				}

\"				{	STACKVAL->strConst = STRINGHOLDER.StringWithLateDestruction(ucopystr(ReadQuotedString()));
					return STRING_CONST;
				}

\/\* 			{	IgnoreCStyleComments();		}
\/\/			{	IgnoreCPPStyleComments();	}

.				return yytext[0];

%%

//------------------------------------------------------------------

int DeltaCompiler_yylex (YYSTYPE* yylval, YYLTYPE* yylloc, ParsingContext& ctx)
	{ return ctx.GetLexer().yylex(yylval, yylloc); }

char yyFlexLexer::yyinput_wrapper(void* closure) { return ((yyFlexLexer*) closure)->yyinput(); }

void yyFlexLexer::yyunput_wrapper(char c, void *closure) {
	yyFlexLexer* lexer = (yyFlexLexer*) closure;
	lexer->yyunput(c, lexer->yytext_ptr);
}

void yyFlexLexer::IgnoreCStyleComments (void)  {
	util_ui32 line = PARSEPARMS.GetLine();
	std::string error;
	bool result = uignoreCcomments(
		umakecallback(yyinput_wrapper, this),
		umakecallback(yyunput_wrapper, this),
		&line,
		&error
	);
	if (!result)
		COMPMESSENGER.Error(error.c_str());
	PARSEPARMS.SetLine(line);
} 

void yyFlexLexer::IgnoreCPPStyleComments (void) {
	if (uignoreCPPcomments(umakecallback(yyinput_wrapper, this)))
		PARSEPARMS.NextLine();
}

const std::string yyFlexLexer::ReadQuotedString (void) {
	util_ui32 line = PARSEPARMS.GetLine();
	std::string s;
	std::string error;
	if (!ureadquotedstring(s, umakecallback(yyinput_wrapper, this), &line, &error))
		COMPMESSENGER.Error(error.c_str());
	PARSEPARMS.SetLine(line);
	return uextendescapesequences(s.c_str());
}

//------------------------------------------------------------------
