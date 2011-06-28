/**
 *	NewItemDialog.cpp
 *
 *	-- IDE Common Component --
 *
 *	NewItemDialog class is component selection dialog
 *	to insert in a tree-view. Listed items are extracted
 *	from the basic API that a TreeItemComponent provides
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#include "NewItemDialog.h"

#include "ImageList.h"
#include "GenericDialogs.h"
#include "IDEDialogs.h"

#include "BitmapRegistry.h"
#include "ComponentRegistry.h"
#include "Call.h"

#include "Algorithms.h"

#include <wx/stattext.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dirdlg.h>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <exception>

namespace ide
{
	enum ButtonCommands {
		BUTTON_OK,
		BUTTON_CANCEL,
		BUTTON_BROWSE,
	};

	BEGIN_EVENT_TABLE(NewItemDialog, wxDialog)
		EVT_BUTTON(BUTTON_OK, NewItemDialog::onOk)
		EVT_BUTTON(BUTTON_CANCEL, NewItemDialog::onCancel)
		EVT_BUTTON(BUTTON_BROWSE, NewItemDialog::onBrowse)
	END_EVENT_TABLE();


	//-------------------------------------------------------//
	//---- class NewItemDialog ------------------------------//

	NewItemDialog::NewItemDialog(wxWindow* parent, const StdStringList& items, const String& path)
		: wxDialog(parent, wxID_ANY, _("New Item Selection"), wxDefaultPosition, wxDefaultSize,
		  wxDEFAULT_DIALOG_STYLE), items(items), isOk(false)
	{
		//-- Image List
		ImageList* images = new ImageList(16,16);
		BOOST_FOREACH(const std::string& classId, items)
		{
			String key = util::std2str(classId);
			const ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(classId);
			const wxImage& image = BitmapRegistry::Instance().Get(key);
			images->Add(key, (image.Ok() ? image : BitmapRegistry::Instance().Get(_T("Cog"))));
		}

		//-- List View
		wxStaticText* lblItems = new wxStaticText(this, wxID_ANY, _("Select Item:"));
		wxStaticText* lblDescription = new wxStaticText(this, wxID_ANY, _(" Create a new item. "),
			wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
		lstItems = new wxListView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			wxLC_SMALL_ICON | wxLC_SINGLE_SEL);
		lstItems->AssignImageList(images, wxIMAGE_LIST_SMALL);

		BOOST_FOREACH(const std::string& classId, items)
			lstItems->InsertItem(lstItems->GetItemCount(), util::std2str(classId),
				images->ResolveImageIndex(util::std2str(classId))
			);
		assert(lstItems->GetItemCount());
		lstItems->Select(0);

		//-- Text Input
		wxStaticText* lblName = new wxStaticText(this, wxID_ANY, _("File Name:"));
		txtName = new wxTextCtrl(this, wxID_ANY);
		txtName->SetFocus();
		wxStaticText* lblLocation = new wxStaticText(this, wxID_ANY, _("Location:"));
		txtLocation = new wxTextCtrl(this, wxID_ANY);
		txtLocation->SetValue(path);

		//-- Buttons
		btnOk = new wxButton(this, BUTTON_OK, _("Ok"),
			wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
		btnOk->SetDefault();
		this->SetAffirmativeId(BUTTON_OK);
		btnCancel = new wxButton(this, BUTTON_CANCEL, _("Cancel"),
			wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
		this->SetEscapeId(BUTTON_CANCEL);
		btnBrowse = new wxButton(this, BUTTON_BROWSE, _("Browse..."),
			wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

		//-- Layout
			//-- list view of items
		wxBoxSizer* listSizer = new wxBoxSizer(wxVERTICAL);
		listSizer->Add(lblItems,
			wxSizerFlags().Border(wxUP | wxDOWN, 2));
		listSizer->Add(lstItems,
			wxSizerFlags().Proportion(2).Expand().Align(wxALIGN_CENTER_VERTICAL));
		listSizer->Add(lblDescription,
			wxSizerFlags().Border(wxUP | wxDOWN, 4));

			//-- text controls
		wxFlexGridSizer* inputSizer = new wxFlexGridSizer(3, 3, 5, 5);
		inputSizer->AddGrowableCol(1, 1);
		inputSizer->Add(lblName,
			wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxUP | wxDOWN, 5));
		inputSizer->Add(txtName,
			wxSizerFlags(10).Expand().Border(1));
		inputSizer->AddSpacer(5);
		inputSizer->Add(lblLocation,
			wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxUP | wxDOWN, 5));
		inputSizer->Add(txtLocation,
			wxSizerFlags(10).Expand().Border(1));
		inputSizer->Add(btnBrowse,
			wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT).Border(wxLEFT | wxRIGHT, 8));
		inputSizer->AddSpacer(1);

			//-- ok / cancel buttons
		wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
		buttonSizer->Add(btnOk, 0, wxALIGN_BOTTOM | wxALL, 8);
		buttonSizer->Add(btnCancel, 0, wxALIGN_BOTTOM | wxALL, 8);

			//-- external sizer
		wxBoxSizer* vsizer = new wxBoxSizer(wxVERTICAL);
		vsizer->AddSpacer(5);
		vsizer->Add(listSizer,
			wxSizerFlags(2).Expand().Border(wxLEFT | wxRIGHT, 10));
		vsizer->AddSpacer(5);
		vsizer->Add(inputSizer,
			wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT, 10));
		vsizer->AddSpacer(5);
		vsizer->Add(buttonSizer,
			wxSizerFlags().Align(wxALIGN_RIGHT).Border(wxLEFT | wxRIGHT, 10));
		vsizer->AddSpacer(5);

		this->SetSizer(vsizer);
		//this->Fit();
	}

	//-----------------------------------------------------------------------

	NewItemDialog::~NewItemDialog(void)
	{
	}

	//-----------------------------------------------------------------------

	String NewItemDialog::GetItemType(void) const
	{
		long selected = GetSelectedItem();
		if (selected < 0)
			return String();
		return lstItems->GetItemText(selected);
	}

	//-----------------------------------------------------------------------

	long NewItemDialog::GetSelectedItem(void) const
	{
		for (long i=0; i < lstItems->GetItemCount(); ++i)
			if (lstItems->GetItemState(i, wxLIST_STATE_SELECTED))
				return i;
		return -1;
	}

	//-----------------------------------------------------------------------

	void NewItemDialog::onOk(wxCommandEvent& event)
	{
		if (GetSelectedItem() < 0) {
			gui::displayMessage(0, _("Error"),
				_("Please select an item type."));
			return;
		}

		if (GetItemName().empty() || GetItemURI().empty()) {
			gui::displayMessage(0, _("Error"),
				_("Name and/or URI are not correctly specified."));
			return;
		}

		const String& location = txtLocation->GetValue();
		boost::filesystem::path path(util::str2std(location));
		try {
			if (!boost::filesystem::exists(path))
				if (gui::getConfirmation(0, _("Confirmation"),
					_("Directory does not exist.\nDo you want to create it?")))
					boost::filesystem::create_directory(path);
				else
					return;
		} catch (const std::exception& e) {
			gui::displayMessage(0, _("Error"),
				_("Location is an invalid path: ") + util::std2str(e.what()));
			return;
		}

		//-- ensure an seperator is appended at the end of the path
		if (!boost::algorithm::ends_with(location, _T("/"))	&&
			!boost::algorithm::ends_with(location, _T("\\")))
			txtLocation->SetValue(location + _T("/"));

		//-- ensure addition of proper extension
		std::string classId = util::str2std(GetItemType());
		const StringList& extensions =
			Call<StringList (const std::string&)>(classId, classId, "GetSupportedExtensions")(classId);

		if (!util::exists(extensions.begin(), extensions.end(), _T(".*")))
		{
			const String& name = GetItemName();
			bool hasExtension =
				util::reduce(
					extensions.begin(),
					extensions.end(),
					std::logical_or<bool>(),
					boost::bind(
						boost::algorithm::ends_with<String, String>, name, _1
					)
				);
			if (!hasExtension && !extensions.empty())
				txtName->SetValue(name + extensions.front());
		}

		isOk=true;
		Hide();
	}

	//-----------------------------------------------------------------------

	void NewItemDialog::onCancel(wxCommandEvent& event)
	{
		Hide();
	}

	//-----------------------------------------------------------------------

	void NewItemDialog::onBrowse(wxCommandEvent& event)
	{
		wxDirDialog dirDlg(0);
		if (dirDlg.ShowModal() != wxID_OK)
			return;

		txtLocation->SetValue(dirDlg.GetPath());
	}

	//-----------------------------------------------------------------------
}
