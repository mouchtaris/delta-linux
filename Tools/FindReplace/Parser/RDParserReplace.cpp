/*
 * RDParserReplace.cpp
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

/*	New Grammar
 *	Regex:	Regexs Regex 
 *		|	epsilon
 *		;
 *
 *	Regexs:	'\' Num
 *		|	Num		//But must retern a string
 *		|	String
 *		;
 */


#include <assert.h>
#include <iostream>
#include <algorithm>
#include "MyTemplates.h"
#include "RDParserReplace.h"



namespace frep {

//-----------------------------------------------------------------------

RDParserReplace::RDParserReplace(void){
	hasError		= false;
	lookAhead.type	= TokenSTART_SYMBOL;
}

//-----------------------------------------------------------------------

RDParserReplace::~RDParserReplace(void)
	{ Clear(); }

//-----------------------------------------------------------------------

RegExpr * RDParserReplace::Analyse (TokenList* _tokens) {
	assert(_tokens);
	tokens				= _tokens;
	tokenListIter		= tokens->begin();
	Match(TokenSTART_SYMBOL);
	RegExpr * tmp = Regex();

	if( !HasError() && (lookAhead == TokenEOF) ) 
		return tmp;
	return (RegExpr *)0;
}

//-----------------------------------------------------------------------

void RDParserReplace::Clear(void){
	lookAhead.type	= TokenSTART_SYMBOL; 
	hasError		= false;
	errorMsg.clear();
	
	std::for_each(toDelete.begin(), toDelete.end(), ClearFunctor());
	toDelete.clear();
}

//******************************************
//Grammar implementation

//-----------------------------------------------------------------------

RegExpr * RDParserReplace::Regex(void){
	RegExpr * e  = (RegExpr *)0;
	RegExpr * e1 = (RegExpr *)0;
	RegExpr * e2 = (RegExpr *)0;

	if( ( (e1 = Regexs()) && (e2 = Regex()) ) || ( e = Epsilon()) ){		
		if( e1 && e2 ){
			ConcatExpr * concat = new ConcatExpr();
			concat->e1 = e1;
			concat->e2 = e2;
			return concat;
		}
		else{
			if(e1) { delete e1; }				//petixe to e1 alla oxi to e2
			toDelete.push_back(e);
			return e; 
		}
	}
	return (RegExpr *)0;
}

//-----------------------------------------------------------------------

RegExpr * RDParserReplace::Regexs(void){
	RegExpr * e = (RegExpr *)0;

	if( lookAhead == TokenBACKSLASH )
		e = BackslashNumber();

	else if( lookAhead == TokenNUM )
		e = Number();


	else if( lookAhead == TokenREGEX_STRING )
		e = String();

	if( e ){
		toDelete.push_back(e);
		return e;
	}

	return (RegExpr *)0;
} 

//-----------------------------------------------------------------------

RegExpr * RDParserReplace::String(void){
	if( lookAhead == TokenREGEX_STRING ){
		Match(TokenREGEX_STRING);
		ReplaceStringExpr * str = new ReplaceStringExpr();
		str->text = GetCurrToken().value.strVal;
		return str;
	}
	else 
		assert(0);
	return (RegExpr *)0;
}

//-----------------------------------------------------------------------

RegExpr * RDParserReplace::BackslashNumber(void){
	if( lookAhead == TokenBACKSLASH ){
		Match(TokenBACKSLASH);
		Match(TokenNUM);
		
		if( !HasError() ){
			NumberExpr * num = new NumberExpr();
			num->num = GetCurrToken().value.intVal;
			return num;
		}
		else
			return (RegExpr *)0;
	}
	else 
		assert(0);
	return (RegExpr *)0;
}

//-----------------------------------------------------------------------

RegExpr * RDParserReplace::Number(void){
	if( lookAhead == TokenNUM ){
		Match(TokenNUM);
		
		if( !HasError() ){
			ReplaceStringExpr * str = new ReplaceStringExpr();
			str->text = string_cast(GetCurrToken().value.intVal);
			return str;
		}
		else
			return (RegExpr *)0;
	}
	else 
		assert(0);
	return (RegExpr *)0;
}

//-----------------------------------------------------------------------

RegExpr * RDParserReplace::Epsilon(void)
	{ return new EpsilonExpr(); }

}	//namespace frep