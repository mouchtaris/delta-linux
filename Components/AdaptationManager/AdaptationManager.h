/**
 *	AdaptationManager.h
 *
 *	-- IDE Adaptation Manager Component --
 *
 *	Profile manager component is a visual component
 *	that allows selection / manipulation and organization
 *	of IDE configuration profiles.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007
 */
#ifndef IDE_ADAPTATION_MANAGER_COMPONENT_H_
#define IDE_ADAPTATION_MANAGER_COMPONENT_H_

#include "Common.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"
#include "ComponentUserCommand.h"

#include "TreeCtrlComponent.h"

#include <wx/panel.h>

///////////////////////////////////////////////////////////
// DEFINE DLL EXPORT //////////////////////////////////////

#ifdef ADAPTATIONMANAGER_EXPORTS
#	define _ADAPTATION_MANAGER_API		UTIL_EXPORT
#	define _ADAPTATION_MANAGER_C_API	UTIL_C_EXPORT
#else
#	define _ADAPTATION_MANAGER_API		UTIL_IMPORT
#	define _ADAPTATION_MANAGER_C_API	UTIL_C_IMPORT
#endif

#pragma warning(disable: 4251)

class wxBoxSizer;

namespace ide
{
	class AdaptationManager :
		public TreeCtrlComponent
	{
		DECLARE_IDE_COMPONENT(AdaptationManager);

	public:
		///--- type definitions

		///--- constructors / destructor
		AdaptationManager(void);
		~AdaptationManager(void);

		///--- public overridden API
		virtual wxWindow* GenerateWindow(wxWindow* parent);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_FUNCTION_(const StringList, GetProfiles, (void),
			_("Returns list of names of the available profiles"));
		DECLARE_EXPORTED_FUNCTION_(const Handle, GetProfile, (const String& name),
			_("Returns a handle to the requested profile"));
		DECLARE_EXPORTED_MEMBER_(bool, ApplyProfile, (const String& name),
			_("Apply the requested profile to the environment"));

		DECLARE_EXPORTED_STATIC_(void, View, (void),
			_("Show the Adaptation Manager"));

	private:
		friend struct ProfileLoader;

		///--- event table
		//DECLARE_EVENT_TABLE();

		///--- private API
		///--- private members
	};
}

#endif	//IDE_ADAPTATION_MANAGER_COMPONENT_H_
