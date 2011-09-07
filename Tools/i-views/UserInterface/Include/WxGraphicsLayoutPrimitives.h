/**
 *	WxGraphicsLayoutPrimitives.h
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	July 2010
 */

#ifndef WX_GRAPHICS_LAYOUT_PRIMITIVES_H
#define WX_GRAPHICS_LAYOUT_PRIMITIVES_H

#include <string>

#include <wx/dc.h>
#include <wx/pen.h>
#include <wx/font.h>

#include "Dimension.h"
#include "RenderingTypes.h"
#include "GraphicsLayoutPrimitives.h"

namespace iviews {

class WxGraphicsLayoutPrimitives : public GraphicsLayoutPrimitives {

public:

	WxGraphicsLayoutPrimitives (
		wxWindow*		owner,
		const wxFont&	font
	);

	void				SetFont (const wxFont & font);
	virtual length_t	GetTextWidth (const std::string & text);
	virtual length_t	GetTextHeight (const std::string & text);
	virtual geo::Dimension<length_t>	
						GetTextDimension (const std::string & text);
	
	//Gets the average character width of the currently set font.
	virtual length_t	GetCharWidth (void);

	//Gets the average character height of the currently set font.
	virtual length_t	GetCharHeight (void);

private:
	wxFont		font;
	wxWindow *	owner;

	//wxDC * dc;
};

}	//namespace iviews


#endif	//WX_GRAPHICS_LAYOUT_PRIMITIVES_H