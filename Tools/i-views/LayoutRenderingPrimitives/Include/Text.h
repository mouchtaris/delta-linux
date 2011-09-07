/**
 *	Text.h
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	June 2010
 */

#ifndef TEXT_H
#define TEXT_H

#include <string>

#include "Point.h"
#include "Colour.h"
#include "Primitive.h"
#include "RenderingTypes.h"

namespace iviews {

class Text : public Primitive {
public:
	typedef geo::Colour			Colour;
	typedef geo::Point<coord_t> CoordTPoint;

	///---- Constractor(s)
	Text (const Text & t);
	Text (
		const std::string	& text_			= "",
		const Colour		& textColour_	= Colour(),
		const CoordTPoint	& textPos_		= CoordTPoint(0, 0)
	);

	///////////////////////////////////////////////////
	//	public methods

	//--------------------------------------------------------
	void				SetText (const std::string & text_);
	const std::string & GetText (void) const;


	//--------------------------------------------------------
	void				SetTextPosition (const coord_t & x, const coord_t &);
	void				SetTextPosition (const CoordTPoint & pos);
	CoordTPoint			GetTextPosition (void) const;


	//--------------------------------------------------------
	void				SetTextXCoordinate (const coord_t & x);
	coord_t				GetTextXCoordinate (void) const;

	//--------------------------------------------------------
	void				SetTextYCoordinate (const coord_t & y);
	coord_t				GetTextYCoordinate (void) const;


	//--------------------------------------------------------
	void				SetColour (const Colour & colour);
	const Colour &		GetColour (void) const;

private:
	///////////////////////////////////////////////////
	//	private members
	std::string			text;
	Colour				textColour;
	CoordTPoint			textPosition;


};


}	//namespace iviews

#endif //TEXT_H