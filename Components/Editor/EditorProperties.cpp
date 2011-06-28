/**
 *	EditorProperties.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "EditorProperties.h"
#include "ExtProperties.h"
#include "PropertyUtils.h"

#include "EditorMain/EditorWindow.h"

#include <wx/colour.h>

namespace ide {

////////////////////////////////////////////////////////////////////////

StyleProperty::StyleProperty (
		const String&	label,
		wxColour		fgColor,
		wxColour		bgColor,
		const String&	desc
	) : AggregateProperty(label, desc)
{
	this->AddProperty("fg", new conf::ColorProperty(_("Foreground color"),
		fgColor, _("The style's foreground color")));
	this->AddProperty("bg", new conf::ColorProperty(_("Background color"),
		bgColor, _("The style's background color")));
}

//**********************************************************************

StyleProperty::~StyleProperty (void)
{
}

//**********************************************************************

void StyleProperty::SetValue (const StyleProperty& p)
{
	wxColour fg = conf::get_prop_value<const conf::ColorProperty>(p.GetProperty("fg"));
	wxColour bg = conf::get_prop_value<const conf::ColorProperty>(p.GetProperty("bg"));

	if (fg != wxNullColour)
		conf::set_prop_value<conf::ColorProperty>(this->GetProperty("fg"), fg);
	if (bg != wxNullColour)
		conf::set_prop_value<conf::ColorProperty>(this->GetProperty("bg"), bg);
}

//**********************************************************************

void StyleProperty::Apply (editor::EditorWindow* editor, int styleId) const
{
	wxColour fg = conf::get_prop_value<const conf::ColorProperty>(this->GetProperty("fg"));
	wxColour bg = conf::get_prop_value<const conf::ColorProperty>(this->GetProperty("bg"));

	if (fg != wxNullColour)
		editor->StyleSetForeground(styleId, fg);

	if (bg != wxNullColour)
		editor->StyleSetBackground(styleId, bg);
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
