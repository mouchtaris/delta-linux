#include "XmlToGraph.h"



#include <wx/msgdlg.h>
#include <wx/tokenzr.h>


#include <utility>
#include <fstream>
#include <assert.h>
#include "StringUtils.h"
#include "TypesConverter.h"

namespace iviews {

#define CHAECK_FOR_ERRORS()											\
	if (!HasError()) {												\
		if (graph->TotalNumberOfVertices() != verticesNum)			\
			SetErrorMessage("With the total number of vertices");	\
		if (graph->TotalNumbersOfEdges() != edgesNum)				\
			SetErrorMessage("With the total number of edges");		\
		if (depth < 0)												\
			SetErrorMessage("With the depth");						\
	} else

//-----------------------------------------------------------------------

XmlToGraph::XmlToGraph() 
	{ Init(); }

//-----------------------------------------------------------------------

bool XmlToGraph::operator() (const std::string & path){
	Init();
	wxXmlDocument doc;
	if(!doc.Load(std2str(path), wxT("UTF-8"), wxXMLDOC_KEEP_WHITESPACE_NODES)) 
		return SetErrorMessage("While opening xml file: '" + path + "'");

	ParseRoot(doc.GetRoot());

	for(wxXmlNode * child = doc.GetRoot()->GetChildren(); 
		child && !HasError(); 
		child = child->GetNext())
		if (child->GetName() == wxT("Vertex")) 
			ParseVertex(child);
	
	CHAECK_FOR_ERRORS();

	if (HasError()) {
		delete graph;
		delete mapping;
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------

long XmlToGraph::GetDepth(void) const						
	{ return depth; }

//-----------------------------------------------------------------------

bool XmlToGraph::HasError(void) const
	{ return hasError; }

//-----------------------------------------------------------------------

const std::string &	XmlToGraph::GetErrorMessage(void) const
	{ return errorMessage; }

//-----------------------------------------------------------------------

graphs::Graph * XmlToGraph::GetGraph(void) const						
	{ return graph; }

//-----------------------------------------------------------------------

graphs::Vertex * XmlToGraph::GetStartVertex(void) const
	{ return startVertex; }

//-----------------------------------------------------------------------

graphs::VertexToDebugInfoMapping * XmlToGraph::GetVertexToDebugInfoMapping(void) const
	{ return mapping; }


////////////////////////////////////////////
//	Private method API implementation

//-----------------------------------------------------------------------

void XmlToGraph::Init(void) {
	depth		= -1;
	edgesNum	= -1;
	hasError	= false;
	verticesNum	= -1;

	errorMessage= "";
	graph		= static_cast<Graph *>(0);
	startVertex = static_cast<Vertex *>(0);
	mapping		= static_cast<VertexToDebugInfoMapping*>(0);
}

//-----------------------------------------------------------------------

bool XmlToGraph::SetErrorMessage(const std::string & message) {
	hasError		= true;
	errorMessage	= message;
	return false;
}

//-----------------------------------------------------------------------

void XmlToGraph::ParseRoot(wxXmlNode * root) {
	id_t startVertexId;
	wxXmlProperty * properties = root->GetProperties();

	assert(properties);

	while(properties) {
		if (properties->GetName() == wxT("edgesNum"))
			edgesNum = ParseLong(properties->GetValue());
		
		else 
		if (properties->GetName() == wxT("verticesNum"))
			verticesNum = ParseLong(properties->GetValue());

		else 
		if (properties->GetName() == wxT("startVertex"))
			startVertexId = ParseLong(properties->GetValue());

		else 
		if (properties->GetName() == wxT("depth"))
			depth = ParseLong(properties->GetValue());
		else {
			SetErrorMessage("Unsupported xml file");
			return;
		}

		properties = properties->GetNext();
	}

	assert(edgesNum >= 0 && verticesNum > 0 && startVertexId >= 0 && depth >= 0);
	graph		= new Graph(verticesNum);
	mapping		= new VertexToDebugInfoMapping();
	startVertex = const_cast<Vertex *>(GetVertex(startVertexId));
	assert(*startVertex);
}

//-----------------------------------------------------------------------

void XmlToGraph::ParseVertex(wxXmlNode * vertex) {
	const Vertex * source;

	source = GetVertex(GetId(vertex));
	
	for(wxXmlNode * child = vertex->GetChildren(); child; child = child->GetNext())	{
		if (child->GetName() == wxT("Outgoing"))
			ParseOutgoingEdges(source, child);
		else if (child->GetName() == wxT("DebugInfo"))
			ParseDebugInfo(source, child);
	}
}

//-----------------------------------------------------------------------

void XmlToGraph::ParseOutgoingEdges(const Vertex * source, wxXmlNode * edges) {
	wxStringTokenizer tkz(edges->GetNodeContent(), wxT(" "));

	while(tkz.HasMoreTokens())
		graph->AddEdge(*source, *GetVertex(tkz.GetNextToken()));
}

//-----------------------------------------------------------------------

void XmlToGraph::ParseDebugInfo(const Vertex * source, wxXmlNode * dInfo) {
	assert(dInfo);
	assert(dInfo->HasProp(wxT("absRef")));
	
	dinfo::DebugInfo debugInfo(str2std(dInfo->GetPropVal(wxT("absRef"), wxT(""))));
		
	for(wxXmlNode * slotDebugInfo = dInfo->GetChildren(); 
		slotDebugInfo; 
		slotDebugInfo = slotDebugInfo->GetNext())
		if (slotDebugInfo->GetName() == wxT("SlotDebugInfo"))
			ParseSlotDebugInfo(debugInfo, slotDebugInfo);
	mapping->insert(std::make_pair(const_cast<Vertex *>(source), debugInfo));
}

//-----------------------------------------------------------------------

void XmlToGraph::ParseSlotDebugInfo(dinfo::DebugInfo & debugInfo, wxXmlNode * slotDebugInfo) {
	SlotDebugInfo slotDInfo;
	wxXmlProperty * properties = slotDebugInfo->GetProperties();

	while(properties) {

		if (properties->GetName() == wxT("id") )
			slotDInfo.SetId(ParseLong(properties->GetValue()));
		
		else if (properties->GetName() == wxT("stringId") )
			slotDInfo.SetIdRelevantToDebugInfo(str2std(properties->GetValue()));

		else if (properties->GetName() == wxT("isKeyAggregate") )
			slotDInfo.KeyAggregate(ParseBoolean(properties->GetValue()));

		else if (properties->GetName() == wxT("keyString") )
			slotDInfo.SetKeyString(str2std(properties->GetValue()));
		
		else if (properties->GetName() == wxT("keyTypeInfo") )
			slotDInfo.SetKeyTypeInfo( (dinfo::KeyTypeInformation) ParseLong(properties->GetValue()));
		
		else if (properties->GetName() == wxT("keyTargetVertex") )
			slotDInfo.SetKeyTargetVertex(GetVertex(properties->GetValue()));
		
		else if (properties->GetName() == wxT("isContentAggergate") )
			slotDInfo.ContentAggregate(ParseBoolean(properties->GetValue()));
		
		else if (properties->GetName() == wxT("contentString") )
			slotDInfo.SetContentString(str2std(properties->GetValue()));
		
		else if (properties->GetName() == wxT("contentTargetVertex") )
			slotDInfo.SetContentTargetVertex(GetVertex(properties->GetValue()));
		
		else 
			assert(0);

		properties = properties->GetNext();
	}

	debugInfo.AddSlot(slotDInfo);
}


//-----------------------------------------------------------------------

id_t XmlToGraph::GetId(wxXmlNode * vertex) {
	assert(vertex);
	assert(vertex->HasProp(wxT("id")));
	return ParseLong(vertex->GetPropVal(wxT("id"), wxT("-1")));
}

//-----------------------------------------------------------------------

graphs::Vertex * XmlToGraph::GetVertex(const wxString & id) {
	assert(graph);

	if (id == wxT("null"))
		return static_cast<Vertex *>(0);

	const Vertex * vertex = &graph->GetVertex(ParseLong(id));
	assert(*vertex);
	
	return const_cast<Vertex*>(vertex);
}


//-----------------------------------------------------------------------

const graphs::Vertex * XmlToGraph::GetVertex(const id_t & id) {
	assert(graph);

	const Vertex * vertex = &graph->GetVertex(id);
	assert(*vertex);
	
	return vertex;
}

//-----------------------------------------------------------------------

long XmlToGraph::ParseLong(const wxString & value) {
	long tmp = -1;
	value.ToLong(&tmp);
	return tmp;
}

//-----------------------------------------------------------------------

double XmlToGraph::ParseDouble(const wxString & value) {
	double tmp = -1;
	value.ToDouble(&tmp);
	return tmp;
}

//-----------------------------------------------------------------------

bool XmlToGraph::ParseBoolean(const wxString & value) {
	return (value == wxT("true") ? true : false);
}

}	//namespace iviews

