/*
 * DFARunner.cpp 
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#include <assert.h>
#include <algorithm>

#include "DFARunner.h"
#include "MyTemplates.h"


#define MARK() unsigned marker = s.Mark()
#define FAIL() currFind = -1; return false
#define UNMARK() s.Retract(marker); s.Unmark(marker); return result




namespace frep {

//******************************************************************************************
//		Nonmembers functions

//-----------------------------------------------------------------------
//	use in RunAutomaton
static bool IsWholeWord(TextSource &s, const unsigned marker, const unsigned charNum) {

	char last = s.GetSpecificChar(marker+ charNum);
	
	if( (marker != 0) && (last != EOF) ) { 
		if( isalnum(s.GetSpecificChar(marker-1)) && 
			isalnum(s.GetSpecificChar(last)) ) { return false; }
		else { return true; }
	}
	else if ( (marker == 0) && (last == EOF) ){
		return true;			//single word in the file
	}
	else if ( marker == 0 ){	//First word in file
		if( isalnum(last) ) { return false; }
		else { return true; }
	}
	else{						//Last word in file
		if( isalnum(s.GetSpecificChar(marker-1)) ) { return false; }
		else { return true; }
	}//end of else 

	return false;	
}	


//******************************************************************************************
//		private functions

//-----------------------------------------------------------------------

const std::string DFARunner::MatchPolicy (const std::string str) const {
	if( !isCaseSensitive )
		return ToLowerCopy(str);
	return str;
}

//-----------------------------------------------------------------------
void DFARunner::Clear (void)
	{	activeTags.clear();	}

//-----------------------------------------------------------------------
//	State einai to state pou 8a kita3oume na doume an exei tag kai se ti katastash einai
//	kai string einai to sting pou 8a kanoume append an ginete :D
void DFARunner::ManageTags (
		const State cur, 
		const State next, 
		Block & block, 
		const std::string & str
	) {
	block.hasTags = true;
	//8elw na kanw append kai sto end state gia to logo oti mporei
	//na exw brei thn end state enos tag kai na brw alh mia pou
	//prepei na kanw pali append
	if (dfa->HasTagEndStates(cur))
		DeactivateTags (cur, next, block, str);

	if (!activeTags.empty())
		UpdateActiveTags( block, str);
	
	if (dfa->HasTagStartStates(cur))
		ActivateTags (cur, next, block, str);
}

//-----------------------------------------------------------------------

void DFARunner::ActivateTags (	
		const State cur, 
		const State next, 
		Block & block, 
		const std::string & str
	) {
	const DFA::TagList & list = dfa->GetTagsStartStates(cur);
	
	//diatrexoume olh thn lista me ta tag pou exoun arxikes katastaseis se auth thn state an uparxoun
	for(DFA::TagList::const_iterator i = list.begin(); i != list.end(); ++i ){
		//An sthn katastash pou prokite na pame einia pali arxiki katastash tou idiou tag den to energopioume
		if( dfa->HasTagStartStateAtSpecificState(next, (*i)) ) { continue; }
		
		//edw mono an den uparxei to bazoume kai tote to kanoume add
		std::list<TagNumber>::iterator toInsert = std::find(activeTags.begin(), activeTags.end(), (*i));
		if( toInsert == activeTags.end() ) { 
			activeTags.push_back((*i));
			block.tags[(*i)].clear();
			block.tags[(*i)].append(str);
		}
	}
	return;
}

//-----------------------------------------------------------------------

void DFARunner::DeactivateTags (
		const State cur, 
		const State next, 
		Block & block, 
		const std::string & str
	) {
	const DFA::TagList & list = dfa->GetTagsEndStates(cur);

	//diatrexoume olh thn lista me ta tag pou exoun telikes katastaseis se auth thn state tou automa tou
	for(DFA::TagList::const_iterator i = list.begin(); i != list.end(); ++i ){
		//An sthn state pou prokite na pame einai telikh state tou eidiou tag kanoume apend
		if( dfa->HasTagEndStateAtSpecificState(next, (*i)))
			block.tags[(*i)].append(str);

		//An kanpoio tag h katastash tou einai energh kai einai active (to tag) to diagrafoume
		std::list<TagNumber>::iterator toDelete = std::find(activeTags.begin(), activeTags.end(), (*i));		
		if( toDelete != activeTags.end() ) { activeTags.erase( toDelete );  }
	}//end of for
}

//-----------------------------------------------------------------------

void DFARunner::UpdateActiveTags (Block & block, std::string str){
	std::list<TagNumber>::iterator i = activeTags.begin();
	for(; i != activeTags.end(); ++i)
		block.tags[(*i)].append(str);
	return;
}



//******************************************************************************************
//		public functions

//-----------------------------------------------------------------------

void DFARunner::SetFindSetting (bool _isCaseSensitive, bool _wholeWord){
	wholeWord		= _wholeWord;
	isCaseSensitive = _isCaseSensitive;
	return;
}

//-----------------------------------------------------------------------

bool DFARunner::RunAutomaton (DFA * _dfa, TextSource& s, Block & block) {
	assert(_dfa);
	
	MARK();
	dfa						= _dfa;
	char inputChac			= '\0';
	unsigned charCnt		= 0;		//Metraei tous xaraktires pou kanoume match
	bool result				= false;
	block.start				= s.GetPos();
	Automaton::State curr	= dfa->GetDFAStartState();
	Clear();

	while( !s.IsEof() ){
		DFA::Transitions * trans = dfa->GetTransitions(curr);
		DFA::Transitions::iterator mapIte;

		if ( (mapIte = trans->find ( MatchPolicy(string_cast(inputChac = s.GetCurr()))) ) != trans->end() ){
			//Eixame bre8ei se telikh katastash kai 8a pame kapou pou den einai telikh katastash.
			if (result && !dfa->IsDFAEndState(mapIte->second)) 
				break;

			if( dfa->HasTags() )
				ManageTags(curr, mapIte->second, block, string_cast(inputChac));

			//Ftasame se telikh katastash. Kratame thn 8esh ths kai sunexizoume na katanalonoume
			//Isodo. Auto ginete gia to logo oti ena automato mporei na mhn stamataei sthn telikh 
			//katastash pou einai, alla na prpei na parei kai alh isodo
			if( dfa->IsDFAEndState(curr = mapIte->second) ){
				result		= true;								//Shmiwnoume oti brikame kati
				block.end	= s.GetPos();						//Kai se poia 8esh to brikame
			}
		}// end of if
		else { break; }		//Den ekane match to symbolo isodou ara den uparxei transition ara stop
	}//end of while

	if ( result && wholeWord ) { result = IsWholeWord(s, marker, charCnt); }

	UNMARK();
}

}	//namespace frep
