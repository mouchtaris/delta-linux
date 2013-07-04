/**
 *	TreeListViewComponent.h
 *
 *	-- Sparrow Core Components --
 *
 *	TreeListView component is a base class component
 *	providing a tree list view interface to the derived
 *	components.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	July 2008
 */
#ifndef SPARROW_TREE_LIST_VIEW_COMPONENT_H_
#define SPARROW_TREE_LIST_VIEW_COMPONENT_H_

#include "Common.h"

#include "Component.h"
#include "ComponentHandle.h"

#include "ImageList.h"
#include "treelistctrl.h"
#include "DockableComponent.h"

#include "UndefineAllWin32Shit.h"
#include "ExtSerialization.h"

namespace ide
{
	class _IDE_API TreeListViewComponent :
		public wxCustomTreeListCtrl,
		public DockableComponent
	{
		DECLARE_IDE_COMPONENT(TreeListViewComponent);

	public:
		///--- type definitions
		class TreeItemData : public wxTreeItemData {
		private:
			uint serial;
		public:
			uint GetSerial(void) const { return serial; }

			///--- constructors / destructor
			TreeItemData(uint serial = 0) : serial(serial) {}
			virtual ~TreeItemData(void) {}
		};

		///--- constructors / destructor
		TreeListViewComponent(void);
		~TreeListViewComponent(void);

		///--- public overridden API
		virtual wxWindow* GenerateWindow(wxWindow* parent);
		virtual wxWindow* GetWindow(void);

		///-- exported API
		DECLARE_EXPORTED_FUNCTION_(void, SetTitle, (const String& title),
			_("Set the window's title"));
		DECLARE_EXPORTED_FUNCTION_(const String&, GetTitle, (void),
			_("Get the window's title"));

		DECLARE_EXPORTED_FUNCTION_(void, SetColumns, (const StringList& columns),
			_("Set the column titles of the tree list view"));
		DECLARE_EXPORTED_FUNCTION_(void, SetImages, (const StringList& images),
			_("Set image ids that will be used by the tree list view"));

		DECLARE_EXPORTED_FUNCTION_(uint, Append, (uint parentSerial, const StringList& fields),
			_("Appends an item to the end of the poarent branch using the fields from the given list"));
		DECLARE_EXPORTED_FUNCTION_(uint, Insert, (uint parentSerial, uint index, const StringList& fields),
			_("Insert an item at the specified positionusing the fields from the given list"));
		DECLARE_EXPORTED_FUNCTION_(bool, Remove, (uint serial),
			_("Remove an item from the treelist"));

		DECLARE_EXPORTED_FUNCTION_(int, GetRelativePosition, (uint serial),
			_("Return the relative position of a node with respect to its siblings"));
		DECLARE_EXPORTED_FUNCTION_(uint, GetTotalChildren, (uint serial, bool recursively),
			_("Return the number of item in the branch (all descendants if used recursively)"));
		DECLARE_EXPORTED_FUNCTION_(UIntList, GetChildren, (uint serial, bool recursively),
			_("Return the children of the given branch (all descendants if used recursively)"));
		DECLARE_EXPORTED_FUNCTION_(uint, GetParent, (uint serial),
			_("Return the parent of the given branch"));
		DECLARE_EXPORTED_FUNCTION_(uint, GetRoot, (void),
			_("Return the root of the tree list view"));
		DECLARE_EXPORTED_FUNCTION_(void, SetRootVisibility, (bool val),
			_("Set the visibility status of the root node of the tree list view"));

		DECLARE_EXPORTED_FUNCTION_(const String, GetText, (uint serial, uint col),
			_("Return the text of the specified field of the tree list view"));
		DECLARE_EXPORTED_FUNCTION_(void, SetText, (uint serial, uint col, const String& text),
			_("Set the text in the specified field of the tree list view"));
		DECLARE_EXPORTED_FUNCTION_(const wxColour, GetTextColor, (uint serial),
			_("Return the text color of the specified item of the tree list view"));
		DECLARE_EXPORTED_FUNCTION_(void, SetTextColor, (uint serial, const wxColour& color),
			_("Set the text color for the specified item of the tree list view"));
		DECLARE_EXPORTED_FUNCTION_(const wxFont, GetFont, (uint serial),
			_("Return the font of the specified item of the tree list view"));
		DECLARE_EXPORTED_FUNCTION_(void, SetFont, (uint serial, const wxFont& font),
			_("Set the font for the specified item of the tree list view"));
		DECLARE_EXPORTED_FUNCTION_(String, GetImage, (uint serial, uint col),
			_("Return image id in the specified field of the tree list view"));
		DECLARE_EXPORTED_FUNCTION_(void, SetImage, (uint serial, uint col, const String& image),
			_("Set image id in the specified field of the tree list view"));

		DECLARE_EXPORTED_FUNCTION_(uint, GetColumnWidth, (uint col),
			_("Return the width for the specified tree list view column"));
		DECLARE_EXPORTED_FUNCTION_(void, SetColumnWidth, (uint col, uint width),
			_("Set the width for the specified tree list view column"));

		DECLARE_EXPORTED_FUNCTION_(void, Clear, (void),	_("Clear the treelist"));

		DECLARE_EXPORTED_FUNCTION_(bool, SetSingleSelectionMode, (bool value),
			_("Change the selection mode of the tree list view to single or multiple (requires at least the root)"));
		DECLARE_EXPORTED_FUNCTION_(bool, HasSingleSelection, (void),
			_("Check if the tree list view is in single selection mode"));
		DECLARE_EXPORTED_FUNCTION_(int, GetSingleSelection, (void),
			_("Get the single selection of the tree list view (returns the first selected when in multiple mode)"));
		DECLARE_EXPORTED_FUNCTION_(UIntList, GetMultipleSelection, (void),
			_("Get the multiple selection of the tree list view (returns the single selection in single mode)"));
		DECLARE_EXPORTED_FUNCTION_(void, SetSingleSelection, (uint serial),
			_("Set the single selection of the tree list view (can also be used in multiple mode)"));
		DECLARE_EXPORTED_FUNCTION_(void, SetMultipleSelection, (UIntList serials),
			_("Set the multiple selection of the tree list view (can only be used in multiple mode)"));

		DECLARE_EXPORTED_FUNCTION_(void, SetItemHasChildren, (uint serial, bool val),
			_("Force appearance of the button next to the item. Useful when adding children while expanding items"));
		DECLARE_EXPORTED_FUNCTION_(void, Collapse, (uint serial), _("Collapse the given item"));
		DECLARE_EXPORTED_FUNCTION_(void, Expand, (uint serial), _("Expand the given item"));
		DECLARE_EXPORTED_FUNCTION_(void, EnsureVisible, (uint serial), 
			_("Ensure that the given item is visible by scrolling and/or expanding items"));
		DECLARE_EXPORTED_FUNCTION_(void, ScrollTo, (uint serial), _("Scroll to a given item"));

		DECLARE_EXPORTED_FUNCTION_(void, OnItemActivated, (uint serial), _("Handler for an item being activated"));
		DECLARE_EXPORTED_FUNCTION_(void, OnItemExpanding, (uint serial), _("Handler for an item being expanded"));		
		DECLARE_EXPORTED_FUNCTION_(void, OnItemCollapsing, (uint serial), _("Handler for an item being activated"));
		DECLARE_EXPORTED_FUNCTION_(void, OnItemSelected, (uint serial), _("Handler for an item being selected"));
		DECLARE_EXPORTED_FUNCTION_(void, OnDeleteItem, (uint serial), _("Handler for deleting an item"));
		DECLARE_EXPORTED_FUNCTION_(void, OnWidthChanged, (uint width), _("Handler for when the treelistctrl width has changed"));
		
	protected:
		///--- protected API
		virtual void ComponentAddedUserCommand(const String& path, const UserCommand& cmd);
		virtual void ComponentRemovedUserCommand(const String& path);

	private:
		///--- event table
		DECLARE_EVENT_TABLE();

		///--- private API
		wxTreeItemId GetNode(uint serial) const;

		void onDisplay			(wxCommandEvent& event);
		void onMenuCommand		(wxCommandEvent& event);
		void onContextMenu		(wxTreeEvent& event);
		void onItemActivated	(wxTreeEvent& event);
		void onItemExpanding	(wxTreeEvent& event);
		void onItemCollapsing	(wxTreeEvent& event);
		void onItemSelected		(wxTreeEvent& event);
		void onKeyDown			(wxTreeEvent& event);
		void onColEndDrag		(wxListEvent& event);

		///--- private members
		ImageList* imageList;
		String title;
		uint treeItemIdCounter;
		unsigned ignoreCollapseAndExpandChanges;

		UserCommand popupMenu;
		ComponentMenu* menu;
	};
}

#endif	//SPARROW_TREE_LIST_VIEW_COMPONENT_H_
