/**
 *	main.cpp
 *
 *	-- Delta IDE --
 *
 *	Delta language Integrated Development Environment
 *	Application.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#include "Common.h"
#include "Streams.h"
#include "StringUtils.h"

#include "IDECore.h"
#include "GenericDialogs.h"
#include "UndefineAllWin32Shit.h"

#ifndef NO_VLD
#include <vld.h>
#endif

#include <boost/program_options.hpp>

#include <wx/defs.h>
#include <wx/app.h>
#include <wx/toplevel.h>
#include <wx/image.h>
#include <wx/event.h>
#include <wx/log.h>
#include <wx/frame.h>
#include <wx/cmdline.h>


DECLARE_LOCAL_EVENT_TYPE(EVENT_PRINT, -1);
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

	void onDisplay(wxCommandEvent& event) {
		this->DoLogString(
#if wxCHECK_VERSION(2, 9, 0)
			event.GetString().c_str().AsChar(),
#else
			event.GetString().c_str(),
#endif
			0
		);
	}

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

#if wxCHECK_VERSION(2, 9, 0)
#include <wx/apptrait.h>
class MaintainCLocaleTraits : public wxGUIAppTraits {
public:
	virtual void SetLocale(void) {}
};
#endif

//----------------------------
//-- class IDE

class IDE :
	public wxApp
{
public:
#if wxCHECK_VERSION(2, 9, 0)
	virtual wxAppTraits * CreateTraits(void) { return new MaintainCLocaleTraits; }
#endif
	bool OnInit(void)
	{
#ifndef NDEBUG
		logWindow = new LogWindow(_("Debug window"));
#endif
		std::string configFile, profile, workspace;
		int32 messageRouterServerNegotiationPort;

		const std::string defaultComponents = "DefaultComponents.xdf";
		try {
			//-- Parse command line options
			boost::program_options::options_description desc("USAGE");
			desc.add_options()
				("help,h",		"produce this help message")

				("input,i",
					boost::program_options::value<StdStringVec>(),
					"input workspace")

				("configuration,c",
					boost::program_options::value<std::string>(),
					"component configuration")
				("profile,p",
					boost::program_options::value<std::string>(),
					"startup profile")
				("negotiationport,n",
					boost::program_options::value<unsigned int>(),
					"debug client-supplied negotiation port")
				("messagerouter_negotiationport,m",
					boost::program_options::value<int32>(),
					"message router client-supplied negotiation port")
			;

			boost::program_options::positional_options_description p;
			p.add("input", -1);

			boost::program_options::variables_map vars;
			boost::program_options::store(boost::program_options::basic_command_line_parser<wxChar>(argc, argv).
				  options(desc).positional(p).run(), vars);
			boost::program_options::notify(vars);

			configFile = vars.count("configuration") ?
				vars["configuration"].as<std::string>() : defaultComponents;
			profile = vars.count("profile") ?
				vars["profile"].as<std::string>() : "";
			messageRouterServerNegotiationPort = vars.count("messagerouter_negotiationport") ?
				vars["messagerouter_negotiationport"].as<int32>() : 0;
			workspace = vars.count("input") ? vars["input"].as<StdStringVec>()[0] : "";
		}
		catch(...) {
			gui::displayMessage(0, _("Initialization Exception"), _("Unable to parse command line arguments"));
			return false;
		}

		if (!ide::IDECore::Initialize(configFile, messageRouterServerNegotiationPort)) {
			gui::displayMessage(0, _("Error starting IDE"),
				_("Installation appears to be corrupted. Please reinstall Sparrow."));
			return false;
		}

		try {
			if (!ide::IDECore::Run(profile, workspace)) {
				const String msg = configFile == defaultComponents ? 
					_("Unable to load default component configuration") : 
					_("Invalid component configuration given.");
				gui::displayMessage(0, _("Error starting IDE"), msg);
				ide::IDECore::CleanUp();
				return false;
			}
		}
		catch (std::exception& e) {
			gui::displayMessage(0, _("Run-time Exception"), util::std2str(e.what()));
			ide::IDECore::CleanUp();
		}
		return true;
	}

	virtual int OnExit(void)
	{
		ide::IDECore::CleanUp();
		return wxApp::OnExit();
	}

private:
	LogWindow* logWindow;
};
 
DECLARE_APP(IDE);
IMPLEMENT_APP(IDE);
