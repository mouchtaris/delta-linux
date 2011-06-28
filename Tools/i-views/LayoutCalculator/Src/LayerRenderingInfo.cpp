#include "LayerRenderingInfo.h"

#include <cmath>
#include <assert.h>
#include <algorithm>

#include "Functional.h"

namespace iviews {

//-----------------------------------------------------------------------

LayerRenderingInfo::LayerRenderingInfo (
	const id_t	&			_id, 
	const Layer	&			l, 
	Rectangle *				_rect,
	const LayoutParams &	params
) :	id				(_id),
	layer			(l), 
	rect			(_rect),
	initSize		(rect->GetDimension()),
	layoutParams	(params)
{ assert(rect && rect->GetWidth() && rect->GetHeight()); }

//-----------------------------------------------------------------------

LayerRenderingInfo::~LayerRenderingInfo () {

	if (rect != static_cast<Rectangle *>(0)) {
		delete rect;
		rect = static_cast<Rectangle *>(0);
	}

	std::for_each(	
			verticesOrder.begin(), 
			verticesOrder.end(), 
			utility::DeletePointersFromList<VertexRenderingInfo>()
	);
	verticesOrder.clear();
}

//-----------------------------------------------------------------------

/*
We use the screen coordinate system.
The axis’ begin is the top left corner of the screen.
X axis increases from left to right 
Y axis increases from top to bottom
*/
void LayerRenderingInfo::SetPosition(const CoordTPoint & newPos) {
	length_t width	= 0;
	length_t height	= 0;
	CoordTPoint oldPos = rect->GetPosition();

	//We calulate new positions for all the containing verticies
	for(VerticesMapping::iterator i = vertices.begin(); i != vertices.end(); ++i) {
		VertexRenderingInfo * vertex	= i->second;
		CoordTPoint vertexPos			= vertex->GetPosition();

		//we calulate the distance between old layer's position and old vertex's position
		width	= (length_t)std::fabs((vertexPos.GetX() - oldPos.GetX()));
		height	= (length_t)std::fabs((vertexPos.GetY() - oldPos.GetY()));

		vertex->SetPosition(newPos.GetX() + width, newPos.GetY() + height);
	}

	rect->SetPosition(newPos);
}

//-----------------------------------------------------------------------
//When we add a new vertex, 
//1) We increase the layer's height
//2) We calculate vertex position, inside the layer, and update all vertex contents
void LayerRenderingInfo::AddVertex(graphs::Vertex * v, VertexRenderingInfo * vri) {
	assert(v);
	assert(vri);

	if (vertices.find(v) == vertices.end()) {
		SetVertexPos(vri);
		verticesOrder.push_back(vri);

		vertices[v] = vri;
		if (vertices.size() == 1)
			rect->SetHeight(rect->GetHeight() + vri->GetHeight());
		else
			rect->SetHeight(rect->GetHeight() + vri->GetHeight() + layoutParams.layerParams.verticesDistance);
	}
	else
		assert(0);
}

//-----------------------------------------------------------------------

void LayerRenderingInfo::SetVertexPos(VertexRenderingInfo * vri) {
	assert(vri);
	CoordTPoint newPos;

	CalcPositionForVertex(
		newPos, 
		verticesOrder.empty() ? (VertexRenderingInfo *)0: verticesOrder.back()
	);
	vri->SetPosition(newPos);
}

//-----------------------------------------------------------------------

//When recalculate width we set the new vertex pos
void LayerRenderingInfo::RecalculateHeightAndUpdateVerticesPosition(void) {

	rect->SetDimension(initSize);
	
	//Calculate new positions for the vertices
	VertexRenderingInfoPtrList::iterator curr, prev;
	for(curr = verticesOrder.begin(); curr != verticesOrder.end(); prev = curr++){
		CoordTPoint newPos;
		CalcPositionForVertex(
			newPos, 
			(curr ==  verticesOrder.begin()) ? (VertexRenderingInfo *)0 : (*prev)
		);
		(*curr)->SetPosition(newPos);
	}

	//Calc and set layer's height
	length_t verticesDY	= (length_t)std::fabs(
									verticesOrder.front()->GetY() - 
									verticesOrder.back()->GetY()
						);

	rect->SetHeight(verticesDY + verticesOrder.back()->GetHeight() + rect->GetHeight());
}

//-----------------------------------------------------------------------

VertexRenderingInfo * LayerRenderingInfo::GetVertexRenderingInfo(graphs::Vertex * v) {
	assert(v);
	
	VerticesMapping::iterator result;
	if ((result = vertices.find(v)) != vertices.end() )
		return result->second;
	return static_cast<VertexRenderingInfo *>(0);
}

//-----------------------------------------------------------------------

id_t LayerRenderingInfo::Id(void) const 
	{ return id; }

//-----------------------------------------------------------------------

LayerRenderingInfo::Layer LayerRenderingInfo::GetLayer(void) const 
	{ return layer; }

//-----------------------------------------------------------------------

void LayerRenderingInfo::SetPosition(const coord_t & x, const coord_t & y) 
	{ SetPosition(CoordTPoint(x,y)); }

//-----------------------------------------------------------------------

Rectangle * LayerRenderingInfo::GetRectangle(void) const 
	{ return rect; }

//-----------------------------------------------------------------------

void LayerRenderingInfo::SetX(const coord_t & x) 
	{ rect->SetX(x); }

//-----------------------------------------------------------------------

coord_t LayerRenderingInfo::GetX(void) const 
	{ return rect->GetX(); }

//-----------------------------------------------------------------------

void LayerRenderingInfo::SetY(const coord_t & y) 
	{ rect->SetY(y); }

//-----------------------------------------------------------------------

coord_t	LayerRenderingInfo::GetY(void) const 
	{ return rect->GetY(); }

//-----------------------------------------------------------------------

length_t LayerRenderingInfo::GetWidth(void) 
	const { return rect->GetWidth(); }

//-----------------------------------------------------------------------

length_t LayerRenderingInfo::GetHeight(void) 
	const { return rect->GetHeight(); }

//-----------------------------------------------------------------------

LayerRenderingInfo::VertexRenderingInfoPtrList & LayerRenderingInfo::GetVerticesRenderingInfo(void) 
	{ return verticesOrder; }

//-----------------------------------------------------------------------

const LayerRenderingInfo::VertexRenderingInfoPtrList & LayerRenderingInfo::GetVerticesRenderingInfo(void) const 
	{ return verticesOrder; }

//-----------------------------------------------------------------------

const LayerRenderingInfo::CoordTPoint & LayerRenderingInfo::GetPosition(void) const 
	{ return rect->GetPosition(); }

//-----------------------------------------------------------------------

bool LayerRenderingInfo::operator == (const LayerRenderingInfo * l) 
	{ return layer == l->layer; }

//-----------------------------------------------------------------------

bool LayerRenderingInfo::operator != (const LayerRenderingInfo * l)
	{ return !((*this) == l); }

//-----------------------------------------------------------------------

EdgesParameters & LayerRenderingInfo::GetEdgesParameters (void)
	{ return edgesParams; }	

//-----------------------------------------------------------------------

const EdgesParameters & LayerRenderingInfo::GetEdgesParameters (void) const
	{ return edgesParams; }	

//-----------------------------------------------------------------------

void LayerRenderingInfo::CalcPositionForVertex (
		CoordTPoint & pos, 
		VertexRenderingInfo * prevVertex
	) {
	if (!prevVertex) {		//Vertex is first																					
		pos.SetX(rect->GetPosition().GetX() + layoutParams.layerParams.distanceFromLayer.GetWidth());		
		pos.SetY(rect->GetPosition().GetY() + layoutParams.layerParams.distanceFromLayer.GetHeight());	
	}																										
	else {																									
		pos.SetX(prevVertex->GetPosition().GetX());														
		pos.SetY(
			prevVertex->GetPosition().GetY()		+												
			prevVertex->GetHeight()				+												
			layoutParams.layerParams.verticesDistance
		);												
	}
}

}	//namespace iviews