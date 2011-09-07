/**
 *	WxGraphicsLayoutPrimitives.cpp
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	July 2010
 */

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif

#include <assert.h>
#include <wx/dcclient.h>
#include "TypesConverter.h"
#include "WxGraphicsLayoutPrimitives.h"


namespace iviews {

#define PREPARE_DC()		\
	assert(owner);			\
	wxClientDC dc(owner);	\
	dc.SetFont(font)

//-----------------------------------------------------------------------

WxGraphicsLayoutPrimitives::WxGraphicsLayoutPrimitives(
	wxWindow* owner_,
	const wxFont & font_
) : owner(owner_),
	font(font_)
{}

//-----------------------------------------------------------------------

void WxGraphicsLayoutPrimitives::SetFont(const wxFont & font_)
	{ font = font_; }

//-----------------------------------------------------------------------

length_t WxGraphicsLayoutPrimitives::GetTextWidth(const std::string & text) {
	PREPARE_DC();
	return dc.GetTextExtent(std2str(text)).GetWidth();
}

//-----------------------------------------------------------------------

length_t WxGraphicsLayoutPrimitives::GetTextHeight(const std::string & text) {
	PREPARE_DC();
	return dc.GetTextExtent(std2str(text)).GetHeight();
}

//-----------------------------------------------------------------------

geo::Dimension<length_t> WxGraphicsLayoutPrimitives::GetTextDimension(const std::string & text) {
	return geo::Dimension<length_t>(GetTextWidth(text), GetTextHeight(text));
}

//-----------------------------------------------------------------------

length_t WxGraphicsLayoutPrimitives::GetCharWidth(void) {
	PREPARE_DC();
	return dc.GetCharWidth();
}

//-----------------------------------------------------------------------

length_t WxGraphicsLayoutPrimitives::GetCharHeight(void) {
	PREPARE_DC();
	return dc.GetCharHeight();
}


}	//namespace iviews