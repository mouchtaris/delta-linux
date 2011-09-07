/**
 *	LayerRenderingInfo.h
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	June 2010
 */

#ifndef LAYOUT_RENDERING_INFO_H
#define LAYOUT_RENDERING_INFO_H

#include <map>
#include <list>
#include <vector>
#include "Point.h"
#include "Vertex.h"
#include "Rectangle.h"
#include "Dimension.h"
#include "Functional.h"
#include "GraphLayers.h"
#include "CommonTypes.h"
#include "LayoutParams.h"
#include "RenderingTypes.h"
#include "EdgesParameters.h"
#include "VertexRenderingInfo.h"

namespace iviews {

class LayerRenderingInfo {

public:

	///////////////////////////////////
	//	public typedef(s)
	typedef std::map<	
				graphs::Vertex *, 
				VertexRenderingInfo *, 
				utility::ComparatorFunctor<graphs::Vertex>
			> VerticesMapping;

	typedef graphs::GraphLayers::Layer			Layer;
	typedef geo::Point<coord_t>					CoordTPoint;
	typedef std::vector<LayerRenderingInfo *>	LayerRenderingInfoPtrVec;
	typedef VertexRenderingInfo::VertexRenderingInfoPtrList 
												VertexRenderingInfoPtrList;
	
	~LayerRenderingInfo ();
	LayerRenderingInfo (	
		const id_t &		 id, 
		const Layer &		 l, 
		Rectangle *			 _rect, 
		const LayoutParams & params
	);


	///////////////////////////////////
	//	public methods
	id_t				Id (void) const;
	Layer				GetLayer (void) const;
	void				SetPosition (const coord_t & x, const coord_t & y);
	void				SetPosition (const CoordTPoint & newPos);
	void				RecalculateHeightAndUpdateVerticesPosition (void);
	void				AddVertex (graphs::Vertex * v, VertexRenderingInfo * vri);		
	void				SetX (const coord_t & x);
	coord_t				GetX (void) const;
	void				SetY (const coord_t & y);
	coord_t				GetY (void) const;
	length_t			GetWidth (void) const;
	length_t			GetHeight (void) const;
	Rectangle * 		GetRectangle (void) const;
	bool				operator == (const LayerRenderingInfo * l);
	bool				operator != (const LayerRenderingInfo * l);



	EdgesParameters &	GetEdgesParameters (void);
	const EdgesParameters &
						GetEdgesParameters (void) const;



	const CoordTPoint &	GetPosition (void) const;
	VertexRenderingInfo *
						GetVertexRenderingInfo (graphs::Vertex * v);
	VertexRenderingInfoPtrList &			
						GetVerticesRenderingInfo (void);
	const VertexRenderingInfoPtrList &	
						GetVerticesRenderingInfo (void) const;


private:
	void				 CalcPositionForVertex (
							CoordTPoint & pos, 
							VertexRenderingInfo * prevVertex
						);

	///////////////////////////////////
	//	
	void				SetVertexPos(VertexRenderingInfo * vri);
	
	bool				isAllEdgesHidden;
	bool				isInnerEdgesHidden;
	


	id_t				id;
	Layer				layer;
	LayoutParams		layoutParams;
	Rectangle *			rect;
	VerticesMapping		vertices;	//we hold the verticies and the corresponding rendering info for each vertex
	EdgesParameters		edgesParams;


	geo::Dimension<length_t>		initSize;	//Is the init size of rec without to add vertices in layer
	//In this list, we hold the order of each vertex that we add in the layer
	VertexRenderingInfoPtrList	verticesOrder;	
	
	

};

}	//namespace iviews

#endif	//LAYOUT_RENDERING_INFO_H