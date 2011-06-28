/*
 * Automaton.hpp
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef AUTOMATON_H
#define AUTOMATON_H

namespace frep {

	#define EPSILON		"Epsilon"	//String gia thn epsilon metabash

	class Automaton {
	public:
		typedef unsigned State;	
		virtual State NewState (void) = 0;
		virtual ~Automaton(void){}
	};

} // namespace frep
#endif