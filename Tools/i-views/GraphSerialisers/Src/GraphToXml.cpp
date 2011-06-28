


#include <fstream>
#include <assert.h>

#include "StringUtils.h"
#include "GraphToXml.h"
#include "XmlEscape.h"


#define ONE_TAB		std::string("\t")
#define TWO_TABS	std::string("\t\t")
#define THREE_TABS	std::string("\t\t\t")	
#define XML_INFO	std::string("<?xml version=\"1.0\" encoding=\"utf-8\" ?>")

#define GET_STRINGFIELD_VALUE(VALUE)									\
	std::string(														\
		"\""														+	\
		XmlEscape::StringToXMLString(utility::string_cast(VALUE))	+	\
		"\""															\
	)


namespace iviews {

GraphToXml::GraphToXml () 
	{ Init(); }

//-----------------------------------------------------------------------

bool GraphToXml::operator () (
		const std::string& path, 
		const long						depth_,	
		const Graph&					g,
		const Vertex&					start,
		const VertexToDebugInfoMapping& debugInfo
	) {
	Init();
	bool result = false;
	
	std::fstream filestr(path.c_str(), std::fstream::out);
	if (filestr.is_open()) {
		depth		= depth_;
		graph		= &g;
		dInfo		= &debugInfo;
		startVertex = &start;
		
		filestr << GetGraphToXml();
		filestr.close();
		result = true;
	}
	
	return result;

}

//-----------------------------------------------------------------------

void GraphToXml::Init (void) {
	depth		= -1;
	graph		= static_cast<Graph *>(0);
	dInfo		= static_cast<VertexToDebugInfoMapping *>(0);
	startVertex = static_cast<Vertex *>(0);
	XmlEscape::Initialise();
}

//-----------------------------------------------------------------------

std::string GraphToXml::GetGraphToXml (void) {
	std::string graphXMLstring = XML_INFO + "\n<Graph" + GetGraphAttributes() + ">\n";

	VerticesPtrVec vertices = graph->GetAllVertices();
	for(VerticesPtrVec::const_iterator v = vertices.begin(); v != vertices.end(); ++v)
		if (**v)
			graphXMLstring += GetVertexXmlString(*v);

	return graphXMLstring + "</Graph>";
}

//-----------------------------------------------------------------------

std::string GraphToXml::GetGraphAttributes (void) {
	return std::string(
				" edgesNum="	+ GET_STRINGFIELD_VALUE(graph->TotalNumbersOfEdges())	+
				" verticesNum="	+ GET_STRINGFIELD_VALUE(graph->TotalNumberOfVertices())	+
				" startVertex="	+ GET_STRINGFIELD_VALUE(startVertex->Id())				+
				" depth="		+ GET_STRINGFIELD_VALUE(depth)
			);
}

//-----------------------------------------------------------------------

std::string	GraphToXml::GetVertexXmlString (const Vertex * v) {
	assert(v);

	return std::string(
				ONE_TAB			+ 
				"<Vertex "		+ GetVertexAttributes(v)				+ ">\n"				+ 
				TWO_TABS		+ 
				"<Outgoing>"	+ GetVertexOutgoingEdgesString(v)		+ "</Outgoing>\n"	+ 
				TWO_TABS		+ GetDebugInfoXmlString(v)				+ ONE_TAB			+
				"</Vertex>\n"
			);
}

//-----------------------------------------------------------------------

std::string GraphToXml::GetVertexOutgoingEdgesString (const Vertex * v) {
	assert(v);
	std::string verticesString;

	const VerticesPtrList & edges = graph->GetOutgoingEdges(*v);
	for(VerticesPtrList::const_iterator vertex = edges.begin(); vertex != edges.end(); ++vertex)
		verticesString += utility::string_cast((*vertex)->Id()) + " ";

	return verticesString;
}

//-----------------------------------------------------------------------

std::string GraphToXml::GetDebugInfoXmlString (const Vertex * v) {
	assert(v);
	VertexToDebugInfoMapping::const_iterator i = dInfo->find(const_cast<Vertex *>(v));
	assert(i != dInfo->end());

	std::string debugInfoString = TWO_TABS + "<DebugInfo " + GetDebugInfoAttributes(i->second) + ">\n";
		
	const SlotDebugInfoList & slots = i->second.GetAllSlots();
	for(SlotDebugInfoList::const_iterator slot = slots.begin(); slot != slots.end(); ++slot)
		debugInfoString += THREE_TABS + GetSlotDebugInfoXmlString(*slot);

	return debugInfoString + TWO_TABS + "</DebugInfo>\n";
}

//-----------------------------------------------------------------------

std::string GraphToXml::GetSlotDebugInfoXmlString (const dinfo::SlotDebugInfo & slot) {
	Vertex * v = static_cast<Vertex *>(0);
	std::string keyVertexId		= ((v = slot.GetKeyTargetVertex()) ? utility::string_cast(v->Id()): "null");
	std::string contentVertexId = ((v = slot.GetContentTargetVertex()) ? utility::string_cast(v->Id()): "null");;
	
	return std::string(
			"<SlotDebugInfo id="	+ GET_STRINGFIELD_VALUE(slot.Id())							+
			" stringId="			+ GET_STRINGFIELD_VALUE(slot.GetIdRelevantToDebugInfo())	+						+
			" isKeyAggregate="		+ GET_STRINGFIELD_VALUE(slot.IsKeyAggregate())				+
			" keyString="			+ GET_STRINGFIELD_VALUE(slot.GetKeyString())				+
			" keyTypeInfo="			+ GET_STRINGFIELD_VALUE(slot.GetKeyTypeInfo())				+
			" keyTargetVertex="		+ GET_STRINGFIELD_VALUE(keyVertexId)						+
			" isContentAggergate="	+ GET_STRINGFIELD_VALUE(slot.IsContentAggregate())			+
			" contentString="		+ GET_STRINGFIELD_VALUE(slot.GetContentString())			+
			" contentTargetVertex="	+ GET_STRINGFIELD_VALUE(contentVertexId)					+
			"/>\n"
		);
}

//-----------------------------------------------------------------------

std::string GraphToXml::GetVertexAttributes (const Vertex * v) {
	assert(v);
	return "id=" + GET_STRINGFIELD_VALUE(v->Id());
}

//-----------------------------------------------------------------------

std::string GraphToXml::GetDebugInfoAttributes (const dinfo::DebugInfo & info) 
	{	return "absRef=" + GET_STRINGFIELD_VALUE(info.GetAbsolutRef());	}

}	//namespace iviews

