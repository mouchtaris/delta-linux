/**
 *	main.cpp
 *
 *	-- Talkie Centralized Directory stand-alone Server --
 *
 *	Centralized directory server of the talkie
 *	application as a stand-alone binary.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#define WIN32_LEAN_AND_MEAN	///< Be lean and mean

#include "Streams.h"
#include "StringUtils.h"

#include "MainWindow.h"

#include <wx/defs.h>
#include <wx/app.h>
#include <wx/toplevel.h>
#include <wx/image.h>
#include <wx/event.h>
#include <wx/log.h>
#include <wx/frame.h>

#include <vld.h>

DECLARE_EVENT_TYPE(EVENT_PRINT, -1);
DEFINE_EVENT_TYPE(EVENT_PRINT);

//----------------------------
//-- class LogWindow

class LogWindow :
	public wxEvtHandler, public wxLogWindow, public util::streamview
{
public:
	LogWindow(const wxString& title) : wxLogWindow(0, title, true)
		{ util::get_dbg_stream().add_view(this); }
	~LogWindow(void)
		{ util::get_dbg_stream().remove_view(this); }

protected:
	bool OnFrameClose(wxFrame* frame) {
		frame->Destroy();
		return true;
	}

private:
	DECLARE_EVENT_TABLE();

	void onDisplay(wxCommandEvent& event)	{ this->DoLogString(event.GetString().c_str(), 0); }

	virtual void display(const char* buffer, size_t len) {
		if (buffer[len - 1] == '\n')
			--len;
		wxString output(buffer, wxConvUTF8, len);

		wxCommandEvent event(EVENT_PRINT);
		event.SetString(output);
		this->AddPendingEvent(event);
	}

	virtual void close (void) {}
};

//-----------------------------------------------------------------------

BEGIN_EVENT_TABLE(LogWindow, wxEvtHandler)
	EVT_COMMAND(wxID_ANY, EVENT_PRINT, LogWindow::onDisplay)
END_EVENT_TABLE();

//-----------------------------------------------------------------------

using namespace talkie;
using namespace net;

//----------------------------
//-- class CentralizedDirectoryGUIApp

class CentralizedDirectoryGUIApp :
	public wxApp
{
public:
	bool OnInit(void)
	{
#ifndef NDEBUG
		//logWindow = new LogWindow(_("Debug window"));
#endif
		//-- Create main window
		MainWindow* mainWindow = new MainWindow(_T("Talkie Directory Server"));
		SetTopWindow(mainWindow);
		//mainWindow->Show();

		return true;
	}

	virtual int OnExit(void)
	{
		return wxApp::OnExit();
	}

private:
	LogWindow* logWindow;
};

//-----------------------------------------------------------------------
 
DECLARE_APP(CentralizedDirectoryGUIApp);
IMPLEMENT_APP(CentralizedDirectoryGUIApp);

//-----------------------------------------------------------------------
