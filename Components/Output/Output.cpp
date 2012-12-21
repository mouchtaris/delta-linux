/**
 *	Output.cpp
 *
 *	-- IDE Console Output --
 *
 *	Console output component for the IDE.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#include "Output.h"

#include "Streams.h"
#include "StringUtils.h"
#include "PropertyUtils.h"
#include "ExtProperties.h"

#include "Call.h"
#include "ComponentRegistry.h"
#include "ComponentConfigurationDialog.h"

#include "UndefineAllWin32Shit.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#ifndef NO_VLD
#include <vld.h>
#endif

namespace ide
{
	//-------------------------------------------------------//
	//---- Custom Events ------------------------------------//

	DECLARE_LOCAL_EVENT_TYPE(EVENT_PRINT_OUTPUT, -1);
	DEFINE_EVENT_TYPE(EVENT_PRINT_OUTPUT);

	BEGIN_EVENT_TABLE(Output, TextCtrl)
		EVT_COMMAND(wxID_ANY, EVENT_PRINT_OUTPUT, Output::onDisplay)
		EVT_LEFT_DCLICK(Output::onSelect)
	END_EVENT_TABLE();


	//-------------------------------------------------------//
	//---- class Output -------------------------------------//

	COMPONENT_METADATA(
		Output,
		_("Output"),
		_("Output console for the IDE"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_WX_COMPONENT_(Output, TextCtrl, DockableComponent);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(Output, table)
	{
		using namespace conf;

		EnumStringProperty* docking = const_cast<EnumStringProperty*>(
			safe_prop_cast<const EnumStringProperty>(table.GetProperty("docking"))
		);
		if (docking)
			docking->SetOption(4);
		
		wxFont defaultFont(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

		const String defaultCategory = _("Default");
		table.AddProperty("font", new FontProperty(_("Font"), defaultFont,
			_("The output's main font"), defaultCategory));
		table.AddProperty("bg_color", new ColorProperty(_("Background Color"),
			wxColour(255, 255, 255), _("The starting color for the output main background"), defaultCategory));
		table.AddProperty("fg_color", new ColorProperty(_("Foreground Color"),
			wxColour(0, 0, 0), _("The starting color for plain text main foreground"), defaultCategory));

		const String errors = _("Errors");
		table.AddProperty("error_font", new FontProperty(_("Font"), defaultFont, _("The output's error font"), errors));
		table.AddProperty("error_bg_color", new ColorProperty(_("Background Color"),
			wxColour(255, 255, 255), _("The starting color for the output error background"), errors));
		table.AddProperty("error_fg_color", new ColorProperty(_("Foreground Color"),
			wxColour(255, 0, 0), _("The starting color for plain text error foreground"), errors));

		const String warnings = _("Warnings");
		table.AddProperty("warning_font", new FontProperty(_("Warning Font"), defaultFont,
			_("The output's warning font"), warnings));
		table.AddProperty("warning_bg_color", new ColorProperty(_("Warning Background Color"),
			wxColour(255, 255, 255), _("The starting color for the output warning background"), warnings));
		table.AddProperty("warning_fg_color", new ColorProperty(_("Warning Foreground Color"),
			wxColour(238, 118, 0), _("The starting color for plain text warning foreground"), warnings));
	}

	//-----------------------------------------------------------------------

	Output::Output(void)
	{
	}

	//-----------------------------------------------------------------------

	Output::~Output(void)
	{
	}

	//-----------------------------------------------------------------------

	wxWindow* Output::GenerateWindow(wxWindow* parent)
	{
		using namespace conf;
		Create(parent, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxTE_NOHIDESEL);
		wxTextAttr style = GetDefaultStyle();
		style.SetFont(get_prop_value<FontProperty>(GetProperty("font")));
		style.SetBackgroundColour(get_prop_value<ColorProperty>(GetProperty("bg_color")));
		style.SetTextColour(get_prop_value<ColorProperty>(GetProperty("fg_color")));
		SetDefaultStyle(style);
		return this;
	}

	//-----------------------------------------------------------------------

	wxWindow* Output::GetWindow(void)
	{
		return this;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Output, void, Append, (const String& text))
	{
		wxCommandEvent event(EVENT_PRINT_OUTPUT);
		event.SetString(text);
		this->AddPendingEvent(event);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Output, void, AppendOrdered, (const String& text, const UIntList& order))
	{
		wxCommandEvent event(EVENT_PRINT_OUTPUT);
		event.SetString(OrderedText(text, order));
		this->AddPendingEvent(event);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Output, const String, GetText, (void))
	{
		return TextCtrl::GetValue();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Output, void, Clear, (void))
	{
		TextCtrl::Clear();
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_STATIC(Output, View, _("/{10}View/{100}Output\tCtrl+Shift+O"), MT_MAIN, "")
	{
		EnsureVisibility("Output");
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_STATIC(Output, Configure, _("/{2}Configure/Output..."), MT_MAIN, "")
	{
		ComponentConfigurationDialog().Configure(s_classId);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(Output, void, OnWorkspaceLoaded, (const Handle& workspace, const String& uri),
		"WorkspaceLoaded")
	{
		Clear();
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(Output, void, OnResourceWorkStarted, (const Handle& resource,
		const String& task, const UIntList& workId), "ResourceWorkStarted")
	{
		const String type = util::std2str(resource->GetClassId());
		const String& name  = Call<const String& (void)>(this, resource, "GetName")();
		const String delim = _T("----------------");
		const String space = _(" ");
		const String str = delim + space + task + _T(" started: ") + type + space + name + space + delim + _("\n");
		AppendOrdered(str, workId);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(Output, void, OnResourceWorkCompleted, (const Handle& resource,
		const String& task, const UIntList& workId), "ResourceWorkCompleted")
	{
		Component *workspace = ComponentRegistry::Instance().GetFocusedInstance("Workspace");
		assert(workspace);
		const Handle& rootResource = Call<const Handle& (void)>(this, workspace, "GetRootWorkingResource")();
		const String type = util::std2str(resource->GetClassId());
		const String& name = Call<const String& (void)>(this, resource, "GetName")();		
		const String delim = resource == rootResource ? _T("================") : _T("----------------");
		const String space = _(" ");
		const String str = delim + space + type + space + name + _T(": ") + task + _T(" completed ") + delim + _("\n");
		AppendOrdered(str, workId);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(Output, void, OnWorkStarted, (const std::string workspace, const Handle& root,
		const String& task), "WorkStarted")
	{
		Clear();
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(Output, void, OnWorkCanceled, (const std::string workspace, const Handle& root, 
		const String& task), "WorkCanceled")
	{
		const String delim = _T("================");
		const String str = delim + _T(" ") + task + _(" Canceled By User ") + delim + _("\n");
		const UIntList workId = UIntList(1, 1);
		AppendOrdered(str, workId);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(Output, void, OnCompilationMessage, (const std::string& caller, const UIntList& buildId,
		const String& type, const String& content, const String& file, uint line), "CompilationMessage")
	{
		String str;
		if (type == _T("Error") || type == _T("Warning"))
			str = String::Format((type + _T(", file '%s', line %d: %s\n")).c_str(), file, line, content);
		else if (type == _T("See original source"))
			str = String::Format((type + _T(", file '%s', line %d.\n")).c_str(), file, line);
		else
			str = content;
		AppendOrdered(str, buildId);
	}

	//-----------------------------------------------------------------------

	void Output::onDisplay(wxCommandEvent& event)
	{
		const wxTextAttr style = TextCtrl::GetDefaultStyle();
		const String str = event.GetString();
		String text;
		String::size_type buildIdEnd = str.find_first_of(_T(">"));
		if (buildIdEnd != String::npos)
			text = str.substr(buildIdEnd + 1);
		else
			text = str;
		std::string propertyPrefix;
		if (text.StartsWith(_T("Error")) || text.Matches(_T("*Runtime error*")))
			propertyPrefix = "error_";
		else if (text.StartsWith(_T("Warning")) || text.Matches(_T("*Runtime warning*")))
			propertyPrefix = "warning_";

		if (!propertyPrefix.empty()) {
			using namespace conf;
			wxTextAttr changed = style;
			changed.SetFont(get_prop_value<FontProperty>(GetProperty(std::string(propertyPrefix) + "font")));
			changed.SetBackgroundColour(get_prop_value<ColorProperty>(GetProperty(std::string(propertyPrefix) + "bg_color")));
			changed.SetTextColour(get_prop_value<ColorProperty>(GetProperty(std::string(propertyPrefix) + "fg_color")));
			TextCtrl::SetDefaultStyle(changed);
		}
		TextCtrl::AppendText(str);
		TextCtrl::Refresh();
		if (!propertyPrefix.empty())
			TextCtrl::SetDefaultStyle(style);
	}

	//-----------------------------------------------------------------------

	void Output::onSelect(wxMouseEvent& event)
	{
		try
		{
			long lineStart	= (long)GetRange(0, GetInsertionPoint()).find_last_of(_T('\n')) +1;
			long lineEnd	= GetInsertionPoint() + (long)GetRange(GetInsertionPoint(), GetLastPosition()).find(_T('\n'));
			String text		= GetRange(lineStart, lineEnd);
			if (text.empty())
				return;
			SetSelection(lineStart, lineEnd);

			String::size_type buildIdEnd = text.find_first_of(_T(">"));
			if (buildIdEnd != String::npos)
				text = text.substr(buildIdEnd + 1);

			String filename;
			int lineno;

			if (text.Matches(_T("*Error*")) || text.Matches(_T("*Warning*")) || text.Matches(_T("*See original source*")))
			{
				std::vector<String> tokens;
				util::stringtokenizer(tokens, text, _T(","));	///< initally seperate by commas
				if (tokens.size() < 3)		///< should have three parts (type, source file, line and message)
					return;

				std::vector<String> inner_tokens;
				util::stringtokenizer(inner_tokens, tokens[2], _T(":."));

				filename = tokens[1].substr(7, tokens[1].length() - 8);
				lineno = boost::lexical_cast<int>(inner_tokens[0].substr(6));
			}
			else if (text.Matches(_T("*Runtime error*")) || text.Matches(_T("*Runtime warning*")))
			{
				static const size_t source_header_size = strlen(" source '");
				static const size_t line_header_size = strlen(" line ");

				std::vector<String> tokens;
				util::stringtokenizer(tokens, text, _T(","));	///< initally seperate by commas
				if (tokens.size() < 3)		///< should have three parts (type and vm, source file, line and message)
					return;

				std::vector<String> inner_tokens;
				util::stringtokenizer(inner_tokens, tokens[2], _T(":"));

				filename = tokens[1].substr(source_header_size, tokens[1].size() - source_header_size - 1);
				lineno = boost::lexical_cast<int>(inner_tokens[0].substr(line_header_size));
			}
			else if (text.Matches(_T("*Stopped at line: *")))
			{
				std::vector<String> tokens;
				util::stringtokenizer(tokens, text, _T(","));	///< initally seperate by commas
				if (tokens.size() < 3)		///< should have three parts (line, vm, scope)
					return;

				std::vector<String> inner_tokens1;
				util::stringtokenizer(inner_tokens1, tokens[0], _T(" "));
			
				const String target = _T(" vm: ");
				if (!tokens[1].StartsWith(target))
					return;

				filename = tokens[1].substr(target.size());
				lineno = boost::lexical_cast<int>(inner_tokens1.back());
			}
			else if (text.StartsWith(_T("Debugging console: ")))
			{
				static const size_t size = strlen("Debugging console: ");
				DockableComponent::EnsureVisibility("EditorManager");
				Call<const Handle (const String&)>(this, "EditorManager", "OpenDocument")(text.substr(size));
				return;
			}
			else
				return;

			Call<void (const String&, int)>(this, "DeltaVM", "GotoSymbolicDocument")(filename, lineno);
		}
		catch(...) { /* catch possible bad cast errors */}
	}

	//-----------------------------------------------------------------------

	const String Output::OrderedText(const String& text, const UIntList& order)
	{
		if (order.empty())
			return text;
		else {
			String id;
			bool isFirst = true;
			BOOST_FOREACH(uint i, order) {
				if (isFirst)
					isFirst = false;
				else
					id += _(".");
				id += boost::lexical_cast<String>(i);
			}
			return id + _T(">") + text;
		}
	}

	//-----------------------------------------------------------------------
}