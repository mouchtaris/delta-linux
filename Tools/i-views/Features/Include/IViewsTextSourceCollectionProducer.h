/**
 *	IViewsTextSourceCollectionProducer.h
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	Octomber 2010
 */

#ifndef IVIEWS_TEXT_SOURCE_COLLECTION_PRODUCER_H
#define IVIEWS_TEXT_SOURCE_COLLECTION_PRODUCER_H

#include <wx/scrolwin.h>

#include "Vertex.h"
#include "Colour.h"
#include "FindAddOn.h"
#include "GraphLayers.h"
#include "ColourAddOnManager.h"
#include "LayersRenderingInfo.h"
#include "VertexToDebugInfoMapping.h"
#include "TextSourceCollectionProducer.h"
#include "IViewsTextSourceSelectionManager.h"


namespace iviews {

class IViewsTextSourceCollectionProducer : public frep::TextSourceCollectionProducer {
public:
	typedef graphs::Vertex						Vertex;
	typedef Vertex::VerticesPtrList				VerticesPtrList;
	typedef graphs::GraphLayers					GraphLayers;
	typedef graphs::VertexToDebugInfoMapping	VertexToDebugInfoMapping;
	
	
	virtual ~IViewsTextSourceCollectionProducer();
	IViewsTextSourceCollectionProducer(
		wxScrolledWindow	* owner,
		FindAddOn			* addOn,
		ColourAddOnManager	* colourAddOnManager
	);


	void		SetSelectionColour (const geo::Colour & c);
	void 		SetLastVisitedVertex(Vertex * v);
	void 		SetGraphLayers(GraphLayers * glayers);
	void 		SetLayersRenderingInfo(LayersRenderingInfo * renInfo);
	void 		SetVertexToDebugInfoMapping(VertexToDebugInfoMapping * vertexToDInfo);
	void		SetScaleFactor(const double scaleFactor);

	virtual std::list<frep::TextSource *> & 
				GetTextSourceList(
					const frep::LookIn lookIn, 
					const bool findInScripts
				);
	
private:
	typedef GraphLayers::Layer	Layer;

	void 		Clear(void);
	void 		CreateTextSourceForEntireGraph(void);
	void 		CreateTextSourceForVertex(Vertex * vertex);
	void 		CreateTextSourceForLayers(const Layer & begin, const Layer & end);
	

	double						  scaleFactor;
	Vertex						* lastVisitedVertex;
	FindAddOn					* findAddOn;
	GraphLayers					* graphLayers;
	wxScrolledWindow			* owner;
	ColourAddOnManager			* addOnManager;
	LayersRenderingInfo			* layersRenderingInfo;
	VertexToDebugInfoMapping	* vertexToDInfo;
	std::list<frep::TextSource *> tsList;

	IViewsTextSourceSelectionManager selectionManager;

};


}	//namespace iviews

#endif	//IVIEWS_TEXT_SOURCE_COLLECTION_PRODUCER_H