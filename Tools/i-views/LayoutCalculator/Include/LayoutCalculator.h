/**
 *	LayoutCalculator.h
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	June 2010
 */

#ifndef LAYOUT_CALCULATOR_H 
#define LAYOUT_CALCULATOR_H

#include <map>
#include <set>
#include "Point.h"
#include "Colour.h"
#include "Vertex.h"
#include "DebugInfo.h"
#include "Rectangle.h"
#include "Primitive.h"
#include "Functional.h"
#include "CommonTypes.h"
#include "GraphLayers.h"
#include "LayoutParams.h"
#include "SlotDebugInfo.h"
#include "RenderingParams.h"
#include "LayerRenderingInfo.h"
#include "LayersRenderingInfo.h"
#include "VertexToDebugInfoMapping.h"
#include "InnerEdgesOrderingHeuristic.h"

namespace iviews {
class LayoutCalculator {
public:
	///////////////////////////////////////////////////
	//	public typedef(s)
	typedef graphs::Vertex						Vertex;
	typedef Primitive::PrimitivePtrList			PrimitivePtrList;
	typedef graphs::Vertex::VerticesSet			VerticesSet;
	typedef graphs::VertexToDebugInfoMapping	VertexToDebugInfoMapping;
	typedef LayerLayoutParams::LayerAlignment	LayerAlignment;
	

	LayoutCalculator (LayoutParams* lParams, RenderingParams* rParams);

	void					SetLayoutParams (LayoutParams * lParams);
	void					SetRenderingParams (RenderingParams * rParams);

	void					Clear (void);
	PrimitivePtrList *		GetPrimitivesList (void);

	
	bool					RemoveBookmark (VertexRenderingInfo * v);
	bool 					HideContentsOfVertex (VertexRenderingInfo* v, LayersRenderingInfo* layers);
	bool					ShowContentsOfVertex (VertexRenderingInfo* v, LayersRenderingInfo* layers);
	void					SetCommonBeginingEdges (LayersRenderingInfo * layers, bool enable);
	void					DrawLayersRectangle (LayersRenderingInfo * layers, bool draw);
	void					UpdateColours (LayersRenderingInfo * layers);
	void					ShowVerticesWithFullContents (LayersRenderingInfo * layers, bool show);
	void					SetLayersAlignmentTo (LayersRenderingInfo * layers, LayerAlignment alignment);
	//return null if vertex has alredy a bookmark
	Rectangle*				AddBookmark (VertexRenderingInfo * v);
	
	//caller is responsible to delete the instance
	LayersRenderingInfo *	Calculate (graphs::GraphLayers* graphL, VertexToDebugInfoMapping * debugImfo);
	
	//caller is responsible to delete the instance
	VertexRenderingInfo *	CalculateVertexWithFullContents (Vertex* v, VertexToDebugInfoMapping* debugInfo);

private:
	enum RectangleType {
		SLOT			= 0,
		LAYER			= 1,
		VERTEX			= 2,
		BOOKMARK		= 3,
		EXPAND_BUTTON	= 4
	};


	typedef std::vector<bool>			BoolVec;
	typedef std::vector<int>			IntVec;
	typedef geo::Point<coord_t>			CoordTPoint;
	typedef geo::Dimension<length_t>	Dimension;
	typedef Vertex::VerticesSet			VerticesSet;
	typedef Vertex::VerticesPtrList		VerticesPtrList;

	typedef std::map<	
				Vertex *, 
				LayerRenderingInfo *,
				utility::ComparatorFunctor<Vertex>
			> VertexToLayerRenderingInfoMapping;

	typedef std::map<	
				Vertex *,		//source vertex
				VerticesSet,	//target vertices	
				utility::ComparatorFunctor<Vertex>
			> Edges;		
	
	typedef dinfo::SlotDebugInfo							SlotDebugInfo;
	typedef VertexRenderingInfo::Slots						Slots;
	typedef dinfo::DebugInfo::SlotDebugInfoList				SlotDebugInfoList;
	typedef graphs::GraphLayers::Layer						Layer;
	typedef graphs::Vertex::VerticesPtrList					VerticesPtrList;
	
	typedef EdgeRenderingInfo::EdgeRenderingInfoPtrList		EdgeRenderingInfoPtrList;
	typedef LayerRenderingInfo::LayerRenderingInfoPtrVec	LayerRenderingInfoPtrVec;
	typedef VertexRenderingInfo::VertexRenderingInfoPtrList	VertexRenderingInfoPtrList;
	
	

	void 				CreatePrimitiveList (void);		
	void 				CreateEdges (LayersRenderingInfo & layers);
	void 				CreateLayers (LayersRenderingInfo & layers);
	void 				AddContentesInVertexRenderingInfo (VertexRenderingInfo & vertex);
	void				EnableCommonBeginingEdges (LayersRenderingInfo * layers, VertexRenderingInfo * vertex);
	void				DisableCommonBeginingEdges (LayersRenderingInfo * layers, VertexRenderingInfo * vertex);
	void				ForAllEdgesSetConditionalySlotRenderingInfo (
							EdgeRenderingInfoPtrList&	edges,
							const Vertex*				vertex, 
							SlotRenderingInfo*			slot
						);
	void 				AddVerticesRenderingInfoInLayer (
							LayerRenderingInfo & lri, 
							const VerticesPtrList & vertices
						);

	void 				SetLayersCoordinatesTo (
							LayerLayoutParams::LayerAlignment alignment, 
							LayersRenderingInfo & layers
						);
	void				AddSlotRenderingInfoInOutgoingEdges (VertexRenderingInfo & v);
	void 				CreateEdgeForExpandGraph (
							Vertex *		source, 
							Vertex *		target, 
							SlotDebugInfo *	sourceSlotDebugInfo, 
							Edges &			createdEdges
						);

	void				CreateEdgeForShrinkGraph (
							Vertex* source, 
							Vertex* target, 
							Edges&	createdEdges
						);
	
	void				RemoveContentsOfVertex (VertexRenderingInfo * vertex);
	void				RecalculateEdgesPos (
							LayerRenderingInfo*	 sourceLayer, 
							LayersRenderingInfo* layers
						);

	void				RecalculateEdgesPos (LayersRenderingInfo * layers);

	void				CreateEdgesRenderingInfoIfConectedVertices (
							LayerRenderingInfo * source, 
							LayerRenderingInfo * target
						);

	void				CreateConditionalEdge(
							Vertex*			source,
							Vertex*			target,
							SlotDebugInfo*	slot,
							Edges &			createdEdges
						);

	Dimension			GetRectangleDimension (RectangleType rectType);

	CoordTPoint			CalcEdgeRenderingInfoTail (
							const Layer & sourceLayer,
							const Layer & targetLayer,
							const VertexRenderingInfo * source,
							const Rectangle * sourceExpandButtonRect
						);

	CoordTPoint			CalcEdgeRenderingInfoHead (
							const Layer & sourceLayer,
							const Layer & targetLayer,
							const Rectangle * targetRect
						);

	CoordTPoint			GetTextPosition(const Rectangle* rect, const Dimension & distanceFromRect);



	geo::Colour			GetSlotColourByKeyTypeInfo (
							const SlotDebugInfo	*	slotDebInfo, 
							bool*					isSpecialKeyValue = (bool *)0
						);
	geo::Colour			GetEdgeColour (
							const SlotDebugInfo*		sourceSlot,
							const VertexRenderingInfo * source, 
							const VertexRenderingInfo * target
						);

	LayerRenderingInfo *	
						GetLayerWithBigestHeight (const LayerRenderingInfoPtrVec & layersList);

	Rectangle *			GetProperExpandButton (
							const Vertex*				target, 
							const SlotRenderingInfo*	slot, 
							bool*						isKey = (bool *)0
						);

	//caller is responsible to delete the instance
	Rectangle *			CreateRectangleForLayer (void);	

	//caller is responsible to delete the instance
	Rectangle *			CreateRectangleForVertex (void);

	//caller is responsible to delete the instance
	Text	  *			CreateAbsoluteReferenceText (const Vertex * v, const Rectangle * vertexRect);

	//caller is responsible to delete the instance
	Text	  *			CreateTextForBookmark (const Rectangle * bookmarkRect);

	//caller is responsible to delete the instance
	Text	  *			CreateTextForSlot (
							const bool			  textForKey, 
							const Rectangle		* container, 
							const SlotDebugInfo	* slotDebInfo
						);

	//caller is responsible to delete the instance
	Rectangle *			CreateRectangleForSlotKey (const VertexRenderingInfo * vertexRendInfo);

	//caller is responsible to delete the instance
	Rectangle *			CreateRectangleForSlotContent (const Rectangle * slotKeyRect);

	//caller is responsible to delete the instance
	Rectangle *			CreateRectangleForBookmark (const CoordTPoint & ownerVertexPos);

	//caller is responsible to delete the instance
	Rectangle *			LayoutCalculator::CreateRectangleForSlotExpandButton (
							bool isKey,			
							const Rectangle * keyRect
						);

	//caller is responsible to delete the instance		
	SlotRenderingInfo*	CreateSlotRenderingInfo (
								Vertex * v, 
								VertexRenderingInfo * vertexRendInfo, 
								SlotDebugInfo * slotDebInfo
							);

	//caller is responsible to delete the instance		
	VertexRenderingInfo * 
						CreateVertexRenderingInfo (Vertex * v, const Layer sourceLayer);


	//caller is responsible to delete the instance		
	EdgeRenderingInfo * NewEdgeRenderingInfo (
							const SlotRenderingInfo   * sourceSlot,
							const VertexRenderingInfo * source,
							const VertexRenderingInfo * target,
							const geo::Colour &			edgeColour				= geo::Colour (),
							Rectangle *					sourceExpandButtonRect	= static_cast<Rectangle *>(0)
						);

	std::string 		TextFitsInRect (	
							const std::string & text, 
							const Rectangle * rect, 
							const length_t & seperation
						);


	LayerRenderingInfo*	 GetLayerRenderingInfo (Vertex * vertex);
	VertexRenderingInfo* GetVertexRenderingInfo (LayerRenderingInfo *, Vertex * vertx);


	bool				ExistsLayerWithInnerEdge (void);
	void				InitHasLayersInnerEdges(const LayersRenderingInfo & layers);

	id_t				nextEdgeRenInfoId;		
	id_t				nextSlotRenInfoId;
	id_t				nextLayerRenInfoId;
	id_t				nextVertexRenInfoId;
	id_t				nextBookmarkRenInfoId;	
	BoolVec				hasLayerInnerEdgesVec;		//if layer has inner edge

	bool			 	existsDirtyList;		//If in one of the following list haped a change
	PrimitivePtrList 	layersPrimitives;		//holds the primitives for the layers
	PrimitivePtrList 	verticesPrimitives;		//holds the primitives for the vertices
	PrimitivePtrList 	edgesPrimitives;		//holds the primitives for the edges
	PrimitivePtrList 	bookmarksPrimitives;	//holds the primitives for the bookmarks
	PrimitivePtrList 	primitives;				//holds all primitives for the graph

	graphs::GraphLayers			* graphL;
	LayoutParams				* lParams;
	RenderingParams				* rParams;
	VertexToDebugInfoMapping	* debugInfo;

	InnerEdgesOrderingHeuristic			heuristic;
	VertexToLayerRenderingInfoMapping	vertexToLayer;

	
	
};
}	//namespace iviews

#endif //LAYOUT_CALCULATOR_H


