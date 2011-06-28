/**
 *	Geometry.h
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	May 2010
 */

#ifndef COMMON_GEOMETRY_H
#define COMMON_GEOMETRY_H


namespace iviews {

namespace geo {


/****************************** About Colour *****************************************/

// flags for Colour -> std::string conversion (see Colour::GetAsString)
#define STYLE_CSS_SYNTAX        2   // return colour in rgb(r,g,b) syntax
#define STYLE_HTML_SYNTAX       4   // return colour in #rrggbb syntax


const unsigned char ALPHA_TRANSPARENT	= 0;
const unsigned char ALPHA_OPAQUE		= 0xff;

/**************************** End Colour *********************************************/


}	//namespace geo

}	//namespace iviews

#endif	//COMMON_GEOMETRY_H