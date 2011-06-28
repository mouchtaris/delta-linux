/**
 *	DeltaComponentDirectory.cpp
 *
 *	-- IDE Delta Component Directory --
 *
 *	Delta Component Directory component is a visual component
 *	that allows browsing and management of available Delta
 *	script components, a utility for Sparrow extension authors.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007
 */
#include "DeltaComponentDirectory.h"

#include "xml.h"
#include "StringUtils.h"
#include "PropertyUtils.h"
#include "ImageList.h"
#include "IDEDialogs.h"
#include "BitmapRegistry.h"

#include "Call.h"
#include "ComponentFunctionCallerSafe.h"
#include "ComponentFactory.h"
#include "ComponentRegistry.h"
#include "ComponentLoader.h"
#include "UndefineAllWin32Shit.h"

#include <wx/listctrl.h>
#include <wx/toolbar.h>
#include <wx/sizer.h>

#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#ifndef NO_VLD
#include <vld.h>
#endif

#include "Icons/loaded.xpm"
#include "Icons/nloaded.xpm"
#include "Icons/add.xpm"
#include "Icons/remove.xpm"
#include "Icons/load.xpm"
#include "Icons/unload.xpm"

#define MAX_LIBRARY_FUNC_DESCRIPTION	1024

//-------------------------------------------------------//
//---- free Init/Clean functions  -----------------------//

_DELTA_COMPONENT_DIRECTORY_C_API void Initialize(void)
{
	ide::BitmapRegistry::Instance().Insert(_T("loaded"), loaded_xpm);
	ide::BitmapRegistry::Instance().Insert(_T("nloaded"), nloaded_xpm);

	ide::BitmapRegistry::Instance().Insert(_T("add"), add_xpm);
	ide::BitmapRegistry::Instance().Insert(_T("remove"), remove_xpm);
	ide::BitmapRegistry::Instance().Insert(_T("load"), load_xpm);
	ide::BitmapRegistry::Instance().Insert(_T("unload"), unload_xpm);
}

//-----------------------------------------------------------------------

_DELTA_COMPONENT_DIRECTORY_C_API void CleanUp(void)
{
	ide::BitmapRegistry::Instance().Remove(_T("loaded"));
	ide::BitmapRegistry::Instance().Remove(_T("nloaded"));

	ide::BitmapRegistry::Instance().Remove(_T("add"));
	ide::BitmapRegistry::Instance().Remove(_T("remove"));
	ide::BitmapRegistry::Instance().Remove(_T("load"));
	ide::BitmapRegistry::Instance().Remove(_T("unload"));
}

//-----------------------------------------------------------------------


namespace ide
{
	enum ButtonCommands {
		TOOL_REGISTER,
		TOOL_UNREGISTER,
		TOOL_LOAD,
		TOOL_UNLOAD,
	};

	BEGIN_EVENT_TABLE(DeltaComponentDirectory, wxPanel)
		EVT_LIST_ITEM_SELECTED(wxID_ANY, DeltaComponentDirectory::onItemSelected)
		EVT_LIST_ITEM_ACTIVATED(wxID_ANY, DeltaComponentDirectory::onItemActivated)
		EVT_MENU(TOOL_REGISTER, DeltaComponentDirectory::onComponentRegister)
		EVT_MENU(TOOL_UNREGISTER, DeltaComponentDirectory::onComponentUnregister)
		EVT_MENU(TOOL_LOAD, DeltaComponentDirectory::onComponentLoad)
		EVT_MENU(TOOL_UNLOAD, DeltaComponentDirectory::onComponentUnload)
	END_EVENT_TABLE();

	//-------------------------------------------------------//
	//---- class DeltaComponentDirectory --------------------//

	COMPONENT_METADATA(
		DeltaComponentDirectory,
		_("Delta Component Directory"),
		_("Directory of Delta scripts that are loaded as components in the system"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_WX_COMPONENT_(DeltaComponentDirectory, wxPanel, DockableComponent);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(DeltaComponentDirectory, table)
	{
		conf::EnumStringProperty* docking = const_cast<conf::EnumStringProperty*>(
			conf::safe_prop_cast<const conf::EnumStringProperty>(
				table.GetProperty("docking")
			)
		);
		if (docking)
			docking->SetOption(3);
	}

	//-----------------------------------------------------------------------

	DeltaComponentDirectory::DeltaComponentDirectory(void)
	{

		ComponentLoader::Instance().sigComponentLoaded.connect(
			boost::bind(&DeltaComponentDirectory::slotComponentLoaded, this, _1)
		);
		ComponentLoader::Instance().sigComponentUnloaded.connect(
			boost::bind(&DeltaComponentDirectory::slotComponentUnloaded, this, _1)
		);
	}

	//-----------------------------------------------------------------------

	DeltaComponentDirectory::~DeltaComponentDirectory(void)
	{
		ComponentLoader::Instance().sigComponentLoaded.disconnect(
			boost::bind(&DeltaComponentDirectory::slotComponentLoaded, this, _1)
		);
		ComponentLoader::Instance().sigComponentUnloaded.disconnect(
			boost::bind(&DeltaComponentDirectory::slotComponentUnloaded, this, _1)
		);
	}

	//-----------------------------------------------------------------------

	wxWindow* DeltaComponentDirectory::GenerateWindow(wxWindow* parent)
	{
		Create(parent);

		//-- Image List
		images = new ImageList(16,16);
		images->Add(_T("loaded"), BitmapRegistry::Instance().Get(_T("loaded")));
		images->Add(_T("nloaded"), BitmapRegistry::Instance().Get(_T("nloaded")));
		images->Add(_T("add"), BitmapRegistry::Instance().Get(_T("add")));

		//-- Create Project Tree Control
		listview = new wxListView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			wxLC_LIST | wxLC_SINGLE_SEL);
		listview->AssignImageList(images, wxIMAGE_LIST_SMALL);

		//-- Populate Delta component list
		StdStringList components;
		ComponentLoader::Instance().ListAvailableComponents(components);
		BOOST_FOREACH(const std::string& classId, components)
			if (ComponentLoader::Instance().GetComponentType(classId) == "DeltaScript")
				InsertComponent(classId);

		//-- Relize Toolbar
		toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
		toolbar->SetToolBitmapSize(wxSize(16,16));
		ConstructToolbar(false, false);

		wxBoxSizer* vSizer = new wxBoxSizer(wxVERTICAL);
		vSizer->Add(toolbar, 0, wxALL, 0);
		vSizer->AddSpacer(3);
		vSizer->Add(listview, 1, wxEXPAND | wxALL, 0);

		SetSizer(vSizer);
		vSizer->SetSizeHints(this);		///< set size hints to honour minimum size

		return this;
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DeltaComponentDirectory, bool, RegisterComponent, (const String& uri))
	{
		//-- Extract the source and binary files
		const std::string output_path = GetExtensionDirectory();
		if (!boost::filesystem::exists(output_path))
			boost::filesystem::create_directory(output_path);

		std::string source = util::normalizepath(util::str2std(uri));
		size_t index = source.find_last_of('/');
		index = (index == std::string::npos) ? 0 : index;

		const std::string basename = source.substr(index +1, source.find_last_of(".") - index -1);
		const std::string binary = output_path + basename + ".dbc";

		if (listview->FindItem(-1, util::std2str(basename)) != -1 || !boost::filesystem::exists(source))
			return false;

		//-- Ensure that if script is open in an editor it is saved before compilation
		std::vector<Handle> editors =
			MCall<const Handle& (const String&)>(this, "EditorManager", "GetEditor")(util::std2str(source));

		//-- Save script file
		BOOST_FOREACH(const Handle& editor, editors)
			if (editor && Call<bool (void)>(this, editor, "IsModified")())
				Call<bool (void)>(this, editor, "Save")();

		//-- The profile is considered a domain specific project, as such it should have a local copy
		//	 of the added configuration script, not relying on the remote instance of it, in order for
		//	 a profile to be considered as a whole package without external references
		const String original_uri = uri;
		const String symbolic = util::std2str(basename) + _T(".dsc");
		const String local_uri = util::std2str(output_path) + util::std2str(basename) + _T(".dsc");

		if (!boost::filesystem::equivalent(util::str2std(original_uri), util::str2std(local_uri)))
			///< in this case we need a local copy
		{
			boost::filesystem::copy_file(util::str2std(original_uri), util::str2std(local_uri));
			source = util::str2std(local_uri);
		}

		//-- add XML definition file in xdf dir
		CreateXDF(basename, source);

		//-- load definition file
		if (!ComponentLoader::Instance().LoadDefinitionFile(GetXDFDirectory() + basename + ".xdf"))
			return false;

		//-- append component in the list
		InsertComponent(basename);
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DeltaComponentDirectory, bool, UnregisterComponent, (const std::string& classId))
	{
		const std::string extensionDir = GetExtensionDirectory();
		const std::string source = extensionDir + classId + ".dsc";
		const std::string binary = extensionDir + classId + ".dbc";

		//-- make sure the component is unloaded before it is removed from the system
		if (!ComponentLoader::Instance().UnloadComponent(classId))
			return false;

		//-- unload definition file from loader
		ComponentLoader::Instance().UnloadDefinitionFile(GetXDFDirectory() + classId + ".xdf");

		//-- remove XML definition file in xdf dir
		DeleteXDF(classId);

		//-- remove component from the list
		RemoveComponent(classId);

		//-- remove source and binary files
		if (boost::filesystem::exists(source))
			boost::filesystem::remove(source);
		if (boost::filesystem::exists(binary))
			boost::filesystem::remove(binary);

		//-- update the toolbar
		ConstructToolbar(false, false);
		return true;
	}

	//-----------------------------------------------------------------------

	void DeltaComponentDirectory::AssumeFailed (const std::string& classId) 
		{ failed[classId] = false; }

	void DeltaComponentDirectory::AssumeSucceeded (const std::string& classId) {
		FailedMap::iterator i = failed.find(classId);
		if (i != failed.end())
			failed.erase(i);
	}

	bool DeltaComponentDirectory::HadFailed (const std::string& classId) const
		{ return failed.find(classId) != failed.end(); }

	EXPORTED_MEMBER(DeltaComponentDirectory, bool, CheckByteCodeFileExistsAndUpTodate, (const std::string& classId)) {

		if (HadFailed(classId))
			return false;

		const std::string	output_path	= GetExtensionDirectory();
		const std::string	source		= ComponentLoader::Instance().GetComponentURI(classId);
		size_t				slash_index = source.find_last_of("/") + 1;
		const std::string	basename	= source.substr(slash_index, source.find_last_of('.') - slash_index);
		const std::string	binary		= output_path + basename + ".dbc";

		if (!boost::filesystem::exists(source))
			return false;

		if (!boost::filesystem::exists(binary)) {
			IDE_ERROR_DELTA_BYTE_CODE_FILE_NOT_FOUND("component", binary, classId);
			AssumeFailed(classId);
		}
		else	
		if (boost::filesystem::last_write_time(binary) < boost::filesystem::last_write_time(source))
			IDE_WARNING_DELTA_BYTE_CODE_FILE_NOT_UP_TO_DATE("component", binary, classId);

		return true;
	}
	
	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DeltaComponentDirectory, bool, LoadComponent, (const std::string& classId))
	{
		assert(ComponentLoader::Instance().GetComponentType(classId) == "DeltaScript");
		if (ComponentLoader::Instance().LoadComponent(classId))
			{ AssumeSucceeded(classId); return true; }
		else
			{ AssumeFailed(classId); return false; }
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DeltaComponentDirectory, Handle, CreateComponent, (const std::string& classId))
	{
		Component* comp, *shell = ComponentRegistry::Instance().GetFocusedInstance("Shell");
		if(	ComponentLoader::Instance().GetComponentType(classId) != "DeltaScript"	||
			!(comp = ComponentFactory::Instance().CreateComponent(classId, shell))
		)
			return Handle();

		//-- update toolbar	
		ConstructToolbar(true);
		return comp;
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DeltaComponentDirectory, bool, UnloadComponent, (const std::string& classId))
	{
		//-- unload component along with its derived classes
		if (ComponentEntry& compEntry = ComponentRegistry::Instance().GetComponentEntry(classId)) {
			ComponentEntry::DerivedClassList classIds = compEntry.GetDerivedClasses(true);
			classIds.push_front(classId);

			for(ComponentEntry::DerivedClassList::const_reverse_iterator i = classIds.rbegin(); i != classIds.rend(); ++i) {
				ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(*i);
				const std::string unloadNegotiationFunc = "RequestDestruction";
				if (entry.HasFunction(unloadNegotiationFunc)) {
					const ComponentFuncEntry& funcEntry = entry.GetFuncEntry(unloadNegotiationFunc);
					if (funcEntry.IsMemberFunc()) {
						BOOST_FOREACH(Component* comp, entry.GetInstances())
							if (!Call<bool (void)>(this, comp, unloadNegotiationFunc)())
								return false;
					}
					else if (funcEntry.IsStaticFunc())
						if (!Call<bool (void)>(this, *i, unloadNegotiationFunc)())
							return false;
				}
			}

			for(ComponentEntry::DerivedClassList::const_reverse_iterator i = classIds.rbegin(); i != classIds.rend(); ++i)
				ComponentLoader::Instance().UnloadComponent(*i);

			//-- update toolbar
			ConstructToolbar(false);
			return true;
		}
		else
			return false;
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DeltaComponentDirectory, StringList, GetAvailableComponents, (void))
	{
		StringList list;
		wxListItem item;

		for (int i=0; i < listview->GetItemCount(); ++i)
		{
			item.SetId(i);
			item.SetMask(wxLIST_MASK_TEXT);
			listview->GetItem(item);
			list.push_back(item.GetText());
		}

		return list;
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DeltaComponentDirectory, const String, GetComponentFromSymbolicURI, (const String& symbolic))
	{
		size_t index = symbolic.find(_T(".dsc"));
		if (index == String::npos)
			return String();

		const std::string classId = util::str2std(symbolic.substr(0, index));
		return ComponentLoader::Instance().GetComponentType(classId) == "DeltaScript" ?
			util::std2str(classId) : String();
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DeltaComponentDirectory, const String, GetURIFromComponent, (const String& classId))
	{
		return util::std2str(GetExtensionDirectory()) + classId + _T(".dsc");
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_STATIC(DeltaComponentDirectory, View, _("/{10}View/{200}Delta Component Directory\tCtrl+Shift+C"), MT_MAIN, "")
	{
		EnsureVisibility("DeltaComponentDirectory");
	}

	//-----------------------------------------------------------------------

	const std::string DeltaComponentDirectory::GetExtensionDirectory(void) const
		{ return util::str2std(Call<const String (void)>(const_cast<DeltaComponentDirectory*>(this), "DeltaVM", "GetExtensionScriptDirectory")()); }

	//-----------------------------------------------------------------------

	const std::string DeltaComponentDirectory::GetXDFDirectory(void) const
	{
		const std::string sparrowDir = util::normalizepath(IDECore::GetInstallationDir()) + "/";
		return sparrowDir + "xdf/";
	}

	//-----------------------------------------------------------------------

	void DeltaComponentDirectory::ConstructToolbar(bool loaded, bool selected)
	{
		toolbar->ClearTools();

		toolbar->AddTool(
			TOOL_REGISTER, _("Register"),
			BitmapRegistry::Instance().Get(_T("add")),
			_("Register a Delta script component")
		);

		if (!selected) {
			toolbar->Realize();
			return;
		}

		toolbar->AddTool(
			TOOL_UNREGISTER, _T("Unregister"),
			BitmapRegistry::Instance().Get(_T("remove")),
			_("Unregister a Delta script component")
		);
		toolbar->AddSeparator();

		if (loaded)
			toolbar->AddTool(
				TOOL_UNLOAD, _("Unload"),
				BitmapRegistry::Instance().Get(_T("unload")),
				_("Unload Delta script component")
			);
		else
			toolbar->AddTool(
				TOOL_LOAD, _("Load"),
				BitmapRegistry::Instance().Get(_T("load")),
				_("Load Delta script component")
			);
		toolbar->Realize();
	}

	//-----------------------------------------------------------------------

	void DeltaComponentDirectory::CreateXDF(const std::string& classId, const std::string& source)
	{
		//-- Create XML document
		xml::Node root;
		root.Create(_T("extension"));
		root.SetProperty(_T("version"), _T("alpha"));

		xml::Node node;
		node.Create(_T("component"));
		node.SetProperty(_T("id"), util::std2str(classId));
		node.SetProperty(_T("type"), _T("DeltaScript"));
		node.SetProperty(_T("uri"), util::std2str(source));
		root.InsertChild(node);

		wxXmlDocument doc;
		doc.SetFileEncoding(_T("utf-8"));
		doc.SetRoot(root.NativeType());
		doc.Save(util::std2str(GetXDFDirectory() + classId + ".xdf"), 2);
	}

	//-----------------------------------------------------------------------

	void DeltaComponentDirectory::DeleteXDF(const std::string& classId)
	{
		//-- Delete xdf file
		std::string path(GetXDFDirectory() + classId + ".xdf");
		if (boost::filesystem::exists(path))
			boost::filesystem::remove(path);
	}

	//-----------------------------------------------------------------------

	void DeltaComponentDirectory::InsertComponent(const std::string& classId)
	{
		listview->InsertItem(listview->GetItemCount(), util::std2str(classId),
			images->ResolveImageIndex(
				(ComponentLoader::Instance().IsLoaded(classId) ? _T("loaded") : _T("nloaded"))
			)
		);
	}

	//-----------------------------------------------------------------------

	void DeltaComponentDirectory::RemoveComponent(const std::string& classId)
	{
		listview->DeleteItem(listview->FindItem(-1, util::std2str(classId)));
	}

	//-----------------------------------------------------------------------

	void DeltaComponentDirectory::slotComponentLoaded(const std::string& classId)
	{
		if (ComponentLoader::Instance().GetComponentType(classId) != "DeltaScript")
			return;

		long index = listview->FindItem(-1, util::std2str(classId));
		if (index < 0) {
			InsertComponent(classId);
			return;
		}

		//-- update component state
		listview->SetItemImage(index, images->ResolveImageIndex(_T("loaded")));

		//-- update toolbar actions
		if (listview->GetFocusedItem() == index)
			ConstructToolbar(true);
	}

	//-----------------------------------------------------------------------

	void DeltaComponentDirectory::slotComponentUnloaded(const std::string& classId)
	{
		if (ComponentLoader::Instance().GetComponentType(classId) != "DeltaScript")
			return;

		long index = listview->FindItem(-1, util::std2str(classId));
		if (index < 0)
			return;

		//-- update component state
		listview->SetItemImage(index, images->ResolveImageIndex(_T("nloaded")));
	}

	//-----------------------------------------------------------------------

	void DeltaComponentDirectory::onItemSelected(wxListEvent& event)
	{
		ConstructToolbar(
			ComponentLoader::Instance().IsLoaded(util::str2std(event.GetItem().GetText()))
		);
	}

	//-----------------------------------------------------------------------

	void DeltaComponentDirectory::onItemActivated(wxListEvent& event)
	{
		const std::string compId = util::str2std(event.GetItem().GetText());
		const String uri = util::std2str(ComponentLoader::Instance().GetComponentURI(compId));
		const String message = _("Extension scripts can only be edited in the Sparrow workspace.\nDo you want to open the Sparrow workspace to edit component '") + util::std2str(compId) + _T("'?");
		if (Call<bool (const String&)>(this, "Shell", "LoadSparrowWorkspaceDialog")(message)) {
			DockableComponent::EnsureVisibility("EditorManager");
			if (const Handle script = Call<Handle (const String&)>(this, "ProjectManager", "GetResourceByURI")(uri))
				Call<void (void)>(this, script, "Open")();
		}
	}

	//-----------------------------------------------------------------------

	void DeltaComponentDirectory::onComponentRegister(wxCommandEvent& event)
	{
		FileDialog fileDlg(0, _("Register Delta Component script"), String(), String(), _T("Delta script (*.dsc)|*.dsc"),
			wxFD_OPEN | wxFD_FILE_MUST_EXIST);
		if (fileDlg.ShowModal() != wxID_OK)
			return;

		RegisterComponent(fileDlg.GetPath());
	}

	//-----------------------------------------------------------------------

	void DeltaComponentDirectory::onComponentUnregister(wxCommandEvent& event)
	{
		wxListItem item;
		item.SetId(listview->GetFocusedItem());
		item.SetMask(wxLIST_MASK_TEXT);
		listview->GetItem(item);

		const String classId = item.GetText();
		const String msg = _("This action will permanently remove ") + classId +
			_(" from the Delta Component Directory and cannot be undone.\nAre you sure?");
		if (gui::getConfirmation(0, _T("Sparrow"), msg))
			UnregisterComponent(util::str2std(classId));
	}

	//-----------------------------------------------------------------------

	void DeltaComponentDirectory::onComponentLoad(wxCommandEvent& event)
	{
		wxListItem item;
		item.SetId(listview->GetFocusedItem());
		item.SetMask(wxLIST_MASK_TEXT);
		listview->GetItem(item);

		const String classId = item.GetText();
		const Handle profile = Call<Handle (void)>(this, "Shell", "GetActiveProfile")();
		if (!Call<bool (const String&)>(this, profile, "LoadComponent")(classId))
			LoadComponent(util::str2std(classId));
	}

	//-----------------------------------------------------------------------

	void DeltaComponentDirectory::onComponentUnload(wxCommandEvent& event)
	{
		wxListItem item;
		item.SetId(listview->GetFocusedItem());
		item.SetMask(wxLIST_MASK_TEXT);
		listview->GetItem(item);

		UnloadComponent(util::str2std(item.GetText()));
	}

	//-----------------------------------------------------------------------
}
