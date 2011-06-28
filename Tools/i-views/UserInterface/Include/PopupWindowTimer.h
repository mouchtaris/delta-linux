#ifndef POPUP_WINDOW_TIMER_H
#define POPUP_WINDOW_TIMER_H



#include <wx/timer.h>
#include <wx/window.h>
//#include "PopupWindow.h"


namespace iviews {
class PopupWindowTimer : public wxTimer {
public:
	PopupWindowTimer(wxWindow * popupWindow);
	virtual void Notify(void);

private:
	wxWindow * popupWindow;
};

}	//namespace iviews 

#endif //POPUP_WINDOW_TIMER_H