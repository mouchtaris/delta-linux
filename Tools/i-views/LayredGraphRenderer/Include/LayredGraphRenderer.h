//july 2010
#ifndef	LAYRED_GRAPH_RENDERER_H
#define	LAYRED_GRAPH_RENDERER_H

#include "Primitive.h"
#include "RenderingAPI.h"
#include "LayoutCalculator.h"	//primitives list

namespace iviews {
	
class LayredGraphRenderer {
public:
	typedef Primitive::PrimitivePtrList PrimitivePtrList;

	LayredGraphRenderer (RenderingAPI * _api) : api(_api){}
	~LayredGraphRenderer(){}

	void SetRenderingAPI (RenderingAPI * api);
	void operator()( const PrimitivePtrList & primitiveList);

private: 
	RenderingAPI * api;
};

}	//namespace iviews



#endif	//LAYRED_GRAPH_RENDERER_H