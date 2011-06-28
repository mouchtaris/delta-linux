/**
 *	DeltaScriptConfig.h
 *
 *	-- IDE Adaptation Manager Component --
 *
 *	DeltaScriptConfig component is a Delta script that manipulates the
 *	IDE for configuration of a profile.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	July 2007
 */
#ifndef IDE_PROFILE_DELTA_SCRIPT_CONFIG_COMPONENT_H_
#define IDE_PROFILE_DELTA_SCRIPT_CONFIG_COMPONENT_H_

#include "Common.h"

#include "Config.h"

namespace ide
{
	class DeltaScriptConfig :
		public Config
	{
		DECLARE_IDE_COMPONENT(DeltaScriptConfig);

	public:
		///--- type definitions

		///--- constructors / destructor
		DeltaScriptConfig(void);
		~DeltaScriptConfig(void);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));

		DECLARE_EXPORTED_FUNCTION_(bool, Run, (void),
			_("Execute configurations"));
		DECLARE_EXPORTED_FUNCTION(void, Open, (void));

		DECLARE_EXPORTED_FUNCTION_(bool, LoadComponent, (const String& classId),
			_("Execute configuration for the specific component"));

	private:
		///--- private API
		const std::string GetSparrowLibfuncFile(void) const;
		const StringList GetSparrowLibraryFunctions(void) const;

		///--- private members
		String vmId;
	};
}

#endif	//IDE_PROFILE_DELTA_SCRIPT_CONFIG_COMPONENT_H_
