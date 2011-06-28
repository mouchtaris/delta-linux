/*
 * BasicStringReplace.h 
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#include "RegExpr.h"
#include "BasicStringMatching.h"



namespace frep {
#define MARK() unsigned marker = s.Mark()
#define UNMARK() s.Retract(marker); return result//s.Unmark(marker); return result


//-----------------------------------------------------------------------

BasicStringMatching::BasicStringMatching(void) 
	{ StringExpr::Initialise(); }

//-----------------------------------------------------------------------

void BasicStringMatching::SetError(const std::string str){
	this->hasError = true;
	this->errorMsg = str;
}

//-----------------------------------------------------------------------

void BasicStringMatching::Clear (void) {
	hasError = false;
	errorMsg.clear();
}

//-----------------------------------------------------------------------

void BasicStringMatching::SetFindSetting (bool isCaseSensitive, bool wholeWord){
	StringExpr::wholeWord		= wholeWord;
	StringExpr::SetSensitivity( isCaseSensitive );
}

//-----------------------------------------------------------------------

bool BasicStringMatching::Run (const std::string _str, TextSource& s, TextSource::Block & block){
	Clear();
	StringExpr str(_str);

	MARK();
	block.start	= s.GetPos();
	bool result = str.Match(s);
	if (result)
		block.file = &s;
	block.end	= s.GetPos();

	UNMARK();
	//return result;
}

}	//namespace frep