/**
 *	Talkie.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#define WIN32_LEAN_AND_MEAN // Be lean and mean

#include "Streams.h"
#include "StringUtils.h"
#include "MainWindow.h"

#include "SimpleTimer.h"
#include "SoundManager.h"
#include "Mixer.h"

#include "IOServiceThread.h"

#include <wx/defs.h>
#include <wx/app.h>
#include <wx/toplevel.h>
#include <wx/image.h>
#include <wx/event.h>

#include <wx/log.h>
#include <vld.h>

////////////////////////////////////////////////////////////////////////
// Class LogWindow for debugging messages
//
DECLARE_EVENT_TYPE(EVENT_PRINT, -1);
DEFINE_EVENT_TYPE(EVENT_PRINT);

//**********************************************************************

class LogWindow : public wxEvtHandler, public wxLogWindow, public util::streamview {
public:
	LogWindow (const wxString& title) : wxLogWindow(0, title, true)
		{ util::get_dbg_stream().add_view(this); }
	~LogWindow (void) { util::get_dbg_stream().remove_view(this); }

protected:
	bool OnFrameClose (wxFrame* frame) {
		frame->Destroy();
		return true;
	}

private:
	DECLARE_EVENT_TABLE();

	void onDisplay (wxCommandEvent& event)
		{ this->DoLogString(event.GetString().c_str(), 0); }

	virtual void display (const char* buffer, size_t len) {
		if (buffer[len - 1] == '\n')
			--len;
		wxString output(buffer, wxConvUTF8, len);

		wxCommandEvent event(EVENT_PRINT);
		event.SetString(output);
		this->AddPendingEvent(event);
	}
	virtual void close (void) {}
};

//**********************************************************************

BEGIN_EVENT_TABLE(LogWindow, wxEvtHandler)
	EVT_COMMAND(wxID_ANY, EVENT_PRINT, LogWindow::onDisplay)
END_EVENT_TABLE();

////////////////////////////////////////////////////////////////////////

using namespace talkie;
using namespace net;

class TalkieApp : public wxApp {
public:
	bool OnInit (void) {

#ifndef NDEBUG
		m_logWin = new LogWindow(_("Debug window"));
#endif

		//wxImage::AddHandler(new wxPNGHandler);

		m_service = new IOServiceThread;
		m_directory = new DirectoryClient(*m_service);

		timer::SimpleTimer::Initialize();
		sound::Mixer::Initialize();
		sound::SoundManager::Initialize();

		MainWindow* mainWindow = new MainWindow(_T("Talkie"), *m_service, *m_directory);
		this->SetTopWindow(mainWindow);
		mainWindow->Show();

		m_service->Start();
		return true;
	}
	virtual int OnExit (void) {

		sound::SoundManager::CleanUp();
		sound::Mixer::CleanUp();
		timer::SimpleTimer::CleanUp();

		m_service->Stop();
		delete m_directory;
		delete m_service;

		return this->wxApp::OnExit();
	}

private:
	LogWindow*			m_logWin;
	IOServiceThread*	m_service;
	DirectoryClient*	m_directory;
 };

////////////////////////////////////////////////////////////////////////
 
DECLARE_APP(TalkieApp);
IMPLEMENT_APP(TalkieApp);
