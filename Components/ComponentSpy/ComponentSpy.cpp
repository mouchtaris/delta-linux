/**
 *	ComponentSpy.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ComponentSpy.h"
#include "ExtProperties.h"
#include "PropertyUtils.h"

#ifndef NO_VLD
#include <vld.h>
#endif

namespace ide {

////////////////////////////////////////////////////////////////////////
// ComponentSpy class
//
COMPONENT_METADATA(
	ComponentSpy,
	_("Introspection"),
	_("Shamelessly exposes and manipulates the component introspection data"),
	_T("Giannis Georgalis <jgeorgal@ics.forth.gr>"),
	_T("0.1a")
);
IMPLEMENT_WX_COMPONENT_(ComponentSpy, ComponentSpyWindow, DockableComponent);

COMPONENT_SET_PROPERTIES_FUNCTION(ComponentSpy, table)
{
	conf::EnumStringProperty* docking = const_cast<conf::EnumStringProperty*>(
		conf::safe_prop_cast<const conf::EnumStringProperty>(
			table.GetProperty("docking")
		)
	);
	if (docking)
		docking->SetOption(6);

	table.AddProperty("active-color",
		new conf::ColorProperty(
			_("Active color"),
			wxColour(0, 0, 160),
			_("The color of active instances")
		)
	);
}

//**********************************************************************

wxWindow* ComponentSpy::GenerateWindow (wxWindow* parent)
{
	this->Create(parent);

	conf::PropertyIdVec changed;
	changed.push_back("active-color");
	this->ComponentAppliedChangedProperties(GetPropertyTable(), changed);

	return this;
}

//**********************************************************************

wxWindow* ComponentSpy::GetWindow (void)
{
	return this;
}

//**********************************************************************

EXPORTED_CMD_STATIC(ComponentSpy, View, _("/{10}View/{500}Component Spy"), MT_MAIN, "")
{
	EnsureVisibility("ComponentSpy");
}

//**********************************************************************

void ComponentSpy::ComponentAppliedChangedProperties (const conf::PropertyTable& old, const conf::PropertyIdVec& changed)
{
	const conf::Property* prop = this->GetProperty("active-color");
	this->SetActiveColor(conf::get_prop_value<conf::ColorProperty>(prop));
	DockableComponent::ComponentAppliedChangedProperties(old, changed);
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
