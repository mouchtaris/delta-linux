/**
 *	WxRenderingAPI.h
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	July 2010
 */

#ifndef WX_RENDERING_API_H
#define WX_RENDERING_API_H

#include <wx/dc.h>

#include "Line.h"
#include "Text.h"
#include "Arrow.h"
#include "Triangle.h"
#include "Rectangle.h"
#include "RenderingAPI.h"


namespace iviews {

class WxRenderingAPI : public RenderingAPI{
public:
	WxRenderingAPI ();
	~WxRenderingAPI (){}
	void SetWxDC (wxDC * _dc);

	virtual void DrawLine (const Line * line);
	virtual void DrawText (const Text * text);
	virtual void DrawArrow (const Arrow * arrow);
	virtual void DrawTriangle (const Triangle * triangle);
	virtual void DrawRectangle (const Rectangle * rect);
	virtual void DrawCurveLine (const CurveLine * line);
	virtual void DrawSpliceLine (const SpliceLine * line);
	virtual void DrawCurveArrow (const CurveArrow * arrow);
	virtual void DrawEllipticArc (const Rectangle * rect);
	virtual void DrawSpliceArrow (const SpliceArrow * arrow);

private:
	wxDC * dc;

};

}	//namespace iviews


#endif //WX_RENDERING_API_H