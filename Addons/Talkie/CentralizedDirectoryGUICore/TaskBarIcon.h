/**
 *	TaskBarIcon.h
 *
 *	-- Talkie Centralized Directory stand-alone Server --
 *
 *	Tray icon placed on the task bar for quick access
 *	of the directory.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#ifndef CENTRALIZED_DIRECTORY_GUI_TASKBAR_ICON_H_
#define CENTRALIZED_DIRECTORY_GUI_TASKBAR_ICON_H_

#include "Common.h"
#include "StringDefs.h"
#include "CommonFwdDecls.h"
#include "UndefineAllWin32Shit.h"

#include <wx/taskbar.h>

class wxMenu;

namespace talkie
{
	class MainWindow;

	//----------------------------
	//-- class TaskBarIcon

	class TaskBarIcon :
		public wxTaskBarIcon
	{
	public:
		///--- constructors / destructor
		TaskBarIcon(MainWindow* mainWindow)
			: mainWindow(mainWindow) {}
		~TaskBarIcon(void) {}

		///--- public API
		virtual wxMenu* CreatePopupMenu(void);

	private:
		///--- event table
		DECLARE_EVENT_TABLE();

		///--- private API
		void onDClick(wxTaskBarIconEvent& event);

		void onStart(wxCommandEvent& event);
		void onStop(wxCommandEvent& event);
		void onConfig(wxCommandEvent& event);
		void onExit(wxCommandEvent& event);

		///--- private members
		MainWindow* mainWindow;
	};
}

#endif	//CENTRALIZED_DIRECTORY_GUI_TASKBAR_ICON_H_
