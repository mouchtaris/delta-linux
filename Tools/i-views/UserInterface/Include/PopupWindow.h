/**
 *	PopupWindow.h
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	August 2010
 */

#ifndef POPUP_WINDOW_H
#define POPUP_WINDOW_H


#include <wx/popupwin.h>

#include "SlotTooltip.h"
#include "RenderingAPI.h"
#include "RenderingTypes.h"
#include "WxRenderingAPI.h"
#include "VertexRenderingInfo.h"
#include "LayredGraphRenderer.h"




namespace iviews {
class PopupWindow : public wxPopupWindow {
public:
	PopupWindow (wxWindow *parent, int flags = wxBORDER_NONE);

	void 					OnPaint (wxPaintEvent & event);
	void 					OnLeftDown (wxMouseEvent & event);
	void 					OnRightDown (wxMouseEvent & event);
	void 					OnMouseMove (wxMouseEvent & event);
	void 					OnMouseWheel (wxMouseEvent & event);
	void					SetVertexToDraw (VertexRenderingInfo * v);

private:
	void					GetLogicalCoordinates (coord_t * x, coord_t * y, const wxPoint& pos);

	int						wheelRotation;
	SlotTooltip				slotTooltip;
	WxRenderingAPI			renderingAPI;
	LayredGraphRenderer		renderer;
	VertexRenderingInfo *	vertexToDraw;

   DECLARE_EVENT_TABLE ()

};

}	//namespace iviews

#endif	//POPUP_WINDOW_H