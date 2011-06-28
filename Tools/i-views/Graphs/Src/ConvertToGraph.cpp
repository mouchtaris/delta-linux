/**
 *	ConvertToGraph.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	March 2010
 */

#include <assert.h>

#include "DebugInfo.h"
#include "ConvertToGraph.h"

namespace iviews {
namespace graphs {

//-----------------------------------------------------------------------

ConvertToGraph::ConvertToGraph (void) :	
	nextSlotId				(0),
	graph					(static_cast<Graph *>(0)),
	vertToDebugInfoMapping	(static_cast<VertexToDebugInfoMapping *>(0)),
	hasError				(false),
	isSimpleObj				(false),
	errorMessage			(""),													
	descriptionOfSimpleObj	("")
{}

//-----------------------------------------------------------------------
//caller is responsible to delete the instance
VertexToDebugInfoMapping * ConvertToGraph::GetDebugInfo () const			
	{ return vertToDebugInfoMapping; }

//-----------------------------------------------------------------------

Vertex * ConvertToGraph::GetStartVertex (void) const					
	{ return startVertex; }

//-----------------------------------------------------------------------

bool ConvertToGraph::HasError (void) const						
	{ return hasError; }

//-----------------------------------------------------------------------

bool ConvertToGraph::IsSimpleObject (void) const					
	{ return isSimpleObj; }

//-----------------------------------------------------------------------

std::string ConvertToGraph::GetDescriptionOfSimpleObject (void) const	
	{ return descriptionOfSimpleObj; }

//-----------------------------------------------------------------------

std::string	ConvertToGraph::GetErrorMessage (void) const					
	{ return errorMessage; }

//-----------------------------------------------------------------------

Graph * ConvertToGraph::operator() (const ObjectGraphData & ogd) {
	Init();
	
	ObjectGraphMap m = ogd.get<0>();
	size_t s = ogd.get<0>().size();

	if (ogd.get<2>()) {					//not error
		if (!ogd.get<1>().compare(""))	//composite objet
			graph = ProduceGraphFromCompositeExpression(ogd);
		else {							//simple object
			isSimpleObj				= true;
			descriptionOfSimpleObj	= ogd.get<1>();		
		}
	}
	else {								//error
		hasError		= true;
		errorMessage	= ogd.get<1>();
		if (!errorMessage.size())
			errorMessage = "Probably did not start a debugging season";
	}

	return graph;
}

//-----------------------------------------------------------------------

void ConvertToGraph::Init(void) {
	nextSlotId				= 0;													
	graph					= static_cast<Graph *>(0);								
	vertToDebugInfoMapping	= static_cast<VertexToDebugInfoMapping *>(0);	
	hasError				= false;												
	isSimpleObj				= false;												
	errorMessage			= "";													
	descriptionOfSimpleObj	= "";													
	objToVertMapping.clear();
}


//-----------------------------------------------------------------------

Vertex * ConvertToGraph::GetNewOrExistingVertex(
			const std::string & refrence, 
			const ObjectGraphMap & objGraphMap
	) {
	assert(graph);
	assert(vertToDebugInfoMapping);
	
	Vertex * vertex = static_cast<Vertex *>(0);
	ObjectRefToVertexMapping::iterator i;

	//Den exei dimiourgi8ei autos o kombos
	if ( (i = objToVertMapping.find(refrence)) == objToVertMapping.end()) {	
		//O kombos pou 8a dimourgi8ei 8a prepei na uparxei ston arxiko grafo
		if (objGraphMap.find(refrence) != objGraphMap.end()) {
			vertex = const_cast<Vertex *>(&graph->NewVertex());
			objToVertMapping[refrence]	= vertex;
			vertToDebugInfoMapping->insert(std::make_pair(vertex, dinfo::DebugInfo(refrence)));
			assert(graph->TotalNumberOfVertices() == vertToDebugInfoMapping->size());
		}
	}
	else
		vertex = i->second;
	
	return vertex;
}

//-----------------------------------------------------------------------

Graph * ConvertToGraph::ProduceGraphFromCompositeExpression(const ObjectGraphData & ogd) {
	assert(!ogd.get<1>().compare("") && ogd.get<2>());
	
	graph								= new Graph();
	vertToDebugInfoMapping				= new VertexToDebugInfoMapping();
	const ObjectGraphMap & objGraphMap	= ogd.get<0>();

	size_t s = objGraphMap.size();

	ObjectGraphMap::const_iterator i	= objGraphMap.begin();

	for (; i != objGraphMap.end(); ++i){
		if (i->first == "_<STARTNODE>_"){	//we have a pseudo node
			ObjectNodeList::const_iterator j = i->second.begin();
			assert(j != i->second.end());
			startVertex =  GetNewOrExistingVertex((*j).get<2>(), objGraphMap);
		}
		else {
			Vertex * parent = GetNewOrExistingVertex(i->first, objGraphMap);
			assert(parent);
			ConditionalCreateAndConectChildrenWithParent(*parent, i->second, objGraphMap);
		}
	}

	assert(graph->TotalNumberOfVertices() == s-1);
	return graph;
}

//-----------------------------------------------------------------------

void ConvertToGraph::ConditionalCreateAndConectChildrenWithParent(	
		const Vertex & parent, 
		const ObjectNodeList & contents,
		const ObjectGraphMap & objGraphMap
	) {
	ObjectNodeList::const_iterator i = contents.begin();

	for(; i != contents.end(); ++i){
		Vertex * key	= static_cast<Vertex *>(0);
		Vertex * value	= static_cast<Vertex *>(0);

		//key is compisite object
		if ((*i).get<1>() &&  (key = GetNewOrExistingVertex((*i).get<0>(), objGraphMap)) != (Vertex *)0)
			graph->AddEdge(parent, *(key));

		//value is composite object
		if ((*i).get<3>() && (value = GetNewOrExistingVertex((*i).get<2>(), objGraphMap)) != (Vertex *)0)
			graph->AddEdge(parent, *(value));

		VertexToDebugInfoMapping::iterator j = vertToDebugInfoMapping->find(const_cast<Vertex *>(&parent));
		if ( j != vertToDebugInfoMapping->end() )
			j->second.AddSlot(		
						nextSlotId++,
						(*i).get<1>(),	//is key composite
						(*i).get<0>(),	//key string
						(*i).get<4>(),	//key type information
						key,			//target vertex for the key if exists
						(*i).get<3>(),	//is content composite
						(*i).get<2>(),	//content string
						value			//target vertex for the value if exists
					);			
		else
			assert(0);
	}
}

}	//namespace graphs

}	//namespace iviews