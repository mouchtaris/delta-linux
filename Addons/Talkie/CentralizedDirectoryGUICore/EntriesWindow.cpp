/**
 *	EntriesWindow.cpp
 *
 *	-- Talkie Centralized Directory stand-alone Server --
 *
 *	Window displaying the user entries of the directory
 *	in the talkie network. (don't tell anyone)
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#include "EntriesWindow.h"

#include "StringUtils.h"
#include "Adaptors.h"

#include "StorageService.h"

#include <wx/statbmp.h>
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/imaglist.h>
#include <wx/menu.h>

#include <boost/lexical_cast.hpp>

namespace talkie
{
	enum ContextMenuCommands {
		CMC_EDIT, CMC_REMOVE,
	};

	BEGIN_EVENT_TABLE(EntriesWindow, wxDialog)
		EVT_LIST_ITEM_RIGHT_CLICK(wxID_ANY, EntriesWindow::onRightClick)
		EVT_MENU(wxID_ANY,					EntriesWindow::onMenuItemSelected)

		//EVT_CLOSE(EntriesWindow::onClose)
	END_EVENT_TABLE();

	//-------------------------------------------------------//
	//---- class EntriesWindow ------------------------------//

	EntriesWindow::EntriesWindow(const String& title)
		: wxDialog(0, wxID_ANY, title, wxDefaultPosition, wxSize(260, 320),
		/*wxMINIMIZE_BOX | */wxCLOSE_BOX | /*wxRESIZE_BORDER | */wxCAPTION | wxSYSTEM_MENU /*| wxSTAY_ON_TOP*/)
	{
		//-- Initialize GUI stuff
		SetIcon(wxIcon(_T("directory_icon")));
		SetBackgroundColour(*wxWHITE);

		wxFont font(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

		images = new wxImageList(32, 32);
		images->Add(wxBitmap(_("person")));
		images->Add(wxBitmap(_("person_red")));
		images->Add(wxBitmap(_("person_prince")));
		images->Add(wxBitmap(_("person_bender")));
		images->Add(wxBitmap(_("person_towelie")));
		images->Add(wxBitmap(_("person_owl")));
		images->Add(wxBitmap(_("person_notis")));
		images->Add(wxBitmap(_("person_guitar")));
		images->Add(wxBitmap(_("person_cartman")));
		images->Add(wxBitmap(_("person_alig")));

		imgLogo = new wxStaticBitmap(this, wxID_ANY, wxBitmap(_T("logo_text")));
		lstEntries = new wxListView(this, wxID_ANY, wxPoint(0,0), wxSize(250, 200),
			wxLC_ICON | wxLC_SINGLE_SEL | wxLC_AUTOARRANGE);
		lstEntries->SetImageList(images, wxIMAGE_LIST_NORMAL);

		wxBoxSizer* vsizer = new wxBoxSizer(wxVERTICAL);
		vsizer->AddSpacer(5);
		vsizer->Add(imgLogo, 0, wxALIGN_CENTER_HORIZONTAL);
		vsizer->AddSpacer(20);
		vsizer->Add(lstEntries, 0, wxALIGN_CENTER_HORIZONTAL);

		SetSizer(vsizer);

		//-- Create context menu
		contextMenu = new wxMenu;
		contextMenu->Append(CMC_EDIT, _("Edit Account"), _("Let's fiddle with him a bit!"));
		contextMenu->Append(CMC_REMOVE, _("Remove Account"), _("Ban him for good!"));
		contextMenu->Enable(CMC_EDIT, false);

		//-- Update Data
		PopulateUserList();
	}

	//-----------------------------------------------------------------------

	EntriesWindow::~EntriesWindow(void)
	{
		delete images;
	}

	//-----------------------------------------------------------------------

	void EntriesWindow::onRightClick(wxListEvent& event)
	{
		PopupMenu(contextMenu);
	}

	//-----------------------------------------------------------------------

	void EntriesWindow::onMenuItemSelected(wxCommandEvent& event)
	{
		DBGOUT << event.GetId() << std::endl;

		switch (event.GetId()) {
			case CMC_EDIT:			EditSelected();			break;
			case CMC_REMOVE:		RemoveSelected();		break;
			default:										break;
		}
	}

	//-----------------------------------------------------------------------

	void EntriesWindow::EditSelected(void)
	{
	}

	//-----------------------------------------------------------------------

	void EntriesWindow::RemoveSelected(void)
	{
	}

	//-----------------------------------------------------------------------

	void EntriesWindow::PopulateUserList(void)
	{
		struct ExtractUsers :
			public std::unary_function<const StorageService::Directory::value_type&, void>
		{
			ExtractUsers(wxListView* list)
				: list(list) {}

			result_type operator ()(argument_type pair) {
				const DirectoryEntry& user = pair.second;
				long index = list->InsertItem(0, user.userInfo.GetHandle(), user.userInfo.GetImage());
				list->SetItemData(index, pair.first.to_long());
			}

		private:
			wxListView* list;
		};

		std::for_each(
			StorageService::Instance().GetDirectory().begin(),
			StorageService::Instance().GetDirectory().end(),
			ExtractUsers(lstEntries)
		);
	}

	//-----------------------------------------------------------------------
}
