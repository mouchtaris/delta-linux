/**
 *	GraphToXml.h
 *	
 *	Save a Graph In XML Format
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	July 2010
 */

#ifndef GRAPH_TO_XML_H
#define GRAPH_TO_XML_H

#include <map>
#include <string>
#include "Graph.h"
#include "Vertex.h"
#include "DebugInfo.h"
#include "SlotDebugInfo.h"
#include "ConvertToGraph.h"

namespace iviews {

class GraphToXml {
public:
	typedef graphs::Graph						Graph;
	typedef graphs::Vertex						Vertex;
	typedef graphs::VertexToDebugInfoMapping	VertexToDebugInfoMapping;

	///---- Constractor (s)
	GraphToXml ();

	///////////////////////////////////////////////////
	//	overloaded operator (s)

	bool operator () (
			const std::string& 				path, 
			const long			  			depth,	
			const Graph	& 					g,
			const Vertex& 					start,
			const VertexToDebugInfoMapping&	debugInfo
		);


private:
	///////////////////////////////////////////////////
	//	private typedefs
	typedef Vertex::VerticesPtrVec				VerticesPtrVec;
	typedef Vertex::VerticesPtrList				VerticesPtrList;
	typedef dinfo::SlotDebugInfo				SlotDebugInfo;
	typedef dinfo::DebugInfo::SlotDebugInfoList	SlotDebugInfoList;


	/////////////////////////////////////////////////////
	////	private methods
	void		Init (void);
	std::string GetGraphToXml (void);			
	std::string GetGraphAttributes (void);
	std::string GetVertexAttributes (const Vertex * v);
	std::string GetDebugInfoAttributes (const dinfo::DebugInfo & info);
	std::string	GetVertexXmlString (const Vertex * v);
	std::string GetDebugInfoXmlString (const Vertex * v);
	std::string GetSlotDebugInfoXmlString (const dinfo::SlotDebugInfo & slot);
	std::string GetVertexOutgoingEdgesString (const Vertex * v);



	/////////////////////////////////////////////////////
	////	private members
	long								depth;
	const Graph *						graph;
	const Vertex *						startVertex;
	const VertexToDebugInfoMapping *	dInfo;
		


};

}	//namespace iviews

#endif	//GRAPH_TO_XML_H