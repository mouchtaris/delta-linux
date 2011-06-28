/**
 *	ProfileConfig.h
 *
 *	-- IDE Adaptation Manager Component --
 *
 *	Profile config component is a onfiguration resource
 *	for IDE's Adaptation Manager.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
 */
#ifndef IDE_PROFILE_CONFIG_COMPONENT_H_
#define IDE_PROFILE_CONFIG_COMPONENT_H_

#include "Common.h"

#include "Config.h"

namespace ide
{
	class ProfileConfig :
		public Config
	{
		DECLARE_IDE_COMPONENT(ProfileConfig);

	public:
		///--- type definitions

		///--- constructors / destructor
		ProfileConfig(void);
		~ProfileConfig(void);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));

		DECLARE_EXPORTED_FUNCTION_(bool, Run, (void),
			_("Execute configurations"));

	private:
		///--- private API
		void ApplyConfig(Component* comp);

		///--- private members
	};
}

#endif	//IDE_PROFILE_CONFIG_COMPONENT_H_
