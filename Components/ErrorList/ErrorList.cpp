/**
 *	ErrorList.cpp
 *
 *	-- IDE Console Output --
 *
 *	List control component for compilation error/warning.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#include "ErrorList.h"

#include "StringUtils.h"
#include "PropertyUtils.h"
#include "ImageList.h"

#include "Call.h"
#include "ComponentRegistry.h"
#include "BitmapRegistry.h"

#include "UndefineAllWin32Shit.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#ifndef NO_VLD
#include <vld.h>
#endif

#include "Icons/error.xpm"
#include "Icons/warning.xpm"

namespace ide
{
	//-------------------------------------------------------//
	//---- Custom Events ------------------------------------//

	BEGIN_EVENT_TABLE(ErrorList, ItemListCtrl)
		EVT_LIST_ITEM_ACTIVATED(wxID_ANY, ErrorList::onItemActivated)
	END_EVENT_TABLE();


	//-------------------------------------------------------//
	//---- class ErrorList ----------------------------------//

	COMPONENT_METADATA(
		ErrorList,
		_("Errors"),
		_("Error list report for the Delta compiler"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_WX_COMPONENT_(ErrorList, ItemListCtrl, DockableComponent);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(ErrorList, table)
	{
		conf::EnumStringProperty* docking = const_cast<conf::EnumStringProperty*>(
			conf::safe_prop_cast<const conf::EnumStringProperty>(
				table.GetProperty("docking")
			)
		);
		if (docking)
			docking->SetOption(5);
	}

	//-----------------------------------------------------------------------

	ErrorList::ErrorList(void)
		: counter(0)
	{
	}

	//-----------------------------------------------------------------------

	ErrorList::~ErrorList(void)
	{
	}

	//-----------------------------------------------------------------------

	wxWindow* ErrorList::GenerateWindow(wxWindow* parent)
	{
		Create(parent, wxLC_REPORT | wxLC_SINGLE_SEL);

		ImageList* images = new ImageList(16,16);
		images->Add(_T("Error"), BitmapRegistry::Instance().Get(_T("error")));
		images->Add(_T("Warning"), BitmapRegistry::Instance().Get(_T("warning")));
		ItemListCtrl::AssignImageList(images, wxIMAGE_LIST_SMALL);

		ItemListCtrl::InsertColumn(0, String(), wxLIST_FORMAT_LEFT, 25);
		ItemListCtrl::InsertColumn(1, _T("#"), wxLIST_FORMAT_LEFT, 25);
		ItemListCtrl::InsertColumn(2, _T("Description"), wxLIST_FORMAT_LEFT, 300);
		ItemListCtrl::InsertColumn(3, _T("File"), wxLIST_FORMAT_LEFT, 200);
		ItemListCtrl::InsertColumn(4, _T("Line"), wxLIST_FORMAT_LEFT, 50);

		//wxTextAttr style = GetDefaultStyle();
		//style.SetFont(wxFont(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
		//SetDefaultStyle(style);

		return this;
	}

	//-----------------------------------------------------------------------

	wxWindow* ErrorList::GetWindow(void)
	{
		return this;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(ErrorList, void, Initialize, (void))
	{
		BitmapRegistry::Instance().Insert(_T("error"), error_xpm);
		BitmapRegistry::Instance().Insert(_T("warning"), warning_xpm);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(ErrorList, void, CleanUp, (void))
	{
		//BitmapRegistry::Instance().Remove(_T("error"));
		//BitmapRegistry::Instance().Remove(_T("warning"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ErrorList, void, Append, (const String& type, const String& content, const String& file, uint line))
	{
		if (type == _T("Error") || type == _T("Warning"))
		{
			int imageIndex = static_cast<ImageList*>(GetImageList(wxIMAGE_LIST_SMALL))->ResolveImageIndex(type);
			assert(imageIndex != -1);
			long id = ItemListCtrl::InsertItem(GetItemCount(), imageIndex);
			ItemListCtrl::SetItem(id, 1, util::std2str(boost::lexical_cast<std::string>(++counter)));
			ItemListCtrl::SetItem(id, 2, content);
			ItemListCtrl::SetItem(id, 3, file);
			ItemListCtrl::SetItem(id, 4, util::std2str(boost::lexical_cast<std::string>(line)));
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ErrorList, void, Clear, (void))
	{
		ItemListCtrl::DeleteAllItems();
		counter = 0;
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_STATIC(ErrorList, View, _("/{10}View/{100}Error List\tCtrl+Shift+R"), MT_MAIN, "")
	{
		EnsureVisibility(s_classId);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(ErrorList, void, OnWorkspaceLoaded, (const Handle& workspace, const String& uri),
		"WorkspaceLoaded")
	{
		Clear();
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(ErrorList, void, OnWorkStarted, (const std::string workspace, const Handle& root,
		const String& task), "WorkStarted")
	{
		Clear();
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(ErrorList, void, OnCompilationMessage, (const std::string& caller, const UIntList& buildId,
		const String& type, const String& content, const String& file, uint line), "CompilationMessage")
	{
		Append(type, content, file, line);
	}
	
	//-----------------------------------------------------------------------

	void ErrorList::onItemActivated(wxListEvent& event)
	{
		String filename;
		uint lineno;

        wxListItem info;
		info.m_itemId = event.GetIndex();
        info.m_mask = wxLIST_MASK_TEXT;
        info.m_col = 3;
        if (GetItem(info))
			filename = info.GetText();

        info.m_col = 4;
        if (GetItem(info))
			lineno = boost::lexical_cast<uint>(info.GetText());

		Call<void (const String&, int)>(this, "DeltaVM", "GotoSymbolicDocument")(filename, lineno);
	}

	//-----------------------------------------------------------------------
}
