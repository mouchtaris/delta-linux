
#include <assert.h>
#include "Line.h"


namespace iviews {

//-----------------------------------------------------------------------

Line::Line(const Line & line) 
:	begin(line.begin),
	end(line.end),
	colour(line.colour),
	borderWidth(line.borderWidth)
{ type = line.type; }

//-----------------------------------------------------------------------

Line::Line(
	const CoordTPoint & begin_,
	const CoordTPoint & end_,
	const Colour	  & colour_,
	const length_t		borderWidth_
) :	Primitive	(),
	begin		(begin_),
	end			(end_),
	colour		(colour_),
	borderWidth (borderWidth_)
{ type = Primitive::LINE; }

//--------------------------------------------------------

void Line::SetBegin(const CoordTPoint & p)	
	{ begin = p; }

//--------------------------------------------------------

const Line::CoordTPoint & Line::GetBegin(void) const					
	{ return begin; }

//--------------------------------------------------------

void Line::SetEnd(const CoordTPoint & p)	
	{ end = p; }

//--------------------------------------------------------

const Line::CoordTPoint & Line::GetEnd(void) const					
	{ return end; }


//--------------------------------------------------------
void Line::SetColour( const Colour & c)	
	{ colour = c; }

//--------------------------------------------------------

const Line::Colour & Line::GetColour(void) const				
	{ return colour; }

//--------------------------------------------------------

void Line::SetBorderWidth(const length_t & width)		
	{ borderWidth = width; }

//--------------------------------------------------------

length_t Line::GetBorderWidth(void) const		
	{ return borderWidth; }




///////////////////////////////////////////////////////////////////////////



//-----------------------------------------------------------------------

CurveLine::CurveLine(const CurveLine & line) {
	end			= line.end;
	type		= line.type;
	begin		= line.begin;
	colour		= line.colour;
	curvePeak	= line.curvePeak;
	borderWidth = line.borderWidth;
	
}
		
//-----------------------------------------------------------------------

CurveLine::CurveLine(
	const CoordTPoint & begin_,
	const CoordTPoint & end_,
	const CoordTPoint & peak_,
	const Colour	  & colour_,
	const length_t		borderWidth_
):	Line(
		begin_, 
		end_, 
		colour_, 
		borderWidth_
	),
	curvePeak(peak_)
{
	type = Primitive::CURVE_LINE;
}

//--------------------------------------------------------

const CurveLine::CoordTPoint & CurveLine::GetCurvePeak(void) const 
	{ return curvePeak; }

//--------------------------------------------------------

void CurveLine::SetCurvePeak (const CoordTPoint & peak)
 { curvePeak = peak; }




///////////////////////////////////////////////////////////////////////////


SpliceLine::SpliceLine (
	const Colour&		 colour_,
	const length_t		 borderWidth_
) : Line() {
	colour		= colour_;
	borderWidth = borderWidth_;
}

//--------------------------------------------------------

SpliceLine::SpliceLine (
	const ControlPoints& controlPoints_,
	const Colour&		 colour_,
	const length_t		 borderWidth_
) : Line			(),
	controlPoints	(controlPoints_) 
{
	assert(controlPoints.size() > 1);
	SetBegin(controlPoints.front());
	SetEnd(controlPoints.back());
	colour		= colour_;
	borderWidth = borderWidth_;
}

//--------------------------------------------------------

void SpliceLine::SetBegin (const CoordTPoint & p) {
	assert(controlPoints.size() > 0);
	begin = controlPoints.front() = p;
}

//--------------------------------------------------------

void SpliceLine::SetEnd (const CoordTPoint & p) {
	assert(controlPoints.size() > 0);
	end	= controlPoints.back() = p;
}

//--------------------------------------------------------

size_t SpliceLine::GetNumberOfControlPoints (void) const
	{ return controlPoints.size(); }

//--------------------------------------------------------

const SpliceLine::ControlPoints & SpliceLine::GetControlPoints (void) const 
	{ return controlPoints; }

//--------------------------------------------------------

SpliceLine::ControlPoints & SpliceLine::GetControlPoints (void)
	{ return controlPoints; }

//--------------------------------------------------------
void SpliceLine::SetControlPoints (const ControlPoints & cPoints) { 
	assert(cPoints.size() > 1);
	controlPoints = cPoints; 
	SetBegin(cPoints.front());
	SetEnd(cPoints.back());
}



}	//namespace iviews