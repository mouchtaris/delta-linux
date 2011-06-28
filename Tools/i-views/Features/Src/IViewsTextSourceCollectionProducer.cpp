#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif

#include <assert.h>
#include <algorithm>

#include "Functional.h"
#include "IViewsTextSource.h"
#include "LayerRenderingInfo.h"
#include "VertexRenderingInfo.h"
#include "IViewsTextSourceCollectionProducer.h"


namespace iviews {

//-----------------------------------------------------------------------

IViewsTextSourceCollectionProducer::IViewsTextSourceCollectionProducer(
	wxScrolledWindow	* owner_,
	FindAddOn			* addOn,
	ColourAddOnManager	* manager
) :	owner				(owner_),
	scaleFactor			(1.0),
	lastVisitedVertex	(static_cast<Vertex *>(0)),
	findAddOn			(addOn),
	graphLayers			(static_cast<GraphLayers *>(0)),
	addOnManager		(manager),
	layersRenderingInfo	(static_cast<LayersRenderingInfo *>(0)),
	vertexToDInfo		(static_cast<VertexToDebugInfoMapping *>(0)),
	tsList				(),
	selectionManager	()
{}

//-----------------------------------------------------------------------

IViewsTextSourceCollectionProducer::~IViewsTextSourceCollectionProducer() 
	{ Clear(); } 

//-----------------------------------------------------------------------

void IViewsTextSourceCollectionProducer::SetGraphLayers(GraphLayers * glayers) {
	assert(glayers);
	graphLayers = glayers;
}

//-----------------------------------------------------------------------

void IViewsTextSourceCollectionProducer::SetSelectionColour (const geo::Colour & c) { 
	assert(findAddOn);
	findAddOn->SetSelectionColour(c); 
}

//-----------------------------------------------------------------------

void IViewsTextSourceCollectionProducer::SetLastVisitedVertex(Vertex * v) {
	assert(v);
	lastVisitedVertex = v;
}

//-----------------------------------------------------------------------

void IViewsTextSourceCollectionProducer::SetLayersRenderingInfo(LayersRenderingInfo * renInfo) {
	assert(renInfo);
	layersRenderingInfo = renInfo;

	typedef std::list<frep::TextSource *> TextSourceList;

	for(TextSourceList::iterator i = tsList.begin(); i != tsList.end(); ++i)
		dynamic_cast<IViewsTextSource *>(*i)->SetLayersRenderingInfo(layersRenderingInfo);
}

//-----------------------------------------------------------------------

void IViewsTextSourceCollectionProducer::SetVertexToDebugInfoMapping(
		VertexToDebugInfoMapping * vertexToDInfo_
	){
	assert(vertexToDInfo_);
	vertexToDInfo = vertexToDInfo_;
}

//-----------------------------------------------------------------------

std::list<frep::TextSource *> & IViewsTextSourceCollectionProducer::GetTextSourceList(
		frep::LookIn lookIn, 
		const bool findInScripts
	) {
	Clear();
	if (lookIn == frep::ENTIRE_GRAPH)
		CreateTextSourceForEntireGraph();
	else if (lookIn == frep::LAST_VISITED_VERTEX && lastVisitedVertex)
		CreateTextSourceForVertex(lastVisitedVertex);

	return tsList;
}

//-----------------------------------------------------------------------

void IViewsTextSourceCollectionProducer::Clear(void) {
	std::for_each(
		tsList.begin(), 
		tsList.end(), 
		utility::DeletePointersFromList<frep::TextSource>()
	);
	tsList.clear();
	selectionManager.Clear();
}

//-----------------------------------------------------------------------

void IViewsTextSourceCollectionProducer::SetScaleFactor(const double scaleFactor_) {
	typedef std::list<frep::TextSource *> TextSourceList;

	scaleFactor = scaleFactor_;
	for(TextSourceList::iterator i = tsList.begin(); i != tsList.end(); ++i)
		dynamic_cast<IViewsTextSource *>(*i)->SetScaleFactor(scaleFactor);
}



//**********************************************************
//	private method implementation


//-----------------------------------------------------------------------

void IViewsTextSourceCollectionProducer::CreateTextSourceForEntireGraph(void) {
	if (!graphLayers)
		return;
	
	if (!lastVisitedVertex) 
		CreateTextSourceForLayers(
			graphLayers->GetFirstLayer(),
			graphLayers->GetLastLayer()+1	//+1 we wont a layr  after the last
		);		

	//we wont to add in tsList firt all the vertex from the last viseted vertex until the end
	//and after that from the first vertex of the first layer in until last visited vertex
	else if (graphLayers->VertexExist(*lastVisitedVertex)) {
		CreateTextSourceForLayers(
			graphLayers->GetLayerThatContainVertex(*lastVisitedVertex),
			graphLayers->GetLastLayer()+1	//+1 we wont a layr  after the last
		);		

		CreateTextSourceForLayers(
			graphLayers->GetFirstLayer(),
			graphLayers->GetLayerThatContainVertex(*lastVisitedVertex)
		);		
	}
}

//-----------------------------------------------------------------------

//create text source for layers: [begin end)
void IViewsTextSourceCollectionProducer::CreateTextSourceForLayers(
		const Layer & begin, 
		const Layer & end
	) {
	for (Layer l = begin; l < end; l = graphLayers->GetNextLayer(l)) {
		const VerticesPtrList & vertices = graphLayers->GetAllVerticesInLayer(l);
		VerticesPtrList::const_iterator vertex;
		
		for (vertex = vertices.begin(); vertex != vertices.end(); ++vertex)
			CreateTextSourceForVertex(*vertex);
	}
}

//-----------------------------------------------------------------------

void IViewsTextSourceCollectionProducer::CreateTextSourceForVertex(Vertex * vertex) {
	assert(vertex && vertexToDInfo && graphLayers);
	assert(graphLayers->VertexExist(*vertex));

	VertexToDebugInfoMapping::iterator debugInfo = vertexToDInfo->find(vertex);
	assert(debugInfo != vertexToDInfo->end());

	IViewsTextSource * textSource = new IViewsTextSource(
											owner,
											vertex, 
											layersRenderingInfo,
											findAddOn, 
											addOnManager,
											debugInfo->second,
											&selectionManager,
											scaleFactor
									);

	selectionManager.RegisterTextSource(textSource->GetPath());
	tsList.push_back(textSource);
}


}	//namespace iviews