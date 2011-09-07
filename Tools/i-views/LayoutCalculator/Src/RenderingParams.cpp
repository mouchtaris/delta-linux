/**
 *	RenderingParams.cpp
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	June 2010
 */

#include "RenderingParams.h"

namespace iviews {

//-----------------------------------------------------------------------

RenderingParams::RenderingParams(void) : colours() {}

//-----------------------------------------------------------------------

RenderingParams::RenderingParams(const RenderingParams & rp)  : colours(rp.colours) {}

//-----------------------------------------------------------------------

RenderingParams::Colours::Colours(void) 
:	pathHighlightingIncomingEdgesColour	(0	, 255, 255),
	pathHighlightingOutgoingEdgesColour	(255, 255, 0  ),
	highlightCurrentSlotColour			(0	, 255, 255),
	highlightCurrentVertexColour		(255, 165, 0  ),
	highlightRelativeSlotsColour		(0	, 0	 , 255),
	selectionColour						(0	, 255, 255)
{}

//-----------------------------------------------------------------------

RenderingParams::Colours::Colours(const Colours & c)
: 	absRefColour						(c.absRefColour),
	defaultColour						(c.defaultColour),						
	delegationColour					(c.delegationColour),					
	inheritanceColour					(c.inheritanceColour),					
	operatorColour						(c.operatorColour),
	attributeColour						(c.attributeColour),
	externidColour						(c.externidColour),
	internalColour						(c.internalColour),
	backEdgeColour						(c.backEdgeColour),						
	innerUpwardEdgeColour				(c.innerUpwardEdgeColour),
	innerDownwardEdgeColour				(c.innerDownwardEdgeColour),
	forwardEdgeColour					(c.forwardEdgeColour),					
	pathHighlightingIncomingEdgesColour	(c.pathHighlightingIncomingEdgesColour),
	pathHighlightingOutgoingEdgesColour	(c.pathHighlightingOutgoingEdgesColour), 
	highlightCurrentSlotColour			(c.highlightCurrentSlotColour),			
	highlightCurrentVertexColour		(c.highlightCurrentVertexColour),		
	highlightRelativeSlotsColour		(c.highlightRelativeSlotsColour),
	selectionColour						(c.selectionColour)
{}

//-----------------------------------------------------------------------

RenderingParams & RenderingParams::operator= (const RenderingParams & rp) {

	colours.absRefColour						= rp.colours.absRefColour;
	colours.defaultColour						= rp.colours.defaultColour;
	colours.delegationColour					= rp.colours.delegationColour;
	colours.inheritanceColour					= rp.colours.inheritanceColour;
	colours.operatorColour						= rp.colours.operatorColour;
	colours.attributeColour						= rp.colours.attributeColour;
	colours.externidColour						= rp.colours.externidColour;
	colours.internalColour						= rp.colours.internalColour;
	colours.backEdgeColour						= rp.colours.backEdgeColour;
	colours.innerUpwardEdgeColour				= rp.colours.innerUpwardEdgeColour;
	colours.innerDownwardEdgeColour				= rp.colours.innerDownwardEdgeColour;
	colours.forwardEdgeColour					= rp.colours.forwardEdgeColour;
	colours.pathHighlightingIncomingEdgesColour = rp.colours.pathHighlightingIncomingEdgesColour;
	colours.pathHighlightingOutgoingEdgesColour = rp.colours.pathHighlightingOutgoingEdgesColour;
	colours.highlightCurrentSlotColour			= rp.colours.highlightCurrentSlotColour;
	colours.highlightCurrentVertexColour		= rp.colours.highlightCurrentVertexColour;
	colours.highlightRelativeSlotsColour		= rp.colours.highlightRelativeSlotsColour;
	colours.selectionColour						= rp.colours.selectionColour;

	return *this;
}


bool operator != (const RenderingParams& params1, const RenderingParams& params2) {
	return	params1.colours.absRefColour						!= params2.colours.absRefColour							||
			params1.colours.defaultColour						!= params2.colours.defaultColour						||
			params1.colours.delegationColour					!= params2.colours.delegationColour						||
			params1.colours.inheritanceColour					!= params2.colours.inheritanceColour					||
			params1.colours.operatorColour						!= params2.colours.operatorColour;
			params1.colours.attributeColour						!= params2.colours.attributeColour;
			params1.colours.externidColour						!= params2.colours.externidColour;
			params1.colours.internalColour						!= params2.colours.internalColour;
			params1.colours.backEdgeColour						!= params2.colours.backEdgeColour						||
			params1.colours.innerUpwardEdgeColour				!= params2.colours.innerUpwardEdgeColour				||
			params1.colours.innerDownwardEdgeColour				!= params2.colours.innerDownwardEdgeColour				||
			params1.colours.forwardEdgeColour					!= params2.colours.forwardEdgeColour					||
			params1.colours.pathHighlightingIncomingEdgesColour != params2.colours.pathHighlightingIncomingEdgesColour	||
			params1.colours.pathHighlightingOutgoingEdgesColour != params2.colours.pathHighlightingOutgoingEdgesColour	||
			params1.colours.highlightCurrentSlotColour			!= params2.colours.highlightCurrentSlotColour			||
			params1.colours.highlightCurrentVertexColour		!= params2.colours.highlightCurrentVertexColour			||
			params1.colours.highlightRelativeSlotsColour		!= params2.colours.highlightRelativeSlotsColour			||
			params1.colours.selectionColour						!= params2.colours.selectionColour;
}

}	//namespace iviews 