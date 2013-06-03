/**
 *	Shell.cpp
 *
 *	-- IDE Shell Component --
 *
 *	Shell component is the basic skeleton of the IDE
 *	where the other visual components are attached
 *	in order to build the whole environment.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007
 */
#include "Shell.h"

#include "ComponentFactory.h"
#include "ComponentRegistry.h"
#include "ComponentInitializer.h"
#include "ComponentConfigurationDialog.h"
#include "Call.h"
#include "ComponentFunctionCallerSafe.h"
#include "DockableComponent.h"
#include "ComponentMenu.h"
#include "UndoManager.h"
#include "BitmapRegistry.h"
#include "TextInputDialog.h"
#include "MultipleTextInputDialog.h"

#include "StringUtils.h"
#include "PropertyUtils.h"
#include "Encoding.h"
#include "NotebookCnt.h"
#include "GenericDialogs.h"
#include "CommonGUIUtils.h"
#include "Streams.h"
#include "Algorithms.h"
#include "Adaptors.h"
#include "DelayedCaller.h"

#include "UndefineAllWin32Shit.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <wx/busyinfo.h>
#include <wx/filename.h>
#include <wx/event.h>
#include <wx/wfstream.h>
#include <wx/aui/aui.h>
#include <wx/textctrl.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
#ifndef NO_VLD
#include <vld.h>
#endif

#include "Icons/cog.xpm"
#include "Icons/undo.xpm"
#include "Icons/redo.xpm"
#include "Icons/keyboard.xpm"
#include "Icons/exit.xpm"

#undef InsertMenu

namespace ide
{
	//-------------------------------------------------------//
	//---- static members -----------------------------------//

	Shell::DirectionMapping	Shell::str2ide;
	Shell::IntArray			Shell::ide2wx;


	//-------------------------------------------------------//
	//---- Custom Events ------------------------------------//

	//DECLARE_EVENT_TYPE(EVENT_SHELL_CONSTRUCT, -1);
	DEFINE_EVENT_TYPE(EVENT_SHELL_CONSTRUCT);

	BEGIN_EVENT_TABLE(Shell, KeyBinderFrame)
		EVT_MENU(wxID_ANY, Shell::onMenuSelect)
		EVT_MENU_OPEN(Shell::onMenuOpen)
		EVT_MENU_CLOSE(Shell::onMenuClose)
		EVT_CLOSE(Shell::onClose)

		EVT_SIZE(Shell::onSizeChanged)
		EVT_MAXIMIZE(Shell::onMaximize)

		EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, Shell::onComponentClosed)

		EVT_COMMAND(wxID_ANY, EVENT_SHELL_CONSTRUCT, Shell::onConstruct)
	END_EVENT_TABLE();

	//-------------------------------------------------------//
	//---- class Shell::FileDropTarget ----------------------//

	bool Shell::FileDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
	{
		DockableComponent::EnsureVisibility("EditorManager");
		for(unsigned i = 0; i < filenames.size(); ++i)
			Call<const Handle (const String&)>("Shell", "EditorManager", "OpenDocument")(filenames[i]);
		return true;
	}

	//-------------------------------------------------------//
	//---- class Shell --------------------------------------//

	COMPONENT_METADATA(
		Shell,
		_("Shell"),
		_("Skeleton, placeholder of the IDE components"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_WX_COMPONENT_(Shell, KeyBinderFrame, IDEComponent);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(Shell, table)
	{
		//-- IDE Boot profile
		const String sparrowDir = util::std2str(IDECore::GetInstallationDir());
		table.AddProperty(
			"init_profile",
			new conf::StringProperty(
				_("Boot Profile"),
				sparrowDir + _T("profiles/welcome/welcome.pfl"),
				_("Profile loaded at the initilization of the IDE")
			)
		);

		//-- IDE window state (window, maximized)
		StringVec state;
		state.push_back(_T("window"));
		state.push_back(_T("maximized"));

		table.AddProperty("state", new conf::EnumStringProperty(_("Window state"), state, 1,
			_("State of the IDE window at initialization"))
		);

		//-- IDE initial size & position
		conf::AggregateProperty* size = new conf::AggregateProperty(_("Window size"), _("initial window size"));
		size->AddProperty("width", new conf::IntProperty(_("width"), 1024, _("width of the window")));
		size->AddProperty("height", new conf::IntProperty(_("height"), 768, _("height of the window")));
		table.AddProperty("size", size);

		conf::AggregateProperty* position = new conf::AggregateProperty(_("Window position"), _("initial window position"));
		position->AddProperty("x", new conf::IntProperty(_("x"), 80, _("x coordinate of top-left corner")));
		position->AddProperty("y", new conf::IntProperty(_("y"), 30, _("y coordinate of top-left corner")));
		table.AddProperty("position", position);
	}

	//-----------------------------------------------------------------------

	Shell::Shell(void)
		: onShutdown(false), onConstruction(false), rootMenu(UserCommand()), menubar(0), menuActive(false), aui(0), profile(0)
	{
	}

	//-----------------------------------------------------------------------

	Shell::~Shell(void)
	{
		if (profile) {
			profile->Destroy();
			profile=0;
		}

		if (menubar) {
			SetMenuBar(0);
			menubar->Destroy();
			menubar=0;
		}

		ComponentRegistry::Instance().sigAddedUserCommand.disconnect(
			boost::bind(&Shell::ClassAddedUserCommand, this, _1, _2, _3));
		ComponentRegistry::Instance().sigRemovedUserCommand.disconnect(
			boost::bind(&Shell::ClassRemovedUserCommand, this, _1, _2, _3));
		ComponentRegistry::Instance().sigEnabledUserCommand.disconnect(
			boost::bind(&Shell::ClassEnabledUserCommand, this, _1, _2, _3));
		ComponentRegistry::Instance().sigDisabledUserCommand.disconnect(
			boost::bind(&Shell::ClassDisabledUserCommand, this, _1, _2, _3));
		ComponentRegistry::Instance().sigMergedUserCommands.disconnect(
			boost::bind(&Shell::ClassMergedUserCommands, this, _1, _2));
		ComponentRegistry::Instance().sigUnMergedUserCommands.disconnect(
			boost::bind(&Shell::ClassUnMergedUserCommands, this, _1, _2));

		ComponentRegistry::Instance().sigCreatedFirstInstance.disconnect(
			boost::bind(&Shell::FirstInstanceCreated, this, _1));
		ComponentRegistry::Instance().sigDestroyedLastInstance.disconnect(
			boost::bind(&Shell::LastInstanceDestroyed, this, _1));

		// Toolbars are destroyed by their parent
		//
		//BOOST_FOREACH(ToolBarList::value_type& pair, toolbars)
		//	pair.second->Destroy();
		//toolbars.clear();

		for (int i=0; i < SHELL_TOTAL_DIRECTIONS; ++i)
			if (panes[i] != 0)
				DestroyPane(i);

		aui->UnInit();
		delete aui;
	}

	//-----------------------------------------------------------------------

	wxWindow* Shell::GenerateWindow(wxWindow* parent)
	{
		const conf::AggregateProperty* size_prop =
			static_cast<const conf::AggregateProperty*>(GetProperty("size"));
		wxSize size = wxSize(
			conf::get_prop_value<conf::IntProperty>(size_prop->GetProperty("width")),
			conf::get_prop_value<conf::IntProperty>(size_prop->GetProperty("height"))
		);

		Create(0, _("Sparrow"), wxDEFAULT_FRAME_STYLE, size);

		//-- Setup Main IDE frame
		SetIcon(wxIcon(_T("ide_icon")));

		//-- Create MenuBar
		rootMenu += GetUserCommands();
		menubar = rootMenu.GenerateMenuBar(new ComponentMenuBar);
		SetMenuBar(menubar);

		CreateStatusBar(STATUS_SIZE);
		int sizes[STATUS_SIZE] = { -1, -1, -1, 120, 70, };
		GetStatusBar()->SetStatusWidths(STATUS_SIZE, sizes);
		PushStatusText(_("Ready"));

		//-- Create wxAUIManager
		aui = new wxAuiManager;
		aui->SetManagedWindow(this);
		aui->Update();

		//-- Construct AUI panes
		for (uint i=0; i < SHELL_TOTAL_DIRECTIONS; ++i)
			panes[i]=0;

		//-- Create Toolbar
		ComponentToolBar* toolbar = new ComponentToolBar(this);
		toolbar->SetToolBitmapSize(wxSize(16,16));
		rootMenu.GenerateToolBar(toolbar);
		toolbar->sigToolSelected.connect(
			boost::bind(&Shell::slotToolSelected, this, _1)
		);
		toolbar->Realize();

		wxAuiPaneInfo toolInfo;
		toolInfo.ToolbarPane()
			.Top()
			.LeftDockable(false)
			.RightDockable(false)
			.BottomDockable(false)
			.Floatable(false)
			.Movable(false);
		aui->AddPane(toolbar, toolInfo);

		//-- Commit changes to the Manager
		Maximize(conf::get_prop_value<conf::EnumStringProperty>(GetProperty("state")) == _T("maximized"));

		ComponentRegistry::Instance().sigAddedUserCommand.connect(
			boost::bind(&Shell::ClassAddedUserCommand, this, _1, _2, _3));
		ComponentRegistry::Instance().sigRemovedUserCommand.connect(
			boost::bind(&Shell::ClassRemovedUserCommand, this, _1, _2, _3));
		ComponentRegistry::Instance().sigEnabledUserCommand.connect(
			boost::bind(&Shell::ClassEnabledUserCommand, this, _1, _2, _3));
		ComponentRegistry::Instance().sigDisabledUserCommand.connect(
			boost::bind(&Shell::ClassDisabledUserCommand, this, _1, _2, _3));
		ComponentRegistry::Instance().sigMergedUserCommands.connect(
			boost::bind(&Shell::ClassMergedUserCommands, this, _1, _2));
		ComponentRegistry::Instance().sigUnMergedUserCommands.connect(
			boost::bind(&Shell::ClassUnMergedUserCommands, this, _1, _2));

		ComponentRegistry::Instance().sigCreatedFirstInstance.connect(
			boost::bind(&Shell::FirstInstanceCreated, this, _1));
		ComponentRegistry::Instance().sigDestroyedLastInstance.connect(
			boost::bind(&Shell::LastInstanceDestroyed, this, _1));

		this->SetDropTarget(new FileDropTarget());
		return this;
	}

	//-----------------------------------------------------------------------

	wxWindow* Shell::GetWindow(void)
	{
		return this;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Shell, void, Initialize, (void))
	{
		BitmapRegistry::Instance().Insert(_T("Cog"), wxImage(cog_xpm));

		//-- Create str to ide directions translation table
		str2ide[_("left top")]		= SHELL_LEFT_TOP;
		str2ide[_("left bottom")]	= SHELL_LEFT_BOTTOM;
		str2ide[_("right top")]		= SHELL_RIGHT_TOP;
		str2ide[_("right bottom")]	= SHELL_RIGHT_BOTTOM;
		str2ide[_("bottom left")]	= SHELL_BOTTOM_LEFT;
		str2ide[_("bottom right")]	= SHELL_BOTTOM_RIGHT;
		str2ide[_("center")]		= SHELL_CENTER;

		//-- Create ide to wx directions translation table
		ide2wx[SHELL_LEFT_TOP]		= wxAUI_DOCK_LEFT;
		ide2wx[SHELL_LEFT_BOTTOM]	= wxAUI_DOCK_LEFT;
		ide2wx[SHELL_RIGHT_TOP]		= wxAUI_DOCK_RIGHT;
		ide2wx[SHELL_RIGHT_BOTTOM]	= wxAUI_DOCK_RIGHT;
		ide2wx[SHELL_BOTTOM_LEFT]	= wxAUI_DOCK_BOTTOM;
		ide2wx[SHELL_BOTTOM_RIGHT]	= wxAUI_DOCK_BOTTOM;
		ide2wx[SHELL_CENTER]		= wxAUI_DOCK_CENTER;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, void, Construct, (const String& profile, const String& workspace))
	{
		if (CanClearComponents())
		{
			wxCommandEvent event(EVENT_SHELL_CONSTRUCT);
			const String profileStr = profile.empty() ? 
				conf::get_prop_value<conf::StringProperty>(GetProperty("init_profile")) : profile;
			event.SetString(profileStr + _T("\a") + workspace);	//custom delimiter
			this->AddPendingEvent(event);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, void, SetStartupProfile, (const String& profile))
	{
		conf::PropertyTable* old = static_cast<conf::PropertyTable*>(GetPropertyTable().Clone());
		conf::PropertyIdVec changed;
		changed.push_back("init_profile");

		conf::set_prop_value<conf::StringProperty>(const_cast<conf::Property*>(GetProperty("init_profile")), profile);
		ComponentRegistry::Instance().GetComponentEntry(this->GetClassId()).NotifyPropertiesChanged(*old, changed);
		delete old;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, Handle, GetActiveProfile, (void))
	{
		return (profile ? profile : Handle());
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, void, RunConfiguration, (const String& conf))
	{
		StringList decisions = GetDecisions(util::std2str(s_classId), conf);

		BOOST_FOREACH(String& decision, decisions)
		{
			DBGOUT << decision << DBGENDL;
			String target = _T("activate ");
			if (boost::algorithm::starts_with(decision, target))
				AddComponent(util::str2std(decision.substr(target.size())), -1);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, Handle, AddComponent, (const std::string& id, int direction))
	{
		Component* comp = ComponentFactory::Instance().CreateComponent(id);
		if (!comp)
			return Handle();
		comp->SetParent(this);

		if (direction < 0 || direction >= SHELL_TOTAL_DIRECTIONS)
			direction =
				str2ide[conf::get_prop_value<conf::EnumStringProperty>(comp->GetProperty("docking"), _T("center"))];

		uint serial = comp->GetSerial();

		GUI_SCOPED_FREEZE(this);
		if (!panes[direction])
			ConstructPane(direction);
		else
			EnsurePaneVisible(direction);
		ComponentContainer * container = panes[direction];
		if (comp->GenerateWindow(container))
		{
			panes[direction]->AddComponent(comp);
			if (!onConstruction)
				aui->Update();
		}
		else {
			DestroyPaneIfEmpty(panes[direction]);
			if (!ComponentRegistry::Instance().IsValidInstance(comp, serial))
				return Handle();
		}

		return Handle(comp);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, bool, MoveComponent, (const Handle& component, int direction))
	{
		//-- Don't reinsert it in the same pane, because it will change its tab's position
		if (panes[direction] && panes[direction]->Contains(component))
			return true;

		for (int i=0; i < SHELL_TOTAL_DIRECTIONS; ++i)
			if (panes[i] && panes[i]->RemoveComponent(component))
				break;

		if (!panes[direction])
			ConstructPane(direction);
		else
			EnsurePaneVisible(direction);
		bool success = panes[direction]->AddComponent(component.Resolve());
		if (!onConstruction)
			aui->Update();
		return success;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, bool, RemoveComponent, (const Handle& component))
	{
		for (int i=0; i < SHELL_TOTAL_DIRECTIONS; ++i)
			if (panes[i] && panes[i]->RemoveComponent(component))
			{
				ComponentFactory::Instance().DestroyComponent(component.Resolve());
				if (!onConstruction)
					aui->Update();
				return true;
			}

		// Remove non gui component
		const List& children = this->Component::GetChildren();
		BOOST_FOREACH(Component *comp, children)
			if(comp == component.Resolve())
			{
				ComponentFactory::Instance().DestroyComponent(comp);
				return true;
			}

		return false;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, bool, FocusComponent, (const Handle& component))
	{
		if (onConstruction)
			return false;
		for (int i=0; i < SHELL_TOTAL_DIRECTIONS; ++i)
			if (panes[i] && panes[i]->Contains(component))
			{
				EnsurePaneVisible(i);
				panes[i]->FocusComponent(component);
				component->GetWindow()->SetFocus();
				return true;
			}
		return false;
	}
	
	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, void, ClearComponents, (void))
	{
		GUI_SCOPED_FREEZE(this);

		//-- Destroy all components
		while (!this->Component::GetChildren().empty())
			this->Component::GetChildren().back()->Destroy();

		//-- Close all panes
		//	 wxWidgets note: NOTEBOOK_PAGE_CLOSED events is not generated, thus I have to manually close them
		for (int i=0; i < SHELL_TOTAL_DIRECTIONS; ++i)
			if (panes[i])
				DestroyPane(i);

		BOOST_FOREACH(ToolBarList::value_type& pair, toolbars)
			pair.second->Destroy();
		toolbars.clear();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, bool, CanClearComponents, (void))
	{
		const Component::List children = this->Component::GetChildren();
		//copy the list to avoid having children removed due to other code running concurrently with this iteration
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

	EXPORTED_FUNCTION(Shell, bool, RenameWindow, (const Handle& component, const String& title))
	{
		for (int i=0; i < SHELL_TOTAL_DIRECTIONS; ++i)
			if (panes[i] && panes[i]->Rename(component, title))
				return true;
		return false;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, bool, SetStatusText, (const String& msg, size_t pos, size_t sec))
	{
		if (pos >= STATUS_SIZE)
			return false;

		Status& status = statusbar[pos];
		while (!status.empty())
			status.pop();
		status.push(msg);

		GetStatusBar()->SetStatusText(msg, (int)pos);
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, const String&, GetStatusText, (size_t pos))
	{
		static String null_string;
		if (pos >= STATUS_SIZE)
			return null_string;

		return statusbar[pos].top();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, bool, PushStatusText, (const String& msg, size_t pos, size_t sec))
	{
		if (pos >= STATUS_SIZE)
			return false;

		statusbar[pos].push(msg);
		GetStatusBar()->SetStatusText(msg, (int)pos);
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, bool, PopStatusText, (size_t pos))
	{
		if (pos >= STATUS_SIZE)
			return false;

		statusbar[pos].pop();
		if (!statusbar[pos].empty())
			GetStatusBar()->SetStatusText(statusbar[pos].top(), (int)pos);
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, bool, ConfigureComponent, (const String& classId))
	{
		return ComponentConfigurationDialog().Configure(util::str2std(classId));
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, void, ShowMessage, (const String& title, const String& label))
	{
		gui::displayMessage(0, title, label);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, bool, GetConfirmation, (const String& title, const String& message))
	{
		return gui::getConfirmation(0, title, message);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, StringBoolPair, PromptInput, (const String& title, const String& label, const String& defaultValue))
	{
		TextInputDialog dlg(title, label, defaultValue);
		return dlg.ShowModal() == wxID_OK ? StringBoolPair(dlg.GetInput(), true) : StringBoolPair(String(), false);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, StringList, PromptMultipleInput, (const String& title, const StringList& labels, const StringList& defaultValues))
	{
		MultipleTextInputDialog dlg(title, labels, defaultValues);
		return dlg.ShowModal() == wxID_OK ? dlg.GetInput() : StringList();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, const String, GetSparrowWorkspaceURI, (void)) 
		{ return util::std2str(IDECore::GetInstallationDir() + "scripts/Sparrow.wsp"); }

	EXPORTED_FUNCTION(Shell, bool, LoadSparrowWorkspaceDialog, (const String& message))
	{
		const String sparrowWorkspace = GetSparrowWorkspaceURI();
		
		Handle wspComponent;
		if (ComponentRegistry::Instance().GetFocusedInstance("ProjectManager"))
			wspComponent = Call<const Handle& (void)>(this, "ProjectManager", "GetWorkspace")();

		if (wspComponent && Call<const String& (void)>(this, wspComponent, "GetURI")() == sparrowWorkspace)
			return true;
		else if (gui::getConfirmation(0, _("Sparrow"), message)) {
			DockableComponent::EnsureVisibility("ProjectManager");
			Call<bool (const String&), SafeCall>("IDECore", "ProjectManager", "OpenWorkspace")(sparrowWorkspace);
			return true;
		}
		else
			return false;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Shell, bool, HandleInternalSparrowError, (void))
	{
		bool retval = false;

		const StringList errors = Call<const StringList (void)>(this, "DeltaVM", "GetFailedConfigAndExtensionScriptErrors")();
		assert(!errors.empty());
		const String error = errors.size() == 1 ? errors.front() :
				_("Some errors occured in the configuration and/or extension scripts.");

		const String message = error + _("\nDo you want to open the Sparrow workspace to resolve errors?");
		if (LoadSparrowWorkspaceDialog(message)) {
			const StringList uris = Call<const StringList (void)>(this, "DeltaVM", "GetFailedConfigAndExtensionScriptURIs")();
			BOOST_FOREACH(const String& uri, uris)
				if (const Handle script = Call<Handle (const String&)>(this, "ProjectManager", "GetResourceByURI")(uri))
					Call<void (void)>(this, script, "Open")();
			retval = true;
		}

		BOOST_FOREACH(const String& error, errors)
			Call<void (const String&), SafeCall>(this, "Output", "Append")(error + _T("\n"));
		Call<void (void)>(this, "DeltaVM", "ClearFailedConfigAndExtensionScripts")();

		return retval;
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Shell, "undo", undo_xpm);
	EXPORTED_CMD_FUNCTION(Shell, GlobalUndo, _("/{1}Edit/{0}--Undo"), MT_ALL, "undo")
	{
		UndoManager::Instance().Undo();
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Shell, "redo", redo_xpm);
	EXPORTED_CMD_FUNCTION(Shell, GlobalRedo, _("/{1}Edit/{1}Redo--"), MT_ALL, "redo")
	{
		UndoManager::Instance().Redo();
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Shell, "kbd", keyboard_xpm);
	EXPORTED_CMD_FUNCTION(Shell, ConfigureKeys, _("/{1}Edit/{9999}--Configure keys"), MT_ALL, "kbd")
	{
		KBConfigureKeys();
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Shell, "exit", exit_xpm);
	EXPORTED_CMD_FUNCTION(Shell, Exit, _("/{0}File/{9999}--Exit\tCtrl+Q"), MT_ALL, "exit")
	{
		if (gui::getConfirmation(this, _("Exit?!?"), _("Do you really want to exit?")))
			Close();
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(Shell, void, slotEditorCursorChanged,
		(const Handle& editor, int line, int column, bool overtype),
		"FocusedEditorCursorChanged")
	{
		String status = _T("Ln ") + boost::lexical_cast<String>(line)
			+ _T("\tCol ") + boost::lexical_cast<String>(column);
		SetStatusText(status, STATUS_SIZE-2);
		SetStatusText(((overtype) ? _T("OVR") : _T("INS")), STATUS_SIZE-1);
	}

	//-----------------------------------------------------------------------

	void Shell::ComponentDestroyed(void)
	{
	}

	//-----------------------------------------------------------------------

	void Shell::ChildAddedAsFirstOfItsKind(Component* component)
	{
		rootMenu += component->GetUserCommands();
		UpdateMenuBar();
		CreateToolbar(component);
	}

	//-----------------------------------------------------------------------

	void Shell::ChildRemovedAsLastOfItsKind(Component* component)
	{
		if (onShutdown)
			return;

		rootMenu.SubtractNonStatic(component->GetUserCommands());
		UpdateMenuBar();
		DestroyToolbar(component);
	}

	//-----------------------------------------------------------------------

	void Shell::ChildDestroyed(Component* comp)
	{
		if (comp == profile)
			profile=0;
		else
			for (int i=0; i < SHELL_TOTAL_DIRECTIONS; ++i)
				if (panes[i])
					panes[i]->RemoveComponent(comp);
	}

	//-----------------------------------------------------------------------

	void Shell::ChildAddedUserCommand(Component* component, const String& path, const UserCommand& cmd)
	{
		ClassAddedUserCommand(component->GetClassId(), path, cmd);
		UpdateToolbar(component);
	}

	//-----------------------------------------------------------------------

	void Shell::ChildRemovedUserCommand(Component* component, const String& path, uint flags)
	{
		ClassRemovedUserCommand(component->GetClassId(), path, flags);
		if (flags & MT_TOOLBAR)
			UpdateToolbar(component);
	}

	//-----------------------------------------------------------------------

	void Shell::ChildEnabledUserCommand(Component* component, const String& path, uint flags)
	{
		ClassEnabledUserCommand(component->GetClassId(), path, flags);
		if (flags & MT_TOOLBAR)
			UpdateToolbar(component);
	}

	//-----------------------------------------------------------------------

	void Shell::ChildDisabledUserCommand(Component* component, const String& path, uint flags)
	{
		ClassDisabledUserCommand(component->GetClassId(), path, flags);
		if (flags & MT_TOOLBAR)
			UpdateToolbar(component);
	}

	//-----------------------------------------------------------------------

	void Shell::ChildMergedUserCommands(Component* component, const UserCommand& cmds)
	{
		ClassMergedUserCommands(component->GetClassId(), cmds);
		UpdateToolbar(component);
	}

	//-----------------------------------------------------------------------

	void Shell::ChildUnMergedUserCommands(Component* component, const UserCommand& cmds)
	{
		ClassUnMergedUserCommands(component->GetClassId(), cmds);
		UpdateToolbar(component);
	}

	//-----------------------------------------------------------------------

	void Shell::ChildFocused(Component* component)
	{
		for (int i=0; i < SHELL_TOTAL_DIRECTIONS; ++i)
			if (panes[i] && panes[i]->Contains(component))
				panes[i]->FocusComponent(component);
	}

	//-----------------------------------------------------------------------

	void Shell::FirstInstanceCreated(Component* component)
	{
		rootMenu += component->GetUserCommands();
		UpdateMenuBar();
	}

	//-----------------------------------------------------------------------

	void Shell::LastInstanceDestroyed(Component* component)
	{
		if (onShutdown)
			return;

		rootMenu.SubtractNonStatic(component->GetUserCommands());
		UpdateMenuBar();
	}

	//-----------------------------------------------------------------------

	void Shell::ClassAddedUserCommand(const std::string& classId, const String& path, const UserCommand& cmd)
	{
		if (!cmd.IsStatic() && ComponentRegistry::Instance().GetComponentEntry(classId).GetInstances(true).empty())
			return;

		rootMenu.InsertUserCommand(path, cmd);
		UpdateMenuBar();
	}

	void Shell::ClassRemovedUserCommand(const std::string& classId, const String& path, uint flags)
	{
		if (flags & MT_MAIN) {
			rootMenu.RemoveUserCommand(path);
			UpdateMenuBar();
		}
	}

	void Shell::ClassEnabledUserCommand(const std::string& classId, const String& path, uint flags)
	{
		if (flags & MT_MAIN) {
			rootMenu.EnableUserCommand(path);
			UpdateMenuBar();
		}
	}

	void Shell::ClassDisabledUserCommand(const std::string& classId, const String& path, uint flags)
	{
		if (flags & MT_MAIN) {
			rootMenu.DisableUserCommand(path);
			UpdateMenuBar();
		}
	}

	void Shell::ClassMergedUserCommands(const std::string& classId, const UserCommand& cmds)
	{
		rootMenu += cmds;
		UpdateMenuBar();
	}

	void Shell::ClassUnMergedUserCommands(const std::string& classId, const UserCommand& cmds)
	{
		rootMenu.Subtract(cmds);
		UpdateMenuBar();
	}

	//-----------------------------------------------------------------------

	void Shell::UpdateMenuBar(void)
	{
		if (onConstruction)
			return;

		ComponentMenuBar* oldMenuBar = static_cast<ComponentMenuBar*>(GetMenuBar());
		menubar = rootMenu.GenerateMenuBar(new ComponentMenuBar);
		SetMenuBar(menubar);
		DestroyMenuBar(oldMenuBar);
	}

	//-----------------------------------------------------------------------

	void Shell::CreateToolbar(Component* component)
	{
		if (onConstruction) {
			toolbarsToCreate.insert(component);
			return;
		}

		ComponentToolBar* toolbar = new ComponentToolBar(this);
		toolbar->SetToolBitmapSize(wxSize(16,16));
		component->GetUserCommands().GenerateToolBar(toolbar);
		if (toolbar->GetToolsCount() == 0)
		{
			toolbar->Destroy();
			return;
		}

		toolbar->Realize();
		toolbar->sigToolSelected.connect(
			boost::bind(&Shell::slotToolSelected, this, _1)
		);
		toolbars[component->GetClassId()] = toolbar;

		wxAuiPaneInfo toolInfo;
		toolInfo.ToolbarPane()
			.Top()
			.LeftDockable(false)
			.RightDockable(false)
			.BottomDockable(false)
			.Floatable(false)
			.Movable(false);
		aui->AddPane(toolbar, toolInfo);

		//-- Commit changes to the Manager
		aui->Update();
	}

	//-----------------------------------------------------------------------
	
	void Shell::DestroyToolbar(Component* component)
	{
		if (onConstruction) {
			toolbarsToCreate.erase(component);
			return;
		}

		ToolBarList::iterator it = toolbars.find(component->GetClassId());
		if (it != toolbars.end()) {
			aui->DetachPane(it->second);
			it->second->Destroy();
			toolbars.erase(it);
			aui->Update();
		}
	}

	//-----------------------------------------------------------------------
	
	void Shell::UpdateToolbar(Component* component)
	{
		if (!onConstruction) {
			DestroyToolbar(component);
			CreateToolbar(component);
		}
	}

	//-----------------------------------------------------------------------

	void Shell::ConstructPane(uint position)
	{
		assert(panes[position] == 0);

		wxAuiPaneInfo info;
		info.DefaultPane()
			.Floatable(true)
			.Gripper(false)
			.BestSize(wxSize(200, 200))
			.CaptionVisible(/*false*/);

		if (position == SHELL_CENTER)
			info.CloseButton(false);
		else
			info.PinButton(true);

		panes[position] = new ComponentContainer(this);
		info.Direction(ide2wx[position]);
		aui->AddPane(panes[position], info);
	}

	//-----------------------------------------------------------------------

	void Shell::DestroyPane(uint position)
	{
		assert(panes[position] != 0);

		aui->DetachPane(panes[position]);
		panes[position]->Destroy();
		panes[position]=0;
	}

	//-----------------------------------------------------------------------

	void Shell::DestroyPaneIfEmpty(ComponentContainer* pane, bool lately)
	{
		if (pane->GetPageCount() == 0)
			for (uint i=0; i < SHELL_TOTAL_DIRECTIONS; ++i)
				if (panes[i] == pane) {
					if (lately)
						timer::DelayedCaller::Instance().PostDelayedCall(
							boost::bind(&Shell::LatelyDestroyPane, this, i)
						);
					else
						LatelyDestroyPane(i);
					break;
				}
	}

	//-----------------------------------------------------------------------

	void Shell::LatelyDestroyPane(uint position)
	{
		DestroyPane(position);
		if (!onConstruction)
			aui->Update();
	}

	//-----------------------------------------------------------------------

	void Shell::EnsurePaneVisible(uint position)
	{
		wxAuiPaneInfo& info = aui->GetPane(panes[position]);
		if (!info.IsShown()) {
			info.Show();
			aui->Update();
		}
	}

	//-----------------------------------------------------------------------

	void Shell::DestroyMenuBar(ComponentMenuBar* menubar)
	{
		boost::mutex::scoped_lock lock(menuMutex);
		if (menuActive)
			menuDeleteList.push_back(menubar);
		else
			menubar->Destroy();
	}

	//-----------------------------------------------------------------------

	void _callComponentCallback(const UserCommand::Callback& action)
	{
		action();
	}

	//-----------------------------------------------------------------------

	void Shell::slotToolSelected(const Callback& action)
	{
		timer::DelayedCaller::Instance().PostDelayedCall(
			boost::bind(_callComponentCallback, action)
		);
	}

	//-----------------------------------------------------------------------

	void Shell::onMenuSelect(wxCommandEvent& event)
	{
		timer::DelayedCaller::Instance().PostDelayedCall(
			boost::bind(_callComponentCallback, menubar->GetAction(event.GetId()))
		);

		//Upon activating an accelerator a MENU_OPEN event is sent before
		//the selection so clear the menuActive flag here as well.
		{
			boost::mutex::scoped_lock lock(menuMutex);
			BOOST_FOREACH(ComponentMenuBar* menu, menuDeleteList)
				menu->Destroy();
			menuDeleteList.clear();
			menuActive = false;
		}
	}

	//-----------------------------------------------------------------------

	void Shell::onMenuOpen(wxMenuEvent& event)
	{
		boost::mutex::scoped_lock lock(menuMutex);
		menuActive = true;
	}

	//-----------------------------------------------------------------------

	void Shell::onMenuClose(wxMenuEvent& event)
	{
		boost::mutex::scoped_lock lock(menuMutex);
		BOOST_FOREACH(ComponentMenuBar* menu, menuDeleteList)
			menu->Destroy();
		menuDeleteList.clear();
		menuActive = false;
	}

	//-----------------------------------------------------------------------

	void Shell::onClose(wxCloseEvent& event)
	{
		if (CanClearComponents())
		{
			onShutdown=true;
			this->Component::Destroy();
			timer::DelayedCaller::Instance().CancelAllDelayedCalls();
		}

		event.Veto();
	}

	//-----------------------------------------------------------------------

	void Shell::onSizeChanged(wxSizeEvent& event)
	{
		conf::EnumStringProperty* state = (conf::EnumStringProperty*)GetProperty("state");
		if (state->GetValue() == _T("maximized"))
			return;

		//-- IDE initial size & position
		conf::AggregateProperty* size = (conf::AggregateProperty*)GetProperty("size");
		static_cast<conf::IntProperty*>(size->GetProperty("width"))->SetValue(event.GetSize().GetWidth());
		static_cast<conf::IntProperty*>(size->GetProperty("height"))->SetValue(event.GetSize().GetHeight());
	}

	//-----------------------------------------------------------------------

	void Shell::onMaximize(wxMaximizeEvent& event)
	{
		conf::EnumStringProperty* state = (conf::EnumStringProperty*)GetProperty("state");
		state->SetValue(_T("maximized"));
	}

	//-----------------------------------------------------------------------

	void Shell::onComponentClosed(wxAuiNotebookEvent& event)
	{
		ComponentContainer* pane = static_cast<ComponentContainer*>(event.GetEventObject());
		Component* comp = pane->GetComponent(event.GetSelection());

		const ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(comp->GetClassId());
		assert(entry);
		if (!entry.HasFunction("CanDestroy") || Call<bool (void)>(this, comp, "CanDestroy")())
		{
			pane->RemoveComponent(comp);
			comp->Destroy();
			DestroyPaneIfEmpty(pane, true);
		}
		event.Veto();
	}

	//-----------------------------------------------------------------------

	void Shell::onConstruct(wxCommandEvent& event)
	{
		GUI_SCOPED_FREEZE(this);
		ClearComponents();
		toolbarsToCreate.clear();
		
		wxBusyInfo wait(_T("Loading IDE components..."));
		onConstruction = true;

		const String& str = event.GetString();
		String::size_type delim = str.find('\a');
		String profileUri = str.substr(0, delim);
		wxFileName filename(profileUri);
		if (!filename.IsAbsolute()) {
			const String sparrowDir = util::std2str(IDECore::GetInstallationDir());
			profileUri = sparrowDir + profileUri;	// if relative it should always be in the default installation dir
		}
		String workspaceUri = str.substr(delim + 1);

		bool status = true;
		profile = ComponentFactory::Instance().CreateComponent("Profile", this);
		if (profile) {
			try { Call<bool (const String&)>(this, profile, "Load")(profileUri); }
			catch(...) { status = false; }
		}
		else
			status = false;

		if (!status) {
			gui::displayMessage(0, _("Fatal Error"),
				_("Unable to instantiate a Shell profile! Reinstalling the application may solve the problem.\n\nProgram is terminating...\n")
			);

			//-- Gracefully exit the application
			onConstruction = false;
			onShutdown = true;
			this->Component::Destroy();
			return;
		}

		BOOST_FOREACH(Component* comp, profile->GetChildren())
		{
			try { status &= Call<bool (void)>(this, comp, "Run")(); }
			catch(...) { status = false; }
		}

		const StringList errors = Call<const StringList (void), SafeCall>(this, "DeltaVM", "GetFailedConfigAndExtensionScriptErrors")();
		if (!errors.empty()) {
			if (HandleInternalSparrowError())
				workspaceUri.clear();	//do not load any other workspace if already opened Sparrow workspace
		}
		else if (!status)
			gui::displayMessage(0, _("Warning"),
				_("An error occured while loading the profile!\nProfile instantiation may be incomplete.")
			);

		if (!workspaceUri.empty())
			Call<bool (const String&), SafeCall>("IDECore", "ProjectManager", "OpenWorkspace")(workspaceUri);

		onConstruction = false;
		UpdateMenuBar();
		BOOST_FOREACH(Component* comp, toolbarsToCreate)
			if (comp != profile)	//to avoid creating a toolbar for the profile
				CreateToolbar(comp);
		toolbarsToCreate.clear();
		aui->Update();
	}

	//-----------------------------------------------------------------------
}
