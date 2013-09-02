%{
// Lexical analyser for AOP Pointcuts.
// ScriptFighter Project.
// ScriptFigher Project.
// Y. Lilis, August 2013.
//

#include <cstring>
#include <cstdio>
#include <ctype.h>
#include <cstdarg>


#include "ulexutil.h"
#include "ustrings.h"
#include "Pointcut.h"	//must be included before PointcutSyntax.h
#include "PointcutSyntax.h"
//#include "CompilerStringHolder.h"
#include "ufunctors.h"

#define	YY_SKIP_YYWRAP
#define YY_USE_PROTOS
#define	YY_NEVER_INTERACTIVE 1

#define YY_DECL	int yyFlexLexer::yylex (YYSTYPE* yylval, YYLTYPE* yylloc)

#ifndef	_MSC_VER
extern "C" { static int isatty YY_PROTO(( int )) { return 0; } }
#endif

%}

ws				[ \t\r\n]
letter			[a-zA-Z]
decdigit		[0-9]
underscore		[_]
ident			({letter}|"*")({letter}|{decdigit}|{underscore}|"*")*
real			{decdigit}+"."{decdigit}+
attr			[@]

%option c++
%option prefix="PointcutScanner"
%option outfile="PointcutScanner.cpp"
%option verbose
%option noyywrap
%option yylineno
%%

ast				return	AST;
attribute		return	ATTRIBUTE;
child			return	CHILD;
parent			return	PARENT;
descendant		return	DESCENDANT;
ascendant		return	ASCENDANT;
execution		return	EXECUTION;
call			return	CALL;
exception		return	EXCEPTION;
class			return	CLASS;

function		return  FUNCTION;
onevent			return	ONEVENT;
method			return	METHOD;
and				return	AND;
not				return	NOT;
or				return	OR;

@operator		return	OPERATOR;
"@"				return	ATTRIBUTE;
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
"."				return	DOT;
".="			return	DOT_ASSIGN;

".."			return	DOUBLE_DOT;
"..."			return	TRIPLE_DOT;

@lambda			return	LAMBDA_REF;
@self			return	NEWSELF;
self			return	SELF;
arguments		return	ARGUMENTS;

true			return	TRUE;
false			return	FALSE;

{ws}*			{}

{decdigit}+		{	ucastassign(yylval->numberVal, ustrdectodouble(yytext));
					return NUMBER;
				}

{real}			{	ucastassign(yylval->numberVal, ustrfracttodouble(yytext));
					return NUMBER;
				}

{ident}			{	yylval->stringVal = usaveidstr(yytext, &yy_id_buffer); return IDENT; }

{attr}{ident}	{	yylval->stringVal = usaveidstr(yytext, &yy_id_buffer); return ATTRIBUTE_IDENT; }
				
\"				{	
					//yylval->stringVal = GET_STRINGHOLDER.StringWithLateDestruction(ucopystr(ReadQuotedString()));
					yylval->stringVal = ucopystr(ReadQuotedString());
					return STRING;
				}

\/\* 			{	IgnoreCStyleComments(); }

.				return yytext[0];

%%

//------------------------------------------------------------------

int PointcutSyntax_yylex (YYSTYPE* yylval, YYLTYPE* yylloc, yyFlexLexer& lexer)
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
	util_ui32 unused;
	std::string error;
	bool result = uignoreCcomments(
		umakecallback(yyinput_wrapper, this),
		umakecallback(yyunput_wrapper, this),
		&unused,
		&error
	);
	if (!result)
		;//GET_COMPMESSENGER.Error(error.c_str());
} 

const std::string yyFlexLexer::ReadQuotedString (void) {
	util_ui32 unused;
	std::string s;
	std::string error;
	if (!ureadquotedstring(s, umakecallback(yyinput_wrapper, this), &unused, &error))
		;//GET_COMPMESSENGER.Error(error.c_str());
	return uextendescapesequences(s.c_str());
}

//------------------------------------------------------------------
