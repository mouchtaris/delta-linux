/**
 *	LensViewWindow.cpp
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	December 2010
 */

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif

#include <assert.h>
#include <stdlib.h>
#include <wx/sizer.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/dcclient.h>

#include "Lens.xpm"
#include "LensViewWindow.h"

namespace iviews {

BEGIN_EVENT_TABLE(LensViewWindow, wxFrame)
	EVT_CLOSE	(LensViewWindow::OnClose)	
END_EVENT_TABLE()

#define	LENS_WINDOW_DEFAULT_WIDTH	256
#define	LENS_WINDOW_DEFAULT_HEIGHT	256
#define	LENS_WINDOW_DEFAULT_SIZE	wxSize(LENS_WINDOW_DEFAULT_WIDTH, LENS_WINDOW_DEFAULT_HEIGHT)
#define	MOUSE_MIN_DX				4
#define	MOUSE_MIN_DY				4

#define	LENS_FRAME_STYLE				\
		wxSYSTEM_MENU				|	\
		wxRESIZE_BORDER				|	\
		wxCLOSE_BOX					|	\
		wxCAPTION					|	\
		wxCLIP_CHILDREN				|	\
		wxBORDER_SIMPLE				|	\
		wxNO_FULL_REPAINT_ON_RESIZE 

inline void NormaliseDec (int* val, int d) {
	if (*val > d)
		*val -= d;
	else
		*val = 0;
}

//-----------------------------------------------------------------------

LensViewWindow::LensViewWindow(wxWindow * parent, const wxFont& font)
:	wxFrame(
		parent, 
		wxID_ANY, 
		wxT("i-views (Lens View)"), 
		wxDefaultPosition, 
		LENS_WINDOW_DEFAULT_SIZE,
		LENS_FRAME_STYLE
	) {
	ResetVars();
	canvas = new LensViewCanvas(this, font);
	canvas->SetScrollbars(0, 0, 0, 0);
	wxBoxSizer * sizer = new wxBoxSizer( wxVERTICAL );
    sizer->Add(canvas, 1, wxEXPAND);
    SetAutoLayout(true);
    SetSizer(sizer);
}

//-----------------------------------------------------------------------

void LensViewWindow::ResetVars (void) {
	isEnabled		 = false;
	isInitialised	 = false;
	lensViewX		 = 0;
	lensViewY		 = 0;
	lensViewW		 = 0; 
	lensViewH		 = 0;
}

//-----------------------------------------------------------------------

void LensViewWindow::SetLensFont(const wxFont & font)
	{ assert(canvas); canvas->SetLensFont(font);}

//-----------------------------------------------------------------------

void LensViewWindow::SetPrimitiveList(PrimitivePtrList * primitives)
	{ canvas->SetPrimitiveList(primitives); }

//-----------------------------------------------------------------------

bool LensViewWindow::Show (bool show) {
	if (show)
		GetParent()->SetCursor(wxCursor(wxCURSOR_MAGNIFIER));
	else
		GetParent()->SetCursor(wxCursor(wxCURSOR_ARROW));
	isEnabled = show;
	return this->wxTopLevelWindow::Show(show);
}

//-----------------------------------------------------------------------

void LensViewWindow::ZoomIn (void) 
	{ canvas->ZoomIn(); }

void LensViewWindow::ZoomOut (void)
	{ canvas->ZoomOut(); }

void LensViewWindow::ZoomRestore (void)
	{ canvas->ZoomRestore(); }

//-----------------------------------------------------------------------

void LensViewWindow::RefreshLogicalViewSize (wxDC& mainCanvasDC) {

	wxSize size = canvas->GetClientSize();
	lensViewW = mainCanvasDC.DeviceToLogicalXRel(size.GetWidth()) / 2;
	lensViewH = mainCanvasDC.DeviceToLogicalYRel(size.GetHeight()) / 2;
}

//-----------------------------------------------------------------------

void LensViewWindow::ComputeLogicalOrigin (
		int		canvasX,
		int		canvasY,
		int*	lensX,
		int*	lensY,
		double	mainScaleFactor
	) {

	wxClientDC dc(canvas);
	PrepareDC(dc);

	wxSize size = canvas->GetClientSize();
	
	int w = dc.DeviceToLogicalXRel(size.GetWidth() / 2) / canvas->GetZoomParams().GetScaleFactor();
	int h = dc.DeviceToLogicalYRel(size.GetHeight() / 2) / canvas->GetZoomParams().GetScaleFactor();

	*lensX = canvasX;
	*lensY = canvasY;

	NormaliseDec(lensX, w);
	NormaliseDec(lensY, h);
}

//-----------------------------------------------------------------------

void LensViewWindow::HandleMouseMoveOnMainCanvas (
		const wxPoint&	mainCanvasLogicalPosition, 
		double			mainScaleFactor
	) {

	int	lensX, lensY;

	ComputeLogicalOrigin(
		mainCanvasLogicalPosition.x, 
		mainCanvasLogicalPosition.y, 
		&lensX, 
		&lensY, 
		mainScaleFactor
	);

	canvas->SetLensLogicalOrigin(lensX, lensY);
}

//-----------------------------------------------------------------------

void LensViewWindow::MoveLens (
		wxScrolledWindow*	mainCanvas,
		wxDC&				mainCanvasDC,
		const wxPoint&		logicalMousePos,
		const wxPoint&		deviceMousePos,
		double				mainScaleFactor,
		bool				clearOldRect
	) {

	assert(!IsInitialised() || (lensViewW && lensViewH));

	if (!isEnabled)
		return;

	if ((oldDeviceMousePos.x != deviceMousePos.x || oldDeviceMousePos.y != deviceMousePos.y) &&
		abs(oldDeviceMousePos.x - deviceMousePos.x) < MOUSE_MIN_DX	&& 
		abs(oldDeviceMousePos.y - deviceMousePos.y) < MOUSE_MIN_DY)
		return;

	oldDeviceMousePos	= deviceMousePos;
	oldLogicalMousePos	= logicalMousePos;

	int	oldLensViewX = lensViewX;
	int	oldLensViewY = lensViewY;
	int	oldLensViewW = lensViewW;
	int	oldLensViewH = lensViewH;

	HandleMouseMoveOnMainCanvas(logicalMousePos, mainScaleFactor);
	RefreshLogicalViewSize(mainCanvasDC);

	lensViewX = logicalMousePos.x, lensViewY = logicalMousePos.y;
	lensViewW = (mainScaleFactor * lensViewW) / canvas->GetZoomParams().GetScaleFactor();
	lensViewH = (mainScaleFactor * lensViewH) / canvas->GetZoomParams().GetScaleFactor();

	NormaliseDec(&lensViewX, lensViewW);
	NormaliseDec(&lensViewY, lensViewH);
	lensViewH *= 2, lensViewW *= 2;

	// This ensures we get values with deterministic conversion to device coords.
	lensViewX	= mainCanvasDC.DeviceToLogicalX(mainCanvasDC.LogicalToDeviceX(lensViewX));
	lensViewY	= mainCanvasDC.DeviceToLogicalY(mainCanvasDC.LogicalToDeviceY(lensViewY));
	lensViewW	= mainCanvasDC.DeviceToLogicalXRel(mainCanvasDC.LogicalToDeviceXRel(lensViewW));
	lensViewH	= mainCanvasDC.DeviceToLogicalYRel(mainCanvasDC.LogicalToDeviceYRel(lensViewH));

	isInitialised = true;

	if (clearOldRect) {
		bool viewChanged =	mainCanvasDC.LogicalToDeviceX(oldLensViewX)		!= mainCanvasDC.LogicalToDeviceX(lensViewX)		||
							mainCanvasDC.LogicalToDeviceY(oldLensViewY)		!= mainCanvasDC.LogicalToDeviceY(lensViewY)		||
							mainCanvasDC.LogicalToDeviceXRel(oldLensViewW)	!= mainCanvasDC.LogicalToDeviceXRel(lensViewW)	||
							mainCanvasDC.LogicalToDeviceYRel(oldLensViewH)	!= mainCanvasDC.LogicalToDeviceYRel(lensViewH);

		if (IsInitialised() && viewChanged)
			Display(		// CLEAR
				oldLensViewX,
				oldLensViewY,
				oldLensViewW,
				oldLensViewH,				
				mainCanvasDC
			);
		
		if (viewChanged)	// DRAW
			Display(
				lensViewX,
				lensViewY,
				lensViewW,
				lensViewH,				
				mainCanvasDC
			);
	} 
	else
		Display(
			lensViewX,
			lensViewY,
			lensViewW,
			lensViewH,				
			mainCanvasDC
		);

	mainCanvas->Update();
}

//-----------------------------------------------------------------------

void LensViewWindow::Display (
		int		lx,
		int		ly,
		int		lw,
		int		lh,
		wxDC&	mainCanvasDC
	) {

	assert(IsInitialised());

	int oldFunction = mainCanvasDC.GetLogicalFunction();
	mainCanvasDC.SetLogicalFunction(wxXOR);
	mainCanvasDC.SetBrush(*wxTRANSPARENT_BRUSH);
	mainCanvasDC.SetPen(wxPen(*wxWHITE, 2, wxLONG_DASH));	
	mainCanvasDC.DrawRectangle(lx, ly, lw, lh);
	mainCanvasDC.SetLogicalFunction(oldFunction);
}

void LensViewWindow::DisplayLensRectangle (wxDC& mainCanvasDC) 
	{ Display(lensViewX, lensViewY, lensViewW, lensViewH, mainCanvasDC); }

//-----------------------------------------------------------------------

wxString LensViewWindow::ToString (void) const {
	if (isEnabled) {
		wxString message;
		message.Printf(wxT("Lens view [x:%d y:%d w:%d h:%d]"), lensViewX, lensViewY, lensViewW, lensViewH);
		return message;
	}
	else
		return wxT("");
}

//-----------------------------------------------------------------------

void LensViewWindow::OnClose(wxCloseEvent &event) { 
	Show(false); 
	ResetVars();
}

}	//namespace iviews