#ifndef	LENS_TOOL_H
#define	LENS_TOOL_H


#include <wx/dc.h>
#include <wx/font.h>
#include <wx/frame.h>
#include <wx/gdicmn.h>
#include <wx/scrolwin.h>

#include "Primitive.h"
#include "LensViewCanvas.h"

namespace iviews {

class LensViewWindow : public wxFrame { 
	public:
	typedef Primitive::PrimitivePtrList	PrimitivePtrList;

	LensViewWindow(wxWindow * parent, const wxFont& font);

	void		SetLensFont(const wxFont & font);
	void		SetPrimitiveList (PrimitivePtrList * primitives);

	bool		IsEnabled (void) const
					{ return isEnabled; }
	bool		IsInitialised (void) const 
					{ return isInitialised; }

	void		ZoomIn (void);
	void		ZoomOut (void);
	void		ZoomRestore (void);
						
	virtual bool 
				Show (bool show = true);
	wxString	ToString (void) const;

	void		ComputeLogicalOrigin (
					int					canvasX,
					int					canvasY,
					int*				lensX,
					int*				lensY,
					double				mainScaleFactor
				);
	void		MoveLens (
					wxScrolledWindow*	mainCanvas,
					wxDC&				mainCanvasDC,
					const wxPoint&		logicalMousePos,
					const wxPoint&		deviceMousePos,
					double				mainScaleFactor,
					bool				clearOldRect
				);
	void		DisplayLensRectangle (wxDC& mainCanvasDC);
	const wxPoint
				GetOldDeviceMousePos (void) const 
					{ return oldDeviceMousePos; }
	const wxPoint
				GetOldLogicalMousePos (void) const 
					{ return oldLogicalMousePos; }

	private:
	void 		HandleMouseMoveOnMainCanvas (
					const wxPoint&	mainCanvasLogicalPosition, 
					double			mainScaleFactor
				);
	void		ResetVars (void);
	void		RefreshLogicalViewSize (wxDC& mainCanvasDC);
	void		Display (
					int		lX,
					int		lY,
					int		lW,
					int		lH,
					wxDC&	mainCanvasDC
				);

	void		OnClose(wxCloseEvent &event);

	bool				isEnabled;
	bool				isInitialised;
	LensViewCanvas*		canvas;
	int					lensViewX;
	int					lensViewY;
	int					lensViewW;
	int					lensViewH;
	wxPoint				oldDeviceMousePos;
	wxPoint				oldLogicalMousePos;

	DECLARE_EVENT_TABLE()
};

}


#endif	//LENS_TOOL_H