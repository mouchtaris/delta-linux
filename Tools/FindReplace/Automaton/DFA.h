/*
 * DFA.h
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef DFA_H
#define DFA_H

#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <stdlib.h>
#include <functional>

#include "NFA.h"
#include "Automaton.hpp"


namespace frep {

	class DFA : public Automaton {
	public:
		//****************************
		//		public struct declarations


		//****************************
		//		public typedefs declarations
		typedef unsigned										TagNumber;
		typedef std::list<TagNumber>							TagList; 
		typedef std::map<std::string, DFA::TagList>				TagStartEndState;
		typedef std::map<State, TagStartEndState>				DFATags;
		typedef std::set<State>									NFAStates;
		typedef std::map<std::string, State>					Transitions;



		//****************************
		//		public functions
		//Create and manage States of DFA
		State			NewState (void);
		bool			HasState (const std::set<State>*);
		State			GetState (NFAStates *);
		NFAStates &		GetStates (State);
		Transitions *	GetTransitions (State);
		const unsigned	GetTotal(void) const { assert(total == table.size()); return total; }



		//Manage start and end state of DFA
		void					SetDFAStartState (const State s) {startState = s; } 
		const State				GetDFAStartState (void) const { return startState;}
		bool					IsDFAEndState (const State e) const;
		void					AddDFAEndState (const State e) { endStates.insert(e); }
		const std::set<State>&	GetDFAEndStates (void) const { return endStates;}
		

		//Create and manage tag states of DFA
		bool			HasTags (void) const { return hasTags; }
		bool			HasTagStartStates (const State) const;
		bool			HasTagEndStates (const State) const;//Genika ama exei end states sthn sugkekrimenh katastash
		bool			HasTagStartStateAtSpecificState (const State, const TagNumber) const;
		bool			HasTagEndStateAtSpecificState (const State, const TagNumber) const;	
		void			AddTagStartState (const State, const TagNumber num);
		void			AddTagEndState (const State, const TagNumber num);
		const TagList &	GetTagsStartStates (const State) const;
		const TagList &	GetTagsEndStates (const State) const;



		void PrintDFA(FILE *);

		DFA (void);
		~DFA (void);


	private:
		//****************************
		//		private variables
		bool			hasTags;
		State			startState;
		unsigned		total;				//Egrafes sto table
		DFATags			tags;
		std::set<State>	endStates;			//edw set gia na mhn exoume duploegrafes
		std::vector< std::pair<NFAStates, Transitions> > table;
		
		
		//****************************
		//		private functors
		/*struct ClearFunctor : public std::unary_function<std::pair<NFAStates, Transitions>, void> {
			void operator() (std::pair<NFAStates, Transitions> p) const {
				p.first.clear();
				p.second.clear();
			}
		};//end of ClearFunctor
		*/
	};

} // namespace frep

#endif