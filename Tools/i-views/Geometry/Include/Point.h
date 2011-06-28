/**
 *	Point.h
 *	
 *	A point representing a location in (x, y) coordinate space, 
 *	precision is be specified by a template parameter.
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	May 2010
 */

#ifndef POINT_H
#define POINT_H

#include <string>
#include "StringUtils.h"


namespace iviews {
	namespace geo {

		template <typename T>
		class Point {

		public:
			///--------------- constructor(s)
			Point(const Point<T> & p) : x(p.x), y(p.y) 		{} 
			Point(const T &_x = 0, const T &_y = 0) : x(_x), y(_y)	{}
			

			//--------------------------------------------------------
			T			GetX (void) const { return x; }
			void		SetX (const T & _x) { x = _x; }

			//--------------------------------------------------------
			T			GetY (void) const { return y; }
			void		SetY (const T & _y) { y = _y; }

			//--------------------------------------------------------
			std::string toStringExpanted (void) const { 
				return std::string( 
							"Point["				+
							utility::string_cast(x)	+
							","						+
							utility::string_cast(y)	+
							"]"
						);
			}
			
			//--------------------------------------------------------

			std::string toString (void) const { 
				return std::string( 
							utility::string_cast(x)	+
							","						+
							utility::string_cast(y)
						);
			}
			///--------------- operators 
			bool		operator== (const Point<T> & p) const { return x == p.GetX() && y == p.GetY(); }
			bool		operator!= (const Point<T> & p) const { return !(*this == p); }

			bool		operator < (const Point<T> & p) const { return (x < p.GetX()) && (y < p.GetY()); }
			Point &		operator= (const Point<T> & p) { 
							x = p.x; 
							y = p.y; 
							return *this; 
						}	

			template <typename T>
			friend std::ostream & operator<< (std::ostream &, const Point<T> & p);
			

		private:
			T x;
			T y;
		};


	}	//namespace geo
}	//namespace iviews


//-----------------------------------------------------------------------
template <typename T>
std::ostream & operator<< (std::ostream & os, const iviews::geo::Point<T> & p);


//-----------------------------------------------------------------------
template <typename T>
inline std::ostream & operator<< (std::ostream & os, const iviews::geo::Point<T> & p) 
	{	return os << p.toString();	}


#endif //POINT_H