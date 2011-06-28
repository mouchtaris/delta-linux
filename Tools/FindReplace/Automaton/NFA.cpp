/*
 * NFA.cpp
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#include "NFA.h"

#include <string>
#include <iostream>
#include <assert.h>
#include <algorithm>


namespace frep {

	//-----------------------------------------------------------------------

	NFA::NFA (void):total(0) {
		hasTags = false;
		table.reserve(100);
	}

	//-----------------------------------------------------------------------

	NFA::~NFA (void){
		for_each(table.begin(), table.end(), ClearTransitionsFunctor());
		hasTags = false;
		tags.clear();
		table.clear();
	}

	//****************************************************
	//		public functions

	//-----------------------------------------------------------------------
	
	void NFA::RemoveAllTransitions (State state){
		assert(state < total);

		for(Transitions::iterator s = table[state].begin(); s != table[state].end(); s++){
			s->second.clear();
		}
		table[state].clear();
		return;
	}

	//-----------------------------------------------------------------------
	
	Automaton::State NFA::NewState (void) { 
		table.push_back(Transitions());
		return total++;
	}

	//-----------------------------------------------------------------------
	
	NFA::Transitions * NFA::GetTransitions (State state) {
		assert(state < total);
		return &table[state];
	}

	//-----------------------------------------------------------------------
	
	void NFA::AddTagsStates(NFATagInfo &tag){
		hasTags = true;
		tags.push_back(tag);
		return;
	}

	//-----------------------------------------------------------------------
	
	void NFA::PrintNFA(FILE *fp) {
		unsigned cnt = 0;
		std::vector<NFA::Transitions>::iterator i = table.begin();
		for(; i != table.end(); ++i, cnt++ ){
			Transitions::iterator mapStart = i->begin();
			for(; mapStart != i->end(); ++mapStart){
				fprintf(fp, "\n\nState number: \t%ld\n", cnt);
				fprintf(fp, "Transition: \t%s\nGo to state(s): {",mapStart->first.c_str());
				for(States::iterator j = mapStart->second.begin(); j != mapStart->second.end(); ++j)
				{ fprintf(fp, "%ld ", (*j)); }
			}
			fprintf(fp, "}\n");
		}
		if( hasTags ){
			fprintf(fp, "########  Tags states  ########\n");
			std::vector<NFATagInfo>::iterator i = tags.begin();
			for(; i != tags.end(); i++){
				fprintf(fp, "Tag number: \t\t%ld\n", i->tagNum);
				fprintf(fp, "Tag start state: \t%ld\n", i->start);
				fprintf(fp, "Tag end state: \t\t%ld\n", i->end);
			}
		}
		fprintf(fp, "\nStar state of nfa are: \t%ld", startState);
		fprintf(fp, "\nEnd state of nfa are: \t%ld", endState);
		return;
	}

}	//namesapce frep