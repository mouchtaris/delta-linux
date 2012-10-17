/**
 *	GraphLayers.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	March 2010
 */

#include "GraphLayers.h"


#include <assert.h>
#include <algorithm>

namespace iviews {
namespace graphs {

//-----------------------------------------------------------------------

GraphLayers::GraphLayers(const Graph & _basicGraph)
:	table(),
	nextLayer(0),
	verticesNum(0),
	basicGraph(_basicGraph)
{}

//-----------------------------------------------------------------------

GraphLayers::GraphLayers(
	const Layer & _maxNumOfLayers, 
	const Graph & _basicGraph
) :	table(),
	nextLayer(0),
	verticesNum(0),
	basicGraph(_basicGraph)
{}

//-----------------------------------------------------------------------

bool GraphLayers::LayerExists(const Layer & l) const 
	{	return l < table.size();	}


//-----------------------------------------------------------------------

bool GraphLayers::IsEmpty(void) const
	{ return table.size() == 0; }

//-----------------------------------------------------------------------

bool GraphLayers::AddVertexInLayer(const Vertex & v, const Layer & l) {
	bool result = false;
	assert (LayerExists(l));

	if (GetVertexFromLayer(v, l) == table[l].end()) {
		allVertices.push_back(&(const_cast<Vertex & >(v)));
		table[l].push_back(&(const_cast<Vertex & >(v)));
		++verticesNum;
		result = true;
	}
	return result;
}


//-----------------------------------------------------------------------
bool GraphLayers::DeleteLastLayer(void) {
	bool result = false;
	
	if (nextLayer > 0) {
		result = true;
		--nextLayer;
		table.pop_back();
	}

	return false;
}

//-----------------------------------------------------------------------

bool GraphLayers::VertexExist(const Vertex & v) const { 
	VerticesPtrList::const_iterator list;
	for (Layer l = GetFirstLayer(); HasNextLayer(l); l = GetNextLayer(l))
		if ( (list = GetVertexFromLayer(v, l)) != table[l].end())
			return true;

	return false;	
}

//-----------------------------------------------------------------------

bool GraphLayers::VertexExistsInLayer(const Vertex & v, const Layer & l){ 
	assert(LayerExists(l));
	return GetVertexFromLayer(v, l) != table[l].end();
}	

//-----------------------------------------------------------------------

bool GraphLayers::SwapVerticesInLayer(const Vertex & v1, const Vertex & v2, const Layer & l) {
	assert(LayerExists(l));

	bool result = false;
	Vertex::VerticesPtrList::iterator si = GetVertexFromLayer(v1, l);
	Vertex::VerticesPtrList::iterator ti = GetVertexFromLayer(v2, l);

	if (si != table[l].end() && ti != table[l].end()) {
		result = true;
		table[l].splice(si, table[l], ti);
	}

	return result;
}

//-----------------------------------------------------------------------

GraphLayers::Layer GraphLayers::NewLayer(void) {
	table.push_back(Vertex::VerticesPtrList());
	return nextLayer++;
}

//-----------------------------------------------------------------------

unsigned GraphLayers::TotalNumberOfLayers(void) const 
	{ return table.size(); }				

//-----------------------------------------------------------------------

unsigned GraphLayers::TotalNumberOfVertices(void) const 
	{ return verticesNum;	}		

//-----------------------------------------------------------------------

unsigned GraphLayers::TotalNumberOfVerticesInLayer(const Layer & l) const 
	{ return static_cast<unsigned>(GetAllVerticesInLayer(l).size());	}	

//-----------------------------------------------------------------------

//v1 and v2 to exists in layer l.
long GraphLayers::GetDistanceOfVerticesInLayer (
		const Vertex & v1,
		const Vertex & v2, 
		const Layer & l
	) const {
	assert(LayerExists(l));

	Vertex::VerticesPtrList::const_iterator pos1 = GetVertexFromLayer(v1, l);
	Vertex::VerticesPtrList::const_iterator pos2 = GetVertexFromLayer(v2, l);
	assert(pos1 != table[l].end() && pos2 != table[l].end());	

	long dist1 = (long)std::distance(table[l].begin(), pos1);
	long dist2 = (long)std::distance(table[l].begin(), pos2);
	long dist = dist2 - dist1;

	return dist <= 0 ? -dist : dist - 1;	// convert distance to positive number
											// because we do not know which vertex is first
}

//-----------------------------------------------------------------------

const Graph	& GraphLayers::GetGraph(void) const 
	{ return basicGraph; }

//-----------------------------------------------------------------------

const Vertex::VerticesPtrList & GraphLayers::GetAllVertices(void) const 
	{	return allVertices; }

//-----------------------------------------------------------------------

const Vertex::VerticesPtrList & GraphLayers::GetAllVerticesInLayer(const Layer & l) const {
	assert(LayerExists(l));
	return table[l];
}

//-----------------------------------------------------------------------

GraphLayers::Layer GraphLayers::GetLayerThatContainVertex(const Vertex & v) const {
	assert(VertexExist(v));
	
	for (Layer l = GetFirstLayer(); HasNextLayer(l); l = GetNextLayer(l))
		if (GetVertexFromLayer(v, l) != table[l].end())
			return l;

	assert(0);	//It is impossible to arrive in this statement because vertex v must be exists.
	return 0;
}

//-----------------------------------------------------------------------

GraphLayers::Layer GraphLayers::GetLastLayer(void) const 
	{ return !table.size() ? 0 : (table.size()-1); }

//-----------------------------------------------------------------------

GraphLayers::Layer GraphLayers::GetFirstLayer(void) const 
	{ return 0; }

//-----------------------------------------------------------------------

bool GraphLayers::HasNextLayer(const Layer & l) const 
	{ return l < nextLayer; }

//-----------------------------------------------------------------------

GraphLayers::Layer GraphLayers::GetNextLayer(const Layer & l) const {
	assert(HasNextLayer(l));
	return l+1;
}

//-----------------------------------------------------------------------

std::ostream & operator<< (std::ostream & stream, const GraphLayers * gl) {
	
	unsigned layers = 0;
	GraphLayers::LayersTable::const_iterator i;
	for(i = gl->table.begin(); i != gl->table.end(); ++i, ++layers){
		stream << "\nLayer (" << layers << ")\n";
		
		Vertex::VerticesPtrList::const_iterator j;
		for(j = i->begin(); j != i->end(); ++j)
			stream << "Vertex: " << (*j)->Id() << "\n";
	}
	return stream;
}

std::ostream & operator<< (std::ostream & stream, const GraphLayers & gl) {
	stream << (&gl);
	return stream;
}

//***************************************
// private method API

//-----------------------------------------------------------------------

Vertex::VerticesPtrList::iterator GraphLayers::GetVertexFromLayer(const Vertex & v, const Layer & l) {
	assert(LayerExists(l));
	return std::find(table[l].begin(), table[l].end(), v);
}

//-----------------------------------------------------------------------

Vertex::VerticesPtrList::const_iterator GraphLayers::GetVertexFromLayer(
		const Vertex & v, 
		const Layer & l
	) const {
	assert(LayerExists(l));
	return std::find(table[l].begin(), table[l].end(), v);
}

}	//namespace graphs

}	//namespace iviews