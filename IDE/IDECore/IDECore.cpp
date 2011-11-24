/**
 *	IDECore.cpp
 *
 *	-- Delta IDE Core --
 *
 *	Core IDE dll exporting communication interface
 *	of the IDE with its plugins/components.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007
 */
#include "IDECore.h"

#include "DynamicMain.h"
#include "DynamicLoader.h"

#include "ComponentLoader.h"
#include "ComponentHolder.h"
#include "ComponentSignalRegistry.h"
#include "ComponentRegistry.h"
#include "ComponentFactory.h"

#include "IOServiceThread.h"
#include "MessageRouter.h"
#include "MessageRouterServer.h"

#include "UndoManager.h"
#include "BitmapRegistry.h"

#include "ConsoleHost.h"
#include "FileChangeWatcher.h"
#include "PathManager.h"
#include "SimpleTimer.h"
#include "DelayedCaller.h"
#include "PropertiesFactory.h"
#include "StringUtils.h"

#include "Call.h"
#include "ComponentInitializer.h"
#include "ComponentDataTypeConverter.h"

#include "UndefineAllWin32Shit.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <wx/app.h>
#include <wx/frame.h>

#ifndef NDEBUG
#ifndef NO_VLD
#include <vld.h>
#endif
#endif

namespace ide
{
	//-------------------------------------------------------//
	//---- class IDECore ------------------------------------//

	std::string IDECore::installationDirectory;

	//-----------------------------------------------------------------------

	bool IDECore::Initialize(const std::string& componentConfiguration, int32 messageRouterServerNegotiationPort)
	{
		installationDirectory = util::str2std(util::normalizepath(wxFileName(wxTheApp->argv[0]).GetPath()));
		if (!installationDirectory.empty() && installationDirectory[installationDirectory.size() - 1] != '/')
			installationDirectory += '/';

		fs::PathManager::Initialize();
		fs::PathManager::Instance().AppendLocalDirectory(_T("Sparrow"));

		net::IOServiceThreadSingleton::Initialize();

		util::ConsoleHost::Initialize();
		util::FileChangeWatcher::Initialize();
		timer::SimpleTimer::Initialize();
		timer::DelayedCaller::Initialize();
		conf::PropertiesFactory::Initialize();

		ComponentDataTypeConverter::Initialize();
		MessageRouter::Initialize();
		MessageRouterServer::Initialize(messageRouterServerNegotiationPort);
		UndoManager::Initialize();
		BitmapRegistry::Initialize();

		ComponentHolder::Initialize();
		ComponentSignalRegistry::Initialize();
		ComponentRegistry::Initialize();
		ComponentFactory::Initialize();
		ComponentLoader::Initialize(componentConfiguration);

		return true;
	}

	//-----------------------------------------------------------------------

	void IDECore::CleanUp(void)
	{
		ComponentInitializer::ClearPendingComponents();

		ComponentLoader::CleanUp();
		ComponentFactory::CleanUp();
		ComponentRegistry::CleanUp();
		ComponentSignalRegistry::CleanUp();
		ComponentHolder::CleanUp();

		BitmapRegistry::CleanUp();
		UndoManager::CleanUp();
		MessageRouterServer::CleanUp();
		MessageRouter::CleanUp();
		ComponentDataTypeConverter::CleanUp();

		conf::PropertiesFactory::CleanUp();
		timer::DelayedCaller::CleanUp();
		timer::SimpleTimer::CleanUp();
		util::FileChangeWatcher::CleanUp();
		util::ConsoleHost::CleanUp();

		net::IOServiceThreadSingleton::CleanUp();

		fs::PathManager::CleanUp();
	}

	//-----------------------------------------------------------------------

	bool IDECore::Run(const std::string& profile, const std::string& workspace)
	{
		//-- Create main window
		Component* shell = ComponentFactory::Instance().CreateComponent("Shell");
		if (!shell)
			return false;

		wxTheApp->SetTopWindow(shell->GenerateWindow(0));
		shell->GetWindow()->Show();
		Call<void (const String&, const String&)>("IDECore", shell, "Construct")
			(util::std2str(profile), util::std2str(workspace));

		net::IOServiceThreadSingleton::Instance().Start();
		return true;
	}

	//-----------------------------------------------------------------------

	const std::string IDECore::GetInstallationDir(void) { return installationDirectory; }

	//-----------------------------------------------------------------------
}
