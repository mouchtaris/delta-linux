#include "LayersRenderingInfo.h"

#include <limits>
#include <utility>
#include <assert.h>
#include <algorithm>
#include <functional>

#include "Point.h"
#include "Rectangle.h"
#include "RenderingTypes.h"
#include "EdgeRenderingInfo.h"
#include "RenderingInfoAlgorithmsMacros.h"


//-----------------------------------------------------------------------





namespace iviews {

//-----------------------------------------------------------------------

LayersRenderingInfo::~LayersRenderingInfo(){
	std::for_each (	
		layers.begin(), 
		layers.end(), 
		utility::DeletePointersFromList<LayerRenderingInfo>()
	);
}

//-----------------------------------------------------------------------

LayersRenderingInfo::LayersRenderingInfo (const Layer & _maxNumOfLayers) 
													:	nextLayer(0), 
														maxNumOfLayers(_maxNumOfLayers){}


//-----------------------------------------------------------------------

bool LayersRenderingInfo::LayerExists (const Layer & l) const 
	{	return l < layers.size();	}

//-----------------------------------------------------------------------

bool LayersRenderingInfo::IsEmpty (void) const
	{ return layers.size() == 0; }

//-----------------------------------------------------------------------

void LayersRenderingInfo::AddLayerRenderingInfoInLayer (LayerRenderingInfo * renInfo, const Layer & l) {
	assert(renInfo);

	if (!LayerExists(l))
		NewLayer(renInfo);
	else {
		delete layers[l];
		layers[l] = renInfo;
	}
}

//-----------------------------------------------------------------------

LayerRenderingInfo * LayersRenderingInfo::GetLayerThatContainsVertex (graphs::Vertex * v) {
	LayerRenderingInfoPtrVec::iterator layer;

	for(layer = layers.begin(); layer != layers.end(); ++layer) 
		if ((*layer)->GetVertexRenderingInfo(v))
			return *layer;

	return static_cast<LayerRenderingInfo *>(0);
}

//-----------------------------------------------------------------------

VertexRenderingInfo	* LayersRenderingInfo::GetVertexRenderingInfo (graphs::Vertex * v) {
	LayerRenderingInfoPtrVec::iterator layer;

	for(layer = layers.begin(); layer != layers.end(); ++layer) 
		if (VertexRenderingInfo	* vertexRenInfo = (*layer)->GetVertexRenderingInfo(v))
			return vertexRenInfo;

	return static_cast<VertexRenderingInfo *>(0);
}

//-----------------------------------------------------------------------
		
LayerRenderingInfo * LayersRenderingInfo::GetLayerRenderingInfo (const Layer & l) const {
	assert(LayerExists(l));
	return layers[l];
}

//-----------------------------------------------------------------------
/*
	bb's point takes the x coordinate from the first layer
	and the y coordinate from the bigest layer
*/
Rectangle LayersRenderingInfo::CalcBoundingBox (length_t distanceBetweenLayers) const {
	Rectangle bb(geo::Point<coord_t>(std::numeric_limits<coord_t>::min (), std::numeric_limits<coord_t>::min ()));
	
	LayerRenderingInfoPtrVec::const_iterator l = layers.begin();
	
	if (l != layers.end())
		bb.SetX((*l)->GetX());

	for(; l != layers.end(); ++l) {	//Find the bigest height
		bb.SetHeight(std::max (bb.GetHeight(), (*l)->GetHeight()));
		if (bb.GetHeight() == (*l)->GetHeight()) //Find layer that is biger that us
			bb.SetY((*l)->GetY());
	}

	//if we have splice arrow we must add the max Minor Radious to find the total width.
	bb.SetWidth(
		GetMaxMinorRadiusFromLayer(layers.back())	+
		layers.back()->GetX()						+ 
		layers.back()->GetWidth()					+
		bb.GetX()			//add the distance from the left border of the screen
	);
	bb.SetHeight(
		bb.GetHeight() + 
		2 * bb.GetY()			//add the distance from the top of the screen
	);
	return Rectangle(bb);
}

//-----------------------------------------------------------------------

EdgesParameters & LayersRenderingInfo::GetEdgesParameters (void)
	{ return edgesParams; }	

//-----------------------------------------------------------------------

const EdgesParameters & LayersRenderingInfo::GetEdgesParameters (void) const
	{ return edgesParams; }	

//-----------------------------------------------------------------------

LayersRenderingInfo::Layer LayersRenderingInfo::NewLayer (LayerRenderingInfo * renInfo) {
	assert(renInfo);
	assert(nextLayer < maxNumOfLayers);
	layers.push_back(renInfo);
	return nextLayer++;
}

//-----------------------------------------------------------------------

length_t LayersRenderingInfo::GetMaxMinorRadiusFromLayer (const LayerRenderingInfo * layer) const {
	length_t maxMinorRadius	= 0;

	FOR_ALL_VERTICES_CONST(vertex, layer->GetVerticesRenderingInfo())
		if ((*vertex)->HasSpliceArrow()) {
			length_t vertexMaxMinorRadius = (*vertex)->CalcMaxMinorRadius();
			if (vertexMaxMinorRadius > maxMinorRadius)
				maxMinorRadius = vertexMaxMinorRadius;
		}
	return maxMinorRadius;
}


}	//namespace iviews