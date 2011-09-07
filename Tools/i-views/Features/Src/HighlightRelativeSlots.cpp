/**
 *	HighlightCurrentVertex.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	July 2010
 */

#include "VertexRenderingInfo.h"
#include "HighlightRelativeSlots.h"
#include "SetEdgeOriginSlotColour.h"
#include "RenderingInfoAlgorithmsMacros.h"

namespace iviews {

//-----------------------------------------------------------------------

HighlightRelativeSlots::HighlightRelativeSlots(const Colour & c)
:	AddOn(),
	slotsColour(c)
{}

//-----------------------------------------------------------------------

//if the return value is not null then caller is responsible to delete the pointer
ColourAddOnUndoData * HighlightRelativeSlots::Apply(
	LayersRenderingInfo * layers, 
	ColourAddOnData * data
	) {

	assert(layers && data && data->GetVertex());

	VertexRenderingInfo * vertex = layers->GetVertexRenderingInfo(data->GetVertex());
	if(!vertex)
		return (HighlightRelativeSlotsUndoData *)0;

	HighlightRelativeSlotsUndoData	* undoData	= (HighlightRelativeSlotsUndoData *)0;

	FOR_ALL_EDGES(edge, vertex->GetAllIncomingEdgesRenderingInfo()) {
		VertexRenderingInfo * source = (*edge)->GetSourceVertexRenderingInfo();
		assert(source);

		if (source->HasFullContents() && !source->HasHiddenContents()) {
			if (!undoData)
				undoData = new HighlightRelativeSlotsUndoData();
			
			undoData->undoDataList.push_back(
				std::make_pair(
					source->GetSourceVertex(),
					SetEdgeOriginSlotColour()(*edge, slotsColour)
				)
			);
		}		
	}
	return undoData;
}

//-----------------------------------------------------------------------

void HighlightRelativeSlots::Undo(LayersRenderingInfo * layers, ColourAddOnUndoData * undoData) {
	assert(layers);

	if (!undoData)
		return;

	VertexRenderingInfo				* vertex	= (VertexRenderingInfo *)0;
	LayerRenderingInfo				* layer		= (LayerRenderingInfo *)0;
	HighlightRelativeSlotsUndoData	* undo		= dynamic_cast<HighlightRelativeSlotsUndoData *>(undoData);
	assert(undo);

	SlotDebugInfoUndoRenderingDataPairList::iterator i;
	for(i = undo->undoDataList.begin(); i != undo->undoDataList.end(); ++i){
		if (vertex = layers->GetVertexRenderingInfo(i->first)) {
			if (SlotRenderingInfo * slot = vertex->GetSlotRenderingInfo(i->second.id)) {
				slot->GetKeyRectangle()->SetBagroundColour(i->second.keyColour);
				slot->GetContentRectangle()->SetBagroundColour(i->second.contentColour);
			}
		}
	}
}

//-----------------------------------------------------------------------

void HighlightRelativeSlots::SetSlotsColour(const Colour & c)
	{ slotsColour = c; }

//-----------------------------------------------------------------------

const HighlightRelativeSlots::Colour & HighlightRelativeSlots::GetSlotsColour(void) const
	{ return slotsColour; }

//-----------------------------------------------------------------------

bool HighlightRelativeSlots::CanAddInManager(const ColourAddOnData * data) 
	{ return true; }

//-----------------------------------------------------------------------

void HighlightRelativeSlots::RemovedFromManager(void) 
	{}

//-----------------------------------------------------------------------

bool HighlightRelativeSlots::CanRemoveFromManager(const ColourAddOnData * data)
	{ return false; }





}	//namespace iviews