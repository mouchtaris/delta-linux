#include "Bookmark.h"


namespace iviews {

//-----------------------------------------------------------------------

Bookmark::Bookmark(void) :	
	nullOblect			(false),
	initialVirtualWidth	(0),
	initialVirtualHeight(0),
	zoomFactor			(100),
	scaleFactor			(1.0),
	previousScaleFactor	(1.0),
	focalPoint			(0, 0),
	xStep				(0),
	yStep				(0),
	isMaximized			(false),
	windowSize			(),
	virtualSize			(),
	scrolPos			(0, 0),
	mouseLogicalPos		(0, 0)
{}

//-----------------------------------------------------------------------

Bookmark::Bookmark(
	const int		initialVirtualWidth_,
	const int		initialVirtualHeight_,
	const int		zoomFactor_,
	const double	scaleFactor_,
	const double	previousScaleFactor_,
	const wxPoint&	focalPoint_,
	const int		xStep_,
	const int		yStep_,
	const bool		isMaximized_,
	const wxSize&	windowSize_,
	const wxSize&	virtualSize_,
	const wxPoint&	scrolPos_,
	const wxPoint&	mouseLogicalPos_
) :	nullOblect			(false),
	initialVirtualWidth	(initialVirtualWidth_),
	initialVirtualHeight(initialVirtualHeight_),
	zoomFactor			(zoomFactor_),
	scaleFactor			(scaleFactor_),
	previousScaleFactor	(previousScaleFactor_),
	focalPoint			(focalPoint_),
	xStep				(xStep_),
	yStep				(yStep_),
	isMaximized			(isMaximized_),
	windowSize			(windowSize_),
	virtualSize			(virtualSize_),
	scrolPos			(scrolPos_),
	mouseLogicalPos		(mouseLogicalPos_)
{}

//-----------------------------------------------------------------------

int	Bookmark::GetInitialVirtualWidth (void) const
	{ return initialVirtualWidth; }

//-----------------------------------------------------------------------

void Bookmark::SetInitialVirtualWidth (const int width)
	{ initialVirtualWidth = width; }

//-----------------------------------------------------------------------

int	Bookmark::GetInitialVirtualHeight (void) const
	{ return initialVirtualHeight; }

//-----------------------------------------------------------------------

void Bookmark::SetInitialVirtualHeight (const int height)
	{ initialVirtualHeight = height; }

//-----------------------------------------------------------------------

int	Bookmark::GetZoomFactor (void) const
	{ return zoomFactor; }

//-----------------------------------------------------------------------

void Bookmark::SetZoomFactor (const int factor)
	{ zoomFactor = factor; }

//-----------------------------------------------------------------------

double Bookmark::GetScaleFactor (void) const
	{ return scaleFactor; }

//-----------------------------------------------------------------------

void Bookmark::SetScaleFactor (const double factor)
	{ scaleFactor = factor; }	

//-----------------------------------------------------------------------

double Bookmark::GetPreviousScaleFactor (void) const
	{ return previousScaleFactor; }

//-----------------------------------------------------------------------

void Bookmark::SetPreviousScaleFactor (const double factor)
	{ previousScaleFactor = factor; }

//-----------------------------------------------------------------------

wxPoint Bookmark::GetFocalPoint (void) const
	{ return focalPoint; }

//-----------------------------------------------------------------------

void Bookmark::setFocalPoint (const wxPoint & pt)
	{ focalPoint = pt; }

//-----------------------------------------------------------------------

int	Bookmark::GetXStep (void) const
	{ return xStep; }

//-----------------------------------------------------------------------

void Bookmark::SetXStep (const int step)
	{ xStep = step; }

//-----------------------------------------------------------------------

int Bookmark::GetYStep (void) const
	{ return yStep; }

//-----------------------------------------------------------------------

void Bookmark::SetYStep (const int step)
	{ yStep = step; }	

//-----------------------------------------------------------------------

bool Bookmark::IsMaximized (void) const
	{ return isMaximized; }

//-----------------------------------------------------------------------

void Bookmark::SetMaximized (const bool b)
	{ isMaximized = b; }

//-----------------------------------------------------------------------

wxSize Bookmark::GetWindowSize (void) const
	{ return windowSize; }

//-----------------------------------------------------------------------

void Bookmark::SetWindowSize (const wxSize & size)
	{ windowSize = size; }

//-----------------------------------------------------------------------

wxSize Bookmark::GetVirtualSize (void) const
	{ return virtualSize; }

//-----------------------------------------------------------------------

void Bookmark::SetVirtualSize (const wxSize & size)
	{ virtualSize = size; }

//-----------------------------------------------------------------------

wxPoint	Bookmark::GetScrollPosition (void) const
	{ return scrolPos; }

//-----------------------------------------------------------------------

void Bookmark::SetScrollPosition (const wxPoint & pt)
	{ scrolPos = pt; }

//-----------------------------------------------------------------------

wxPoint Bookmark::GetMouseLogicalPos (void) const 
	{ return mouseLogicalPos; }

//-----------------------------------------------------------------------

void Bookmark::SetMouseLogicalPos (const wxPoint & pt)
	{ mouseLogicalPos = pt; }

//-----------------------------------------------------------------------

Bookmark::operator bool (void) const 
	{ return !nullOblect; }

//-----------------------------------------------------------------------

const Bookmark & Bookmark::NullBookmark(void) {
	static Bookmark null;
	null.nullOblect = true;
	return null;
}

}	//namespace iviwes