#include "PopupWindowTimer.h"

#include <assert.h>

namespace iviews {

PopupWindowTimer::PopupWindowTimer(wxWindow * popupWindow_)
						:	popupWindow(popupWindow_){}
		

void PopupWindowTimer::Notify(void){ 
	assert(!popupWindow->IsShown());
	popupWindow->Show(true);
	popupWindow->Refresh();
}


}	//namespace ivies