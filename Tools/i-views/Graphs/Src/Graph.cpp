/**
 *	Graph.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	March 2010
 */

#include <set>
#include <assert.h>
#include <algorithm>

#include "Graph.h"
#include "Functional.h"
#include "StringUtils.h"


//-----------------------------------------------------------------------

#define FIND_VERTEX(LIST, VERTEX)		\
	std::find (							\
		LIST.begin(),					\
		LIST.end(),						\
		VERTEX							\
	)

namespace iviews {

namespace graphs {

//-----------------------------------------------------------------------

Graph::~Graph(){
	for(VerticesPtrVec::iterator i = allVertices.begin(); i != allVertices.end(); ++i)
		delete *i;
}

//-----------------------------------------------------------------------

Graph::Graph (const unsigned _verticesNum) : edgesNum(0), verticesNum(0), nextVertexId(0) {
	for(unsigned i = 0; i < _verticesNum; ++i)
		NewVertex();
}

//-----------------------------------------------------------------------

bool Graph::Undo (void) {
	if (!undoRedo.CanUndo())
		return false;

	EdgeList undoActions = undoRedo.Undo();
	for(EdgeList::iterator edge = undoActions.begin(); edge != undoActions.end(); ++edge)
		AddEdge(*edge->first, *edge->second);

	return true;
}

//-----------------------------------------------------------------------

bool Graph::Redo (void) {
	if (!undoRedo.CanRedo())
		return false;

	EdgeList redoActions = undoRedo.Redo();
	//New undo action for future undo (store all edges that we will delete)
	//undoRedo.NewUndoAction();
	for(EdgeList::iterator edge = redoActions.begin(); edge != redoActions.end(); ++edge)
		DeleteEdgeImplementation(*edge->first, *edge->second, false);

	return true;
}

//-----------------------------------------------------------------------

bool Graph::IsEmpty (void) const 
	{	return verticesNum == 0;	}

//-----------------------------------------------------------------------

bool Graph::VertexExists (const Vertex & v) const 
{	return table.ContainsVertex(v);	}

//-----------------------------------------------------------------------

bool Graph::EdgeExists (const Vertex &source, const Vertex &target) const {
	bool in		= IncomingEdgeExists(target, source);
	bool out	= OutgoingEdgeExists(source, target);
	
	assert ((in && out) || (!in && !out));
	return in && out;
}


//-----------------------------------------------------------------------

bool Graph::IncomingEdgeExists (const Vertex & to, const Vertex & from) const 
	{ return EdgeExists(to, from, false);	}

//-----------------------------------------------------------------------

bool Graph::OutgoingEdgeExists (const Vertex & source, const Vertex & target) const 
	{ return EdgeExists(source, target, true);	}

//-----------------------------------------------------------------------

int Graph::TotalNumberOfIncomingEdges (const Vertex &v) const 
	{ return GetNumberOfVertices(v, false);	}


//-----------------------------------------------------------------------

int	Graph::TotalNumberOfOutgoingEdges (const Vertex &v) const 
	{ return GetNumberOfVertices(v, true);	}

//-----------------------------------------------------------------------

bool Graph::AddEdge (const Vertex &source, const Vertex &target) {
	bool outExists =  false;
	if (!(outExists = OutgoingEdgeExists(source, target))) {
		++edgesNum;
		table[source].first.push_back(&(const_cast<Vertex & >(target)));
	}

	if (!outExists && !IncomingEdgeExists(target, source)) 
		table[target].second.push_back(&(const_cast<Vertex & >(source)));
	
	return !outExists;
}

//-----------------------------------------------------------------------

bool Graph::DeleteAllEdgesThatContainsVertex (const Vertex &v) {
	undoRedo.NewUndoAction();
	return DeleteAllEdgesThatContainsVertexImplementation(v);
}

//-----------------------------------------------------------------------

bool Graph::DeleteEdge (const Vertex &source, const Vertex &target) {
	undoRedo.NewUndoAction();
	return DeleteEdgeImplementation(source, target, true);
}

//-----------------------------------------------------------------------

void Graph::MarkVertexAsDeleted (Vertex &v) {
	assert(VertexExists(v));
	v.nullObject = true;
}

void Graph::UnmarkVertexAsDeleted (Vertex &v) {
	assert(VertexExists(v) && !v);
	v.nullObject = false;
}

//-----------------------------------------------------------------------

const Vertex & Graph::NewVertex (void) {
	++verticesNum;
	Vertex * v = new Vertex(nextVertexId++);
	allVertices.push_back(v);
	table.push_back(std::make_pair (VerticesPtrList(), VerticesPtrList()));
	return *v;
}

//-----------------------------------------------------------------------

const Vertex & Graph::GetVertex (const id_t & id) const{
	if (id <= (id_t) TotalNumberOfVertices())
		return *allVertices[id];
	return Vertex::BadVertex ();
}

//-----------------------------------------------------------------------

const Vertex::VerticesPtrList & Graph::GetIncomingEdges (const Vertex &v) const 
	{ return GetEdges(v, false); }

//-----------------------------------------------------------------------

const Vertex::VerticesPtrList & Graph::GetOutgoingEdges (const Vertex &v) const 
	{ return GetEdges(v, true); }



////////////////////////////////////////////
//	implementation of private methods
const Vertex::VerticesPtrList &  Graph::GetEdges(const Vertex & v, bool isOutEdge) const {
	assert(VertexExists(v));
	return isOutEdge ? table[v].first : table[v].second;
}

//-----------------------------------------------------------------------

int Graph::GetNumberOfVertices (const Vertex & v, bool isOutEdge) const {
	assert(VertexExists(v));
	return (int)(isOutEdge ? table[v].first.size() : table[v].second.size());
}

//-----------------------------------------------------------------------

bool Graph::EdgeExists (const Vertex & source, const Vertex &target, bool isOutEdge) const {
	assert(VertexExists(source) && VertexExists(target));	

	const VerticesPtrList & list = isOutEdge ? table[source].first : table[source].second;
	VerticesPtrList::const_iterator vli = std::find_if (	
											list.begin(),	
											list.end(),		
											utility::EqualityFunctor<Vertex>(target)
										);											
		return vli != list.end();
}

//-----------------------------------------------------------------------

void Graph::ConditionalEraseVertexFromList (
		VerticesPtrList &	list, 
		const bool			isOutgoingList, 
		const Vertex *		ownerOfList,
		const Vertex *		toDelete
	)  {
	VerticesPtrList::iterator v = std::find (
									list.begin(),
									list.end(),
									toDelete
								);
	
	if (v != list.end()) {
		if (isOutgoingList) {
			--edgesNum;
			undoRedo.AddInUndoStack(std::make_pair (ownerOfList, toDelete));
		}
		list.erase(v);
	}

}

//-----------------------------------------------------------------------

bool Graph::DeleteAllEdgesThatContainsVertexImplementation (const Vertex &v) {
	bool result = false;

	if (VertexExists(v)){
		edgesNum -= table[v].first.size();
		std::for_each (table[v].first.begin(), table[v].first.end(), AddInUndoStackFunctor(&v, undoRedo));
		table[v].first.clear();
		table[v].second.clear();

		for (VerticesPtrVec::iterator vertex = allVertices.begin(); vertex != allVertices.end(); ++vertex)
			if (table.ContainsVertex(**vertex)) {
				ConditionalEraseVertexFromList(table[*vertex].first, true, *vertex, &v);
				ConditionalEraseVertexFromList(table[*vertex].second, false, *vertex, &v);
				table[*vertex].second;
			}
		result = true;
	}
	return result;
}

//-----------------------------------------------------------------------

bool Graph::DeleteEdgeImplementation (const Vertex &source, const Vertex &target, const bool & canAddInUndoStack) {
	bool result = false;
	if (table.ContainsVertex(source) && table.ContainsVertex(target)) {

		//find the position of target in outgoing list
		VerticesPtrList::iterator i = FIND_VERTEX(table[source].first, target);
		
		if (i != table[source].first.end()) {
			if (canAddInUndoStack)
				undoRedo.AddInUndoStack(std::make_pair (&source, &target));

			--edgesNum;
			table[source].first.erase(i);					//erase from outgoing list

			i = FIND_VERTEX(table[target].second, source);		
			assert(i != table[target].second.end());		//always must exist this vertex
			table[target].second.erase(i);					//erase from incoming list
			result = true;
		}
	}

	return result;
}


////////////////////////////////////////////
//	implementation of iner class


///--------------------------------- GraphTable
//-----------------------------------------------------------------------

#define RETURN_ELEMENTS_OF_VERTEX(V)				\
	assert(V.Id() < static_cast<unsigned>(size()));	\
	return at(V.Id())

//-----------------------------------------------------------------------

Graph::VerticesListPair & Graph::GraphTable::operator [](const Vertex &v)
	{ RETURN_ELEMENTS_OF_VERTEX(v);	}

//-----------------------------------------------------------------------

const Graph::VerticesListPair & Graph::GraphTable::operator [](const Vertex &v) const
	{ RETURN_ELEMENTS_OF_VERTEX(v);	}

//-----------------------------------------------------------------------

Graph::VerticesListPair & Graph::GraphTable::operator[] (const Vertex * v) {
	assert(v);
	RETURN_ELEMENTS_OF_VERTEX((*v));
}

//-----------------------------------------------------------------------

const Graph::VerticesListPair & Graph::GraphTable::operator[] (const Vertex * v) const {
	assert(v);
	RETURN_ELEMENTS_OF_VERTEX((*v));
}

//-----------------------------------------------------------------------

bool Graph::GraphTable::ContainsVertex (const Vertex & v) const 
	{ return v.Id() < size(); }


///-------------------------------- UndoRedoHelperGraphImplementation

Graph::UndoRedoHelperForGraph::UndoRedoHelperForGraph ()
	:	UndoRedoHelper() {}

//-----------------------------------------------------------------------

void Graph::UndoRedoHelperForGraph::AddInUndoStack (const Edge & edge) {	
	assert(!undoStack.empty());
	undoStack.back().push_back(edge); 
}


////////////////////////////////////////////
//	implementation of extren function

//-----------------------------------------------------------------------

#define WRITE_DATA_OF_LIST_TO_STREAM(LIST)													\
	for(Vertex::VerticesPtrList::const_iterator j = LIST.begin(); j != LIST.end(); ++j )	\
		stream << id << " ----> " << (*j)->Id() << 	"\n"

//-----------------------------------------------------------------------

std::ostream & operator<< (std::ostream & stream, const Graph & graph){
	stream << "Graph: total number of vertices(" << utility::string_cast (graph.verticesNum) << ")\n";

	id_t id = 0;
	Graph::GraphTable::const_iterator i;
	for (i = graph.table.begin(); i != graph.table.end(); ++i, ++id){
		stream << "\nVertex: " << id << "\noutgoing edges: \n";
		WRITE_DATA_OF_LIST_TO_STREAM(i->first);
		stream << "\nincoming edges: \n";
		WRITE_DATA_OF_LIST_TO_STREAM(i->second);
	}
	return stream;
}

}	//namespace graphs

}	//namespace iviews
