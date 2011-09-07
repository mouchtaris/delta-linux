/**
 *	HighlightCurrentVertex.h
 *
 *	Highlight the vertex that pointetd from the muse
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	July 2010
 */

#ifndef	HIGHLIGHT_CURRENT_VERTEX_H
#define HIGHLIGHT_CURRENT_VERTEX_H

#include <list>
#include <string>
#include <utility>


#include "AddOn.h"
#include "Colour.h"
#include "Vertex.h"
#include "SlotDebugInfo.h"
#include "TypesConverter.h"
#include "ColourAddOnData.h"
#include "SlotRenderingInfo.h"
#include "VertexRenderingInfo.h"




namespace iviews {

class HighlightCurrentVertex : public AddOn {
public:
	typedef geo::Colour	Colour;

	
	HighlightCurrentVertex (
		const Colour & vertexColour	= Colour(),
		const Colour & slotColour	= Colour()
	);

	void				SetVertexColour (const Colour & c);
	const Colour &		GetVertexColour (void) const;

	void				SetSlotColour (const Colour & c);
	const Colour &		GetSlotColour (void) const;


	virtual void		Undo (LayersRenderingInfo * layers, ColourAddOnUndoData * undoData);
	virtual bool		CanAddInManager (const ColourAddOnData * data);
	virtual void		RemovedFromManager (void);
	virtual bool		CanRemoveFromManager (const ColourAddOnData * data);
			
	//if the return value is not null then caller is responsible to delete the pointer
	virtual ColourAddOnUndoData * 
						Apply (LayersRenderingInfo * layers, ColourAddOnData * data);


private:
	typedef graphs::Vertex				Vertex;
	typedef dinfo::SlotDebugInfo		SlotDebugInfo;
	typedef SlotRenderingInfo::SlotRenderingInfoPtrList 
										SlotRenderingInfoPtrList;
	typedef SlotDebugInfoUndoRenderingData::SlotDebugInfoUndoRenderingDataList		
										SlotDebugInfoUndoRenderingDataList;

	void				HighlightCurrentSlot (
							VertexRenderingInfo * vertex, 
							HighlightCurrentVertexAddOnData * data
						);
	

	Colour				slotColour;
	Colour				vertexColour;
	Vertex*				oldVertex;		//old selected vertex
	SlotRenderingInfo*	oldSlot;		//old selected slot
	VertexRenderingInfo*oldVertexRenderingInfo;
	
};


}	//namespace iviews


#endif	//HIGHLIGHT_CURRENT_VERTEX_H