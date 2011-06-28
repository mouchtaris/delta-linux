//June 2010
#ifndef LAYOUT_CALCULATOR_2_H 
#define LAYOUT_CALCULATOR_2_H

#include <map>
#include <set>
#include "Colour.h"
#include "Vertex.h"
#include "DebugInfo.h"
#include "Rectangle.h"
#include "Primitive.h"
#include "Functional.h"
#include "CommonTypes.h"
#include "GraphLayers.h"
#include "LayoutParams.h"
#include "SlotDebugInfo.h"
#include "ConvertToGraph.h"
#include "RenderingParams.h"
#include "LayerRenderingInfo.h"
#include "LayersRenderingInfo.h"

namespace iviews {
	class LayoutCalculator2 {
	public:
		///////////////////////////////////////////////////
		//	public typedef(s)
		typedef Primitive::PrimitivePtrList							PrimitivePtrList;
		typedef graphs::Vertex::VerticesSet							VerticesSet;
		typedef graphs::ConvertToGraph::VertexToDebugInfoMapping	VertexToDebugInfoMapping;
		

		LayoutCalculator2();

		//caller is responsible to delete the instance
		LayersRenderingInfo * Calculate(graphs::GraphLayers		 * graphL,
										LayoutParams			 * lParams,
										RenderingParams			 * rParams,
										VertexToDebugInfoMapping * debugImfo
									);
	
	private:


		

		
	};
}	//namespace iviews

#endif //LAYOUT_CALCULATOR_H


