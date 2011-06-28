/**
 *	Dimension.h
 *	
 *	A colour is an object representing a combination of Red, Green, and Blue (RGB) intensity values,
 *	and is used to determine drawing colours.
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	May 2010
 */

#ifndef COLOUR_H
#define COLOUR_H

#include <string>
#include "Geometry.h"

namespace iviews {

	namespace geo {

		//RGB
		class Colour {
		public:
			Colour (const Colour & colour);
			Colour (
				const unsigned char red		= 0, 
				const unsigned char green	= 0, 
				const unsigned char blue	= 0, 
				const unsigned char alpha	= ALPHA_OPAQUE
			);


			unsigned char	GetRed (void)	const;
			unsigned char	GetGreen (void) const;
			unsigned char	GetBlue (void) const;
			unsigned char	GetAlpha (void) const; 


			void 			SetRed (const unsigned char r);
			void 			SetGreen (const unsigned char g);
			void 			SetBlue (const unsigned char b);
			void 			SetAlpha (const unsigned char a);


			bool 			operator== (const Colour & c) const;
			
			bool 			operator== (const Colour * c) const;

			bool 			operator!= (const Colour & c) const;
			bool 			operator!= (const Colour * c) const;

			
			//flag == STYLE_CSS_SYNTAX or STYLE_HTML_SYNTAX
			std::string		GetAsString (const long flags = STYLE_CSS_SYNTAX | STYLE_HTML_SYNTAX) const;

		private:
			unsigned char red;
			unsigned char green;
			unsigned char blue;
			unsigned char alpha;
			
		};

	}	//namespace geo
}	//namespace iviews
#endif //COLOUR_H