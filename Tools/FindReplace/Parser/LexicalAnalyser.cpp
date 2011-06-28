#include <stdio.h>
#include <assert.h>
#include "LexicalAnalyser.h"


namespace frep {

static const char* tokenClasses[] = {	
	"Start symbol",
	"*",
	"[",
	"]",
	"{",
	"}",								
	"<",
	">",
	"(",
	")",
	"|",
	"\\",
	"+",
	"number",
	"string",
	"identifier",
	"any character",
	"white space",
	"quoted string,"
	"integer",
	"eof",
	"undef"
};

//-----------------------------------------------------------------------

const char* LexicalAnalyser::Token::GetTokenClasses(const TokenType& type) {
	assert(((unsigned) type) < (sizeof(tokenClasses) / sizeof(char*)));
	return tokenClasses[type];
}

//-----------------------------------------------------------------------

void LexicalAnalyser::Token::Write (FILE* fp) 
	{ fprintf(fp, "%s\n", GetTokenClasses(type)); }


}	//namespace frep