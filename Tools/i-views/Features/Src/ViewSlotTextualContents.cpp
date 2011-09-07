/**
 *	ViewSlotTextualContents.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	July 2010
 */

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif

#include <assert.h>

#include "SlotFinder.h"
#include "TextualFrame.h"
#include "SlotDebugInfo.h"
#include "ViewSlotTextualContents.h"


#define BOOL_TO_STRING(BOOL)								\
	(BOOL ? wxString(_T("true")) : wxString(_T("false")))

namespace iviews {

//-----------------------------------------------------------------------

ViewSlotTextualContents::ViewSlotTextualContents(void) {}

//-----------------------------------------------------------------------

SlotRenderingInfo * ViewSlotTextualContents::operator()(
		Window *				parent,
		VertexRenderingInfo*	v, 
		const coord_t &			x, 
		const coord_t &			y
	) {
	assert(parent);
	assert(v);

	SlotRenderingInfo * slotRenInfo = static_cast<SlotRenderingInfo *>(0);

	if(slotRenInfo = SlotFinder()(v, x, y)) {
		wxString nl							= _T("\n\n");
		dinfo::SlotDebugInfo * slotDInfo	= slotRenInfo->GetSlotDebugInfo();
		
		assert(slotDInfo);

		TextualFrame * frame = new TextualFrame(parent);
		//frame->AppendText(_T("Is Key Aggregate: ") + BOOL_TO_STRING(slotDInfo->IsKeyAggregate()) + nl);
		frame->AppendText(_T("Key String: ") + std2str(slotDInfo->GetKeyString()) + nl);
		//frame->AppendText(_T("Key String type: ") + std2str(slotDInfo->GetStringKeyTypeInfo()) + nl);
		//frame->AppendText(_T("Is Content Aggregate: ") + BOOL_TO_STRING(slotDInfo->IsContentAggregate()) + nl);
		frame->AppendText(_T("Content String: ") + std2str(slotDInfo->GetContentString()) + nl);
		frame->Show(true);
	}
	else
		assert(0);

	return slotRenInfo;
}

}	//namespace iviews