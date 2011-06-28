/**
 *	Arrow.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	March 2010
 */


#include "Arrow.h"

#include <cmath>
#include <assert.h>

namespace iviews {

//-----------------------------------------------------------------------

Arrow::Arrow ( 
	const CoordTPoint &	arrowHead_,
	const CoordTPoint &	arrowTail_,
	const length_t		arrowWidth,
	const Colour & 		headBorderColour,	
	const Colour & 		headBagroundColour,
	const Colour & 		shaftBorderColour
) :	Primitive (),
	arrowHead (	
		arrowHead_, 
		CoordTPoint(), 
		CoordTPoint(), 
		headBorderColour, 
		headBagroundColour, 
		arrowWidth
	),
	arrowTail(arrowTail_),
	shaftColour(shaftBorderColour)
{
	type = ARROW;
	CalcArrowLeftRightSide(arrowHead, arrowTail);
}

//-----------------------------------------------------------------------

Line Arrow::GetShaft (void) const 
	{	return Line(arrowTail, arrowHead.GetHead(), shaftColour, arrowHead.GetBorderWidth());	}

//-----------------------------------------------------------------------

void Arrow::SetArrowHead (const CoordTPoint & newHead){
	arrowHead.SetHead(newHead);
	CalcArrowLeftRightSide(arrowHead, arrowTail);
}

//-----------------------------------------------------------------------

void Arrow::SetArrowHead (const coord_t & x, const coord_t & y)
	{ SetArrowHead(CoordTPoint(x, y)); }

//-----------------------------------------------------------------------

const Triangle & Arrow::GetArrowHead (void) const
	{ return arrowHead; }

//-----------------------------------------------------------------------

void Arrow::SetColour (const Colour & c) {
	SetHeadBorderColour(c);
	SetShaftBorderColour(c);
	SetHeadBagroundColour(c);
}

//-----------------------------------------------------------------------

const Arrow::Colour & Arrow::GetColour (void) const
	{	return shaftColour; }

//-----------------------------------------------------------------------

void Arrow::SetShaftBorderColour (const Colour & c) 
	{ shaftColour = c; }

//-----------------------------------------------------------------------

const Arrow::Colour & Arrow::GetShaftBorderColour (void) const
	{ return shaftColour; }

//-----------------------------------------------------------------------

void Arrow::SetHeadBorderColour (const Colour & c)
	{ arrowHead.SetBorderColour(c); }

//-----------------------------------------------------------------------

const Arrow::Colour & Arrow::GetHeadBorderColour (void) const
	{ return arrowHead.GetBorderColour(); }

//-----------------------------------------------------------------------

void Arrow::SetHeadBagroundColour (const Colour & c)
	{ arrowHead.SetBagroundColour(c); }

//-----------------------------------------------------------------------

const Arrow::Colour & Arrow::GetHeadBagroundColour (void) const
	{ return arrowHead.GetBagroundColour(); }

//-----------------------------------------------------------------------

void Arrow::SetWidth (const length_t width) 
	{ arrowHead.SetBorderWidth(width); }

//-----------------------------------------------------------------------

length_t Arrow::GetWidth (void) const
	{ return arrowHead.GetBorderWidth(); }

//-----------------------------------------------------------------------

const Arrow::CoordTPoint & Arrow::GetHead (void) const
	{ return arrowHead.GetHead(); }

//-----------------------------------------------------------------------

void Arrow::SetHead(const CoordTPoint & head_) { 
	arrowHead.SetHead(head_); 
	CalcArrowLeftRightSide(arrowHead, arrowTail);
}

//-----------------------------------------------------------------------

void Arrow::SetHead(const coord_t & x, const coord_t & y)
	{ SetHead(CoordTPoint(x, y)); }

//-----------------------------------------------------------------------

const Arrow::CoordTPoint & Arrow::GetTail (void) const
	{ return arrowTail; }

//-----------------------------------------------------------------------

void Arrow::SetTail(const CoordTPoint & tail_){ 
	arrowTail = tail_; 
	CalcArrowLeftRightSide(arrowHead, arrowTail);
}

//-----------------------------------------------------------------------

void Arrow::SetTail(const coord_t & x, const coord_t & y) 
	{ SetTail(CoordTPoint(x, y)); }

//-----------------------------------------------------------------------

const Arrow::CoordTPoint & Arrow::GetLeftSide (void) const
	{ return arrowHead.GetLeftSide(); }

//-----------------------------------------------------------------------

void Arrow::SetLeftSide(const CoordTPoint & leftSide)
	{ arrowHead.SetLeftSide(leftSide); }

//-----------------------------------------------------------------------

const Arrow::CoordTPoint & Arrow::GetRightSide (void)	const
	{ return arrowHead.GetRightSide(); }

//-----------------------------------------------------------------------

void Arrow::SetRightSide(const CoordTPoint & rightSide)
	{ arrowHead.SetRightSide(rightSide); }

//-----------------------------------------------------------------------

void Arrow::CalcArrowLeftRightSide (Triangle & arrowHead, const CoordTPoint & arrowTail) {
	CoordTPoint pBase, arrowLeftSide, arrowRightSide;
	float vecLine[2], vecLeft[2], fLength, th, ta, theta = 60.0f, width = 10;

	// build the line vector
	vecLine[0] = (float) (arrowHead.GetHead().GetX() - arrowTail.GetX());
	vecLine[1] = (float) (arrowHead.GetHead().GetY() - arrowTail.GetY());

	// build the arrow base vector - normal to the line
	vecLeft[0] = - vecLine[1];
	vecLeft[1] = vecLine[0];

	// setup length parameters
	fLength = (float) std::sqrt(vecLine[0] * vecLine[0] + vecLine[1] * vecLine[1]);
	if(fLength != 0) {
		th = width / (2.0f * fLength);
		ta = width / (2.0f * (std::tanf(theta) / 2.0f) * fLength);
		
		// find the base of the arrow
		pBase.SetX((coord_t) (arrowHead.GetHead().GetX() + -ta * vecLine[0]));
		pBase.SetY((coord_t) (arrowHead.GetHead().GetY() + -ta * vecLine[1]));

		// build the points on the sides of the arrow
		arrowLeftSide.SetX((coord_t) (pBase.GetX() + th * vecLeft[0]));
		arrowLeftSide.SetY((coord_t) (pBase.GetY() + th * vecLeft[1]));
		arrowRightSide.SetX((coord_t) (pBase.GetX() + -th * vecLeft[0]));
		arrowRightSide.SetY((coord_t) (pBase.GetY() + -th * vecLeft[1]));
	}
	else
		assert(0);
	arrowHead.SetLeftSide(arrowLeftSide);
	arrowHead.SetRightSide(arrowRightSide);
}




///////////////////////////////////////////////////////////////////////////



CurveArrow::CurveArrow (
	const CoordTPoint & 	arrowHead_,
	const CoordTPoint & 	arrowTail,
	const length_t		arrowWidth,
	const Colour & headBorderColour,
	const Colour & headBagroundColour,
	const Colour & shaftBorderColour
):	Arrow( 
		arrowHead_,
		arrowTail,
		arrowWidth,
		headBorderColour,
		headBagroundColour,
		shaftBorderColour
	) {
	//assert(arrowHead.GetX() == arrowTail.GetX());
	type = CURVE_ARROW;
	//Set the curve pice point
	curvePeak.SetX(arrowHead.GetHead().GetX() + 80);
	curvePeak.SetY((arrowHead.GetHead().GetY() + arrowTail.GetY())/2.0f);


	
	//set the ecplise rect
	eclpiiseRect.SetX(arrowHead_.GetX()-40);

	if (arrowHead_.GetY() < arrowTail.GetY()) 		//arrow from low to up
		eclpiiseRect.SetY(arrowHead_.GetY());
	else 
		eclpiiseRect.SetY(arrowTail.GetY());		//arrow from up to low

	eclpiiseRect.SetWidth(80);
	eclpiiseRect.SetHeight(std::fabs(arrowTail.GetY() - arrowHead_.GetY()));
	
	eclpiiseRect.SetBorderWidth(arrowWidth);
	eclpiiseRect.SetBorderColour(shaftBorderColour);
}

//-----------------------------------------------------------------------

CurveLine CurveArrow::GetShaft (void) const 
	{	return CurveLine(arrowTail, arrowHead.GetHead(), curvePeak, shaftColour, arrowHead.GetBorderWidth());	}

//-----------------------------------------------------------------------

const CurveArrow::CoordTPoint & CurveArrow::GetCurvePeak (void) const 
	{ return curvePeak; }

//-----------------------------------------------------------------------

CurveArrow::CoordTPoint & CurveArrow::GetCurvePeak (void)
	{ return curvePeak; }

//-----------------------------------------------------------------------

const Rectangle & CurveArrow::GetEclipseRect (void) const
	{	return eclpiiseRect; }

//-----------------------------------------------------------------------

Rectangle &	CurveArrow::GetEclipseRect (void)
	{	return eclpiiseRect; }




///////////////////////////////////////////////////////////////////////////


SpliceArrow::SpliceArrow (
	const CoordTPoint &	arrowHead_,
	const CoordTPoint &	arrowTail,
	const length_t 		minorRadius_,
	const length_t		arrowWidth,
	const Colour &		headBorderColour,
	const Colour &		headBagroundColour,
	const Colour &		shaftBorderColour
) : Arrow ( 
		arrowHead_,
		arrowTail,
		arrowWidth,
		headBorderColour,
		headBagroundColour,
		shaftBorderColour
	),
	minorRadius (minorRadius_)
{
	type = SPLICE_ARROW;
	shaft.SetBorderWidth(arrowWidth);
	shaft.SetColour(shaftBorderColour);

	ControlPoints cPoints;
	cPoints.push_back(arrowHead_);
	cPoints.push_back(CoordTPoint(arrowHead_.GetX() + minorRadius, arrowHead_.GetY()));
	cPoints.push_back(CoordTPoint(arrowTail.GetX() + minorRadius, arrowTail.GetY()));
	cPoints.push_back(arrowTail);
	shaft.SetControlPoints(cPoints);

	CalcArrowLeftRightSide(arrowHead, cPoints[1]); 
}

//-----------------------------------------------------------------------

void SpliceArrow::SetHead(const CoordTPoint & head) {
	arrowHead.SetHead(head);
	shaft.GetControlPoints()[0] = head;
	shaft.GetControlPoints()[1].SetX(head.GetX() + minorRadius);
	shaft.GetControlPoints()[1].SetY(head.GetY());
	CalcArrowLeftRightSide(arrowHead, shaft.GetControlPoints()[1]);
}

//-----------------------------------------------------------------------

void SpliceArrow::SetHead(const coord_t & x, const coord_t & y)
	{ SetHead(CoordTPoint(x, y)); }

//-----------------------------------------------------------------------

void SpliceArrow::SetTail(const coord_t & x, const coord_t & y)
	{ SetTail(CoordTPoint(x, y)); }

//-----------------------------------------------------------------------

void SpliceArrow::SetTail(const CoordTPoint & tail) {
	arrowTail = tail;
	shaft.GetControlPoints()[3] = arrowTail;
	shaft.GetControlPoints()[2].SetX(arrowTail.GetX() + minorRadius);
	shaft.GetControlPoints()[2].SetY(arrowTail.GetY());
}

//-----------------------------------------------------------------------

length_t SpliceArrow::GetMinorRadius (void) const
	{ return minorRadius; }

//-----------------------------------------------------------------------

void SpliceArrow::SetMinorRadius (const length_t & minorRadius_) {
	minorRadius = minorRadius_;
	shaft.GetControlPoints()[1].SetX(arrowHead.GetHead().GetX() + minorRadius);
	shaft.GetControlPoints()[1].SetY(arrowHead.GetHead().GetY());

	shaft.GetControlPoints()[2].SetX(arrowTail.GetX() + minorRadius);
	shaft.GetControlPoints()[2].SetY(arrowTail.GetY());
}

//-----------------------------------------------------------------------
//we holde the head in contrilpointp[0] 
//we holde the point after head in contrilpointp[1] 
//we holde the point befor tail in contrilpointp[2] 
//we holde the tail in contrilpointp[3] 
SpliceLine SpliceArrow::GetShaft (void) const
	{ return shaft; }

//-----------------------------------------------------------------------

const SpliceArrow::ControlPoints & SpliceArrow::GetControlPoints(void) const 
	{ return shaft.GetControlPoints(); }

//-----------------------------------------------------------------------

void SpliceArrow::SetColour (const Colour & c) {
	Arrow::SetColour(c);
	shaft.SetColour(c);
}



}	//namespace iviews
