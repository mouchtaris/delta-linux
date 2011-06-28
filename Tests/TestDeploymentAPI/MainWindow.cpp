/**
 *	MainWindow.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "MainWindow.h"
#include "RemoteDeploymentAPI.h"
#include "GenericDialogs.h"

#include <wx/filedlg.h>

#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/statusbr.h>
#include <wx/button.h>
#include <wx/textdlg.h>
#include <wx/accel.h>

using namespace ext;

////////////////////////////////////////////////////////////////////////

MainWindow::OperationEntry MainWindow::s_operations[NUM_OPERATIONS] = {
	{ &MainWindow::opOpenWorkspace, _("Open workspace") },
	{ &MainWindow::opCloseWorkspace, _("Close workspace") },
	{ &MainWindow::opNewWorkspace, _("New workspace") },
	{ &MainWindow::opRenameWorkspace, _("Rename workspace") },
	{ &MainWindow::opAddProject, _("Add project") },
	{ &MainWindow::opRemoveProject, _("Remove project") },
	{ &MainWindow::opNewProject, _("New project") },
	{ &MainWindow::opRenameProject, _("Rename project") },
	{ &MainWindow::opAddFile, _("Add file") },
	{ &MainWindow::opRemoveFile, _("Remove file") },
	{ &MainWindow::opNewFile, _("New file") },
	{ &MainWindow::opRenameFile, _("Rename file") },
	{ &MainWindow::opLoadProfile, _("Load profile") },
};

////////////////////////////////////////////////////////////////////////
// MainWindow class
//
BEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_BUTTON(wxID_ANY, MainWindow::onOpButtonPressed)
	EVT_CLOSE(MainWindow::onClose)
END_EVENT_TABLE();

////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow (const String& title) :
	wxFrame(0, wxID_ANY, title, wxDefaultPosition, wxSize(260, 390),
		wxMINIMIZE_BOX | wxCLOSE_BOX | wxRESIZE_BORDER | wxCAPTION | wxSYSTEM_MENU |
			0 /*wxSTAY_ON_TOP*/)
{
	////////////////////////////////////////////////////////////////////
	// Initialize basic stuff
	//
	this->SetIcon(wxIcon(_T("app_icon")));
	this->SetMinSize(wxSize(250, 300));

	////////////////////////////////////////////////////////////////////
	// Initialize gui elements
	//
	wxStatusBar* statusBar = new wxStatusBar(this, wxID_ANY, wxST_SIZEGRIP);
	this->SetStatusBar(statusBar);

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	for (uint i = 0; i < NUM_OPERATIONS; ++i) {
		m_opButtons[i] = new wxButton(this, i, s_operations[i].label,
			wxDefaultPosition, wxDefaultSize);
		sizer->Add(m_opButtons[i], 1, wxEXPAND);
	}
	this->SetSizer(sizer);
}

//**********************************************************************

MainWindow::~MainWindow (void)
{
}

//**********************************************************************

void MainWindow::onOpButtonPressed (wxCommandEvent& event)
{
	uint opId = (uint) event.GetId();
	if (opId < NUM_OPERATIONS) {
		try {
			(this->*s_operations[opId].operation)();
		}
		catch (const std::exception& e) {
			gui::displayMessage(this, _("Error while calling function"),
				_("Could not perform operation '") + s_operations[opId].label + wxT("': ") + util::std2str(e.what()));
		}
	}
}

//**********************************************************************

void MainWindow::onClose (wxCloseEvent& event)
{
	this->Destroy();
}

//**********************************************************************
// Operations
//
void MainWindow::opOpenWorkspace (void)
{
	wxFileDialog fsd(this, _("Select workspace"), _T(""), wxEmptyString,
		_T("*.wsp"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (fsd.ShowModal() == wxID_OK)
		RemoteDeploymentAPI::Call("ProjectManager", "OpenWorkspace", ArgumentList(fsd.GetPath()));
}

//**********************************************************************

void MainWindow::opCloseWorkspace (void)
{
	RemoteDeploymentAPI::Call("ProjectManager", "CloseWorkspace");
}

//**********************************************************************

void MainWindow::opNewWorkspace (void)
{
	const String name = gui::getInput(this, _("New workspace..."), _("Give the workspace name"));
	const String folder = gui::getInput(this, _("New workspace..."), _("Give the workspace folder"));
	if (!name.empty() && !folder.empty())
		RemoteDeploymentAPI::Call("ProjectManager", "NewWorkspace", ArgumentList(name, folder));
}

//**********************************************************************

void MainWindow::opRenameWorkspace (void)
{
	gui::displayMessage(this, _("General protection fault"), _("Error at position 0xDEADBEEF"));
}

//**********************************************************************

void MainWindow::opAddProject (void)
{
	gui::displayMessage(this, _("General protection fault"), _("Error at position 0xDEADBEEF"));
}

//**********************************************************************

void MainWindow::opRemoveProject (void)
{
	gui::displayMessage(this, _("General protection fault"), _("Error at position 0xDEADBEEF"));
}

//**********************************************************************

void MainWindow::opNewProject (void)
{
	gui::displayMessage(this, _("General protection fault"), _("Error at position 0xDEADBEEF"));
}

//**********************************************************************

void MainWindow::opRenameProject (void)
{
	gui::displayMessage(this, _("General protection fault"), _("Error at position 0xDEADBEEF"));	
}

//**********************************************************************

void MainWindow::opAddFile (void)
{
	gui::displayMessage(this, _("General protection fault"), _("Error at position 0xDEADBEEF"));
}

//**********************************************************************

void MainWindow::opRemoveFile (void)
{
	gui::displayMessage(this, _("General protection fault"), _("Error at position 0xDEADBEEF"));
}

//**********************************************************************

void MainWindow::opNewFile (void)
{
	gui::displayMessage(this, _("General protection fault"), _("Error at position 0xDEADBEEF"));
}

//**********************************************************************

void MainWindow::opRenameFile (void)
{
	gui::displayMessage(this, _("General protection fault"), _("Error at position 0xDEADBEEF"));
}

//**********************************************************************

void MainWindow::opLoadProfile (void)
{
	const String name = gui::getInput(this, _("Load profile..."),
		_("Give the name of the profile you want to load"));
	if (!name.empty())
		RemoteDeploymentAPI::Call("AdaptationManager", "ApplyProfile", ArgumentList(name));
}

////////////////////////////////////////////////////////////////////////
