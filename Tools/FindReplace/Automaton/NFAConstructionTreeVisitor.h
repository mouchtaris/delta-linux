/*
 * NFAConstructionTreeVisitor.h
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef NFACONSTRUCTIONSTREEVISITOR_H
#define NFACONSTRUCTIONSTREEVISITOR_H

#include <list>
#include <stack>
#include <stdio.h>
 
#include "NFA.h"
#include "RegExpr.h"
#include "Automaton.hpp"
#include "MyTemplates.h"
#include "RegExprTreeVisitor.h"


namespace frep {

class NFAConstructionTreeVisitor : public RegExprTreeVisitor {
private:
	//****************************
	//		private typedefs
	typedef Automaton::State		State;  
	typedef NFA::Transitions		Transitions;
	typedef std::pair<State, State> StartEndState;	//Arxikh, Telikh state


	//****************************
	//		private variables
	NFA		automaton;
	bool	unique;				//monadika sumbol isodou xrisimopieite sthn GetInputSymbols
	bool						anyAdded;
	bool						whiteSpacesAdded;
	bool						digitAdded;
	bool						letterAdded;
	std::list<State>			visitiedStates; //Kratame ola ta states pou htan palia arxikes telikes katastaseis
	std::list<std::string>		inputSymbols;	//Edw ta RegExpr * den 8a ta kanw delete egw
	std::stack<StartEndState>	stateStack;



	//****************************
	//		private functions
	void	CreateWhiteSpaceAutomaton (void);
	void	CreateAnyCharAutomaton(void);
	void	CreateDigitAutomaton (void);
	void	CreateLetterAutomaton (void);
	void	CreateZeroOrMoreAutoamton (void);
	void	CreateMultiOrAutomaton (unsigned);
	void	CreateOrConections (State, State);
	void	ConcatenateTwoAutomataWithEpsilon(void);


	//****************************
	//		private templates

	//-----------------------------------------------------------------------
	//	Dimiourgei ena automato, me transition to class T.
	//	To automato mpenei sthn state stack
	template<class T>
	void CreateAutomaton ( const T transition) {
		State s1			= automaton.NewState();
		State s2			= automaton.NewState();
		Transitions * t		= automaton.GetTransitions(s1);
		std::string token	= string_cast(transition);
		
		(*t)[token].push_back( s2 );
		inputSymbols.push_back(token);
		stateStack.push(StartEndState(s1,s2));
		return;
	}


	
public:
	virtual void VisitOr			(OrExpr*);	
	virtual void VisitTag			(TagExpr *);
	virtual void VisitString		(StringExpr*);
	virtual void VisitConcat		(ConcatExpr*);
	virtual void VisitEpsilon		(EpsilonExpr*);
	virtual void VisitAnyChar		(AnyCharExpr *);
	virtual void VisitOneOrMore		(OneOrMoreExpr *);
	virtual void VisitDeltaIdent	(DeltaIdentExpr*);
	virtual void VisitWhiteSpace	(WhiteSpaceExpr*);
	virtual void VisitZeroOrMore	(ZeroOrMoreExpr *);
	virtual void VisitIntegerExpr	(IntegerExpr *);
	virtual void VisitQuotedString	(QuotedStringExpr *);

	void PrintInputSymbolList (FILE * fp);
	NFA & GetNFA(void) { return automaton; }
	const std::list<std::string> &		GetInputSymbols(void);
	const std::stack<StartEndState> &	GetStateStack(void) const { return stateStack; }
	

	NFAConstructionTreeVisitor(void);
	~NFAConstructionTreeVisitor(void);
};


} //namespace frep

#endif