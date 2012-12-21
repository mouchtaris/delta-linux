/**
 *	BuildSystem.h
 *	IDE Build System Component
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	February 2012
 */
#ifndef IDE_BUILD_SYSTEM_COMPONENT_H_
#define IDE_BUILD_SYSTEM_COMPONENT_H_

#include "Common.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"

namespace ide
{
	//----------------------------
	//-- class BuildSystem

	class BuildSystem :
		public Component
	{
		DECLARE_IDE_COMPONENT(BuildSystem);

	public:
		///--- type definitions
		typedef std::map<Component*, UIntList> ScriptBuildInfo;

		///--- constructors / destructor
		BuildSystem(void);
		~BuildSystem(void);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));

		DECLARE_EXPORTED_STATIC_(uint, GetServerPort, (void),
			_("Get the port for the build server"));

		DECLARE_EXPORTED_STATIC(void, OnResourceWorkCompleted, (const Handle& resource, const String& task, const UIntList& workId));		
		DECLARE_EXPORTED_STATIC(void, OnWorkStarted, (const std::string& invoker, const Handle& root, const String& task));
		DECLARE_EXPORTED_STATIC(void, OnWorkCompleted, (const std::string& invoker, const Handle& root, const String& task));
		DECLARE_EXPORTED_STATIC(void, OnWorkCanceled, (const std::string& invoker, const Handle& root, const String& task));

	private:
		///--- private API
	};
}

#endif	//IDE_BUILD_SYSTEM_COMPONENT_H_
