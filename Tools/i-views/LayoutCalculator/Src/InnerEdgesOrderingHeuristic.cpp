/**
 *	InnerEdgesOrderingHeuristic.cpp
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	December 2010
 */

#include <assert.h>
#include <algorithm>

#include "Line.h"
#include "Arrow.h"
#include "RenderingTypes.h"
#include "InnerEdgesOrderingHeuristic.h"
#include "RenderingInfoAlgorithmsMacros.h"


#define INITIAL_EDGES_MINOR_RADIUS_SIZE	10


namespace iviews {

/////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void InnerEdgesOrderingHeuristic::operator() (
		const length_t			defaultMinotRadius,
		const length_t			layersDistance,
		const GraphLayers*		graphLayer, 
		LayersRenderingInfo*	layers,
		const BoolVec&			hasLayerInnerEdgesVec
	){
	graph = graphLayer;

	maxMinorRadiusInLayer.clear();
	maxMinorRadiusInLayer.resize(graphLayer->TotalNumberOfLayers());

	for (Layer l = 0; l < hasLayerInnerEdgesVec.size(); ++l)
		if (hasLayerInnerEdgesVec[l]) {
			CalcInnerEdgesMinorRadiusInLayer(*layers->GetLayerRenderingInfo(l));
			UpdateEdgesCoordinates(defaultMinotRadius, l);
		}

	UpdateLayersCoordinates(*layers, layersDistance);
}

//-----------------------------------------------------------------------

void InnerEdgesOrderingHeuristic::InitEdgesMinorRadius (void) {
	edgesMinorRadius.clear();
	edgesMinorRadius.resize(INITIAL_EDGES_MINOR_RADIUS_SIZE);
}

//-----------------------------------------------------------------------

void InnerEdgesOrderingHeuristic::UpdateEdgesCoordinates (
		const length_t	defaultMinorRadius, 
		const Layer		layer
	) {
	for (size_t index = 0; index < edgesMinorRadius.size(); ++index) {
		EdgesDistance::iterator edges;
		for (edges = edgesMinorRadius[index].begin(); edges != edgesMinorRadius[index].end(); ++edges) {
			FOR_ALL_EDGES(edge, (*edges)) {
				length_t minorRadius = (index*defaultMinorRadius) + defaultMinorRadius;
				if (maxMinorRadiusInLayer[layer] < minorRadius)
					maxMinorRadiusInLayer[layer] = minorRadius;
				SetEdgeSpliceArrowMinorRadius(*edge, minorRadius);
			}
		}
	}
}

//-----------------------------------------------------------------------

void InnerEdgesOrderingHeuristic::SetEdgeSpliceArrowMinorRadius (
		EdgeRenderingInfo * edge, 
		length_t			minorRadius
	) {
	SpliceArrow * arrow = dynamic_cast<SpliceArrow *>(edge->GetArrow());
	assert(arrow);
	arrow->SetMinorRadius(minorRadius);
}

//-----------------------------------------------------------------------

void InnerEdgesOrderingHeuristic::StoreEdgesByDistance (EdgeRenderingInfoPtrList & edges, const Layer l) {
	typedef SpliceLine::ControlPoints	ControlPoints;
	assert(edgesMinorRadius.size() > 0);
	
	if (edgesMinorRadius[0].size() < graph->TotalNumberOfVerticesInLayer(l))
		edgesMinorRadius[0].resize(graph->TotalNumberOfVerticesInLayer(l));

	FOR_ALL_EDGES(edge, edges)
		if ((*edge)->IsInnerEdge()){
			long dist = graph->GetDistanceOfVerticesInLayer(
							*(*edge)->GetSourceVertexRenderingInfo()->GetSourceVertex(), 
							*(*edge)->GetTargetVertexRenderingInfo()->GetSourceVertex(), 
							l
						);
			edgesMinorRadius[0][dist].push_back(*edge);
		}
}

//-----------------------------------------------------------------------

void InnerEdgesOrderingHeuristic::CalcInnerEdgesMinorRadiusInLayer (LayerRenderingInfo & layer) {
	InitEdgesMinorRadius();
	FOR_ALL_VERTICES(vertex, layer.GetVerticesRenderingInfo())
		if ((*vertex)->HasSpliceArrow())
			StoreEdgesByDistance((*vertex)->GetAllOutgoingEdgesRenderingInfo(), layer.GetLayer());
	DoOrdering();	//for one layer only
}

//-----------------------------------------------------------------------

bool InnerEdgesOrderingHeuristic::IsEdgeOverlappingWithAny (
		EdgeRenderingInfo *					edge, 
		const EdgeRenderingInfoPtrList &	edges
	) {
	FOR_ALL_EDGES_CONST(i, edges)
		if (!CommonSourceTargetVertex(edge, *i) &&  ExistsOverlapping(edge, *i))
			return true;
	return false;
}

//-----------------------------------------------------------------------

bool InnerEdgesOrderingHeuristic::IsEdgeOverlappingWithAnyInDistance (
		EdgeRenderingInfo *		edge,
		const size_t			initialDistance,
		const size_t			finalDistance,
		const EdgesDistance &	distances
	) {
	assert(distances.size() > finalDistance);

	for (size_t dist = initialDistance; dist <= finalDistance; ++dist)
		if (IsEdgeOverlappingWithAny(edge, distances[dist]))
			return true;
	return false;
}

//-----------------------------------------------------------------------

void InnerEdgesOrderingHeuristic::DoOrdering (void) {
	assert (edgesMinorRadius.size() == INITIAL_EDGES_MINOR_RADIUS_SIZE);
	EdgesDistance edgesDistance = edgesMinorRadius[0];

	for (size_t finalDist = 0; finalDist < edgesDistance.size(); ++finalDist) {	//ietrate all distances in width 0
		EdgeRenderingInfoPtrList::iterator checkEdgeOverlapping =  edgesDistance[finalDist].begin();

		while(checkEdgeOverlapping != edgesDistance[finalDist].end()) 	
			if (IsEdgeOverlappingWithAnyInDistance(	//check each element of the distance for overlapping.
					*checkEdgeOverlapping,			//if exists, move the element (recursively) in next width in same distance.
					0,
					finalDist,
					edgesDistance
				) ){
				MoveOverlappingElementInProperWidth(1, finalDist, *checkEdgeOverlapping);
				EdgeRenderingInfoPtrList::iterator tmp = checkEdgeOverlapping++;
				edgesDistance[finalDist].erase(tmp);	//we have alredy add the edge in a proper position and we must delete the old.
			}
			else
				 ++checkEdgeOverlapping;

	}
}

//-----------------------------------------------------------------------

void InnerEdgesOrderingHeuristic::MoveOverlappingElementInProperWidth (
		const int			nextRadius, 
		const size_t		dist, 
		EdgeRenderingInfo*	edge
	) {
	assert(edge);

	if (edgesMinorRadius.size() <= nextRadius)
		edgesMinorRadius.resize(nextRadius + 1);

	EdgesDistance & edgesDistance = edgesMinorRadius[nextRadius];
	if (edgesDistance.size() <= dist)
		edgesDistance.resize(dist + 1);

	if (IsEdgeOverlappingWithAnyInDistance(edge, 0, dist, edgesDistance))
		MoveOverlappingElementInProperWidth (nextRadius + 1, dist, edge);
	else
		edgesDistance[dist].push_back(edge);
}

//-----------------------------------------------------------------------

bool InnerEdgesOrderingHeuristic::CommonSourceTargetVertex (
		EdgeRenderingInfo * edge1, 
		EdgeRenderingInfo * edge2
	) {
	Vertex * source1 = edge1->GetSourceVertexRenderingInfo()->GetSourceVertex();
	Vertex * target1 = edge1->GetTargetVertexRenderingInfo()->GetSourceVertex();
	Vertex * source2 = edge2->GetSourceVertexRenderingInfo()->GetSourceVertex();
	Vertex * target2 = edge2->GetTargetVertexRenderingInfo()->GetSourceVertex();

	return source1 == source2 && target1 == target2;
}

//-----------------------------------------------------------------------

bool InnerEdgesOrderingHeuristic::ExistsOverlapping (EdgeRenderingInfo * edge1, EdgeRenderingInfo * edge2) {
	SpliceArrow * arrow1 = dynamic_cast<SpliceArrow *>(edge1->GetArrow());
	SpliceArrow * arrow2 = dynamic_cast<SpliceArrow *>(edge2->GetArrow());
	assert(arrow1 && arrow2);

	coord_t y1	= arrow1->GetControlPoints()[1].GetY();
	coord_t y2	= arrow1->GetControlPoints()[2].GetY();
	coord_t y1_	= arrow2->GetControlPoints()[1].GetY();
	coord_t y2_	= arrow2->GetControlPoints()[2].GetY();

	if (y1 > y2)
		std::swap(y1, y2);
	if (y1_ > y2_)
		std::swap(y1_, y2_);

	return	(y1 <= y1_	&& y2 >= y1_)	||
			(y1 <= y2_	&& y2 >= y2_)	||
			(y1 >= y1_	&& 
			 y2 >= y1_	&& 
			 y1 <= y2_	&& 
			 y2 <= y2_);
}

//-----------------------------------------------------------------------

void InnerEdgesOrderingHeuristic::UpdateLayersCoordinates (
		LayersRenderingInfo & layers, 
		const length_t layersDistance
	) {	
	assert(maxMinorRadiusInLayer.size() == graph->TotalNumberOfLayers());

	for (Layer layer = 0; layer < maxMinorRadiusInLayer.size() - 1; ++layer) { //-1 Ignore last layer's inner edges	
		
		//ignore last layer
		if (maxMinorRadiusInLayer[layer] > layersDistance) {

			LayerRenderingInfo * currLayerRenInfo	= layers.GetLayerRenderingInfo(layer);
			LayerRenderingInfo * nextLayerRenInfo	= layers.GetLayerRenderingInfo(layer + 1);

			length_t movement = currLayerRenInfo->GetPosition().GetX()			+
								currLayerRenInfo->GetRectangle()->GetWidth()	+
								maxMinorRadiusInLayer[layer]					+ 
								layersDistance;
			
			length_t dx = movement - nextLayerRenInfo->GetPosition().GetX();

			nextLayerRenInfo->SetPosition(movement, nextLayerRenInfo->GetPosition().GetY());

			//Move all layers after the next layer for dx
			for (Layer l = layer+2; l < maxMinorRadiusInLayer.size(); ++l) {
				LayerRenderingInfo * info	= layers.GetLayerRenderingInfo(l);
				info->SetPosition(info->GetPosition().GetX() + dx, info->GetPosition().GetY());
			}
		}//if
	}
}

}	//namespace iviews