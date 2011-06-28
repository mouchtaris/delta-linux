#include "LayoutParams.h"


namespace iviews {





////////////////////////////////////////////////
//	Class BorderDrawingData

//-----------------------------------------------------------------------

BorderDrawingData::BorderDrawingData(
	geo::Colour	bagroundColour_,
	geo::Colour	borderColour_,
	length_t	borderWidth_
) :	borderColour	(borderColour_),
	bagroundColour	(bagroundColour_),
	borderWidth		(borderWidth_)
{}

//-----------------------------------------------------------------------

BorderDrawingData::BorderDrawingData(const BorderDrawingData & bdd)
:	borderColour	(bdd.borderColour),
	bagroundColour	(bdd.bagroundColour),
	borderWidth		(bdd.borderWidth)
{}

//-----------------------------------------------------------------------

bool BorderDrawingData::operator != (const BorderDrawingData & data) {
	return borderWidth		!= data.borderWidth			||
		   borderColour		!= data.borderColour		||
		   bagroundColour	!= data.bagroundColour;
}

//-----------------------------------------------------------------------

bool operator != (const BorderDrawingData & data1, const BorderDrawingData & data2) {
	return data1.borderWidth	!= data2.borderWidth		||
		   data1.borderColour	!= data2.borderColour		||
		   data1.bagroundColour	!= data2.bagroundColour;
}


////////////////////////////////////////////////
//	Class LayerLayoutParams

//-----------------------------------------------------------------------

LayerLayoutParams::LayerLayoutParams() 
:	BorderDrawingData			(),
	drawLayersRect				(true),
	drawVertexWithFullContents	(true),
	layersDistance				(256.0f),
	verticesDistance			(64.0f),
	innerEdgesDistance			(100.0f),
	layersAlignment				(LayerLayoutParams::MIDDLE),
	distanceFromLayer			(32.0f, 32.0f),
	distanceFromTopLeftCorner	(128.0f, 128.0f)
{}

//-----------------------------------------------------------------------

LayerLayoutParams::LayerLayoutParams(const LayerLayoutParams & llp)
:	BorderDrawingData			(llp),
	drawLayersRect				(llp.drawLayersRect),
	drawVertexWithFullContents	(llp.drawVertexWithFullContents),
	verticesDistance			(llp.verticesDistance),
	layersDistance				(llp.layersDistance),
	innerEdgesDistance			(llp.innerEdgesDistance),
	layersAlignment				(llp.layersAlignment),
	distanceFromLayer			(llp.distanceFromLayer),
	distanceFromTopLeftCorner	(llp.distanceFromTopLeftCorner)
{}
	



////////////////////////////////////////////////
//	Class VertexLayoutParams

//-----------------------------------------------------------------------

VertexLayoutParams::VertexLayoutParams() 
:	bordersDrawingData		(),
	width					(256.0f),
	bookmarkWidth			(128.0f),
	expandButtonWidth		(16.0f),
	absRefDistance			(8.0f,8.0f),
	distanceFromRect		(8.0f, 8.0f)
{}

//-----------------------------------------------------------------------

VertexLayoutParams::VertexLayoutParams(const VertexLayoutParams & vlp)
:	bordersDrawingData		(vlp.bordersDrawingData),
	width					(vlp.width),
	bookmarkWidth			(vlp.bookmarkWidth),
	expandButtonWidth		(vlp.expandButtonWidth),
	absRefDistance			(vlp.absRefDistance),
	distanceFromRect		(vlp.distanceFromRect)
{}

//-----------------------------------------------------------------------

bool operator != (const VertexLayoutParams & params1, const VertexLayoutParams & params2) {
	return (params1.width										!= params2.width									||
			params1.bookmarkWidth								!= params2.bookmarkWidth							||
			params1.expandButtonWidth							!= params2.expandButtonWidth						||
			params1.absRefDistance								!= params2.absRefDistance						||
			params1.distanceFromRect							!= params2.distanceFromRect						||
			params1.bordersDrawingData.forAbsRerf				!= params2.bordersDrawingData.forAbsRerf			||
			params1.bordersDrawingData.forSlotKey				!= params2.bordersDrawingData.forSlotKey			||
			params1.bordersDrawingData.forSlotContent			!= params2.bordersDrawingData.forSlotContent		||
			params1.bordersDrawingData.forKeyExpandButton		!= params2.bordersDrawingData.forKeyExpandButton	||
			params1.bordersDrawingData.forContentExpandButton	!= params2.bordersDrawingData.forContentExpandButton);
}

//-----------------------------------------------------------------------

VertexLayoutParams::BordersDrawingData::BordersDrawingData(void)
:	forAbsRerf				(),
	forSlotKey				(),
	forSlotContent			(),
	forKeyExpandButton		(),
	forContentExpandButton	()
{}

//-----------------------------------------------------------------------

VertexLayoutParams::BordersDrawingData::BordersDrawingData(const BordersDrawingData & bdd)
:	forAbsRerf				(bdd.forAbsRerf),
	forSlotKey				(bdd.forSlotKey),
	forSlotContent			(bdd.forSlotContent),
	forKeyExpandButton		(bdd.forKeyExpandButton),
	forContentExpandButton	(bdd.forContentExpandButton)
{}




////////////////////////////////////////////////
//	Class LayoutParams

//-----------------------------------------------------------------------

LayoutParams::LayoutParams(GraphicsLayoutPrimitives * glp) 
:	layerParams		(),
	vertexParams	(),
	primitives		(glp)
{}

//-----------------------------------------------------------------------

LayoutParams::LayoutParams(void) 
:	layerParams		(),
	vertexParams	(),
	primitives		((GraphicsLayoutPrimitives *)0)
{}

//-----------------------------------------------------------------------

LayoutParams::LayoutParams(const LayoutParams & lp)
:	layerParams		(lp.layerParams),
	vertexParams	(lp.vertexParams),
	primitives		(lp.primitives)
{}

//-----------------------------------------------------------------------

LayoutParams & LayoutParams::operator= (const LayoutParams & lp) {
	primitives = lp.primitives;

	vertexParams.width										= lp.vertexParams.width;
	vertexParams.bookmarkWidth								= lp.vertexParams.bookmarkWidth;
	vertexParams.absRefDistance								= lp.vertexParams.absRefDistance;
	vertexParams.distanceFromRect							= lp.vertexParams.distanceFromRect;
	vertexParams.expandButtonWidth							= lp.vertexParams.expandButtonWidth;
	vertexParams.bordersDrawingData.forAbsRerf 				= lp.vertexParams.bordersDrawingData.forAbsRerf;
	vertexParams.bordersDrawingData.forSlotKey 				= lp.vertexParams.bordersDrawingData.forSlotKey;
	vertexParams.bordersDrawingData.forSlotContent			= lp.vertexParams.bordersDrawingData.forSlotContent;
	vertexParams.bordersDrawingData.forKeyExpandButton		= lp.vertexParams.bordersDrawingData.forKeyExpandButton;
	vertexParams.bordersDrawingData.forContentExpandButton	= lp.vertexParams.bordersDrawingData.forContentExpandButton;

	layerParams.borderWidth									= lp.layerParams.borderWidth;
	layerParams.borderColour								= lp.layerParams.borderColour;
	layerParams.bagroundColour								= lp.layerParams.bagroundColour;
	layerParams.layersDistance								= lp.layerParams.layersDistance;
	layerParams.drawLayersRect								= lp.layerParams.drawLayersRect;	
	layerParams.layersAlignment								= lp.layerParams.layersAlignment;
	layerParams.verticesDistance							= lp.layerParams.verticesDistance;
	layerParams.distanceFromLayer							= lp.layerParams.distanceFromLayer;
	layerParams.innerEdgesDistance							= lp.layerParams.innerEdgesDistance;
	layerParams.distanceFromTopLeftCorner					= lp.layerParams.distanceFromTopLeftCorner;
	layerParams.drawVertexWithFullContents					= lp.layerParams.drawVertexWithFullContents;
	return *this;
}



} //namespace iviews 