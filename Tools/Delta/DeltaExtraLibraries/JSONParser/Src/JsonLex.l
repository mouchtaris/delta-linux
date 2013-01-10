%{

	#include <string>
	#include "DeltaObject.h"
	#include "DeltaValue.h"
	#include "jsonParser.h"
	#include "jsonLoaderAPI.h"
	#include "jsonLoaderErrorMsg.h"
  

	#define	YY_NEVER_INTERACTIVE 1

	static void* bufferState = (void*) 0;

	void JsonLoaderInitialiseScannerFromString (const char* str) {
		bufferState = yy_scan_string(str);
	}

	void JsonLoaderCleanUpScannerFromString (void) {
		DASSERT(bufferState);
		yy_flush_buffer((YY_BUFFER_STATE) bufferState);
		yy_delete_buffer((YY_BUFFER_STATE) bufferState);
		bufferState = (void*) 0;
	}

%}
/* Flex options */

%option noyywrap
%option yylineno

/* Flex macros */

character [^"]
digit [0-9]
exp ("e"|"e+"|"e-"|"E"|"E+"|"E-")
white [ \n\t]

STRING \"{character}*\"
INTEGER [-]*{digit}+
FLOAT {INTEGER}"."{digit}+
EXPINTEGER {INTEGER}{exp}{INTEGER}
EXPFLOAT {FLOAT}{exp}{INTEGER}

%%

"{"			return BLOCK_L;
"}"			return BLOCK_R;
"["			return BRACKET_L;
"]"			return BRACKET_R;
","			return COMMA;
":"			return DBLDOT;
"true"		return TRUE;
"false"		return FALSE;
"null"		return NIL;

{white}*	{ /*do nothing*/ }

{STRING}	{
				std::string stringtext(yytext);
				yylval.stringValue = DNEWCLASS(std::string, (stringtext.substr(1, stringtext.length() - 2)));
				return STRING;
			}

{INTEGER}	{
				yylval.numberValue = atof(yytext); 
				return INTEGER;
			}

{FLOAT}		{
				yylval.numberValue = atof(yytext); 
				return FLOAT;
			}

{EXPINTEGER} {
				yylval.stringValue = DNEWCLASS(std::string, (yytext)); 
				return EXPINTEGER;
			 }

{EXPFLOAT}	{
				yylval.stringValue = DNEWCLASS(std::string, (yytext)); 
				return EXPFLOAT;
		    }

.			{
				JsonParserLoaderErrorMsg::JsonLoaderError("Did not recognize token %s", yytext);
				return yytext[0];
			}

%%

