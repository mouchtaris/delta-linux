/*
 * ConvertNFAtoDFA.h
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#ifndef CONVERTNFATODFA_H
#define CONVERTNFATODFA_H

#include <set>
#include <list>
#include <string>
#include <functional>


#include "NFA.h"
#include "DFA.h"
#include "RegExpr.h"
#include "Automaton.hpp"


namespace frep {

	class ConvertNFAtoDFA {

	private:
		//****************************
		//		private typedefs
		typedef DFA::NFAStates			NFAStates;
		typedef Automaton::State		State;
		typedef NFA::Transitions		NFATransitions;
		typedef DFA::Transitions		DFATransitions;
		typedef std::list<NFAStates *>	DFAStates;
		

		//****************************
		//		private variables
		NFA * nfa;
		DFA * dfa;
		DFAStates				dstates;
		std::list<NFAStates *>	statesToDelete;

		//****************************
		//		private functors
		struct ClearFunctor : public std::unary_function<NFAStates *, void> {
			void operator() (NFAStates * state) const {
				if( state ) 
					delete state; 
			}
		};//end of ClearFunctor


		//****************************
		//		private functions

		//Einai sthn ousia h e-closure(s)
		NFAStates * EpsilonClosure (State, NFAStates * );
		
		//Otan kaleite me to orisma pou epistrefeu h Move
		//Sthn ousia einai h e-closure(T);
		NFAStates * EpsilonClosure (NFAStates *);
		
		//Ypologizei oles ths kiniseis pou mporoun na ginoun 
		//apo ena sunolo katastasewn me ena sugkekrimeno sumbolo isodou
		NFAStates * Move (NFAStates *, const std::string);

		//kanei add sto DFA mia kainouria katastash
		void Dtran(NFAStates *T, NFAStates * U, const std::string);

		//pros8eth thn arxikh kai telika ktastash an uparxoun
		void AddStartEndState (NFAStates *, State );

		//pros8eth ths arxikes kai telikes katastashs twn tags expressions sto DFA
		void AddTagStartEndState (NFAStates *, State );
		void AddTagsStates (void);

		//Diagrafei ta tranistion pou exei mia kenh katastash
		bool CanPruneTransitions( State s ); //poll

	public:	

		//Kanei to convert gia ena NFA se DFA
		DFA * Convert (NFA *, DFA *, const std::list<std::string> &);
		void CleanAll(void);
		ConvertNFAtoDFA(void){}
		~ConvertNFAtoDFA(void);
	};


} //namespace frep

#endif