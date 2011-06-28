/*
 * RDPParserFind.cpp
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

/*	Grammar
 *	Regex:	Regexs Regex 
 *		|	epsilon
 *		;
 *
 *	Regexs:	'{' Regex '}'
 *		|	'(' Regex ')' '*'
 *		|	'[' Regex ']' '+'
 *		|	'<' Regex | Regex '>'
 *		|	String
 *		|	DeltaId
 *		|	WhtiteSpace
 *		|	AnyChar
 *		|	QuotedString
 *		|	Integer
 *		|	Num //Like string
 *		;
 */


#include <assert.h>
#include <iostream>
#include <algorithm>
#include "RDParserFind.h"
#include "Optimization.h"
#include "MyTemplates.h"


namespace frep {

#define CREATE_AND_RETURN_NODE_EXPRESSION(TOKEN, NODE)	\
	assert(lookAhead == TOKEN);							\
	Match(TOKEN);										\
	return new NODE()


//-----------------------------------------------------------------------

RDParserFind::RDParserFind()
:	tagSerial(0),
	tmpErrorMessage("")
{
	hasError		= false;
	lookAhead.type	= TokenSTART_SYMBOL;
}

//-----------------------------------------------------------------------

RDParserFind::~RDParserFind(void){
	Clear();
}

//-----------------------------------------------------------------------

void RDParserFind::Clear(void){
	lookAhead.type	= TokenSTART_SYMBOL; 
	hasError		= false;
	errorMsg.clear();
	
	std::for_each(toDelete.begin(), toDelete.end(), ClearFunctor());
	toDelete.clear();
}

//-----------------------------------------------------------------------

RegExpr * RDParserFind::Analyse (TokenList* _tokens) {
	assert(_tokens);
	tagSerial			= 0;
	tokens				= _tokens;
	tokenListIter		= tokens->begin();
	tmpErrorMessage		= "";
	Match(TokenSTART_SYMBOL);
	RegExpr * tmp = Regex();

	if( !dynamic_cast<EpsilonExpr *>(tmp)	&& //To e einai la8os
		!HasError()							&&
		(lookAhead == TokenEOF) 
	) {
		Optimization opt;
		return opt.PostOrderOptimization(tmp);
	}
	else {
		if ( !HasError() ) 
			SetErrorMSG(tmpErrorMessage);
		return (RegExpr *)0;
	}
}


//Grammar implementation
//-----------------------------------------------------------------------

RegExpr * RDParserFind::Regex(void){
	RegExpr * e  = (RegExpr *)0;
	RegExpr * e1 = (RegExpr *)0;
	RegExpr * e2 = (RegExpr *)0;

	if( ( (e1 = Regexs()) && (e2 = Regex()) ) || ( e = Epsilon()) ){		
		if( e1 && e2 ){
			if( dynamic_cast<EpsilonExpr*>(e2))
				return e1;
			else {
				ConcatExpr * concat = new ConcatExpr();
				concat->e1 = e1;
				concat->e2 = e2;
				e1->parent = concat;
				e2->parent = concat;
				toDelete.push_back(concat);
				return concat;
			}
		}
		else{
			toDelete.push_back(e);
			return e; 
		}
	}
	return (RegExpr *)0;
}

//-----------------------------------------------------------------------

RegExpr * RDParserFind::Regexs(void){
	RegExpr * e = (RegExpr *)0;

	switch(lookAhead.type) {
		case TokenNUM:				e = Number();		break;
		case TokenINTEGER:			e = Integer();		break;
		case TokenDELTA_ID:			e = DeltaId();		break;
		case TokenLESS_THAN:		e = OR();			break;
		case TokenWHITE_SPACE:		e = WhiteSpace();	break;
		case TokenLEFT_BRACKET:		e = OneOrMore();	break;
		case TokenREGEX_STRING:		e = String();		break;
		case TokenANY_CHARACTER:	e = AnyCharacter(); break;
		case TokenQUOTED_STRING:	e = QuotedString(); break;
		case TokenLEFT_PARENTHESIS: e = ZeroOrMore();	break;
		case TokenLEFT_CURLY_BRACE: e = Tag();			break;
		default:
			//In default case we can go for two reasons.
			//1) We have an unexpected token.
			//2) Regexs rule must returns a null expreesion (We do not have error)
			//For the first case we store an error message to be used in future
			if(!HasError())
				tmpErrorMessage +=	std::string("Unexpected token: '")		+ 
									Token::GetTokenClasses(lookAhead.type)	+ 
									"'\n";
			
	};
	
	if (e)
		toDelete.push_back(e);

	return e;
} 

//-----------------------------------------------------------------------

RegExpr* RDParserFind::OneOrMore(void){
	return	CreateRegExprWithSingleChild<OneOrMoreExpr>(	
				TokenLEFT_BRACKET, 
				TokenRIGHT_BRACKET, 
				TokenPLUS,
				false
			);
}

//-----------------------------------------------------------------------

RegExpr* RDParserFind::ZeroOrMore(void){
	return CreateRegExprWithSingleChild<ZeroOrMoreExpr>(
				TokenLEFT_PARENTHESIS, 
				TokenRIGHT_PARENTHESIS, 
				TokenASTERICK,
				false
			);
}

//-----------------------------------------------------------------------

RegExpr * RDParserFind::Tag(void){
	RegExpr * tagExpr = CreateRegExprWithSingleChild<TagExpr>(
							TokenLEFT_CURLY_BRACE, 
							TokenRIGHT_CURLY_BRACE, 
							TokenEOF,
							true
						);
	if (tagExpr) {
		assert(dynamic_cast<TagExpr *>(tagExpr));
		dynamic_cast<TagExpr *>(tagExpr)->SetTagSerialNumber(tagSerial++);
	}
	return tagExpr;
}

//-----------------------------------------------------------------------

RegExpr * RDParserFind::OR(void){
	assert(lookAhead == TokenLESS_THAN);

	OrExpr * or = new OrExpr();
	Match(TokenLESS_THAN);
	or->e1 = Regex();
	Match(TokenVERTICAL_BAR);
	or->e2 = Regex();
	Match(TokenGREATER_THAN);
	
	if( !HasError() ){
		or->e1->parent = or;
		or->e2->parent = or;
	}
	else{
		delete or;
		or = (OrExpr *)0;
	}
	return or;
}

//-----------------------------------------------------------------------

RegExpr * RDParserFind::String(void){
	assert(lookAhead == TokenREGEX_STRING);

	Match(TokenREGEX_STRING);

	StringExpr * str	= new StringExpr();
	str->text			= GetCurrToken().value.strVal;

	return str;
}

//-----------------------------------------------------------------------

RegExpr * RDParserFind::DeltaId(void)
	{	CREATE_AND_RETURN_NODE_EXPRESSION(TokenDELTA_ID, DeltaIdentExpr);	}

//-----------------------------------------------------------------------

RegExpr * RDParserFind::QuotedString(void) 
	{	CREATE_AND_RETURN_NODE_EXPRESSION(TokenQUOTED_STRING, QuotedStringExpr);	}
//-----------------------------------------------------------------------

RegExpr * RDParserFind::Integer(void) 
	{	CREATE_AND_RETURN_NODE_EXPRESSION(TokenINTEGER, IntegerExpr);	}

//-----------------------------------------------------------------------

RegExpr * RDParserFind::AnyCharacter(void)
	{	CREATE_AND_RETURN_NODE_EXPRESSION(TokenANY_CHARACTER, AnyCharExpr);	}

//-----------------------------------------------------------------------

RegExpr * RDParserFind::WhiteSpace(void)
	{	CREATE_AND_RETURN_NODE_EXPRESSION(TokenWHITE_SPACE, WhiteSpaceExpr);	}

//-----------------------------------------------------------------------

RegExpr * RDParserFind::Number(void){
	assert(lookAhead == TokenNUM);

	Match(TokenNUM);
	StringExpr * str	= new StringExpr();
	str->text			= string_cast(GetCurrToken().value.intVal);

	return str;
}

//-----------------------------------------------------------------------

RegExpr * RDParserFind::Epsilon(void)
	{ return new EpsilonExpr();	}

//-----------------------------------------------------------------------

void RDParserFind::PrintDeleteElements(void){
	std::cout << "=============================" << std::endl;
	std::cout << "size of list: " << toDelete.size() << std::endl;

	for(std::list<RegExpr *>::iterator i = toDelete.begin(); i != toDelete.end(); i++ )
		std::cout << (*i)->UF() << std::endl;

	std::cout << "=============================" << std::endl;
	return;
}

}	//namespace frep