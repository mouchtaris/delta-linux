/**
 *	MainWindow.cpp
 *
 *	-- Talkie Centralized Directory stand-alone Server --
 *
 *	Main window of centralized directory server
 *	for talkie network.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#include "MainWindow.h"
#include "StringUtils.h"

#include "IOServiceThread.h"
#include "DirectoryServer.h"
#include "TaskBarIcon.h"

#include "EntriesWindow.h"

#include <wx/textctrl.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/log.h>

#include <boost/lexical_cast.hpp>

namespace talkie
{
	enum ButtonCommandType {
		BUTTON_START, BUTTON_ENTRIES,
	};

	BEGIN_EVENT_TABLE(MainWindow, wxFrame)
		EVT_BUTTON(BUTTON_START, MainWindow::onStartPressed)
		EVT_BUTTON(BUTTON_ENTRIES, MainWindow::onEntriesPressed)
		EVT_TEXT_ENTER(wxID_ANY, MainWindow::onEnter)

		EVT_CLOSE(MainWindow::onClose)
	END_EVENT_TABLE();

	//-------------------------------------------------------//
	//---- class MainWindow ---------------------------------//

	MainWindow::MainWindow(const String& title)
		: wxFrame(0, wxID_ANY, title, wxDefaultPosition, wxSize(200, 220),
		wxMINIMIZE_BOX | wxCLOSE_BOX | /*wxRESIZE_BORDER | */wxCAPTION | wxSYSTEM_MENU | 0 /*wxSTAY_ON_TOP*/),
		io_service(0), server(0), running(false)
	{
		//-- Initialize GUI stuff
		SetIcon(wxIcon(_T("directory_icon")));
		SetBackgroundColour(*wxWHITE);
	
		wxFont font(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

		imgLogo = new wxStaticBitmap(this, wxID_ANY, wxBitmap(_T("logo_text")));

		wxStaticText* lblPort = new wxStaticText(this, wxID_ANY, _("port number:"));
		txtPort = new wxTextCtrl(this, wxID_ANY, _("2123"), wxDefaultPosition, wxDefaultSize,
			wxTE_PROCESS_ENTER);
		btnStart = new wxButton(this, BUTTON_START, _("Start"), wxDefaultPosition, wxDefaultSize,
			wxNO_BORDER);
		btnEntries = new wxButton(this, BUTTON_ENTRIES, _("Show Entries"), wxDefaultPosition, wxDefaultSize,
			wxNO_BORDER);
		lblMessage = new wxStaticText(this, wxID_ANY, _("Directory Server is not running."));
		lblMessage->SetFont(font);

		wxFlexGridSizer* infoSizer = new wxFlexGridSizer(1, 2, 5, 5);
		infoSizer->Add(lblPort, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
		infoSizer->Add(txtPort, 1, wxEXPAND);

		wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
		buttonSizer->AddSpacer(5);
		buttonSizer->Add(btnStart, 0);
		buttonSizer->AddSpacer(5);
		buttonSizer->Add(btnEntries, 0);

		wxBoxSizer* vsizer = new wxBoxSizer(wxVERTICAL);
		vsizer->AddSpacer(5);
		vsizer->Add(imgLogo, 0, wxALIGN_CENTER_HORIZONTAL);
		vsizer->AddSpacer(20);
		vsizer->Add(infoSizer, 0, wxALIGN_CENTER_HORIZONTAL);
		vsizer->AddSpacer(10);
		vsizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL);
		vsizer->AddSpacer(15);
		vsizer->Add(lblMessage, 0, wxALIGN_CENTER_HORIZONTAL);

		SetSizer(vsizer);

		//-- Initialize IO Service thread
		io_service = new net::IOServiceThread;
		io_service->Start();

		//-- Connect Signals
		sigStart.connect(boost::bind(&MainWindow::StartServer, this));
		sigStop.connect(boost::bind(&MainWindow::StopServer, this));
		sigClose.connect(boost::bind(&MainWindow::Hide, this));

		//-- Create tray icon
		taskBar = new TaskBarIcon(this);
		taskBar->SetIcon(wxIcon(_T("directory_icon")));

		//-- Automatically start server on default port
		StartServer();
	}

	//-----------------------------------------------------------------------

	MainWindow::~MainWindow(void)
	{
		io_service->Interrupt();
		delete io_service;
		delete taskBar;
	}

	//-----------------------------------------------------------------------

	uint16 MainWindow::GetPort(void) const
	{
		String value = util::strip(txtPort->GetValue(), _T(" "));
		return (!value.empty()) ? boost::lexical_cast<uint16>(value) : 0;
	}

	//-----------------------------------------------------------------------

	void MainWindow::SetInfoMessage(const String& message)
	{
		lblMessage->SetForegroundColour(*wxBLUE);
		lblMessage->SetLabel(message);
		Layout();
	}

	//-----------------------------------------------------------------------

	void MainWindow::SetErrorMessage(const String& message)
	{
		lblMessage->SetForegroundColour(*wxRED);
		lblMessage->SetLabel(message);
		Layout();
	}

	//-----------------------------------------------------------------------

	void MainWindow::onEnter(wxCommandEvent& event)
	{
		onStartPressed(event);
	}

	//-----------------------------------------------------------------------

	void MainWindow::onStartPressed(wxCommandEvent& event)
	{
		if (running)
			sigStop();
		else
			sigStart();
	}

	//-----------------------------------------------------------------------

	void MainWindow::onEntriesPressed(wxCommandEvent& event)
	{
		EntriesWindow* dialog = new EntriesWindow(_("Directory Entries"));
		dialog->ShowModal();
	}

	//-----------------------------------------------------------------------

	void MainWindow::onClose(wxCloseEvent& event)
	{
		sigClose();
	}

	//-----------------------------------------------------------------------

	void MainWindow::StartServer(void)
	{
		if (!GetPort())
		{
			SetErrorMessage(_("Please type a port number"));
			txtPort->SetFocus();
			return;
		}

		SetInfoMessage(_("Initializing Directory Server..."));
		btnStart->SetLabel(_("Stop"));
		txtPort->SetEditable(false);

		server = new DirectoryServer(*io_service, GetPort());
		server->Accept();

		SetInfoMessage(_("Directory Server is running!"));
		running=true;
	}

	//-----------------------------------------------------------------------

	void MainWindow::StopServer(void)
	{
		//-- Destroy running server
		server->Close();
		delete server;
		server=0;

		//-- GUI update
		running=false;
		txtPort->SetEditable(true);
		btnStart->SetLabel(_("Start"));
		SetErrorMessage(_("Directory Server stopped!"));
	}

	//-----------------------------------------------------------------------
}
