/**
 *	SlotRenderingInfo.h
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	June 2010
 */

#ifndef SLOT_RENDERING_INFO_H
#define SLOT_RENDERING_INFO_H


#include <list>
#include "Text.h"
#include "Vertex.h"
#include "Colour.h"
#include "Rectangle.h"
#include "CommonTypes.h"
#include "SlotDebugInfo.h"
#include "EdgeRenderingInfo.h"

namespace iviews {

	class SlotRenderingInfo {

	public:
		///////////////////////////////////////////////////
		//	public typedef(s)
		typedef std::list<SlotRenderingInfo *>				SlotRenderingInfoPtrList;
		typedef EdgeRenderingInfo::EdgeRenderingInfoPtrList EdgeRenderingInfoPtrList;

		
		~SlotRenderingInfo();		//delete all the Rendering primitives

		SlotRenderingInfo(
			const id_t &			id,
			graphs::Vertex		* _source, 
			dinfo::SlotDebugInfo	* _slot,
			Text					* _keyText					= static_cast<Text *>(0),
			Rectangle				* _keyRect					= static_cast<Rectangle *>(0),
			Rectangle				* _keyExpandButtonRect		= static_cast<Rectangle *>(0),
			Text					* _conntentText				= static_cast<Text *>(0),
			Rectangle				* _contentRect				= static_cast<Rectangle *>(0),
			Rectangle				* _contentExpandButtonRect	= static_cast<Rectangle *>(0)
		);


		graphs::Vertex *		GetSourceVertex(void) const { return source; }
		dinfo::SlotDebugInfo *	GetSlotDebugInfo(void) const { return slot; }

		id_t		Id() const { return id; }

		bool		IsHidden (void) const;
		void		Hide (bool hide = true);

		void		SetKeyText(Text * text);
		Text *		GetKeyText(void) const { return keyText; }

		void		SetContentText(Text * text);
		Text *		GetContentText(void) const { return contentText; }


		void		SetKeyRectangle(Rectangle * rect);
		Rectangle * GetKeyRectangle(void) const { return keyRect; }

		void		SetContentRectangle(Rectangle * rect);
		Rectangle * GetContentRectangle(void) const { return contentRect; }


		void		SetContentExpandButtonRectangle(Rectangle * rect);
		Rectangle * GetContentExpandButtonRectangle(void) const { return contentExpandButtonRect; }

		void		SetKeyExpandButtonRectangle(Rectangle * rect);
		Rectangle * GetKeyExpandButtonRectangle(void) const { return keyExpandButtonRect; }

		void		AddKeyEdgeRenderingInfo(EdgeRenderingInfo * eri);
		void		AddContentEdgeRenderingInfo(EdgeRenderingInfo * eri);


		
		const EdgeRenderingInfoPtrList & 
					GetAllKeyEdgesRenderingInfo(void) const
						{ return keyEdges; }


		const EdgeRenderingInfoPtrList & 
					GetAllContentEdgesRenderingInfo(void) const
						{ return contentEdges;}


	private:
		id_t					id;
		bool					isHidden;
		graphs::Vertex		 *	source;
		dinfo::SlotDebugInfo *	slot;

		EdgeRenderingInfoPtrList keyEdges;
		EdgeRenderingInfoPtrList contentEdges;


		////////////////////////
		// Rendering primitives
		Text * 		keyText;
		Text * 		contentText;
		Rectangle * keyRect;
		Rectangle * contentRect;

		Rectangle * keyExpandButtonRect;
		Rectangle * contentExpandButtonRect;

	};

}	//namespace iviews




#endif	//SLOT_RENDERING_INFO_H