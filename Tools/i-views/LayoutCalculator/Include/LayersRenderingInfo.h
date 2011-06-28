//June 2010
#ifndef	LAYERS_RENDERING_INFO 
#define	LAYERS_RENDERING_INFO

#include <set>
#include <vector>
#include "Vertex.h"
#include "GraphLayers.h"
#include "EdgesParameters.h"
#include "LayerRenderingInfo.h"
#include "VertexRenderingInfo.h"


namespace iviews {

class LayersRenderingInfo {

public:
	///////////////////////////////////////////////////
	//	public typedef (s)
	typedef graphs::GraphLayers::Layer						Layer;
	typedef LayerRenderingInfo::LayerRenderingInfoPtrVec	LayerRenderingInfoPtrVec;

	~LayersRenderingInfo ();
	LayersRenderingInfo (const Layer & maxNumOfLayers);


	bool				LayerExists (const Layer & l) const;							
	bool				IsEmpty (void) const;		
	void				AddLayerRenderingInfoInLayer (LayerRenderingInfo * renInfo, const Layer & l);
	Rectangle			CalcBoundingBox (length_t distanceBetweenLayers) const;

	EdgesParameters &	GetEdgesParameters (void);
	const EdgesParameters &
						GetEdgesParameters (void) const;



	LayerRenderingInfo*	GetLayerThatContainsVertex (graphs::Vertex * v);
	LayerRenderingInfo*	GetLayerRenderingInfo (const Layer & l) const;

	VertexRenderingInfo*	
						GetVertexRenderingInfo (graphs::Vertex * v);
	
	LayerRenderingInfoPtrVec&	
						GetLayersRenderingInfo (void) { return layers; }
	
	const LayerRenderingInfoPtrVec&	
						GetLayersRenderingInfo (void) const { return layers; }
	
private:
	Layer				NewLayer (LayerRenderingInfo * renInfo);
	length_t			GetMaxMinorRadiusFromLayer (const LayerRenderingInfo * layer) const;


	EdgesParameters		edgesParams;

	Layer				nextLayer;		//next layer that will be created
	Layer				maxNumOfLayers;	//max number of layers in the graph
	LayerRenderingInfoPtrVec	layers;
};



}	//namespace iviews

#endif	//LAYERS_RENDERING_INFO