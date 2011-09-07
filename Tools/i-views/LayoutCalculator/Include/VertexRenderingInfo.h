/**
 *	VertexRenderingInfo.h
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	June 2010
 */

#ifndef VERTEX_RENDERING_INFO
#define VERTEX_RENDERING_INFO

#include <map>
#include <list>
#include <utility>

#include "Text.h"
#include "Point.h"
#include "Vertex.h"
#include "Dimension.h"
#include "Rectangle.h"
#include "Primitive.h"
#include "CommonTypes.h"
#include "GraphLayers.h"
#include "SlotDebugInfo.h"
#include "RenderingTypes.h"
#include "EdgesParameters.h"
#include "SlotRenderingInfo.h"
#include "EdgeRenderingInfo.h"
#include "BookmarkRenderingInfo.h"

namespace iviews {

class VertexRenderingInfo {
public:
	typedef graphs::GraphLayers::Layer					Layer;
	typedef geo::Colour									Colour;
	typedef geo::Point<coord_t>							CoordTPoint;
	typedef Primitive::PrimitivePtrList					PrimitivePtrList;
	typedef SlotRenderingInfo::SlotRenderingInfoPtrList SlotRenderingInfoPtrList;
	typedef EdgeRenderingInfo::EdgeRenderingInfoPtrList EdgeRenderingInfoPtrList;
	typedef std::list<VertexRenderingInfo *>			VertexRenderingInfoPtrList;
	
	typedef std::map<
				dinfo::SlotDebugInfo	*, 
				SlotRenderingInfo		*, 
				dinfo::SlotDebugInfo::SlotDebugInfoComparatorFunctor
			>											Slots;


	~VertexRenderingInfo ();
	VertexRenderingInfo (
		const id_t&		id,
		const Layer		sourceLayer,	
		Text*			absRefText, 
		Rectangle*		mainRect, 
		graphs::Vertex* source
	);
	



	id_t				Id (void) const;
	Layer				GetSourceLayer (void) const;
	bool				HasSpliceArrow (void) const;
	length_t			CalcMaxMinorRadius (void) const;

	bool				HasEdgesCommonStart (void) const;
	void				SetEdgesCommonStart (bool b);
		
	bool				HasHiddenContents (void) const;
	void				HideContents (bool hide = true);

	bool				HasFullContents (void) const;
	void				FullContents (bool b = true);
	void 				AddInEdgeRenderingInfo (EdgeRenderingInfo * in);
	void 				AddOutEdgeRenderingInfo (EdgeRenderingInfo * out);

	//Update all contetns' pos.
	void				SetPosition (const coord_t & x, const coord_t & y);
	void				SetPosition (const CoordTPoint & newPos);

	const Colour &		GetBackgroundColour (void) const;
	void				SetBackgroundColour (const Colour & c);
	void				SetAbosloutRefBackgroundColour (const Colour &c);

	void 				AddSlot (dinfo::SlotDebugInfo * sdi, SlotRenderingInfo * sri);
	void				RemoveAllTheSlots (void);
	const Slots &		GetAllTheSlots (void) const;
	
	SlotRenderingInfo * GetSlotRenderingInfo (const id_t & slotId);
	SlotRenderingInfo * GetSlotRenderingInfo (const std::string & slotId);
	SlotRenderingInfo * GetSlotRenderingInfo (dinfo::SlotDebugInfo * sdi);

	EdgesParameters &	GetEdgesParameters (void);
	const EdgesParameters &
						GetEdgesParameters (void) const;
		
	void				SetVertexMainRectangle (Rectangle * rect);
	Rectangle*			GetVertexMainRectangle (void) const;
	Text*				GetAbsRefText (void) const;		
	graphs::Vertex*		GetSourceVertex (void) const;
	const CoordTPoint&	GetPosition (void) const;
	length_t			GetWidth (void) const;
	length_t			GetHeight (void) const;
	coord_t				GetX (void) const;
	coord_t				GetY (void) const;

	/////////// Bookmarks /////////////
	bool				HasBookmark (void) const;
	void				AddBookmark (BookmarkRenderingInfo * bookmark);
	void				RemoveBookmark (void);
	BookmarkRenderingInfo *	
						GetBookmark (void) const;
	/////////// Bookmarks /////////////

	const geo::Dimension<length_t> &
						GetStartVertexMainDimension (void) const;

	const geo::Dimension<length_t> &
						GetVertexMainDimension (void) const;

	PrimitivePtrList				&	
						GetVertexPrimitives (void);
	const PrimitivePtrList			&	
						GetVertexPrimitives (void) const;
	EdgeRenderingInfoPtrList		&	
						GetAllIncomingEdgesRenderingInfo (void);
	const EdgeRenderingInfoPtrList	&	
						GetAllIncomingEdgesRenderingInfo (void) const;
	EdgeRenderingInfoPtrList		&	
						GetAllOutgoingEdgesRenderingInfo (void);
	const EdgeRenderingInfoPtrList	&	
						GetAllOutgoingEdgesRenderingInfo (void) const;
	SlotRenderingInfoPtrList		&	
						GetAllSlostsRenderingInfo (void);
	const SlotRenderingInfoPtrList	&	
						GetAllSlostsRenderingInfo (void) const;
	


	bool	operator != (const VertexRenderingInfo * v);

	bool	operator == (const VertexRenderingInfo * v);
	
private:
	void				AddEdgeInList (EdgeRenderingInfo * edge, EdgeRenderingInfoPtrList & list);
	void				AddPrimitive (Primitive * p);
	void				DeleteSlot (SlotRenderingInfo * slot);


	id_t						id;
	Layer						sourceLayer;
	Slots						slots;
	bool						hasFullContents;
	bool						hasBookmark;
	bool						hasSpliceArrow;
	bool						hasEdgesCommonStart;
	bool						hasHiddenContents;
	EdgesParameters				edgesParams;

	
	Text*						absRefText;
	Rectangle*					mainRect;
	graphs::Vertex*				source;
	BookmarkRenderingInfo*		bookmark;

	geo::Dimension<length_t> 	startVertexMainDimension;
	PrimitivePtrList		 	primitives;
	SlotRenderingInfoPtrList 	slotsRenInfo;

	EdgeRenderingInfoPtrList 	inEdges;
	EdgeRenderingInfoPtrList 	outEdges;
};



}	//namespace iviews

#endif	//NODE_RENDERING_INFO