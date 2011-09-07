/**
 *	Arrow.h
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	June 2010
 */

#ifndef ARROW_H
#define ARROW_H


#include "Line.h"
#include "Point.h"
#include "Colour.h"
#include "Triangle.h"
#include "Primitive.h"
#include "Rectangle.h"
#include "RenderingTypes.h"

namespace iviews {

	class Arrow : public Primitive {
	public:
		typedef geo::Colour			Colour;
		typedef geo::Point<coord_t> CoordTPoint;
		
		///---- Constractor (s)
		virtual ~Arrow (){}
		Arrow (
			const CoordTPoint &	arrowHead,
			const CoordTPoint &	arrowTail,
			const length_t		arrowWidth = 2,
			const Colour &		headBorderColour	 = Colour (),
			const Colour &		headBagroundColour	 = Colour (),
			const Colour &		shaftBorderColour	 = Colour ()
		);


		///////////////////////////////////////////////////
		//	public methods
		Line				GetShaft (void) const;
		virtual const Triangle &			
							GetArrowHead (void) const;
		virtual void		SetArrowHead (const CoordTPoint & newHead);
		virtual void		SetArrowHead (const coord_t & x, const coord_t & y);

		virtual const Colour &
							GetColour (void) const;
		virtual void		SetColour (const Colour & c);

		virtual const Colour &
							GetShaftBorderColour (void) const;
		virtual void		SetShaftBorderColour (const Colour & c);

		virtual const Colour &
							GetHeadBorderColour (void) const;
		virtual void		SetHeadBorderColour (const Colour & c);

		virtual const Colour &
							GetHeadBagroundColour (void) const;
		virtual void		SetHeadBagroundColour (const Colour & c);

		virtual void		SetWidth (const length_t width);
		virtual length_t	GetWidth (void) const;

		virtual const CoordTPoint &	
							GetHead (void) const;
		virtual void		SetHead(const coord_t & x, const coord_t & y);
		virtual void		SetHead(const CoordTPoint & head);

		virtual const CoordTPoint &	
							GetTail (void) const;
		virtual void		SetTail(const coord_t & x, const coord_t & y);
		virtual void		SetTail(const CoordTPoint & tail);

		virtual const CoordTPoint &	
							GetLeftSide (void) const;
		virtual void		SetLeftSide(const CoordTPoint & leftSide);
		
		virtual const CoordTPoint &	
							GetRightSide (void)	const;		
		virtual void		SetRightSide(const CoordTPoint & rightSide);

	


	protected:
		///////////////////////////////////////////////////
		//	protected methods
		void CalcArrowLeftRightSide (Triangle & head, const CoordTPoint & tail);
	

		///////////////////////////////////////////////////
		//	protected members
		Triangle			arrowHead;
		CoordTPoint			arrowTail;
		Colour				shaftColour;	//body
	};

	///////////////////////////////////////////////////////////////////////////////////////////

	class CurveArrow : public Arrow {
	public:
		typedef geo::Colour			Colour;
		typedef geo::Point<coord_t> CoordTPoint;

		///---- Constractor (s)
		virtual ~CurveArrow (){}
		CurveArrow (
			const CoordTPoint &	arrowHead,
			const CoordTPoint &	arrowTail,
			const length_t		arrowWidth = 2,
			const Colour &		headBorderColour	= Colour (),	
			const Colour &		headBagroundColour	= Colour (),	
			const Colour &		shaftBorderColour	= Colour ()
		);


		CurveLine GetShaft (void) const;

		virtual const CoordTPoint &	
							GetCurvePeak (void) const;
		virtual CoordTPoint&	GetCurvePeak (void);

		virtual const Rectangle &				
							GetEclipseRect (void) const;
		virtual Rectangle &	GetEclipseRect (void);

	private:
		Rectangle			eclpiiseRect;
		CoordTPoint			curvePeak;
	};


	///////////////////////////////////////////////////////////////////////////////////////////

	class SpliceArrow : public Arrow {
	public:
		typedef geo::Colour					Colour;
		typedef geo::Point<coord_t> 		CoordTPoint;
		typedef SpliceLine::ControlPoints	ControlPoints;

		///---- Constractor (s)
		virtual ~SpliceArrow (){}
		SpliceArrow (
			const CoordTPoint &	arrowHead,
			const CoordTPoint &	arrowTail,
			const length_t 		minorRadius			= 2,
			const length_t		arrowWidth			= 2,
			const Colour &		headBorderColour	= Colour (),
			const Colour &		headBagroundColour	= Colour (),
			const Colour &		shaftBorderColour	= Colour ()
		);

		virtual length_t	GetMinorRadius (void) const;
		void				SetMinorRadius (const length_t & minorRadius);
		virtual SpliceLine	GetShaft (void) const;
		virtual void		SetHead (const CoordTPoint & head);
		virtual void		SetHead (const coord_t & x, const coord_t & y);
		virtual void		SetTail (const coord_t & x, const coord_t & y);
		virtual void		SetTail (const CoordTPoint & tail);
		virtual void		SetColour (const Colour & c);

		virtual const ControlPoints &
							GetControlPoints(void) const;
	private:
		
		length_t			minorRadius;
		SpliceLine			shaft;
	};

}	//naemsapce iviews


#endif //ARROW_H