/**
 *	BuildOrder.cpp
 *
 *	-- IDE Console BuildOrder --
 *
 *	BuildOrder for the ordered build.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	June 2010
 */
#include "BuildOrder.h"

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

	DECLARE_LOCAL_EVENT_TYPE(EVENT_PRINT_BUILD_ORDER, -1);
	DEFINE_EVENT_TYPE(EVENT_PRINT_BUILD_ORDER);

	BEGIN_EVENT_TABLE(BuildOrder, TextCtrl)
		EVT_COMMAND(wxID_ANY, EVENT_PRINT_BUILD_ORDER, BuildOrder::onDisplay)
		EVT_LEFT_DCLICK(BuildOrder::onSelect)
	END_EVENT_TABLE();


	//-------------------------------------------------------//
	//---- class BuildOrder ---------------------------------//

	COMPONENT_METADATA(
		BuildOrder,
		_("Build Order"),
		_("Build order output console for the IDE"),
		_T("Ioannis Lilis <lilis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_WX_COMPONENT_(BuildOrder, TextCtrl, DockableComponent);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(BuildOrder, table)
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

	BuildOrder::BuildOrder(void)
	{
		ResetOrder();
	}

	//-----------------------------------------------------------------------

	BuildOrder::~BuildOrder(void)
	{
	}

	//-----------------------------------------------------------------------

	wxWindow* BuildOrder::GenerateWindow(wxWindow* parent)
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

	wxWindow* BuildOrder::GetWindow(void)
	{
		return this;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(BuildOrder, void, AppendOrdered, (const String& text, const UIntList& workId))
	{
		if (workId.empty()) {
			wxCommandEvent event(EVENT_PRINT_BUILD_ORDER);
			event.SetString(text);
			this->AddPendingEvent(event);
		}
		else if (working && !ProcessText(workId, text))
			messages.push_back(WorkMessage(TEXT, workId, text));
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(BuildOrder, void, Enter, (const UIntList& workId))
	{
		if (working && !workId.empty())
			if (!ProcessEnter(workId))
				messages.push_back(WorkMessage(ENTER, workId, String()));
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(BuildOrder, void, Exit, (const UIntList& workId))
	{
		if (working && !workId.empty()) {
			if (ProcessExit(workId))
				HandlePendingMessages();
			else
				messages.push_back(WorkMessage(EXIT, workId, String()));
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(BuildOrder, const String, GetText, (void))
	{
		return TextCtrl::GetValue();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(BuildOrder, void, Clear, (void))
	{
		TextCtrl::Clear();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(BuildOrder, void, ResetOrder, (void))
	{
		current.clear();
		while(!nextIdLevels.empty())
			nextIdLevels.pop();
		nextIdLevels.push(1);
		messages.clear();
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_STATIC(BuildOrder, Configure, _("/{2}Configure/Build Order..."), MT_MAIN, "")
	{
		ComponentConfigurationDialog().Configure(s_classId);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(BuildOrder, void, OnWorkspaceLoaded, (const Handle& workspace, const String& uri),
		"WorkspaceLoaded")
	{
		Clear();
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(BuildOrder, void, OnResourceWorkStarted, (const Handle& resource,
		const String& task, const UIntList& workId), "ResourceWorkStarted")
	{
		if (working) {	//Ingore pending messages from a WorkCanceled
			const String type = util::std2str(resource->GetClassId());
			const String& name  = Call<const String& (void)>(this, resource, "GetName")();
			const String delim = _T("----------------");
			const String space = _(" ");
			const String str = delim + space + task + _T(" started: ") + type + space + name + space + delim + _("\n");
			Enter(workId);
			AppendOrdered(str, workId);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(BuildOrder, void, OnResourceWorkCompleted, (const Handle& resource,
		const String& task, const UIntList& workId), "ResourceWorkCompleted")
	{
		if (working) {	//Ingore pending messages from a WorkCanceled
			Component *workspace = ComponentRegistry::Instance().GetFocusedInstance("Workspace");
			assert(workspace);
			const Handle& rootResource = Call<const Handle& (void)>(this, workspace, "GetRootWorkingResource")();
			const String type = util::std2str(resource->GetClassId());
			const String& name = Call<const String& (void)>(this, resource, "GetName")();		
			const String delim = resource == rootResource ? _T("================") : _T("----------------");
			const String space = _(" ");
			const String str = delim + space + type + space + name + _T(": ") + task + _T(" completed ") + delim + _("\n");
			AppendOrdered(str, workId);
			Exit(workId);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(BuildOrder, void, OnWorkStarted, (const std::string workspace, const Handle& root,
		const String& task), "WorkStarted")
	{
		Clear();
		working = true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(BuildOrder, void, OnWorkCompleted, (const std::string workspace, const Handle& root,
		const String& task), "WorkCompleted")
	{
		working = false;
		assert(nextIdLevels.size() == 1 && nextIdLevels.top() == 1 && current.empty() && messages.empty());
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(BuildOrder, void, OnWorkCanceled, (const std::string workspace, const Handle& root, 
		const String& task), "WorkCanceled")
	{
		const String delim = _T("================");
		const String str = delim + _T(" ") + task + _(" Canceled By User ") + delim + _("\n");
		const UIntList workId = UIntList(1, 1);
		ResetOrder();
		Enter(workId);
		AppendOrdered(str, workId);
		Exit(workId);
		working = false;
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(BuildOrder, void, OnCompilationMessage, (const std::string& caller, const UIntList& buildId,
		const String& type, const String& content, const String& file, uint line), "CompilationMessage")
	{
		const String str = type == _T("Error") || type == _T("Warning") ?
			String::Format((type + _T(", file '%s', line %d: %s\n")).c_str(), file, line, content) : content;
		AppendOrdered(str, buildId);
	}

	//-----------------------------------------------------------------------

	void BuildOrder::onDisplay(wxCommandEvent& event)
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
		if (text.StartsWith(_T("Error")))
			propertyPrefix = "error_";
		else if (text.StartsWith(_T("Warning")))
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

	void BuildOrder::onSelect(wxMouseEvent& event)
	{
		try
		{
			long lineStart	= (long)GetRange(0, GetInsertionPoint()).find_last_of(_T('\n')) +1;
			long lineEnd	= GetInsertionPoint() + (long)GetRange(GetInsertionPoint(), GetLastPosition()).find(_T('\n'));
			String text		= GetRange(lineStart, lineEnd);
			if (text.empty())
				return;
			SetSelection(lineStart, lineEnd);

			String::size_type workIdEnd = text.find_first_of(_T(">"));
			assert(workIdEnd != String::npos);
			text = text.substr(workIdEnd + 1);

			String filename;
			int lineno;

			if (text.StartsWith(_T("Error")) || text.StartsWith(_T("Warning")))
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
			else if (text.StartsWith(_T("Runtime error")) || text.StartsWith(_T("Runtime warning")))
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
			else if (text.StartsWith(_T("Stopped at line: ")))
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

	bool BuildOrder::ProcessEnter (const UIntList& workId)
	{
		assert(!workId.empty());
		if (current.size() + 1 == workId.size()) {
			UIntList::const_iterator i, j;
			for(i = current.begin(), j = workId.begin(); i != current.end() && *i == *j; ++i, ++j);
			if (i == current.end() && *j == nextIdLevels.top()) {
				current.push_back(*j);
				nextIdLevels.push(1);
				return true;
			}
		}
		return false;
	}

	//-----------------------------------------------------------------------

	bool BuildOrder::ProcessExit (const UIntList& workId)
	{
		assert(!workId.empty());
		if (current.size() == workId.size()) {
			UIntList::const_iterator i, j;
			for(i = current.begin(), j = workId.begin(); i != current.end() && *i == *j; ++i, ++j);
			if (i == current.end()) {
				current.pop_back();
				nextIdLevels.pop();
				assert(nextIdLevels.size() != 1 || current.empty() && messages.empty());
				if (nextIdLevels.size() != 1)
					nextIdLevels.top()++;
				return true;
			}
		}
		return false;
	}

	//-----------------------------------------------------------------------

	bool BuildOrder::ProcessText (const UIntList& workId, const String& text)
	{
		assert(!workId.empty());
		if (current.size() == workId.size()) {
			UIntList::const_iterator i, j;
			for(i = current.begin(), j = workId.begin(); i != current.end() && *i == *j; ++i, ++j);
			if (i == current.end()) {
				wxCommandEvent event(EVENT_PRINT_BUILD_ORDER);
				event.SetString(OrderedText(text, workId));
				this->AddPendingEvent(event);
				return true;
			}
		}
		return false;
	}

	//-----------------------------------------------------------------------

	void BuildOrder::HandlePendingMessages (void)
	{
		bool updated;
		do {
			updated = false;
			for(WorkMessageList::iterator i = messages.begin(); i != messages.end(); ++i) {
				MessageType type = i->get<0>();
				UIntList& workId = i->get<1>();
				if (type == ENTER)
					updated = ProcessEnter(workId);
				else if (type == EXIT)
					updated = ProcessExit(workId);
				else
					updated = ProcessText(workId, i->get<2>());
				if (updated) {
					messages.erase(i);
					break;
				}
			}
		} while(updated);
	}

	//-----------------------------------------------------------------------

	const String BuildOrder::OrderedText(const String& text, const UIntList& workId)
	{
		if (workId.empty())
			return text;
		else {
			String id;
			bool isFirst = true;
			BOOST_FOREACH(uint i, workId) {
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
