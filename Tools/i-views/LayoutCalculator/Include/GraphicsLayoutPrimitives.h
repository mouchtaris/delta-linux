/**
 *	GraphicsLayoutPrimitives.h
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	June 2010
 */

#ifndef GRAPHICS_LAYOUT_PRIMITIVES_H
#define GRAPHICS_LAYOUT_PRIMITIVES_H

#include <string>
#include "Dimension.h"
#include "RenderingTypes.h"

namespace iviews {
//API to raper any grapichs library we wont to use
class GraphicsLayoutPrimitives {

public:
	virtual ~GraphicsLayoutPrimitives () {}

	virtual length_t	GetTextWidth (const std::string & text) = 0;
	virtual length_t	GetTextHeight (const std::string & text) = 0;
	virtual geo::Dimension<length_t>	
						GetTextDimension (const std::string & text) = 0;
	
	//Gets the average character width of the currently set font.
	virtual length_t	GetCharWidth(void) = 0;

	//Gets the average character height of the currently set font.
	virtual length_t	GetCharHeight(void) = 0;
};

}	//namespace iviews


#endif	//GRAPHICS_LAYOUT_PRIMITIVES_H