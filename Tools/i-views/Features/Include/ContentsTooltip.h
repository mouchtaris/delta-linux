//August 2010
#ifndef CONTENTS_TOOLTIP_H
#define	CONTENTS_TOOLTIP_H


#include "Point.h"
#include "PopupWindow.h"
#include "LayoutParams.h"
#include "TypesConverter.h"
#include "RenderingTypes.h"
#include "RenderingParams.h"
#include "PopupWindowTimer.h"
#include "LayoutCalculator.h"
#include "VertexRenderingInfo.h"
#include "VertexToDebugInfoMapping.h"


namespace iviews {
	
class ContentsTooltip {
public:
	typedef graphs::VertexToDebugInfoMapping	VertexToDebugInfoMapping;

	virtual ~ContentsTooltip ();
	ContentsTooltip (
		Window*				parent, 
		LayoutParams*		lParams_,
		RenderingParams*	rParams_,
		long				delay,
		int					minZoomFactor
	);

	virtual bool 		MouseMove (
							VertexRenderingInfo*		v, 
							VertexToDebugInfoMapping*	mapping, 
							const geo::Point<coord_t>&	mousePos,
							int							currentZoomFactor
						);

	virtual int			GetMinimunZoomFactor (void) const;
	virtual void		SetMinimunZoomFactor (int newMinUserScale);


	virtual bool 		IsShown (void) const;
	virtual void 		OnMouseWheel (wxMouseEvent & evt);
	virtual void 		Stop (void);
	virtual long 		GetDelay (void) const; 
	virtual void 		SetDelay (long delay_);
	virtual void 		SetLayoutParams (LayoutParams * lParams);
	virtual void 		SetRenderingParams (RenderingParams * rParams);

protected:
	virtual void		ConditionalStopTimerAndHideWindow (void);
	virtual void		SetVerticesAndWindow (
							VertexRenderingInfo*		v, 
							VertexToDebugInfoMapping*	mapping
						);
	virtual bool		ConditionForFaultExecution (
							VertexRenderingInfo*		v,
							VertexToDebugInfoMapping*	mapping
						);
	

	//////////////////////////////////////////////////
	//
	Window*				parent;
	LayoutParams*		lParams;
	RenderingParams*	rParams;
	VertexRenderingInfo*newVertex;			//vertex for window creation
	VertexRenderingInfo*lastVisitedVertex;

	long				delay;
	int					minZoomFactor;
	int					currentZoomFactor;		
	PopupWindow			window;	
	PopupWindowTimer	timer;	
	LayoutCalculator	calculator;		

};

}	//namespace iviews

#endif //CONTENTS_TOOLTIP_H