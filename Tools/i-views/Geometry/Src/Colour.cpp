/**
 *	Colour.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	May 2010
 */

#include "Colour.h"

#include <stdio.h>
#include <assert.h>

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about sprintf usage.
	#pragma warning( disable : 4996)
#endif


#define SET_VALUE_TO_ATTRIBUTE(ATTR, VALUE)	\
	assert(ATTR >= 0 && ATTR <= 255);		\
	ATTR = VALUE

namespace iviews {

namespace geo {

//-----------------------------------------------------------------------

Colour::Colour(const Colour & colour){
	SET_VALUE_TO_ATTRIBUTE(red, colour.red);
	SET_VALUE_TO_ATTRIBUTE(green, colour.green);
	SET_VALUE_TO_ATTRIBUTE(blue, colour.blue);
	SET_VALUE_TO_ATTRIBUTE(alpha, colour.alpha);
}

//-----------------------------------------------------------------------

Colour::Colour(
	const unsigned char r, 
	const unsigned char g, 
	const unsigned char b, 
	const unsigned char a)
{
	SET_VALUE_TO_ATTRIBUTE(red, r);
	SET_VALUE_TO_ATTRIBUTE(green, g);
	SET_VALUE_TO_ATTRIBUTE(blue, b);
	SET_VALUE_TO_ATTRIBUTE(alpha, a);
}

//-----------------------------------------------------------------------

unsigned char Colour::GetRed (void)	const 
	{ return red; }

//-----------------------------------------------------------------------

unsigned char Colour::GetGreen (void) const 
	{ return green; }

//-----------------------------------------------------------------------

unsigned char Colour::GetBlue (void) const 
	{ return blue; }

//-----------------------------------------------------------------------

unsigned char Colour::GetAlpha (void) const 
	{ return alpha;	}

//-----------------------------------------------------------------------

void Colour::SetAlpha(unsigned char a)
	{	SET_VALUE_TO_ATTRIBUTE(alpha, a); }

//-----------------------------------------------------------------------

void Colour::SetRed(unsigned char r)
	{	SET_VALUE_TO_ATTRIBUTE(red, r); }

//-----------------------------------------------------------------------

void Colour::SetGreen(unsigned char g)
	{	SET_VALUE_TO_ATTRIBUTE(green, g); }

//-----------------------------------------------------------------------

void Colour::SetBlue(unsigned char b)
	{	SET_VALUE_TO_ATTRIBUTE(blue, b); }

//-----------------------------------------------------------------------

bool Colour::operator== (const Colour & c) const { 
	return	red		== c.red	&& 
			green	== c.green	&&
			blue	== c.blue	&&
			alpha	== c.alpha;
};

//-----------------------------------------------------------------------

bool Colour::operator== (const Colour * c) const 
	{ return this->operator ==(*c); }

//-----------------------------------------------------------------------

bool Colour::operator!= (const Colour & c) const 
	{ return !(*this == c);}

//-----------------------------------------------------------------------

bool Colour::operator!= (const Colour * c) const 
	{ return !(*this == *c);}

//-----------------------------------------------------------------------

std::string Colour::GetAsString(const long flags) const {
	assert((flags & STYLE_CSS_SYNTAX) || (flags & STYLE_HTML_SYNTAX));

	char buffer[1024];

	if (flags & STYLE_CSS_SYNTAX)	//return it in CSS syntax
		sprintf(buffer, "rgb(%d, %d, %d)", red, green, blue);
	else							//return it in HTML syntax
		sprintf(buffer, "#%02X%02X%02X", red, green, blue);
	
	return std::string(buffer);
}

}	//namespace geo

}	//namespace iviews