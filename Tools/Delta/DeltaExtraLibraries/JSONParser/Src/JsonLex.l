%{
// Lexical analyser of the json format.
// Giannhs Apostolidhs, january 2013.
//

#include <iostream>
#include <string>
#include <string.h>

#include "ulexutil.h"
#include "ustrings.h"
#include "DeltaObject.h"
#include "DeltaValue.h"
#include "JsonParser.h"
#include "JsonParseActions.h"
#include "JsonLoaderErrorMsg.h"

#define	YY_NEVER_INTERACTIVE 1

// Must be static local since every lexer
// has its own scan buffer.

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
INTEGER [-]?{digit}+
FLOAT {INTEGER}"."{digit}+
EXPINTEGER {INTEGER}{exp}{INTEGER}
EXPFLOAT {FLOAT}{exp}{INTEGER}

%%

"{"			return BLOCK_L;
"}"			return BLOCK_R;
"["			return BRACKET_L;
"]"			return BRACKET_R;
","			return COMMA;
":"			return COLON;
"true"		return TRUE;
"false"		return FALSE;
"null"		return NIL;

{white}*	{ /*do nothing*/ }

{STRING}	{
				std::string s(yytext);
				s = s.substr(1, s.length() - 2).c_str();
				yylval.stringValue = JsonParserLoaderActions::Manage_StringWithLateDestruction(ucopystr(s));
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
				std::string s(yytext);
				yylval.stringValue = JsonParserLoaderActions::Manage_StringWithLateDestruction(ucopystr(s));
				return EXPINTEGER;
			 }

{EXPFLOAT}	{
				std::string s(yytext);
				yylval.stringValue = JsonParserLoaderActions::Manage_StringWithLateDestruction(ucopystr(s));
				return EXPFLOAT;
		    }

.			{
				JsonParserLoaderErrorMsg::JsonLoaderError("Did not recognize token %s", yytext);
				return yytext[0];
			}

%%

