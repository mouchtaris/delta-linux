%{
#define YY_NEVER_INTERACTIVE 1

//Remove automatically generated file warnings
#ifdef  _MSC_VER
#pragma warning(disable:4102)
#pragma warning(disable:4244)
#pragma warning(disable:4267)
#pragma warning(disable:4996)
#endif

#include "ManageActions.h"
#include "util.h"

using namespace dmsl;

#include "ProfileBisonParser.h"	//must be included after ManageActions

#define YY_DECL int yylex (YYSTYPE* lvalp)
%}
%option prefix = "Profile_yy"
%option outfile = "Src/ProfileScanner.cpp"
%option noyywrap
%option yylineno

id				[a-zA-Z][a-zA-Z_0-9]*
integer			(0[xX][0-9A-Fa-f]+)|(-?[0-9]+)
real			-?[0-9]*\.[0-9](e|E)?[0-9]*
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

<<EOF>> 		{ return EOF; }

{comment1}		/* eat comment 1 */
{comment2}		/* eat comment 2 */

"="				return '=';
"."				return '.';
","				return ',';
"{"				return '{';
"}"				return '}';
"["				return '[';
"]"				return ']';

".."			return DOUBLEDOT;

"true"	| 
"TRUE"			{ lvalp->boolVal = true;									return BOOL_VALUE;	}

"false"	|
"FALSE"			{ lvalp->boolVal = false;									return BOOL_VALUE;	}

{id}            { lvalp->stringVal = util::strdup(yytext);					return STRING_VALUE;	}
{string}		{ lvalp->stringVal = util::strdup(yytext + 1, yyleng - 2);	return STRING_VALUE;	}

{integer}		{ lvalp->intVal = atoi(yytext);								return INT_VALUE;	}
{real}			{ lvalp->realVal = (double) atof(yytext);					return REAL_VALUE;	}

{space}+				/* eat the whitespace */
.				return PROFILE_ERROR;

%%
