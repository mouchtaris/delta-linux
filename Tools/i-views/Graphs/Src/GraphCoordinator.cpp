/**
 *	GraphCoordinator.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	March 2010
 */


#include "GraphCoordinator.h"


namespace iviews {
							
namespace graphs {


GraphCoordinator::GraphCoordinator () : 
	hasError				(false),
	isSimpleObject			(false),
	errorMsg				(""),
	simpleObjectDescription	(""),
	graph					(static_cast<Graph *>(0)), 
	graphLayers				(static_cast<GraphLayers *>(0)),
	mapping					(static_cast<VertexToDebugInfoMapping *>(0))
{}

//-----------------------------------------------------------------------

GraphCoordinator::~GraphCoordinator()
	{	Clean();	}

//-----------------------------------------------------------------------

GraphLayers * GraphCoordinator::Undo(void) {
	if (!undoRedo.CanUndo() && graph && mapping)
		return (GraphLayers *)0;

	VerticesPtrList undoList = undoRedo.Undo();

	//eixame arxikh vertex sta pros diagrafh ara den pira3ame tipota
	if (std::find(undoList.begin(), undoList.end(), startVertex) != undoList.end()) {
		assert(graphLayers);
		return graphLayers;
	}

	graph->UnmarkVertexAsDeleted(*undoList.back());
	graph->Undo();
	mapping->Undo();
	return layerComputation(*graph, *startVertex);
}

//-----------------------------------------------------------------------

bool GraphCoordinator::CanUndo(void) 
	{	return !undoRedo.CanUndo() && graph && mapping; }

//-----------------------------------------------------------------------

bool GraphCoordinator::CanRedo(void) 
	{	return undoRedo.CanRedo() && graph && mapping; }

//-----------------------------------------------------------------------

GraphLayers * GraphCoordinator::Redo(void) {
	if (!undoRedo.CanRedo() && graph && mapping)
		return (GraphLayers *)0;

	VerticesPtrList redoList = undoRedo.Redo();

	////exoume arxikh vertex sta pros diagrafh ara den pira3ame tipota
	if (std::find(redoList.begin(), redoList.end(), startVertex) != redoList.end()) {
		assert(graphLayers);
		return (GraphLayers *)0;
	}

	graph->MarkVertexAsDeleted(*redoList.back());
	graph->Redo();
	mapping->Redo();
	
	return layerComputation(*graph, *startVertex);
}

//-----------------------------------------------------------------------

GraphLayers * GraphCoordinator::operator()(const std::string & expression, const int _depth){ 
	Clean();
	depth = _depth;
	graph = converter(objGraphCreator(expression, depth));
	if (graph == static_cast<Graph *>(0)) {
		if (converter.HasError())
			SetError(converter.GetErrorMessage());
		else if(converter.IsSimpleObject())
			SetSimpleObject(converter.GetDescriptionOfSimpleObject());
		else
			assert(0);
	}
	else {
		mapping		= converter.GetDebugInfo();
		startVertex = converter.GetStartVertex();
		graphLayers = layerComputation(*graph, *startVertex);
	}

	return graphLayers;
}

//-----------------------------------------------------------------------

GraphLayers * GraphCoordinator::ExcludeVertex(Vertex & v) {
	undoRedo.NewUndoAction();
	undoRedo.AddInUndoStack(&v);

	//If v is the same with the startVertex we do not have graph layer
	if (v == *startVertex)
		return (GraphLayers *)0;
	
	if (graph) {			
		DeleteGraphLayers();
		graph->DeleteAllEdgesThatContainsVertex(v);
		graph->MarkVertexAsDeleted(v);
		//From the prev. graph (That we have excluted the vertex v) we create a new graph layer.
		
		graphLayers = layerComputation(*graph, *startVertex);
		ClearVertexToDebugInfoMappingFromUnnessecaryVertices();
		mapping->RemoveVertexFromSlots(v);
	}
	return graphLayers;
}

//-----------------------------------------------------------------------

GraphLayers * GraphCoordinator::operator()(	
		long		depth_,
		Vertex *	startVertex_, 
		Graph  *	graph_, 
		VertexToDebugInfoMapping * mapping_
	) {
	Clean();
	depth		= depth_;
	graph		= graph_;
	mapping		= mapping_;
	startVertex	= startVertex_;
	graphLayers = layerComputation(*graph, *startVertex);
	return graphLayers;
}

//-----------------------------------------------------------------------

void GraphCoordinator::DeleteGraphLayers() {
	if (graphLayers) {					
		delete graphLayers;												
		graphLayers = static_cast<GraphLayers *>(0);					
	}																	
}
//-----------------------------------------------------------------------

void GraphCoordinator::Clean (void){
	depth					= 0;
	startVertex				= static_cast<Vertex *>(0);
	hasError				= false;
	isSimpleObject			= false;
	errorMsg				= "";
	simpleObjectDescription = "";

	if (graph) {								
		delete graph;													
		graph = static_cast<Graph *>(0);								
	}																	
	if (mapping != static_cast<VertexToDebugInfoMapping *>(0)) {	
		delete mapping;													
		mapping = static_cast<VertexToDebugInfoMapping *>(0);	
	}
	undoRedo.Clean();
	DeleteGraphLayers();
}
//-----------------------------------------------------------------------

bool GraphCoordinator::HasError(void) const 
	{ return hasError; }

//-----------------------------------------------------------------------

const std::string & GraphCoordinator::GetErrorMessage(void) const 
	{ return errorMsg; }

//-----------------------------------------------------------------------

bool GraphCoordinator::GraphIsSimpleObje(void) const 
	{ return isSimpleObject; }

//-----------------------------------------------------------------------

const std::string & GraphCoordinator::GetDescriptionOfSimpleObject(void) const 
	{ return simpleObjectDescription; }

//-----------------------------------------------------------------------

long GraphCoordinator::GetDepth(void) const
	{ return depth; }

//-----------------------------------------------------------------------

Graph *	GraphCoordinator::GetGraph(void) const
	{ return graph; }

//-----------------------------------------------------------------------

Vertex * GraphCoordinator::GetStartVertex(void) const
	{ return startVertex; }

//-----------------------------------------------------------------------

GraphLayers * GraphCoordinator::GetGraphLayers(void) const
	{ return graphLayers; }

//-----------------------------------------------------------------------

VertexToDebugInfoMapping * GraphCoordinator::GetVertexToDebugInfoMapping(void) const
	{ return mapping; }

//-----------------------------------------------------------------------

//Remove all the vertices tha exist in DebugInfoMapping 
//but do not exist in graph layer
void GraphCoordinator::ClearVertexToDebugInfoMappingFromUnnessecaryVertices(void){
	assert(graphLayers && mapping);
	
	mapping->NewUndoAction();
	Vertex::VerticesPtrList vertices		= graphLayers->GetAllVertices();
	VertexToDebugInfoMapping::iterator i	= mapping->begin();
	
	while (i != mapping->end()){
		VertexToDebugInfoMapping::iterator toDelete = i++;
		if (std::find(vertices.begin(), vertices.end(), toDelete->first) == vertices.end())
			mapping->erase(toDelete);
	}
}

//-----------------------------------------------------------------------

void GraphCoordinator::SetError(const std::string & message) {
	hasError = true;
	errorMsg = message;
}

//-----------------------------------------------------------------------

void GraphCoordinator::SetSimpleObject(const std::string & description) {
	isSimpleObject = true;
	simpleObjectDescription = description;
}

//-----------------------------------------------------------------------

GraphCoordinator::UndoRedoHelperForGraphCoordinator::UndoRedoHelperForGraphCoordinator(void) 
: UndoRedoHelper(){}

//-----------------------------------------------------------------------

void GraphCoordinator::UndoRedoHelperForGraphCoordinator::AddInUndoStack(Vertex * v) 
	{ undoStack.back().push_back(v); }





}	//namespace graphs
}	//namespace iviews