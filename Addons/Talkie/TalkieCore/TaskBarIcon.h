/**
 *	TaskBarIcon.h
 *
 *	-- Talkie client application --
 *
 *	Tray icon placed on the task bar for quick access
 *	of the client application.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#ifndef TALKIE_TASKBARICON_H
#define TALKIE_TASKBARICON_H

#include "Common.h"
#include <wx/taskbar.h>

#include <boost/signal.hpp>
class wxMenu;

namespace talkie {

////////////////////////////////////////////////////////////////////////

class TaskBarIcon: public wxTaskBarIcon
{
public:

	typedef boost::signal<void (void)>		Sig1;
	typedef boost::signal<void (wxMenu*)>	Sig2;

	TaskBarIcon (wxEvtHandler* menuOwner) : m_menuOwner(menuOwner) {}
	~TaskBarIcon (void) {}

	virtual wxMenu* CreatePopupMenu (void);

	Sig1 sigDoubleClick;
	Sig1 sigClick;
	Sig2 sigCreateMenu;

private:
	DECLARE_EVENT_TABLE();

	void onClick (wxTaskBarIconEvent& event);
	void onDoubleClick (wxTaskBarIconEvent& event);
	void onMenu (wxCommandEvent& event);

	wxEvtHandler* m_menuOwner;
};

////////////////////////////////////////////////////////////////////////

} // namespace talkie

#endif	//TALKIE_TASKBARICON_H
