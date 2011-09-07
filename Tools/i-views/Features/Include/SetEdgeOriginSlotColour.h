/**
 *	SetEdgeOriginSlotColour.h
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	July 2010
 */

#ifndef COLOUR_SLOT_FROM_ESGE_H
#define COLOUR_SLOT_FROM_ESGE_H


//new
#include <functional.h>
#include "Colour.h"
#include "ColourAddOnData.h"
#include "EdgeRenderingInfo.h"

namespace iviews {

struct ColourSlotFromEdge {
	Primitive * operator()(EdgeRenderingInfo * edge, const geo::Colour & c) { return 0; }
};

struct SetEdgeOriginSlotColour : public std::binary_function<
											EdgeRenderingInfo *, 
											const geo::Colour &,
											SlotDebugInfoUndoRenderingData
										>
{
	SlotDebugInfoUndoRenderingData operator()(EdgeRenderingInfo * edge, const geo::Colour & c);

};



}

#endif	//COLOUR_SLOT_FROM_ESGE_H