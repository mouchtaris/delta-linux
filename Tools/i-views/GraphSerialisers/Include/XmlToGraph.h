/**
 *	XmlToGraph.h
 *	
 *	Read a Graph from an XML file
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	July 2010
 */

#ifndef XML_TO_GRAPH_H
#define XML_TO_GRAPH_H

#include <string>
#include <wx/xml/xml.h>

#include "Graph.h"
#include "Vertex.h"
#include "DebugInfo.h"
#include "SlotDebugInfo.h"
#include "ConvertToGraph.h"

namespace iviews {

class XmlToGraph {
public:
	typedef graphs::Graph						Graph;
	typedef graphs::Vertex						Vertex;
	typedef graphs::VertexToDebugInfoMapping	VertexToDebugInfoMapping;

	///---- Constractor(s)
	XmlToGraph ();


	///////////////////////////////////////////////////
	//	overloaded operator(s)

	bool			operator() (const std::string & path);
	
	long			GetDepth (void) const;
	Graph *			GetGraph (void) const;
	Vertex *		GetStartVertex (void) const;
	bool			HasError (void) const;
	const std::string &			
					GetErrorMessage (void) const;
	VertexToDebugInfoMapping *	
					GetVertexToDebugInfoMapping (void) const;


private:
	///////////////////////////////////////////////////
	//	private typedefs
	typedef dinfo::SlotDebugInfo				SlotDebugInfo;
	typedef dinfo::DebugInfo::SlotDebugInfoList	SlotDebugInfoList;


	/////////////////////////////////////////////////////
	////	private methods
	void			Init (void);
	bool			SetErrorMessage (const std::string & message);
	long			ParseLong (const wxString & value);
	double			ParseDouble (const wxString & value);
	bool			ParseBoolean (const wxString & value);			
	void 			ParseRoot (wxXmlNode * root);
	void 			ParseVertex (wxXmlNode * vertex);
	void 			ParseDebugInfo (const Vertex * source, wxXmlNode * dInfo);
	void 			ParseOutgoingEdges (const Vertex * source, wxXmlNode * edges);
	void			ParseSlotDebugInfo (dinfo::DebugInfo & debugInfo, wxXmlNode * slotDebugInfo);
	id_t			GetId (const wxString & id);
	id_t			GetId (wxXmlNode * vertex);
	Vertex *		GetVertex (const wxString & id);
	const Vertex *	GetVertex (const id_t & id);
	
	///////////////////////////////////////////////////
	//	private members
	long			depth;
	long			edgesNum;
	long			verticesNum;
	bool			hasError;
	std::string 	errorMessage;

	Graph	* 		graph;
	Vertex	* 		startVertex;
	VertexToDebugInfoMapping *	mapping;
};

}	//namespace iviews

#endif	//XML_TO_GRAPH_H