/**
 *	LensViewCanvas.cpp
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	December 2010
 */

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif

#include <assert.h>
#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include "LensViewCanvas.h"

namespace iviews {

BEGIN_EVENT_TABLE(LensViewCanvas, wxScrolledWindow)
    EVT_PAINT	(LensViewCanvas::OnPaint)
END_EVENT_TABLE()

//-----------------------------------------------------------------------

LensViewCanvas::LensViewCanvas (wxFrame * parent, const wxFont& font_)
:	wxScrolledWindow(
		parent, 
		wxID_ANY, 
		wxDefaultPosition, 
		wxDefaultSize,
		wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE
	),
	font			(font_),
	xLogicalOrigin	(0), 
	yLogicalOrigin	(0),
	rederingApi		(),
	renderer		(&rederingApi),
	primitives		((PrimitivePtrList*)0),
	zoomParams		(this)  
{}

//-----------------------------------------------------------------------

void LensViewCanvas::SetLensLogicalOrigin (int x, int y) {

	if (xLogicalOrigin != x || yLogicalOrigin != y) {
		xLogicalOrigin = x, yLogicalOrigin = y;
		Refresh();
	}
}

//-----------------------------------------------------------------------

void LensViewCanvas::SetLensFont (const wxFont & font_)
	{ font = font_; }

//-----------------------------------------------------------------------

void LensViewCanvas::SetPrimitiveList (PrimitivePtrList * primitives_) 
	{ primitives = primitives_; }

//-----------------------------------------------------------------------

void LensViewCanvas::ZoomPrepare (void) {
	zoomParams.Initialise();
	wxSize clientSize = GetClientSize();
	zoomParams.SetFocalPoint(wxPoint(clientSize.GetWidth() / 2, clientSize.GetHeight() / 2));
}

//-----------------------------------------------------------------------

void LensViewCanvas::ZoomApply (void) {
	int newWidth, newHeight;
	zoomParams.CalculateViewParameters(&xLogicalOrigin, &yLogicalOrigin, &newWidth, &newHeight);
}

//-----------------------------------------------------------------------

void LensViewCanvas::ApplyZoomOperation (void (ZoomParameters::*op)(void)) {
	Freeze();
	(zoomParams.*op)();	
	ZoomApply();
	Thaw();
	Refresh();
}

//-----------------------------------------------------------------------

void LensViewCanvas::ZoomIn (void) {
	ZoomPrepare();
	if (zoomParams.CanZoomIn())
		ApplyZoomOperation(&ZoomParameters::ZoomIn);		
}

//-----------------------------------------------------------------------

void LensViewCanvas::ZoomOut (void) {
	ZoomPrepare();
	if (zoomParams.CanZoomOut())
		ApplyZoomOperation(&ZoomParameters::ZoomOut);
}

//-----------------------------------------------------------------------

void LensViewCanvas::ZoomRestore (void) {
	ZoomPrepare();
	ApplyZoomOperation(&ZoomParameters::ZoomRestore);
}

//-----------------------------------------------------------------------

bool LensViewCanvas::CanPaint (void) const
	{ return primitives && primitives->size() != 0; }

//-----------------------------------------------------------------------

void LensViewCanvas::OnPaint (wxPaintEvent &event) {

	if (CanPaint()) {

		wxPaintDC dc(this);
		DoPrepareDC(dc);

		dc.Clear();
		dc.SetLogicalOrigin(xLogicalOrigin, yLogicalOrigin);
		dc.SetAxisOrientation(true, false);
		dc.SetUserScale(zoomParams.GetScaleFactor(), zoomParams.GetScaleFactor());
		dc.SetMapMode(wxMM_TEXT);	//Each logical unit is 1 pixel.
		dc.SetFont(font);

		rederingApi.SetWxDC(&dc);
		assert(primitives);
		renderer(*primitives);
	}
}

}	//namespace iviews 