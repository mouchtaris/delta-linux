/**
 *	DockableComponent.h
 *
 *	-- IDE Common Components --
 *
 *	Common component of a dockable component that
 *	can be moved around in the IDE.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#ifndef COMMON_DOCKABLE_COMPONENT_H_
#define COMMON_DOCKABLE_COMPONENT_H_

#include "Common.h"
#include "IDECommon.h"
#include "IDEComponent.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"

namespace ide
{
	//----------------------------
	//-- class DockableComponent

	class _IDE_API DockableComponent :
		public IDEComponent
	{
		DECLARE_IDE_COMPONENT(DockableComponent);

	public:
		///--- type definitions

		///--- constructors / destructor
		DockableComponent(void);
		~DockableComponent(void);

		///--- public API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, EnsureVisibility, (const std::string& class_id));

	protected:
		///--- protected API
		void ComponentAppliedChangedProperties(const conf::PropertyTable& old, const conf::PropertyIdVec& changed);
	};
}

#endif	//COMMON_DOCKABLE_COMPONENT_H_
