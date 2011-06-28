//july 2010
#ifndef VERTEX_FINDER_H
#define VERTEX_FINDER_H



#include "RenderingTypes.h"
#include "LayerRenderingInfo.h"
#include "LayersRenderingInfo.h"
#include "VertexRenderingInfo.h"


namespace iviews {
	
class VertexFinder {
public:
	VertexRenderingInfo * operator()(
							LayersRenderingInfo * layersRenInfo, 
							const coord_t & x, 
							const coord_t & y
						);
private:

	typedef LayerRenderingInfo::LayerRenderingInfoPtrVec LayerRenderingInfoPtrVec;
	typedef VertexRenderingInfo::VertexRenderingInfoPtrList VertexRenderingInfoPtrList;
};


}	//namespace iviews 


#endif	//VERTEX_FINDER_H