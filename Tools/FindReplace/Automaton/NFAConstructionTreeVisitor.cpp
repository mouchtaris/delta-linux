/*
 * NFAConstructionTreeVisitor.cpp
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#include <ctype.h>
#include <iostream>
#include <typeinfo>
#include "NFAConstructionTreeVisitor.h"


namespace frep {

#define KINDS_OF_WHITE_SPACE	6			/*Posa diaforetika idi white space exoumes*/


#define POP_FROM_STATE_STACK(START_END_STATE)	visitiedStates.push_back(START_END_STATE.first);		\
												visitiedStates.push_back(START_END_STATE.second);		\
												stateStack.pop()



static unsigned tagNum = 0;		//Global metablhth h' opoia metra twn ari8mwn twn tags

//-----------------------------------------------------------------------

NFAConstructionTreeVisitor::NFAConstructionTreeVisitor(void) 
:	unique(false), 
	anyAdded(false),
	letterAdded(false),
	digitAdded(false),
	whiteSpacesAdded(false)
{ tagNum = 0; }

//-----------------------------------------------------------------------

NFAConstructionTreeVisitor::~NFAConstructionTreeVisitor(void)
	{ inputSymbols.clear(); }


//****************************************************
//		Nonmembers functions

//-----------------------------------------------------------------------
//	Epistrefei thn le3ikografikh sugkrish duo strings
bool InputSymbolSorting(const std::string str1, const std::string str2)
	{ return str1 > str2;	}

//-----------------------------------------------------------------------
//	Epistrefei true an duo string einai le3ikografika isa alios false
bool InputSymbolUnique(const std::string str1, const std::string str2)
	{ return str1 == str2; }


//****************************************************
//		private functions

//-----------------------------------------------------------------------
//	Pernei thn arxikh kai telikh katastash tou or automatou.
//	Sthn sunexia pernei ena upoautomato apo thn stiba kai to sundeeei mesa
//	sto upokataskeuei or automato.
void NFAConstructionTreeVisitor::CreateOrConections ( State orStart, State orEnd ){
	StartEndState subAutomaton	= stateStack.top();
	Transitions * t1			= automaton.GetTransitions(orStart);
	Transitions * t2			= automaton.GetTransitions(subAutomaton.second);

	(*t1)[EPSILON].push_back(subAutomaton.first);
	(*t2)[EPSILON].push_back(orEnd);		

	POP_FROM_STATE_STACK(subAutomaton);
}

//-----------------------------------------------------------------------
//	Dimourgei to automato gia to digit. Auto pou kanei
//	einai na dimiourgei mia arxikh kai mia telikh katastash
//	apo thn opoia mporeis na pas me transitions pou einai
//	oi xaraktires apo to 0-9
void NFAConstructionTreeVisitor::CreateDigitAutomaton (void){
	State s1		= automaton.NewState();
	State s2		= automaton.NewState();
	Transitions& t	= *automaton.GetTransitions(s1);

	for( char i = '0'; i <= '9'; ++i ){
		std::string token = string_cast(i);// ( std::string() + i);
		t[token].push_back(s2);
		if (!digitAdded) { inputSymbols.push_back(token); }	
	}

	digitAdded = true;					//Gia na mhn 3anabalei ta idia input symbols sth lista
	stateStack.push(StartEndState(s1,s2));
}

//-----------------------------------------------------------------------
//	Dimourgei to automato gia to letter. Auto pou kanei
//	einai na dimiourgei mia arxikh kai mia telikh katastash
//	apo thn opoia mporeis na pas me transitions pou einai
//	oi xaraktires apo to A-Z kai apo to a-z.
void NFAConstructionTreeVisitor::CreateLetterAutomaton (void){
	State s1		= automaton.NewState();
	State s2		= automaton.NewState();
	Transitions& t	= *automaton.GetTransitions(s1);

	for( char i = 'A'; i <= 'Z'; ++i){
		std::string token = string_cast(i);// ( std::string() + i);
		t[token].push_back(s2);
		if (!letterAdded) 
			inputSymbols.push_back(token);
	}
	for( char i = 'a'; i <= 'z'; ++i){
		std::string token = string_cast(i);// ( std::string() + i);
		t[token].push_back(s2);
		if (!letterAdded) 
			inputSymbols.push_back(token);
	}
	
	letterAdded = true;					//Gia na mhn 3anabalei ta idia input symbols
	stateStack.push(StartEndState(s1,s2));
}

//-----------------------------------------------------------------------
//	Deimiourgei ta upoautomata tou whiteSpace ta opoia bazei sthn 
//	stoiba kai epistrefei ton ari8mo tous;
void NFAConstructionTreeVisitor::CreateWhiteSpaceAutomaton (void){
	State s1		= automaton.NewState();
	State s2		= automaton.NewState();
	Transitions& t	= *automaton.GetTransitions(s1);


	//Create transition for char ' '
	std::string token = string_cast(' ');
	t[token].push_back(s2);
	inputSymbols.push_back(token); 

	//The chars '\t', '\n', '\v', '\f', '\r'
	for(char i = 9; i <= 13; ++i) {
		std::string token = string_cast(i);
		t[token].push_back(s2);
		if (!whiteSpacesAdded) 
			inputSymbols.push_back(token);
	}
	whiteSpacesAdded = true;
	stateStack.push(StartEndState(s1,s2));
}

//-----------------------------------------------------------------------
//	Dimourgei to automato gia to AnyChar. Auto pou kanei
//	einai na dimiourgei mia arxikh kai mia telikh katastash
//	apo thn opoia mporeis na pas me transitions pou einai
//	print char or white spaces.
void NFAConstructionTreeVisitor::CreateAnyCharAutomaton (void) {

	State s1			= automaton.NewState();
	State s2			= automaton.NewState();
	Transitions& t		= *automaton.GetTransitions(s1);
	
	for(unsigned short i = 1; i <= 255; ++i) {
		if (isspace(i) || isprint(i)) {
			std::string token(std::string() + (char) i);
			t[token].push_back(s2);
			if (!anyAdded)
				inputSymbols.push_back(token);
		}
	}
	anyAdded = true;
	stateStack.push(StartEndState(s1,s2));
}

//-----------------------------------------------------------------------
//	Dimiourgei ena automaton ths morfhs (automaton)*. Dhl pernei 
//	ena automato apo thn stack kai ftiaxnei to parapanw automaton
//	bazontas to sthn stack.
void NFAConstructionTreeVisitor::CreateZeroOrMoreAutoamton (void){
	StartEndState subAutomaton = stateStack.top();
	POP_FROM_STATE_STACK(subAutomaton);

	//start to create (subAutomaton)* automaton
	State s1		= automaton.NewState();
	State s2		= automaton.NewState();
	Transitions * t = automaton.GetTransitions(s1);

	// new start state with epsilon to old start state and new end state
	(*t)[EPSILON].push_back(subAutomaton.first);
	(*t)[EPSILON].push_back(s2);

	t = automaton.GetTransitions(subAutomaton.second);

	//old end state with epsilon to old start state and new end state
	(*t)[EPSILON].push_back(subAutomaton.first);
	(*t)[EPSILON].push_back(s2);

	stateStack.push(StartEndState(s1, s2));
}

//-----------------------------------------------------------------------
//	Pernei ton ori8mo twn automatwn pou 8eloume na paroume apo thn stiba
//	kai ta enwnei ola auta se ena multi or automato to opoio bazei sthn 
//	stack me ta automata.
void NFAConstructionTreeVisitor::CreateMultiOrAutomaton (unsigned numOfAutomata){
	assert(numOfAutomata >= 2);

	State orStart	= automaton.NewState();
	State orEnd		= automaton.NewState();

	for( unsigned i = 0; i < numOfAutomata; i++ ) 
		CreateOrConections(orStart, orEnd);
	stateStack.push(StartEndState(orStart, orEnd));
}

//-----------------------------------------------------------------------
//	concatanete two automata with Epsilon transition
void NFAConstructionTreeVisitor::ConcatenateTwoAutomataWithEpsilon(void) {
	assert(stateStack.size() >= 2);

	//take the last automaton of the stack -> a1
	StartEndState a1		= stateStack.top();
	POP_FROM_STATE_STACK(a1);

	//take the last to last automaton of the stack -> a2
	StartEndState a2		= stateStack.top();
	POP_FROM_STATE_STACK(a2);

	
	//from a2.end state with epsilon transition go to a1.start state
	Transitions * t = automaton.GetTransitions(a2.second);
	(*t)[EPSILON].push_back(a1.first);
	
	//The start state of the new automaton is the a2.start state
	//The end state of the new automaton is the a1.end state
	stateStack.push(StartEndState(a2.first, a1.second));
}


//****************************************************
//		public functions

//-----------------------------------------------------------------------
//		Estw exw to string fun.
//		To automato pou 8a dimiourgi8ei 8a einai ths morhs
//		apo thn State 1 me f paw sthn 2
//		apo thn State 2 me u paw sthn 3
//		apo thn State 3 me n paw sthn 4
//
void NFAConstructionTreeVisitor::VisitString (StringExpr * e){
	State s0, s1, s2;
	std::string symbol;
	
	s0	= automaton.NewState();
	s1	= s0;
	
	for( std::string::iterator i = e->text.begin(); i != e->text.end(); ++i ){
		symbol.clear();
		symbol			= (*i);
		s2				= automaton.NewState();
		Transitions * t = automaton.GetTransitions(s1);
		(*t)[symbol].push_back(s2);		
		inputSymbols.push_back( symbol);
		s1 = s2;
	}
	stateStack.push(StartEndState(s0,s2));
}

//-----------------------------------------------------------------------
//	Dimiourgei ena automato me Epsilon metabash.
void NFAConstructionTreeVisitor::VisitEpsilon (EpsilonExpr * e)
	{ CreateAutomaton<std::string>(EPSILON); }

//-----------------------------------------------------------------------
//	Dimourgei to automato gia to AnyChar. Auto pou kanei
//	einai na dimiourgei mia arxikh kai mia telikh katastash
//	apo thn opoia mporeis na pas me transitions pou einai
//	print char or white spaces.
void NFAConstructionTreeVisitor::VisitAnyChar (AnyCharExpr * e)
	{	CreateAnyCharAutomaton();	}

//-----------------------------------------------------------------------
//	Gia DeltaId dimiourgoume ena upoautomato ths morfhs: letter(letter|digit|_)*
//	opou letter einai ena automaton ths morfhs (A|B|C|...Z|a|b|c|...|z)
//	kai opou digit einai en automato ths morfhs	0|1|...|9
void NFAConstructionTreeVisitor::VisitDeltaIdent (DeltaIdentExpr * e){
	//Dimourgoume ta 2 letter to digit digit kai to underscore
	CreateLetterAutomaton();

	CreateLetterAutomaton();
	CreateDigitAutomaton();
	CreateAutomaton<char>('_');	//We create an automaton that can regognize the underscore

	//create automaton letter|digit|_ (in the stack we have:letter, letter, digit, underscore) 
	CreateMultiOrAutomaton (3);

	//create (letter|digit|_)* automaton 
	CreateZeroOrMoreAutoamton();

	//Start to create letter(letter|digit|_)* automaton
	//Inside stack we have two automata: letter, (letter|digit|_)*
	ConcatenateTwoAutomataWithEpsilon();
}

//-----------------------------------------------------------------------
//	San white space theroume ta e3eis:
//	' '		= space (SPC)
//	'\t'	= horizontal tab (TAB)
//	'\n'	= newline (LF)
//	'\v'	= vertical tab (VT)
//	'\f'	= feed (FF)
//	'\r'	= carriage return (CR)
//	To automato pou 8a dimiourgisoume 8a exei thn morfh: (isspace)*
//	Opou gia to isspace 8a einai ena or me ths parapanw 6 periptwseis;
void NFAConstructionTreeVisitor::VisitWhiteSpace (WhiteSpaceExpr * e){
	//create the isspace automaton
	CreateWhiteSpaceAutomaton();

	//Create (isspace)* automaton
	CreateZeroOrMoreAutoamton();
}

//-----------------------------------------------------------------------
//	QuotedString := ".*"
void NFAConstructionTreeVisitor::VisitQuotedString(QuotedStringExpr * expr) {
	CreateAutomaton<char>('"');	//We create an automaton that can regognize the double qota
	CreateAnyCharAutomaton();
	CreateZeroOrMoreAutoamton();
	
	//The last two automata of the stack are: ", .*
	ConcatenateTwoAutomataWithEpsilon();
	CreateAutomaton<char>('"');	//We create an automaton that can regognize the double qota

	//The last two automata of the stack are: ".*, "
	ConcatenateTwoAutomataWithEpsilon();
}

//-----------------------------------------------------------------------
//	Integer := digit(digit)*
void NFAConstructionTreeVisitor::VisitIntegerExpr(IntegerExpr *) {
	CreateDigitAutomaton();
	CreateDigitAutomaton();
	CreateZeroOrMoreAutoamton();

	//The last two automata of the stack are: digit, (digit)*
	ConcatenateTwoAutomataWithEpsilon();
}

//-----------------------------------------------------------------------
//	Sto tag exoume ena automato pou exei mia arxikh kai telika 
//	katastash. Auto pou pame kai kanoume einai na dimiourgoume 
//	mia new Aarxikh kai mia new telika katastash h opoies 
//	anaparistoun thn arxikh kai telikh katastah tou tag. 
//	H arxikh katastash tou tag ginete h arxhkh katastash tou 
//	automa tou kai h telikh tou ( tou tag) ginete h telika
//	katastash tou automato. H kenouria arxikh enwnete me thn 
//	palia katastash tou automa tou me epsilon, kai h palia 
//	telikh enwnete me thn kenouria telikh me epsilon
void NFAConstructionTreeVisitor::VisitTag (TagExpr * expr){
	State s1		= automaton.NewState();
	State s2		= automaton.NewState();
	Transitions* t	= automaton.GetTransitions(s1);

	StartEndState child = stateStack.top();
	POP_FROM_STATE_STACK(child);

	// s->Tag(e)start state with epsilon
	// Tag start state with e to old start state
	(*t)[EPSILON].push_back(child.first);

	// from old end state with epsilon to tag end state
	t = automaton.GetTransitions( child.second );
	(*t)[EPSILON].push_back(s2);

	//8elw oles tis katastaseis pou periexontai mesa sto tagStart, tagEnd
	std::list<State>::iterator i = visitiedStates.begin();
	std::set<State> containingStates;
	for(; i != visitiedStates.end(); ++i)
		containingStates.insert(*i);
		
	containingStates.insert(s1);
	containingStates.insert(s2);
	automaton.AddTagsStates( NFA::NFATagInfo(s1,s2, ++tagNum, containingStates) );
	stateStack.push(StartEndState(s1,s2));
}

//-----------------------------------------------------------------------

void NFAConstructionTreeVisitor::VisitOneOrMore	(OneOrMoreExpr *) {
	StartEndState subAutomaton = stateStack.top();
	POP_FROM_STATE_STACK(subAutomaton);

	//start to create (subAutomaton)* automaton
	State s1		= automaton.NewState();
	State s2		= automaton.NewState();
	Transitions * t = automaton.GetTransitions(s1);

	// new start state with epsilon to old start state and new end state
	(*t)[EPSILON].push_back(subAutomaton.first);

	t = automaton.GetTransitions(subAutomaton.second);

	//old end state with epsilon to old start state and new end state
	(*t)[EPSILON].push_back(subAutomaton.first);
	(*t)[EPSILON].push_back(s2);

	stateStack.push(StartEndState(s1, s2));
}

//-----------------------------------------------------------------------
//	Arxika pros8etoume mia new start state kai mia new end state.
//	H new start state me epsilon pigenei sthn old start state kai sthn
//	new end state. H old end state me epsilon pigenei sthn new end state
//	Episeis me epsilon apo thn old end state pigenoume sthn old start state.
void NFAConstructionTreeVisitor::VisitZeroOrMore (ZeroOrMoreExpr * expr)
	{ CreateZeroOrMoreAutoamton(); }

//-----------------------------------------------------------------------
//Edw 2 giati uparxoun 2 automata sthn stoiba mas pou 
//8eloume na ta enwsoume sthn stoiba mas
void NFAConstructionTreeVisitor::VisitOr (OrExpr * expr)
	{ CreateMultiOrAutomaton(2); }

//-----------------------------------------------------------------------
//	Enwnei duo automata pou uparxoun sto stack.
void NFAConstructionTreeVisitor::VisitConcat (ConcatExpr * expr){
	assert(stateStack.size() >= 2);
	StartEndState secondAutomato	= stateStack.top();
	POP_FROM_STATE_STACK(secondAutomato);
	StartEndState firstAutomato		= stateStack.top();
	POP_FROM_STATE_STACK(firstAutomato);

	Transitions * t					= automaton.GetTransitions(firstAutomato.second);
	(*t)[EPSILON].push_back(secondAutomato.first);
	stateStack.push( StartEndState(firstAutomato.first, secondAutomato.second) );
	
	if (automaton.HasTags()) {
		TagExpr * tag1 = dynamic_cast<TagExpr *>(expr->e1);
		TagExpr * tag2 = dynamic_cast<TagExpr *>(expr->e2);

		if(tag1) {
			NFA::NFATagInfo & tagInfo	= automaton.GetTag(tag1->GetTagSerialNumber());
			tagInfo.containingStates.insert(secondAutomato.first);
		}
		if(tag2) {
			NFA::NFATagInfo & tagInfo = automaton.GetTag(tag2->GetTagSerialNumber());
			tagInfo.containingStates.insert(firstAutomato.second);
		}
	}
}

//-----------------------------------------------------------------------
//	Thn prwth fora pou kalite prepei na 8a aferesei apo thn list me ta 
//	input symbols oles ths diploegrafes. Gia na to kanei auto  8a prepei
//	na ginei ta3inomish prwta.
const std::list<std::string> & NFAConstructionTreeVisitor::GetInputSymbols(void) {
	if (!unique){
		unique = true;
		assert(inputSymbols.size() > 0);
		inputSymbols.sort(InputSymbolSorting);
		inputSymbols.unique(InputSymbolUnique);
	}
	return inputSymbols;
}

//-----------------------------------------------------------------------

void NFAConstructionTreeVisitor::PrintInputSymbolList (FILE * fp){
	unsigned cnt = 0;
	for(std::list<std::string>::iterator i = inputSymbols.begin(); i != inputSymbols.end(); ++i, cnt++)
		fprintf(fp, "%ld)\t>%s<\n", cnt, i->c_str());
}

} //namespace frep