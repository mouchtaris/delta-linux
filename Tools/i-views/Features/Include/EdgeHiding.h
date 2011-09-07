/**
 *	EdgeHiding.h
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	July 2010
 */

#ifndef EDGE_HIDING_H
#define EDGE_HIDING_H


#include "GraphLayers.h"
#include "EdgesParameters.h"
#include "EdgeRenderingInfo.h"
#include "LayersRenderingInfo.h"
#include "VertexRenderingInfo.h"


namespace iviews {

class EdgeHiding {
public:
	enum EdgeHidingPreferences {
		HideInnerEdges		= 0,
		HideIcomingEdges	= 1,
		HideOutgoingEdges	= 2,
		HideAllEdges		= 3,
		ShowInnerEdges		= 4,
		ShowIncomingEdges	= 5,
		ShowOutgoingEdges	= 6,
		ShowAllEdges		= 7
	};


	~EdgeHiding () {}
	EdgeHiding () {}


	void HideOrShowEdgesForVertex (VertexRenderingInfo * vertex, const EdgeHidingPreferences & preference);
	void HideOrShowEdgesForLayre (LayerRenderingInfo * layer, const EdgeHidingPreferences & preference);
	void HideOrShowEdgesForLayres (LayersRenderingInfo * layers, const EdgeHidingPreferences & preference);
	

private:
	typedef graphs::GraphLayers::Layer						Layer;
	typedef EdgeRenderingInfo::EdgeRenderingInfoPtrList		EdgeRenderingInfoPtrList;
	typedef VertexRenderingInfo::VertexRenderingInfoPtrList	VertexRenderingInfoPtrList;

	bool IsHideOrShow (const EdgeHidingPreferences& preference);

	void SetEdgesParameters (
			EdgesParameters&				params,
			const EdgeHidingPreferences&	preference
		);

	void HideOrShowVertices (
			VertexRenderingInfoPtrList&	vertices, 
			const EdgeHidingPreferences&	preference
		);

	void HideOrShowVertex (
			VertexRenderingInfo*			vertex, 
			const EdgeHidingPreferences&	preference
		);

	void HideOrShowInnerEdges (
			EdgeRenderingInfoPtrList&		edges, 
			const EdgeHidingPreferences&	preference
		);
	void HideOrShowBackEdges (
			EdgeRenderingInfoPtrList&		edges, 
			const EdgeHidingPreferences&	preference
		);
	void HideOrShowFrontEdges (
			EdgeRenderingInfoPtrList&		edges, 
			const EdgeHidingPreferences&	preference
		);
	void HideOrShowAllEdges (
			EdgeRenderingInfoPtrList&		edges, 
			const EdgeHidingPreferences&	preference
		);


};

}	//namespace iviews 


#endif	//EDGE_HIDING