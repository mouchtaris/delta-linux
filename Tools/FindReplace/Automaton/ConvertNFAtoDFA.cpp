/*
 * ConvertNFAtoDFA.cpp
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#include <list>
#include <stack>
#include <assert.h>
#include <typeinfo>
#include <iostream>
#include <algorithm>


#include "KeyLogger.h"
#include "MyTemplates.h"
#include "ConvertNFAtoDFA.h"


namespace frep {

ConvertNFAtoDFA::~ConvertNFAtoDFA(void)
	{ CleanAll(); }

//****************************************************
//		Private functions

//-----------------------------------------------------------------------
//	State einai apo poio state tou NFA 8a arxizei h anazhthsh gia e metabaseis
//	NFAStates * einai pointer se NFAState sto opoio prokite na kanoume thn isagwgh
//	ton epsilon states
//	H ebresh ton state pou mporoume na pame me epsilon katastaseis 8a ginei me thn xrish 
//	depth first alori8mou se grafo kombwn pou einai prositoi apo ena dedomeno sunolo kombwn
ConvertNFAtoDFA::NFAStates * ConvertNFAtoDFA::EpsilonClosure(State state, NFAStates * states){	
	
	std::stack<State> stateStack;		//to stack tou depth first algorithm
	stateStack.push(state);

	while( !stateStack.empty() ){
		State tmp = stateStack.top();
		stateStack.pop();

		NFATransitions * t = nfa->GetTransitions(tmp); assert(t);

		//elegxoume an uparxei epsilon transition apo ena sugkekrimeno state tou NFA automaton
		if( (*t).find(EPSILON) != (*t).end() ){
			
			//Diatrexoume olh thn lista pou mporoume na pame me epsilon transitions
			std::list<State>::iterator start	= (*t)[EPSILON].begin();
			std::list<State>::iterator end		= (*t)[EPSILON].end();
			for(; start != end; start++){
				//An thn sugkekrimenh katastash den thn exoume mesa sto set pou filame tis
				//nfa katastaseis tote to proo8oume sthn stack oste na to ele3oume argotera
				if( states->find((*start)) == states->end() ) { stateStack.push((*start)); }
			}
		}

		states->insert(tmp);	//isagoume to sigkekrimeno state sto set me ta NFA states
								//To bazoume edw giati parolo pou mporei na mhn exei epsilon metabash
								//to state pou pernoume san isodo prepei na to isagoume mesa sto set mas
	}
	return states;
}

//-----------------------------------------------------------------------
//	Dexete ena sunolo apo nfa katastaseis. Gia thn ka8e mia 8a upologisei
//	oles tis epsilon transition pou exei kai tis bazei mesa se ena ena
//	kainourio set apo katastaseis.
ConvertNFAtoDFA::NFAStates * ConvertNFAtoDFA::EpsilonClosure(NFAStates * states){
	NFAStates * newNFAStates = new NFAStates();
	for( NFAStates::iterator i = states->begin(); i != states->end(); i++){
		newNFAStates = EpsilonClosure((*i), newNFAStates);
	}
	return newNFAStates;
}

//-----------------------------------------------------------------------
//	To state einai to sunolo ton katastasewn tou NFA pou 8a ele3oume 
//	an exoun transitions me to sigkekrimeno sumbolo isodou
//	8a epistreyoume ena neo sinolo me oles tis katastaseis stis opoies
//	mporesame na pame me to sigkekrimeno sumbolo isodou
//	Pos to kanoume: Gia ka8e mia state pou uparxei mesa sto NFAStates pou 
//	pernoume san isodo blepoume an exei transitions me to sugkekrimeno 
//	sumbolo isodou. An exei tote pros8etoume oles ths tis katastaseis
//	pou pigenei mesa sto set pou 8a epistreyoume
ConvertNFAtoDFA::NFAStates * ConvertNFAtoDFA::Move(NFAStates * state, const std::string inputSym) {
	assert(state);

	NFAStates * returnStates = new NFAStates();
	statesToDelete.push_back(returnStates);
	
	for(NFAStates::iterator i = state->begin(); i != state->end(); ++i){
		NFATransitions * t = nfa->GetTransitions( (*i) );	assert(t);
		NFATransitions::iterator mapIte;
		if ( (mapIte = t->find(inputSym)) != t->end() ){

			std::list<State>::iterator listIte = mapIte->second.begin();
			for(; listIte != mapIte->second.end(); ++listIte)
				returnStates->insert( (*listIte) );
		}
	}
	return returnStates;
}

//-----------------------------------------------------------------------

void ConvertNFAtoDFA::AddStartEndState (NFAStates *T, State s){
	if( T->find(nfa->GetNFAStartState()) != T->end() )
		dfa->SetDFAStartState(s);

	if( T->find( nfa->GetNFAEndState() ) != T->end() )
		dfa->AddDFAEndState(s);
	return;	
}

//-----------------------------------------------------------------------

void ConvertNFAtoDFA::AddTagStartEndState (NFAStates *T, State s){
	std::vector<NFA::NFATagInfo> tags = nfa->GetTagsStates();

	std::vector<NFA::NFATagInfo>::iterator i;
	for( i = tags.begin(); i != tags.end(); ++i ) {
		if( T->find( i->start ) != T->end() )
			dfa->AddTagStartState( s, i->tagNum );			

		//if( T->find( i->end ) != T->end() && Contains(i->containingStates, *T))
		if( T->find( i->end ) != T->end() )
			dfa->AddTagEndState( s, i->tagNum );				
	}
	return;
}

//-----------------------------------------------------------------------

void ConvertNFAtoDFA::AddTagsStates(){
	unsigned cnt = 0;
	for(DFAStates::iterator i = dstates.begin();  i != dstates.end(); ++i, ++cnt)
		AddTagStartEndState((*i), cnt);
	return;
}

//-----------------------------------------------------------------------

bool ConvertNFAtoDFA::CanPruneTransitions( State s )
	{ return dfa->GetStates(s).empty(); }

//-----------------------------------------------------------------------

void ConvertNFAtoDFA::Dtran(NFAStates *T, NFAStates * U, const std::string inputSym){
	assert(T && U);
	State st;
	State su;

	if ( !dfa->HasState(T) ){
		st = dfa->NewState();
		dfa->GetStates(st) = *T;
	}
	else
		st = dfa->GetState(T);
	
	if( !dfa->HasState(U) ){
		su = dfa->NewState();
		dfa->GetStates(su) = *U;
	}
	else
		su = dfa->GetState(U);

	AddStartEndState(T, st);
	if( CanPruneTransitions(su) ) { return; } //an pigenoume se kenh state to stamatame

	DFA::Transitions * t = dfa->GetTransitions(st);
	(*t)[inputSym] = su;
	return;
}



//****************************************************
//		Public functions
//-----------------------------------------------------------------------
//Ylopoihsh tou algori8mou gia thn metatroph enos NFA se DFA automato
DFA * ConvertNFAtoDFA::Convert(
		NFA * _nfa, 
		DFA * _dfa, 
		const std::list<std::string> &inputSym
	) {
	assert(_nfa && _dfa);
	nfa			= _nfa;
	dfa			= _dfa;

	dstates.push_back( EpsilonClosure(nfa->GetNFAStartState(), new NFAStates()) );	
	for( DFAStates::iterator T = dstates.begin(); T != dstates.end(); T++){
		std::list<std::string>::const_iterator i = inputSym.begin();
		for(; i != inputSym.end(); i++ ){
			NFAStates * U = EpsilonClosure( Move((*T), (*i)) );
			
			//>0 oste ta kena  state na mhn ta bazw sto automato mou
			//if( !dfa->HasState(U) && ( !U->empty() )) { 
			if( !dfa->HasState(U) )  
				dstates.push_back(U); 
			Dtran((*T),U,(*i));
		}
	}
	if( nfa->HasTags() ) { AddTagsStates(); }
	return dfa;
}

//-----------------------------------------------------------------------

void ConvertNFAtoDFA::CleanAll(void){
	nfa = (NFA *)0;
	dfa = (DFA *)0;

	for_each(dstates.begin(), dstates.end(), ClearFunctor());
	dstates.clear();

	for_each(statesToDelete.begin(), statesToDelete.end(), ClearFunctor());
	statesToDelete.clear();
}

}	//namespace frep