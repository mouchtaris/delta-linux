/*
 * LexicalAnalyserReplace.cpp
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#include <cassert>
#include <ctype.h>
#include <memory.h>
#include <algorithm>

#include "LexicalAnalyserReplace.h"




namespace frep {

//-----------------------------------------------------------------------

LexicalAnalyserReplace::LexicalAnalyserReplace(void){
	hasError		= false;
	errorMsg		= (char *)0;
	totalLookAhead	= 0;
	ptrInputStr		= 0;
	tokenList		= (TokenList*) 0;
}

//-----------------------------------------------------------------------

LexicalAnalyserReplace::~LexicalAnalyserReplace(void)
	{ Clear(); }


//-----------------------------------------------------------------------

void LexicalAnalyserReplace::Clear(void){
	if (tokenList) {
		std::for_each( tokenList->begin(), tokenList->end(), Token::ClearFunctor() );
		delete tokenList;
	}

	if (HasError())
		delete []errorMsg;

	hasError		= false;
	errorMsg		= (char *)0;
	totalLookAhead	= 0;
	ptrInputStr		= 0;
	tokenList		= (TokenList*) 0;
}

//-----------------------------------------------------------------------

void LexicalAnalyserReplace::SetError(char * msg){
	hasError = true;
	errorMsg = _strdup(msg);
}

//-----------------------------------------------------------------------

void LexicalAnalyserReplace::Unput (void) {
	if ( ptrInputStr < 0 )
		SetError("Max Unput exceeded!");
	else 
		ptrInputStr--;
}

//-----------------------------------------------------------------------

char LexicalAnalyserReplace::Input (void) {
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

bool LexicalAnalyserReplace::IsSingleCharToken (char token) {
	char c = Input();
	
	if (c == token) 
		return true;
	
	else {
		Unput();
		return false;
	}
}

//-----------------------------------------------------------------------

bool LexicalAnalyserReplace::IsINT (unsigned int* val) {
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

bool LexicalAnalyserReplace::IsSTRING (char** val) {
	std::list<char> str;
	char c = Input();
	
	while ( true) 
	{
		if( (c == '\\') || (c == '\0')){
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

bool LexicalAnalyserReplace::Analyse (std::string & str) {
	inputStr = str;
	totalLookAhead = str.length();

	tokenList	= new TokenList;
	Token token;
	
	token.type = TokenSTART_SYMBOL;
	tokenList->push_back( token );

	while ( (ptrInputStr < inputStr.length()) ) {

		if ( ptrInputStr >= inputStr.length() ) { break; }

		else if ( !HasError() && IsINT(&token.value.intVal))
			token.type = TokenNUM;
		
		else if ( !HasError() && IsSTRING(&token.value.strVal))//panta prhn apo ton elegxo gia single '\\'
			token.type = TokenREGEX_STRING;

		else if ( !HasError() && IsSingleCharToken('\\'))
			token.type = TokenBACKSLASH;
	
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

void LexicalAnalyserReplace::DumpTokens (FILE* fp) {

	if (tokenList){
		TokenList::iterator tokenIter;
		for (tokenIter = tokenList->begin(); tokenIter != tokenList->end(); ++tokenIter )
			tokenIter->Write(fp); 
	}//end of if
	
	return;
}

}	//namespace frep