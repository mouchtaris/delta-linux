#ifndef LAYER_COMPUTATION_H
#define LAYER_COMPUTATION_H


#include "Graph.h"
#include "GraphLayers.h"

namespace iviews {
	namespace graphs {
		struct LayerComputation {
			///////////////////////////////////////////////////
			//	overloaded operator(s)

			//caller is responsible to delete the instance
			GraphLayers * operator()(	
							const Graph & g, 
							const Vertex & startVertex
						);
		};
	}	//namespace graphs
}	//namespace iviews

#endif	//LAYER_COMPUTATION_H