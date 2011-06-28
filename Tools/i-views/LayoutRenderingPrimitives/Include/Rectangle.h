//June 2010
#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Point.h"
#include "Colour.h"
#include "Dimension.h"
#include "Primitive.h"
#include "RenderingTypes.h"

namespace iviews {

	class Rectangle : public Primitive {
	public:
		typedef geo::Colour			Colour;
		typedef geo::Point<coord_t> CoordTPoint;


		///---- Constractor(s)
		Rectangle (const Rectangle & rect);
		
		Rectangle (
			const CoordTPoint				& position_			= CoordTPoint(-1, -1),
			const geo::Dimension<length_t>	& dimension_		= geo::Dimension<length_t>(0, 0),
			const Colour					& bagroundColour_	= Colour(255, 255, 255),
			const Colour					& borderColour_		= Colour(),
			const length_t					  borderWidth_		= 1
		);


		///////////////////////////////////////////////////
		//	public methods

		//--------------------------------------------------------
		unsigned char		GetTransparency (void) const;
		void				SetTransparency (const unsigned char alpha = geo::ALPHA_OPAQUE) ;		
		bool				IsTransparent (void)	const;

		//--------------------------------------------------------
		const CoordTPoint &	GetPosition (void) const;
		void				SetPosition (const coord_t & x, const coord_t & y);
		void				SetPosition (const CoordTPoint & p);

		//--------------------------------------------------------
		void				SetX (const coord_t & x);
		coord_t				GetX (void) const;

		//--------------------------------------------------------
		void				SetY (const coord_t & y);
		coord_t				GetY (void) const;

		//--------------------------------------------------------
		const geo::Dimension<length_t>	&
							GetDimension (void) const;
		void				SetDimension (const geo::Dimension<length_t> & d);

		//--------------------------------------------------------
		length_t			GetWidth (void) const;
		void				SetWidth (const length_t & width);


		//--------------------------------------------------------
		length_t			GetHeight (void) const;
		void				SetHeight (const length_t & height);
		
		//--------------------------------------------------------
		const Colour &		GetBorderColour (void) const;
		void				SetBorderColour ( const Colour & c);

		//--------------------------------------------------------
		const Colour &		GetBagroundColour (void) const;
		void				SetBagroundColour ( const Colour & c);

		//--------------------------------------------------------
		void				SetBorderWidth (const length_t & width);
		length_t			GetBorderWidth (void) const;


		bool				operator!= (const Rectangle * rect);
		bool				operator!= (const Rectangle & rect);
		bool				operator== (const Rectangle * rect);
		bool				operator== (const Rectangle & rect);

		/////////////////////////////////////////////////
		//	Static method API
		
		static bool	InBoundaries (const Rectangle * rect, const CoordTPoint & p);
		static bool InBoundaries (const Rectangle * rect, const coord_t & x, const coord_t & y);
	
	private:
		///////////////////////////////////////////////////
		//	private members

		length_t			borderWidth;
		Colour				borderColour;
		Colour				bagroundColour;
		CoordTPoint			position;
		geo::Dimension<length_t>	dimension;
	};

}	//namespace iviews

#endif //RECTANGLE_H