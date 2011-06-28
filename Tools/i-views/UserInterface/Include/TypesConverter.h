//july 2010
#ifndef	TYPES_CONVERTER_H
#define	TYPES_CONVERTER_H


#include <string>

#include <wx/dc.h>
#include <wx/window.h>
#include <wx/string.h>
#include <wx/colour.h>
#include <wx/gdicmn.h>
#include <wx/scrolwin.h>

#include "Point.h"
#include "Colour.h"
#include "Rectangle.h"


namespace iviews {

#define WX_IS_USED = 1;

#ifdef WX_IS_USED
	#define DC				wxDC	
	#define Window			wxWindow
	#define ScrolledWindow	wxScrolledWindow
#else
	#error	No suproted Window type yet
#endif


#ifdef UNICODE

inline const std::string str2std (const wxString& str)
	{ return std::string(str.mb_str(wxConvUTF8)); }

inline const wxString std2str (const std::string& std)
	{ return wxString(std.c_str(), wxConvUTF8); }

#else // ! UNICODE

inline const std::string str2std (const wxString& str)
	{ return std::string(str.str(), str.size()); }

inline const wxString std2str (const std::string& std)
	{ return wxString(std.c_str(), std.size()); }

#endif // UNICODE


inline geo::Colour wxColour2Colour(const wxColour & colour) 
	{	return geo::Colour(colour.Red(), colour.Green(), colour.Blue(), colour.Alpha()); }

inline wxColour colour2wxColour(const geo::Colour & colour) 
	{	return wxColour(colour.GetRed(), colour.GetGreen(), colour.GetBlue(), colour.GetAlpha());	}



inline wxPoint	point2wxPoint(const geo::Point<coord_t> & point)
	{ return wxPoint(point.GetX(), point.GetY()); }

inline geo::Point<coord_t> wxPoint2Point(const wxPoint & point)
	{ return geo::Point<coord_t>(point.x, point.y); }
/*
inline Rectangle wxRect2Rect() {
}

inline wxRect rect2wxRect


}	//namespace iviews
*/

}

#endif	//TYPES_CONVERTER_H