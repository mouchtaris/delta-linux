/**
 *	Text.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	March 2010
 */

#include "Text.h"

#include <assert.h>

namespace iviews {

//-----------------------------------------------------------------------

Text::Text(const Text & t) 
	:	Primitive(),
		text(t.text),
		textColour(t.textColour),
		textPosition(t.textPosition)
{ 
	assert(t.type == TEXT);
	type			= t.type; 
}

//-----------------------------------------------------------------------

Text::Text(	
	const std::string	& text_,
	const Colour		& textColour_,
	const CoordTPoint	& textPos_
) :	Primitive		(),
	text			(text_),
	textColour		(textColour_),
	textPosition	(textPos_)
	{ type = TEXT; }

//--------------------------------------------------------

void Text::SetTextPosition(const coord_t & x, const coord_t &y){ 
	textPosition.SetX(x); 
	textPosition.SetY(y);
}

//--------------------------------------------------------

void Text::SetText(const std::string & text_)	
	{ text = text_; }

//--------------------------------------------------------

const std::string & Text::GetText(void) const					
	{ return text; }


//--------------------------------------------------------
void Text::SetTextPosition(const CoordTPoint & pos) 
	{ textPosition = pos; }

//--------------------------------------------------------

Text::CoordTPoint Text::GetTextPosition(void) const						 
	{ return textPosition; }

//--------------------------------------------------------
void Text::SetTextXCoordinate(const coord_t & x) 
	{ textPosition.SetX(x); }

//--------------------------------------------------------

coord_t Text::GetTextXCoordinate(void) const	
	{ return textPosition.GetX(); }

//--------------------------------------------------------

void Text::SetTextYCoordinate(const coord_t & y) 
	{ textPosition.SetY(y); }

//--------------------------------------------------------

coord_t	Text::GetTextYCoordinate(void) const	
	{ return textPosition.GetY(); }

//--------------------------------------------------------
void Text::SetColour(const Colour & colour)	
	{ textColour = colour; }

//--------------------------------------------------------

const Text::Colour & Text::GetColour(void) const					
	{ return textColour; }


}	//namespace iviews
