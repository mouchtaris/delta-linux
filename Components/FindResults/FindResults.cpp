/**
 *	FindResults.cpp
 *
 *	-- IDE Find Results FindResults --
 *
 *	FindResults for the find results for the IDE.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	October 2008
 */
#include "FindResults.h"

#include "Streams.h"
#include "StringUtils.h"
#include "PropertyUtils.h"

#include "Call.h"
#include "ComponentRegistry.h"

#include "UndefineAllWin32Shit.h"

#include <boost/lexical_cast.hpp>

#include <vld.h>

namespace ide
{
	//-------------------------------------------------------//
	//---- Custom Events ------------------------------------//

	DECLARE_LOCAL_EVENT_TYPE(EVENT_PRINT_OUTPUT, -1);
	DEFINE_EVENT_TYPE(EVENT_PRINT_OUTPUT);

	BEGIN_EVENT_TABLE(FindResults, TextCtrl)
		EVT_COMMAND(wxID_ANY, EVENT_PRINT_OUTPUT, FindResults::onDisplay)
		EVT_LEFT_DCLICK(FindResults::onSelect)
	END_EVENT_TABLE();


	//-------------------------------------------------------//
	//---- class FindResults -------------------------------------//

	COMPONENT_METADATA(
		FindResults,
		_("FindResults"),
		_("FindResults console for the IDE"),
		_T("Ioannis Lilis <lilis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_WX_COMPONENT_(FindResults, TextCtrl, DockableComponent);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(FindResults, table)
	{
		conf::EnumStringProperty* docking = const_cast<conf::EnumStringProperty*>(
			conf::safe_prop_cast<const conf::EnumStringProperty>(
				table.GetProperty("docking")
			)
		);
		if (docking)
			docking->SetOption(4);
	}

	//-----------------------------------------------------------------------

	FindResults::FindResults(void)
	{
	}

	//-----------------------------------------------------------------------

	FindResults::~FindResults(void)
	{
	}

	//-----------------------------------------------------------------------

	wxWindow* FindResults::GenerateWindow(wxWindow* parent)
	{
		Create(parent,
			wxTE_MULTILINE | wxTE_READONLY |
			wxTE_RICH2 | wxTE_NOHIDESEL
		);

		wxTextAttr style = GetDefaultStyle();
		style.SetFont(wxFont(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
		SetDefaultStyle(style);

		return this;
	}

	//-----------------------------------------------------------------------

	wxWindow* FindResults::GetWindow(void)
	{
		return this;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(FindResults, void, Append, (const String& text))
	{
		wxCommandEvent event(EVENT_PRINT_OUTPUT);
		event.SetString(text);
		this->AddPendingEvent(event);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(FindResults, const String, GetText, (void))
	{
		return TextCtrl::GetValue();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(FindResults, void, Clear, (void))
	{
		TextCtrl::Clear();
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_STATIC(FindResults, View, _("/{10}View/{100}FindResults\tCtrl+Shift+F"), MT_MAIN, "")
	{
		EnsureVisibility("FindResults");
	}

	//-----------------------------------------------------------------------

	void FindResults::onDisplay(wxCommandEvent& event)
	{
		TextCtrl::AppendText(event.GetString());
		TextCtrl::Refresh();
	}

	//-----------------------------------------------------------------------

	void FindResults::onSelect(wxMouseEvent& event)
	{
		try
		{
			long lineStart	= (long)GetRange(0, GetInsertionPoint()).find_last_of(_T('\n')) +1;
			long lineEnd	= GetInsertionPoint() + (long)GetRange(GetInsertionPoint(), GetLastPosition()).find(_T('\n'));
			String text = GetRange(lineStart, lineEnd);
			SetSelection(lineStart, lineEnd);

			String filename;
			int lineno = -1;

			//lala.txt (line 5, cols 34-36)
			if (text.StartsWith(_T("Error")) || text.StartsWith(_T("Warning")))
			{
				std::vector<String> tokens;
				util::stringtokenizer(tokens, text, _T(","));	///< initally seperate by commas
				if (tokens.size() < 3)		///< should have three parts (type, source file, line and message)
					return;

				std::vector<String> inner_tokens;
				util::stringtokenizer(inner_tokens, tokens[2], _T(":."));

				filename = tokens[1].substr(7, tokens[1].length() -8);
				lineno = boost::lexical_cast<int>(inner_tokens[0].substr(6));
			}
			else
				return;

			DockableComponent::EnsureVisibility("EditorManager");
			if (lineno == -1)
				Call<void (const String&)>(this, "EditorManager", "OpenDocument")(filename);
			else
				Call<void (const String&, int)>(this, "EditorManager", "GotoDocument")(filename, lineno);
		}
		catch(...) { /* catch possible bad cast errors */}
	}

	//-----------------------------------------------------------------------
}
