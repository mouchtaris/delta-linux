/*
 * NFA.h
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#ifndef NFA_H
#define NFA_H

#include <assert.h>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <stdlib.h>
#include <functional>



#include "Automaton.hpp"


namespace frep {

	class NFA : public Automaton{
	public:
		//****************************
		//		public declarations
		typedef std::list<State>		States;
		typedef std::pair<State, State> StartEndState;


		struct NFATagInfo {
			State			start;
			State			end;
			unsigned		tagNum;
			std::set<State> containingStates; //ola ta states pou perikluontai anamesa sthn start kai thn end state
			
			NFATagInfo( State _start, 
						State _end, 
						unsigned num,
						const std::set<State> & conStates) :
				start (_start),
				end (_end),
				tagNum (num),
				containingStates (conStates)
			{}
		};


		//key ka8e string 8a einai enas mono char, sthn periptwsh tou epsilon 8a einai to string epsilon
		typedef std::map<std::string, States> Transitions;


		//****************************
		//		public functions
		
		//Manage NFA states
		State			NewState (void);
		Transitions *	GetTransitions (State );
		void			RemoveAllTransitions (State );
		void			SetNFAStartState (State s) { startState = s; }
		const State		GetNFAStartState (void) const { return startState; }
		void			SetNFAEndState (State e) { endState = e; }
		const State		GetNFAEndState (void) const { return endState; }
		const unsigned	GetTotal(void) const { assert(total == table.size()); return total; }


		//Manage tag NFA states
		bool							HasTags (void) const { return hasTags; }
		void							AddTagsStates (NFATagInfo &);
		NFATagInfo &					GetTag (unsigned tagNum) { 
											assert(tagNum < tags.size());
											return tags[tagNum]; 
										}
		std::vector<NFATagInfo>&		GetTagsStates (void) { return tags; }


		void PrintNFA(FILE *);
		

		NFA  (void);
		~NFA (void);


	private:
		//****************************
		//		private variables
		bool		hasTags;
		State		startState;
		State		endState;
		unsigned	total;

		std::vector<NFATagInfo>			tags;
		std::vector<NFA::Transitions>	table;
		

		//****************************
		//		private functors
		struct ClearTransitionsFunctor : public std::unary_function<Transitions, void> {
			void operator() (Transitions t) const {
				for(Transitions::iterator i = t.begin(); i != t.end(); i++) 
				{ i->second.clear(); }
				t.clear();
			}
		};//end of ClearTransitionsFunctor
	};

} //namespace frep

#endif