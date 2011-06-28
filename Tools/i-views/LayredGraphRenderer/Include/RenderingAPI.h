//july 2010
#ifndef RENDERING_API_H
#define RENDERING_API_H

#include "Line.h"
#include "Text.h"
#include "Arrow.h"
#include "Triangle.h"
#include "Rectangle.h"



namespace iviews {

class RenderingAPI {
public:
	virtual void DrawLine (const Line * line) = 0;
	virtual void DrawText (const Text * text) = 0;
	virtual void DrawArrow (const Arrow * arrow) = 0;
	virtual void DrawRectangle (const Rectangle * rect) = 0;
	virtual void DrawCurveLine (const CurveLine * line) = 0;
	virtual void DrawSpliceLine (const SpliceLine * line) = 0;
	virtual void DrawEllipticArc (const Rectangle * rect) = 0;
	virtual void DrawTriangle (const Triangle * triangle) = 0;
	virtual void DrawCurveArrow (const CurveArrow * arrow) = 0;
	virtual void DrawSpliceArrow (const SpliceArrow * arrow) = 0;
};

}	//namespace iviews


#endif	//RENDERING_API_H