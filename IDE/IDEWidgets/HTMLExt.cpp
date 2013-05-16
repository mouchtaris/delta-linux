/**
 *	HTMLExt.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "HTMLExt.h"
#include "Component.h"
#include "DefaultGUIGenerator.h"
#include "CommonGUIUtils.h"
#include "ComponentRegistry.h"

#include <wx/html/htmlwin.h>
#include <wx/html/m_templ.h>
#include <wx/html/forcelnk.h>

#include <wx/textctrl.h>

namespace ide {

////////////////////////////////////////////////////////////////////////

FORCE_LINK_ME(HTMLExt);

////////////////////////////////////////////////////////////////////////
// HTML tags
//
TAG_HANDLER_BEGIN(COMPPROP, "COMPPROP")

TAG_HANDLER_PROC(tag)
{
	tchar compBuff[512];
	int serial = 0;

	tag.ScanParam(wxT("comp"), wxT("%s"), &compBuff);

	if (tag.HasParam(wxT("serial")))
		tag.ScanParam(wxT("serial"), wxT("%d"), &serial);

	const std::string compId = util::str2std(compBuff);
	ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(compId);

	const conf::PropertyTable& table = (serial == 0) ?
		entry.GetPropertyTable() : entry.GetInstance(serial)->GetInstancePropertyTable();

	GUI_SCOPED_FREEZE(m_WParser->GetWindowInterface()->GetHTMLWindow());

	conf::DefaultGUIGenerator dgg;
	dgg.SetReadOnly();
	wxWindow* win = dgg.CreateGUIFromProperties(
			m_WParser->GetWindowInterface()->GetHTMLWindow(), table
		);
	m_WParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(win, 100));
	return false;
}

TAG_HANDLER_END(COMPPROP);

//**********************************************************************

TAGS_MODULE_BEGIN(CompProp)
	TAGS_MODULE_ADD(COMPPROP)
TAGS_MODULE_END(CompProp);

////////////////////////////////////////////////////////////////////////
// HTMLWindowExt class
//
HTMLWindowExt::HTMLWindowExt (wxWindow* parent) :
	gui::HTMLWindow(parent)
{

}

//**********************************************************************

bool HTMLWindowExt::Create (wxWindow* parent)
{
	return this->HTMLWindow::Create(parent);
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
