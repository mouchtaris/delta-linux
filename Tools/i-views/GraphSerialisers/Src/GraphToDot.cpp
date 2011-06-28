/**
 *	GraphToDot.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	April 2010
 */

#include <fstream>
#include <assert.h>
#include "GraphToDot.h"
#include "StringUtils.h"

namespace iviews {

GraphToDot::GraphToDot() :  
	hasDebugInfo	(false), 
	displayContens	(false),
	debugInfo		((VertexToDebugInfoMapping *)0) 
{}


//-----------------------------------------------------------------------

std::string GraphToDot::operator() (const Graph & g, const Vertex & start) {
	std::string	dotRepresentation			= "digraph G {\n";
	const Vertex::VerticesPtrVec & vertices	= g.GetAllVertices();
	
	Vertex::VerticesPtrVec::const_iterator i;
	for(i = vertices.begin(); i != vertices.end(); ++i)
		AppendToStringAllOutgoingEdges(g, *i, dotRepresentation);

	for(i = vertices.begin(); displayContens && (i != vertices.end()); ++i)
		AddContentsOfVertexInDotFormatToString(*i, dotRepresentation);

	dotRepresentation.append("\t"									+ 
							 CretateVertexDotRepresentation(const_cast<Vertex *>(&start))	+ 
							 " [style=filled, color=red];\n}");

	return dotRepresentation;	
}

//-----------------------------------------------------------------------

std::string GraphToDot::operator() (
		const Graph&					g, 
		const Vertex&					start,
		const bool						_displayContens,
		const VertexToDebugInfoMapping& debugInfo
	) {
	hasDebugInfo					= true;
	displayContens					= _displayContens;
	this->debugInfo					= const_cast<VertexToDebugInfoMapping *>(&debugInfo);
	
	std::string	dotRepresentation	= this->operator ()(g, start);
	
	displayContens					= false;
	hasDebugInfo					= false;

	return dotRepresentation;
}

//-----------------------------------------------------------------------

bool GraphToDot::WriteToFile(
		const std::string&	fileName,
		const Graph&		g,
		const Vertex&		start
	) {
	bool result = false;
	
	std::fstream filestr(fileName.c_str(), std::fstream::out);
	if (filestr.is_open()) {
		filestr << this->operator ()(g, start);
		filestr.close();
		result = true;
	}
	
	return result;
}

//-----------------------------------------------------------------------

bool GraphToDot::WriteToFile(
		const std::string&				fileName, 
		const Graph&					g, 
		const Vertex&					start,	
		const bool						_displayContens,
		const VertexToDebugInfoMapping& debugInfo
	) {
	hasDebugInfo	= true;
	displayContens	= _displayContens;
	this->debugInfo	= const_cast<VertexToDebugInfoMapping *>(&debugInfo);

	bool result		= WriteToFile(fileName, g, start);

	displayContens	= false;
	hasDebugInfo	= false;

	return result;
}


////////////////////////////////////////////
//	implementation of private method(s)


//-----------------------------------------------------------------------

void GraphToDot::AddContentsOfVertexInDotFormatToString(
		Vertex *		v, 
		std::string &	dotRepresentation
	){
	assert(displayContens);
	assert(v);
	VertexToDebugInfoMapping::const_iterator i = debugInfo->find(v);
	assert(i != debugInfo->end());

	const SlotDebugInfoList & debugSlots = i->second.GetAllSlots();

	std::string vertexDotRep = CretateVertexDotRepresentation(v);
	dotRepresentation.append( "\t" + vertexDotRep + "[\n\t\tshape=box, label=\"AbsRef: \\" + vertexDotRep);
	dotRepresentation.erase(dotRepresentation.size()-1, dotRepresentation.size()); //delete the character " of vertexDotRep
	dotRepresentation.append("\\\"\\n");
	
	for(SlotDebugInfoList::const_iterator j = debugSlots.begin(); j != debugSlots.end(); ++j) 
		dotRepresentation.append(
			"key: <" + 
			j->GetKeyString()+ 
			">, value: <" + 
			j->GetContentString()+ 
			">\\n"
		);
	
	dotRepresentation.append("\"\n\t]\n");
}

//-----------------------------------------------------------------------

std::string GraphToDot::CretateVertexDotRepresentation(Vertex * v) {
	std::string vertexRepresentation;

	if (hasDebugInfo){
		VertexToDebugInfoMapping::const_iterator i = debugInfo->find(v);
		assert(i != debugInfo->end());
		vertexRepresentation = "\"" + i->second.GetAbsolutRef() + "\"";
	}
	else
		vertexRepresentation = "vertex" + utility::string_cast(v->Id());

	return vertexRepresentation;
}

//-----------------------------------------------------------------------

void GraphToDot::AppendToStringAllOutgoingEdges( 
		const Graph	&	graph, 
		Vertex	*		v, 
		std::string	&	dotRepresentation
	) {
	const Vertex::VerticesPtrList outgoing = graph.GetOutgoingEdges(*v);
	
	Vertex::VerticesPtrList::const_iterator i;
	for(i = outgoing.begin(); i != outgoing.end(); ++i) {
		dotRepresentation += "\t"								+ 
							 CretateVertexDotRepresentation(v)	+
							 " -> "								+
							 CretateVertexDotRepresentation(*i)+
							 "\n";
	}
}

}	//namespace iviews

