/**
 *	SetEdgeOriginSlotColour.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	July 2010
 */

#include <assert.h>

#include "Vertex.h"
#include "Rectangle.h"
#include "SlotRenderingInfo.h"
#include "VertexRenderingInfo.h"
#include "SetEdgeOriginSlotColour.h"

namespace iviews {

SlotDebugInfoUndoRenderingData  SetEdgeOriginSlotColour::operator()(
	EdgeRenderingInfo * edge, 
	const geo::Colour & c) {

	assert(
		edge															&& 
		edge->GetSourceVertexRenderingInfo()->HasFullContents()			&&
		!edge->GetSourceVertexRenderingInfo()->HasHiddenContents()
	);
	
	Rectangle			* rect			= (Rectangle *)0;
	SlotRenderingInfo	* sourceSlot	= edge->GetSourceSlotRenderingInfo();
	graphs::Vertex		* keyVertex		= sourceSlot->GetSlotDebugInfo()->GetKeyTargetVertex();
	graphs::Vertex		* contentVertex	= sourceSlot->GetSlotDebugInfo()->GetContentTargetVertex();
	graphs::Vertex		* targetVertex	= edge->GetTargetVertexRenderingInfo()->GetSourceVertex();

	geo::Colour oldKeyColour			= sourceSlot->GetKeyRectangle()->GetBagroundColour();
	geo::Colour oldContentColour		= sourceSlot->GetContentRectangle()->GetBagroundColour();

	if (keyVertex == targetVertex)
		rect = sourceSlot->GetKeyRectangle();
	else if(contentVertex == targetVertex) 
		rect = sourceSlot->GetContentRectangle();
	else 
		assert(0);
	
	rect->SetBagroundColour(c);

	return SlotDebugInfoUndoRenderingData(
				sourceSlot->GetSlotDebugInfo()->GetIdRelevantToDebugInfo(),
				oldKeyColour,
				oldContentColour
			);
}


}	//namespace iviews 