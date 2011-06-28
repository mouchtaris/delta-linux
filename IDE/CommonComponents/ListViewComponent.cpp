/**
 *	ListViewComponent.cpp
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
#include "ListViewComponent.h"

#include "StringUtils.h"
#include "ImageList.h"
#include "BitmapRegistry.h"

#include "ComponentMenu.h"

#include "Call.h"
#include "DelayedCaller.h"

#include <wx/menu.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <vector>

namespace ide
{
	//-------------------------------------------------------//
	//---- Custom Events ------------------------------------//

	DECLARE_LOCAL_EVENT_TYPE(EVENT_PRINT_LIST_VIEW, -1);
	DEFINE_EVENT_TYPE(EVENT_PRINT_LIST_VIEW);

	BEGIN_EVENT_TABLE(ListViewComponent, ItemListCtrl)
		EVT_COMMAND(wxID_ANY, EVENT_PRINT_LIST_VIEW, ListViewComponent::onDisplay)
		EVT_MENU(wxID_ANY, ListViewComponent::onMenuCommand)
		EVT_LIST_ITEM_RIGHT_CLICK(wxID_ANY, ListViewComponent::onContextMenu)
		EVT_LIST_ITEM_ACTIVATED(wxID_ANY, ListViewComponent::onItemActivated)
		EVT_KEY_DOWN(ListViewComponent::onKeyDown)
	END_EVENT_TABLE();


	//-------------------------------------------------------//
	//---- class ListViewComponent --------------------------//

	COMPONENT_METADATA(
		ListViewComponent,
		_("List View"),
		_("ListView is a base class component providing a list view interface"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_WX_COMPONENT_(ListViewComponent, ItemListCtrl, DockableComponent);

	//-----------------------------------------------------------------------

	EXPORTED_SIGNAL(ListViewComponent, ListItemActivated, (uint index));
	EXPORTED_SIGNAL(ListViewComponent, DeleteListItem, (uint index));

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(ListViewComponent, table)
	{
	}

	//-----------------------------------------------------------------------

	ListViewComponent::ListViewComponent(void)
		: imageList(0), popupMenu(UserCommand()), menu(0)
	{
	}

	//-----------------------------------------------------------------------

	ListViewComponent::~ListViewComponent(void)
	{
		if (menu) {
			delete menu;
			menu = 0;
		}
	}

	//-----------------------------------------------------------------------

	wxWindow* ListViewComponent::GenerateWindow(wxWindow* parent)
	{
		Create(parent, wxLC_REPORT | wxLC_SINGLE_SEL);
		menu = popupMenu.GenerateMenu(new ComponentMenu);
		return this;
	}

	//-----------------------------------------------------------------------

	wxWindow* ListViewComponent::GetWindow(void)
	{
		return this;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, void, SetTitle, (const String& title))
	{
		this->title = title;
		Call<bool (const Handle&, const String&)>(this, "Shell", "RenameWindow")(this, title);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, const String&, GetTitle, (void))
	{
		return title;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, void, SetColumns, (const StringList& columns))
	{
		int i=0;
		BOOST_FOREACH(const String& title, columns)
		{
			std::vector<String> tokens;
			util::stringtokenizer(tokens, title, _T(":"));
			ItemListCtrl::InsertColumn(i++, tokens[0], wxLIST_FORMAT_LEFT,
				(tokens.size() == 2 ? boost::lexical_cast<int>(tokens[1]) : 150));
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, void, SetImages, (const StringList& images))
	{
		imageList = new ImageList(16,16);

		BOOST_FOREACH(const String& image, images)
			imageList->Add(image, BitmapRegistry::Instance().Get(image));

		ItemListCtrl::AssignImageList(imageList, wxIMAGE_LIST_SMALL);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, void, Append, (const String& line))
	{
		Insert(line, GetItemCount());
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, void, AppendFields, (const StringList& fieldList))
	{
		InsertFields(fieldList, GetItemCount());
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, void, Insert, (const String& line, uint index))
	{
		long id = ItemListCtrl::InsertItem(index, String());

		std::vector<String> tokens;
		util::stringtokenizer(tokens, line, _T("#"));

		int i=0;
		BOOST_FOREACH(const String& text, tokens)
			ItemListCtrl::SetItem(id, i++, text);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, void, InsertFields, (const StringList& fieldList, uint index))
	{
		long id = ItemListCtrl::InsertItem(index, String());

		int i=0;
		BOOST_FOREACH(const String& text, fieldList)
			ItemListCtrl::SetItem(id, i++, text);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, void, Remove, (uint index))
	{
		if (index >= (uint) GetItemCount())
			return;

		ItemListCtrl::DeleteItem(index);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, uint, GetTotalLines, (void))
	{
		return GetItemCount();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, String, GetText, (uint row, uint col))
	{
		if (row >= (uint) GetItemCount())
			return String();

        wxListItem info;
		info.m_itemId = row;
        info.m_mask = wxLIST_MASK_TEXT;
        info.m_col = col;

		return (GetItem(info)) ? info.GetText() : String();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, void, SetText, (uint row, uint col, const String& text))
	{
		if (row >= (uint) GetItemCount())
			return;

		wxListItem info;
		info.m_itemId = row;
        info.m_col = col;
        info.m_mask = wxLIST_MASK_TEXT;
		info.SetText(text);

		SetItem(info);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, const wxColour, GetTextColor, (uint row))
	{
		return row < (uint) GetItemCount() ? GetItemTextColour(row) : wxColour();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, void, SetTextColor, (uint row, const wxColour& color))
	{
		if (row < (uint) GetItemCount())
			SetItemTextColour(row, color);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, const wxFont, GetFont, (uint row))
	{
		return row < (uint) GetItemCount() ? GetItemFont(row) : wxFont();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, void, SetFont, (uint row, const wxFont& font))
	{
		if (row < (uint) GetItemCount())
			SetItemFont(row, font);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, String, GetImage, (uint row, uint col))
	{
		if (row >= (uint) GetItemCount())
			return String();

        wxListItem info;
		info.m_itemId = row;
        info.m_mask = wxLIST_MASK_IMAGE;
        info.m_col = col;

		return (GetItem(info)) ? imageList->GetImageKeyByIndex(info.GetImage()) : String();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, void, SetImage, (const String& image, uint row, uint col))
	{
		if (row >= (uint) GetItemCount())
			return;

		wxListItem info;
		info.m_itemId = row;
        info.m_col = col;
        info.m_mask = wxLIST_MASK_IMAGE;
		info.SetImage(imageList->ResolveImageIndex(image));

		SetItem(info);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, bool, IsFontStyleEnabled, (uint row, const String& style))
	{
		wxFont font = GetItemFont(row);
		if(style == _T("bold"))
			return font.GetWeight() == wxFONTWEIGHT_BOLD;
		else if(style == _T("italic"))
			return font.GetStyle() == wxFONTSTYLE_ITALIC;
		else if(style == _T("underlined"))
			return font.GetUnderlined();
		else
			return false;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, void, SetFontStyleEnabled, (uint row, const String& style, bool enabled))
	{
		wxFont font = GetItemFont(row);
		
		if(style == _T("bold"))
			font.SetWeight(enabled ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
		else if(style == _T("italic"))
			font.SetStyle(enabled ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
		else if(style == _T("underlined"))
			font.SetUnderlined(enabled);

		SetItemFont(row, font);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, bool, HasSingleSelection, (void))
	{
		return !!(GetWindowStyle() & wxLC_SINGLE_SEL);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, void, SetSingleSelectionMode, (bool value))
	{
		SetSingleStyle(wxLC_SINGLE_SEL, value);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, int, GetSingleSelection, (void))
	{
		return GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, UIntList, GetMultipleSelection, (void))
	{
		UIntList selected;
		int item = -1;
		while((item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)) != -1)
			selected.push_back(item);
		return selected;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, void, SetSingleSelection, (uint index))
	{
		if (index >= (uint) GetItemCount())
			return;
		for(uint i = 0; i < (uint) GetItemCount(); ++i)
			SetItemState(i, i == index ? wxLIST_STATE_SELECTED : 0, wxLIST_STATE_SELECTED);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, void, SetMultipleSelection, (UIntList items))
	{
		for(uint i = 0; i < (uint) GetItemCount(); ++i)
			SetItemState(i, 0, wxLIST_STATE_SELECTED);
		BOOST_FOREACH(uint index, items)
			if(index < (uint) GetItemCount())
				 SetItemState(index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ListViewComponent, void, Clear, (void))
	{
		ItemListCtrl::DeleteAllItems();
	}

	//-----------------------------------------------------------------------

	void ListViewComponent::ComponentAddedUserCommand(const String& path, const UserCommand& cmd)
	{
		ComponentMenu *oldMenu = menu;
		popupMenu.InsertUserCommand(path, cmd);
		menu = popupMenu.GenerateMenu(new ComponentMenu);
		delete oldMenu;
	}

	//-----------------------------------------------------------------------

	void ListViewComponent::ComponentRemovedUserCommand(const String& path)
	{
		ComponentMenu *oldMenu = menu;
		popupMenu.RemoveUserCommand(path);
		menu = popupMenu.GenerateMenu(new ComponentMenu);
		delete oldMenu;
	}

	//-----------------------------------------------------------------------

	static void _callComponentCallback(const UserCommand::Callback& action)
	{
		action();
	}

	//-----------------------------------------------------------------------

	void ListViewComponent::onDisplay(wxCommandEvent& event)
	{
	}

	//-----------------------------------------------------------------------

	void ListViewComponent::onMenuCommand(wxCommandEvent& event)
	{
		timer::DelayedCaller::Instance().PostDelayedCall(
			boost::bind(_callComponentCallback, menu->GetAction(event.GetId()))
		);
	}

	//-----------------------------------------------------------------------

	void ListViewComponent::onContextMenu (wxListEvent& event)
	{
		SetSingleSelection(event.GetIndex());
		PopupMenu(menu);
	}

	//-----------------------------------------------------------------------

	void ListViewComponent::onItemActivated(wxListEvent& event)
	{
		sigListItemActivated(this, event.GetIndex());
	}

	//-----------------------------------------------------------------------

	void ListViewComponent::onKeyDown(wxKeyEvent& event)
	{
		int key = event.GetKeyCode();
		if (key == WXK_DELETE || key == WXK_NUMPAD_DELETE)
		{
			UIntList selected = GetMultipleSelection();
			// Perform reverse iteration to ensure correct numbering while deleting
			for(UIntList::const_reverse_iterator i = selected.rbegin(); i != selected.rend(); ++i)
				sigDeleteListItem(this, *i);
		}
		else if (!HasSingleSelection() && key == 'A' && event.ControlDown())
			for(uint i = 0; i < (uint) GetItemCount(); ++i)	//Select all
				SetItemState(i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
		else
			event.Skip();
	}

	//-----------------------------------------------------------------------
}
