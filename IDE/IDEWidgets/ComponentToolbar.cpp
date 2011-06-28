/**
 *	ComponentToolBar.cpp
 *
 *	-- IDEWidgets --
 *
 *	Component toolbar is a wrapper of wxToolbar
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	May 2007
 */
#include "ComponentToolBar.h"
#include "BitmapRegistry.h"
#include "Call.h"

#include <wx/bitmap.h>

#include "Icons/cog.xpm"

namespace ide
{
	BEGIN_EVENT_TABLE(ComponentToolBar, wxToolBar)
		EVT_MENU(wxID_ANY, ComponentToolBar::onToolSelected)
	END_EVENT_TABLE();

	//-------------------------------------------------------//
	//---- class ComponentToolBar ---------------------------//

	wxToolBarToolBase* ComponentToolBar::AddTool(const String& title, const wxBitmap& bitmap, const Callback& action,
		const String& longHelpStr)
	{
		canSeparate=true;
		actions->push_back(action);
		return wxToolBar::AddTool((int)actions->size() -1, title,
			((bitmap.IsOk()) ? bitmap : BitmapRegistry::Instance().Get(_T("Cog"))), wxNullBitmap,
			wxITEM_NORMAL, title, longHelpStr
		);
	}

	//-----------------------------------------------------------------------

	wxToolBarToolBase* ComponentToolBar::AddCheckTool(const String& title, const wxBitmap& bitmap, const Callback& action,
		const String& longHelpStr)
	{
		canSeparate=true;
		actions->push_back(action);
		return wxToolBar::AddTool((int)actions->size() -1, title,
			((bitmap.IsOk()) ? bitmap : BitmapRegistry::Instance().Get(_T("Cog"))), wxNullBitmap,
			wxITEM_CHECK, title, longHelpStr
		);
	}

	//-----------------------------------------------------------------------

	wxToolBarToolBase* ComponentToolBar::AddSeparator(void)
	{
		if (canSeparate)
		{
			canSeparate=false;
			return wxToolBar::AddSeparator();
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	void ComponentToolBar::ClearTools(void)
	{
		canSeparate=false;
		wxToolBar::ClearTools();
	}

	//-----------------------------------------------------------------------

	void ComponentToolBar::onToolSelected(wxCommandEvent& event)
	{
		sigToolSelected(GetToolAction(event.GetId()));
	}

	//-----------------------------------------------------------------------
}
