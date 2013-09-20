/**
 *	ProjectManager.cpp
 *
 *	-- IDE Project Manager Component --
 *
 *	Project manager component is a visual component
 *	that allows selection / manipulation and organization
 *	of IDE projects and scripts.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007
 */
#include "ProjectManager.h"

#include "StringUtils.h"
#include "Call.h"
#include "ComponentFunctionCallerSafe.h"
#include "Undo.h"
#include "ComponentFactory.h"
#include "ComponentRegistry.h"
#include "PropertyUtils.h"

#include "IDEDialogs.h"
#include "NewItemDialog.h"
#include "TreeItemDropTarget.h"

#include "BitmapRegistry.h"
#include "UndefineAllWin32Shit.h"

#ifndef NO_VLD
#include <vld.h>
#endif
#include <wx/filename.h>

#include <boost/foreach.hpp>

#include "Icons/workspace.xpm"
#include "Icons/project.xpm"
#include "Icons/aspect_project.xpm"
#include "Icons/filter.xpm"
#include "Icons/generic_file.xpm"
#include "Icons/text_file.xpm"
#include "Icons/script.xpm"
#include "Icons/aspect.xpm"

#include "Icons/stage_source.xpm"
#include "Icons/stage_result.xpm"
#include "Icons/aspect_result.xpm"

#include "Icons/open.xpm"
#include "Icons/save.xpm"
#include "Icons/save_all.xpm"
#include "Icons/project_add.xpm"


//-------------------------------------------------------//
//---- free Init/Clean functions  -----------------------//

_PROJECT_MANAGER_C_API void Initialize(void)
{
	//Components
	ide::BitmapRegistry::Instance().Insert(_T("Workspace"),		workspace_xpm);
	ide::BitmapRegistry::Instance().Insert(_T("Project"),		project_xpm);
	ide::BitmapRegistry::Instance().Insert(_T("AspectProject"),	aspect_project_xpm);
	ide::BitmapRegistry::Instance().Insert(_T("Filter"),		filter_xpm);
	ide::BitmapRegistry::Instance().Insert(_T("GenericFile"),	generic_file_xpm);
	ide::BitmapRegistry::Instance().Insert(_T("TextFile"),		text_file_xpm);
	ide::BitmapRegistry::Instance().Insert(_T("Script"),		script_xpm);
	ide::BitmapRegistry::Instance().Insert(_T("Aspect"),		aspect_xpm);
	ide::BitmapRegistry::Instance().Insert(_T("StageSource"),	stage_source_xpm);
	ide::BitmapRegistry::Instance().Insert(_T("StageResult"),	stage_result_xpm);
	ide::BitmapRegistry::Instance().Insert(_T("AspectResult"),	aspect_result_xpm);
}

//-----------------------------------------------------------------------

_PROJECT_MANAGER_C_API void CleanUp(void)
{
	ide::BitmapRegistry::Instance().Remove(_T("AspectResult"));
	ide::BitmapRegistry::Instance().Remove(_T("StageResult"));
	ide::BitmapRegistry::Instance().Remove(_T("StageSource"));
	ide::BitmapRegistry::Instance().Remove(_T("Aspect"));
	ide::BitmapRegistry::Instance().Remove(_T("Script"));
	ide::BitmapRegistry::Instance().Remove(_T("TextFile"));
	ide::BitmapRegistry::Instance().Remove(_T("GenericFile"));
	ide::BitmapRegistry::Instance().Remove(_T("Filter"));
	ide::BitmapRegistry::Instance().Remove(_T("AspectProject"));
	ide::BitmapRegistry::Instance().Remove(_T("Project"));
	ide::BitmapRegistry::Instance().Remove(_T("Workspace"));
}

//-----------------------------------------------------------------------


namespace ide
{
	BEGIN_EVENT_TABLE(ProjectManager, TreeCtrlComponent)
		EVT_TREE_BEGIN_DRAG(wxID_ANY, ProjectManager::onBeginDrag)
		EVT_TREE_END_DRAG(wxID_ANY, ProjectManager::onEndDrag)
	END_EVENT_TABLE();

	//-------------------------------------------------------//
	//---- class ProjectManager -----------------------------//

	COMPONENT_METADATA(
		ProjectManager,
		_("Project Manager"),
		_("Manager of projects and scripts"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_WX_COMPONENT_(ProjectManager, wxPanel, TreeCtrlComponent);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(ProjectManager, table)
	{
		conf::EnumStringProperty* docking = const_cast<conf::EnumStringProperty*>(
			conf::safe_prop_cast<const conf::EnumStringProperty>(
				table.GetProperty("docking")
			)
		);
		if (docking)
			docking->SetOption(0);
		table.AddProperty("dnd_confirmation", new conf::BoolProperty(_("Drag Confirmation"), false,
			_("Request a confirmation for the Project Manager drag&drop actions"))
		);
	}

	//-----------------------------------------------------------------------

	ProjectManager::ProjectManager(void)
	{
	}

	//-----------------------------------------------------------------------

	ProjectManager::~ProjectManager(void)
	{
	}

	//-----------------------------------------------------------------------

	wxWindow* ProjectManager::GenerateWindow(wxWindow* parent)
	{
		TreeCtrlComponent::GenerateWindow(parent);
		this->SetDropTarget(new TreeItemDropTarget(GetTreeView()));
		return this;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ProjectManager, bool, CanDestroy, (void))
	{
		const Component::List& children = this->Component::GetChildren();
		BOOST_FOREACH(Component* comp, children)
		{
			const ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(comp->GetClassId());
			assert(entry);
			if (entry.HasFunction("CanDestroy")&& !Call<bool (void)>(this, comp, "CanDestroy")())
				return false;
		}
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ProjectManager, bool, OpenWorkspace, (const String& uri))
	{
		if (uri.empty())
			return false;

		CloseWorkspace();

		Component *workspace = GetTreeRoot().Resolve();
		if (!workspace)
			workspace = CreateWorkspace();
		if (!workspace)
			return false;
		else if (Call<bool (const String&)>(this, workspace, "Load")(uri)) {
			//Manually restore the scrollbar to the beginning
			//(if no scrollbar yet create one that will later automatically removed)
			const int orientations[2] = {wxHORIZONTAL, wxVERTICAL};
			ComponentTreeView* treeview = GetTreeView();
			for(unsigned i = 0; i < SIZEOF_ARRAY(orientations); ++i)
			if (treeview->HasScrollbar(orientations[i]))
				treeview->SetScrollPos(orientations[i], 0);
			else
				treeview->SetScrollbar(orientations[i], 0, 0, 0);

			Undo<void (void)>(this, "CloseWorkspace")();
			return true;
		}
		else {
			workspace->Destroy();
			return false;
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ProjectManager, const Handle&, GetWorkspace, (void))
	{
		return GetTreeRoot();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ProjectManager, void, NewWorkspace, (const String& name, const String& folder))
	{
		if (!name.empty() && !folder.empty()) {
			CloseWorkspace();
			Component* workspace = GetTreeRoot().Resolve();
			if (!workspace)
				workspace = CreateWorkspace();

			const wxChar *workspaceExtension = _T(".wsp");

			String pureName;
			if (!name.EndsWith(workspaceExtension, &pureName))
				pureName = name;

			String uri = (folder.Last() != _T('\\') && folder.Last() != _T('/'))
				? folder + _T('/') + pureName + workspaceExtension
				: folder + pureName + workspaceExtension;

			Call<void (const String&)>(this, workspace, "SetName")(pureName);
			Call<void (const String&)>(this, workspace, "SetURI")(uri);
			Call<void (void)>(this, workspace, "CreateDefaultSymbolicURI")();
			Call<bool (void)>(this, workspace, "Save")();
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(ProjectManager, "project_add", project_add_xpm);
	EXPORTED_CMD_FUNCTION(ProjectManager, NewWorkspaceMenu, _("/{0}File/{100}--New/{20}Workspace...\tCtrl+Shift+N"), MT_MAIN, "project_add")
		{ NewWorkspaceDialog(); }
	EXPORTED_CMD_FUNCTION(ProjectManager, NewWorkspaceToolbar, _("/New Workspace..."), MT_TOOLBAR, "project_add")
		{ NewWorkspaceDialog();	}
	EXPORTED_FUNCTION(ProjectManager, void, NewWorkspaceDialog, (void))
	{
		if (GetTreeRoot()	&&
			!gui::getConfirmation(0, _("Close Workspace"), 
				_("Are you sure you want to close your currently open workspace?"))
		)
			return;
		
		const String normalizedCwd = util::normalizepath(wxFileName::GetCwd());
		NewItemDialog dlg(0, StdStringList(1, "Workspace"), normalizedCwd);
		dlg.ShowModal();

		if (dlg.IsOk())
			NewWorkspace(dlg.GetItemName(), dlg.GetItemPath());
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(ProjectManager, "open", open_xpm);
	EXPORTED_CMD_FUNCTION(ProjectManager, OpenWorkspaceMenu, _("/{0}File/{110}Open--/{20}Workspace...\tCtrl+Shift+O"), MT_MAIN, "open")
		{ OpenWorkspaceDialog(); }
	EXPORTED_CMD_FUNCTION(ProjectManager, OpenWorkspaceToolbar, _("/Open Workspace..."), MT_TOOLBAR, "open")
		{ OpenWorkspaceDialog(); }
	EXPORTED_FUNCTION(ProjectManager, void, OpenWorkspaceDialog, (void))
	{
		FileDialog fileDlg(0, _("Open Workspace"), _T("Default DIR"), String(), _T("*.wsp"),
			wxFD_OPEN | wxFD_FILE_MUST_EXIST);
		if (fileDlg.ShowModal() != wxID_OK)
			return;

		OpenWorkspace(fileDlg.GetPath());
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(ProjectManager, "save", save_xpm);
	EXPORTED_CMD_FUNCTION(ProjectManager, SaveWorkspaceMenu, _("/{0}File/{130}--Save/{20}Workspace"), MT_MAIN, "save")
		{ SaveWorkspace(); }
	EXPORTED_CMD_FUNCTION(ProjectManager, SaveWorkspaceToolbar, _("/Save Workspace"), MT_TOOLBAR, "save")
		{ SaveWorkspace(); }
	EXPORTED_FUNCTION(ProjectManager, void, SaveWorkspace, (void))
	{
		Call<void (void), SafeCall>(this, GetTreeRoot(), "Save")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(ProjectManager, "save_all", save_all_xpm);
	EXPORTED_CMD_FUNCTION(ProjectManager, SaveAllMenu, _("/{0}File/{140}Save All--/{20}Workspace Items\tCtrl+Alt+S"), MT_MAIN, "save_all")
		{ SaveAll(); }
	EXPORTED_CMD_FUNCTION(ProjectManager, SaveAllToolbar, _("/Save All Workspace Items"), MT_TOOLBAR, "save_all")
		{ SaveAll(); }
	EXPORTED_FUNCTION(ProjectManager, void, SaveAll, (void))
	{
		if (GetTreeRoot())
			Call<bool (void)>(this, GetTreeRoot(), "SaveAll")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_FUNCTION(ProjectManager, CloseWorkspace, _("/{0}File/{120}--Close--/{20}Workspace\tCtrl+Shift+F4"), MT_MAIN, "")
	{
		const Handle& root = GetTreeRoot();
		if (root)
		{
			const String uri = Call<String (void)>(this, root, "GetURI")();
			Undo<void (String)>(this, "OpenWorkspace")(uri);
			Call<bool (void)>(this, root, "Close")();
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ProjectManager, void, SetStartupResource, (const Handle& comp, bool enable))
	{
		wxTreeItemId id = GetTreeView()->GetNode(comp);
		if(id.IsOk())
			GetTreeView()->SetItemBold(id, enable);
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_STATIC(ProjectManager, View, _("/{10}View/{100}Project Manager\tCtrl+Shift+W"), MT_MAIN, "")
	{
		EnsureVisibility("ProjectManager");
	}

	//-----------------------------------------------------------------------

	Component* ProjectManager::CreateWorkspace(void)
	{
		Component* workspace = ComponentFactory::Instance().CreateComponent("Workspace");
		if (!workspace)
			return (Component *) 0;
		workspace->SetParent(this);

		GetTreeView()->SetComparator(boost::bind(&ProjectManager::OnCompareItems, this, _1, _2));

		AddComponent(Handle(this), workspace);
		Call<void (const String&)>(this, workspace, "SetName")(_T("Workspace"));
		treeview->SelectItem(treeview->GetRootItem());
		return workspace;
	}

	//-----------------------------------------------------------------------

	static wxTreeItemId startDrag;
	void ProjectManager::onBeginDrag(wxTreeEvent& event)
	{
		if (event.GetItem() != GetTreeView()->GetRootItem()) {
			startDrag = event.GetItem();
			event.Allow();
		}
	}

	//-----------------------------------------------------------------------

	void ProjectManager::onEndDrag(wxTreeEvent& event)
	{
		wxTreeItemId endDrag = event.GetItem();
		if (startDrag.IsOk() && endDrag.IsOk() && startDrag != endDrag) {
			//Do NOT use references for the dragged item as they will be invalidated after removal
			const Handle dragged = GetTreeView()->GetComponent(startDrag);
			const Handle& target = GetTreeView()->GetComponent(endDrag);

			const StdStringList targetTypes = Call<const StdStringList (void)>(this, target, "GetChildrenTypes")();
			Handle parent;
			StdStringList parentTypes;
			if (targetTypes.empty()) {
				parent = Handle(target->GetParent());
				parentTypes = Call<const StdStringList (void)>(this, parent, "GetChildrenTypes")();
			}
			else {
				parent = target;
				parentTypes = targetTypes;
			}

			const std::string draggedType = dragged.GetClassId();
			List children;
			dragged->GetChildrenRecursively(children);
			if (std::find(children.begin(), children.end(), parent.Resolve()) != children.end())	//Don't mess up hierarchy
				gui::displayMessage(0, _("Illegal Operation"), _("Cannot move a project to a descendant project!"));
			else if (	draggedType != "Filter" &&
						std::find(parentTypes.begin(), parentTypes.end(), draggedType) == parentTypes.end()
			) {
				const String msg = _("Drop target (") + util::std2str(parent.GetClassId()) +_(") does not support ") +
					util::std2str(draggedType) + _T(" items!");
				gui::displayMessage(0, _("Illegal Operation"), msg);
			}
			else if (dragged != parent && Handle(dragged->GetParent()) != parent) {
				const String& draggedName = Call<const String& (void)>(this, dragged, "GetName")();
				bool performDrag = true;
				BOOST_FOREACH(Component* child, parent->GetChildren())
					if (Call<bool (const String&, const String&)>(this, child, "HasNamingConflict")(draggedName, String())) {
						performDrag = false;
						gui::displayMessage(0, _("Illegal Operation"),
							_("There is already an item with the same Name in the drop target!"));
					}
				if (performDrag && conf::get_prop_value<conf::BoolProperty>(GetProperty("dnd_confirmation"))) {
					const String& parentName = Call<const String& (void)>(this, parent, "GetName")();
					const String msg = _("Dragging ") + util::std2str(draggedType) + _(" ") + draggedName +
						_(" to ") + util::std2str(parent.GetClassId()) + _(" ") + parentName + _(". Continue?");
					performDrag = gui::getConfirmation(0, _("Warning"), msg);
				}
				if (performDrag) {
					RemoveComponent(dragged);
					Call<bool (void), SafeCall>(this, dragged->GetParent(), "Save")();
					dragged->SetParent(parent.Resolve());
					AddComponent(parent, dragged);
					Call<bool (void), SafeCall>(this, parent, "Save")();
					List children;
					dragged->GetChildrenRecursively(children);
					BOOST_FOREACH(Component* child, children)
						AddComponent(child->GetParent(), child);
				}
			}
		}
		event.Skip();
	}

	//-----------------------------------------------------------------------
}
