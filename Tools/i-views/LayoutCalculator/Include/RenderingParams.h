/**
 *	RenderingParams.h
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	June 2010
 */

#ifndef RENDERING_PARMAS_H
#define RENDERING_PARMAS_H

#include "Colour.h"

namespace iviews {

struct RenderingParams {
	struct Colours {
		typedef geo::Colour	Colour;

		Colour absRefColour;
		Colour defaultColour;
		Colour delegationColour;
		Colour inheritanceColour;
		Colour operatorColour;
		Colour attributeColour;
		Colour externidColour;
		Colour internalColour;
		Colour backEdgeColour;
		Colour innerUpwardEdgeColour;
		Colour innerDownwardEdgeColour;
		Colour forwardEdgeColour;
		Colour pathHighlightingIncomingEdgesColour;
		Colour pathHighlightingOutgoingEdgesColour;
		Colour highlightCurrentSlotColour;
		Colour highlightCurrentVertexColour;
		Colour highlightRelativeSlotsColour;
		Colour selectionColour;


		Colours (void);
		Colours (const Colours &);
	} colours;

	RenderingParams (void);
	RenderingParams (const RenderingParams &);

	RenderingParams & operator= (const RenderingParams &);
	friend bool operator != (const RenderingParams& params1, const RenderingParams& params2);
};

extern bool operator != (const RenderingParams& params1, const RenderingParams& params2);
}	//namespace iviews


#endif		//RENDERING_PARMAS_H