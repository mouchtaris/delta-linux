/**
 *	VertexFinder.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	July 2010
 */

#include "Rectangle.h"
#include "VertexFinder.h"
#include "RenderingInfoAlgorithmsMacros.h"


namespace iviews {



VertexRenderingInfo * VertexFinder::operator()(
		LayersRenderingInfo * layersRenInfo, 
		const coord_t & x, 
		const coord_t & y
	) { 
	assert(layersRenInfo);
	VertexRenderingInfo * v = static_cast<VertexRenderingInfo *>(0);

	FOR_ALL_LAYERS(layer, layersRenInfo->GetLayersRenderingInfo()) {
		Rectangle * layerRect = (*layer)->GetRectangle();

		//briskoume an tairiazei sto layer
		if (Rectangle::InBoundaries(layerRect, x, y)) {
			
			//8a an einai mesa se kapoio vertex
			FOR_ALL_VERTICES (vertex, (*layer)->GetVerticesRenderingInfo()) {
				Rectangle * vertexRect = (*vertex)->GetVertexMainRectangle();
				if (Rectangle::InBoundaries(vertexRect, x, y))
					return (*vertex);
			}
		}
	}
	

	return v;
}

}	//namespace iviews