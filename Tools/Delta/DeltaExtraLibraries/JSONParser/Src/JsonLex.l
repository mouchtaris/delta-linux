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

const std::string ReadQuotedString (void);

%}
/* Flex options */

%option noyywrap
%option yylineno

/* Flex macros */

digit [0-9]
exp ("e"|"e+"|"e-"|"E"|"E+"|"E-")
white [ \n\t]

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

\"			{
				yylval.stringValue = JsonParserLoaderActions::Manage_StringWithLateDestruction(ucopystr(ReadQuotedString()));
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

static char input_wrapper(void) { return yyinput(); }

static const std::string ReadQuotedString (void) {
	std::string s;
	std::string error;
	if (!ureadquotedstring(s, input_wrapper, (util_ui32 *)&yylineno, &error))
		JsonParserLoaderErrorMsg::JsonLoaderError("%s", error.c_str());
	return uextendescapesequences(s.c_str());
}
