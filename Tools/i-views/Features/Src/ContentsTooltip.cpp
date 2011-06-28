#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif

#include <wx/dcclient.h>
#include "TypesConverter.h"
#include "ContentsTooltip.h"
#include "CoordinateConverter.h"
#include "WxGraphicsLayoutPrimitives.h"

namespace iviews {


//-----------------------------------------------------------------------

ContentsTooltip::ContentsTooltip(
	Window*				parent_,  
	LayoutParams*		lParams_,
	RenderingParams*	rParams_,
	long				delay_,
	int					minZoomFactor_) 
:	delay				(delay_),
	parent				(parent_),
	window				(parent_),
	lParams				(lParams_),
	rParams				(rParams_),
	calculator			(lParams_, rParams_),
	timer				(&window),
	minZoomFactor		(minZoomFactor_),
	currentZoomFactor	(0),	
	newVertex			((VertexRenderingInfo *)0),
	lastVisitedVertex	((VertexRenderingInfo *)0)
{}

//-----------------------------------------------------------------------

ContentsTooltip::~ContentsTooltip(){
	if (newVertex)
		delete newVertex;
}

//-----------------------------------------------------------------------

bool ContentsTooltip::IsShown(void) const
	{ return window.IsShown(); }

//-----------------------------------------------------------------------

void ContentsTooltip::OnMouseWheel(wxMouseEvent & evt) 
	{ window.OnMouseWheel(evt); }


int ContentsTooltip::GetMinimunZoomFactor(void) const
	{ return minZoomFactor; }

//-----------------------------------------------------------------------

void ContentsTooltip::SetMinimunZoomFactor(int newMinZoomFactor)
	{ minZoomFactor = newMinZoomFactor; }


//-----------------------------------------------------------------------

void ContentsTooltip::Stop(void) 
	{ ConditionalStopTimerAndHideWindow(); }

//-----------------------------------------------------------------------

bool ContentsTooltip::MouseMove(	
		VertexRenderingInfo*		v, 
		VertexToDebugInfoMapping*	mapping,
		const geo::Point<coord_t>&	devMousePos,							
		int							currentZoomFactor_
	) {
	currentZoomFactor = currentZoomFactor_;
	ConditionalStopTimerAndHideWindow();
	
	if (ConditionForFaultExecution(v, mapping))
		return false;

	SetVerticesAndWindow(v, mapping);

	//startTimer and set mouse position
	if (Rectangle::InBoundaries(v->GetVertexMainRectangle(), devMousePos.GetX(), devMousePos.GetY()) ) {
		timer.Start(delay, true);
		window.SetPosition(wxGetMousePosition());
	}
	return true;
}

//-----------------------------------------------------------------------

long ContentsTooltip::GetDelay(void) const 
	{ return delay; }

//-----------------------------------------------------------------------

void ContentsTooltip::SetDelay(long delay_) 
	{ delay = delay_; }

//-----------------------------------------------------------------------

void ContentsTooltip::SetLayoutParams(LayoutParams * _lParams) {
	assert(_lParams);
	lParams = _lParams;
}

//-----------------------------------------------------------------------

void ContentsTooltip::SetRenderingParams(RenderingParams * _rParams) {
	assert(_rParams);
	rParams = _rParams;
}

//-----------------------------------------------------------------------

void ContentsTooltip::ConditionalStopTimerAndHideWindow(void) {
	if (timer.IsRunning())
		timer.Stop();
	if (window.IsShown())
		window.Show(false);
}

//-----------------------------------------------------------------------

void ContentsTooltip::SetVerticesAndWindow(
		VertexRenderingInfo			* v, 
		VertexToDebugInfoMapping	* mapping
	) {
	if ((!lastVisitedVertex && !newVertex)				|| 
		(lastVisitedVertex && lastVisitedVertex != v)) 
	{	
		if (newVertex)
			delete newVertex;
	
		wxClientDC dc(parent);
	    parent->PrepareDC(dc);
		lastVisitedVertex	= v;

		bool oldValue		= lParams->layerParams.drawVertexWithFullContents;	//hold the old value
		lParams->layerParams.drawVertexWithFullContents = true;
		newVertex			=  calculator.CalculateVertexWithFullContents(
									v->GetSourceVertex(),
									mapping
								);
		lParams->layerParams.drawVertexWithFullContents = oldValue;
		window.SetVertexToDraw(newVertex);
		window.SetPosition(wxGetMousePosition());
	}
}

//-----------------------------------------------------------------------

bool ContentsTooltip::ConditionForFaultExecution (
		VertexRenderingInfo			* v,
		VertexToDebugInfoMapping	* mapping
	) { 
	return	!v										|| 
			!mapping								|| 
			(v->HasFullContents() && !v->HasHiddenContents() && (minZoomFactor < currentZoomFactor)); 
}

}	//namespace iviews 