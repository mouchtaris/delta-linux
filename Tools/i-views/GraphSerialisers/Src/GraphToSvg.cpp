/**
 *	GraphToSvg.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	January 2011
 */
#include <fstream>
#include <assert.h>

#include "XmlEscape.h"
#include "Primitive.h"
#include "GraphToSvg.h"
#include "StringUtils.h"


#define SVG_END_STRING	 "</svg>"


namespace iviews {

//-----------------------------------------------------------------------

GraphToSvg::GraphToSvg (void) : svgString("")  {}

//-----------------------------------------------------------------------

void GraphToSvg::BeginDrawing (const length_t width, const length_t height) 
	{ svgString = GetSvgBeginString(width, height); }

//-----------------------------------------------------------------------

const std::string & GraphToSvg::EndDrawing (void) 
	{ return svgString.append(GetSvgEndString()); }

//-----------------------------------------------------------------------

const std::string & GraphToSvg::GetSvgStringRepresentation (void) const{ 
	assert(!svgString.empty());
	return svgString; 
}

//-----------------------------------------------------------------------

void GraphToSvg::DrawLine (const Line * line) 
	{ svgString.append(CreateSvgLine(line, " Line")); }

//-----------------------------------------------------------------------

void GraphToSvg::DrawText (const Text * text) 
	{ svgString.append(CreateSvgText(text, " Text")); }

//-----------------------------------------------------------------------

void GraphToSvg::DrawArrow (const Arrow * arrow)
	{ svgString.append(CreateSvgArrow(arrow, " Arrow")); }

//-----------------------------------------------------------------------

void GraphToSvg::DrawRectangle (const Rectangle * rect)
	{ svgString.append(CreateSvgRectangle(rect, " Rect")); }

//-----------------------------------------------------------------------

void GraphToSvg::DrawCurveLine (const CurveLine * line)
	{ svgString.append(CreateSvgCurveLine(line, " CurveLine")); }

//-----------------------------------------------------------------------

void GraphToSvg::DrawSpliceLine (const SpliceLine * line)
	{ svgString.append(CreateSvgSpliceLine(line, " SpliceLine")); }

//-----------------------------------------------------------------------

void GraphToSvg::DrawEllipticArc (const Rectangle * rect)
	{ assert(0);}

//-----------------------------------------------------------------------

void GraphToSvg::DrawTriangle (const Triangle * triangle)
	{ svgString.append(CreateSvgTriangle(triangle, " Triangle"));}

//-----------------------------------------------------------------------

void GraphToSvg::DrawCurveArrow (const CurveArrow * arrow)
	{ svgString.append(CreateSvgCurveArrow(arrow, " CurveArrow")); }

//-----------------------------------------------------------------------

void GraphToSvg::DrawSpliceArrow (const SpliceArrow * arrow)
	{ svgString.append(CreateSvgSpliceArrow(arrow, " SpliceArrow")); }

//-----------------------------------------------------------------------

std::string GraphToSvg::GetSvgBeginString (const length_t width, const length_t height) const { 
	return "<?xml version=\"1.0\" standalone=\"no\"?>\n<!DOCTYPE svg PUBLIC \"-//W3C//DTD GraphToSvg 1.1//EN\"\n\"http://www.w3.org/Graphics/GraphToSvg/1.1/DTD/svg11.dtd\">\n\n<svg width=\""+
		utility::string_cast((unsigned long)width)	+
		"%\" height=\""								+
		utility::string_cast((unsigned long)height)	+
		"%\" version=\"1.1\"\nxmlns=\"http://www.w3.org/2000/svg\">\n\n";
}

//-----------------------------------------------------------------------

std::string GraphToSvg::GetSvgEndString (void) const
	{ return SVG_END_STRING; }

//-----------------------------------------------------------------------

std::string GraphToSvg::CreateSvgRectangle(const Rectangle * rect, const std::string & comment) const{

	length_t borderWidth					= rect->GetBorderWidth();
	geo::Colour borderColour				= rect->GetBorderColour();
	geo::Colour bagroundColour				= rect->GetBagroundColour();
	geo::Point<iviews::coord_t> pos			= rect->GetPosition();
	geo::Dimension<iviews::length_t> dim	= rect->GetDimension();

	return std::string(
				"<rect x=\""							+ 
				utility::string_cast(pos.GetX())		+ 
				"\" y=\""								+ 
				utility::string_cast(pos.GetY())		+ 
				"\"  width=\""							+ 
				utility::string_cast(dim.GetWidth())	+ 
				"\" height=\""							+ 
				utility::string_cast(dim.GetHeight())	+ 
				"\"\nstyle=\"fill:"						+
				bagroundColour.GetAsString()			+
				";stroke-width:"						+
				utility::string_cast(borderWidth)		+ 
				";stroke:"								+
				borderColour.GetAsString()				+
				"\"/><!--" 								+ 
				comment 								+ 
				"-->\n\n"	
			);
}

//-----------------------------------------------------------------------

std::string GraphToSvg::CreateSvgText(const Text * text, const std::string & comment) const{

	return std::string(
				"<text x=\""													+ 
				utility::string_cast(text->GetTextXCoordinate())				+ 
				"\" y=\""														+ 
				utility::string_cast(text->GetTextYCoordinate() + 12)			+	//+12 magic number for svg text aligment
				"\">"															+
				XmlEscape::StringToXMLString(text->GetText())	+
				"</text><!--"													+ 
				comment															+ 
				"-->\n\n"
			);
}

//-----------------------------------------------------------------------

std::string GraphToSvg::CreateSvgTriangle(const Triangle * triangle, const std::string & comment) const {
	length_t widht				= triangle->GetBorderWidth();
	geo::Colour border			= triangle->GetBorderColour();
	geo::Colour baground		= triangle->GetBagroundColour();
	geo::Point<coord_t> head	= triangle->GetHead(); 
	geo::Point<coord_t> left	= triangle->GetLeftSide();
	geo::Point<coord_t> right	= triangle->GetRightSide();

	return std::string(
				"<polygon points=\""				+ 
				utility::string_cast(head.GetX())	+ 
				","									+
				utility::string_cast(head.GetY())	+ 
				" "									+
				utility::string_cast(left.GetX())	+
				","									+
				utility::string_cast(left.GetY())	+ 
				" "									+
				utility::string_cast(right.GetX())	+
				","									+
				utility::string_cast(right.GetY())	+ 
				"\"\nstyle=\"fill:"					+
				baground.GetAsString()				+
				";stroke:" 							+
				border.GetAsString()				+
				";border-width:"					+
				utility::string_cast(widht)			+
				"\"/><!--"							+ 
				comment								+ 
				"-->\n\n"								
			);
}

//-----------------------------------------------------------------------

std::string	GraphToSvg::CreateSvgLine (const Line * line,  const std::string & comment) const {
	assert(line);

	return std::string(
				"<line x1=\""									+
				utility::string_cast(line->GetBegin().GetX())	+
				"\" y1=\""										+
				utility::string_cast(line->GetBegin().GetY())	+
				"\" x2=\"" 										+
				utility::string_cast(line->GetEnd().GetX())		+
				"\" y2=\"" 										+
				utility::string_cast(line->GetEnd().GetY())		+
				"\"\nstyle=\"stroke:" 							+
				line->GetColour().GetAsString()					+
				";stroke-width:"								+
				utility::string_cast(line->GetBorderWidth())	+
				"\"/><!--"										+ 
				comment											+ 
				"-->\n\n"											
			);
}

//-----------------------------------------------------------------------

std::string	GraphToSvg::CreateSvgCurveLine (const CurveLine * line, const std::string & comment) const {
	assert(line);
	return std::string(
				"<path style=\"fill:rgb(255,255,255);stroke-width:"	+
				utility::string_cast(line->GetBorderWidth())		+
				";stroke:"											+
				line->GetColour().GetAsString()						+
				"\"\nd=\"M " 										+
				utility::string_cast(line->GetBegin().GetX())		+
				","													+
				utility::string_cast(line->GetBegin().GetY())		+
				" Q "												+
				utility::string_cast(line->GetCurvePeak().GetX())	+
				","													+
				utility::string_cast(line->GetCurvePeak().GetY())	+
				","													+
				utility::string_cast(line->GetEnd().GetX())			+
				","													+
				utility::string_cast(line->GetEnd().GetY())			+
				"\"/><!--"											+ 
				comment												+ 
				"-->\n\n"											
			);
}

//-----------------------------------------------------------------------

std::string	GraphToSvg::CreateSvgSpliceLine (const SpliceLine * line, const std::string & comment) const {
	assert(line);
	typedef SpliceLine::ControlPoints ControlPoints;

	const ControlPoints & controlPoints = line->GetControlPoints();
	std::string points = "";

	for (size_t i = 0; i < controlPoints.size(); ++i) {
		points.append(controlPoints[i].toString());
		if (i < controlPoints.size() - 1 ) //equal of hasNext
			points.append(" ");
	}

	return std::string(
				"<polyline points=\""							+
				points											+
				"\"\nstyle=\"stroke:" 							+
				line->GetColour().GetAsString()					+
				";fill:white;fill-opacity: 0.0;stroke-width:"	+	//0.0 is entirely transparent, 1.0 is entirely opaque
				utility::string_cast(line->GetBorderWidth())	+
				"\"/><!--"										+ 
				comment											+ 
				"-->\n\n"											
			);
}

//-----------------------------------------------------------------------

std::string GraphToSvg::CreateSvgArrow(const Arrow * arrow, const std::string & comment) const {
	return	CreateSvgTriangle(&arrow->GetArrowHead(), comment + " Haid") + 
			CreateSvgLine(&arrow->GetShaft(), comment + " Shaft");
}

//-----------------------------------------------------------------------

std::string GraphToSvg::CreateSvgCurveArrow(const CurveArrow * arrow, const std::string & comment) const {
	return	CreateSvgTriangle(&arrow->GetArrowHead(), comment + " Haid") + 
			CreateSvgCurveLine(&arrow->GetShaft(), comment + " Shaft");
}

//-----------------------------------------------------------------------

std::string GraphToSvg::CreateSvgSpliceArrow(const SpliceArrow * arrow, const std::string & comment) const{
	return	CreateSvgTriangle(&arrow->GetArrowHead(), comment + " Haid") + 
			CreateSvgSpliceLine(&arrow->GetShaft(), comment + " Shaft");
}

}	//namespace iviews