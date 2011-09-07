/**
 *	Rectangle.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	June 2010
 */

#include "Rectangle.h"

#include <assert.h>

namespace iviews {

//-----------------------------------------------------------------------

Rectangle::Rectangle(const Rectangle & rect) 
	:	Primitive(),
		position(rect.position),
		dimension(rect.dimension),
		bagroundColour(rect.bagroundColour),
		borderColour(rect.borderColour),
		borderWidth(rect.borderWidth)
{
	assert(rect.type == RECTANGLE);
	type			= rect.type; 
}
	
//-----------------------------------------------------------------------

Rectangle::Rectangle(	
	const CoordTPoint				& position_,
	const geo::Dimension<length_t>	& dimension_,
	const Colour					& bagroundColour_,
	const Colour					& borderColour_,
	const length_t					  borderWidth_
) :	Primitive		(),
	position		(position_),
	dimension		(dimension_),
	bagroundColour	(bagroundColour_),
	borderColour	(borderColour_),
	borderWidth		(borderWidth_)
{ type = RECTANGLE; }

//--------------------------------------------------------
void Rectangle::SetTransparency(const unsigned char alpha) 
	{ bagroundColour.SetAlpha(alpha); }

//--------------------------------------------------------

unsigned char Rectangle::GetTransparency(void) const 
	{ return bagroundColour.GetAlpha(); }

//--------------------------------------------------------

bool Rectangle::IsTransparent(void)	const 
	{ return bagroundColour.GetAlpha() == geo::ALPHA_TRANSPARENT; }

//--------------------------------------------------------

void Rectangle::SetPosition(const coord_t & x, const coord_t & y) 
	{SetX(x); SetY(y); }

//--------------------------------------------------------

void Rectangle::SetPosition(const CoordTPoint & p) 
	{ position = p; }

//--------------------------------------------------------

const Rectangle::CoordTPoint & Rectangle::GetPosition(void) const				
	{ return position; }

//--------------------------------------------------------

void Rectangle::SetX(const coord_t & x) 
	{ position.SetX(x); }

//--------------------------------------------------------

coord_t Rectangle::GetX(void) const	
	{ return position.GetX(); }

//--------------------------------------------------------

void Rectangle::SetY(const coord_t & y) 
	{ position.SetY(y); }

//--------------------------------------------------------

coord_t	Rectangle::GetY(void) const	
	{ return position.GetY(); }

//--------------------------------------------------------

void Rectangle::SetDimension(const geo::Dimension<length_t> & d) 
	{ dimension = d; }

//--------------------------------------------------------

const geo::Dimension<length_t> & Rectangle::GetDimension(void) const						 
	{ return dimension; }

//--------------------------------------------------------

void Rectangle::SetWidth(const length_t & width)	
	{ dimension.SetWidth(width); }

//--------------------------------------------------------

length_t Rectangle::GetWidth(void) const				
	{ return dimension.GetWidth(); }

//--------------------------------------------------------

void Rectangle::SetHeight(const length_t & height)	
	{ dimension.SetHeight(height); }

//--------------------------------------------------------

length_t Rectangle::GetHeight(void) const				
	{ return dimension.GetHeight(); }

//--------------------------------------------------------

void Rectangle::SetBorderColour( const Colour & c) 
	{ borderColour = c; }

//--------------------------------------------------------

const Rectangle::Colour & Rectangle::GetBorderColour(void) const				
	{ return borderColour; }

//--------------------------------------------------------

void Rectangle::SetBagroundColour( const Colour & c)	
	{ bagroundColour = c; }

//--------------------------------------------------------

const Rectangle::Colour & Rectangle::GetBagroundColour(void) const				
	{ return bagroundColour; }

//--------------------------------------------------------

void Rectangle::SetBorderWidth(const length_t & width)		
	{ borderWidth = width; }

//--------------------------------------------------------

length_t Rectangle::GetBorderWidth(void) const					
	{ return borderWidth; }


bool Rectangle::operator!= (const Rectangle * rect) {
	assert(rect);
	return this->operator !=(*rect);
}

bool Rectangle::operator!= (const Rectangle & rect) {
	return	borderWidth		!= rect.GetBorderWidth()	||
			borderColour	!= rect.GetBorderColour()	||
			bagroundColour	!= rect.GetBagroundColour()	||
			position		!= rect.GetPosition()		||
			dimension		!= rect.GetDimension();
}

bool Rectangle::operator== (const Rectangle * rect) {
	assert(rect);
	return this->operator ==(*rect);
}

bool Rectangle::operator== (const Rectangle & rect)
	{ return !this->operator !=(rect); }


//////////////////////////////////////////////////
//	Static methods implementation

//--------------------------------------------------------

bool Rectangle::InBoundaries(const Rectangle * rect, const CoordTPoint & p)  
	{	return Rectangle::InBoundaries(rect, p.GetX(), p.GetY());	}


//--------------------------------------------------------

bool Rectangle::InBoundaries(const Rectangle * rect, const coord_t & x, const coord_t & y) {
	return	x >= rect->GetX()						&&
			x <= rect->GetX() + rect->GetWidth()	&&
			y >= rect->GetY()						&&
			y <= rect->GetY() + rect->GetHeight();
}



}	//namespace iviews
