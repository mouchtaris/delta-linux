#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif


#include <assert.h>
#include "Dimension.h"
#include "Rectangle.h"
#include "SlotFinder.h"
#include "SlotRenderingInfo.h"
#include "LayerRenderingInfo.h"
#include "HighlightCurrentVertex.h"




namespace iviews {

//-----------------------------------------------------------------------

HighlightCurrentVertex::HighlightCurrentVertex(
	const Colour & vertexColour_,
	const Colour & slotColour_
) :	AddOn			(),
	oldVertex		((Vertex *)0),
	oldSlot			((SlotRenderingInfo *)0),
	slotColour		(slotColour_),
	vertexColour	(vertexColour_)
{}


//-----------------------------------------------------------------------

ColourAddOnUndoData * HighlightCurrentVertex::Apply(
		LayersRenderingInfo *	layers, 
		ColourAddOnData *		data
	){
	assert(layers && data && data->GetVertex());
	
	VertexRenderingInfo	* vertex = layers->GetVertexRenderingInfo(data->GetVertex());	
	oldVertexRenderingInfo = vertex;
	if (!vertex)
		return (HighlightRelativeSlotsUndoData *)0;

							 
	oldVertex									= data->GetVertex();
	HighlightCurrentVertexUndoData  * undoData	= new HighlightCurrentVertexUndoData();
	undoData->source							= oldVertex;
	undoData->mainRectColour					= vertex->GetBackgroundColour();
	SlotRenderingInfoPtrList & slots			= vertex->GetAllSlostsRenderingInfo();

	for (SlotRenderingInfoPtrList::iterator slot = slots.begin(); slot != slots.end(); ++slot) {
		undoData->slotsColours.push_back(
			SlotDebugInfoUndoRenderingData(
				(*slot)->GetSlotDebugInfo()->GetIdRelevantToDebugInfo(),
				(*slot)->GetKeyRectangle()->GetBagroundColour(),
				(*slot)->GetContentRectangle()->GetBagroundColour()
			)
		);
	}
	vertex->SetBackgroundColour(vertexColour);
	
	if (vertex->HasFullContents() && !vertex->HasHiddenContents())
		HighlightCurrentSlot(vertex, dynamic_cast<HighlightCurrentVertexAddOnData *>(data));
	return undoData;
}

//-----------------------------------------------------------------------

void HighlightCurrentVertex::Undo(LayersRenderingInfo * layers, ColourAddOnUndoData * undoData) {
	assert(layers);

	if (!undoData)
		return;
	
	HighlightCurrentVertexUndoData * undo	= dynamic_cast<HighlightCurrentVertexUndoData *>(undoData);
	assert(undo && undo->source);

	VertexRenderingInfo * vertex = layers->GetVertexRenderingInfo(undo->source);		

	if (vertex) {
		vertex->GetVertexMainRectangle()->SetBagroundColour(undo->mainRectColour);
	
		SlotDebugInfoUndoRenderingDataList::iterator i;
		for(i = undo->slotsColours.begin(); i != undo->slotsColours.end(); ++i) 
			if (SlotRenderingInfo * slot = vertex->GetSlotRenderingInfo((*i).id)) {
				slot->GetKeyRectangle()->SetBagroundColour((*i).keyColour);
				slot->GetContentRectangle()->SetBagroundColour((*i).contentColour);
			}
	}
}

//-----------------------------------------------------------------------

bool HighlightCurrentVertex::CanAddInManager(const ColourAddOnData * data) {
	assert(data);						
	Vertex * recenVertext = data->GetVertex();

	//eimatse akoma ston xoro pou pianei to absolute reference
	if (oldVertex && recenVertext && recenVertext == oldVertex && oldVertexRenderingInfo) {	
		Rectangle rect(oldVertexRenderingInfo->GetPosition(), oldVertexRenderingInfo->GetStartVertexMainDimension());
		HighlightCurrentVertexAddOnData * data_ = dynamic_cast<HighlightCurrentVertexAddOnData *>(const_cast<ColourAddOnData *>(data));
		if (Rectangle::InBoundaries(&rect, data_->GetX(), data_->GetY()) && !oldSlot)
			return false;
	}


	if ((recenVertext && !oldVertex) || (recenVertext && oldVertex && (recenVertext != oldVertex)))						
		return true;	

	if (recenVertext && oldVertex && (recenVertext == oldVertex) && !oldSlot)
		return true;
		
	return false;

}

//-----------------------------------------------------------------------
#define IN_BOUNDARIES(RECT)							\
	Rectangle::InBoundaries(oldSlot->RECT(), x, y)

bool HighlightCurrentVertex::CanRemoveFromManager(const ColourAddOnData * data) {
	assert(data);						
	Vertex * recenVertext = data->GetVertex();
	
	
	if (!oldVertex && recenVertext)	//prhn den eimastan se vertex eno twra eimatse
		return false;

	if ((!recenVertext && oldVertex) ||								//prin eimastan se vertex eno twra den eimatse
		(recenVertext && oldVertex && (recenVertext != oldVertex))) //h prohgoumenh vertex me thn torinh einai diaforetikes						
		return true;	

	//eimatse akoma ston xoro pou pianei to absolute reference
	if (oldVertex && recenVertext && recenVertext == oldVertex && oldVertexRenderingInfo) {	
		Rectangle rect(oldVertexRenderingInfo->GetPosition(), oldVertexRenderingInfo->GetStartVertexMainDimension());
		HighlightCurrentVertexAddOnData * data_ = dynamic_cast<HighlightCurrentVertexAddOnData *>(const_cast<ColourAddOnData *>(data));
		if (Rectangle::InBoundaries(&rect, data_->GetX(), data_->GetY()) && !oldSlot)
			return false;
	}

	if (recenVertext && oldVertex && (recenVertext == oldVertex)) {
		HighlightCurrentVertexAddOnData * data_ = dynamic_cast<HighlightCurrentVertexAddOnData *>(const_cast<ColourAddOnData *>(data));
		coord_t x = data_->GetX();
		coord_t y = data_->GetY();
		if (!oldSlot)	//eimaste se slot eno prhn den eimastan
			return true;

		//den briskomaste plewn sto palio slot.
		if (oldSlot && !IN_BOUNDARIES(GetKeyRectangle) && !IN_BOUNDARIES(GetContentRectangle))
			return true;
	}
	return false;

}

//-----------------------------------------------------------------------

void HighlightCurrentVertex::RemovedFromManager(void) { 
	oldSlot		= (SlotRenderingInfo*)0;
	oldVertex	= (Vertex *)0; 
}

//-----------------------------------------------------------------------

void HighlightCurrentVertex::SetVertexColour(const Colour & c)
	{ vertexColour = c; }

//-----------------------------------------------------------------------

const HighlightCurrentVertex::Colour & HighlightCurrentVertex::GetVertexColour(void) const
	{ return vertexColour; }

//-----------------------------------------------------------------------

void HighlightCurrentVertex::SetSlotColour(const Colour & c)
	{ slotColour = c; }

//-----------------------------------------------------------------------

const HighlightCurrentVertex::Colour & HighlightCurrentVertex::GetSlotColour(void) const 
	{ return slotColour; }	

//////////////////////////////////////////////////////////////////////////////
//	Private method API

//-----------------------------------------------------------------------

void HighlightCurrentVertex::HighlightCurrentSlot(
		VertexRenderingInfo				* vertex, 
		HighlightCurrentVertexAddOnData * data
	) {
	assert(vertex && data);

	oldSlot = SlotFinder()(vertex, data->GetX(), data->GetY());
	if (oldSlot) {	//mouse may points in main rect and not in a slot 		
		oldSlot->GetKeyRectangle()->SetBagroundColour(slotColour);
		oldSlot->GetContentRectangle()->SetBagroundColour(slotColour);
	}
}

}	//namespace iviews