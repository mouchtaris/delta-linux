/**
 *	SlotFinder.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	July 2010
 */

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif


#include <assert.h>
#include "Text.h"
#include "Rectangle.h"
#include "SlotFinder.h"
#include "VertexFinder.h"



namespace iviews {

SlotRenderingInfo * SlotFinder::operator()(
					LayersRenderingInfo * layersRenInfo, 
					const coord_t & x, 
					const coord_t & y
				)
{
	assert(layersRenInfo);
	
	if (VertexRenderingInfo * vertex = VertexFinder()(layersRenInfo, x, y))
		return this->operator ()(vertex, x, y);
	return static_cast<SlotRenderingInfo *>(0);
}

//-----------------------------------------------------------------------

SlotRenderingInfo * SlotFinder::operator()(
						VertexRenderingInfo * v, 
						const coord_t & x, 
						const coord_t & y
					)
{
	assert(v);	
	const SlotRenderingInfoPtrList & slotsList = v->GetAllSlostsRenderingInfo();

	for(SlotRenderingInfoPtrList::const_iterator slot = slotsList.begin(); 
		slot != slotsList.end(); 
		++slot
	){
		Rectangle * keyRect		= (*slot)->GetKeyRectangle(); 
		Rectangle * contentRect	= (*slot)->GetContentRectangle();
		//briskoume an uparxei slot pou na tairiazoun ta coordinates
		if (Rectangle::InBoundaries(keyRect, x, y)		|| 
			Rectangle::InBoundaries(contentRect, x, y))
			return *slot;
	}

	return static_cast<SlotRenderingInfo *>(0);
}

//-----------------------------------------------------------------------

SlotRenderingInfo * SlotFinder::operator()(
						VertexRenderingInfo * v, 
						const std::string & text
					)
{
	assert(v);	
	const SlotRenderingInfoPtrList & slotsList = v->GetAllSlostsRenderingInfo();

	for(SlotRenderingInfoPtrList::const_iterator slot = slotsList.begin(); 
		slot != slotsList.end(); 
		++slot
	){
		Text * keyText		= (*slot)->GetKeyText(); 
		Text * contentText	= (*slot)->GetContentText();

		if (keyText && (keyText->GetText().find(text) != std::string::npos) )
			return *slot;
		if (contentText && (contentText->GetText().find(text) != std::string::npos) )
			return *slot;
	}

	return static_cast<SlotRenderingInfo *>(0);
}

}	//namespace iviews