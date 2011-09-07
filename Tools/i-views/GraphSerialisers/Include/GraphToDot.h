/**
 *	GraphToDot.h
 *	
 *	Create a string or write to file a Graph in dot representation
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	April 2010
 */

#ifndef GRAPH_TO_DOT_H
#define GRAPH_TO_DOT_H

#include <string>
#include "Graph.h"
#include "Vertex.h"
#include "ConvertToGraph.h"
#include "DebugInfo.h"
#include "SlotDebugInfo.h"

namespace iviews {

class GraphToDot {
public:
	typedef graphs::Graph						Graph;
	typedef graphs::Vertex						Vertex;
	typedef graphs::VertexToDebugInfoMapping	VertexToDebugInfoMapping;

	///---- Constractor(s)
	GraphToDot ();


	///////////////////////////////////////////////////
	//	public methods

	bool WriteToFile (
			const std::string& fileName, 
			const Graph&		g, 
			const Vertex&		start
		);		

	bool WriteToFile (
			const std::string&					fileName, 
			const Graph&						g,
			const Vertex&						start,
			const bool							_displayContens,
			const VertexToDebugInfoMapping&		debugInfo
		);


	///////////////////////////////////////////////////
	//	overloaded operator(s)
	std::string operator() (const Graph & g, const Vertex & start);
	
	std::string operator() (
					const Graph&					g,
					const Vertex&					start,
					const bool						_displayContens,
					const VertexToDebugInfoMapping&	
				);
	

private:
	///////////////////////////////////////////////////
	//	private typedefs
	typedef dinfo::SlotDebugInfo					 SlotDebugInfo;
	typedef dinfo::DebugInfo::SlotDebugInfoList		 SlotDebugInfoList;

	///////////////////////////////////////////////////
	//	private methods
	void		AddContentsOfVertexInDotFormatToString (Vertex * v, std::string & dotRepresentation);
	std::string CretateVertexDotRepresentation (Vertex * v);
	void		AppendToStringAllOutgoingEdges ( 
					const Graph &	graph, 
					Vertex *		v, 
					std::string &	dotRepresentation
				);


	///////////////////////////////////////////////////
	//	private members
	bool hasDebugInfo;
	bool displayContens;
	const VertexToDebugInfoMapping * debugInfo;

};

}	//namespace iviews

#endif	//GRAPH_TO_DOT_H