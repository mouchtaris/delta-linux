//June 2010
#ifndef COORDINATE_COONVERTER_H
#define COORDINATE_COONVERTER_H

//convert from  cartesian
//to screen 

#include "EdgeRenderingInfo.h"
#include "SlotRenderingInfo.h"
#include "LayerRenderingInfo.h"
#include "VertexRenderingInfo.h"
#include "LayersRenderingInfo.h"

namespace iviews {
	class CoordinateConverter {
	public:
		void operator() (LayersRenderingInfo &);
		void operator() (VertexRenderingInfo * v);
	private:
		typedef EdgeRenderingInfo::EdgeRenderingInfoPtrList		EdgeRenderingInfoPtrList;
		typedef SlotRenderingInfo::SlotRenderingInfoPtrList		SlotRenderingInfoPtrList;
		typedef LayerRenderingInfo::LayerRenderingInfoPtrVec	LayerRenderingInfoPtrVec;
		typedef VertexRenderingInfo::VertexRenderingInfoPtrList	VertexRenderingInfoPtrList;
		
		void ConvertVerticesCoordinates(LayerRenderingInfo * layer);
		void ConvertSlotsCoordinates(VertexRenderingInfo * vertex);
		void ConvertEdgesCoordinates(EdgeRenderingInfoPtrList & edges);


	};

}	//namespace iviews

#endif