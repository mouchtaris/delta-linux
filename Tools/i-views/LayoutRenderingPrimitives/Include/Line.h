/**
 *	Line.h
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	June 2010
 */

#ifndef LINE_H
#define LINE_H


#include <vector>

#include "Point.h"
#include "Colour.h"
#include "Primitive.h"
#include "RenderingTypes.h"


namespace iviews {

	class Line : public Primitive {
	public:
		typedef	geo::Colour			Colour;
		typedef geo::Point<coord_t> CoordTPoint;

		virtual ~Line (){}
		Line (const Line & line);
		
		Line (
			const CoordTPoint& begin_		= CoordTPoint(0, 0),
			const CoordTPoint& end_			= CoordTPoint(0, 0),
			const Colour	 & colour_		= Colour(),
			const length_t	   borderWidth_ = 1
		);
		
		//--------------------------------------------------------
		virtual const CoordTPoint &
							GetBegin (void) const;
		virtual void		SetBegin (const CoordTPoint & p);

		//--------------------------------------------------------
		virtual const CoordTPoint &
							GetEnd (void) const;
		virtual void		SetEnd (const CoordTPoint & p);

		//--------------------------------------------------------
		virtual const Colour & 
							GetColour (void) const;
		virtual void		SetColour (const Colour & c);

		//--------------------------------------------------------
		virtual length_t	GetBorderWidth (void) const;
		virtual void		SetBorderWidth (const length_t & width);
		

	protected:
		length_t borderWidth;

		Colour colour;

		CoordTPoint end;
		CoordTPoint begin;
	};


	///////////////////////////////////////////////////////////////////////////////////////////


	class CurveLine : public Line {
	public:
		typedef	Line::Colour			Colour;	
		typedef Line::CoordTPoint	CoordTPoint;
		
		~CurveLine (){}
		CurveLine (const CurveLine & line);
		CurveLine (
			const CoordTPoint& begin_		= CoordTPoint(0, 0),
			const CoordTPoint& end_			= CoordTPoint(0, 0),
			const CoordTPoint& peak_		= CoordTPoint(0, 0),
			const Colour	 & colour_		= Colour(),
			const length_t	   borderWidth_	= 1
		);

		//--------------------------------------------------------
		virtual const CoordTPoint &	
							GetCurvePeak (void) const;
		virtual void		SetCurvePeak (const CoordTPoint & peak);

	protected:
		CoordTPoint curvePeak;
	};


	///////////////////////////////////////////////////////////////////////////////////////////


	class SpliceLine : public Line {
	public:
		typedef	Line::Colour				Colour;	
		typedef Line::CoordTPoint			CoordTPoint;
		typedef std::vector<CoordTPoint>	ControlPoints;

		virtual ~SpliceLine() {}

		//empty line
		SpliceLine (
			const Colour&		 colour_		= Colour(),
			const length_t		 borderWidth_	= 1
		);

		SpliceLine (
			const ControlPoints& controlPoints_,
			const Colour&		 colour_		= Colour(),
			const length_t		 borderWidth_	= 1
		);

		//--------------------------------------------------------
		virtual void		SetBegin (const CoordTPoint & p);

		//--------------------------------------------------------
		virtual void		SetEnd (const CoordTPoint & p);


		virtual const ControlPoints & 
							GetControlPoints (void) const;
		virtual ControlPoints & 
							GetControlPoints (void);

		virtual void		SetControlPoints (const ControlPoints & cPoints);
		virtual size_t		GetNumberOfControlPoints (void) const;


		//const 
	private:
		ControlPoints controlPoints;
	};


}	//namespace iviews

#endif	//LINE_H