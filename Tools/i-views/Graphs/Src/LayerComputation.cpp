/**
 *	LayerComputation.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	March 2010
 */

#include "LayerComputation.h"

#include <iostream>

//-----------------------------------------------------------------------

#define CREATE_LAYER(LAYER)				\
	unsigned newLayer = gl->NewLayer();	\
	assert(newLayer == LAYER)


namespace iviews {

namespace graphs {

//-----------------------------------------------------------------------

static void AddConditionallyVerticesInNewLayer(
				GraphLayers * gl,
				const Vertex::VerticesPtrList & list,
				const unsigned currLayer,
				const unsigned newLayer
			) {
	for(Vertex::VerticesPtrList::const_iterator j = list.begin(); j != list.end(); ++j)			
		if (!gl->VertexExistsInLayer(**j, currLayer))											
			if (currLayer > 0){	/*Maybe we are in lavel 0 and we can not go to prev layer*/		
				if(!gl->VertexExistsInLayer(**j, currLayer-1))									
					gl->AddVertexInLayer(**j, newLayer);										
			}																					
			else																				
				gl->AddVertexInLayer(**j, newLayer);

}

//-----------------------------------------------------------------------

static void AddVerticesInLayersGreaterThanZero(
				const Graph & g, 
				GraphLayers * gl, 
				const unsigned currLayer
			) {
	CREATE_LAYER(currLayer+1);

	const Vertex::VerticesPtrList & l = gl->GetAllVerticesInLayer(currLayer);
	for (Vertex::VerticesPtrList::const_iterator i = l.begin(); i != l.end(); ++i) {
		const Vertex::VerticesPtrList & outList = g.GetOutgoingEdges(**i);
		AddConditionallyVerticesInNewLayer(gl, outList, currLayer, newLayer);
		
		const Vertex::VerticesPtrList & inList = g.GetIncomingEdges(**i);
		AddConditionallyVerticesInNewLayer(gl, inList, currLayer, newLayer);
	}
	
	if (!gl->TotalNumberOfVerticesInLayer(newLayer))
		gl->DeleteLastLayer();
	else
		AddVerticesInLayersGreaterThanZero(g, gl, newLayer);
}

//-----------------------------------------------------------------------

//caller is responsible to delete the instance
GraphLayers * LayerComputation::operator()(
				const Graph & g, 
				const Vertex & startVertex
			) {
	assert(g.VertexExists(startVertex));
	GraphLayers * gl = new GraphLayers(g);

	CREATE_LAYER(0);
	gl->AddVertexInLayer(startVertex, newLayer);
	
	AddVerticesInLayersGreaterThanZero(g, gl, newLayer);
	return gl;
}

}	//namespace graphs

}	//namespace iviews