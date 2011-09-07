/**
 *	TipTimer.cpp
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	Sempteber 2010
 */

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif

#include "TipTimer.h"
#include "TypesConverter.h"


namespace iviews {

//-----------------------------------------------------------------------

TipTimer::TipTimer(void)
	:	text(),
		parent((wxWindow *)0),
		tipWindow((TipWindow *)0)
{}

//-----------------------------------------------------------------------

void TipTimer::Notify(void){
	tipWindow = new TipWindow(parent, std2str(text));
	tipWindow->SetBoundingRect(wxRect(wxGetMousePosition(), wxSize(10,10)));
}

//-----------------------------------------------------------------------

void TipTimer::SetTipPropeties(wxWindow * parent_, const std::string & text_) {
	text	= text_;
	parent	= parent_;
}


}	//namespace iviews 