#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif

#include <assert.h>

#include <wx/gdicmn.h>

#include "Rectangle.h"
#include "IviewsCanvas.h"
#include "IviewsWindow.h"
#include "FitToWindowTool.h"

namespace iviews {
//-----------------------------------------------------------------------

bool FitToWindowTool::operator()(
		IviewsCanvas * owner_,
		const Rectangle & layersBoundingBox,
		const Dimension & distanceFromTopLeftCorner
	) {
	assert(owner_);
	owner = owner_;

	if (CanFitToWindo(layersBoundingBox, distanceFromTopLeftCorner))
		return false;	//False == does not need Refresh

	if (IsBiggerThanWindow(layersBoundingBox, distanceFromTopLeftCorner))
		FitInSmallerWindow(layersBoundingBox, distanceFromTopLeftCorner);
	else
		FitInBiggerWindow(layersBoundingBox, distanceFromTopLeftCorner);

	return true;		//True == needs Refresh
}

////////////////////////////////////////////////
//	Private Method API Implementation

//#define COMPARE_BB_WITH_WINDOW(OPERATOR)

#define GET_SCALE_FACTOR()\
	owner->GetZoomManager().GetZoomParams().GetScaleFactor()

#define BB_MAX_WIDTH()														\
	bb.GetWidth() * GET_SCALE_FACTOR()

#define BB_MAX_HAIGHT()														\
	bb.GetHeight() * GET_SCALE_FACTOR()


#define BB_MIN_WIDTH()														\
	(bb.GetWidth() - distanceFromTopLeftCorner.GetWidth()) * GET_SCALE_FACTOR()

#define BB_MIN_HAIGHT()														\
	(bb.GetHeight() - distanceFromTopLeftCorner.GetHeight()) * GET_SCALE_FACTOR()


//-----------------------------------------------------------------------

bool FitToWindowTool::CanFitToWindo(
		const Rectangle & bb,
		const Dimension & distanceFromTopLeftCorner
	) {
	wxSize viewSize = owner->GetClientSize();
	length_t maxWidth	= BB_MAX_WIDTH();
	length_t maxHeight	= BB_MAX_HAIGHT();

	length_t minWidth	= BB_MIN_WIDTH();
	length_t minHeight	= BB_MIN_HAIGHT();

	if (minWidth <= viewSize.GetWidth() && minHeight <= viewSize.GetHeight())
		if (maxWidth >= viewSize.GetWidth() && maxHeight >= viewSize.GetHeight())
			return true;
		else {
			//to window einai megalitero apo to bb. 8a prpei na doume an mporei na ginei zoom in oste
			//na einai fit sto window
			minWidth	*= GET_SCALE_FACTOR();
			minHeight	*= GET_SCALE_FACTOR();		

			if (!(minWidth <= viewSize.GetWidth() && minHeight <= viewSize.GetHeight()))
				return true;
		}
	return false;
}

//-----------------------------------------------------------------------

bool FitToWindowTool::IsBiggerThanWindow(
		const Rectangle & bb,
		const Dimension & distanceFromTopLeftCorner
	) {
	wxSize viewSize = owner->GetSize();

	return	BB_MIN_WIDTH() > viewSize.GetWidth() ||
			BB_MIN_HAIGHT() > viewSize.GetHeight();
}

//-----------------------------------------------------------------------

void FitToWindowTool::FitInBiggerWindow(
		const Rectangle & bb,
		const Dimension & distanceFromTopLeftCorner
	) {
	wxSize		viewSize	= owner->GetSize();
	length_t	width		= BB_MIN_WIDTH();
	length_t	height		= BB_MIN_HAIGHT();

	assert(width < viewSize.GetWidth() && height < viewSize.GetHeight());

	while (width < viewSize.GetWidth() && height < viewSize.GetHeight()){
		owner->GetZoomManager().ZoomIn();
		width	*= GET_SCALE_FACTOR();
		height	*= GET_SCALE_FACTOR();		
	}
	owner->Scroll(0, 0);
}

//-----------------------------------------------------------------------

void FitToWindowTool::FitInSmallerWindow(
		const Rectangle & bb,
		const Dimension & distanceFromTopLeftCorner
	) {
	wxSize		viewSize	= owner->GetSize();
	length_t	width		= BB_MIN_WIDTH();
	length_t	height		= BB_MIN_HAIGHT();

	assert(width > viewSize.GetWidth() || height > viewSize.GetHeight());

	while (width > viewSize.GetWidth() || height > viewSize.GetHeight()){
		owner->GetZoomManager().ZoomOut();
		width	/= GET_SCALE_FACTOR();
		height	/= GET_SCALE_FACTOR();		
	}
	owner->Scroll(0, 0);
}

}	//namespace iviews