#ifndef	LENS_TOOL_CANVAS_H
#define	LENS_TOOL_CANVAS_H

#include <wx/dc.h>
#include <wx/scrolwin.h>

#include <wx/frame.h>
#include "WxRenderingAPI.h"
#include "LayredGraphRenderer.h"
#include "ZoomManager.h"

namespace iviews {

	class LensViewCanvas : public wxScrolledWindow{
	public:
		typedef Primitive::PrimitivePtrList	PrimitivePtrList;

		LensViewCanvas (wxFrame * parent, const wxFont& font_);

		void	SetLensLogicalOrigin (int x, int y);
		void 	SetLensFont (const wxFont & font);
		void 	SetPrimitiveList (PrimitivePtrList * primitives);

		void 	ZoomIn (void);
		void 	ZoomOut (void);
		void 	ZoomRestore (void);
		ZoomParameters& 
				GetZoomParams (void) 
					{ return zoomParams; }

	private:
		void	UpdateView (void);
		bool 	CanPaint (void) const;
		void 	OnPaint (wxPaintEvent &event);
		void 	OnClose (wxCloseEvent &event);
		void 	ZoomPrepare (void);
		void 	ZoomApply (void);
		void 	ApplyZoomOperation (void (ZoomParameters::*op)(void));

		int					xLogicalOrigin;
		int					yLogicalOrigin;
		wxFont				font;
		WxRenderingAPI		rederingApi;
		PrimitivePtrList*	primitives;
		LayredGraphRenderer renderer;
		ZoomParameters		zoomParams;

		DECLARE_EVENT_TABLE()
	};
}	//namespace iviews




#endif	//LENS_TOOL_CANVAS_H