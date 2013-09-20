/**
 *	MetaBuildSystem.h
 *	IDE Meta Build System Component
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	February 2012
 */
#include "MetaBuildSystem.h"
#include "MetaBuildServer.h"

#ifndef NO_VLD
#include <vld.h>
#endif

namespace ide
{
	//-------------------------------------------------------//
	//---- class MetaBuildSystem -------------------------------//

	COMPONENT_METADATA(
		MetaBuildSystem,
		_("Meta Build System"),
		_("Used to build stage sources and handle stage results and aspect transformation results"),
		_T("Yannis Lilis <lilis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_IDE_COMPONENT(MetaBuildSystem);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(MetaBuildSystem, table)
	{
	}

	//-----------------------------------------------------------------------

	MetaBuildSystem::MetaBuildSystem(void)
	{
	}

	//-----------------------------------------------------------------------

	MetaBuildSystem::~MetaBuildSystem(void)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(MetaBuildSystem, void, Initialize, (void))
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(MetaBuildSystem, void, CleanUp, (void))
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(MetaBuildSystem, uint, GetServerPort, (void))
	{
		return MetaBuildServer::GetPort();
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(MetaBuildSystem, void, OnResourceWorkCompleted, (const Handle& resource, const String& task, const UIntList& workId), "ResourceWorkCompleted")
	{
		const std::string classId = resource.GetClassId();
		if (task == _T("Build") && (classId == "StageSource" || classId == "StageResult" || classId == "AspectResult"))
			MetaBuildServer::OnCompileFinished(resource.Resolve(), workId);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(MetaBuildSystem, void, OnWorkStarted, (const std::string& invoker, const Handle& root, const String& task), "WorkStarted")
	{
		if (task == _T("Build"))
			MetaBuildServer::Initialise(BUILD_SERVER_PORT);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(MetaBuildSystem, void, OnWorkCompleted, (const std::string& invoker, const Handle& root, const String& task), "WorkCompleted")
	{
		if (task == _T("Build"))
			MetaBuildServer::CleanUp();
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(MetaBuildSystem, void, OnWorkCanceled, (const std::string& invoker, const Handle& root, const String& task), "WorkCanceled")
	{
		if (task == _T("Build"))
			MetaBuildServer::CleanUp();
	}

	//-----------------------------------------------------------------------

}
