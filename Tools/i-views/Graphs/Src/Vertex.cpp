/**
 *	Vertex.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	March 2010
 */

#include "Vertex.h"

#include <assert.h>

namespace iviews {
namespace graphs {

//-----------------------------------------------------------------------

bool Vertex::operator == (const Vertex *v) const { 
	assert(v); 
	return id == v->id; 
}

//-----------------------------------------------------------------------

bool Vertex::operator != (const Vertex *v) const { 
	assert(v); 
	return id != v->id; 
}

//-----------------------------------------------------------------------

const Vertex & Vertex::BadVertex() { 
	static Vertex badVertex;
	badVertex.nullObject = true;
	return badVertex; 
}

//-----------------------------------------------------------------------

bool operator == (const Vertex *l, const Vertex &r) {
	assert(l);
	return (*l) == r;
}

//-----------------------------------------------------------------------

bool operator != (const Vertex *l, const Vertex &r) {
	assert(l);
	return (*l) != r;
}

}	//namespace graphs
}	//namespace iviews