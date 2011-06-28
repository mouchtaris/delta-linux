/**
 *	DockableComponent.cpp
 *
 *	-- IDE Common Components --
 *
 *	Common component of a dockable component that
 *	can be moved around in the IDE.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#include "DockableComponent.h"
#include "Call.h"
#include "ComponentRegistry.h"

#include "PropertyUtils.h"

namespace ide
{
	//-------------------------------------------------------//
	//---- class DockableComponent --------------------------//

	COMPONENT_METADATA(
		DockableComponent,
		_("Dockable Component"),
		_("Base class component for dockable GUI components"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_COMPONENT_(DockableComponent, IDEComponent);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(DockableComponent, table)
	{
		StringVec dockingOptions;
		dockingOptions.push_back(_T("left top"));
		dockingOptions.push_back(_T("left bottom"));
		dockingOptions.push_back(_T("right top"));
		dockingOptions.push_back(_T("right bottom"));
		dockingOptions.push_back(_T("bottom left"));
		dockingOptions.push_back(_T("bottom right"));
		dockingOptions.push_back(_T("center"));

		table.AddProperty("docking", new conf::EnumStringProperty(_("Dock area"), dockingOptions, 0,
			_("Docking area of the component"), _("General"))
		);
	}

	//-----------------------------------------------------------------------

	DockableComponent::DockableComponent(void)
	{
	}

	//-----------------------------------------------------------------------

	DockableComponent::~DockableComponent(void)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DockableComponent, void, Initialize, (void))
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DockableComponent, void, EnsureVisibility, (const std::string& class_id))
	{
		Component* comp = ComponentRegistry::Instance().GetFocusedInstance(class_id);
		if (!comp) {
			const Handle handle = Call<Handle (const std::string& id, int direction)>
				(s_classId, "Shell", "AddComponent")(class_id, -1);
			comp = handle.Resolve();
		}
		if (comp) {
			comp->Focus();
			Call<bool (const Handle&)>(s_classId, "Shell", "FocusComponent")(comp);
		}
	}

	//-----------------------------------------------------------------------

	void DockableComponent::ComponentAppliedChangedProperties(const conf::PropertyTable& old, const conf::PropertyIdVec& changed)
	{
		if (std::find(changed.begin(), changed.end(), "docking") != changed.end()) {
			const String& choice = conf::get_prop_value<conf::EnumStringProperty>(GetProperty("docking"));
			if (choice == _("left"))
				Call<bool (const Handle&, uint)>(this, "Shell", "MoveComponent")(this, 0);
			if (choice == _("right"))
				Call<bool (const Handle&, uint)>(this, "Shell", "MoveComponent")(this, 1);
			if (choice == _("down"))
				Call<bool (const Handle&, uint)>(this, "Shell", "MoveComponent")(this, 2);
			if (choice == _("center"))
				Call<bool (const Handle&, uint)>(this, "Shell", "MoveComponent")(this, 3);
		}
		IDEComponent::ComponentAppliedChangedProperties(old, changed);
	}

	//-----------------------------------------------------------------------
}
