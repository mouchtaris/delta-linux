/**
 *	TaskBarIcon.cpp
 *
 *	-- Talkie Centralized Directory stand-alone Server --
 *
 *	Tray icon placed on the task bar for quick access
 *	of the directory.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#include "TaskBarIcon.h"
#include "MainWindow.h"

#include <wx/menu.h>
#include <wx/app.h>

namespace talkie
{
	enum TrayMenuCoomandType {
		TRM_START, TRM_STOP, TRM_CONFIG, TRM_EXIT,
	};

	BEGIN_EVENT_TABLE(TaskBarIcon, wxTaskBarIcon)
		EVT_MENU(TRM_START,		TaskBarIcon::onStart)
		EVT_MENU(TRM_STOP,		TaskBarIcon::onStop)
		EVT_MENU(TRM_CONFIG,	TaskBarIcon::onConfig)
		EVT_MENU(TRM_EXIT,		TaskBarIcon::onExit)

		EVT_TASKBAR_LEFT_DCLICK(TaskBarIcon::onDClick)
	END_EVENT_TABLE();

	//-------------------------------------------------------//
	//---- class TaskBarIcon --------------------------------//

	wxMenu* TaskBarIcon::CreatePopupMenu(void)
	{
		//-- Create Tray Menu
		wxMenu* trayMenu = new wxMenu;

		trayMenu->Append(TRM_START, _("Start Server"), _("Start Directory Server"));
		trayMenu->Append(TRM_STOP, _("Stop Server"), _("Stop Directory Server"));
		trayMenu->AppendSeparator();
		trayMenu->Append(TRM_CONFIG, _("Configure Server"), _("Configure Directory Server"));
		trayMenu->AppendSeparator();
		trayMenu->Append(TRM_EXIT, _("Exit"), _("Leaving so soon?"));

		trayMenu->Enable(TRM_START, !mainWindow->IsServerRunning());
		trayMenu->Enable(TRM_STOP,  mainWindow->IsServerRunning());

		return trayMenu;
	}

	//-----------------------------------------------------------------------

	void TaskBarIcon::onDClick(wxTaskBarIconEvent& event)
	{
		if (mainWindow->IsShown())
			mainWindow->Hide();
		else
			mainWindow->Show();
	}

	//-----------------------------------------------------------------------

	void TaskBarIcon::onStart(wxCommandEvent& event)
	{
		mainWindow->sigStart();
	}

	//-----------------------------------------------------------------------

	void TaskBarIcon::onStop(wxCommandEvent& event)
	{
		mainWindow->sigStop();
	}

	//-----------------------------------------------------------------------

	void TaskBarIcon::onConfig(wxCommandEvent& event)
	{
		mainWindow->Show();
	}

	//-----------------------------------------------------------------------

	void TaskBarIcon::onExit(wxCommandEvent& event)
	{
		mainWindow->Destroy();
	}

	//-----------------------------------------------------------------------
}
