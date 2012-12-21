/**
 *	BuildSystem.h
 *	IDE Build System Component
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	February 2012
 */
#include "BuildSystem.h"
#include "BuildServer.h"

#ifndef NO_VLD
#include <vld.h>
#endif

namespace ide
{
	//-------------------------------------------------------//
	//---- class BuildSystem -------------------------------//

	COMPONENT_METADATA(
		BuildSystem,
		_("Build System"),
		_("Used to build projects and scripts"),
		_T("Yannis Lilis <lilis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_IDE_COMPONENT(BuildSystem);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(BuildSystem, table)
	{
	}

	//-----------------------------------------------------------------------

	BuildSystem::BuildSystem(void)
	{
	}

	//-----------------------------------------------------------------------

	BuildSystem::~BuildSystem(void)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(BuildSystem, void, Initialize, (void))
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(BuildSystem, void, CleanUp, (void))
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(BuildSystem, uint, GetServerPort, (void))
	{
		return BuildServer::GetPort();
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(BuildSystem, void, OnResourceWorkCompleted, (const Handle& resource, const String& task, const UIntList& workId), "ResourceWorkCompleted")
	{
		if (resource.GetClassId() == "StageSource")
			BuildServer::OnCompileFinished(resource.Resolve(), workId);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(BuildSystem, void, OnWorkStarted, (const std::string& invoker, const Handle& root, const String& task), "WorkStarted")
	{
		if (task == _T("Build"))
			BuildServer::Initialise(BUILD_SERVER_PORT);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(BuildSystem, void, OnWorkCompleted, (const std::string& invoker, const Handle& root, const String& task), "WorkCompleted")
	{
		if (task == _T("Build"))
			BuildServer::CleanUp();
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(BuildSystem, void, OnWorkCanceled, (const std::string& invoker, const Handle& root, const String& task), "WorkCanceled")
	{
		if (task == _T("Build"))
			BuildServer::CleanUp();
	}

	//-----------------------------------------------------------------------

}
