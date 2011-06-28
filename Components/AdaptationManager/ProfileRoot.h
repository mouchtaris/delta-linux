/**
 *	ProfileRoot.h
 *
 *	-- IDE Adaptation Manager Component --
 *
 *	ProfileRoot component is the root component of
 *	the Adaptation Manager's tree view.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
 */
#ifndef IDE_PROFILE_ROOT_COMPONENT_H_
#define IDE_PROFILE_ROOT_COMPONENT_H_

#include "Common.h"

#include "TreeItemComponent.h"

namespace ide
{
	//----------------------------
	//-- class ProfileRoot

	class ProfileRoot :
		public TreeItemComponent
	{
		DECLARE_IDE_COMPONENT(ProfileRoot);

	public:
		///--- type definitions

		///--- constructors / destructor
		ProfileRoot(void);
		~ProfileRoot(void);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_FUNCTION(void, EditDMSL, (void));

		DECLARE_EXPORTED_FUNCTION(void, ScanProfiles, (void));
		DECLARE_EXPORTED_FUNCTION(void, ClearProfiles, (void));

		DECLARE_EXPORTED_FUNCTION(void, AddProfile, (void));
		DECLARE_EXPORTED_FUNCTION(void, Refresh, (void));
		DECLARE_EXPORTED_FUNCTION(void, Properties, (void));

	private:
		///--- private API

		///--- private members
	};
}

#endif IDE_PROFILE_ROOT_COMPONENT_H_
