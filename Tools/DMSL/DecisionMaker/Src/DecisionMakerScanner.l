%{
#define YY_NEVER_INTERACTIVE 1

//Remove automatically generated file warnings
#ifdef  _MSC_VER
#pragma warning(disable:4102)
#pragma warning(disable:4244)
#pragma warning(disable:4267)
#pragma warning(disable:4996)
#endif

#include <stdlib.h>

#include "Expressions.h"
#include "Statements.h"
#include "util.h"

using namespace dmsl;

#include "DecisionMakerBisonParser.h"	//must be included after Expressions and Statements

#define YY_DECL int yylex (YYSTYPE* lvalp)
%}
%option prefix = "DMSL_yy"
%option outfile = "Src/DecisionMakerScanner.cpp"
%option noyywrap
%option yylineno

id				[a-zA-Z][a-zA-Z_0-9]*
integer			(0[xX][0-9A-Fa-f]+)|([0-9]+)
real			[0-9]*\.[0-9](e|E)?[0-9]*
space			[\r\n \t\v]
string			\"[^\n"]*\"

comment1		"//".*
comment2		"#".*

%x COMMENT

%%

"/*"                    BEGIN(COMMENT);
<COMMENT>[^*]*        	/* eat anything that's not a '*' */
<COMMENT>"*"+[^*/]*   	/* eat up '*'s not followed by '/'s */
<COMMENT>"*"+"/"        BEGIN(INITIAL);

<<EOF>> 				{ fclose(yyin);	return EOF; }

{comment1}				/* eat comment 1 */
{comment2}				/* eat comment 2 */

"+"				return '+';
"-"				return '-';
"*"				return '*';
"/"				return '/';
"%"				return '%';

"("				return '(';
")"				return ')';
"["				return '[';
"]"				return ']';
"{"				return '{';
"}"				return '}';

"<"				return LT;
">"				return GT;
"<="			return LE;
">="			return GE;

"="	|
"=="			return EQ;
"!="			return NEQ;

"."				return '.';
","				return ',';
":"				return ':';

"&&" |
"and"			return AND;

"||" |
"or"			return OR;

"!" |
"not"			return NOT;

".."			return DOUBLEDOT;

"if"			return IF;
"then"			return THEN;
"else"			return ELSE;

"case"			return CASE;
"when"			return WHEN;
"otherwise"		return OTHERWISE;

"true" |
"TRUE"			{ lvalp->boolVal = true;	return BOOL_VALUE; }

"false"	|
"FALSE"			{ lvalp->boolVal = false;	return BOOL_VALUE; }

"component"		return COMPONENT;

"activate"		return ACTIVATE;
"cancel"		return CANCEL;
"evaluate" |
"adapt"			return EVALUATE;

"call" |
"library"		return LIBRARY;

"in"			return DMSL_IN;

"isactive"		return IS_ACTIVE;
"tonumber"		return TO_NUMBER;
"hasattr"		return HAS_ATTR;
"random"		return RANDOM;
"cansupport"	return CAN_SUPPORT;

"params"		return PARAMS;

"stereotype"	return STEREOTYPE;

"def"			return DEFINE;

{id}                    {
							lvalp->stringVal = util::strdup(yytext);
							return ID_VALUE;
						}

{string}				{
							lvalp->stringVal = util::strdup(yytext + 1, yyleng - 2);
							return STRING_VALUE;
						}

{integer}				{
							lvalp->intVal = atoi(yytext);
							return INT_VALUE;
						}
{real}					{
							lvalp->realVal = (double) atof(yytext);
							return REAL_VALUE;
						}
{space}+				/* eat the whitespace */
.						{ return DMSL_ERROR; }

%%
