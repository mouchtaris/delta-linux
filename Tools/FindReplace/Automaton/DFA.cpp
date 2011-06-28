/*
 * DFA.cpp
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#include "DFA.h"

#include <assert.h>
#include <iostream>
#include <algorithm>

#define START	"start"				//Dixnoume se ti katastash einai ena tag sto DFA
#define END		"end"


using namespace frep;

namespace frep {
//-----------------------------------------------------------------------

DFA::DFA (void):total(0) {
	hasTags = false;
	table.reserve(100);
}

//-----------------------------------------------------------------------

DFA::~DFA (void){
	//for_each(table.begin(),table.end(), ClearFunctor());
	table.clear();
	endStates.clear();

	for(DFATags::iterator i = tags.begin(); i != tags.end(); ++i){
		TagStartEndState::iterator j = i->second.begin();
		for(; j != i->second.end(); ++j) { j->second.clear(); }
		i->second.clear();
	}
	tags.clear();
}


//****************************************************
//		public functions

//-----------------------------------------------------------------------
Automaton::State DFA::NewState(void){
	table.push_back( std::pair<NFAStates, Transitions>());
	return total++;
}

//-----------------------------------------------------------------------
//	Elegxei an to DFA exei mia state sthn opia to set me ths NFa katastaseis
//	einai idio me to set pou tou dinete san orisma. Arkei na bre8ei mia
//	mono tetia katastash kai epistrefei true, alios epistrefei false
bool DFA::HasState (const std::set<State>* state){
	for (unsigned i = 0; i < total; ++i) {
		std::pair<NFAStates, Transitions>& p = table[i];
		if ( p.first == *state) return true;
	}//end of for
	
	return false; //if !total == 0 or state found
}

//-----------------------------------------------------------------------

DFA::Transitions * DFA::GetTransitions (State state) {
	assert(state < total);
	return &table[state].second;
}

//-----------------------------------------------------------------------

DFA::NFAStates & DFA::GetStates (State state){
	assert(state < total);
	return table[state].first;
}

//-----------------------------------------------------------------------

DFA::State DFA::GetState (NFAStates * state){
	assert(state && HasState(state) );
	std::vector< std::pair<NFAStates, Transitions> >::iterator i = table.begin();

	for( State s = 0; i != table.end(); i++, s++ )
		if( *state == (*i).first ) 
			return s;
	return 0;
}

//-----------------------------------------------------------------------

bool DFA::IsDFAEndState (State e) const 
	{ return (endStates.find(e) != endStates.end()) ? true : false; }

//-----------------------------------------------------------------------

void DFA::AddTagStartState (const State s, const TagNumber num){
	hasTags	= true;
	tags[s][START].push_back( num );
}

//-----------------------------------------------------------------------

void DFA::AddTagEndState (const State s, const TagNumber num){
	hasTags	= true;
	tags[s][END].push_back( num );
}

//-----------------------------------------------------------------------

bool DFA::HasTagStartStates (State s) const {
	assert( HasTags() );
	DFATags::const_iterator tag;
	if( ( (tag = tags.find(s)) != tags.end()) && 
		( tag->second.find(START) != tag->second.end()) ){
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------

bool DFA::HasTagEndStates (State s) const {
	assert( HasTags() );
	DFATags::const_iterator tag;
	if( ( (tag = tags.find(s)) != tags.end()) && 
		( tag->second.find(END) != tag->second.end()) ){
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------

bool DFA::HasTagStartStateAtSpecificState (const State state, const TagNumber num) const {
	DFATags::const_iterator ite = tags.find(state);
	if( (ite != tags.end()) && HasTagStartStates(state) ){

		DFA::TagList list = ite->second.find(START)->second;//lista
		if ( std::find(list.begin(), list.end(), num) != list.end() )
			return true;
	}
	return false;	
}

//-----------------------------------------------------------------------

bool DFA::HasTagEndStateAtSpecificState (const State state, const TagNumber num) const {
	DFATags::const_iterator ite = tags.find(state);
	if( (ite != tags.end()) && HasTagEndStates(state) ){

		DFA::TagList list = ite->second.find(END)->second;//lista
		if ( std::find(list.begin(), list.end(), num) != list.end() )
			return true;
	}
	return false;	
}

//-----------------------------------------------------------------------

const DFA::TagList & DFA::GetTagsStartStates (State s) const {
	assert( HasTagStartStates(s) );
	return tags.find(s)->second.find(START)->second;
}

//-----------------------------------------------------------------------

const DFA::TagList & DFA::GetTagsEndStates (State s) const {
	assert( HasTagEndStates(s) );
	return tags.find(s)->second.find(END)->second;
}

//-----------------------------------------------------------------------

void DFA::PrintDFA(FILE * fp){
	std::vector< std::pair<NFAStates, Transitions> >::iterator i = table.begin();	
	for(State s = 0; i != table.end(); i++, s++ ){
		fprintf(fp, "\n\nState: \t\t%ld \nNFAState(s): \t{ ", s);
		for(NFAStates::iterator j = (*i).first.begin(); j != (*i).first.end(); ++j) { fprintf(fp, "%ld ", (*j)); }
		fprintf(fp, "}\n");
		for( Transitions::iterator j = i->second.begin(); j != i->second.end(); j++){
			fprintf(fp, "Transition: \t%s\n", j->first.c_str());
			fprintf(fp, "Go to state: \t%ld\n", j->second);
		}
	}

	if( hasTags ){
		fprintf(fp, "\n########  Tags states  ########\n");
		for ( DFATags::iterator i = tags.begin(); i != tags.end(); ++i ){
			fprintf(fp, "\nState: \t\t%ld\n", i->first);
			for(TagStartEndState::iterator j = i->second.begin(); j != i->second.end(); ++j){
				fprintf(fp,"State of tags: \t%s\nTags Number: \t", j->first.c_str() );
				for(TagList::iterator z = j->second.begin(); z != j->second.end(); ++z) { fprintf(fp, "%ld ", (*z));}
				fprintf(fp, "\n");
			}//second for
		}//first for
	}
	
	fprintf(fp, "\nStar state of dfa are:\t\t%ld", GetDFAStartState());
	fprintf(fp,"\nEnd(s) state(s) of dfa are:\t{");
	for(std::set<State>::iterator i = endStates.begin(); i != endStates.end(); ++i) { fprintf(fp, "%ld ", (*i)); }
	fprintf(fp,"}\n\n\n");
	return;
}

}	//namespace frep