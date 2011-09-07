/**
 *	PopupWindow.cpp
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	August 2010
 */

#include "PopupWindow.h"
#include <assert.h>
#include <wx/dcclient.h>
#include "GotoSourcePoint.h"
#include "ViewSlotTextualContents.h"

namespace iviews {

BEGIN_EVENT_TABLE(PopupWindow, wxPopupWindow)
	EVT_PAINT		(PopupWindow::OnPaint)
	EVT_MOTION		(PopupWindow::OnMouseMove)
	EVT_LEFT_DOWN	(PopupWindow::OnLeftDown)
	EVT_RIGHT_DOWN	(PopupWindow::OnRightDown)
	EVT_MOUSEWHEEL	(PopupWindow::OnMouseWheel)
END_EVENT_TABLE()

//-----------------------------------------------------------------------

PopupWindow::PopupWindow (wxWindow *parent, int flags)
:	wxPopupWindow(parent, flags),
	wheelRotation(0),
	renderingAPI(),
	renderer(&renderingAPI),
	slotTooltip(1500),
	vertexToDraw(static_cast<VertexRenderingInfo *>(0))
{}

//-----------------------------------------------------------------------

void PopupWindow::OnPaint (wxPaintEvent & event) {

	wxPaintDC dc(this);
	PrepareDC(dc);
    dc.SetLogicalOrigin(0, 0);
    dc.SetAxisOrientation(true, false);
    dc.SetUserScale(1.0, 1.0);
    dc.SetMapMode(wxMM_TEXT);

	dc.Clear();
	dc.SetPen(*wxBLACK_PEN);
	dc.SetFont( wxFont( 12, wxSWISS, wxNORMAL, wxNORMAL ) );

	if (vertexToDraw) {
		renderingAPI.SetWxDC(&dc);
		renderer(vertexToDraw->GetVertexPrimitives());
	}
}

//-----------------------------------------------------------------------

void PopupWindow::OnLeftDown (wxMouseEvent & event) {
	assert(vertexToDraw);
	coord_t x, y;
	GetLogicalCoordinates(&x, &y, event.GetPosition());
	ViewSlotTextualContents()(this, vertexToDraw, x, y);
}

//-----------------------------------------------------------------------

void PopupWindow::OnRightDown (wxMouseEvent & event) {
	//if (!vertexToDraw)
	//	return;
	
	//if (vertexToDraw->HasFullContents()) {
		Show(false);
		GetParent()->ProcessEvent(event);
	//}
	//else
	//	GotoSourcePoint()(vertexToDraw);	
}

//-----------------------------------------------------------------------

void PopupWindow::OnMouseMove (wxMouseEvent & event) {
	//TODO an kouni8ei to muse ektos tou window fige??
	assert(vertexToDraw);
	coord_t x, y;
	GetLogicalCoordinates(&x, &y, event.GetPosition());
	slotTooltip(this, vertexToDraw, geo::Point<coord_t>(x, y));
}

//-----------------------------------------------------------------------

void PopupWindow::OnMouseWheel (wxMouseEvent & event) {
	wxPoint pos = GetScreenPosition();
	
	wheelRotation = event.GetWheelRotation();

	if (wheelRotation > 0)
		SetPosition(wxPoint(pos.x, pos.y-10));
	else 
		SetPosition(wxPoint(pos.x, pos.y+10));
}

//-----------------------------------------------------------------------

void PopupWindow::SetVertexToDraw (VertexRenderingInfo * v) {
	assert(v);
	assert(v->HasFullContents() && !v->HasHiddenContents());
	vertexToDraw = v;
	SetSize(v->GetWidth(), v->GetHeight());
}

//-----------------------------------------------------------------------

void PopupWindow::GetLogicalCoordinates (coord_t * x, coord_t * y, const wxPoint& pos) {
	wxClientDC dc(this);
    PrepareDC(dc);
    *x = dc.DeviceToLogicalX(pos.x);			
    *y = dc.DeviceToLogicalY(pos.y);
}


}	//namespace iviews