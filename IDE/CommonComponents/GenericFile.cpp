/**
 *	GenericFile.cpp
 *
 *	-- IDE Common Components --
 *
 *	GenericFile component is the component representing
 *	a generic file in the tree view.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#include "GenericFile.h"

#include "StringUtils.h"
#include "Properties.h"
#include "PropertyUtils.h"

#include "TextInputDialog.h"
#include "ComponentRegistry.h"
#include "ComponentFactory.h"
#include "ComponentFunctionCallerSafe.h"
#include "Call.h"
#include "ConsoleHost.h"

#include "ComponentConfigurationDialog.h"
#include "GenericDialogs.h"

#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <wx/filename.h>

#include <fstream>

#include "Icons/delete.xpm"
#include "Icons/properties.xpm"

namespace ide
{
	static const char *fileContainers[] = { "Workspace", "Project", "AspectProject" };

	//-------------------------------------------------------//
	//---- class GenericFile --------------------------------//

	COMPONENT_METADATA(
		GenericFile,
		_("Generic File"),
		_("A component representing a generic file in the workspace"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("alpha")
		);
	IMPLEMENT_COMPONENT_(GenericFile, TreeItemComponent);

	//-----------------------------------------------------------------------

	EXPORTED_SIGNAL(GenericFile, FileRenamed, (const String& oldUri, const String& newUri));

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(GenericFile, table)
	{
		StringVec options;
		options.push_back(_T("GenericFile"));

		table.AddProperty("treeCtrl_icon", new conf::EnumStringProperty(_("Icon"), options, 0,
			_("Icon representation of a generic file in Project Manager's tree-view"))
		);
	}

	//-----------------------------------------------------------------------

	GenericFile::GenericFile(void)
	{
	}

	//-----------------------------------------------------------------------

	GenericFile::~GenericFile(void)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(GenericFile, void, Initialize, (void))
	{
		Call<void (const std::string&, const std::string&)>
			Register(s_classId, "TreeItemComponent", "RegisterChildType");
		BOOST_FOREACH(const char* container, fileContainers)
			Register(container, s_classId);

		extensions.insert(std::make_pair(s_classId, ".*"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(GenericFile, void, CleanUp, (void))
	{
		extensions.erase(s_classId);

		Call<void (const std::string&, const std::string&), SafeCall>
			Unregister(s_classId, "TreeItemComponent", "UnregisterChildType");
		BOOST_FOREACH(const char* container, fileContainers)
			Unregister(container, s_classId);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(GenericFile, bool, Load, (const String& uri))
	{
		SetURI(uri);
		CreateDefaultSymbolicURI();

		StringList tokens;
		util::stringtokenizer(tokens, GetURI(), _T("/"));
		SetName(tokens.back());

		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(GenericFile, bool, Save, (void))
	{
		if (!GetURI().empty())
			return SaveAs(GetURI());
		else
			return false;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(GenericFile, bool, SaveAs, (const String& uri))
	{
		try {
			//-- Check path existance
			std::string path = util::str2std(uri.substr(0, uri.find_last_of('/')));
			if (!path.empty() && !boost::filesystem::exists(path))
				boost::filesystem::create_directory(path);

			//-- Just copy the file
			if (!boost::filesystem::exists(util::str2std(uri)))
			{
				std::ofstream out;
				out.open(util::str2std(uri).c_str());
				out.close();
				return true;
			}
			if (boost::filesystem::equivalent(util::str2std(uri), util::str2std(GetURI())))
				return true;

			std::ifstream in;
			in.open(util::str2std(GetURI()).c_str());

			std::ofstream out;
			out.open(util::str2std(uri).c_str());

			while (!in.eof())
			{
				std::string buffer;
				in >> buffer;
				out << buffer;
			}
			out.close();
			in.close();

			return true;
		}
		catch(...) { return false; }
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(GenericFile, void, DefaultAction, (void))
	{
		Open();
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_FUNCTION(GenericFile, Open, _("/{0}Open"), MT_CTX, "")
	{
		util::ConsoleHost::ExecuteShell(util::str2std(GetURI()));
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(GenericFile, "delete", delete_xpm);
	EXPORTED_CMD_FUNCTION(GenericFile, RemoveCtx, _("/{50}--Remove"), MT_CTX, "delete")
	{
		assert(treeview);
		const Handle& parent = Call<const Handle& (const Handle&)>(this, treeview, "GetParent")(this);
		Call<bool (const Handle&)>(this, treeview, "RemoveComponent")(this);
		Destroy();

		if (parent)
			Call<void (void)>(s_classId, parent, "Save")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_FUNCTION(GenericFile, RenameDialog, _("/{60}Rename--"), MT_CTX, "")
	{
		StringBoolPair result = 
			Call<StringBoolPair (const String&, const String&, const String&)>(this, "Shell", "PromptInput")
				(_("Rename"), _("New name:"), GetName());
		if (result.second)
			Rename(result.first);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(GenericFile, bool, Rename, (const String& newName))
	{
		assert(treeview);

		if (newName.empty() || newName == GetName())
			return false;
		else if (newName.find_first_of(wxFileName::GetForbiddenChars()) != String::npos) {
			gui::displayMessage(0, _("Rename"), _("Given name contains forbidden characters!"));
			return false;
		}

		const String oldUri = GetURI();
		String uri = oldUri;
		size_t index = uri.find_last_of(_T("/"));
		if (index == String::npos)	///< uri doesn't contain path part
			uri = newName;
		else
		{
			if (newName.find(_T('.')) != String::npos)	///< user provided a new extension
				uri = uri.substr(0, index+1) + newName;
			else										///< user didn't provide a new extension (use old one)
				uri = uri.substr(0, index+1) + newName + uri.substr(uri.find_last_of(_T('.')));
		}

		size_t end = uri.find_last_of(_T("/"));
		const String name = (end != String::npos) ? uri.substr(end +1) : uri;
		BOOST_FOREACH(Component* sibling, GetParent()->GetChildren())
			if (Call<bool (const String&, const String&)>(this, sibling, "HasNamingConflict")(name, uri))
			{
				gui::displayMessage(0, _("Rename"),
					_("There is already an item with the same Name/URI in the project!"));
				return false;
			}

		if (boost::filesystem::exists(util::str2std(uri))) {
			gui::displayMessage(0, _("Rename"), _("Can not rename file: File already exists!"));
			return false;
		}

		if (!boost::filesystem::exists(util::str2std(oldUri)))
			gui::displayMessage(0, _("Rename"), _("Source file does not exist. Updating reference to the destination file."));
		else
			boost::filesystem::rename(util::str2std(oldUri), util::str2std(uri));

		SetURI(uri);
		if (HasDefaultSymbolicURI())
			CreateDefaultSymbolicURI();
		SetName(GetLeafURI());
		Call<void (const Handle&)>(this, treeview, "SortChildren")(GetParent());

		sigFileRenamed(this, oldUri, uri);
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(GenericFile, "properties", properties_xpm);
	EXPORTED_CMD_FUNCTION(GenericFile, Properties, _("/{5000}--Properties"), MT_NMAIN, "properties")
	{
		ComponentConfigurationDialog().ConfigureInstance(this);
	}

	//-----------------------------------------------------------------------

	void GenericFile::ComponentAppliedChangedProperties(const conf::PropertyTable& old, const conf::PropertyIdVec& changed)
	{
		TreeItemComponent::ComponentAppliedChangedProperties(old, changed);
	}

	//-----------------------------------------------------------------------
}