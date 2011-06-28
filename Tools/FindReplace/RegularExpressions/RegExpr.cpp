/*
 * RegExpr.cpp
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#include <ctype.h>
#include <assert.h>
#include <iostream>
#include <algorithm>

#include "RegExpr.h"
#include "RegExprTreeVisitor.h"

namespace frep {

#define MARK() unsigned marker = s.Mark()
#define FAIL() return false/*s.Retract(marker); return false*/
#define UNMARK()  return true/*s.Retract(marker); return true//s.Unmark(marker); return true*/


bool StringExpr::wholeWord;
bool StringExpr::isCaseSensitive;
bool (*StringExpr::MatchPolicy)(char, char);

std::map<unsigned ,std::string> TagExpr::allTags;

//**************************** WhiteSpaceExpr **************************************

//-----------------------------------------------------------------------

void WhiteSpaceExpr::Accept (RegExprTreeVisitor * visitor){
	assert(visitor);
	visitor->VisitWhiteSpace(this);
	return;
}

//**************************** QuotedStringExpr **************************************

//-----------------------------------------------------------------------

void QuotedStringExpr::Accept (RegExprTreeVisitor * visitor){
	assert(visitor);
	visitor->VisitQuotedString(this);
	return;
}

//**************************** QuotedStringExpr **************************************

//-----------------------------------------------------------------------

void IntegerExpr::Accept (RegExprTreeVisitor * visitor){
	assert(visitor);
	visitor->VisitIntegerExpr(this);
	return;
}


//**************************** DeltaIdentExpr **************************************

//-----------------------------------------------------------------------

void DeltaIdentExpr::Accept(RegExprTreeVisitor* visitor){
	assert(visitor);
	visitor->VisitDeltaIdent(this);
	return;
}

//**************************** AnyCharExpr **************************************

//-----------------------------------------------------------------------

void AnyCharExpr::Accept(RegExprTreeVisitor* visitor){
	assert(visitor);
	visitor->VisitAnyChar(this);
	return;
}


//**************************** StringExpr **************************************

//-----------------------------------------------------------------------

bool IsWholeWord(TextSource &s, unsigned marker, std::string &str) {
	char last = s.GetSpecificChar(marker+ str.length());
	
	if( (marker != 0) && (last != EOF) ) { 
		if (isalnum(s.GetSpecificChar(marker-1))) { //apo ta aristera uparxei alnum
			return false;
		}
		else if (isalnum(last) || last == '_')			//apo ta de3ia uparxei alnum
			return false;
		else
			return true;
	}
	else if ( (marker == 0) && (last == EOF) ){
		return true;			//single word in the file
	}
	else if ( marker == 0 ){	//First word in file
		if( isalnum(last) || last == '_' ) { return false; }
		else { return true; }
	}
	else{						//Last word in file
		if( isalnum(s.GetSpecificChar(marker-1)) ) { return false; }
		else { return true; }
	}//end of else
	return false;	
}	

//-----------------------------------------------------------------------

void StringExpr::Initialise (void){
	StringExpr::wholeWord			= false;
	StringExpr::isCaseSensitive		= false;
}

//-----------------------------------------------------------------------

bool StringExpr::MatchCaseSensitive (char c1, char c2){
	return c1 == c2;
}

//-----------------------------------------------------------------------

bool StringExpr::MatchCaseInsensitive (char c1, char c2){
	if( tolower(c1) == tolower(c2))
		return true;
	else
		return false;
}

//-----------------------------------------------------------------------

bool StringExpr::Match(TextSource &s){
	MARK(); //unsigned marker = s.Mark()
	std::string::iterator i;
	
	if ( text.begin() == text.end() ) { assert(0); }

	for (	i = text.begin(); i != text.end() && !s.IsEof() && (*MatchPolicy)(*i, s.GetCurr()); ++i	)
	{ ; }
	
	if (i == text.end()){ // Match text
		if( wholeWord ){
			if( IsWholeWord(s, marker, text) ) { UNMARK(); }
			else { FAIL(); }
		}
		else { UNMARK(); }
	}
	
	FAIL();
}

//-----------------------------------------------------------------------

void StringExpr::Accept(RegExprTreeVisitor* visitor){
	assert(visitor);
	visitor->VisitString(this);
	return;
}



//**************************** OrExpr **************************************

//-----------------------------------------------------------------------

void OrExpr::Accept(RegExprTreeVisitor* visitor) {
	assert(visitor);
	e1->Accept(visitor);
	e2->Accept(visitor);

	visitor->VisitOr(this);
	return;
}


//**************************** ConcatExpr **************************************

//-----------------------------------------------------------------------

void ConcatExpr::Accept(RegExprTreeVisitor* visitor){
	assert(visitor);
	e1->Accept(visitor);
	e2->Accept(visitor);

	visitor->VisitConcat(this);
	return;
}


//**************************** TagExpr **************************************

//-----------------------------------------------------------------------

void TagExpr::Accept(RegExprTreeVisitor* visitor){
	assert(visitor);
	e->Accept(visitor);
	visitor->VisitTag(this);
	return;
}




//**************************** ZeroOrMoreExpr **************************************

//-----------------------------------------------------------------------

void ZeroOrMoreExpr::Accept(RegExprTreeVisitor* visitor){
	assert(visitor);
	e->Accept(visitor);
	visitor->VisitZeroOrMore(this);
	return;
}



//**************************** OneOrMoreExpr **************************************

//-----------------------------------------------------------------------

void OneOrMoreExpr::Accept(RegExprTreeVisitor* visitor){
	assert(visitor);
	e->Accept(visitor);
	visitor->VisitOneOrMore(this);
	return;
}



//**************************** EpsilonExpr **************************************

//-----------------------------------------------------------------------

void EpsilonExpr::Accept(RegExprTreeVisitor* visitor){
	assert(visitor);
	visitor->VisitEpsilon(this);
	return;
}

}	//namespace frep




//**************************** NumberExpr **************************************
/*
//-----------------------------------------------------------------------
bool NumberExpr::Match(TextSource& s){

	if( num < TagExpr::tagsVec.size() ){
		RegExpr * regex = TagExpr::tagsVec[num];
		if( regex->Match(s) )
			l.append( regex->Lexeme() );
		else
			l = "";
	}
	else
		l = "";
	return true;
	
	return false;
}
*/

//**************************** WhiteSpaceExpr **************************************
/*
//-----------------------------------------------------------------------
bool WhiteSpaceExpr::Match(TextSource &s) {
	MARK();
	char c[2] = { ' ', '\0' };
	l.clear();

	while (!s.IsEof()) {	// (isspace)*
		c[0] = s.GetNext();
		if (isspace(c[0]) && !s.IsEof() )
			l.append(c);
		else
			break;
	}

	if ( !s.IsEof() ) { s.Unput(); } // Non-space returned back.
	if (l.compare("")!=0) { UNMARK(); } 
	FAIL();
}
*/


//**************************** ZeroOrMoreExpr **************************************
/*
//-----------------------------------------------------------------------
bool ZeroOrMoreExpr::Match(TextSource &s){
	while(e->Match(s)){
		l.append( e->Lexeme() );
	}
	return l.compare("") != 0;
}
*/

//**************************** TagExpr **************************************

//-----------------------------------------------------------------------
/*void TagExpr::Initialise (void){
	allTags.clear();
}*/


/*
//-----------------------------------------------------------------------
bool TagExpr::Match(TextSource &s){
	MARK(); //unsigned marker = s.Mark()

	if (e->Match(s)) {

		unsigned marker2 = s.Mark();
		for( unsigned i = marker; i <= marker2; i++ ){	
			l.append( 1, s.GetSpecificChar(i) ); 
		}
		
		s.Unmark(marker2);
		UNMARK();
	}
	FAIL();
}
*/

//**************************** ConcatExpr **************************************
/*
//-----------------------------------------------------------------------
bool ConcatExpr::Match(TextSource &s){
	MARK();
	l.clear();
	if (e1->Match(s)) {	
		l.append(e1->Lexeme());
		if (e2->Match(s)) { 
			l.append(e2->Lexeme()); 
			UNMARK(); 
		}//second if
	}//first if
	FAIL();
}
*/

//**************************** OrExpr **************************************
/*
//-----------------------------------------------------------------------
bool OrExpr::Match(TextSource &s){
	MARK();
	l.clear();
	
	if (e1->Match(s)) 
		{ l = e1->Lexeme(); UNMARK(); }
	
	else if (e2->Match(s)) 
		{ l = e2->Lexeme(); UNMARK(); }

	FAIL();
}
*/

//**************************** AnyCharExpr **************************************
/*
//-----------------------------------------------------------------------
bool AnyCharExpr::Match(TextSource &s) {
	char c[2] = { s.GetNext(), '\0' };
	
	if (!s.IsEof()){ 
		l = c; 
		return true; 
	}
	return false;
}
*/

//**************************** DeltaIdentExpr **************************************
/*
//-----------------------------------------------------------------------
bool DeltaIdentExpr::Match(TextSource &s){
	MARK();
	l.clear();
	char c[2] = { s.GetNext(), '\0' };

	if (!s.IsEof() && isalpha(c[0])) { // letter(letter|digit)*
		while (true) {
			l.append(c);
			c[0] = s.GetNext();

			if (!s.IsEof() && isalnum(c[0]))
				l.append(c);
			else
				break;
		}
		if (!s.IsEof()) s.Unput();
		if (l.compare("")!=0) UNMARK();
	}//end of while

	else { FAIL(); }
	return false;
}
*/

//**************************** WhiteSpaceExpr **************************************
/*
//-----------------------------------------------------------------------
bool WhiteSpaceExpr::Match(TextSource &s) {
	MARK();
	char c[2] = { ' ', '\0' };
	l.clear();

	while (!s.IsEof()) {	// (isspace)*
		c[0] = s.GetNext();
		if (isspace(c[0]) && !s.IsEof() )
			l.append(c);
		else
			break;
	}

	if ( !s.IsEof() ) { s.Unput(); } // Non-space returned back.
	if (l.compare("")!=0) { UNMARK(); } 
	FAIL();
}
*/
