//June 2010
#ifndef TRIANGLE_H
#define TRIANGLE_H



#include "Point.h"
#include "Colour.h"
#include "Primitive.h"
#include "RenderingTypes.h"


namespace iviews {

	class Triangle : public Primitive {
	public:
		typedef geo::Colour			Colour;
		typedef geo::Point<coord_t> CoordTPoint;

		Triangle (const Triangle & triangle);
		
		Triangle (
			const CoordTPoint&  head_			= CoordTPoint(0, 0),
			const CoordTPoint&  leftSide_		= CoordTPoint(0, 0),
			const CoordTPoint&  rightSide_		= CoordTPoint(0, 0),
			const Colour	 &  bagroundColour_	= Colour(255, 255, 255),
			const Colour	 &  borderColour_	= Colour(),
			const length_t		borderWidth_	= 1
		);

		//--------------------------------------------------------
		void				SetHead (const CoordTPoint & p);
		const CoordTPoint&	GetHead (void) const	;

		//--------------------------------------------------------
		void			  	SetLeftSide (const CoordTPoint & p);
		const CoordTPoint& 	GetLeftSide (void) const;

		//--------------------------------------------------------
		void			  	SetRightSide (const CoordTPoint & p);
		const CoordTPoint& 	GetRightSide (void) const;

		//--------------------------------------------------------
		void				SetBorderColour ( const Colour & c);
		const Colour    &	GetBorderColour (void) const;

		//--------------------------------------------------------
		void				SetBagroundColour ( const Colour & c);
		const Colour	&	GetBagroundColour (void) const;

		//--------------------------------------------------------
		void				SetBorderWidth (const length_t & width);
		length_t			GetBorderWidth (void) const;

	private:
		length_t			borderWidth;
		Colour 				borderColour;
		Colour 				bagroundColour;
		CoordTPoint 		head;
		CoordTPoint 		leftSide;
		CoordTPoint 		rightSide;

	};

}	//namespace iviews

#endif	//TRIANGLE_H