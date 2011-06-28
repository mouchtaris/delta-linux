#include "VertexRenderingInfo.h"

#include <cmath>
#include <utility>
#include <assert.h>
#include <algorithm>

#include "Dimension.h"
#include "Functional.h"
#include "RenderingInfoAlgorithmsMacros.h"


#include "arrow.xpm"


namespace iviews{

#define CALC_AND_SET_NEW_POS(OBJ_POS, SET_POS)											\
	if (true) {																			\
		width	= (length_t)std::fabs ((long double)(OBJ_POS.GetX() - oldPos.GetX()));	\
		height	= (length_t)std::fabs ((long double)(OBJ_POS.GetY() - oldPos.GetY()));	\
		SET_POS(newPos.GetX() + width, newPos.GetY() + height);							\
	} else

//-----------------------------------------------------------------------

VertexRenderingInfo::VertexRenderingInfo (
	const id_t &		id_, 
	const Layer			sourceLayer_,	
	Text*				absRefText_,
	Rectangle*			mainRect_,
	graphs::Vertex* 	source_
) :	id					(id_),
	sourceLayer			(sourceLayer_),
	absRefText			(absRefText_),
	mainRect			(mainRect_),
	source				(source_),
	hasFullContents		(false),
	hasBookmark			(false),
	hasSpliceArrow		(false),
	hasEdgesCommonStart	(false),
	hasHiddenContents	(false),
	bookmark			()
{
	assert(source && absRefText && mainRect);
	assert(!absRefText->GetText().empty());
	assert(mainRect->GetHeight() && mainRect->GetWidth());
	startVertexMainDimension = mainRect->GetDimension();
	AddPrimitive(mainRect);
	AddPrimitive(absRefText);
}

//-----------------------------------------------------------------------

VertexRenderingInfo::~VertexRenderingInfo() {
	
	if (absRefText) {
		delete absRefText;
		absRefText = static_cast<Text *>(0);
	}
	
	if (mainRect) {
		delete mainRect;
		mainRect = static_cast<Rectangle *>(0);
	}

	if (bookmark) {
		delete bookmark;
		bookmark = static_cast<BookmarkRenderingInfo *>(0);
	}

	std::for_each (	
		slotsRenInfo.begin(), 
		slotsRenInfo.end(), 
		utility::DeletePointersFromList<SlotRenderingInfo>()
	);

	slotsRenInfo.clear();
}

//-----------------------------------------------------------------------

VertexRenderingInfo::Layer VertexRenderingInfo::GetSourceLayer (void) const
	{ return sourceLayer; }

//-----------------------------------------------------------------------

void VertexRenderingInfo::SetBackgroundColour (const Colour & c) {
	mainRect->SetBagroundColour(c);

	SlotRenderingInfoPtrList::iterator i;
	for(i = slotsRenInfo.begin(); i != slotsRenInfo.end(); ++i){
		(*i)->GetKeyRectangle()->SetBagroundColour(c);
		(*i)->GetContentRectangle()->SetBagroundColour(c);
	}
}

//-----------------------------------------------------------------------

void VertexRenderingInfo::SetAbosloutRefBackgroundColour (const Colour & c) 
	{ mainRect->SetBagroundColour(c); }

//-----------------------------------------------------------------------

void VertexRenderingInfo::AddSlot (dinfo::SlotDebugInfo * sdi, SlotRenderingInfo * sri){

	assert(sdi && sri);
	assert(mainRect->GetWidth() ==	sri->GetKeyRectangle()->GetWidth()		+ 
									sri->GetContentRectangle()->GetWidth()
			);

	hasFullContents = true;
	slotsRenInfo.push_back(sri);
	AddPrimitive(sri->GetKeyRectangle());
	AddPrimitive(sri->GetKeyText());
	
	if (sri->GetKeyExpandButtonRectangle())
		AddPrimitive(sri->GetKeyExpandButtonRectangle());
	
	AddPrimitive(sri->GetContentRectangle());
	AddPrimitive(sri->GetContentText());
	
	if (sri->GetContentExpandButtonRectangle())
		AddPrimitive(sri->GetContentExpandButtonRectangle());
	slots[sdi] = sri;

	//equivalent to take sri->GetContentRectangle()->GetHeight();
	mainRect->SetHeight(mainRect->GetHeight() + sri->GetKeyRectangle()->GetHeight());	 
}

//-----------------------------------------------------------------------

void VertexRenderingInfo::RemoveAllTheSlots (void) {
	hasSpliceArrow		= false;
	hasFullContents		= false;
	hasEdgesCommonStart	= false;
	hasHiddenContents	= false;

	slots.clear();
	primitives.clear();
	
	AddPrimitive(mainRect);
	AddPrimitive(absRefText);

	SlotRenderingInfoPtrList::iterator slot;
	for(slot = slotsRenInfo.begin(); slot != slotsRenInfo.end(); ++slot)
		DeleteSlot(*slot);

	slotsRenInfo.clear();
	
	std::for_each (		//for all edges, set slot rendering info to null
		outEdges.begin(),																		
		outEdges.end(),																			
		std::bind2nd (																		
			std::mem_fun (&EdgeRenderingInfo::SetSoruceSlotRenderingInfo),					
			(SlotRenderingInfo *)0															
		)																					
	);
}

//-----------------------------------------------------------------------

SlotRenderingInfo * VertexRenderingInfo::GetSlotRenderingInfo (const id_t & slotId) {
	for (Slots::iterator slot = slots.begin(); slot != slots.end(); ++slot)
		if (slot->first->Id() == slotId)
			return slot->second;

	return static_cast<SlotRenderingInfo *>(0);	
}

//-----------------------------------------------------------------------

SlotRenderingInfo * VertexRenderingInfo::GetSlotRenderingInfo (const std::string & slotId) {
	for (Slots::iterator slot = slots.begin(); slot != slots.end(); ++slot)
		if (slot->first->GetIdRelevantToDebugInfo() == slotId)
			return slot->second;

	return static_cast<SlotRenderingInfo *>(0);	
}

//-----------------------------------------------------------------------

SlotRenderingInfo * VertexRenderingInfo::GetSlotRenderingInfo (dinfo::SlotDebugInfo * sdi) {
	assert(sdi);
	Slots::iterator i = slots.find(sdi);
	if ( i != slots.end() )
		return i->second;
	return static_cast<SlotRenderingInfo *>(0);
}

//-----------------------------------------------------------------------

void VertexRenderingInfo::AddInEdgeRenderingInfo (EdgeRenderingInfo * in) 
	{ AddEdgeInList(in, inEdges); }

//-----------------------------------------------------------------------

void VertexRenderingInfo::AddOutEdgeRenderingInfo (EdgeRenderingInfo * out) 
	{ AddEdgeInList(out, outEdges); }

//-----------------------------------------------------------------------

/*
We use the screen coordinate system.
The axis’ begin is the down left corner of the screen.
X axis increases from left to right 
Y axis increases from top to bottom
*/
void VertexRenderingInfo::SetPosition (const CoordTPoint & newPos) {
	length_t width		= 0;
	length_t height		= 0;
	CoordTPoint oldPos	= mainRect->GetPosition();

	CALC_AND_SET_NEW_POS(absRefText->GetTextPosition(), absRefText->SetTextPosition);

	for(Slots::iterator i = slots.begin(); i != slots.end(); ++i) {
		SlotRenderingInfo * slot = i->second;

		CALC_AND_SET_NEW_POS(slot->GetKeyText()->GetTextPosition(), slot->GetKeyText()->SetTextPosition);
		CALC_AND_SET_NEW_POS(slot->GetContentText()->GetTextPosition(), slot->GetContentText()->SetTextPosition);
		CALC_AND_SET_NEW_POS(slot->GetKeyRectangle()->GetPosition(), slot->GetKeyRectangle()->SetPosition);
		CALC_AND_SET_NEW_POS(slot->GetContentRectangle()->GetPosition(), slot->GetContentRectangle()->SetPosition);
		if (slot->GetKeyExpandButtonRectangle()) 
			CALC_AND_SET_NEW_POS(slot->GetKeyExpandButtonRectangle()->GetPosition(), slot->GetKeyExpandButtonRectangle()->SetPosition); 
		if (slot->GetContentExpandButtonRectangle())
			CALC_AND_SET_NEW_POS(slot->GetContentExpandButtonRectangle()->GetPosition(), slot->GetContentExpandButtonRectangle()->SetPosition);
	}
	
	FOR_ALL_EDGES(edge, inEdges)
		CALC_AND_SET_NEW_POS((*edge)->GetArrow()->GetHead(), (*edge)->GetArrow()->SetHead);

	FOR_ALL_EDGES(edge, outEdges)
		CALC_AND_SET_NEW_POS((*edge)->GetArrow()->GetTail(), (*edge)->GetArrow()->SetTail);

	if (HasBookmark())	//Update bookmark pos if it has.
		bookmark->SetPosition(
			newPos.GetX(),
			newPos.GetY() - bookmark->GetHeight()
		);
	mainRect->SetPosition(newPos);
}

//-----------------------------------------------------------------------

id_t VertexRenderingInfo::Id () const 
	{ return id; }

//-----------------------------------------------------------------------

bool VertexRenderingInfo::HasSpliceArrow (void) const
	{ return hasSpliceArrow; }

length_t VertexRenderingInfo::CalcMaxMinorRadius (void) const {
	assert(HasSpliceArrow());
	length_t maxMinorRadius = 0;

	const EdgeRenderingInfoPtrList & edges = GetAllOutgoingEdgesRenderingInfo();
	EdgeRenderingInfoPtrList::const_iterator i;
	for (i = edges.begin(); i != edges.end(); ++i)
		if ((*i)->IsInnerEdge()) {
		//if ((*i)->GetArrow()->Type() == Primitive::SPLICE_ARROW) {
			length_t mRadius = ((SpliceArrow *)(*i)->GetArrow())->GetMinorRadius();
			if (mRadius > maxMinorRadius)
				maxMinorRadius = mRadius;
		}

	return maxMinorRadius;
}

//-----------------------------------------------------------------------

bool VertexRenderingInfo::HasEdgesCommonStart (void) const 
	{ return hasEdgesCommonStart; }

void VertexRenderingInfo::SetEdgesCommonStart (bool b) {
	if (!b || (b && HasFullContents() && !HasHiddenContents()))
		hasEdgesCommonStart = b;
	else
		assert(0);
}

//-----------------------------------------------------------------------

bool VertexRenderingInfo::HasHiddenContents (void) const 
	{ return hasHiddenContents; }

void VertexRenderingInfo::HideContents (bool hide) { 
	if (hasHiddenContents == hide)
		return;
	
	hasHiddenContents = hide; 
	SlotRenderingInfoPtrList::iterator slot;
	for(slot = slotsRenInfo.begin(); slot != slotsRenInfo.end(); ++slot)
		(*slot)->Hide(hide);

	if (hide)
		mainRect->SetDimension(startVertexMainDimension);
	else {
		Rectangle * key = slotsRenInfo.back()->GetKeyRectangle();
		mainRect->SetHeight (
			(key->GetY() - mainRect->GetY()) +
			key->GetHeight()
		);
	}
}

//-----------------------------------------------------------------------

bool VertexRenderingInfo::HasFullContents (void) const
	{ return hasFullContents; }

void VertexRenderingInfo::FullContents (bool b) 
	{hasFullContents = b; }

//-----------------------------------------------------------------------

const VertexRenderingInfo::Colour & VertexRenderingInfo::GetBackgroundColour (void) const 
	{ return mainRect->GetBagroundColour(); }

//-----------------------------------------------------------------------

const VertexRenderingInfo::Slots & VertexRenderingInfo::GetAllTheSlots (void) const 
	{ return slots; }

//-----------------------------------------------------------------------

const VertexRenderingInfo::SlotRenderingInfoPtrList & VertexRenderingInfo::GetAllSlostsRenderingInfo (void) const 
	{ return slotsRenInfo; }

//-----------------------------------------------------------------------

VertexRenderingInfo::SlotRenderingInfoPtrList & VertexRenderingInfo::GetAllSlostsRenderingInfo (void) 
	{ return slotsRenInfo; }

//-----------------------------------------------------------------------

void VertexRenderingInfo::SetVertexMainRectangle (Rectangle * rect) 
	{ mainRect = rect; }

//-----------------------------------------------------------------------

Rectangle *	VertexRenderingInfo::GetVertexMainRectangle (void) const 
	{ return mainRect; }

//-----------------------------------------------------------------------

graphs::Vertex * VertexRenderingInfo::GetSourceVertex (void) const 
	{ return source; }

//-----------------------------------------------------------------------

void VertexRenderingInfo::SetPosition (const coord_t & x, const coord_t & y) 
	{ SetPosition(CoordTPoint(x, y)); }

//-----------------------------------------------------------------------

const VertexRenderingInfo::CoordTPoint & VertexRenderingInfo::GetPosition (void) const	
	{ return mainRect->GetPosition(); }

//-----------------------------------------------------------------------

length_t VertexRenderingInfo::GetWidth (void) const 
	{ return mainRect->GetWidth(); }

//-----------------------------------------------------------------------

length_t VertexRenderingInfo::GetHeight (void) const 
	{ return mainRect->GetHeight(); }

//-----------------------------------------------------------------------

const geo::Dimension<length_t> & VertexRenderingInfo::GetStartVertexMainDimension (void) const
	{ return startVertexMainDimension; }

//-----------------------------------------------------------------------

const geo::Dimension<length_t> & VertexRenderingInfo::GetVertexMainDimension (void) const 
	{ return mainRect->GetDimension(); }

//-----------------------------------------------------------------------

coord_t	VertexRenderingInfo::GetX (void) const 
	{ return mainRect->GetX(); }

//-----------------------------------------------------------------------

coord_t	VertexRenderingInfo::GetY (void) const 
	{ return mainRect->GetY(); }

//-----------------------------------------------------------------------

Text * VertexRenderingInfo::GetAbsRefText (void) const 
	{ return absRefText; }

//-----------------------------------------------------------------------

const VertexRenderingInfo::EdgeRenderingInfoPtrList & VertexRenderingInfo::GetAllIncomingEdgesRenderingInfo (void) const
	{ return inEdges; }

//-----------------------------------------------------------------------

VertexRenderingInfo::EdgeRenderingInfoPtrList & VertexRenderingInfo::GetAllIncomingEdgesRenderingInfo (void)
	{ return inEdges; }

//-----------------------------------------------------------------------

VertexRenderingInfo::EdgeRenderingInfoPtrList & VertexRenderingInfo::GetAllOutgoingEdgesRenderingInfo (void)
	{ return outEdges; }

//-----------------------------------------------------------------------

const VertexRenderingInfo::EdgeRenderingInfoPtrList & VertexRenderingInfo::GetAllOutgoingEdgesRenderingInfo (void) const
	{ return outEdges; }

//-----------------------------------------------------------------------

bool VertexRenderingInfo::operator != (const VertexRenderingInfo * v) 
	{ return id != v->id; }

//-----------------------------------------------------------------------

bool VertexRenderingInfo::operator == (const VertexRenderingInfo * v) 
	{ return id == v->id; }

//-----------------------------------------------------------------------

const VertexRenderingInfo::PrimitivePtrList & VertexRenderingInfo::GetVertexPrimitives (void) const
	{ return primitives; }

//-----------------------------------------------------------------------

VertexRenderingInfo::PrimitivePtrList & VertexRenderingInfo::GetVertexPrimitives (void)
	{ return primitives; }

//-----------------------------------------------------------------------

bool VertexRenderingInfo::HasBookmark (void) const
	{ return hasBookmark; }

//-----------------------------------------------------------------------

void VertexRenderingInfo::RemoveBookmark (void) {
	assert(HasBookmark() && bookmark);

	hasBookmark = false;
	delete bookmark;
	bookmark = (BookmarkRenderingInfo *)0;
}

//-----------------------------------------------------------------------

void VertexRenderingInfo::AddBookmark (BookmarkRenderingInfo * bookmark_) {
	hasBookmark = true;
	bookmark = bookmark_;
}

//-----------------------------------------------------------------------

BookmarkRenderingInfo *	VertexRenderingInfo::GetBookmark (void) const
	{ return bookmark; }


//-----------------------------------------------------------------------

EdgesParameters & VertexRenderingInfo::GetEdgesParameters (void)
	{ return edgesParams; }	

//-----------------------------------------------------------------------

const EdgesParameters & VertexRenderingInfo::GetEdgesParameters (void) const
	{ return edgesParams; }	

//-----------------------------------------------------------------------

void VertexRenderingInfo::AddPrimitive (Primitive * p) {
	assert(p);
	primitives.push_back(p);
}

//-----------------------------------------------------------------------

void VertexRenderingInfo::DeleteSlot (SlotRenderingInfo * slot) {
	mainRect->SetHeight(mainRect->GetHeight() - slot->GetKeyRectangle()->GetHeight());	
	delete slot;																		
	slot = (SlotRenderingInfo*)0;														
}

//-----------------------------------------------------------------------

void VertexRenderingInfo::AddEdgeInList (EdgeRenderingInfo * edge, EdgeRenderingInfoPtrList & list) {
	assert(edge);
	Arrow * arrow = edge->GetArrow();
	if (arrow->Type() == Primitive::SPLICE_ARROW)
		hasSpliceArrow = true;
	list.push_back(edge);	
}

}	//namespace iviews