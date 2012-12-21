/**
 *	Shell.h
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
#ifndef IDE_SHELL_COMPONENT_H_
#define IDE_SHELL_COMPONENT_H_

#include "Common.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentUserCommand.h"
#include "ComponentContainer.h"
#include "ComponentMenu.h"
#include "ComponentToolbar.h"
#include "IDEComponent.h"

#include "KeyBinderFrame.h"

#include <boost/array.hpp>
#include <boost/thread/mutex.hpp>
#include <wx/dnd.h>

#include <string>
#include <stack>
#include <set>

///////////////////////////////////////////////////////////
// DEFINE DLL EXPORT //////////////////////////////////////

#pragma warning(disable: 4251)

class wxAuiManager;

namespace ide
{
	//----------------------------
	//-- class Shell

	class Shell :
		public KeyBinderFrame,
		public IDEComponent
	{
		DECLARE_IDE_COMPONENT(Shell);

	public:
		///--- type definitions
		enum { STATUS_SIZE =5 };
		enum Direction {
			SHELL_LEFT_TOP=0, SHELL_LEFT_BOTTOM, SHELL_RIGHT_TOP, SHELL_RIGHT_BOTTOM,
			SHELL_BOTTOM_LEFT, SHELL_BOTTOM_RIGHT, SHELL_CENTER, SHELL_TOTAL_DIRECTIONS,
			SHELL_PROPERTY=-1
		};

		typedef UserCommand::Callback Callback;

		///--- constructors / destructor
		Shell(void);
		~Shell(void);

		///--- public overridden API
		wxWindow* GenerateWindow(wxWindow* parent);
		wxWindow* GetWindow(void);

		///-- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));

		DECLARE_EXPORTED_FUNCTION(void, Construct, (const String& profile, const String& workspace));
		DECLARE_EXPORTED_FUNCTION(void, SetStartupProfile, (const String& profile));
		DECLARE_EXPORTED_FUNCTION(Handle, GetActiveProfile, (void));

		DECLARE_EXPORTED_FUNCTION(void, RunConfiguration, (const String& conf));

		DECLARE_EXPORTED_FUNCTION(Handle, AddComponent, (const std::string& id, int direction));
		DECLARE_EXPORTED_FUNCTION(bool, MoveComponent, (const Handle& component, int direction));
		DECLARE_EXPORTED_FUNCTION(bool, RemoveComponent, (const Handle& component));
		DECLARE_EXPORTED_FUNCTION(bool, FocusComponent, (const Handle& component));
		DECLARE_EXPORTED_FUNCTION(void, ClearComponents, (void));
		DECLARE_EXPORTED_FUNCTION(bool, CanClearComponents, (void));
		DECLARE_EXPORTED_FUNCTION(bool, RenameWindow, (const Handle& component, const String& title));

		DECLARE_EXPORTED_FUNCTION(bool, SetStatusText, (const String& msg, size_t pos=0, size_t sec=0));
		DECLARE_EXPORTED_FUNCTION(const String&, GetStatusText, (size_t pos=0));
		DECLARE_EXPORTED_FUNCTION(bool, PushStatusText, (const String& msg, size_t pos=0, size_t sec=0));
		DECLARE_EXPORTED_FUNCTION(bool, PopStatusText, (size_t pos=0));

		DECLARE_EXPORTED_FUNCTION(bool, ConfigureComponent, (const String& classId));
		DECLARE_EXPORTED_FUNCTION(void, ShowMessage, (const String& title, const String& label));
		DECLARE_EXPORTED_FUNCTION(bool, GetConfirmation, (const String& title, const String& message));
		DECLARE_EXPORTED_FUNCTION(StringBoolPair, PromptInput, (const String& title, const String& label, const String& defaultValue));
		DECLARE_EXPORTED_FUNCTION(StringList, PromptMultipleInput, (const String& title, const StringList& labels, const StringList& defaultValues));

		DECLARE_EXPORTED_FUNCTION(bool, LoadSparrowWorkspaceDialog, (const String& message));
		DECLARE_EXPORTED_FUNCTION(const String, GetSparrowWorkspaceURI, (void));
		DECLARE_EXPORTED_FUNCTION(bool, HandleInternalSparrowError, (void));

		///--- exported COMMANDS
		DECLARE_EXPORTED_FUNCTION_(void, GlobalUndo, (void),
			_("Undo the last global operation"));
		DECLARE_EXPORTED_FUNCTION_(void, GlobalRedo, (void),
			_("Redo the last global operation"));

		DECLARE_EXPORTED_FUNCTION_(void, ConfigureKeys, (void),
			_("Configure keyboard shortcuts"));
		DECLARE_EXPORTED_FUNCTION_(void, Exit, (void),
			_("Close Sparrow IDE"));

		///--- exported SLOTS
		DECLARE_EXPORTED_FUNCTION(void, slotEditorCursorChanged,
			(const Handle& editor, int line, int column, bool overtype));

	protected:
		///--- protected API
		virtual void ComponentDestroyed(void);
		virtual void ChildDestroyed(Component* comp);

		virtual void ChildAddedAsFirstOfItsKind(Component* component);
		virtual void ChildRemovedAsLastOfItsKind(Component* component);
		virtual void ChildAddedUserCommand(Component* component, const String& path, const UserCommand& cmd);
		virtual void ChildRemovedUserCommand(Component* component, const String& path, uint flags);
		virtual void ChildEnabledUserCommand(Component* component, const String& path, uint flags);
		virtual void ChildDisabledUserCommand(Component* component, const String& path, uint flags);
		virtual void ChildMergedUserCommands(Component* component, const UserCommand& cmds);
		virtual void ChildUnMergedUserCommands(Component* component, const UserCommand& cmds);

		virtual void ChildFocused(Component* component);

	private:
		///--- event table
		DECLARE_EVENT_TABLE();

		///--- type definitions
		class FileDropTarget : public wxFileDropTarget {
		public:
			virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
		};

		///--- private API
		void FirstInstanceCreated(Component* component);
		void LastInstanceDestroyed(Component* component);

		void ClassAddedUserCommand(const std::string& classId, const String& path, const UserCommand& cmd);
		void ClassRemovedUserCommand(const std::string& classId, const String& path, uint flags);
		void ClassEnabledUserCommand(const std::string& classId, const String& path, uint flags);
		void ClassDisabledUserCommand(const std::string& classId, const String& path, uint flags);
		void ClassMergedUserCommands(const std::string& classId, const UserCommand& cmds);
		void ClassUnMergedUserCommands(const std::string& classId, const UserCommand& cmds);

		void UpdateMenuBar(void);
		void CreateToolbar(Component* component);
		void DestroyToolbar(Component* component);
		void UpdateToolbar(Component* component);

		void ConstructPane(uint position);
		void DestroyPane(uint position);
		void DestroyPaneIfEmpty(ComponentContainer* pane, bool lately = false);
		void LatelyDestroyPane(uint position);
		void EnsurePaneVisible(uint position);
		void DestroyMenuBar(ComponentMenuBar* menubar);

		void slotToolSelected(const Callback& action);

		void onMenuSelect(wxCommandEvent& event);
		void onMenuOpen(wxMenuEvent& event);
		void onMenuClose(wxMenuEvent& event);
		void onClose(wxCloseEvent& event);

		void onSizeChanged(wxSizeEvent& event);
		void onMaximize(wxMaximizeEvent& event);
		void onComponentClosed(wxAuiNotebookEvent& event);

		void onConstruct(wxCommandEvent& event);

		///--- private members
		bool onShutdown;
		bool onConstruction;
		UserCommand rootMenu;
		ComponentMenuBar* menubar;

		bool menuActive;
		std::list<ComponentMenuBar*> menuDeleteList;
		boost::mutex menuMutex;

		std::set<Component*> toolbarsToCreate;
		typedef std::map<std::string, ComponentToolBar*> ToolBarList;
		ToolBarList toolbars;

		typedef std::stack<String> Status;
		typedef boost::array<Status, STATUS_SIZE> Statusbar;
		Statusbar statusbar;

		typedef boost::array<ComponentContainer*, SHELL_TOTAL_DIRECTIONS> PanesTable;
		PanesTable panes;

		typedef std::map<String, Direction> DirectionMapping;
		static DirectionMapping str2ide;
		typedef boost::array<uint, SHELL_TOTAL_DIRECTIONS> IntArray;
		static IntArray ide2wx;

		wxAuiManager* aui;
		Component* profile;			///< profile component
		Component* prevProfile;		///< stored in order to revert in case of error occurance when applying a new profile
	};
}

#endif	//IDE_SHELL_COMPONENT_H_
