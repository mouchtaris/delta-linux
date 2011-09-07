/**
 *	ColourAddOnData.h
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	Sempteber 2010
 */

#ifndef	COLOUR_ADDON_DATA_H
#define COLOUR_ADDON_DATA_H


#include <map>
#include <list>
#include <string>
#include <utility>
#include "Colour.h"
#include "Vertex.h"
#include "Functional.h"
#include "CommonTypes.h"
#include "RenderingTypes.h"
#include "VertexRenderingInfo.h"




namespace iviews {


	////////////////////////////////////////////////////////////////////////////

	class ColourAddOnData {
	public:
		ColourAddOnData (graphs::Vertex * vertex_ = (graphs::Vertex *)0) { vertex = vertex_; }

		void				SetVertex (graphs::Vertex * vertex_) {vertex_ = vertex; }
		graphs::Vertex *	GetVertex (void) const { return vertex; }

		virtual ~ColourAddOnData () {}

	protected:
		graphs::Vertex * vertex;
	};

	////////////////////////////////////////////////////////////////////////////

	class FindAddOnData : public ColourAddOnData {
	public:
		FindAddOnData (
			graphs::Vertex *	vertex,
			const bool			isKey_,
			const int			slotIndex_
		) :	ColourAddOnData (vertex),
			isKey			(isKey_),
			slotIndex		(slotIndex_)
		{}
	

		int	 GetSlotIndex (void) const { return slotIndex; }
		bool IsKey (void) const { return isKey; }
		 

	private:
		bool isKey;
		int	 slotIndex;
	};

	////////////////////////////////////////////////////////////////////////////

	class HighlightCurrentVertexAddOnData : public ColourAddOnData {
	public:
		HighlightCurrentVertexAddOnData (
			graphs::Vertex * vertex_,
			const coord_t & x_,
			const coord_t & y_
		) : ColourAddOnData (vertex_),
			x (x_),
			y (y_)
		{}


		coord_t GetX (void) { return x; }
		coord_t GetY (void) { return y; }

	private:
		coord_t x;
		coord_t y;

	};


	////////////////////////////////////////////////////////////////////////////

	
	class PathHighlighting;
	enum HighlightingDirection;

	class PathHighlightingAddOnData : public ColourAddOnData {
	public:		
		PathHighlightingAddOnData (
			HighlightingDirection direction_,
			graphs::Vertex * vertex_			= (graphs::Vertex *)0, 
			bool highlightInvolvedSlots			= false
		) :	ColourAddOnData (vertex_),
			direction		(direction_),
			highlightSlots	(highlightInvolvedSlots)
		{}

		
		bool HighlightInvolvedSlots (void) const { return highlightSlots; }
		void HighlightInvolvedSlots (const bool b) { highlightSlots = b; }

		HighlightingDirection	GetHighlightingDirection (void) const { return direction; }
		void					SetHighlightingDirection (const HighlightingDirection d) { direction = d; }

	private:
		bool highlightSlots;
		HighlightingDirection direction;
	};



	////////////////////////////////////////////////////////////////////////////

	struct ColourAddOnUndoData {
		virtual ~ColourAddOnUndoData (){}
	};

	////////////////////////////////////////////////////////////////////////////

	struct SlotDebugInfoUndoRenderingData {
		typedef std::list<SlotDebugInfoUndoRenderingData> SlotDebugInfoUndoRenderingDataList;

		std::string id;			//SlotDebugInfo's stringfield id
		geo::Colour keyColour;		
		geo::Colour contentColour;	

		SlotDebugInfoUndoRenderingData (
			const std::string & id_				= "", 
			const geo::Colour & keyColour_		= geo::Colour(), 
			const geo::Colour & contentColour_  = geo::Colour()
		) : id				(id_),
			keyColour		(keyColour_),
			contentColour	(contentColour_)
		{}
	};


	////////////////////////////////////////////////////////////////////////////

	struct EdgeUndoRenderingData {
		std::string edgeStringId;
		geo::Colour	edgeUndoColour;	
		

		EdgeUndoRenderingData (
			const std::string & edgeStringId_	= "",
			const geo::Colour & edgeUndoColour_	= geo::Colour()	
		) : edgeStringId	(edgeStringId_),
			edgeUndoColour	(edgeUndoColour_)
		{}
	};


	////////////////////////////////////////////////////////////////////////////

	struct FindUndoData : public ColourAddOnUndoData {
		bool			 hasFullContents;	//has vertex full contes?
		bool			 isKey;				//Slot's colour is for the key or for the conent
		bool			 isAbsolutRef;		//If the text is absolut refrence area
		std::string		 id;				//SlotDebugInfo's stringfield id
		geo::Colour		 oldColour;
		graphs::Vertex * source;
		

		FindUndoData (
			graphs::Vertex *	source_,
			const geo::Colour &	oldColour_,
			const bool			hasFullContents_,
			const bool			isAbsolutRef_,
			const bool			isKey_,
			const std::string &	id_
		) :	source			(source_),
			oldColour		(oldColour_),
			hasFullContents (hasFullContents_),
			isAbsolutRef	(isAbsolutRef_),
			isKey			(isKey_),
			id				(id_)
		{ 
			assert(
				(isAbsolutRef && id.empty()) ||
				(!isAbsolutRef && !id.empty()) 
			);
		}
	};


	////////////////////////////////////////////////////////////////////////////

	struct HighlightCurrentVertexUndoData : public ColourAddOnUndoData {
		typedef SlotDebugInfoUndoRenderingData
				::SlotDebugInfoUndoRenderingDataList SlotDebugInfoUndoRenderingDataList;
		
		geo::Colour							mainRectColour;
		graphs::Vertex *					source;
		SlotDebugInfoUndoRenderingDataList	slotsColours; 
	};

	////////////////////////////////////////////////////////////////////////////

	struct HighlightRelativeSlotsUndoData : public ColourAddOnUndoData {
		typedef std::pair<
					graphs::Vertex *,				//vertex that conteins slot
					SlotDebugInfoUndoRenderingData	//old slot's colour
				>												SlotDebugInfoUndoRenderingDataPair;
		typedef std::list<SlotDebugInfoUndoRenderingDataPair>	SlotDebugInfoUndoRenderingDataPairList;

		SlotDebugInfoUndoRenderingDataPairList undoDataList;
	};

	////////////////////////////////////////////////////////////////////////////

	struct PathHighlightingUndoData : public ColourAddOnUndoData {
		typedef std::list<EdgeUndoRenderingData>	EdgeRenderingUndoDataList;
		typedef SlotDebugInfoUndoRenderingData
				::SlotDebugInfoUndoRenderingDataList SlotDebugInfoUndoRenderingDataList;
		
		struct UndoDataStruct {
			EdgeRenderingUndoDataList			incomintEdges;
			EdgeRenderingUndoDataList			outgoingEdges;
			SlotDebugInfoUndoRenderingDataList	slots;
		};

		typedef std::map<
					graphs::Vertex *,
					UndoDataStruct,
					utility::ComparatorFunctor<graphs::Vertex>
				>	PathHighlightingUndoDataMap;

		PathHighlightingUndoDataMap undoDataMap;
	};

}	//namespace iviews 


#endif	//COLOUR_ADDON_DATA_H