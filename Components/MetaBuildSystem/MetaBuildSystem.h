/**
 *	MetaBuildSystem.h
 *	IDE Meta Build System Component
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	February 2012
 */
#ifndef IDE_META_BUILD_SYSTEM_COMPONENT_H_
#define IDE_META_BUILD_SYSTEM_COMPONENT_H_

#include "Common.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"

namespace ide
{
	//----------------------------
	//-- class MetaBuildSystem

	class MetaBuildSystem :
		public Component
	{
		DECLARE_IDE_COMPONENT(MetaBuildSystem);

	public:
		///--- type definitions
		typedef std::map<Component*, UIntList> ScriptBuildInfo;

		///--- constructors / destructor
		MetaBuildSystem(void);
		~MetaBuildSystem(void);

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

#endif	//IDE_META_BUILD_SYSTEM_COMPONENT_H_
