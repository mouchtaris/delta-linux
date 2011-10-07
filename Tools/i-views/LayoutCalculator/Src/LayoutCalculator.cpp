/**
 *	LayoutCalculator.cpp
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	June 2010
 */

#include <assert.h>
#include <algorithm>

#include <cmath>
#include "Arrow.h"
#include "Dimension.h"
#include "RenderingTypes.h"
#include "LayoutCalculator.h"
#include "RenderingInfoAlgorithmsMacros.h"

#define INNER_EDGES_DIST	lParams->layerParams.innerEdgesDistance

namespace iviews {

//-----------------------------------------------------------------------

LayoutCalculator::LayoutCalculator (LayoutParams* lParams_, RenderingParams* rParams_) :	
	nextEdgeRenInfoId		(0),
	nextSlotRenInfoId		(0),
	nextLayerRenInfoId		(0),
	nextVertexRenInfoId		(0),
	nextBookmarkRenInfoId	(0),
	existsDirtyList			(true),
	layersPrimitives		(),
	verticesPrimitives		(),
	edgesPrimitives			(),
	primitives				(),
	graphL					(static_cast<graphs::GraphLayers *>(0)),
	lParams					(lParams_),
	rParams					(rParams_),
	debugInfo				(static_cast<VertexToDebugInfoMapping *>(0))
{}

//-----------------------------------------------------------------------

void LayoutCalculator::SetLayoutParams (LayoutParams * lParams_)
	{ lParams = lParams_; }

//-----------------------------------------------------------------------

void LayoutCalculator::SetRenderingParams (RenderingParams * rParams_)
	{ rParams = rParams_; }

//-----------------------------------------------------------------------

LayersRenderingInfo * LayoutCalculator::Calculate (
		graphs::GraphLayers*		graphL_,
		VertexToDebugInfoMapping*	debugInfo_
	) {
	assert(graphL_ && debugInfo_);

	graphL		= graphL_;
	debugInfo	= debugInfo_;

	LayersRenderingInfo * layers = new LayersRenderingInfo(graphL->TotalNumberOfLayers());

	//Fistly: We create the layers
	CreateLayers(*layers);

	//Seondly: We set the corect coordinates for each layer
	SetLayersCoordinatesTo(lParams->layerParams.layersAlignment, *layers);	

	//Lastly: We create the edges
	CreateEdges(*layers);
	
	CreatePrimitiveList();
	return layers;
}

//-----------------------------------------------------------------------

void LayoutCalculator::EnableCommonBeginingEdges(LayersRenderingInfo * layers, VertexRenderingInfo * vertex) {
	vertex->SetEdgesCommonStart(true);
	Dimension startDim = vertex->GetStartVertexMainDimension();

	CoordTPoint newEdgesTailPos( 
					vertex->GetX() + startDim.GetWidth(),
					vertex->GetY() + (startDim.GetHeight() / 2)
				);

	CoordTPoint newBackEdgesTailPos( 
					vertex->GetX(),
					vertex->GetY() + (startDim.GetHeight() / 2)
				);

	FOR_ALL_EDGES (edge, vertex->GetAllOutgoingEdgesRenderingInfo()) {
		SlotRenderingInfo * slot = (*edge)->GetSourceSlotRenderingInfo();
		Layer sourceLayer = vertex->GetSourceLayer(); 
		Layer targetLayer = (*edge)->GetTargetVertexRenderingInfo()->GetSourceLayer(); 

		if (slot->GetKeyExpandButtonRectangle())
			slot->GetKeyExpandButtonRectangle()->Show(false);

		if (slot->GetContentExpandButtonRectangle())
			slot->GetContentExpandButtonRectangle()->Show(false);
		
		if (sourceLayer > targetLayer)	//backEdge
			(*edge)->GetArrow()->SetTail(newBackEdgesTailPos);
		else
			(*edge)->GetArrow()->SetTail(newEdgesTailPos);
	}
}

//-----------------------------------------------------------------------

void LayoutCalculator::DisableCommonBeginingEdges(LayersRenderingInfo * layers, VertexRenderingInfo * vertex) {
	vertex->SetEdgesCommonStart(false);

	FOR_ALL_EDGES (edge, vertex->GetAllOutgoingEdgesRenderingInfo()) {
		Layer sourceLayer = vertex->GetSourceLayer();
		Layer targetLayer = (*edge)->GetTargetVertexRenderingInfo()->GetSourceLayer();
	
		Rectangle * properExpandButton = GetProperExpandButton(
											(*edge)->GetTargetVertexRenderingInfo()->GetSourceVertex(),
											(*edge)->GetSourceSlotRenderingInfo()
										);
		properExpandButton->Show(true);
		CoordTPoint newEdgesTailPos = CalcEdgeRenderingInfoTail (
										sourceLayer,
										targetLayer,
										vertex,
										properExpandButton
									);
		(*edge)->GetArrow()->SetTail(newEdgesTailPos);
	}
}

//-----------------------------------------------------------------------

void LayoutCalculator::SetCommonBeginingEdges (LayersRenderingInfo * layers, bool enable) {
	assert(layers);
	FOR_ALL_LAYERS(layer, layers->GetLayersRenderingInfo()) {
		FOR_ALL_VERTICES (vertex, (*layer)->GetVerticesRenderingInfo())
			if ((*vertex)->HasFullContents() && !(*vertex)->HasHiddenContents()) {
				if (enable  && !(*vertex)->HasEdgesCommonStart())
					EnableCommonBeginingEdges(layers, *vertex);
				else if (!enable  && (*vertex)->HasEdgesCommonStart())
					DisableCommonBeginingEdges(layers, *vertex);
				else
					assert (0);				
			}
	}
}

//-----------------------------------------------------------------------

void LayoutCalculator::DrawLayersRectangle (LayersRenderingInfo * layers, bool draw) {
	assert(layers);
	FOR_ALL_LAYERS(layer, layers->GetLayersRenderingInfo()) 
		(*layer)->GetRectangle()->Show(draw);
}

//-----------------------------------------------------------------------

void LayoutCalculator::UpdateColours (LayersRenderingInfo * layers) {
	assert(layers);
	
	FOR_ALL_LAYERS(layer, layers->GetLayersRenderingInfo()) {
		(*layer)->GetRectangle()->SetBagroundColour(lParams->layerParams.bagroundColour);
		(*layer)->GetRectangle()->SetBorderColour(lParams->layerParams.borderColour);

		FOR_ALL_VERTICES(vertex, (*layer)->GetVerticesRenderingInfo()) {
			(*vertex)->GetVertexMainRectangle()->SetBagroundColour(lParams->vertexParams.bordersDrawingData.forAbsRerf.bagroundColour);
			(*vertex)->GetVertexMainRectangle()->SetBorderColour(lParams->vertexParams.bordersDrawingData.forAbsRerf.borderColour);
			(*vertex)->GetAbsRefText()->SetColour(rParams->colours.absRefColour);

			if ((*vertex)->HasFullContents())
				FOR_ALL_SLOTS (slot, (*vertex)->GetAllSlostsRenderingInfo()) {
					(*slot)->GetKeyText()->SetColour(GetSlotColourByKeyTypeInfo((*slot)->GetSlotDebugInfo()));	
					(*slot)->GetKeyText()->SetColour(GetSlotColourByKeyTypeInfo((*slot)->GetSlotDebugInfo()));
				}

			FOR_ALL_EDGES(edge, (*vertex)->GetAllOutgoingEdgesRenderingInfo()) 
				(*edge)->GetArrow()->SetColour(
					GetEdgeColour(
						(*edge)->GetSourceSlotRenderingInfo()->GetSlotDebugInfo(),
						(*edge)->GetSourceVertexRenderingInfo(),
						(*edge)->GetTargetVertexRenderingInfo()
					)
				);
		}//for all edges
	}
}

//-----------------------------------------------------------------------

void LayoutCalculator::ShowVerticesWithFullContents (LayersRenderingInfo * layers, bool show) {
	FOR_ALL_LAYERS(layer, layers->GetLayersRenderingInfo()) {
		FOR_ALL_VERTICES (vertex, (*layer)->GetVerticesRenderingInfo())
			if (show)
				ShowContentsOfVertex(*vertex, layers);
			else
				HideContentsOfVertex(*vertex, layers);
	}
}

//-----------------------------------------------------------------------

void LayoutCalculator::SetLayersAlignmentTo (LayersRenderingInfo * layers, LayerAlignment alignment) {
	length_t height = lParams->layerParams.distanceFromTopLeftCorner.GetHeight();

	switch(alignment){
		case LayerLayoutParams::BOTTOM:{
			Rectangle bb = layers->CalcBoundingBox(lParams->layerParams.layersDistance);
			FOR_ALL_LAYERS(layer, layers->GetLayersRenderingInfo())
				(*layer)->SetPosition((*layer)->GetX(), bb.GetHeight() - ((*layer)->GetHeight() + height)  );
			break;
		}
		case LayerLayoutParams::MIDDLE: {
			LayerRenderingInfo * bigestLayer = GetLayerWithBigestHeight(layers->GetLayersRenderingInfo());
			
			FOR_ALL_LAYERS(layer, layers->GetLayersRenderingInfo()) {
				length_t dHeight = (bigestLayer->GetHeight() - (*layer)->GetHeight()) / 2;
				(*layer)->SetPosition((*layer)->GetX(), bigestLayer->GetY() + dHeight );
			}
			break;
		}
		case LayerLayoutParams::TOP: {
			FOR_ALL_LAYERS(layer, layers->GetLayersRenderingInfo())
				(*layer)->SetPosition((*layer)->GetX(), height);
			break;
		}
		default: assert(0);
	};
}

//-----------------------------------------------------------------------

void LayoutCalculator::Clear (void) { 
	existsDirtyList = true;
	primitives.clear(); 
	vertexToLayer.clear(); 
	edgesPrimitives.clear();
	layersPrimitives.clear();
	verticesPrimitives.clear();
	bookmarksPrimitives.clear();
	hasLayerInnerEdgesVec.clear();
}

//-----------------------------------------------------------------------

LayoutCalculator::PrimitivePtrList * LayoutCalculator::GetPrimitivesList (void) { 
	if (existsDirtyList)
		CreatePrimitiveList();

	return &primitives; 
}

//-----------------------------------------------------------------------

Rectangle * LayoutCalculator::AddBookmark (VertexRenderingInfo * v) {
	assert(v);
	if (!v->HasBookmark()) {
		Rectangle	* rect	= CreateRectangleForBookmark(v->GetPosition());
		Text		* text	= CreateTextForBookmark(rect);
		
		v->AddBookmark(new BookmarkRenderingInfo(nextBookmarkRenInfoId++, text, rect));
		existsDirtyList = true;
		return rect;
	}
	return (Rectangle*)0;
}

//-----------------------------------------------------------------------

bool LayoutCalculator::RemoveBookmark (VertexRenderingInfo * v) {
	assert(v);
	if (v->HasBookmark()) {
		std::pair<id_t, id_t> ids = v->GetBookmark()->GetPrimitivesIds();
		bookmarksPrimitives.remove_if(Primitive::PredicateFunctor (ids.first));
		bookmarksPrimitives.remove_if(Primitive::PredicateFunctor (ids.second));

		v->RemoveBookmark();
		return existsDirtyList = true;
	}
	return false;
}

//-----------------------------------------------------------------------

VertexRenderingInfo * LayoutCalculator::CalculateVertexWithFullContents (
		graphs::Vertex			 * v,
		VertexToDebugInfoMapping * debugInfo_
	) {
	assert(v && debugInfo_);
	debugInfo	= debugInfo_;	
	return CreateVertexRenderingInfo(v, 0);
}

//-----------------------------------------------------------------------

void LayoutCalculator::ForAllEdgesSetConditionalySlotRenderingInfo(
		EdgeRenderingInfoPtrList&	edges,
		const Vertex*				vertex, 
		SlotRenderingInfo*			slot
	) {
		
	FOR_ALL_EDGES (edge, edges)
		if (!(*edge)->GetSourceSlotRenderingInfo()  && 
			(*edge)->GetTargetVertexRenderingInfo()->GetSourceVertex() == vertex
			) {
			(*edge)->SetSoruceSlotRenderingInfo(slot);	
			(*edge)->GetArrow()->SetColour(
				GetEdgeColour(
					slot->GetSlotDebugInfo(),
					(*edge)->GetSourceVertexRenderingInfo(),
					(*edge)->GetTargetVertexRenderingInfo()
				)
			);
			break;
		}
}

//-----------------------------------------------------------------------

void LayoutCalculator::AddSlotRenderingInfoInOutgoingEdges (VertexRenderingInfo & v) {
	Slots slots							= v.GetAllTheSlots();
	EdgeRenderingInfoPtrList & edges	= v.GetAllOutgoingEdgesRenderingInfo();

	for(Slots::iterator slot = slots.begin(); slot != slots.end(); ++slot) {
		if (slot->first->GetKeyTargetVertex())
			ForAllEdgesSetConditionalySlotRenderingInfo(
				edges, 
				slot->first->GetKeyTargetVertex(), 
				slot->second
			);

		if (slot->first->GetContentTargetVertex())
			ForAllEdgesSetConditionalySlotRenderingInfo(
				edges, 
				slot->first->GetContentTargetVertex(), 
				slot->second
			);
	}	
}

//-----------------------------------------------------------------------

void LayoutCalculator::ForAllEdgesCalcHeadPosition (
		EdgeRenderingInfoPtrList &	edges,
		const Rectangle*			vertexMainRectangle
	) {
	FOR_ALL_EDGES(edge, edges) {
		CoordTPoint head  = CalcEdgeRenderingInfoHead(
								(*edge)->GetSourceVertexRenderingInfo()->GetSourceLayer(),
								(*edge)->GetTargetVertexRenderingInfo()->GetSourceLayer(),
								vertexMainRectangle
							);
		(*edge)->GetArrow()->SetHead(head);
	}
}

//-----------------------------------------------------------------------

void LayoutCalculator::ForAllEdgesCalcTailPosition (
		EdgeRenderingInfoPtrList &	edges,
		bool						calcExpandButtonRect
	) {
	FOR_ALL_EDGES(edge, edges) {
		Rectangle * expandButtonRect = static_cast<Rectangle *> (0);
		if (calcExpandButtonRect)
			expandButtonRect = GetProperExpandButton(
									(*edge)->GetTargetVertexRenderingInfo()->GetSourceVertex(),
									(*edge)->GetSourceSlotRenderingInfo()
								);
		
		CoordTPoint tail = CalcEdgeRenderingInfoTail(
								(*edge)->GetSourceVertexRenderingInfo()->GetSourceLayer(),
								(*edge)->GetTargetVertexRenderingInfo()->GetSourceLayer(),
								(*edge)->GetSourceVertexRenderingInfo(),
								expandButtonRect
							);
		(*edge)->GetArrow()->SetTail(tail);
	}
}

//-----------------------------------------------------------------------

void LayoutCalculator::UpdateLayerPos (LayerRenderingInfo * layer, bool showVertexContets) {
	Dimension oldLayerDimension = layer->GetRectangle()->GetDimension();
	CoordTPoint oldLayerPos		= layer->GetPosition();
	layer->RecalculateHeightAndUpdateVerticesPosition();


	Dimension newLayerDimension = layer->GetRectangle()->GetDimension();
	CoordTPoint newLayerPos		= layer->GetPosition();
	length_t dh					= (length_t)std::fabs(
										newLayerDimension.GetHeight() - oldLayerDimension.GetHeight()
									);
	switch(lParams->layerParams.layersAlignment){
		case LayerLayoutParams::BOTTOM:{
			newLayerPos.SetY(
				showVertexContets	? oldLayerPos.GetY() - dh
									: oldLayerPos.GetY() + dh
			);
			layer->SetPosition(newLayerPos);
			break;
		}
		case LayerLayoutParams::MIDDLE:{
			newLayerPos.SetY(
				showVertexContets	? oldLayerPos.GetY() - (dh/2)
									: oldLayerPos.GetY() + (dh/2)
			);
			layer->SetPosition(newLayerPos);
			break;
		}
		case LayerLayoutParams::TOP:	
			break;
		default: assert(0);
	};

	assert (newLayerPos.GetY() >= 0);
}

//-----------------------------------------------------------------------

bool LayoutCalculator::ShowContentsOfVertex (
		VertexRenderingInfo		 * v,
		LayersRenderingInfo		 * layers
	) {
	assert(v );

	LayerRenderingInfo * layer = layers->GetLayerRenderingInfo(v->GetSourceLayer());
	assert(layer);	

	if (v->HasFullContents() && !v->HasHiddenContents())
		return false;
	v->HideContents(false);
	
	if (!v->HasFullContents()) {
		AddContentesInVertexRenderingInfo(*v);
		AddSlotRenderingInfoInOutgoingEdges(*v);
		CreatePrimitiveList();
	}

	ForAllEdgesCalcTailPosition(v->GetAllOutgoingEdgesRenderingInfo(), true);
	ForAllEdgesCalcHeadPosition(v->GetAllIncomingEdgesRenderingInfo(), v->GetVertexMainRectangle());
	UpdateLayerPos (layer, true);
	return true;
}

//-----------------------------------------------------------------------

bool LayoutCalculator::HideContentsOfVertex (VertexRenderingInfo * v, LayersRenderingInfo * layers) {
	assert(v && layers);

	LayerRenderingInfo * layer = layers->GetLayerRenderingInfo(v->GetSourceLayer());
	assert(layer);	

	if (!v->HasFullContents() && v->HasHiddenContents())
		return false;
	v->HideContents(true);
	
	ForAllEdgesCalcTailPosition(v->GetAllOutgoingEdgesRenderingInfo(), false);
	ForAllEdgesCalcHeadPosition(v->GetAllIncomingEdgesRenderingInfo(), v->GetVertexMainRectangle());
	UpdateLayerPos (layer, false);
	return true;
}

//-----------------------------------------------------------------------

void LayoutCalculator::RecalculateEdgesPos (LayerRenderingInfo * sourceLayer, LayersRenderingInfo * layers){
	FOR_ALL_VERTICES(sourceVertex, sourceLayer->GetVerticesRenderingInfo()){											
		FOR_ALL_EDGES(edge, (*sourceVertex)->GetAllOutgoingEdgesRenderingInfo()) {							
																							
			Rectangle			* expandButton	= (Rectangle *)0;							
			VertexRenderingInfo * targetVertex	= (*edge)->GetTargetVertexRenderingInfo();	
			LayerRenderingInfo	* targetLayer	= layers->GetLayerRenderingInfo(targetVertex->GetSourceLayer());
																							
			if ((*sourceVertex)->HasFullContents() && !(*sourceVertex)->HasHiddenContents())											
				expandButton = GetProperExpandButton(										
									targetVertex->GetSourceVertex(),						
									(*edge)->GetSourceSlotRenderingInfo()					
								);															
																							
			CoordTPoint head = CalcEdgeRenderingInfoHead(
									sourceLayer->GetLayer(), 
									targetLayer->GetLayer(), 
									targetVertex->GetVertexMainRectangle()
								);
			
			CoordTPoint tail = CalcEdgeRenderingInfoTail(
									sourceLayer->GetLayer(), 
									targetLayer->GetLayer(), 
									*sourceVertex, 
									expandButton
								);
			(*edge)->GetArrow()->SetTail(tail);
			(*edge)->GetArrow()->SetArrowHead(head);
		}
	}
}

//-----------------------------------------------------------------------

void LayoutCalculator::RecalculateEdgesPos (LayersRenderingInfo * layers){
	FOR_ALL_LAYERS(layer, layers->GetLayersRenderingInfo())
		RecalculateEdgesPos(*layer, layers);
}

//-----------------------------------------------------------------------

Rectangle *	LayoutCalculator::GetProperExpandButton (
		const Vertex*				target, 
		const SlotRenderingInfo*	slot, 
		bool*						isKey
	) {
	assert(target && slot);

	Rectangle		* expandButton	= (Rectangle *)0;			
	SlotDebugInfo	* debugSlot		= slot->GetSlotDebugInfo();
	
	if (debugSlot->GetKeyTargetVertex() && debugSlot->GetKeyTargetVertex()->Id() == target->Id()) {
		if (isKey)
			*isKey = true;
		expandButton = slot->GetKeyExpandButtonRectangle();
	}
	else if (debugSlot->GetContentTargetVertex() && debugSlot->GetContentTargetVertex()->Id() == target->Id()) {
		if (isKey)
			*isKey = false;
		expandButton = slot->GetContentExpandButtonRectangle();
	}
	else
		assert(0);
	
	return expandButton;
}




//////////////////////////////////////////////////////////////////////////////////////////



/************************** Set Layers Coordinates *******************************/

//-----------------------------------------------------------------------

LayerRenderingInfo * LayoutCalculator::GetLayerWithBigestHeight (const LayerRenderingInfoPtrVec & layersList) {
	length_t bigestHeight			 = 0;
	LayerRenderingInfo * bigestLayer = static_cast<LayerRenderingInfo *>(0);
	
	FOR_ALL_LAYERS_CONST(layer, layersList) {
		if (bigestHeight < (*layer)->GetHeight()) {
			bigestLayer	 = (*layer);
			bigestHeight = (*layer)->GetHeight();
		}
	}

	return bigestLayer;
}

//-----------------------------------------------------------------------

#define X_FIRST_LAYER	(*layer)->GetX()+ lParams->layerParams.distanceFromTopLeftCorner.GetWidth()

#define X_LAYER			prevLayer->GetX() + prevLayer->GetWidth() + lParams->layerParams.layersDistance

#define Y_LAYER			bigestY + dHeight

#define D_HEIGHT		alignment == LayerLayoutParams::TOP							\
						?	0														\
						:	(bigest->GetHeight() - (*layer)->GetHeight())/alignment


void LayoutCalculator::SetLayersCoordinatesTo (
		LayerLayoutParams::LayerAlignment alignment, 
		LayersRenderingInfo & layers
	) {
	LayerRenderingInfo * prevLayer	= static_cast<LayerRenderingInfo *>(0);
	LayerRenderingInfo * bigest		= GetLayerWithBigestHeight(layers.GetLayersRenderingInfo());
		
	assert(bigest);
	coord_t bigestY = bigest->GetY() + lParams->layerParams.distanceFromTopLeftCorner.GetHeight();
	

	LayerRenderingInfoPtrVec & layersList			= layers.GetLayersRenderingInfo();
	LayerRenderingInfoPtrVec::const_iterator layer	= layersList.begin();
	
	for(; layer != layersList.end(); ++layer) {
		length_t dHeight = D_HEIGHT;

		if (layer == layersList.begin()) 		//first layer
			if ((*layer) == bigest)				//The first layer is the bigest layer
				bigest->SetPosition(X_FIRST_LAYER, bigestY);
			else
				(*layer)->SetPosition(X_FIRST_LAYER, Y_LAYER);
		
		else {
			if ((*layer) == bigest)
				bigest->SetPosition(X_LAYER, bigestY);
			else
				(*layer)->SetPosition(X_LAYER, Y_LAYER);
		}
		prevLayer = (*layer);
	}
}


/************************** Edges Creation *******************************/

//-----------------------------------------------------------------------

LayoutCalculator::CoordTPoint LayoutCalculator::CalcEdgeRenderingInfoTail (
		const Layer &				sourceLayer,
		const Layer &				targetLayer,
		const VertexRenderingInfo*	source,
		const Rectangle*			sourceExpandButtonRect
	) {
	coord_t x = 0, y = 0;
	Rectangle * sourceRect = source->GetVertexMainRectangle();

	//******** calc x coordinate
	if (targetLayer < sourceLayer)
		x = sourceRect->GetX();
	else if (sourceExpandButtonRect)
		x = sourceExpandButtonRect->GetX() + sourceExpandButtonRect->GetWidth();
	else
		x = sourceRect->GetX() + sourceRect->GetWidth();
		

	//******** calc y coordinate
	if (sourceExpandButtonRect)
		y = sourceExpandButtonRect->GetY() + (sourceExpandButtonRect->GetHeight() / 2.0f);
	else
		y = sourceRect->GetY() + (sourceRect->GetHeight() / 2.0f); //shrink graph
		

	return CoordTPoint(x, y);
}

//-----------------------------------------------------------------------

LayoutCalculator::CoordTPoint LayoutCalculator::CalcEdgeRenderingInfoHead (
		const Layer&		sourceLayer,
		const Layer&		targetLayer,
		const Rectangle*	targetRect
	) {
	coord_t x = 0, y = 0;

	//******** calc x coordinate
	if (targetLayer <= sourceLayer)
		x = targetRect->GetX() + targetRect->GetWidth();
	else
		x = targetRect->GetX();
		

	//******** calc y coordinate
	y = targetRect->GetY() + (targetRect->GetHeight() / 2.0f);
		
	return CoordTPoint(x, y);
}

//-----------------------------------------------------------------------

EdgeRenderingInfo * LayoutCalculator::NewEdgeRenderingInfo (	
		const SlotRenderingInfo   * sourceSlot,
		const VertexRenderingInfo * source,
		const VertexRenderingInfo * target,
		const geo::Colour &			edgeColour,
		Rectangle *					sourceExpandButtonRect
	) {
	assert(source && target);
	Layer sourceLayer = source->GetSourceLayer();
	Layer targetLayer = target->GetSourceLayer();

	CoordTPoint head = CalcEdgeRenderingInfoHead(
							sourceLayer, 
							targetLayer, 
							target->GetVertexMainRectangle()
						);

	CoordTPoint tail = CalcEdgeRenderingInfoTail(
							sourceLayer, 
							targetLayer, 
							source, 
							sourceExpandButtonRect
						);

	Arrow * arrow = 0;
	
	if (sourceLayer == targetLayer) {
		head.SetX(head.GetX() + lParams->vertexParams.expandButtonWidth);
		hasLayerInnerEdgesVec[sourceLayer] = true;
		arrow = new SpliceArrow(head, tail, INNER_EDGES_DIST, 2, edgeColour, edgeColour, edgeColour);
		assert(((SpliceArrow *)arrow)->GetShaft().GetNumberOfControlPoints() == 4);
	}
	else
		arrow = new Arrow(head, tail, 2, edgeColour, edgeColour, edgeColour);
	
	edgesPrimitives.push_back(arrow);
	return new EdgeRenderingInfo(
					nextEdgeRenInfoId++, 
					arrow, 
					const_cast<VertexRenderingInfo *>(source), 
					const_cast<VertexRenderingInfo *>(target), 
					const_cast<SlotRenderingInfo *>(sourceSlot)
				);
}

//-----------------------------------------------------------------------
//Todo delete this 
LayerRenderingInfo * LayoutCalculator::GetLayerRenderingInfo (Vertex * v){
	assert(v);

	VertexToLayerRenderingInfoMapping::iterator i = vertexToLayer.find(v);
	LayerRenderingInfo * layer = (i != vertexToLayer.end()) ? i->second : (LayerRenderingInfo *)0;

	assert(layer);

	return layer;
}

//-----------------------------------------------------------------------

VertexRenderingInfo * LayoutCalculator::GetVertexRenderingInfo (LayerRenderingInfo * layer, Vertex * v) {
	assert(v);
	assert(layer);
	
	VertexRenderingInfo * vertexRenInfo = layer->GetVertexRenderingInfo(v);
	assert(vertexRenInfo);

	return vertexRenInfo;
}

//-----------------------------------------------------------------------
geo::Colour LayoutCalculator::GetEdgeColour(
		const SlotDebugInfo*		sourceSlot,
		const VertexRenderingInfo*	source, 
		const VertexRenderingInfo*	target
	) {
		
	if (sourceSlot) {
		bool isSpecial	= false;
		geo::Colour c	= GetSlotColourByKeyTypeInfo(sourceSlot, &isSpecial);
		if (isSpecial)
			return c;
	}

	if (source->GetX() == target->GetX())						//Inner Edge
		if (source->GetY() < target->GetY())
			return rParams->colours.innerDownwardEdgeColour;
		else
			return rParams->colours.innerUpwardEdgeColour;
	else 
	if (source->GetX() < target->GetX())						//forward Edge
		return rParams->colours.forwardEdgeColour;
	else														//back Edge
		return rParams->colours.backEdgeColour;
}

//-----------------------------------------------------------------------

#define EDGE_CREATION_COMMON_INITS()																	\
	assert(source);																						\
	assert(target);																						\
	LayerRenderingInfo * sourceLayerRenInfo		= GetLayerRenderingInfo(source);						\
	LayerRenderingInfo * targetLayerRenInfo		= GetLayerRenderingInfo(target);						\
	VertexRenderingInfo * sourceVertexRenInfo	= GetVertexRenderingInfo(sourceLayerRenInfo, source);	\
	VertexRenderingInfo * targetVertexRenInfo	= GetVertexRenderingInfo(targetLayerRenInfo, target);


void LayoutCalculator::CreateEdgeForExpandGraph (
		Vertex*			source, 
		Vertex*			target, 
		SlotDebugInfo*	sourceSlotDebugInfo, 
		Edges &			createdEdge
	) {
	assert(lParams->layerParams.drawVertexWithFullContents);
	EDGE_CREATION_COMMON_INITS();

	bool isKey = false;
	SlotRenderingInfo * sri	 = sourceVertexRenInfo->GetSlotRenderingInfo(sourceSlotDebugInfo);	
	EdgeRenderingInfo * edge = NewEdgeRenderingInfo(												
									sri,															
									sourceVertexRenInfo,												
									targetVertexRenInfo,												
									GetEdgeColour(
										sourceSlotDebugInfo, 
										sourceVertexRenInfo, 
										targetVertexRenInfo
									),
									GetProperExpandButton(target, sri, &isKey)
								);
	if (isKey)
		sri->AddKeyEdgeRenderingInfo(edge);
	else
		sri->AddContentEdgeRenderingInfo(edge);

	sourceVertexRenInfo->AddOutEdgeRenderingInfo(edge);
	targetVertexRenInfo->AddInEdgeRenderingInfo(edge);
}

//-----------------------------------------------------------------------

void LayoutCalculator::CreateEdgeForShrinkGraph (
		Vertex* source, 
		Vertex*	target, 
		Edges&	createdEdges
	) {
	assert(!lParams->layerParams.drawVertexWithFullContents);
	EDGE_CREATION_COMMON_INITS();

	Edges::iterator i = createdEdges.find(source);
	if ( (i == createdEdges.end()) || (i->second.find(target) != i->second.end()) ) {	//we skip the duplicate vertices
		EdgeRenderingInfo * edge = NewEdgeRenderingInfo(
										static_cast<SlotRenderingInfo *>(0),
										sourceVertexRenInfo,
										targetVertexRenInfo,
										GetEdgeColour(
											(SlotDebugInfo *)0, 
											sourceVertexRenInfo, 
											targetVertexRenInfo
										)
									);
		sourceVertexRenInfo->AddOutEdgeRenderingInfo(edge);
		targetVertexRenInfo->AddInEdgeRenderingInfo(edge);
	}
}

//-----------------------------------------------------------------------

void LayoutCalculator::CreateConditionalEdge(
		Vertex*			source,
		Vertex*			target,
		SlotDebugInfo*	slot,
		Edges &			createdEdges					
	) {
	assert(source);
	if (target && *target)
		if (lParams->layerParams.drawVertexWithFullContents)													
			CreateEdgeForExpandGraph(source, target, slot, createdEdges);	
		else																									
			CreateEdgeForShrinkGraph(source, target, createdEdges);
}

//-----------------------------------------------------------------------
void LayoutCalculator::CreateEdges (LayersRenderingInfo & layers) {
	InitHasLayersInnerEdges(layers);
	Edges createdEdges;
	
	for(VertexToDebugInfoMapping::iterator i = debugInfo->begin(); i != debugInfo->end(); ++i) {
		
		const SlotDebugInfoList & slots = i->second.GetAllSlots();
		for(SlotDebugInfoList::const_iterator slot = slots.begin(); slot != slots.end(); ++slot) {
			CreateConditionalEdge(
				i->first,
				(*slot).GetKeyTargetVertex(),
				const_cast<SlotDebugInfo *>(&(*slot)),
				createdEdges
			);

			CreateConditionalEdge(
				i->first,
				(*slot).GetContentTargetVertex(),
				const_cast<SlotDebugInfo *>(&(*slot)),
				createdEdges
			);

		}
	}

	if (ExistsLayerWithInnerEdge())
		heuristic(
			lParams->layerParams.innerEdgesDistance,
			lParams->layerParams.layersDistance,
			graphL, 
			&layers, 
			hasLayerInnerEdgesVec
		);
}




/************************** Layers Creation *******************************/

//-----------------------------------------------------------------------

void LayoutCalculator::CreateLayers (LayersRenderingInfo & layers) {
	for (Layer l = graphL->GetFirstLayer(); graphL->HasNextLayer(l); l = graphL->GetNextLayer(l) ) {

		if (graphL->TotalNumberOfVerticesInLayer(l)) {
			LayerRenderingInfo * lri = new LayerRenderingInfo(	
												nextLayerRenInfoId++, 
												l, 
												CreateRectangleForLayer(),
												*lParams
											);
			AddVerticesRenderingInfoInLayer(*lri, graphL->GetAllVerticesInLayer(l));
			layers.AddLayerRenderingInfoInLayer(lri, l);
		}
		else
			assert(0);	//It it impossible to exist an empty layer
	}
}

//-----------------------------------------------------------------------

Rectangle * LayoutCalculator::CreateRectangleForLayer () {
	Rectangle * rect = new Rectangle(	
								CoordTPoint(0,0),
								GetRectangleDimension(LAYER),
			 					lParams->layerParams.bagroundColour,
			 					lParams->layerParams.borderColour,
			 					lParams->layerParams.borderWidth
							);
	rect->Show(lParams->layerParams.drawLayersRect);
	layersPrimitives.push_back(rect);
	return rect;
}

//-----------------------------------------------------------------------

void LayoutCalculator::AddVerticesRenderingInfoInLayer (
		LayerRenderingInfo &	lri, 
		const VerticesPtrList & vertices
	) {
	for(VerticesPtrList::const_iterator vertex = vertices.begin(); vertex != vertices.end(); ++vertex) {
		vertexToLayer[*vertex] = &lri;
		lri.AddVertex(*vertex , CreateVertexRenderingInfo(*vertex, lri.GetLayer()));
	}
}

//-----------------------------------------------------------------------

VertexRenderingInfo * LayoutCalculator::CreateVertexRenderingInfo (Vertex * v, const Layer sourceLayer) {
	assert(v);

	Rectangle	* rect		= CreateRectangleForVertex();
	Text		* absRef	= CreateAbsoluteReferenceText(v, rect);

	VertexRenderingInfo * vertexRendInfo = new VertexRenderingInfo(
													nextVertexRenInfoId++, 
													sourceLayer,
													absRef, 
													rect, 
													v
												);

	if (lParams->layerParams.drawVertexWithFullContents)
		AddContentesInVertexRenderingInfo(*vertexRendInfo);		
	return vertexRendInfo;
}

//-----------------------------------------------------------------------

void LayoutCalculator::AddContentesInVertexRenderingInfo (VertexRenderingInfo & vertex) {
	VertexToDebugInfoMapping::iterator i = debugInfo->find(vertex.GetSourceVertex());
	assert( i != debugInfo->end() );	//vertex v must always exists

	const SlotDebugInfoList & slots = i->second.GetAllSlots();
	for(SlotDebugInfoList::const_iterator slot = slots.begin(); slot != slots.end(); ++slot){
		vertex.FullContents(true);
		vertex.AddSlot(	
			const_cast<SlotDebugInfo *>(&(*slot)), 
			CreateSlotRenderingInfo(
				vertex.GetSourceVertex(), 
				&vertex, 
				const_cast<SlotDebugInfo *>(&(*slot))
			)
		);
	}
}

//-----------------------------------------------------------------------

Rectangle * LayoutCalculator::CreateRectangleForVertex (void){
	Rectangle * rect = new Rectangle(	
								CoordTPoint(0,0), 
								GetRectangleDimension(VERTEX),
								lParams->vertexParams.bordersDrawingData.forAbsRerf.bagroundColour,
								lParams->vertexParams.bordersDrawingData.forAbsRerf.borderColour,
								lParams->vertexParams.bordersDrawingData.forAbsRerf.borderWidth
							);
	verticesPrimitives.push_back(rect);
	return rect;
}

//-----------------------------------------------------------------------

SlotRenderingInfo * LayoutCalculator::CreateSlotRenderingInfo (
		Vertex * v,
		VertexRenderingInfo * vertexRendInfo,
		SlotDebugInfo * slotDebugInfo
	) {
	assert(v && slotDebugInfo && vertexRendInfo);

	Rectangle * keyRect					= CreateRectangleForSlotKey(vertexRendInfo);
	Rectangle * contentRect				= CreateRectangleForSlotContent(keyRect);
	Rectangle * keyExpandButtonRect		= (Rectangle *)0;
	Rectangle * contentExpandButtonRect = (Rectangle *)0;

	if (slotDebugInfo->GetKeyTargetVertex())
		keyExpandButtonRect = CreateRectangleForSlotExpandButton(true, keyRect);

	if (slotDebugInfo->GetContentTargetVertex())
		contentExpandButtonRect = CreateRectangleForSlotExpandButton(false, keyRect);

	return new SlotRenderingInfo(	
					nextSlotRenInfoId++,
					v,
					slotDebugInfo,
					CreateTextForSlot(true, keyRect, slotDebugInfo),
					keyRect,
					keyExpandButtonRect,
					CreateTextForSlot(false, contentRect, slotDebugInfo),
					contentRect,
					contentExpandButtonRect
				);
}

//-----------------------------------------------------------------------

Rectangle * LayoutCalculator::CreateRectangleForSlotKey (const VertexRenderingInfo * vertexRendInfo) {
	assert(vertexRendInfo);
	CoordTPoint pos;

	if(vertexRendInfo->GetAllSlostsRenderingInfo().empty()) {
		pos.SetX(vertexRendInfo->GetX());
		pos.SetY(vertexRendInfo->GetY() + vertexRendInfo->GetHeight());
	}
	else {
		SlotRenderingInfo * lastSlot = vertexRendInfo->GetAllSlostsRenderingInfo().back();
		pos.SetX(lastSlot->GetKeyRectangle()->GetX() );	//equivalent to take lastSlot->GetContentRectangle()->GetX()
		pos.SetY(lastSlot->GetKeyRectangle()->GetY() + lastSlot->GetKeyRectangle()->GetHeight());
	}

	Rectangle * rect = new Rectangle(pos, GetRectangleDimension(SLOT));

	verticesPrimitives.push_back(rect);
	return rect;
}

//-----------------------------------------------------------------------
//caller is responsible to delete the instance
Rectangle * LayoutCalculator::CreateRectangleForSlotContent (const Rectangle * slotKeyRect) {
	assert(slotKeyRect);
	CoordTPoint pos;

	pos.SetX(slotKeyRect->GetX() + slotKeyRect->GetWidth());
	pos.SetY(slotKeyRect->GetY());

	Rectangle * rect = new Rectangle(	
								pos,
								GetRectangleDimension(SLOT),
								lParams->vertexParams.bordersDrawingData.forSlotContent.bagroundColour,
								lParams->vertexParams.bordersDrawingData.forSlotContent.borderColour,
								lParams->vertexParams.bordersDrawingData.forSlotContent.borderWidth
							);

	verticesPrimitives.push_back(rect);
	return rect;
}

//-----------------------------------------------------------------------
//caller is responsible to delete the instance
Rectangle * LayoutCalculator::CreateRectangleForBookmark (const CoordTPoint & ownerVertexPos) {
	Dimension dim		= GetRectangleDimension(BOOKMARK);
	Rectangle * rect	= new Rectangle(
								CoordTPoint(
									ownerVertexPos.GetX(), 
									ownerVertexPos.GetY() - dim.GetHeight()
								),
								dim,
								geo::Colour (255, 255, 0)
							);
	bookmarksPrimitives.push_back(rect);
	return rect;
}

//-----------------------------------------------------------------------

Rectangle * LayoutCalculator::CreateRectangleForSlotExpandButton (
				bool isKey,
				const Rectangle * keyRect
	) {
	assert(keyRect);
	CoordTPoint pos;

	if(isKey)
		pos.SetY(keyRect->GetY());
	else 
		pos.SetY(keyRect->GetY() + (keyRect->GetHeight() / 2.0f));
	
	pos.SetX(keyRect->GetX() + (2 * keyRect->GetWidth()));	//*2 == one rect for key and one rect for content
	Rectangle * rect = new Rectangle(	
								pos,
								GetRectangleDimension(EXPAND_BUTTON),
								lParams->vertexParams.bordersDrawingData.forContentExpandButton.bagroundColour,
								lParams->vertexParams.bordersDrawingData.forContentExpandButton.borderColour,
								lParams->vertexParams.bordersDrawingData.forContentExpandButton.borderWidth
							);

	verticesPrimitives.push_back(rect);
	return rect;
}

//-----------------------------------------------------------------------

geo::Colour LayoutCalculator::GetSlotColourByKeyTypeInfo (
		const SlotDebugInfo*	slotDebInfo,
		bool*					isSpecialKeyValue
	){
	assert(slotDebInfo);
	geo::Colour c;
	bool isSpecial = true;

	switch(slotDebInfo->GetKeyTypeInfo()){
		case dinfo::OBJECT_DELEGATION_PSEUDO_KEY:	c = rParams->colours.delegationColour;	break;
		case dinfo::OBJECT_INHERITANCE_PSEUDO_KEY:	c = rParams->colours.inheritanceColour;	break;
		case dinfo::OBJECT_OPERATOR_KEY:			c = rParams->colours.operatorColour;	break;
		case dinfo::OBJECT_ATTRIBUTE_KEY:			c = rParams->colours.attributeColour;	break;
		case dinfo::EXTERNID_USER_PSEUDO_KEY:		c = rParams->colours.externidColour;	break;
		case dinfo::OBJECT_PSEUDO_KEY:			
		case dinfo::AGGREGATE_PSEUDO_KEY:		
		case dinfo::EXTERNID_PSEUDO_KEY:			c = rParams->colours.internalColour;	break;
		default:									c = rParams->colours.defaultColour;		isSpecial = false;
	};
	if (isSpecialKeyValue)
		*isSpecialKeyValue = isSpecial;
	return c;

}

//-----------------------------------------------------------------------

LayoutCalculator::CoordTPoint LayoutCalculator::GetTextPosition(
		const Rectangle* rect, 
		const Dimension & distanceFromRect
	) {
	return	CoordTPoint(
				rect->GetX() + distanceFromRect.GetHeight(),
				rect->GetY() + distanceFromRect.GetWidth()
			);
}

//-----------------------------------------------------------------------
//caller is responsible to delete the instance
Text * LayoutCalculator::CreateTextForSlot (
		const bool			  textForKey,	//create text for kay or for content
		const Rectangle		* container,	//The rect tha will contain the text
		const SlotDebugInfo	* slotDebInfo
	) {
	assert(container && slotDebInfo);

	std::string containerText = TextFitsInRect(	
									textForKey ? slotDebInfo->GetKeyString() : slotDebInfo->GetContentString(), 
									container, 
									lParams->vertexParams.distanceFromRect.GetWidth()
								);

	Text * text = new Text( 
						containerText,
						GetSlotColourByKeyTypeInfo(slotDebInfo),
						GetTextPosition(container, lParams->vertexParams.distanceFromRect)
					);	
	verticesPrimitives.push_back(text);
	return text;
}

//-----------------------------------------------------------------------

Text * LayoutCalculator::CreateAbsoluteReferenceText (const Vertex * v, const Rectangle * vertexRect){
	assert(v && vertexRect);

	VertexToDebugInfoMapping::iterator i = debugInfo->find(const_cast<Vertex *>(v));
	assert(i != debugInfo->end());
	
	std::string absText = TextFitsInRect(	
							i->second.GetAbsolutRef(), 
							vertexRect, 
							lParams->vertexParams.absRefDistance.GetWidth()
						);
	
	Text * text = new Text(	
						absText, 
						rParams->colours.absRefColour,
						GetTextPosition(vertexRect, lParams->vertexParams.absRefDistance)
					); 
	verticesPrimitives.push_back(text);
	return text;

}

//-----------------------------------------------------------------------
//caller is responsible to delete the instance
Text * LayoutCalculator::CreateTextForBookmark (const Rectangle * bookmarkRect) {
	assert(bookmarkRect);

	std::string absText = TextFitsInRect(	
							"Bookmark", 
							bookmarkRect, 
							lParams->vertexParams.distanceFromRect.GetWidth()
						);

	Text * text = new Text(
						absText,
						geo::Colour (),
						GetTextPosition(bookmarkRect, lParams->vertexParams.distanceFromRect)
					);
	bookmarksPrimitives.push_back(text);
	return text;
}

//-----------------------------------------------------------------------

std::string LayoutCalculator::TextFitsInRect (
		const std::string&	text, 
		const Rectangle*	rect, 
		const length_t&		seperation
	) {
	std::string newText;
	length_t maxWidth = rect->GetWidth() - (2 * seperation);
	
	if (lParams->primitives->GetTextWidth(text) <= maxWidth)
		newText = text;
	else {
		int max_number_of_cahrs = (int)(maxWidth / lParams->primitives->GetCharWidth());
		newText.append(text, 0, max_number_of_cahrs-3);	//3 is magic number for the three dots that we add later
		newText.append("...");
	}
	return newText;
}

//-----------------------------------------------------------------------

void LayoutCalculator::RemoveContentsOfVertex (VertexRenderingInfo * vertex) {
	assert(vertex);

	existsDirtyList = true;
	PrimitivePtrList & vertexPrimitives = vertex->GetVertexPrimitives();

	for(PrimitivePtrList::iterator i = vertexPrimitives.begin(); i != vertexPrimitives.end(); ++i)
		verticesPrimitives.remove(*i);

	vertex->RemoveAllTheSlots();

	//Add the new primitives of vertex into vertexPrimitives list
	vertexPrimitives = vertex->GetVertexPrimitives();
	for(PrimitivePtrList::iterator i = vertexPrimitives.begin(); i != vertexPrimitives.end(); ++i)
		verticesPrimitives.push_back((*i));
}

//-----------------------------------------------------------------------

void LayoutCalculator::CreatePrimitiveList (void) {
	existsDirtyList = false;

	if (!primitives.empty())
		primitives.clear();

	primitives.resize(
		edgesPrimitives.size()		+
		layersPrimitives.size()		+
		verticesPrimitives.size()	+
		bookmarksPrimitives.size()
	);

	PrimitivePtrList::iterator result;
	result = std::copy (layersPrimitives.begin(), layersPrimitives.end(), primitives.begin());
	result = std::copy (bookmarksPrimitives.begin(), bookmarksPrimitives.end(), result);
	result = std::copy (verticesPrimitives.begin(), verticesPrimitives.end(), result);
	result = std::copy (edgesPrimitives.begin(), edgesPrimitives.end(), result);

	assert(result == primitives.end());
}

//-----------------------------------------------------------------------

LayoutCalculator::Dimension LayoutCalculator::GetRectangleDimension (RectangleType rectType) {
	length_t heightOfRect = 0, widthOfRect	= 0;

	if (rectType == LAYER)
		heightOfRect = lParams->layerParams.distanceFromLayer.GetHeight() * 2;
	else if (rectType == VERTEX)
		heightOfRect = lParams->primitives->GetCharHeight() + 
					   (2 * lParams->vertexParams.absRefDistance.GetWidth());
	else
		heightOfRect = lParams->primitives->GetCharHeight() + 
					   (2 * lParams->vertexParams.distanceFromRect.GetWidth()) ;


	switch(rectType) {
		case SLOT	 : widthOfRect = lParams->vertexParams.width / 2.0f;	break;
		case VERTEX	 : widthOfRect = lParams->vertexParams.width;			break;
		case BOOKMARK: widthOfRect = lParams->vertexParams.bookmarkWidth;	break;
		case LAYER	 : widthOfRect = (lParams->layerParams.distanceFromLayer.GetWidth() * 2) +
									 lParams->vertexParams.width;
						break;
		default:	//EXPAND_BUTTON
			assert(rectType == EXPAND_BUTTON);
			widthOfRect	 = lParams->vertexParams.expandButtonWidth;
			heightOfRect = heightOfRect / 2.0f;
	}

	return Dimension(widthOfRect, heightOfRect);
}

//-----------------------------------------------------------------------

bool LayoutCalculator::ExistsLayerWithInnerEdge (void) {
	return std::find(
				hasLayerInnerEdgesVec.begin(),
				hasLayerInnerEdgesVec.end(),
				true
			) != hasLayerInnerEdgesVec.end();
}

//-----------------------------------------------------------------------

void LayoutCalculator::InitHasLayersInnerEdges(const LayersRenderingInfo & layers) {
	hasLayerInnerEdgesVec.resize(graphL->TotalNumberOfLayers());
	
	for (size_t i = 0; i < hasLayerInnerEdgesVec.size(); ++i)
		hasLayerInnerEdgesVec[i] = false;
}

}	//namespace iviews