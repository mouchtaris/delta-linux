/**
 *	EdgeHiding.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	July 2010
 */

#include <assert.h>

#include "EdgeHiding.h"
#include "EdgesParameters.h"
#include "LayerRenderingInfo.h"
#include "RenderingInfoAlgorithmsMacros.h"



namespace iviews {

//-----------------------------------------------------------------------

#define HIDE_OR_SHOW_EDGES(HIDE_PREF, SHOW_PREF, CONDITION)								\
	if(true) {																			\
		assert(preference == HIDE_PREF || preference == SHOW_PREF);						\
		bool hide = IsHideOrShow(preference);											\
		FOR_ALL_EDGES(edge, edges) {													\
			Layer source = (*edge)->GetSourceVertexRenderingInfo()->GetSourceLayer();	\
			Layer target = (*edge)->GetTargetVertexRenderingInfo()->GetSourceLayer();	\
			if (CONDITION) {															\
				if (hide && !(*edge)->IsHighlighted())									\
					(*edge)->GetArrow()->Show(false);									\
				else																	\
				if (!hide)																\
					(*edge)->GetArrow()->Show(true);									\
			}																			\
		}																				\
	} else

//-----------------------------------------------------------------------

void EdgeHiding::HideOrShowEdgesForVertex (
		VertexRenderingInfo *			vertex, 
		const EdgeHidingPreferences &	preference
	) {
	assert(vertex);
	HideOrShowVertex(vertex, preference);
}

//-----------------------------------------------------------------------

void EdgeHiding::HideOrShowEdgesForLayre (
		LayerRenderingInfo *			layer, 
		const EdgeHidingPreferences &	preference
	) {
	assert(layer);
	SetEdgesParameters(layer->GetEdgesParameters(), preference);	
	HideOrShowVertices(layer->GetVerticesRenderingInfo(), preference);
}

//-----------------------------------------------------------------------

void EdgeHiding::HideOrShowEdgesForLayres (
		LayersRenderingInfo *			layers, 
		const EdgeHidingPreferences &	preference
	) {
	assert(layers);
	SetEdgesParameters(layers->GetEdgesParameters(), preference);	

	FOR_ALL_LAYERS_CONST (layer, layers->GetLayersRenderingInfo())
		HideOrShowEdgesForLayre((*layer), preference);
}

//-----------------------------------------------------------------------

void EdgeHiding::HideOrShowVertices (
		VertexRenderingInfoPtrList&		vertices, 
		const EdgeHidingPreferences&	preference
	){	
	FOR_ALL_VERTICES(vertex, vertices)
		HideOrShowVertex(*vertex, preference);
}

//-----------------------------------------------------------------------

void EdgeHiding::HideOrShowVertex (
		VertexRenderingInfo*			vertex, 
		const EdgeHidingPreferences&	preference
	) {
	assert(vertex);
	if (preference == HideInnerEdges || preference == ShowInnerEdges) {
		vertex->GetEdgesParameters().HideInnerEdges(IsHideOrShow(preference));
		HideOrShowInnerEdges(vertex->GetAllOutgoingEdgesRenderingInfo(), preference);
	}
	else 
	if (preference == HideIcomingEdges || preference == ShowIncomingEdges) {
		vertex->GetEdgesParameters().HideIncomingEdges(IsHideOrShow(preference));
		HideOrShowBackEdges(vertex->GetAllIncomingEdgesRenderingInfo(), preference);
	}
	else 
	if (preference == HideOutgoingEdges || preference == ShowOutgoingEdges) {
		vertex->GetEdgesParameters().HideOutgoingEdges(IsHideOrShow(preference));
		HideOrShowFrontEdges(vertex->GetAllOutgoingEdgesRenderingInfo(), preference);
	}
	else {
		vertex->GetEdgesParameters().HideAllEdges(IsHideOrShow(preference));
		HideOrShowAllEdges(vertex->GetAllIncomingEdgesRenderingInfo(), preference);
		HideOrShowAllEdges(vertex->GetAllOutgoingEdgesRenderingInfo(), preference);
	}
}

//-----------------------------------------------------------------------

void EdgeHiding::HideOrShowInnerEdges (
		EdgeRenderingInfoPtrList&		edges, 
		const EdgeHidingPreferences&	preference
	) 
	{ HIDE_OR_SHOW_EDGES (HideInnerEdges, ShowInnerEdges, (*edge)->IsInnerEdge()); }

//-----------------------------------------------------------------------

void EdgeHiding::HideOrShowBackEdges (
		EdgeRenderingInfoPtrList&		edges, 
		const EdgeHidingPreferences&	preference
	) 
	{ HIDE_OR_SHOW_EDGES (HideIcomingEdges, ShowIncomingEdges, source > target); }

//-----------------------------------------------------------------------

void EdgeHiding::HideOrShowFrontEdges (
		EdgeRenderingInfoPtrList&		edges, 
		const EdgeHidingPreferences&	preference
	) 
	{ HIDE_OR_SHOW_EDGES (HideOutgoingEdges, ShowOutgoingEdges, source < target); }

//-----------------------------------------------------------------------

void EdgeHiding::HideOrShowAllEdges (
		EdgeRenderingInfoPtrList&		edges, 
		const EdgeHidingPreferences&	preference
	) 
	{ HIDE_OR_SHOW_EDGES (HideAllEdges, ShowAllEdges, true); }

//-----------------------------------------------------------------------

bool EdgeHiding::IsHideOrShow (const EdgeHidingPreferences& preference) {
	if (preference == HideInnerEdges	||
		preference == HideIcomingEdges	||
		preference == HideOutgoingEdges	||
		preference == HideAllEdges)
		return true;
	return false;
}

//-----------------------------------------------------------------------

void EdgeHiding::SetEdgesParameters (
		EdgesParameters&				params,
		const EdgeHidingPreferences&	preference
	) {
	if (preference == HideInnerEdges || preference == ShowInnerEdges)
		params.HideInnerEdges(IsHideOrShow(preference));
	else 
	if (preference == HideIcomingEdges || preference == ShowIncomingEdges)
		params.HideIncomingEdges(IsHideOrShow(preference));
	else 
	if (preference == HideOutgoingEdges || preference == ShowOutgoingEdges) 
		params.HideOutgoingEdges(IsHideOrShow(preference));
	else 
		params.HideAllEdges(IsHideOrShow(preference));
}


}	//namespace iviews 