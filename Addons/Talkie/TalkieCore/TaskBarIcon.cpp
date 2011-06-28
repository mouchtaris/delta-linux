/**
 *	TaskBarIcon.cpp
 *
 *	-- Talkie client application --
 *
 *	Tray icon placed on the task bar for quick access
 *	of the client application.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#include "TaskBarIcon.h"
#include "MainWindow.h"

#include <wx/menu.h>

namespace talkie {

////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(TaskBarIcon, wxTaskBarIcon)
	EVT_MENU(wxID_ANY, TaskBarIcon::onMenu)
	EVT_TASKBAR_LEFT_DOWN(TaskBarIcon::onClick)
	EVT_TASKBAR_LEFT_DCLICK(TaskBarIcon::onDoubleClick)
END_EVENT_TABLE();

//**********************************************************************

wxMenu* TaskBarIcon::CreatePopupMenu (void)
{
	wxMenu* trayMenu = new wxMenu; // Deleted by da system

	sigCreateMenu(trayMenu);

	return trayMenu;
}

//**********************************************************************

void TaskBarIcon::onClick (wxTaskBarIconEvent& event)
{
	sigClick();
}

//**********************************************************************

void TaskBarIcon::onDoubleClick (wxTaskBarIconEvent& event)
{
	sigDoubleClick();
}

//**********************************************************************

void TaskBarIcon::onMenu (wxCommandEvent& event)
{
	m_menuOwner->ProcessEvent(event);
}

////////////////////////////////////////////////////////////////////////

} // namespace talkie
