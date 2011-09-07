/**
 *	ZoomManager.cpp
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	December 2010
 */

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif

#include <wx/gdicmn.h>

#include "ZoomManager.h"


namespace iviews {

//-----------------------------------------------------------------------

ZoomManager::ZoomManager(wxScrolledWindow* _canvas) :	
	zoomParams(_canvas),
	isInitialised (false)
	{}

//-----------------------------------------------------------------------

void ZoomManager::Initialise (void) {

	if (isInitialised)
		return;

	zoomParams.Initialise();
	AdjustScrollSteps(zoomParams.GetInitialVirtualWidth(), zoomParams.GetInitialVirtualHeight());

	int	x_step, y_step;
	GetCanvas()->GetScrollPixelsPerUnit(&x_step, &y_step);

	GetCanvas()->Scroll(
		0, 
		(GetCanvas()->GetVirtualSize().GetHeight() / y_step) /  2	// H at origin, V in the middle
	);

	isInitialised = true;
}

//-----------------------------------------------------------------------

void ZoomManager::CleanUp (void) 
	{ zoomParams.CleanUp(); isInitialised = false; }

//-----------------------------------------------------------------------

void ZoomManager::ZoomIn (void) {
	if (zoomParams.CanZoomIn()) {
		Initialise();
		zoomParams.ZoomIn();
		ApplyCurrentZoom(zoomParams.GetFocalPoint());
	}
}

//-----------------------------------------------------------------------

void ZoomManager::ZoomOut(void) {
	if (zoomParams.CanZoomOut()) {
		Initialise();
		zoomParams.ZoomOut();
		ApplyCurrentZoom(zoomParams.GetFocalPoint());
	}
}

//-----------------------------------------------------------------------

void ZoomManager::ZoomFit (void) {
	Initialise();
	zoomParams.ZoomFit();
	ApplyCurrentZoom(zoomParams.GetFocalPoint());
}

//-----------------------------------------------------------------------

void ZoomManager::ZoomRestore (void) {
	Initialise();
	zoomParams.ZoomRestore();
	ApplyCurrentZoom(zoomParams.GetFocalPoint());
}

//-----------------------------------------------------------------------

void ZoomManager::MouseMove (const wxPoint & currentPos) 
	{ zoomParams.SetFocalPoint(currentPos); }

//-----------------------------------------------------------------------

void ZoomManager::AdjustScrollSteps (int newWidth, int newHeight) {

	wxSize viewSize = GetCanvas()->GetClientSize();
	int scrollSpaceH = viewSize.GetWidth() - GetCanvas()->GetScrollThumb(wxHORIZONTAL);
	int scrollSpaceV = viewSize.GetHeight() - GetCanvas()->GetScrollThumb(wxVERTICAL);

	if (scrollSpaceH == 0)
		scrollSpaceH = newWidth;
	
	if (scrollSpaceV == 0)
		scrollSpaceV = newHeight;
	
	int	x_step = newWidth / scrollSpaceH;
	int y_step = newHeight / scrollSpaceV;

	if (x_step && y_step)
		GetCanvas()->SetScrollRate(x_step, y_step);
}

//-----------------------------------------------------------------------

void ZoomManager::ApplyCurrentZoom (const wxPoint& focus) {

	int newViewX, newViewY, newWidth, newHeight;
	zoomParams.SetFocalPoint(focus);
	zoomParams.CalculateViewParameters(&newViewX, &newViewY, &newWidth, &newHeight);
	
	AdjustScrollSteps(newWidth, newHeight);

	int	x_step, y_step;
	GetCanvas()->GetScrollPixelsPerUnit(&x_step, &y_step);
	GetCanvas()->Scroll(newViewX / x_step, newViewY / y_step);
}

//-----------------------------------------------------------------------

}	//namespace iviews