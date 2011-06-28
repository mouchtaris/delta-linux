//sempteber 2010
#ifndef TIP_TIMER_H
#define TIP_TIMER_H


#include <string>


#include <wx/timer.h>
#include <wx/window.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
//you need to add defs with tipwind. Wx problem
//#include <wx/defs.h>
//#include <wx/tipwin.h>
#include "TipWindow.h"


namespace iviews {

class TipTimer : public wxTimer {
public:
	TipTimer(void);

	virtual void Notify (void);
	void SetTipPropeties (wxWindow * parent_, const std::string & text_);

private:
	
	std::string		text;
	wxWindow	*	parent;
	TipWindow	*	tipWindow;
};
	
}	//namespace iviews 


#endif //TIP_TIMER_H