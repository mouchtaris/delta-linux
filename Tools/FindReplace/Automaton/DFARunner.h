/*
 * DFARunner.h 
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#ifndef DFARUNNER_H
#define DFARUNNER_H

#include "DFA.h"
#include "Automaton.hpp"
#include "TextSource.h"

namespace frep {

	class DFARunner {
	private:

		//********************************************************
		//		private typedefs
		typedef Automaton::State		State;
		typedef DFA::TagNumber			TagNumber;
		typedef TextSource::Block		Block;


		//********************************************************
		//		private variables
		DFA *	dfa;				//Den to kanw egw delete to pernw ka8e fora apo isodo
		bool	wholeWord;
		bool	isCaseSensitive;

		std::list<TagNumber>	activeTags;


		//****************************************************
		//		private functions
		const std::string MatchPolicy (const std::string) const;
		void Clear (void);
		void ManageTags (const State, const State, Block &, const std::string &);
		void ActivateTags (const State, const State, Block &, const std::string &);
		void DeactivateTags (const State, const State, Block &, const std::string &);
		void UpdateActiveTags (Block &, std::string);

	public:
		//isCaseSensitive, wholeWord
		void SetFindSetting (bool, bool);
		bool RunAutomaton (DFA *, TextSource& s, Block & block);
		
		DFARunner(void) : dfa( (DFA *)0 ) {}
		~DFARunner(void) { Clear(); }
	};



} // namespace frep 


#endif	//DFARUNNER_H