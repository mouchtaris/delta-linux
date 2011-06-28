/**
 *	ListViewComponent.h
 *
 *	-- Sparrow Core Components --
 *
 *	ListView component is a base class component
 *	providing a list view interface to the derived
 *	components.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	August 2007
 */
#ifndef SPARROW_LIST_VIEW_COMPONENT_H_
#define SPARROW_LIST_VIEW_COMPONENT_H_

#include "Common.h"

#include "Component.h"
#include "ComponentHandle.h"

#include "ImageList.h"
#include "ItemListCtrl.h"
#include "DockableComponent.h"
#include "ExtSerialization.h"

#include "UndefineAllWin32Shit.h"

namespace ide
{
	class _IDE_API ListViewComponent :
		public ItemListCtrl,
		public DockableComponent
	{
		DECLARE_IDE_COMPONENT(ListViewComponent);

	public:
		///--- type definitions

		///--- constructors / destructor
		ListViewComponent(void);
		~ListViewComponent(void);

		///--- public overridden API
		virtual wxWindow* GenerateWindow(wxWindow* parent);
		virtual wxWindow* GetWindow(void);

		///-- exported API
		DECLARE_EXPORTED_FUNCTION_(void, SetTitle, (const String& title),
			_("Set the window's title"));
		DECLARE_EXPORTED_FUNCTION_(const String&, GetTitle, (void),
			_("Get the window's title"));

		DECLARE_EXPORTED_FUNCTION_(void, SetColumns, (const StringList& columns),
			_("Set the column titles of the list view"));
		DECLARE_EXPORTED_FUNCTION_(void, SetImages, (const StringList& images),
			_("Set image ids that will be used by the list view"));

		DECLARE_EXPORTED_FUNCTION_(void, Append, (const String& line),
			_("Append a line at the end of the list, fields separated by '#' character"));
		DECLARE_EXPORTED_FUNCTION_(void, AppendFields, (const StringList& fieldList),
			_("Append a line at the end of the list using the fields from the given list"));
		DECLARE_EXPORTED_FUNCTION_(void, Insert, (const String& line, uint index),
			_("Insert a line in the list, fields separated by '#' character"));
		DECLARE_EXPORTED_FUNCTION_(void, InsertFields, (const StringList& fieldList, uint index),
			_("Insert a line in the list using the fields from the given list"));
		DECLARE_EXPORTED_FUNCTION_(void, Remove, (uint index),
			_("Remove a line from the list"));
		DECLARE_EXPORTED_FUNCTION_(uint, GetTotalLines, (void),
			_("Return number of total lines in the list"));

		DECLARE_EXPORTED_FUNCTION_(String, GetText, (uint row, uint col),
			_("Return text in the specified field of the list view"));
		DECLARE_EXPORTED_FUNCTION_(void, SetText, (uint row, uint col, const String& text),
			_("Set the text in the specified field of the list view"));
		DECLARE_EXPORTED_FUNCTION_(const wxColour, GetTextColor, (uint row),
			_("Get the text color in the row of the list view"));
		DECLARE_EXPORTED_FUNCTION_(void, SetTextColor, (uint row, const wxColour& colour),
			_("Set the text color in the row of the list view"));
		DECLARE_EXPORTED_FUNCTION_(const wxFont, GetFont, (uint row),
			_("Get the font in the row of the list view"));
		DECLARE_EXPORTED_FUNCTION_(void, SetFont, (uint row, const wxFont& font),
			_("Set the font in the row of the list view"));
		DECLARE_EXPORTED_FUNCTION_(String, GetImage, (uint row, uint col),
			_("Return image id in the specified field of the list view"));
		DECLARE_EXPORTED_FUNCTION_(void, SetImage, (const String& image, uint row, uint col),
			_("Set image id in the specified field of the list view"));

		DECLARE_EXPORTED_FUNCTION_(bool, IsFontStyleEnabled, (uint row, const String& style),
			_("Get the style for a specified field of the list view (a list with the style names)"));
		DECLARE_EXPORTED_FUNCTION_(void, SetFontStyleEnabled, (uint row, const String& style, bool enabled),
			_("Set the style for a specified field of the list view"));

		DECLARE_EXPORTED_FUNCTION_(void, SetSingleSelectionMode, (bool value),
			_("Change the selection mode of the list view to single or multiple"));
		DECLARE_EXPORTED_FUNCTION_(bool, HasSingleSelection, (void),
			_("Check if the list view is in single selection mode"));
		DECLARE_EXPORTED_FUNCTION_(int, GetSingleSelection, (void),
			_("Get the single selection of the list view (returns the first selected when in multiple mode)"));
		DECLARE_EXPORTED_FUNCTION_(UIntList, GetMultipleSelection, (void),
			_("Get the multiple selection of the list view (returns the single selection in single mode)"));
		DECLARE_EXPORTED_FUNCTION_(void, SetSingleSelection, (uint index),
			_("Set the single selection of the list view (can also be used in multiple mode)"));
		DECLARE_EXPORTED_FUNCTION_(void, SetMultipleSelection, (UIntList items),
			_("Set the multiple selection of the list view (can only be used in multiple mode)"));

		DECLARE_EXPORTED_FUNCTION_(void, Clear, (void),
			_("Clear the list"));

	protected:
		///--- protected API
		virtual void ComponentAddedUserCommand(const String& path, const UserCommand& cmd);
		virtual void ComponentRemovedUserCommand(const String& path);

	private:
		///--- event table
		DECLARE_EVENT_TABLE();

		///--- private API
		void onDisplay		(wxCommandEvent& event);
		void onMenuCommand	(wxCommandEvent& event);
		void onContextMenu	(wxListEvent& event);
		void onItemActivated(wxListEvent& event);
		void onKeyDown		(wxKeyEvent& event);

		///--- private members
		ImageList* imageList;
		String title;

		UserCommand popupMenu;
		ComponentMenu* menu;
	};
}

#endif	//SPARROW_LIST_VIEW_COMPONENT_H_
