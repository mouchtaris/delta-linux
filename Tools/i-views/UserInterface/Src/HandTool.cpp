#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/dcclient.h>

#include "HandTool.h"
#include "dragger.xpm"

namespace iviews {

//-----------------------------------------------------------------------

HandTool::HandTool(wxScrolledWindow * owner_) :	
	owner					(owner_),
	isActive				(false),
	storedStepX				(1),
	storedStepY				(1),
	oldCursor				(wxCursor(wxCURSOR_ARROW)),
	mousePositionOnClick	(0, 0)
	{}

//-----------------------------------------------------------------------

bool HandTool::IsActive (void) const 
	{ return isActive; }

//-----------------------------------------------------------------------

void HandTool::MouseMove (const wxPoint & currentPos, bool leftMouseButtonIsDown) {

	if (!leftMouseButtonIsDown) {
		if (isActive)
			MouseLeftUp();
	}
	else
	if (!isActive)
		MouseLeftDown(currentPos);
	else {
		int	x_step, y_step;
		owner->GetScrollPixelsPerUnit(&x_step, &y_step);

		wxClientDC dc(owner);
		owner->PrepareDC(dc);

		int dx		= mousePositionOnClick.x - dc.DeviceToLogicalX(currentPos.x);
		int dy		= mousePositionOnClick.y - dc.DeviceToLogicalY(currentPos.y);
		double hPos	= (owner->GetScrollPos(wxHORIZONTAL) * x_step + dx) / ((double) x_step);
		double vPos	= (owner->GetScrollPos(wxVERTICAL) * y_step + dy) / ((double) y_step);
	
		owner->Scroll((int) hPos, (int) vPos);	
	}
}

//-----------------------------------------------------------------------

void HandTool::MouseLeftUp(void) { 
	owner->Freeze();
	isActive = false;
	owner->SetCursor(oldCursor); 

	// Necessary to force change of the cursor.
	wxPoint  mouseState = wxGetMousePosition();
	owner->ScreenToClient(&mouseState.x, &mouseState.y);
	owner->WarpPointer(mouseState.x, mouseState.y + 1);
	owner->WarpPointer(mouseState.x, mouseState.y - 1);

	RestoreStep();
	owner->Thaw();
}

//-----------------------------------------------------------------------

void HandTool::MouseLeftDown (const wxPoint & mousePos) {
	owner->Freeze();
	isActive	= true;
	oldCursor	= owner->GetCursor();

	int	x_step, y_step;
	owner->GetScrollPixelsPerUnit(&x_step, &y_step);
	int hPos = owner->GetScrollPos(wxHORIZONTAL) * x_step;
	int vPos = owner->GetScrollPos(wxVERTICAL) * y_step;
	
	SaveStep();

	owner->SetScrollRate(1, 1);
	owner->Scroll(hPos, vPos);	

	wxClientDC dc(owner);
	owner->PrepareDC(dc);
    mousePositionOnClick.x = dc.DeviceToLogicalX(mousePos.x);
	mousePositionOnClick.y = dc.DeviceToLogicalY(mousePos.y);

	owner->SetCursor(wxCursor(wxBitmap(dragger_xpm).ConvertToImage()));
	owner->Thaw();
}

//-----------------------------------------------------------------------

void HandTool::SaveStep (void) 
	{ owner->GetScrollPixelsPerUnit(&storedStepX, &storedStepY); }

//-----------------------------------------------------------------------

void HandTool::RestoreStep (void) {

	if (storedStepX && storedStepY) {
		int hPos = owner->GetScrollPos(wxHORIZONTAL) / storedStepX;
		int vPos = owner->GetScrollPos(wxVERTICAL) / storedStepY;
		
		owner->SetScrollRate(storedStepX, storedStepY);
		owner->Scroll(hPos, vPos);	
	}
}

//-----------------------------------------------------------------------

}	//namespace iviews 