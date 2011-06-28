/*
 * DFAMatching.h 
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#include <assert.h>



#include "DFAMatching.h"

#include "NFA.h"
#include "RegExpr.h"
#include "ConvertNFAtoDFA.h"
#include "NFAConstructionTreeVisitor.h"




namespace frep {

#define FAIL(X)		SetError(X); return false


//-----------------------------------------------------------------------
DFAMatching::DFAMatching(void) : inputStr(""), dfa( (DFA *)0 ) {}

//-----------------------------------------------------------------------

void DFAMatching::SetError(const std::string str){
	this->hasError = true;
	this->errorMsg = str;
	return;
}

//-----------------------------------------------------------------------

DFAMatching::~DFAMatching(void) {
	if (!dfa){
		delete dfa;
		dfa = (DFA *)0;
	}
}

//******************************************************************************************
//		private functions

//-----------------------------------------------------------------------
//	An to string pou pirame san eisodo einai idio me auto pou eixame kai paliotera
//	Den xriazete na dimiourgisoume neo automato uparxei idi.

//-----------------------------------------------------------------------
bool DFAMatching::IsNewAutomaton (const std::string str) const 
	{ return  (!inputStr.compare(str) && dfa)?false:true;	}

//-----------------------------------------------------------------------

void DFAMatching::CreateAutomaton (RegExpr * expr){
	//Create NFA
	NFAConstructionTreeVisitor nfaCons;
	expr->Accept(&nfaCons);
	NFA & nfa = nfaCons.GetNFA();
	nfa.SetNFAStartState(nfaCons.GetStateStack().top().first);
	nfa.SetNFAEndState(nfaCons.GetStateStack().top().second);

	assert( nfaCons.GetStateStack().size() == 1 );
	
	//Convert to DFA
	ConvertNFAtoDFA converter;
	dfa = converter.Convert(&nfa, new DFA(), nfaCons.GetInputSymbols());
	return;
}

//-----------------------------------------------------------------------

RegExpr *	DFAMatching::CreateRegularExpression (void){
	RegExpr * e = (RegExpr *)0;

	if( analyser.Analyse(inputStr) ){ 
		if ( !(e = parserFind.Analyse( analyser.GetTokens() )) ){
			if( parserFind.HasError() ) { FAIL(parserFind.GetError()); }
		}
	}//first if
	else { FAIL(analyser.GetError()); }
	return e;
}

//******************************************************************************************
//		public functions

//-----------------------------------------------------------------------

void DFAMatching::Clear (void) {
	parserFind.Clear();
	analyser.Clear();
	TagExpr::Initialise();

	hasError = false;
	errorMsg.clear();

	if (!dfa){
		delete dfa;
		dfa = (DFA *)0;
	}
	return;
}

//-----------------------------------------------------------------------

void DFAMatching::SetFindSetting (bool isCaseSensitive, bool wholeWord) 
	{ runner.SetFindSetting(isCaseSensitive, wholeWord); }

//-----------------------------------------------------------------------

bool DFAMatching::Run (const std::string str, TextSource& s, TextSource::Block & block){
	if ( IsNewAutomaton(str) ) {
		Clear();
		inputStr = str;
		if ( RegExpr * e = CreateRegularExpression() ){
			CreateAutomaton( e );
		}
		else { return false; }			//apetixe to parsing pou egine
	}

	bool result = runner.RunAutomaton(dfa, s, block);
	if (result)
		block.file = &s;
	return result;
}

}	//namespace frep