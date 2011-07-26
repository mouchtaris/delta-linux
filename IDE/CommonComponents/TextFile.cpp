/**
 *	TextFile.cpp
 *
 *	-- IDE Common Components --
 *
 *	TextFile component is the component representing
 *	a text file in the tree view.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	July 2009
 */
#include "TextFile.h"

#include "StringUtils.h"
#include "Properties.h"
#include "PropertyUtils.h"

#include "ComponentFunctionCallerSafe.h"
#include "Call.h"
#include "GenericDialogs.h"

#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <wx/filename.h>

#include <fstream>

namespace ide
{
	static const char *textfileContainers[] = { "Workspace", "Project" };

	//-------------------------------------------------------//
	//---- class TextFile -----------------------------------//

	COMPONENT_METADATA(
		TextFile,
		_("Text File"),
		_("A component representing a text file in the workspace"),
		_T("Ioannis Lilis <lilis@ics.forth.gr>"),
		_T("alpha")
		);
	IMPLEMENT_COMPONENT_(TextFile, GenericFile);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(TextFile, table)
	{
		StringVec options;
		options.push_back(_T("TextFile"));

		table.AddProperty("treeCtrl_icon", new conf::EnumStringProperty(_("Icon"), options, 0,
			_("Icon representation of a text file in Project Manager's tree-view"))
		);
	}

	//-----------------------------------------------------------------------

	TextFile::TextFile(void)
	{
	}

	//-----------------------------------------------------------------------

	TextFile::~TextFile(void)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(TextFile, void, Initialize, (void))
	{
		Call<void (const std::string&, const std::string&)>
			Register(s_classId, "TreeItemComponent", "RegisterChildType");
		BOOST_FOREACH(const char* container, textfileContainers)
			Register(container, s_classId);

		extensions.insert(std::make_pair(s_classId, ".*"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(TextFile, void, CleanUp, (void))
	{
		extensions.erase(s_classId);

		Call<void (const std::string&, const std::string&), SafeCall>
			Unregister(s_classId, "TreeItemComponent", "UnregisterChildType");
		BOOST_FOREACH(const char* container, textfileContainers)
			Unregister(container, s_classId);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TextFile, void, DefaultAction, (void))
	{
		Open();
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_FUNCTION(TextFile, Open, _("/{0}Open"), MT_CTX, "")
	{
		Call<void (const std::string&)>(this, "DockableComponent", "EnsureVisibility")("EditorManager");
		if (const Handle editor = Call<const Handle (const String&)>(this, "EditorManager", "OpenDocument")(GetURI()))
			Call<void (const Handle&)>(this, editor, "SetAssociatedComponent")(this);
		else
			gui::displayMessage(0, _("Error"), _("Unable to open file '") + GetURI() + _("'\n"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_FUNCTION(TextFile, View, _("/{5}View--"), MT_CTX, "")
	{
		Call<void (const std::string&)>(this, "DockableComponent", "EnsureVisibility")("EditorManager");
		Call<void (const String&)>(this, "EditorManager", "ViewDocument")(GetURI());
	}

	//-----------------------------------------------------------------------

	void TextFile::ComponentDestroyed(void)
	{
		Call<void (const String&), SafeCall>(this, "EditorManager", "CloseDocument")
			(GetURI());
	}

	//-----------------------------------------------------------------------
}
