/**
 *	TreeCtrlComponent.h
 *
 *	-- IDE Common Components --
 *
 *	Common component of a tree control containing
 *	components.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#ifndef COMMON_TREE_CTRL_COMPONENT_H_
#define COMMON_TREE_CTRL_COMPONENT_H_

#include "Common.h"
#include "IDECommon.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"
#include "DockableComponent.h"

#include "ComponentToolbar.h"
#include "ComponentTreeView.h"
#include "UndefineAllWin32Shit.h"

#include <wx/panel.h>

class wxBoxSizer;

namespace ide
{
	//----------------------------
	//-- class TreeCtrlComponent

	class _IDE_API TreeCtrlComponent :
		public wxPanel,
		public DockableComponent
	{
		DECLARE_IDE_COMPONENT(TreeCtrlComponent);

	public:
		///--- type definitions
		typedef UserCommand::Callback Callback;

		///--- constructors / destructor
		TreeCtrlComponent(void);
		~TreeCtrlComponent(void);

		///--- public overridden API
		virtual wxWindow* GenerateWindow(wxWindow* parent);
		virtual wxWindow* GetWindow(void);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_FUNCTION_(bool, AddComponent, (const Handle& parent, const Handle& handle),
			_("Append a new component in the tree view"));
		DECLARE_EXPORTED_FUNCTION_(bool, RemoveComponent, (const Handle& handle),
			_("Remove an already existing component in the tree view"));
		DECLARE_EXPORTED_FUNCTION_(bool, RenameComponent, (const Handle& handle, const String& name),
			_("Rename an already existing component in the tree view"));
		DECLARE_EXPORTED_FUNCTION_(bool, EditLabel, (const Handle& handle),
			_("Edit the label of a component in the tree view"));

		DECLARE_EXPORTED_FUNCTION_(const Handle&, GetTreeRoot, (void),
			_("Return the root component of the tree view"));
		DECLARE_EXPORTED_FUNCTION_(const Handle&, GetParent, (const Handle& handle),
			_("Return the parent component of the given component in the tree view"));

		DECLARE_EXPORTED_FUNCTION_(void, SortChildren, (const Handle& handle),
			_("Sort the children of the given component in the tree view"));

		DECLARE_EXPORTED_FUNCTION_(void, Collapse, (const Handle& handle),
			_("Collapse the given component in the tree view"));
		DECLARE_EXPORTED_FUNCTION_(void, Expand, (const Handle& handle),
			_("Expand the given component in the tree view"));

		DECLARE_EXPORTED_FUNCTION_(HandleList, GetResources, (const String& type),
			_("Return a list of handles of all components of a given type in the tree view"));
		DECLARE_EXPORTED_FUNCTION_(Handle, GetResource, (const String& name),
			_("Return a handle to a resource in the tree view given its name"));
		DECLARE_EXPORTED_FUNCTION_(Handle, GetResourceByURI, (const String& uri),
			_("Return a handle to a resource in the tree view given its URI"));
		DECLARE_EXPORTED_FUNCTION_(Handle, GetResourceBySymbolicURI, (const String& uri),
			_("Return a handle to a resource in the tree view given its symbolic URI"));

	protected:
		ComponentTreeView *GetTreeView(void) const	{ return treeview; }

	private:
		///--- event table
		DECLARE_EVENT_TABLE();

		///--- private API
		void slotToolSelected(const Callback& action);
		void onSelectionChanged(wxTreeEvent& event);
		void onItemActivated(wxTreeEvent& event);
		void onItemCollapsed(wxTreeEvent& event);
		void onItemExpanded(wxTreeEvent& event);

		int OnCompareItems(const Handle& handle1, const Handle& handle2);

		///--- private members
		wxBoxSizer*			vSizer;
		ComponentToolBar*	toolbar;
		ComponentTreeView*	treeview;
	};
}

#endif	//COMMON_TREE_CTRL_COMPONENT_H_
