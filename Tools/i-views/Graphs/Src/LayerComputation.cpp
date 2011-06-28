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

//-----------------------------------------------------------------------
//
//#define ADD_CONDITIONALLY_VERTICES_IN_NEW_LAYER(LIST)											\
//	for(Vertex::VerticesPtrList::const_iterator j = LIST.begin(); j != LIST.end(); ++j)			\
//		if (!gl->VertexExistsInLayer(**j, currLayer))											\
//			if (currLayer > 0){	/*Maybe we are in lavel 0 and we can not go to prev layer*/		\
//				if(!gl->VertexExistsInLayer(**j, currLayer-1))									\
//					gl->AddVertexInLayer(**j, newLayer);										\
//			}																					\
//			else																				\
//				gl->AddVertexInLayer(**j, newLayer)

namespace iviews {

namespace graphs {



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


//
//void AddConditionallyVerticesInNewLayer(GraphLayers * gl,
//										const Vertex::VerticesPtrList & list,
//										const unsigned currLayer,
//										const unsigned newLayer
//										)
//{
//	for(Vertex::VerticesPtrList::const_iterator j = list.begin(); j != list.end(); ++j)			
//		if (!gl->VertexExistsInLayer(**j, currLayer))											
//			if (currLayer > 0){	/*Maybe we are in lavel 0 and we can not go to prev layer*/		
//				if(!gl->VertexExistsInLayer(**j, currLayer-1))									
//					gl->AddVertexInLayer(**j, newLayer);										
//			}																					
//			else																				
//				gl->AddVertexInLayer(**j, newLayer);
//
//}
//
//
////-----------------------------------------------------------------------
//
//void AddVerticesInLayersGreaterThanZero(const Graph & g, 
//										GraphLayers * gl, 
//										const unsigned currLayer, 
//										const unsigned lastLayer)
//{
//	assert (currLayer < lastLayer);
//	CREATE_LAYER(currLayer+1);
//
//	const Vertex::VerticesPtrList & l = gl->GetAllVerticesInLayer(currLayer);
//	for (Vertex::VerticesPtrList::const_iterator i = l.begin(); i != l.end(); ++i) {
//		const Vertex::VerticesPtrList & outList = g.GetOutgoingEdges(**i);
//		AddConditionallyVerticesInNewLayer(gl, outList, currLayer, newLayer);
//		
//		const Vertex::VerticesPtrList & inList = g.GetIncomingEdges(**i);
//		AddConditionallyVerticesInNewLayer(gl, inList, currLayer, newLayer);
//	}
//
//	if (newLayer < lastLayer-1) {
//		if (gl->TotalNumberOfVerticesInLayer(newLayer) == 0)
//			gl->DeleteLastLayer();
//		else
//			AddVerticesInLayersGreaterThanZero(g, gl, newLayer, lastLayer);
//	}
//	else if (gl->TotalNumberOfVerticesInLayer(newLayer) == 0)
//		gl->DeleteLastLayer();
//}
//
//
////-----------------------------------------------------------------------
//
////caller is responsible to delete the instance
//GraphLayers * LayerComputation::operator()(const Graph & g, 
//										   const Vertex & startVertex, 
//										   const unsigned maxNumOfLayers)
//{
//	assert(g.VertexExists(startVertex));
//	GraphLayers * gl = new GraphLayers(maxNumOfLayers, g);
//
//	if (maxNumOfLayers >= 1) {
//		CREATE_LAYER(0);
//		gl->AddVertexInLayer(startVertex, newLayer);
//
//		if (maxNumOfLayers >= 2)
//			AddVerticesInLayersGreaterThanZero(g, gl, newLayer, maxNumOfLayers);
//	}
//
//	return gl;
//}


}	//namespace graphs

}	//namespace iviews