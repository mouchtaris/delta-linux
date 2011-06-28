#include <assert.h>
#include "EdgeRenderingInfo.h"
#include "SlotRenderingInfo.h"
#include "VertexRenderingInfo.h"


namespace iviews {


///---- Constractor(s)
EdgeRenderingInfo::EdgeRenderingInfo (
	const id_t&				id_, 
	Arrow*					a, 
	VertexRenderingInfo*	source_,
	VertexRenderingInfo*	target_,
	SlotRenderingInfo*		sourceSlot_,
	Text*					t
) :	id				(id_), 
	arrow			(a), 
	text			(t),
	source			(source_),
	target			(target_),
	sourceSlot		(sourceSlot_),
	isHighlighted	(false)
{
	assert(source && target);
	stringId =	source->GetAbsRefText()->GetText()	+
				" -> "								+
				target->GetAbsRefText()->GetText();
}

//-----------------------------------------------------------------------

id_t EdgeRenderingInfo::Id (void) const  
	{ return id; }

//-----------------------------------------------------------------------

std::string EdgeRenderingInfo::GetIdRelevantToVerticesAbsRef (void) const 
	{ return stringId; }

//-----------------------------------------------------------------------

bool EdgeRenderingInfo::IsBackEdge (void) const 
	{ return source->GetSourceLayer() > target->GetSourceLayer(); }

bool EdgeRenderingInfo::IsFrontEdge (void) const
	{ return source->GetSourceLayer() < target->GetSourceLayer(); }

bool EdgeRenderingInfo::IsInnerEdge (void) const
	{ return source->GetSourceLayer() == target->GetSourceLayer();}

//-----------------------------------------------------------------------

void EdgeRenderingInfo::SetText (Text * t) {
	assert(t);
	text = t;
}

//-----------------------------------------------------------------------

Text * EdgeRenderingInfo::GetText (void) 
	{ return text; }

//-----------------------------------------------------------------------

const Text * EdgeRenderingInfo::GetText (void) const 
	{ return text; }

//-----------------------------------------------------------------------

void EdgeRenderingInfo::SetArrow (Arrow * a) {
	assert(a);
	arrow = a;
}

//-----------------------------------------------------------------------

Arrow * EdgeRenderingInfo::GetArrow (void) {
	assert(arrow);
	return arrow;
}

//-----------------------------------------------------------------------

const Arrow * EdgeRenderingInfo::GetArrow (void) const {
	assert(arrow);
	return arrow;
}

//-----------------------------------------------------------------------

void EdgeRenderingInfo::Highlight (bool b)
	{ isHighlighted = b; }

//-----------------------------------------------------------------------

bool EdgeRenderingInfo::IsHighlighted (void) const
	{ return isHighlighted; }

//-----------------------------------------------------------------------

void EdgeRenderingInfo::SetSoruceSlotRenderingInfo (SlotRenderingInfo * sourceSlot_)
	{sourceSlot = sourceSlot_; }

//-----------------------------------------------------------------------

SlotRenderingInfo * EdgeRenderingInfo::GetSourceSlotRenderingInfo (void) const 
	{ return sourceSlot; }

//-----------------------------------------------------------------------

VertexRenderingInfo * EdgeRenderingInfo::GetSourceVertexRenderingInfo (void) const 
	{ return source; }

//-----------------------------------------------------------------------

VertexRenderingInfo * EdgeRenderingInfo::GetTargetVertexRenderingInfo (void) const 
	{ return target; }


}	//namesapce iviews