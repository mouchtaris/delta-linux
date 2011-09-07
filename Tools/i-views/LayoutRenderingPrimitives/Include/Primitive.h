/**
 *	Primitive.h
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	June 2010
 */

#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <list>
#include <functional>

#include "Colour.h"
#include "CommonTypes.h"


namespace iviews {

class Primitive {
public:
	typedef std::list<Primitive *> PrimitivePtrList;
	
	struct PredicateFunctor : public std::unary_function<const Primitive *, bool> {
		id_t id;
		PredicateFunctor (id_t id_) : id(id_) {}
		PredicateFunctor (const Primitive * p) : id(p->Id()) {}

		bool operator() (const Primitive * p)
			{ return p->Id() == id; }
	};

	enum PrimitiveType {
		UNDEFINE			= 0,
		TEXT				= 1,
		LINE				= 2,
		CURVE_LINE			= 3,
		SPLICE_LINE			= 4,
		TRIANGLE			= 5,
		RECTANGLE			= 6,
		ARROW				= 7,
		CURVE_ARROW			= 8,
		SPLICE_ARROW		= 9
	};


	virtual id_t			Id (void) const;
	virtual bool			IsShowing (void) const;
	virtual void			Show (const bool _);
	virtual PrimitiveType	Type (void) const;

	Primitive ();
	virtual ~Primitive (){}

protected:
	id_t			id;
	bool			show;
	PrimitiveType	type;

private:
	

};

}	//namespace iviews


 




#endif	//PRIMITIVE_H


