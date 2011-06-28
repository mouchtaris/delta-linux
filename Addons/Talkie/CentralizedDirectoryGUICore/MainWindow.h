/**
 *	MainWindow.h
 *
 *	-- Talkie Centralized Directory stand-alone Server --
 *
 *	Main window of centralized directory server
 *	for talkie network.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#ifndef CENTRALIZED_DIRECTORY_GUI_MAINWINDOW_H_
#define CENTRALIZED_DIRECTORY_GUI_MAINWINDOW_H_

#include "Common.h"
#include "StringDefs.h"
#include "CommonFwdDecls.h"
#include "UndefineAllWin32Shit.h"

#include <wx/frame.h>

#include <boost/signal.hpp>

class wxButton;
class wxTextCtrl;
class wxStaticText;
class wxStaticBitmap;

namespace net {
	class IOServiceThread;
}

namespace talkie
{
	class DirectoryServer;
	class TaskBarIcon;

	//----------------------------
	//-- class MainWindow

	class MainWindow :
		public wxFrame
	{
	public:
		///--- constructors / destructor
		MainWindow(const String& title);
		~MainWindow(void);

		///--- public API
		uint16 GetPort(void) const;
		bool IsServerRunning(void) const { return running; }

		void SetInfoMessage(const String& message);
		void SetErrorMessage(const String& message);

		void StartServer(void);
		void StopServer(void);

		///--- public Signals
		boost::signal<void ()> sigStart;
		boost::signal<void ()> sigStop;
		boost::signal<void ()> sigClose;

	private:
		///--- event table
		DECLARE_EVENT_TABLE();

		///--- private API
		void onEnter(wxCommandEvent& event);
		void onStartPressed(wxCommandEvent& event);
		void onEntriesPressed(wxCommandEvent& event);
		void onClose(wxCloseEvent& event);

		///--- private members
		net::IOServiceThread* io_service;
		DirectoryServer* server;
		bool running;

		TaskBarIcon* taskBar;

		wxStaticBitmap*	imgLogo;
		wxTextCtrl*		txtPort;
		wxButton*		btnStart;
		wxButton*		btnEntries;
		wxStaticText*	lblMessage;
		wxMenu*			trayMenu;
	};

} // namespace talkie

#endif	// CENTRALIZED_DIRECTORY_GUI_MAINWINDOW_H_
