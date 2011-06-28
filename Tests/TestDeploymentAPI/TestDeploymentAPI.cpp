/**
 *	Talkie.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#define WIN32_LEAN_AND_MEAN // Be lean and mean

#include "MainWindow.h"
#include "RemoteDeploymentAPI.h"

#include <wx/defs.h>
#include <wx/app.h>
#include <wx/toplevel.h>
#include <wx/image.h>
#include <wx/event.h>

#include <wx/log.h>
#include <vld.h>

////////////////////////////////////////////////////////////////////////

class DeploymentAPITester : public wxApp {
public:
	bool OnInit (void)
	{
		//wxImage::AddHandler(new wxPNGHandler);

		MainWindow* mainWindow = new MainWindow(_("Deployment API Tester"));
		this->SetTopWindow(mainWindow);
		mainWindow->Show();

		ext::RemoteDeploymentAPI::Initialize("localhost", 0);	//TODO get the server port and use it here

		return true;
	}

	virtual int OnExit (void)
	{
		ext::RemoteDeploymentAPI::CleanUp();
		return this->wxApp::OnExit();
	}
};

////////////////////////////////////////////////////////////////////////
 
DECLARE_APP(DeploymentAPITester);
IMPLEMENT_APP(DeploymentAPITester);
