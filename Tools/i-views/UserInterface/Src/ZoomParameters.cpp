/**
 *	ZoomParameters.cpp
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	December 2010
 */

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif

#include <wx/gdicmn.h>
#include <wx/dcclient.h>


#include "ZoomParameters.h"


#define	MAX_ZOOM_FACTOR			500
#define	MIN_ZOOM_FACTOR			1
#define	INITIAL_ZOOM_FACTOR		100
#define INITIAL_SCALE_FACTOR	1.0
#define	ZOOM_STEP_FACTOR		5
#define MIN_ZOOM_STEP_FACTOR	2


namespace iviews {
	//-----------------------------------------------------------------------
	
ZoomParameters::ZoomParameters (wxScrolledWindow* _canvas) :
	initialVirtualWidth		(0), 
	initialVirtualHeight	(0),
	scaleFactor				(INITIAL_SCALE_FACTOR),
	previousScaleFactor		(INITIAL_SCALE_FACTOR),
	zoomFactor				(INITIAL_ZOOM_FACTOR),
	isInitialised			(false),
	canvas					(_canvas) {
}

//*********************************

void ZoomParameters::Initialise (void) {
	if (!isInitialised) {
		canvas->GetVirtualSize(&initialVirtualWidth, &initialVirtualHeight);
		zoomFactor			= INITIAL_ZOOM_FACTOR;
		scaleFactor 		= INITIAL_SCALE_FACTOR;
		previousScaleFactor = INITIAL_SCALE_FACTOR;

	}
	isInitialised = true;
}

void ZoomParameters::CleanUp (void) 
	{ isInitialised = false; }

wxString ZoomParameters::ToString (void) const {
	wxString message;
	message.Printf( wxT("Zoom %u%c"), zoomFactor, '%');
	return message;
}

////////////////////////////////////////////////////////////////////////

bool ZoomParameters::CanZoomIn (void) const 
	{ return zoomFactor < MAX_ZOOM_FACTOR; }

void ZoomParameters::ZoomIn (void) {
	if (zoomFactor < ZOOM_STEP_FACTOR)
		if (zoomFactor * MIN_ZOOM_STEP_FACTOR <= ZOOM_STEP_FACTOR)
			zoomFactor *= MIN_ZOOM_STEP_FACTOR;
		else
			zoomFactor = ZOOM_STEP_FACTOR;
	else
		zoomFactor += ZOOM_STEP_FACTOR;
	ComputeScaleFactor();
}

////////////////////////////////////////////////////////////////////////

bool ZoomParameters::CanZoomOut (void) const 
	{ return zoomFactor > MIN_ZOOM_FACTOR; }

void ZoomParameters::ZoomOut (void) {
	if (zoomFactor <= ZOOM_STEP_FACTOR)
		zoomFactor /= MIN_ZOOM_STEP_FACTOR;	// Divide.
	else
		zoomFactor -= ZOOM_STEP_FACTOR;
	ComputeScaleFactor();
}

////////////////////////////////////////////////////////////////////////

void ZoomParameters::ZoomFit (void) {

	wxSize clientSize = GetCanvas()->GetClientSize();

	bool scaleUp = initialVirtualWidth <= clientSize.GetWidth() && initialVirtualHeight <= clientSize.GetHeight();

	int zoomX = (clientSize.GetWidth() * 100)  / initialVirtualWidth;
	int zoomY = (clientSize.GetHeight() * 100) / initialVirtualHeight;

	if (!scaleUp)
		zoomFactor = zoomX < zoomY ? zoomX : zoomY;
	else
		zoomFactor = zoomX > zoomY ? zoomX : zoomY;

	if (zoomFactor < ZOOM_STEP_FACTOR) {
		if (zoomFactor == 3)
			zoomFactor = MIN_ZOOM_STEP_FACTOR;
	}
	else

		zoomFactor -= (zoomFactor % ZOOM_STEP_FACTOR);

	focalPoint.x = clientSize.GetWidth() / 2;
	focalPoint.y = clientSize.GetHeight() / 2;

	ComputeScaleFactor();
}

////////////////////////////////////////////////////////////////////////

void ZoomParameters::ZoomRestore (void) 
	{ zoomFactor = INITIAL_ZOOM_FACTOR; ComputeScaleFactor(); }

////////////////////////////////////////////////////////////////////////

void ZoomParameters::CalculateViewParameters (
		int* viewX,
		int* viewY,
		int* virtualW,
		int* virtualH
	) {
	wxClientDC dc(canvas);
	GetCanvas()->PrepareDC(dc);
	double scaleDiv = scaleFactor / previousScaleFactor;

	*viewX		= dc.DeviceToLogicalX(focalPoint.x) * scaleDiv;
	*viewY		= dc.DeviceToLogicalY(focalPoint.y) * scaleDiv;
	*virtualW	= initialVirtualWidth * scaleFactor;
	*virtualH	= initialVirtualHeight * scaleFactor;

	GetCanvas()->SetVirtualSize(*virtualW, *virtualH);

	*viewX -= (dc.DeviceToLogicalX(focalPoint.x) - dc.DeviceToLogicalX(0));
	*viewY -= (dc.DeviceToLogicalY(focalPoint.y) - dc.DeviceToLogicalY(0));
}

////////////////////////////////////////////////////////////////////////

void ZoomParameters::ComputeScaleFactor (void) {
	previousScaleFactor = scaleFactor;
	scaleFactor = ((double) zoomFactor) / ((double) INITIAL_ZOOM_FACTOR);
}

////////////////////////////////////////////////////////////////////////

void ZoomParameters::SetZoomFactor (const int factor)
	{ zoomFactor = factor; }

int ZoomParameters::GetZoomFactor(void) const
	{ return zoomFactor; }

void ZoomParameters::SetScaleFactor(const double s) 
	{ scaleFactor = s; } 

double ZoomParameters::GetScaleFactor(void) const 
	{ return scaleFactor; }

double ZoomParameters::GetPreviousScaleFactor (void) const
	{ return previousScaleFactor; } 

void ZoomParameters::SetPreviousScaleFactor (const double factor)
	{ previousScaleFactor = factor; }

int ZoomParameters::GetInitialVirtualWidth (void) const
	{ return initialVirtualWidth; }

void ZoomParameters::SetInitialVirtualWidth (const int width)
	{ initialVirtualWidth = width; }

int ZoomParameters::GetInitialVirtualHeight (void) const
	{ return initialVirtualHeight; }

void ZoomParameters::SetInitialVirtualHeight (const int height)
	{ initialVirtualHeight = height; }


}	//naemspace iviews