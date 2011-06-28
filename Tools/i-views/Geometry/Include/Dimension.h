/**
 *	Dimension.h
 *	
 *	The Dimension class encapsulates the width and height of a component in a single object,
 *	precision is be specified by a template parameter.
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	May 2010
 */

#ifndef DIMENSION_H
#define DIMENSION_H


#include <string>
#include <assert.h>

#include "StringUtils.h"

namespace iviews {
	namespace geo {
		template <typename T>
		class Dimension {
		public:
		
			///--------------- constructor(s)
			Dimension (const Dimension & d) : width(d.width), height(d.height) {} 
			Dimension (const T &_width = 0, const T &_height = 0) : 
				width	(_width), 
				height	(_height) 
				{ assert(_width >= 0); assert(_height >= 0); }

			//--------------------------------------------------------
			T			GetWidth (void)  const 
							{ return width; }
			void		SetWidth (const T & _width)	 
							{ assert(_width >= 0); width	= _width; }

			//--------------------------------------------------------
			T			GetHeight (void) const 
							{ return height; }
			void		SetHeight (const T & _height) 
							{ assert(_height >= 0); height	= _height; }

			//--------------------------------------------------------
			Dimension	GetSize (void) const	
							{ return *this; }
			void		SetSize (const Dimension<T> & d)				
							{ SetSize(d.GetWidth(), d.GetHeight()); }
			void		SetSize (const T & _width, const T & _height)
							{ SetWidth(_width); SetHeight(_height);}

			//--------------------------------------------------------
			std::string toString (void) const { 
				return std::string(
							"Dimension[width: "			+
							utility::string_cast(width)	+
							", height: "				+
							utility::string_cast(height)+
							"]"
						);
			}


			///--------------- operators 
			bool		operator== (const Dimension<T> & p) const 
							{ return width == p.GetWidth() && height == p.GetHeight(); }
			bool		operator!= (const Dimension<T> & p) const 
							{ return !(*this == p); }
			Dimension &	operator= (const Dimension<T> & d) 
							{ SetSize(d); return *this; }

			template <typename T>
			friend std::ostream & operator<< (std::ostream &, const Dimension<T> & p);


		private:
			T width;
			T height ;
		};

	}	//namespace geo
}	//namespace iviews

//-----------------------------------------------------------------------

template <typename T>
std::ostream & operator<< (std::ostream & os, const iviews::geo::Dimension<T> & d);


//-----------------------------------------------------------------------

template <typename T>
inline std::ostream & operator<< (std::ostream & os, const iviews::geo::Dimension<T> & d) 
	{	return os << d.toString();	}

#endif	//DIMENSION_H