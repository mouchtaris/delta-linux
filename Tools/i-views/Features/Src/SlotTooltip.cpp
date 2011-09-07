/**
 *	SlotTooltip.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	September 2010
 */

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif

#include <string>
//#include <wx/tipwin.h>

#include "SlotFinder.h"
#include "SlotTooltip.h"
#include "TypesConverter.h"
#include "SlotRenderingInfo.h"

#define TIP_TEXT									\
	slot->GetSlotDebugInfo()->GetKeyString()	+	\
	"\n"										+	\
	slot->GetSlotDebugInfo()->GetContentString()



namespace iviews {

SlotTooltip::SlotTooltip(long delay_) 
	:	delay(delay_),
		timer()
{}

//-----------------------------------------------------------------------

bool SlotTooltip::operator() (
		Window *					parent, 
		VertexRenderingInfo *		v, 
		const geo::Point<coord_t> & mouseOverVertexPos
	) {
	Stop();
	SlotRenderingInfo * slot = static_cast<SlotRenderingInfo *>(0);
	
	if (v						&& 
		parent					&&
		v->HasFullContents()	&&
		!v->HasHiddenContents()	&&
		(slot = SlotFinder()(v, mouseOverVertexPos.GetX(), mouseOverVertexPos.GetY())))
	{
		timer.SetTipPropeties(parent, TIP_TEXT);
		timer.Start(delay, true);
		return true;
	}
	return false;
}


void SlotTooltip::Stop(void) {
	if (timer.IsRunning())
		timer.Stop();
}

}	//namespace iviews