/**
 *	HandTool.h
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	December 2010
 */

#ifndef	HAND_TOOL_H
#define HAND_TOOL_H


#include <wx/gdicmn.h>
#include <wx/cursor.h>
#include <wx/scrolwin.h>

namespace iviews {
class HandTool {

public:
	
	HandTool (wxScrolledWindow * owner);
	bool				IsActive (void) const;
	void				MouseMove (const wxPoint & currentPos, bool leftMouseButtonIsDown);
	void				MouseLeftUp (void);
	void				MouseLeftDown (const wxPoint & mousePos);


private:

	void				SaveStep (void);
	void				RestoreStep (void);

	bool				isActive;	// if hand tool activated.
	wxCursor			oldCursor;
	wxPoint				mousePositionOnClick;
	int					storedStepX, storedStepY;
	wxScrolledWindow *	owner;
};	


}	//namespace iviews 


#endif	//HAND_TOOL_H