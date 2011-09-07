/**
 *	LayoutParams.h
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	June 2010
 */

#ifndef LAYOUT_PARAMS_H
#define LAYOUT_PARAMS_H

#include "Colour.h"
#include "Dimension.h"
#include "RenderingTypes.h"
#include "GraphicsLayoutPrimitives.h"

namespace iviews {

	//-----------------------------------------------------------------------

	struct BorderDrawingData {
		length_t	borderWidth;
		geo::Colour borderColour;
		geo::Colour bagroundColour;
		
		BorderDrawingData(
			geo::Colour	_bagroundColour = geo::Colour(255,255,255),
			geo::Colour	_borderColour	= geo::Colour(0,0,0),
			length_t	_borderWidth	= 1
		);

		bool operator != (const BorderDrawingData & data);
		friend bool operator != (const BorderDrawingData & data1, const BorderDrawingData & data2);
		BorderDrawingData(const BorderDrawingData &);
	};
	extern bool operator != (const BorderDrawingData & data1, const BorderDrawingData & data2);
	
	//-----------------------------------------------------------------------

	struct LayerLayoutParams : public BorderDrawingData {
		enum LayerAlignment {	//It is totally necessary that enum's order to be unchangeable
			TOP		= 0, 
			BOTTOM	= 1,
			MIDDLE	= 2 
		};
		
		bool drawLayersRect;
		bool drawVertexWithFullContents;

		length_t layersDistance;		//distance Between layers
		length_t verticesDistance;		//distance Between Vertices In Layer
		length_t innerEdgesDistance;	//distance between inner edges	//in pixels

		geo::Dimension<length_t> distanceFromLayer; //separation Between Layer And Vertex
		geo::Dimension<length_t> distanceFromTopLeftCorner; 
		LayerAlignment layersAlignment;

		LayerLayoutParams(void);
		LayerLayoutParams(const LayerLayoutParams &);
	};

	//-----------------------------------------------------------------------

	struct VertexLayoutParams {
		length_t width;					//vertex's width
		length_t bookmarkWidth;
		length_t expandButtonWidth;		
		

		geo::Dimension<length_t> absRefDistance;	//separation Between absRefText and mainRect
		geo::Dimension<length_t> distanceFromRect;  //separation Between Text inside rectangle 


		struct BordersDrawingData{
			BorderDrawingData forAbsRerf;	//init birders fir vertex's main rectangle
			BorderDrawingData forSlotKey;
			BorderDrawingData forSlotContent;
			BorderDrawingData forKeyExpandButton;
			BorderDrawingData forContentExpandButton;
			
			BordersDrawingData(void);
			BordersDrawingData(const BordersDrawingData &);
		} bordersDrawingData;

		friend bool operator != (const VertexLayoutParams & parms1, const VertexLayoutParams & parms2);

		VertexLayoutParams();
		VertexLayoutParams(const VertexLayoutParams &);
	};
	extern bool operator != (const VertexLayoutParams & parms1, const VertexLayoutParams & parms2);

	


	//-----------------------------------------------------------------------

	struct  LayoutParams {
		LayerLayoutParams			layerParams;
		VertexLayoutParams			vertexParams; 
		GraphicsLayoutPrimitives *	primitives;

		
		LayoutParams(void);
		LayoutParams(GraphicsLayoutPrimitives * glp);
		LayoutParams(const LayoutParams &);

		LayoutParams & operator= (const LayoutParams &);
	};




}	//namespace iviews

#endif	// LAYOUT_PARAMS_H 