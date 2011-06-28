#ifndef INNER_EDGES_ORDERING_HEURISTIC_H
#define	INNER_EDGES_ORDERING_HEURISTIC_H

#include <map>
#include <list>
#include <vector>

#include "Point.h"
#include "Vertex.h"
#include "Functional.h"
#include "GraphLayers.h"
#include "LayerRenderingInfo.h"
#include "LayersRenderingInfo.h"
#include "VertexRenderingInfo.h"

namespace iviews {

	
class InnerEdgesOrderingHeuristic {
public:
	typedef std::vector<bool>	BoolVec;
	typedef graphs::GraphLayers	GraphLayers;


	void operator() (
			const length_t			edgesDistance,
			const length_t			layersDistance,
			const GraphLayers*		graphLayer, 
			LayersRenderingInfo*	layers,
			const BoolVec&			hasLayerInnerEdgesVec
		);
	
private:
	typedef graphs::Vertex				Vertex;
	typedef geo::Point<coord_t>			CoordTPoint;
	typedef Vertex::VerticesPtrList		VerticesPtrList;
	typedef graphs::GraphLayers::Layer	Layer;
	typedef std::vector<length_t>		LenghtTVec;
	
	typedef EdgeRenderingInfo::EdgeRenderingInfoPtrList 
										EdgeRenderingInfoPtrList;

	typedef VertexRenderingInfo::VertexRenderingInfoPtrList
										VertexRenderingInfoPtrList;

	typedef std::vector<EdgeRenderingInfoPtrList>	
										EdgesDistance;

	typedef std::vector<EdgesDistance>	EdgesMinorRadius;



	void				DoOrdering (void);
	void 				InitEdgesMinorRadius (void);
	void 				SetEdgeSpliceArrowMinorRadius (EdgeRenderingInfo * edge, length_t minorRadius);
	void 				UpdateEdgesCoordinates (const length_t defaultMinotRadius, const Layer layer);
	void				StoreEdgesByDistance (EdgeRenderingInfoPtrList & edges, const Layer l);
	void				CalcInnerEdgesMinorRadiusInLayer (LayerRenderingInfo & layer);
	bool				ExistsOverlapping (EdgeRenderingInfo * edge1, EdgeRenderingInfo * edge2);
	bool				CommonSourceTargetVertex (EdgeRenderingInfo * edge1, EdgeRenderingInfo * edge2);


	void				MoveOverlappingElementInProperWidth (
							const int			nextRadius, 
							const size_t		dist, 
							EdgeRenderingInfo * edge
						);
	void				UpdateLayersCoordinates (
							LayersRenderingInfo & layers, 
							const length_t layersDistance
						);

	
	bool				IsEdgeOverlappingWithAny (
							EdgeRenderingInfo *					edge, 
							const EdgeRenderingInfoPtrList &	edges
						);
	
	bool				IsEdgeOverlappingWithAnyInDistance (
							EdgeRenderingInfo * edge,
							const size_t initialDistance,
							const size_t finalDistance,
							const EdgesDistance & distances
						);




	//CheckedEdges		checkedEdges;
	LenghtTVec			maxMinorRadiusInLayer;
	const GraphLayers*	graph;
	EdgesMinorRadius	edgesMinorRadius;



};


}	//namespace iviews


#endif	//INNER_EDGES_ORDERING_HEURISTIC_H