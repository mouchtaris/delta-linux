#include <math.h>
#include "BookmarkRenderingInfo.h"


namespace iviews {

	//-----------------------------------------------------------------------

BookmarkRenderingInfo::~BookmarkRenderingInfo() {
	if (text) {
		delete text;
		text = static_cast<Text *>(0);
	}

	if (rect) {
		delete rect;
		rect = static_cast<Rectangle *>(0);
	}
}

//-----------------------------------------------------------------------

BookmarkRenderingInfo::BookmarkRenderingInfo(
	const id_t	& id_,
	Text		* text_,
	Rectangle	* rect_
) :	id (id_),
	text(text_),
	rect(rect_)
{}

//-----------------------------------------------------------------------
//Update all contetns' pos.
void BookmarkRenderingInfo::SetPosition(const coord_t & x, const coord_t & y) { 
	coord_t textDx = text->GetTextXCoordinate() - rect->GetX();
	coord_t textDy = text->GetTextYCoordinate() - rect->GetY();
	
	rect->SetPosition(x, y);
	text->SetTextPosition(x + textDx, y + textDy);
}

//-----------------------------------------------------------------------

void BookmarkRenderingInfo::SetPosition(const CoordTPoint & newPos) 
	{ SetPosition(newPos.GetX(), newPos.GetY()); }

//-----------------------------------------------------------------------

const BookmarkRenderingInfo::CoordTPoint & BookmarkRenderingInfo::GetPosition(void) const
	{ return rect->GetPosition(); }

//-----------------------------------------------------------------------

length_t BookmarkRenderingInfo::GetWidth(void) const
	{ return rect->GetWidth(); }	

//-----------------------------------------------------------------------

length_t BookmarkRenderingInfo::GetHeight(void) const
	{ return rect->GetHeight(); }

//-----------------------------------------------------------------------

coord_t BookmarkRenderingInfo::GetX(void) const
	{ return rect->GetX(); }

//-----------------------------------------------------------------------

coord_t	BookmarkRenderingInfo::GetY(void) const
	{ return rect->GetY(); }	

//-----------------------------------------------------------------------

Text * BookmarkRenderingInfo::GetText(void) const
	{ return text; }

//-----------------------------------------------------------------------

Rectangle *	BookmarkRenderingInfo::GetRectangle(void) const
	{ return rect; }

//-----------------------------------------------------------------------

std::pair<id_t, id_t> BookmarkRenderingInfo::GetPrimitivesIds(void) const 
	{ return std::make_pair(text->Id(), rect->Id()); }

}	//namespace iviews 