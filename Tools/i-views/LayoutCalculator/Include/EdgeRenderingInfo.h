/**
 *	EdgeRenderingInfo.h
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	June 2010
 */

#ifndef EDGE_RENDERING_INFO_H
#define EDGE_RENDERING_INFO_H

#include <list>
#include <vector>
#include <string>

#include "Text.h"
#include "Arrow.h"
#include "CommonTypes.h"

namespace iviews {

class SlotRenderingInfo;
class VertexRenderingInfo;

class EdgeRenderingInfo {
public:
	///////////////////////////////////////////////////
	//	public typedef(s)		
	typedef std::list<EdgeRenderingInfo *>		EdgeRenderingInfoPtrList;

	///---- Constractor (s)
	EdgeRenderingInfo (
		const id_t &			id, 
		Arrow*					arrow, 
		VertexRenderingInfo*	source,
		VertexRenderingInfo*	target,
		SlotRenderingInfo*		sourceSlot = static_cast<SlotRenderingInfo *>(0),
		Text*					text = static_cast<Text *>(0)
	);


	///////////////////////////////////////////////////
	//	public methods

	id_t					Id (void) const;
	std::string				GetIdRelevantToVerticesAbsRef (void) const;

	bool					IsBackEdge (void) const;
	bool					IsFrontEdge (void) const;
	bool					IsInnerEdge (void) const;

	void					SetText (Text * text);
	Text *					GetText (void);
	const Text *			GetText (void) const;

	void					SetArrow (Arrow * arrow);
	Arrow *					GetArrow (void);
	const Arrow *			GetArrow (void) const;
	
	void					Highlight (bool b);
	bool					IsHighlighted (void) const;

	void					SetSoruceSlotRenderingInfo (SlotRenderingInfo * sourceSlot_);
	SlotRenderingInfo*		GetSourceSlotRenderingInfo (void) const;
	VertexRenderingInfo*	GetSourceVertexRenderingInfo (void) const;
	VertexRenderingInfo*	GetTargetVertexRenderingInfo (void) const;

private:
	///////////////////////////////////////////////////
	//	private members
	id_t					id;	
	std::string				stringId;
	bool					isHighlighted;
	Text*					text;
	Arrow*					arrow;
	SlotRenderingInfo*	 	sourceSlot;
	VertexRenderingInfo* 	source;
	VertexRenderingInfo* 	target;

};

}	//namespace iviews


#endif	//EDGE_RENDERING_INFO_H