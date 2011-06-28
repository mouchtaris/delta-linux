//June 2010
#ifndef	GRAPH_TO_SVG_H
#define	GRAPH_TO_SVG_H

#include <string>


#include "Line.h"
#include "Text.h"
#include "Arrow.h"
#include "Triangle.h"
#include "Primitive.h"
#include "Rectangle.h"
#include "RenderingAPI.h"
#include "RenderingTypes.h"

namespace iviews {

class GraphToSvg : public RenderingAPI {
public:
	typedef Primitive::PrimitivePtrList PrimitivePtrList;

	~GraphToSvg(){}
	GraphToSvg (void);

	virtual void 		DrawLine (const Line * line);
	virtual void 		DrawText (const Text * text);
	virtual void 		DrawArrow (const Arrow * arrow);
	virtual void 		DrawRectangle (const Rectangle * rect);
	virtual void 		DrawCurveLine (const CurveLine * line);
	virtual void 		DrawSpliceLine (const SpliceLine * line);
	virtual void 		DrawEllipticArc (const Rectangle * rect);
	virtual void 		DrawTriangle (const Triangle * triangle);
	virtual void 		DrawCurveArrow (const CurveArrow * arrow);
	virtual void 		DrawSpliceArrow (const SpliceArrow * arrow);

	void				BeginDrawing (const length_t width, const length_t height);
	const std::string & EndDrawing (void);
	const std::string &	GetSvgStringRepresentation (void) const;

	std::string			GetSvgBeginString (const length_t width, const length_t height) const;
	std::string			GetSvgEndString (void) const;

	std::string			CreateSvgRectangle (const Rectangle * rect, const std::string & comment = "") const;
	std::string			CreateSvgText (const Text * text, const std::string & comment = "") const;
	std::string			CreateSvgTriangle (const Triangle * triangle, const std::string & comment = "") const;
	std::string			CreateSvgArrow (const Arrow * arrow, const std::string & comment = "") const;
	std::string			CreateSvgCurveArrow (const CurveArrow * arrow, const std::string & comment = "") const;
	std::string			CreateSvgSpliceArrow (const SpliceArrow * arrow, const std::string & comment = "") const;
	std::string			CreateSvgLine (const Line * line,  const std::string & comment = "") const;
	std::string			CreateSvgCurveLine (const CurveLine * line, const std::string & comment = "") const;
	std::string			CreateSvgSpliceLine (const SpliceLine * line, const std::string & comment = "") const;

private:
	//typedef std::map<char, std::string>	XMLEntities;

	std::string			svgString;
};

} //namespace iviews
#endif	//GRAPH_TO_SVG_H