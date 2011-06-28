#include "CoordinateConverter.h"

#include <assert.h>
#include "Text.h"
#include "Arrow.h"
#include "Point.h"
#include "Rectangle.h"
#include "Primitive.h"
#include "RenderingTypes.h"









namespace iviews {

//-----------------------------------------------------------------------

static void ConvertToPosetiveRect(Rectangle * rect) {
	assert(rect);

	geo::Point<coord_t> pos = rect->GetPosition();
	if (pos.GetX() < 0)
		rect->SetX(-pos.GetX());
	if (pos.GetY() < 0)
		rect->SetY(-pos.GetY());
}

//-----------------------------------------------------------------------

static void ConvertToPosetiveText(Text * text) {
	assert(text);

	geo::Point<coord_t> pos = text->GetTextPosition();
	if (pos.GetX() < 0)
		text->SetTextXCoordinate(-pos.GetX());
	if (pos.GetY() < 0)
		text->SetTextYCoordinate(-pos.GetY());
}


#define CONVERT_ARROW_POINT(POINT)	\
	if (POINT.GetX() < 0)			\
		POINT.SetX(-POINT.GetX());	\
	if (POINT.GetY() < 0)			\
		POINT.SetY(-POINT.GetY())

static void ConvertToPosetiveArrow(Arrow * arrow) {
	assert(arrow);	

	geo::Point<coord_t> & head = arrow->GetHead();
	CONVERT_ARROW_POINT(head);
	
	geo::Point<coord_t> & tail =  arrow->GetTail();
	CONVERT_ARROW_POINT(tail);
	
	geo::Point<coord_t> & leftSide = arrow->GetLeftSide();
	CONVERT_ARROW_POINT(leftSide);

	geo::Point<coord_t> & rightSide = arrow->GetRightSide();
	CONVERT_ARROW_POINT(rightSide);

	if (arrow->Type() == Primitive::CURVE_ARROW) {
		geo::Point<coord_t> & peak		= ((CurveArrow *)arrow)->GetCurvePeak();
		CONVERT_ARROW_POINT(peak);
		
		geo::Point<coord_t> eclpise	= ((CurveArrow *)arrow)->GetEclipseRect().GetPosition();
		CONVERT_ARROW_POINT(eclpise);
		((CurveArrow *)arrow)->GetEclipseRect().SetPosition(eclpise);
	}
	//pos = arrow->
}





//-----------------------------------------------------------------------
	
void CoordinateConverter::operator() (LayersRenderingInfo & lri) {
	LayerRenderingInfoPtrVec layers				= lri.GetLayersRenderingInfo();
	LayerRenderingInfoPtrVec::iterator layer	= layers.begin();

	for(; layer != layers.end(); ++layer) {
		ConvertToPosetiveRect((*layer)->GetRectangle());
		ConvertVerticesCoordinates((*layer));
	}
}

void CoordinateConverter::operator() (VertexRenderingInfo * v){
	assert(v);
	ConvertToPosetiveText(v->GetAbsRefText());
	ConvertToPosetiveRect(v->GetVertexMainRectangle());
	ConvertSlotsCoordinates(v);
	ConvertEdgesCoordinates(v->GetAllOutgoingEdgesRenderingInfo());
}


//-----------------------------------------------------------------------

void CoordinateConverter::ConvertVerticesCoordinates(LayerRenderingInfo * layer) {
	assert(layer);

	VerticesRenderingInfoPtrList & vertices			= layer->GetVerticesRenderingInfo();	
	VerticesRenderingInfoPtrList::iterator verIte	= vertices.begin();

	for(; verIte != vertices.end(); ++verIte)
		this->operator ()(*verIte);
}

//-----------------------------------------------------------------------

void CoordinateConverter::ConvertEdgesCoordinates(EdgeRenderingInfoPtrList & edges) {
	EdgeRenderingInfoPtrList::iterator edge = edges.begin();

	for(; edge != edges.end(); ++edge) {
		if ((*edge)->GetText() != static_cast<Text *>(0))
			ConvertToPosetiveText((*edge)->GetText());
		ConvertToPosetiveArrow((*edge)->GetArrow());
	}
}


//-----------------------------------------------------------------------

void CoordinateConverter::ConvertSlotsCoordinates(VertexRenderingInfo * vertex) {
	assert(vertex);

	SlotRenderingInfoPtrList & slotsList		= vertex->GetAllSlostsRenderingInfo();
	SlotRenderingInfoPtrList::iterator slotsIte	= slotsList.begin();

	for(; slotsIte != slotsList.end(); ++slotsIte) {
		ConvertToPosetiveText((*slotsIte)->GetKeyText());
		ConvertToPosetiveRect((*slotsIte)->GetKeyRectangle());
		ConvertToPosetiveRect((*slotsIte)->GetKeyExpandButtonRectangle());

		ConvertToPosetiveText((*slotsIte)->GetContentText());
		ConvertToPosetiveRect((*slotsIte)->GetContentRectangle());
		ConvertToPosetiveRect((*slotsIte)->GetContentExpandButtonRectangle());
	}

}

}	//namesapce iviews