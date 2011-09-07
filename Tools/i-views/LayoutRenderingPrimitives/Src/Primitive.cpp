/**
 *	Primitive.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	June 2010
 */

#include "Primitive.h"


iviews::id_t nextId = 0;
namespace iviews {
	
//-----------------------------------------------------------------------

Primitive::Primitive() : show(true), type(UNDEFINE), id(nextId++) {}

//-----------------------------------------------------------------------

id_t Primitive::Id(void) const 
	{ return id; }

//-----------------------------------------------------------------------

bool Primitive::IsShowing(void) const 
	{ return show; }

//-----------------------------------------------------------------------

void Primitive::Show(const bool show_) 
	{ show = show_; }

//-----------------------------------------------------------------------

Primitive::PrimitiveType Primitive::Type(void) const 
	{ return type; }


}	//namespace iviews 