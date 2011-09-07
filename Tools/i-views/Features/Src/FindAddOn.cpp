/**
 *	FindAddOn.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	Octomber 2010
 */

#include <iterator>
#include "FindAddOn.h"


namespace iviews {

//-----------------------------------------------------------------------

FindAddOn::FindAddOn(const geo::Colour & c)
:	selectionColour(c)
{}

//-----------------------------------------------------------------------

geo::Colour FindAddOn::GetSelectionColour(void)
	{ return selectionColour; }

//-----------------------------------------------------------------------

void FindAddOn::SetSelectionColour(const geo::Colour & c)
	{ selectionColour = c; }

//-----------------------------------------------------------------------

ColourAddOnUndoData * FindAddOn::Apply(
		LayersRenderingInfo * layers, 
		ColourAddOnData * data
	) { 
	assert(layers && data && dynamic_cast<FindAddOnData *>(data));

	FindAddOnData		* findAddOnData = dynamic_cast<FindAddOnData *>(data);
	VertexRenderingInfo * vertex		= layers->GetVertexRenderingInfo(data->GetVertex());
	
	if (!vertex || !findAddOnData)
		return (FindUndoData *)0; 

	if (!vertex->HasFullContents() || vertex->HasHiddenContents()) 
		return ApplyInVertexWithNoContents(findAddOnData, vertex);
	else
		return ApplyInVertexWithFullContents(findAddOnData, vertex);
}

//-----------------------------------------------------------------------

void FindAddOn::Undo(LayersRenderingInfo * layers, ColourAddOnUndoData * undoData) {
	assert(layers && undoData && dynamic_cast<FindUndoData *>(undoData));	

	FindUndoData		* findUndoData	= dynamic_cast<FindUndoData *>(undoData);
	VertexRenderingInfo * vertex		= layers->GetVertexRenderingInfo(findUndoData->source);

	if (vertex)
		if (findUndoData->isAbsolutRef										||
			(!findUndoData->hasFullContents && !vertex->HasFullContents()&& vertex->HasHiddenContents())	||
			(!findUndoData->hasFullContents && vertex->HasFullContents() && !vertex->HasHiddenContents()) )
			vertex->SetAbosloutRefBackgroundColour(findUndoData->oldColour);
		else	//kanonika einai key h' content, alla den exoume fullContets
		if (!findUndoData->isAbsolutRef		&& 
			!findUndoData->hasFullContents	&& 
			!vertex->HasFullContents())
			vertex->SetAbosloutRefBackgroundColour(findUndoData->oldColour);
		else	//kanonika einai key h' content, alla den exoume fullContets
		if (!findUndoData->isAbsolutRef		&& 
			!findUndoData->hasFullContents	&& 
			vertex->HasFullContents()		&&
			vertex->HasHiddenContents())
			vertex->SetAbosloutRefBackgroundColour(findUndoData->oldColour);
		else 
		if (!findUndoData->isAbsolutRef		&& 
			findUndoData->hasFullContents	&& 
			vertex->HasFullContents()		&&
			!vertex->HasHiddenContents())
			SetSlotOldColour(findUndoData, vertex);
}

//-----------------------------------------------------------------------

void FindAddOn::RemovedFromManager(void) 
	{}

//-----------------------------------------------------------------------

bool FindAddOn::CanAddInManager(const ColourAddOnData * data) 
	{ return true; }

//-----------------------------------------------------------------------

bool FindAddOn::CanRemoveFromManager(const ColourAddOnData * data) 
	{ return true; }






//**************************************
// private method API implementation

SlotRenderingInfo * FindAddOn::GetNthSlotRenderingInfoFromList(
		SlotRenderingInfoPtrList &	slots,
		const int					distance
	){
	assert(distance >= 0 && distance < slots.size());

	SlotRenderingInfoPtrList::iterator i = slots.begin();
	std::advance(i, distance);

	return *i;
}

//-----------------------------------------------------------------------

FindUndoData * FindAddOn::ApplyInVertexWithNoContents(
		FindAddOnData		* findAddOnData,
		VertexRenderingInfo * vertex
	) {
	assert(findAddOnData && vertex);

	geo::Colour absRefColour = vertex->GetBackgroundColour();
	vertex->SetAbosloutRefBackgroundColour(selectionColour);

	return new FindUndoData(
					vertex->GetSourceVertex(),
					absRefColour, 
					false, 
					findAddOnData->GetSlotIndex() == -1,
					false, 
					findAddOnData->GetSlotIndex() == -1 ? "" : " "
				);
}

//-----------------------------------------------------------------------

FindUndoData * FindAddOn::ApplyInVertexWithFullContents(
			FindAddOnData		* findAddOnData,
			VertexRenderingInfo * vertex
	) {
	assert(findAddOnData && vertex);

	if (findAddOnData->GetSlotIndex() == -1)
		return ApplyInVertexWithNoContents(findAddOnData, vertex);


	SlotRenderingInfo * slot = GetNthSlotRenderingInfoFromList(
									vertex->GetAllSlostsRenderingInfo(),
									findAddOnData->GetSlotIndex()
								);
	geo::Colour slotOldColour; 

	if (findAddOnData->IsKey()) {
		slotOldColour = slot->GetKeyRectangle()->GetBagroundColour();
		slot->GetKeyRectangle()->SetBagroundColour(selectionColour);
	}
	else {
		slotOldColour = slot->GetContentRectangle()->GetBagroundColour();
		slot->GetContentRectangle()->SetBagroundColour(selectionColour);
	}

	return new FindUndoData(
					vertex->GetSourceVertex(),
					slotOldColour,
					true,
					false,
					findAddOnData->IsKey(),
					slot->GetSlotDebugInfo()->GetIdRelevantToDebugInfo()
				);
}

//-----------------------------------------------------------------------

void FindAddOn::SetSlotOldColour(
		FindUndoData		* findUndoData,
		VertexRenderingInfo * vertex
	){
	assert(findUndoData  && vertex);

	SlotRenderingInfo * slot = vertex->GetSlotRenderingInfo(findUndoData->id);
	assert(slot);
	
	if (findUndoData->isKey)
		slot->GetKeyRectangle()->SetBagroundColour(findUndoData->oldColour);
	else
		slot->GetContentRectangle()->SetBagroundColour(findUndoData->oldColour);
}



}	//namespace iviews 