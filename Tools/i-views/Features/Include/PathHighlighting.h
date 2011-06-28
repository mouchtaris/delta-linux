//july 2010
#ifndef	PATH_HIGHLIGHTING_H
#define	PATH_HIGHLIGHTING_H


#include <set>
#include <list>
#include <utility>

#include "AddOn.h"
#include "Colour.h"
#include "ColourAddOnData.h"
#include "EdgeRenderingInfo.h"
#include "VertexRenderingInfo.h"
#include "LayersRenderingInfo.h"


//new


namespace iviews {
	
////////////////////////////////////////////////////////////////////////

enum HighlightingDirection {
	BOTH		= 0,	//do not change the order of enums
	INCOMING	= 1,
	OUTGOING	= 2 
};


////////////////////////////////////////////////////////////////////////

class PathHighlighting : public AddOn {

public:
	typedef geo::Colour			Colour;

	PathHighlighting (
		const Colour & incoming = Colour(), 
		const Colour & outgoing = Colour()
	);


	void				SetOutgoingEdgesColour (const Colour & c);
	const Colour &		GetOutgoingEdgesColour (void) const;

	void				SetIncomingEdgesColour (const Colour & c);
	const Colour &		GetIncomingEdgesColour (void) const;

	virtual void		Undo (LayersRenderingInfo * layers, ColourAddOnUndoData * undoData);
	virtual bool		CanAddInManager (const ColourAddOnData * data);
	virtual void		RemovedFromManager (void);
	virtual bool		CanRemoveFromManager (const ColourAddOnData * data);
			
	//if the return value is not null then caller is responsible to delete the pointer
	virtual ColourAddOnUndoData * 
						Apply (LayersRenderingInfo * layers, ColourAddOnData * data);
private:
	typedef std::set<id_t>											IdsSet;
	typedef EdgeRenderingInfo::EdgeRenderingInfoPtrList				EdgeRenderingInfoPtrList;
	typedef PathHighlightingUndoData::UndoDataStruct				UndoDataStruct;
	typedef PathHighlightingUndoData::PathHighlightingUndoDataMap	PathHighlightingUndoDataMap;
	typedef PathHighlightingUndoData::EdgeRenderingUndoDataList		EdgeRenderingUndoDataList;
	typedef SlotDebugInfoUndoRenderingData
			::SlotDebugInfoUndoRenderingDataList					SlotDebugInfoUndoRenderingDataList;
	

	void HighlightIncomingEdges (
			VertexRenderingInfo		 * mainVertex,
			PathHighlightingUndoData & undoData
		);

	void HighlightOutgoingEdges (
			VertexRenderingInfo		 * mainVertex,	
			PathHighlightingUndoData & undoData
		);

	void SetEdgesColourIfExists (
			VertexRenderingInfo				*	vertex,
			const EdgeRenderingUndoDataList	&	edgeList,
			bool								isIncoming
		);

	void SetOutgoingEdgesColourIfExists (
			VertexRenderingInfo				* vertex,
			const EdgeRenderingUndoDataList	& incoming
		);

	void SetIncomingEdgesColourIfExists (
			VertexRenderingInfo				* vertex,
			const EdgeRenderingUndoDataList	& incoming
		);

	void SetEdgeColour (
			EdgeRenderingInfo			* edge, 
			EdgeRenderingUndoDataList	& edges, 
			const Colour				& colour
		);

	void SetSourceVertexSlotColour (
			EdgeRenderingInfo			* edge, 
			PathHighlightingUndoData	& undoData, 
			const Colour				& colour
		);

	void SetVertexSlotsColoursIfExists (
			VertexRenderingInfo						 * vertex,
			const SlotDebugInfoUndoRenderingDataList & slots
		);

	bool	highlightInvolvedSlots;
	IdsSet	visitedVerticesFromIncomingEdges;
	IdsSet	visitedVerticesFromOutgoingEdges;
	Colour	outgoingColour;
	Colour	incomingColour;


};

}	//namespace iviews


#endif	PATH_HIGHLIGHTING_H