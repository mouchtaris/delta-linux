/*
 * LexicalAnalyserFind.cpp
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#include <cassert>
#include <ctype.h>
#include <memory.h>
#include <algorithm>

#include "LexicalAnalyserFind.h"

namespace frep {

static char * specialChar = "[]{}<>*()|:.\\+";

LexicalAnalyserFind::LexicalAnalyserFind(void){
	hasError		= false;
	errorMsg		= (char *)0;
	totalLookAhead	= 0;
	ptrInputStr		= 0;
	tokenList		= (TokenList*) 0;
}

//-----------------------------------------------------------------------

LexicalAnalyserFind::~LexicalAnalyserFind(void)
	{ Clear(); }

//-----------------------------------------------------------------------
void LexicalAnalyserFind::Clear(void){
	if (tokenList) {
		std::for_each( tokenList->begin(), tokenList->end(), Token::ClearFunctor() );
		delete tokenList;
	}

	if( HasError() ){
		delete []errorMsg;
	}
	hasError		= false;
	errorMsg		= (char *)0;
	totalLookAhead	= 0;
	ptrInputStr		= 0;
	tokenList		= (TokenList*) 0;
}

//-----------------------------------------------------------------------

void LexicalAnalyserFind::SetError(char * msg){
	hasError = true;
	errorMsg = _strdup(msg);
}

//-----------------------------------------------------------------------

void LexicalAnalyserFind::Unput (void) {
	if (ptrInputStr < 0)
		SetError("Max Unput exceeded!");
	else 
		ptrInputStr--;
}

//-----------------------------------------------------------------------

char LexicalAnalyserFind::Input (void) {
	char c;

	if( ptrInputStr > totalLookAhead)
		c = '\0';
	else{
		c = inputStr[ptrInputStr];
		ptrInputStr++;
	}
	
	return c;
}

//-----------------------------------------------------------------------

void LexicalAnalyserFind::IgnoreWhiteSpace (void) {
	while ( ptrInputStr < totalLookAhead) {
		char c = Input();
		if (c==' ')
			continue;
		
		else {
			if (isprint(c))
				Unput();
			return;
		}
	}//end of while
	return;
}

//-----------------------------------------------------------------------

bool LexicalAnalyserFind::IsSingleCharToken (char token) {
	char c = Input();
	
	if (c == token) 
		return true;
	
	else {
		Unput();
		return false;
	}
}

//-----------------------------------------------------------------------

bool LexicalAnalyserFind::IsINT (unsigned int* val) {
	*val = 0;
	char c = Input();
	if (!isdigit(c)) {
		Unput();
		return false;
	}
	do {
		*val = *val * 10 + c - '0';
		c = Input();
	} while (isdigit(c));
	
	Unput();
	return true;
}

//-----------------------------------------------------------------------

bool LexicalAnalyserFind::IsSTRING (char** val) {
	std::list<char> str;
	char c = Input();
	
	while ( true) 
	{
		if( c == '\\' ) {
			c = Input();
			if( !strchr(specialChar, c) ){//den einai char opos: \{, \} ...
				Unput();			//Unput curr C
				Unput();			//Unput '\\'
				break;
			}
		}
		else if( strchr(specialChar, c) || (c == '\0') )
		{
			Unput();
			break;
		}
		str.push_back(c);		
		c = Input();
	}//end of while
	
	if( !str.size() )
		return false;
	else{
		char* p = *val = new char[ str.size() + 1 ];
	
		for ( std::list<char>::iterator charIter = str.begin();
			charIter != str.end();
			++charIter, ++p ) { *p = *charIter; }
		*p = '\0';
		return true;
	}
}//end of IsSTRING

//-----------------------------------------------------------------------

bool LexicalAnalyserFind::Analyse (std::string & str) {
	inputStr = str;
	totalLookAhead = str.length();

	tokenList	= new TokenList;
	Token token;
	
	token.type = TokenSTART_SYMBOL;
	tokenList->push_back( token );

	while ( (ptrInputStr < inputStr.length()) ) {
		//IgnoreWhiteSpace();
		if ( ptrInputStr >= inputStr.length() ) { break; }

		if ( !HasError() && IsSingleCharToken('.') )
			token.type = TokenANY_CHARACTER;

		else if ( !HasError() && IsSingleCharToken(':') ){
			if( IsSingleCharToken('w') )
				token.type = TokenWHITE_SPACE;
			else if ( IsSingleCharToken('i') )
				token.type = TokenDELTA_ID;
			else if ( IsSingleCharToken('q') )
				token.type = TokenQUOTED_STRING;
			else if ( IsSingleCharToken('z') )
				token.type = TokenINTEGER;
			else{
				SetError("Unrecognised spacial char!\n");
				break;
			}
		}
		else if ( !HasError() && IsINT(&token.value.intVal))
			token.type = TokenNUM;
		
		else if ( !HasError() && IsSTRING(&token.value.strVal))//panta prhn apo ton elegxo gia single '\\'
			token.type = TokenREGEX_STRING;

		else if ( !HasError() && IsSingleCharToken('\\'))
			token.type = TokenBACKSLASH;

		else if ( !HasError() && IsSingleCharToken('*'))
			token.type = TokenASTERICK;

		else if ( !HasError() && IsSingleCharToken('['))
			token.type = TokenLEFT_BRACKET;

		else if ( !HasError() && IsSingleCharToken(']'))
			token.type = TokenRIGHT_BRACKET;

		else if ( !HasError() && IsSingleCharToken('{'))
			token.type = TokenLEFT_CURLY_BRACE;
		
		else if ( !HasError() && IsSingleCharToken('}'))
			token.type = TokenRIGHT_CURLY_BRACE;
		
		else if ( !HasError() && IsSingleCharToken('<'))
			token.type = TokenLESS_THAN;

		else if ( !HasError() && IsSingleCharToken('>'))
			token.type = TokenGREATER_THAN;
		
		else if ( !HasError() && IsSingleCharToken('('))
			token.type = TokenLEFT_PARENTHESIS;
		
		else if ( !HasError() && IsSingleCharToken(')'))
			token.type = TokenRIGHT_PARENTHESIS;

		else if ( !HasError() && IsSingleCharToken('|'))
			token.type = TokenVERTICAL_BAR;

		else if ( !HasError() && IsSingleCharToken('+'))
			token.type = TokenPLUS;
		
		else{ 
			if( !HasError() )
				SetError("Unrecognised token!\n");
			break; 
		}
		tokenList->push_back(token);
	}//end of while
	
	//mark the end of file
	token.type = TokenEOF;
	tokenList->push_back(token);
	
	return !hasError;
}//end of Analyse


//-----------------------------------------------------------------------

void LexicalAnalyserFind::DumpTokens (FILE* fp) {

	if (tokenList){
		TokenList::iterator tokenIter;
		for (tokenIter = tokenList->begin(); tokenIter != tokenList->end(); ++tokenIter )
			tokenIter->Write(fp); 
	}//end of if
	
	return;
}

}//namespace frep
