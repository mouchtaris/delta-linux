#include "Triangle.h"


namespace iviews {
Triangle::Triangle(const Triangle & triangle) 
:	head			(triangle.head),
	leftSide		(triangle.leftSide),
	rightSide		(triangle.rightSide),
	bagroundColour	(triangle.bagroundColour),
	borderColour	(triangle.borderColour)
{ type = triangle.type; }

//--------------------------------------------------------

Triangle::Triangle(
	const CoordTPoint& head_	,
	const CoordTPoint& leftSide_,
	const CoordTPoint& rightSide_,
	const Colour	& bagroundColour_,
	const Colour	& borderColour_,
	const length_t	  borderWidth_
) :	Primitive		(),
	head			(head_),
	leftSide		(leftSide_),
	rightSide		(rightSide_),
	bagroundColour	(bagroundColour_),
	borderColour	(borderColour_),
	borderWidth		(borderWidth_)
{ type = Primitive::TRIANGLE; }

//--------------------------------------------------------

void Triangle::SetHead(const CoordTPoint & p)	
	{ head = p; }

//--------------------------------------------------------

const Triangle::CoordTPoint & Triangle::GetHead(void) const						
	{ return head; }

//--------------------------------------------------------

void Triangle::SetLeftSide(const CoordTPoint & p)	
	{ leftSide = p; }

//--------------------------------------------------------

const Triangle::CoordTPoint & Triangle::GetLeftSide(void) const						
	{ return leftSide; }

//--------------------------------------------------------

void Triangle::SetRightSide(const CoordTPoint & p)	
	{ rightSide = p; }

//--------------------------------------------------------

const Triangle::CoordTPoint & Triangle::GetRightSide(void) const						
	{ return rightSide; }

//--------------------------------------------------------

void Triangle::SetBorderColour( const Colour & c)		
	{ borderColour = c; }

//--------------------------------------------------------

const Triangle::Colour & Triangle::GetBorderColour(void) const					
	{ return borderColour; }

//--------------------------------------------------------

void Triangle::SetBagroundColour( const Colour & c)	
	{ bagroundColour = c; }

//--------------------------------------------------------

const Triangle::Colour & Triangle::GetBagroundColour(void) const				
	{ return bagroundColour; }

//--------------------------------------------------------

void Triangle::SetBorderWidth(const length_t & width)		
	{ borderWidth = width; }

//--------------------------------------------------------

length_t Triangle::GetBorderWidth(void) const					
	{ return borderWidth; }


}	//namespace iviews