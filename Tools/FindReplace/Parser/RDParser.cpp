/*
 * RDParser.cpp
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#include <assert.h>
#include <iostream>
#include "RDParser.h"

namespace frep {

#define MARK()	TokenList::iterator t = marck_input_position();
#define FAIL()	retruct_input_till_position(t);


//-----------------------------------------------------------------------

RDParser::Token RDParser::GetNextToken (void){
	if (tokenListIter != tokens->end() )
		++tokenListIter;
	return *tokenListIter;
}

//-----------------------------------------------------------------------

void RDParser::RetractCurrToken (void) {
	assert (tokenListIter != tokens->begin());
	--tokenListIter;
}

//-----------------------------------------------------------------------

void RDParser::Match(TokenType t){
	if( lookAhead == TokenSTART_SYMBOL ){
		lookAhead = GetNextToken();
		return;
	}

	if( lookAhead == t )
		lookAhead = GetNextToken();
	else
		Error(t);
}

//-----------------------------------------------------------------------

void RDParser::Error (TokenType t) {
	hasError = true;
	errorMsg += std::string("Syntax error: '") + Token::GetTokenClasses(t) + "' expected. \n";
}

//-----------------------------------------------------------------------

void RDParser::SetErrorMSG (const std::string msg){ 
	hasError = true; 
	errorMsg = msg; 
}

//-----------------------------------------------------------------------

RDParser::Token & RDParser::GetCurrToken(void){

	if( tokenListIter != tokens->begin() ){
		tokenListIter--;
		Token & tmp = *tokenListIter;
		tokenListIter++;
		return tmp;
	}
	else
		return *tokenListIter;
}

}	//namespace frep
