#include "WxRenderingAPI.h"

#include <assert.h>

#include <cmath>
#include <wx/pen.h>
#include <wx/colour.h>
#include "TypesConverter.h"

namespace iviews {

//-----------------------------------------------------------------------

WxRenderingAPI::WxRenderingAPI() : dc(static_cast<wxDC * >(0)) 
	{}

//-----------------------------------------------------------------------

void WxRenderingAPI::SetWxDC(wxDC * dc_) 
	{ dc = dc_; }

//-----------------------------------------------------------------------
	
void WxRenderingAPI::DrawText(const Text * text){
	assert(text && dc);

	dc->SetTextForeground(colour2wxColour(text->GetColour()));
	dc->DrawText(std2str(text->GetText()), text->GetTextXCoordinate(), text->GetTextYCoordinate());
}

//-----------------------------------------------------------------------

void WxRenderingAPI::DrawArrow(const Arrow * arrow){
	assert(arrow);
	DrawLine(&(arrow->GetShaft()));
	DrawTriangle(&(arrow->GetArrowHead()));
}

//-----------------------------------------------------------------------

void WxRenderingAPI::DrawRectangle(const Rectangle * rect){
	assert(rect && dc);
	dc->SetPen(wxPen(colour2wxColour(rect->GetBorderColour()), rect->GetBorderWidth()));
	dc->SetBrush(wxBrush(colour2wxColour(rect->GetBagroundColour())));
	dc->DrawRectangle(rect->GetX(), rect->GetY(), rect->GetWidth(), rect->GetHeight());
}

//-----------------------------------------------------------------------

void WxRenderingAPI::DrawCurveLine(const CurveLine * line){
	assert(line && dc);
	
	dc->SetBrush(*wxTRANSPARENT_BRUSH);
	dc->SetPen(wxPen(colour2wxColour(line->GetColour()), line->GetBorderWidth()));
	
	dc->DrawArc( 
		line->GetBegin().GetX(), 
		line->GetBegin().GetY(), 
		line->GetEnd().GetX(), 
		line->GetEnd().GetY(),
		line->GetCurvePeak().GetX(),
		line->GetCurvePeak().GetY()
	);
}

//-----------------------------------------------------------------------

void WxRenderingAPI::DrawSpliceLine(const SpliceLine * line){
	assert(line && dc && line->GetNumberOfControlPoints() == 4);
	
	dc->SetPen(wxPen(colour2wxColour(line->GetColour()), line->GetBorderWidth()));


	const SpliceLine::ControlPoints & cPoints = line->GetControlPoints();

	wxPoint spliceLine[4];
	spliceLine[0] = point2wxPoint(cPoints[0]);
	spliceLine[1] = point2wxPoint(cPoints[1]);
	spliceLine[2] = point2wxPoint(cPoints[2]);
	spliceLine[3] = point2wxPoint(cPoints[3]);

	//dc->DrawSpline(
	//	(int)line->GetNumberOfControlPoints(),
	//	spliceLine
	//);

	dc->DrawLine(spliceLine[0], spliceLine[1]);
	dc->DrawLine(spliceLine[1], spliceLine[2]);
	dc->DrawLine(spliceLine[2], spliceLine[3]);
}

void WxRenderingAPI::DrawEllipticArc(const Rectangle * rect) {
	assert(rect && dc);

	dc->SetBrush(*wxTRANSPARENT_BRUSH);
	dc->SetPen(wxPen(colour2wxColour(rect->GetBorderColour()), rect->GetBorderWidth()));

	dc->DrawEllipticArc(
		rect->GetX(),
		rect->GetY(),
		rect->GetWidth(),
		rect->GetHeight(),
		-90,
		90
	);
}


//-----------------------------------------------------------------------

void WxRenderingAPI::DrawTriangle(const Triangle * triangle){
	assert(triangle && dc);
	wxPoint wxTtriangle[3];
	
	wxTtriangle[0] = point2wxPoint(triangle->GetHead());
	wxTtriangle[1] = point2wxPoint(triangle->GetLeftSide());
	wxTtriangle[2] = point2wxPoint(triangle->GetRightSide());

	dc->SetPen(wxPen(colour2wxColour(triangle->GetBorderColour()), triangle->GetBorderWidth()));
	dc->SetBrush(wxBrush(colour2wxColour(triangle->GetBagroundColour())));
	dc->DrawPolygon(3, wxTtriangle);
}

//-----------------------------------------------------------------------

void WxRenderingAPI::DrawCurveArrow(const CurveArrow * arrow){
	assert(arrow);
	DrawEllipticArc(&(arrow->GetEclipseRect()));
	DrawTriangle(&(arrow->GetArrowHead()));
}

//-----------------------------------------------------------------------

void WxRenderingAPI::DrawSpliceArrow(const SpliceArrow * arrow){
	assert(arrow);
	DrawSpliceLine(&(arrow->GetShaft()));
	DrawTriangle(&(arrow->GetArrowHead()));
}

//-----------------------------------------------------------------------

void WxRenderingAPI::DrawLine(const Line * line){
	assert(line);
	dc->SetPen(wxPen(colour2wxColour(line->GetColour()), line->GetBorderWidth()));
	dc->DrawLine(
		line->GetBegin().GetX(), 
		line->GetBegin().GetY(), 
		line->GetEnd().GetX(), 
		line->GetEnd().GetY()
	);
}

}	//namespace iviews