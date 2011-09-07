/**
 *	PathHighlighting.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	July 2010
 */

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif

#include <assert.h>

#include "Arrow.h"
#include "PathHighlighting.h"
#include "SetEdgeOriginSlotColour.h"
#include "ViewSlotTextualContents.h"
#include "RenderingInfoAlgorithmsMacros.h"


namespace iviews {

//-----------------------------------------------------------------------

PathHighlighting::PathHighlighting(const Colour & incoming, const Colour & outgoing)
:	incomingColour(incoming),
	outgoingColour(outgoing)
{}

//-----------------------------------------------------------------------
//if the return value is not null then caller is responsible to delete the pointer
ColourAddOnUndoData * PathHighlighting::Apply(
		LayersRenderingInfo * layers, 
		ColourAddOnData * data
	) {
	assert(layers && data && data->GetVertex());

	visitedVerticesFromIncomingEdges.clear();
	visitedVerticesFromOutgoingEdges.clear();

	PathHighlightingAddOnData * pathData	= dynamic_cast<PathHighlightingAddOnData*>(data);
	assert(pathData);
	VertexRenderingInfo *		vertex		= layers->GetVertexRenderingInfo(pathData->GetVertex());

	if (!vertex)
		return (HighlightRelativeSlotsUndoData *)0;

	highlightInvolvedSlots					= pathData->HighlightInvolvedSlots();
	HighlightingDirection		direction	= pathData->GetHighlightingDirection();
	PathHighlightingUndoData *	undoData	= new PathHighlightingUndoData();

	if (direction == INCOMING || direction == BOTH)
		HighlightIncomingEdges(vertex, *undoData);
	
	if (direction == OUTGOING || direction == BOTH)
		HighlightOutgoingEdges(vertex, *undoData);

	if (!undoData->undoDataMap.size()) {
		delete undoData;
		undoData = (PathHighlightingUndoData *)0;
	}

	return undoData;
}

//-----------------------------------------------------------------------

void PathHighlighting::Undo(LayersRenderingInfo * layers, ColourAddOnUndoData * undoData) {
	assert(layers);
	if (!undoData)
		return;
	PathHighlightingUndoData * undo	= dynamic_cast<PathHighlightingUndoData *>(undoData);
	assert(undo);

	PathHighlightingUndoDataMap::iterator i;
	for(i = undo->undoDataMap.begin(); i != undo->undoDataMap.end(); ++i)
		if (VertexRenderingInfo * vertex = layers->GetVertexRenderingInfo(i->first)) {
			SetVertexSlotsColoursIfExists(vertex, i->second.slots);
			SetIncomingEdgesColourIfExists(vertex, i->second.incomintEdges);
			SetOutgoingEdgesColourIfExists(vertex, i->second.outgoingEdges);
		}
}

//-----------------------------------------------------------------------

void PathHighlighting::SetOutgoingEdgesColour(const Colour & c)
	{ outgoingColour = c; }

//-----------------------------------------------------------------------

const PathHighlighting::Colour & PathHighlighting::GetOutgoingEdgesColour(void) const
	{ return outgoingColour; }

//-----------------------------------------------------------------------

void PathHighlighting::SetIncomingEdgesColour(const Colour & c)
	{ incomingColour = c; }

//-----------------------------------------------------------------------

const PathHighlighting::Colour & PathHighlighting::GetIncomingEdgesColour(void) const
	{ return incomingColour; }	

//-----------------------------------------------------------------------

bool PathHighlighting::CanAddInManager(const ColourAddOnData * data) 
	{ return true; }

//-----------------------------------------------------------------------

void PathHighlighting::RemovedFromManager(void) 
	{}

//-----------------------------------------------------------------------

bool PathHighlighting::CanRemoveFromManager(const ColourAddOnData * data) 
	{ return false; }
	

////////////////////////////////////////////////
//	private method implementation

#define FIND_OR_INSERT_VERTEX_IN_MAP(VERTEX, MAP, ITERATOR)	\
	if (true) {												\
		assert(VERTEX);										\
		ITERATOR = MAP.find(VERTEX);						\
		if (ITERATOR == MAP.end())							\
			ITERATOR = MAP.insert(							\
					std::make_pair(							\
						VERTEX,								\
						UndoDataStruct()					\
					)										\
				).first;									\
	}														\
	else


#define HIGHLIGHT_EDGES(FUNC, VISITED_VERTICES, IS_INCOMING, COLOUR )								\
	if (true) {																						\
		assert(mainVertex);																			\
		EdgeRenderingInfoPtrList &																	\
		edgeList = IS_INCOMING	? mainVertex->GetAllIncomingEdgesRenderingInfo()					\
								: mainVertex->GetAllOutgoingEdgesRenderingInfo();					\
																									\
		if (!edgeList.size())																		\
			return;																					\
																									\
		PathHighlightingUndoDataMap::iterator i;													\
		FIND_OR_INSERT_VERTEX_IN_MAP(																\
			mainVertex->GetSourceVertex(),															\
			undoData.undoDataMap,																	\
			i																						\
		);																							\
		FOR_ALL_EDGES(edge, edgeList) {																\
			(*edge)->Highlight(true);																\
			VertexRenderingInfo * source = IS_INCOMING	? (*edge)->GetSourceVertexRenderingInfo()	\
														: (*edge)->GetTargetVertexRenderingInfo();	\
			assert(source);																			\
																									\
			SetEdgeColour(																			\
				*edge,																				\
				IS_INCOMING ? i->second.incomintEdges												\
							: i->second.outgoingEdges,												\
				COLOUR																				\
			);																						\
			if (highlightInvolvedSlots			&&													\
				mainVertex->HasFullContents()	&&													\
				!mainVertex->HasHiddenContents())													\
				SetSourceVertexSlotColour(*edge, undoData, COLOUR);									\
																									\
			if (VISITED_VERTICES.find(source->Id()) ==  VISITED_VERTICES.end()) {					\
				VISITED_VERTICES.insert(source->Id());												\
				FUNC(source, undoData);																\
			}																						\
		}																							\
	}																								\
	else


//-----------------------------------------------------------------------

void PathHighlighting::HighlightIncomingEdges(
		VertexRenderingInfo * mainVertex,
		PathHighlightingUndoData & undoData
	) {
	HIGHLIGHT_EDGES(
		HighlightIncomingEdges,
		visitedVerticesFromIncomingEdges,
		true,
		incomingColour
	);
}							

//-----------------------------------------------------------------------

void PathHighlighting::HighlightOutgoingEdges(
		VertexRenderingInfo * mainVertex,
		PathHighlightingUndoData & undoData
	) {
	HIGHLIGHT_EDGES(
		HighlightOutgoingEdges,
		visitedVerticesFromOutgoingEdges,
		false,
		outgoingColour
	);
}

//-----------------------------------------------------------------------

void PathHighlighting::SetEdgeColour(
		EdgeRenderingInfo			* edge, 
		EdgeRenderingUndoDataList	& edges,
		const Colour				& colour
	){
	assert(edge);

	edges.push_back(
		EdgeUndoRenderingData(
			edge->GetIdRelevantToVerticesAbsRef(),
			edge->GetArrow()->GetColour()	
		)
	);
	edge->GetArrow()->SetColour(colour);
}

//-----------------------------------------------------------------------

void PathHighlighting::SetSourceVertexSlotColour(
		EdgeRenderingInfo			* edge, 
		PathHighlightingUndoData	& undoData, 
		const Colour				& colour
	) {
	assert(edge);
	PathHighlightingUndoDataMap::iterator i;
	FIND_OR_INSERT_VERTEX_IN_MAP(
		edge->GetSourceSlotRenderingInfo()->GetSourceVertex(),
		undoData.undoDataMap,
		i
	);
	i->second.slots.push_back(SetEdgeOriginSlotColour()(edge, colour));
}

//-----------------------------------------------------------------------

void PathHighlighting::SetVertexSlotsColoursIfExists(
		VertexRenderingInfo * vertex,
		const SlotDebugInfoUndoRenderingDataList & slots
	) {
	assert(vertex);

	if (!vertex->HasFullContents()) {
		assert(vertex->HasHiddenContents()); //when we do not have contents it is impossible to have hidden contents
		return;
	}

	SlotDebugInfoUndoRenderingDataList::const_iterator undoSlotData;
	for(undoSlotData = slots.begin(); undoSlotData != slots.end(); ++undoSlotData) {
		SlotRenderingInfo * slot = vertex->GetSlotRenderingInfo((*undoSlotData).id);
		assert(slot);

		slot->GetKeyRectangle()->SetBagroundColour((*undoSlotData).keyColour);
		slot->GetContentRectangle()->SetBagroundColour((*undoSlotData).contentColour);
	}
}

//-----------------------------------------------------------------------

void PathHighlighting::SetOutgoingEdgesColourIfExists(
		VertexRenderingInfo				* vertex,
		const EdgeRenderingUndoDataList	& outgoing
	)
	{ SetEdgesColourIfExists(vertex, outgoing, false); }

//-----------------------------------------------------------------------

void PathHighlighting::SetIncomingEdgesColourIfExists(
		VertexRenderingInfo				* vertex,
		const EdgeRenderingUndoDataList	& incoming
	)
	{ SetEdgesColourIfExists(vertex, incoming, true); }

//-----------------------------------------------------------------------
//Is more efficient to iterate first the small list and after the big.
void PathHighlighting::SetEdgesColourIfExists(
		VertexRenderingInfo				*	vertex,
		const EdgeRenderingUndoDataList	&	edgeList,
		bool								isIncoming
	) {
	assert(vertex);

	EdgeRenderingUndoDataList::const_iterator undoEdge;
	for(undoEdge = edgeList.begin(); undoEdge != edgeList.end(); ++undoEdge) {
		EdgeRenderingInfoPtrList & edgesRenInfo = isIncoming ? vertex->GetAllIncomingEdgesRenderingInfo()
															 : vertex->GetAllOutgoingEdgesRenderingInfo();
					
		FOR_ALL_EDGES (undoEdgeRenInfo, edgesRenInfo)
			if ((*undoEdgeRenInfo)->GetIdRelevantToVerticesAbsRef() == (*undoEdge).edgeStringId &&
				(*undoEdgeRenInfo)->IsHighlighted()) {
				(*undoEdgeRenInfo)->Highlight(false);
				(*undoEdgeRenInfo)->GetArrow()->SetColour((*undoEdge).edgeUndoColour);
				break;
			}
	}
}


}	//namespace iviews