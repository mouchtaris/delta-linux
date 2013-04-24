/**
 *	Editor.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 *	Extended for "Goto Defintion" support, March 2011, A. Savidis
 */

#include "Editor.h"
#include "FileSelectionDialog.h"
#include "LanguageDescriptions.h"
#include "ExtProperties.h"
#include "PropertyUtils.h"
#include "EditorProperties.h"
#include "Streams.h"
#include "CommonGUIUtils.h"
#include "IDEDialogs.h"

#include "DelayedCaller.h"
#include "FileChangeWatcher.h"
#include "GenericDialogs.h"
#include "ComponentConfigurationDialog.h"
#include "ComponentRegistry.h"

#include "EditorMain/EditorWindow.h"
#include "EditorInterfaces/LanguageModuleIface.h"

#include "Call.h"

#include "StringUtils.h"

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include <wx/filename.h>
#include <wx/utils.h>

#include "Icons/toggle_view.xpm"
#include "Icons/line_numbers.xpm"
#include "Icons/goto_line.xpm"

using namespace editor;

#ifndef NO_VLD
#include <vld.h>
#endif

namespace ide {

////////////////////////////////////////////////////////////////////////

#define INCREMENTAL_EDITING

////////////////////////////////////////////////////////////////////////
// Editor class
//
COMPONENT_METADATA(
	Editor,
	_("Editor"),
	_("Edits stuff (i.e. notepad with some obscure colors)"),
	_T("Giannis Georgalis <jgeorgal@ics.forth.gr>"),
	_T("0.1a")
);
IMPLEMENT_WX_IDE_COMPONENT(Editor, wxSplitterWindow);

COMPONENT_SET_PROPERTIES_FUNCTION(Editor, table)
{
	using namespace conf;

	////////////////////////////////////////////////////////////////////
	// Basic editor properties
	//
	wxFont dFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	table.AddProperty("font", new FontProperty(_("Font"), dFont, _("The editor's main font")));

	table.AddProperty("tab_width", new IntProperty(_("Tab width"),
		4, _("The tab width in space characters")));

	table.AddProperty("line_numbering", new BoolProperty(_("Line Numbering"),
		false, _("The starting status for the line numbering")));

	table.AddProperty("bg_color", new ColorProperty(_("Background Color"),
		wxColour(255, 255, 255), _("The starting color for the editor background")));

	table.AddProperty("fg_color", new ColorProperty(_("Foreground Color"),
		wxColour(0, 0, 0), _("The starting color for plain text foreground")));

	////////////////////////////////////////////////////////////////////
	// Brace properties
	//
	StyleProperty* matchedProp = new StyleProperty(
			_("Matched braces"),
			wxColour(0, 100, 70), wxColour(200, 200, 255),
			_("Style for matched brace characters: ()[]{}<>")
		);
	table.AddProperty("matched_braces", matchedProp);

	StyleProperty* unmatchedProp = new StyleProperty(
			_("Unmatched braces"),
			wxColour(250, 0, 0), wxColour(255, 200, 200),
			_("Style for unmatched brace characters: ()[]{}<>")
		);
	table.AddProperty("unmatched_braces", unmatchedProp);

	////////////////////////////////////////////////////////////////////
	// Caret properties
	//
	AggregateProperty* caretProp = new AggregateProperty(_("Caret"), _("caret settings"));
	caretProp->AddProperty("fg_color", new ColorProperty(_("Color"),
		wxColour(0, 0, 0), _("The starting color for the caret")));
	caretProp->AddProperty("line_colored", new BoolProperty(_("Line Highlighted"),
		false, _("The starting status for the caret line highlighting")));
	caretProp->AddProperty("line_bg_color", new ColorProperty(_("Line Color"),
		wxColour(255, 240, 240), _("The starting color for the caret line")));
	caretProp->AddProperty("width", new IntRangeProperty(_("Width"),
		0, 3, 1, _("The starting width of the caret in pixels")));
	table.AddProperty("caret", caretProp);

	////////////////////////////////////////////////////////////////////
	// Language properties
	//
	if (Property* prop = LanguageDescriptions::Instance().GetHighlightingProperties())
		table.AddProperty("highlighting", prop);
}

//**********************************************************************

enum TimerIds {
	TIMER_PARSE,
	TIMER_CANCELTOOLTIP,
};

//**********************************************************************

BEGIN_EVENT_TABLE(Editor, wxSplitterWindow)
	EVT_SCI_UPDATEUI(wxID_ANY, Editor::onUpdateUI)
	EVT_SCI_SAVEPOINTREACHED(wxID_ANY, Editor::onSavePointReached)
	EVT_SCI_SAVEPOINTLEFT(wxID_ANY, Editor::onLeftSavePoint)
	EVT_SCI_CHARADDED(wxID_ANY, Editor::onCharAdded)
	EVT_SCI_MODIFIED(wxID_ANY, Editor::onTextChanged)
	EVT_SCI_MARGINCLICK(wxID_ANY, Editor::onMarginClicked)
	EVT_SCI_DWELLSTART(wxID_ANY, Editor::onDwellStart)
	EVT_SCI_DWELLEND(wxID_ANY, Editor::onDwellEnd)
	EVT_SCI_HOTSPOT_CLICK(wxID_ANY, Editor::onHotspotClicked)
	EVT_SCI_USERLISTSELECTION(wxID_ANY, Editor::onUserListSelection)
	EVT_LEFT_UP(Editor::onMouseLeftUp)
	EVT_MOTION(Editor::onMouseMove)
	EVT_TIMER(TIMER_PARSE, Editor::onTimerExpired)
	EVT_TIMER(TIMER_CANCELTOOLTIP, Editor::onTooltipCancelTimerExpired)
	EVT_MENU(wxID_ANY, Editor::onAcceleratorCommand)
END_EVENT_TABLE();

//**********************************************************************

EXPORTED_SIGNAL(Editor, EditorClosing, (void));

EXPORTED_SIGNAL(Editor, EditorMarginMarksClicked, (int line));

EXPORTED_SIGNAL(Editor, FileSavePointReached, (void));
EXPORTED_SIGNAL(Editor, FileLeftSavePoint, (void));
EXPORTED_SIGNAL(Editor, FileNameChanged, (const String& newUri));
EXPORTED_SIGNAL(Editor, FileOpened, (const String& uri));

EXPORTED_SIGNAL(Editor, EditCursorChanged, (int line, int column, bool overtype));
EXPORTED_SIGNAL(Editor, EditLinesChangedBy, (int fromLine, int offset));
EXPORTED_SIGNAL(Editor, EditMouseOnSymbol, (int startPos, int endPos, int symbolType, const String& desc));
EXPORTED_SIGNAL(Editor, EditMouseOnSymbolCancel, (void));
EXPORTED_SIGNAL(Editor, EditSelectionChanged, (int startPos, int endPos));
EXPORTED_SIGNAL(Editor, EditTextChanged, (int pos, int length, bool added));
EXPORTED_SIGNAL(Editor, UserListSelected, (int listType, const String& selection));

//**********************************************************************

Editor::Editor (void) : m_editor(0), m_prevStartPos(0), m_prevEndPos(0),
	m_marginDragStartPos(-1), m_temp(true), m_workspaceResource(), m_changeWatcher(0), m_reloadPending(false)
{
}

//**********************************************************************

Editor::~Editor (void)
{
	// We cannot let the visualization window be destroyed automatically
	//
	m_editor->GetLangIface()->DestroyContentVisualization();
}

//**********************************************************************

EXPORTED_STATIC(Editor, void, Initialize, (void))
{
	DBGOUT << "LanguageDescriptions::Initialize();" << DBGENDL;
}

//**********************************************************************

EXPORTED_STATIC(Editor, void, CleanUp, (void))
{
	DBGOUT << "LanguageDescriptions::CleanUp();" << DBGENDL;
}

//**********************************************************************

EXPORTED_MEMBER(Editor, void, Open, (const String& uri))
{
	if (!m_editor->Open(uri))
		return;

	m_temp = false;
	SetLanguageSettings(uri);
	sigFileNameChanged(this, uri);
	sigFileOpened(this, uri);
	
	CancelFileModificationWatcher();
	m_changeWatcher = util::FileChangeWatcher::Instance().Register(
		uri,
		boost::bind(&Editor::OnFileModification, this)
	);
}

EXPORTED_MEMBER(Editor, void, Close, (void))
{
	sigEditorClosing(this);
	this->Component::Destroy();
}

EXPORTED_MEMBER(Editor, const String&, GetURI, (void))
	{ return m_editor->GetURI(); }

EXPORTED_MEMBER(Editor, void, SetURI, (const String& uri))
{
	m_editor->SetURI(uri);
	SetLanguageSettings(uri);
	sigFileNameChanged(this, uri);
}

EXPORTED_MEMBER(Editor, void, SetLanguageSettings, (const String& uri))
{
	LanguageDescriptions::Instance().SetLanguage(uri, m_editor);

	conf::PropertyIdVec changed;
	changed.push_back("highlighting");
	changed.push_back("bg_color");
	changed.push_back("fg_color");
	changed.push_back("caret");
	this->ComponentAppliedChangedProperties(GetPropertyTable(), changed);

	m_editor->GetLangIface()->SetGotoGlobalDefinitionOfFileCallback(&Editor::GotoGlobalDefinitionOfFile, this);
	m_editor->GetLangIface()->SetGotoPositionCallback(&Editor::GotoPosition, this);
	
	m_editor->GetLangIface()->ContentSetup();
}

EXPORTED_MEMBER(Editor, bool, Save, (void))
{
	bool saved = m_editor->Save();
	if (m_temp && saved)
		m_temp = false;
	return saved;
}

EXPORTED_MEMBER(Editor, bool, SaveAs, (const String& uri))
{
	const String oldURI = m_editor->GetURI();
	if (m_editor->SaveAs(uri)) {
		if (m_editor->GetURI() != oldURI) {
			SetLanguageSettings(uri);
			sigFileNameChanged(this, uri);
		}
		m_temp = false;
		return true;
	}
	return false;
}

EXPORTED_MEMBER(Editor, bool, SaveToDisk, (const String& uri))
	{ return m_editor->SaveToDisk(uri); }

EXPORTED_MEMBER(Editor, bool, IsTemp, (void))
	{ return m_temp; }

EXPORTED_MEMBER(Editor, void, SetAssociatedComponent, (const Handle& resource))
	{ m_workspaceResource = resource; }

EXPORTED_MEMBER(Editor, const Handle&, GetAssociatedComponent, (void))
	{ return m_workspaceResource; }

//**********************************************************************

EXPORTED_MEMBER(Editor, const String, GetText, (void))
	{ return m_editor->GetText(); }

EXPORTED_MEMBER(Editor, const String, GetSelectedText, (void))
	{ return m_editor->GetSelectedText(); }

EXPORTED_MEMBER(Editor, const String, GetLineText, (int line))
	{ return m_editor->GetLine(line); }

EXPORTED_MEMBER(Editor, const String, GetTextRange, (int startPos, int endPos))
	{ return m_editor->GetTextRange(startPos, endPos); }

EXPORTED_MEMBER(Editor, void, SetText, (const String& text))
	{ m_editor->SetText(text); }

EXPORTED_MEMBER(Editor, void, InsertText, (int pos, const String& text))
	{ m_editor->InsertText(pos, text); }

EXPORTED_MEMBER(Editor, void, AppendText, (const String& text))
	{ m_editor->AppendText(text); }

EXPORTED_MEMBER(Editor, void, ClearText, (void))
	{ m_editor->ClearAll(); }

EXPORTED_MEMBER(Editor, void, ReplaceSelection, (const String& text))
	{ m_editor->ReplaceSelection(text); }

EXPORTED_MEMBER(Editor, bool, IsReadOnly, (void))
	{ return m_editor->GetReadOnly(); }

EXPORTED_MEMBER(Editor, void, SetReadOnly, (bool value))
	{ m_editor->SetReadOnly(value); }

//**********************************************************************

EXPORTED_MEMBER(Editor, int, SearchNext, (int flags, const String& text))
	{ return m_editor->SearchNext(flags, text); }

EXPORTED_MEMBER(Editor, int, SearchPrev, (int flags, const String& text))
	{ return m_editor->SearchPrev(flags, text); }

//**********************************************************************

EXPORTED_MEMBER(Editor, void, Cut, (void)) { m_editor->Cut(); }
EXPORTED_MEMBER(Editor, void, Copy, (void)) { m_editor->Copy(); }
EXPORTED_MEMBER(Editor, void, Paste, (void)) { m_editor->Paste(); }
EXPORTED_MEMBER(Editor, void, Clear, (void)) { m_editor->Clear(); }
EXPORTED_MEMBER(Editor, bool, CanPaste, (void)) { return m_editor->CanPaste(); }

EXPORTED_MEMBER(Editor, void, CopyRange, (int startPos, int endPos))
	{ m_editor->CopyRange(startPos, endPos); }

EXPORTED_MEMBER(Editor, void, CopyText, (const String& text))
	{ m_editor->CopyText((int) text.size(), text); }

//**********************************************************************

EXPORTED_MEMBER(Editor, void, SetModified, (bool val)) { m_editor->SetModified(val); }
EXPORTED_MEMBER(Editor, bool, IsModified, (void)) { return m_editor->IsModified(); }
EXPORTED_MEMBER(Editor, void, Undo, (void)) { m_editor->Undo(); }
EXPORTED_MEMBER(Editor, bool, CanUndo, (void)) { return m_editor->CanUndo(); }
EXPORTED_MEMBER(Editor, void, Redo, (void)) { m_editor->Redo(); }
EXPORTED_MEMBER(Editor, bool, CanRedo, (void)) { return m_editor->CanRedo(); }

EXPORTED_MEMBER(Editor, void, BeginUndoAction, (void)) { m_editor->BeginUndoAction(); }
EXPORTED_MEMBER(Editor, void, EndUndoAction, (void)) { m_editor->EndUndoAction(); }
EXPORTED_MEMBER(Editor, void, ClearUndoHistory, (void)) { m_editor->EmptyUndoBuffer(); }

//**********************************************************************

EXPORTED_MEMBER(Editor, void, GotoPos, (int pos))
{
	m_editor->GotoPos(pos);
	m_editor->SetFocus();
}

EXPORTED_MEMBER(Editor, int, GetCurrentPos, (void))
	{ return m_editor->GetCurrentPos(); }

//**********************************************************************

EXPORTED_MEMBER(Editor, void, GotoLine, (int line))
{
	m_editor->GotoLine(line - 1);
	m_editor->SetFocus();
}

EXPORTED_MEMBER(Editor, int, GetCurrentLine, (void))
	{ return m_editor->LineFromPosition(m_editor->GetCurrentPos()) + 1; }

EXPORTED_MEMBER(Editor, int, GetLineFromPos, (int pos))
	{ return m_editor->LineFromPosition(pos) + 1; }

//**********************************************************************

EXPORTED_MEMBER(Editor, void, GotoColumn, (int column))
{
	int currLine = this->GetCurrentLine();
	int lineStart = m_editor->PositionFromLine(currLine);
	int lineEnd = m_editor->GetLineEndPosition(currLine);
	int newPosOffset = std::min(lineEnd - lineStart, column - 1);
	m_editor->GotoPos(lineStart + newPosOffset);
	m_editor->SetFocus();
}

EXPORTED_MEMBER(Editor, int, GetCurrentColumn, (void))
	{ return m_editor->GetColumn(m_editor->GetCurrentPos()) + 1; }

//**********************************************************************

EXPORTED_MEMBER(Editor, void, EnsureLineVisible, (int line))
	{ m_editor->EnsureVisibleEnforcePolicy(line); }

EXPORTED_MEMBER(Editor, const SerializedEditorAST, GetAST, (void))
	{ return m_editor->GetLangIface()->GetAST(); }

EXPORTED_MEMBER(Editor, void, SharedWriteAST, (void))
	{ m_editor->GetLangIface()->SharedWriteAST(); }

EXPORTED_MEMBER(Editor, bool, GetOvertype, (void))
	{ return m_editor->GetOvertype(); }

EXPORTED_MEMBER(Editor, int, GetLineCount, (void))
	{ return m_editor->GetLineCount(); }

EXPORTED_MEMBER(Editor, void, SelectAll, (void))
	{ m_editor->SelectAll(); }

EXPORTED_MEMBER(Editor, void, SelectRange, (int start, int end))
{
	//m_editor->GotoPos(end);
	m_editor->SetSelection(start, end);
}

EXPORTED_MEMBER(Editor, void, ShowTooltip, (int start, const String& text))
{
	m_editor->CallTipShow(start, text);
	m_tooltipCancelTimer.Start(5000, wxTIMER_ONE_SHOT);
}

EXPORTED_MEMBER(Editor, void, UserListShow, (int listType, const StringList& items))
{
	String itemList;
	for (StringList::const_iterator i = items.begin(); i != items.end(); ++i) {
		if (i != items.begin())
			itemList += + _T('\a');
		itemList += *i;
	}
	m_editor->UserListShow (listType, itemList);
}

EXPORTED_MEMBER(Editor, void, SetExtraLibraryDefinitions, (const StringList& keywords))
{
	m_editor->GetLangIface()->SetExtraLibraryDefinitions(keywords);
}

EXPORTED_MEMBER(Editor, void, SetByteCodeLoadingPath, (const String& path))
{
	m_editor->GetLangIface()->SetByteCodeLoadingPath(util::str2std(path));
}

//**********************************************************************

EXPORTED_MEMBER(Editor, void, InsertBreakpoint, (int line, bool enabled))
{
	void (editor::EditorWindow::*func)(int) = enabled ? 
		&editor::EditorWindow::InsertEnabledBreakpointToLine :
		&editor::EditorWindow::InsertDisabledBreakpointToLine;
	(m_editor->*func)(line - 1);
}

EXPORTED_MEMBER(Editor, void, RemoveBreakpoint, (int line, bool enabled))
{
	void (editor::EditorWindow::*func)(int) = enabled ? 
		&editor::EditorWindow::RemoveEnabledBreakpointFromLine :
		&editor::EditorWindow::RemoveDisabledBreakpointFromLine;
	(m_editor->*func)(line - 1);
}

//**********************************************************************

EXPORTED_MEMBER(Editor, void, InsertBookmark, (int line))
	{ m_editor->InsertBookmarkToLine(line - 1); }

EXPORTED_MEMBER(Editor, void, RemoveBookmark, (int line))
	{ m_editor->RemoveBookmarkFromLine(line - 1); }

EXPORTED_MEMBER(Editor, void, ToggleBookmark, (int line))
	{ m_editor->ToggleBookmarkAtLine(line - 1); }

EXPORTED_MEMBER(Editor, bool, HasBookmark, (int line))
	{ return m_editor->HasBookmarkAtLine(line - 1); }

EXPORTED_MEMBER(Editor, int, NextBookmarkFromLine, (int line))
	{ return m_editor->NextBookmarkFromLine(line - 1) + 1; }

EXPORTED_MEMBER(Editor, int, PrevBookmarkFromLine, (int line))
	{ return m_editor->PrevBookmarkFromLine(line - 1) + 1; }

EXPORTED_MEMBER(Editor, void, ClearBookmarks, (void))
	{ return m_editor->ClearBookmarks(); }

EXPORTED_MEMBER(Editor, const std::vector<int>, GetAllEnabledBreakpoints, (int fromLine))
	{ return m_editor->GetAllEnabledBreakpointsFromLine(fromLine); }

EXPORTED_MEMBER(Editor, const std::vector<int>, GetAllDisabledBreakpoints, (int fromLine))
	{ return m_editor->GetAllDisabledBreakpointsFromLine(fromLine); }

EXPORTED_MEMBER(Editor, const std::vector<int>, GetAllBookmarks, (int fromLine))
	{ return m_editor->GetAllBookmarksFromLine(fromLine); }

EXPORTED_MEMBER(Editor, void, SetBreakpointArrow, (int line)) { m_editor->SetYellowArrowToLine(line - 1); }
EXPORTED_MEMBER(Editor, void, RemoveBreakpointArrow, (void)) { m_editor->RemoveYellowArrow(); }

EXPORTED_MEMBER(Editor, void, SetTraceArrow, (int line)) { m_editor->SetGreenArrowToLine(line - 1); }
EXPORTED_MEMBER(Editor, void, RemoveTraceArrow, (void)) { m_editor->RemoveGreenArrow(); }

//**********************************************************************

EXPORTED_MEMBER(Editor, bool, GotoGlobalDefinition, (const String& id)) 
	{ return m_editor->GetLangIface()->GotoGlobalDefinition(util::str2std(id)); }

editor::LanguageModuleIface::GotoDefinitionResult Editor::GotoGlobalDefinitionOfFile (
		const std::string&	file,
		const std::string&	id,
		uint				stage,
		void*				closure
	) {

	Editor*	editor		=	(Editor*) closure;
	Handle	currScript  =	editor->GetAssociatedComponent();
	if (!currScript)
		return editor::LanguageModuleIface::GotoDefinitionFailedCurrentFileOutsideWorkspace;

	HandleList	usedScripts	=	Call<const HandleList (const String&, uint)>(
										editor, 
										currScript, 
										"FindScriptsOfUsedByteCodeFile"
									)(util::std2str(file), stage);
	if (usedScripts.empty())
		return editor::LanguageModuleIface::GotoDefinitionFailedFileNotFound;
	else
	if (usedScripts.size() != 1)
		return editor::LanguageModuleIface::GotoDefinitionFailedMultipleFilesFound;
	else {
		String	uri			= Call<const String& (void)>(editor, usedScripts.front(), "GetURI")();
		bool	wasOpened	= true;
		Handle	usedEditor	= Call<const Handle& (const String&)>(editor, "EditorManager", "GetEditor")(uri);

		if (!usedEditor) {	// Was not opened?
			wasOpened	= false;
			usedEditor	= Call<const Handle& (const String&)>(editor, "EditorManager", "OpenDocument")(uri);
			if (!usedEditor)	//failed to open editor
				return editor::LanguageModuleIface::GotoDefinitionFailedDefinitionNotFound;
			Call<void (const Handle&)>(editor, usedEditor, "SetAssociatedComponent")(usedScripts.front());
		}

		if (Call<bool (const String&)>(editor, usedEditor, "GotoGlobalDefinition")(util::std2str(id))) {
			Call<void (const Handle&)>(editor, "EditorManager", "FocusEditor")(usedEditor);
			return editor::LanguageModuleIface::GotoDefinitionSucceeded;
		}
		else {
			if (!wasOpened)	// Should close it as was opened just for got def. 
				Call<void (const Handle&)>(editor, "EditorManager", "CloseEditor")(usedEditor);

			Call<void (const Handle&)>(editor, "EditorManager", "FocusEditor")(editor);
			return editor::LanguageModuleIface::GotoDefinitionFailedDefinitionNotFound;
		}
	}
}

//**********************************************************************
// We should firstly reposition and then focus (for some reason this is required).

void Editor::GotoPosition (int pos, void* closure) {
	Editor*	editor	=	(Editor*) closure;
	editor->m_editor->GotoPos(pos);
	if (Call<const Handle& (void)>(editor, "EditorManager", "GetFocusedEditor")() != Handle(editor))
		Call<void (const Handle&)>(editor, "EditorManager", "FocusEditor")(editor);
}

//**********************************************************************
// User commands
//
EXPORTED_IMAGE(Editor, "toggle_view", toggle_view_xpm);
EXPORTED_CMD_MEMBER(Editor, ToggleContentView, _("/{1}Edit/{200}--Toggle content view\tCtrl+Alt+V"), MT_MAIN, "toggle_view")
	{ this->toggleVisualizeContent(); }

EXPORTED_IMAGE(Editor, "line_numbers", line_numbers_xpm);
EXPORTED_CMD_MEMBER(Editor, ToggleLineNumbers, _("/{1}Edit/{210}Toggle line numbers\tCtrl+Alt+N--"), MT_MAIN, "line_numbers")
{
	if (m_editor->GetMarginWidth(EditorWindow::MARGIN_LINENUMBER))
		m_editor->SetMarginWidth(EditorWindow::MARGIN_LINENUMBER, 0);
	else {
		int width = m_editor->TextWidth(wxSCI_STYLE_LINENUMBER, _T("99999"));	//width for 5 digits
		m_editor->SetMarginWidth(EditorWindow::MARGIN_LINENUMBER, width);
	}
}

EXPORTED_IMAGE(Editor, "goto_line", goto_line_xpm);
EXPORTED_CMD_MEMBER(Editor, PromptGotoLine, _("/{1}Edit/{900}--Go to line...\tCtrl+G"), MT_MAIN, "goto_line")
{
	int lineCount = this->GetLineCount(), currLine = this->GetCurrentLine();
	int value = gui::getInteger(this, _("Goto line..."), _("Line number"), 1, lineCount, currLine);
	if (value != lineCount + 1)
		this->GotoLine(value);
}

EXPORTED_CMD_STATIC(Editor, Configure, _("/{2}Configure/Editor..."), MT_MAIN, "")
	{ ComponentConfigurationDialog().Configure(s_classId); }

//**********************************************************************

EXPORTED_SLOT_MEMBER(Editor, void, OnFileRenamed, (const Handle& file, const String& oldUri, const String& newUri),
	"FileRenamed")
{
	if (wxFileName(oldUri).SameAs(GetURI()))
		SetURI(newUri);
}

//**********************************************************************

wxWindow* Editor::GenerateWindow (wxWindow* parent)
{
	this->Create(parent, wxID_ANY);

	m_editor = new EditorWindow(this);
	m_editor->Connect(wxEVT_LEFT_UP, wxMouseEventHandler(Editor::onMouseLeftUp), 0, this);
	m_editor->Connect(wxEVT_MOTION, wxMouseEventHandler(Editor::onMouseMove), 0, this);
	this->SetMinimumPaneSize(100);
	this->SetSashGravity(0.8);
	this->wxSplitterWindow::Initialize(m_editor);
	//this->SplitHorizontally(m_editor, m_editorView);

	m_timer.SetOwner(this, TIMER_PARSE);
	m_tooltipCancelTimer.SetOwner(this, TIMER_CANCELTOOLTIP);

	if (conf::get_prop_value<conf::BoolProperty>(GetProperty("line_numbering")))
		ToggleLineNumbers();

	return this;
}

//**********************************************************************

wxWindow* Editor::GetWindow (void)
{
	return this;
}

//**********************************************************************

void Editor::ComponentDestroyed (void)
{
	CancelFileModificationWatcher();
}

//**********************************************************************

void Editor::ComponentAppliedChangedProperties (const conf::PropertyTable& old, const conf::PropertyIdVec& changed)
{
	using namespace conf;

	////////////////////////////////////////////////////////////////////
	// Set basic editor properties
	//
	wxFont font = get_prop_value<FontProperty>(this->GetProperty("font"));
	m_editor->StyleSetFont(wxSCI_STYLE_DEFAULT, font);

	m_editor->SetTabWidth(get_prop_value<IntProperty>(this->GetProperty("tab_width")));

	////////////////////////////////////////////////////////////////////
	// Clear previous styles
	//
	m_editor->StyleClearAll();

	////////////////////////////////////////////////////////////////////
	// Highlighting properties. Should be the first styling property
	// because it may clear all styles (so they must follow to reappear)
	//
	const Property* highlighting = this->GetProperty("highlighting");

	if (std::find(changed.begin(), changed.end(), "bg_color") != changed.end()) {
		wxColour bg = get_prop_value<ColorProperty>(this->GetProperty("bg_color"));
		const StyleProperty newBgColor(String(), wxNullColour, bg);
		const AggregateProperty* languagesProp = safe_prop_cast<const AggregateProperty>(highlighting);
		BOOST_FOREACH(const PropertyMap::value_type& prop, languagesProp->GetPropertyMap()) {
			const AggregateProperty* styleProp = safe_prop_cast<const AggregateProperty>(prop.second);
			BOOST_FOREACH(const PropertyMap::value_type& entry, styleProp->GetPropertyMap())
				set_prop_value<StyleProperty>(entry.second, newBgColor);
		}
		m_editor->StyleSetBackground(wxSCI_STYLE_DEFAULT, bg);
		m_editor->SetFoldMarginColour(true, bg);
		m_editor->SetFoldMarginHiColour(true, bg);
		m_editor->StyleClearAll();
	}
	
	if (std::find(changed.begin(), changed.end(), "fg_color") != changed.end()) {
		wxColour fg = get_prop_value<ColorProperty>(this->GetProperty("fg_color"));
		m_editor->StyleSetForeground(wxSCI_STYLE_DEFAULT, fg);
		m_editor->StyleClearAll();
	}

	if (std::find(changed.begin(), changed.end(), "caret") != changed.end()) {
		const AggregateProperty* caret = safe_prop_cast<const AggregateProperty>(this->GetProperty("caret"));
		m_editor->SetCaretForeground(get_prop_value<ColorProperty>(caret->GetProperty("fg_color")));
		m_editor->SetCaretLineVisible(get_prop_value<BoolProperty>(caret->GetProperty("line_colored")));
		m_editor->SetCaretLineBackground(get_prop_value<ColorProperty>(caret->GetProperty("line_bg_color")));
		m_editor->SetCaretWidth(get_prop_value<IntRangeProperty>(caret->GetProperty("width")));
	}

	////////////////////////////////////////////////////////////////////
	// Brace properties
	//
	safe_prop_cast<const StyleProperty>(this->GetProperty("matched_braces"))->Apply(m_editor, wxSCI_STYLE_BRACELIGHT);
	safe_prop_cast<const StyleProperty>(this->GetProperty("unmatched_braces"))->Apply(m_editor, wxSCI_STYLE_BRACEBAD);

	LanguageDescriptions::Instance().SetLanguageHighlighting(this->GetURI(), m_editor, highlighting);

	////////////////////////////////////////////////////////////////////
	// Some additional editor properties
	// Set them again since StyleClearAll was called.
	//

	//TODO: add this code when we want to support the hotspots
	//BOOST_FOREACH(int hotspot, m_editor->GetLangIface()->GetHotspots())
	//	m_editor->StyleSetHotSpot(hotspot, true);

	m_editor->ShowFoldingView(true);
}

//**********************************************************************

void Editor::onUpdateUI (wxScintillaEvent& event) {

	m_editor->UpdateMatchedBrace();

	// i-sense call.
	m_editor->GetLangIface()->CursorRepositioned(this->GetCurrentPos());

	DBGOUT << "Position: " << this->GetCurrentPos() << DBGENDL;
	DBGOUT << "Focus node: " << m_editor->GetLangIface()->GetCursorFocusInformation() << DBGENDL;

	sigEditCursorChanged(
		this, 
		this->GetCurrentLine(), 
		this->GetCurrentColumn(), 
		this->GetOvertype()
	);

	int startPos, endPos;
	m_editor->GetSelection(&startPos, &endPos);
	
	if (m_prevStartPos != m_prevEndPos || startPos != endPos) {
		sigEditSelectionChanged(startPos, endPos);
		m_prevStartPos = startPos, m_prevEndPos = endPos;
	}
}

//**********************************************************************

void Editor::onSavePointReached (wxScintillaEvent& event)
{
	if (!m_editor->IsReloading())
		sigFileSavePointReached(this);
}

//**********************************************************************

void Editor::onLeftSavePoint (wxScintillaEvent& event)
{
	if (!m_editor->IsReloading())
		sigFileLeftSavePoint(this);
}

//**********************************************************************

void Editor::onCharAdded (wxScintillaEvent& event)
{
	m_editor->GetLangIface()->CharacterAdded(event.GetKey());
}

//**********************************************************************

void Editor::onTextChanged (wxScintillaEvent& event)
{
	if (m_editor->GetURI().empty()) // During load
		return;

	if (event.GetModificationType() & wxSCI_MOD_INSERTTEXT)
		this->textAddedOrRemoved(event.GetPosition(), event.GetLength(), 1, event.GetLinesAdded());
	else if (event.GetModificationType() & wxSCI_MOD_DELETETEXT)
		this->textAddedOrRemoved(event.GetPosition(), event.GetLength(), -1, event.GetLinesAdded());
}

//**********************************************************************

void Editor::onMarginClicked (wxScintillaEvent& event)
{
	int pos = event.GetPosition();
	int line = m_editor->LineFromPosition(pos) + 1;
	if (event.GetMargin() == EditorWindow::MARGIN_LINENUMBER) {
		this->SelectRange(pos, m_editor->PositionFromLine(line));
		m_marginDragStartPos = pos;	//handled at MouseMove and MouseLeftUp
	}
	else if (event.GetMargin() == EditorWindow::MARGIN_EDITORMARKS)
		sigEditorMarginMarksClicked(this, line);
	else if (event.GetMargin() == EditorWindow::MARGIN_FOLDING)
		m_editor->RequestToggleFoldingAtPos(pos);
}

//**********************************************************************
// TODO(AS): how is the debugger taking control of the dwell event
// during debugging?

void Editor::onDwellStart (wxScintillaEvent& event)
{
	this->cancelDwellInfo();

	if (event.GetPosition() == -1 || !m_editor->IsShownOnScreen())
		return;

	// Ugly hack to fix the tooltip problem(TM)
	//
	if (::wxFindWindowAtPointer(::wxGetMousePosition()) != m_editor)
		return;

	DBGOUT << "DWELLPOSITION ----> " << event.GetPosition() << DBGENDL;
	DBGOUT << "Dwell Point: (" << event.GetX() << ", " << event.GetY() << ")" << DBGENDL;

	m_editor->GetLangIface()->MouseHoverStarted(event.GetPosition());

	uint startPos, endPos;
	int tokenType;
	String text;
	if (m_editor->GetLangIface()->GetInfoForPosition(event.GetPosition(), &startPos, &endPos, &tokenType, &text)) {
		sigEditMouseOnSymbol(this, startPos, endPos, tokenType, text);
		this->displayDwellInfo((int) startPos, (int) endPos, tokenType, text);
	}
}

//**********************************************************************

void Editor::onDwellEnd (wxScintillaEvent& event)
{
	m_editor->GetLangIface()->MouseHoverEnded();
	this->cancelDwellInfo();
}

//**********************************************************************

void Editor::onHotspotClicked (wxScintillaEvent& event)
{
	//TODO link this info with the help system
	uint startPos, endPos;
	int tokenType;
	String text;
	if (m_editor->GetLangIface()->GetInfoForPosition(event.GetPosition(), &startPos, &endPos, &tokenType, &text)) {
		const String id = m_editor->GetTextRange(startPos, endPos);
	}
}

//**********************************************************************

void Editor::onUserListSelection (wxScintillaEvent& event)
{
	sigUserListSelected(this, event.GetListType(), event.GetText());
}

//**********************************************************************

void Editor::onMouseLeftUp (wxMouseEvent& event)
{
	m_marginDragStartPos = -1;
	event.Skip();
}

//**********************************************************************

void Editor::onMouseMove (wxMouseEvent& event)
{
	if (m_marginDragStartPos != -1 && event.Dragging() &&
		event.ButtonIsDown(wxMOUSE_BTN_LEFT)
	) {
		int startPos = m_marginDragStartPos;
		int endPos = m_editor->PositionFromPoint(event.GetPosition());

		//Move selection to include the starting or ending line
		int *posPtr = endPos >= m_marginDragStartPos ? &endPos : &startPos;
		int line = m_editor->LineFromPosition(*posPtr) + 1;
		*posPtr = m_editor->PositionFromLine(line);
		this->SelectRange(startPos, endPos);
	}
	event.Skip();
}
 
//**********************************************************************
void Editor::onTimerExpired (wxTimerEvent& event)
{
	GUI_SCOPED_FREEZE(this);

	DBGOUT << "TIMER EXPIRED !!!" << DBGENDL;

	m_editor->GetLangIface()->ContentSetup();
}

//**********************************************************************

void Editor::onTooltipCancelTimerExpired (wxTimerEvent& event)
{
	DBGOUT << "CANCEL TOOLTIP TIMER EXPIRED !!!" << DBGENDL;

	// Ugly hack to fix the tooltip problem(TM)
	//
	if (::wxFindWindowAtPointer(::wxGetMousePosition()) != m_editor)
		this->cancelDwellInfo();
	else
		m_tooltipCancelTimer.Start(5000, wxTIMER_ONE_SHOT);
}

//**********************************************************************

void Editor::onAcceleratorCommand (wxCommandEvent& event)
	{ m_editor->GetLangIface()->DispatchAcceleratorCommand(event.GetId()); }

//**********************************************************************

static const String _GetAndFormatLine (EditorWindow* editor, int lineno)
{
	String line = editor->GetLine(lineno);
	bool seenNws = false;
	for (size_t i = 0; i < line.size(); ++i) {
		int c = line[i];
		if (c == '\r' || c == '\n')
			line.erase(i--, 1);
		else if (c == '\t') {
			if (seenNws)
				line.replace(i, 1, _T(' '));
			else
				line.erase(i--, 1);
		}
		else
			seenNws = true;
	}
	return (!seenNws) ? _("Empty line") : line;
}

//**********************************************************************

void Editor::displayDwellInfo (int start, int end, int type, const String& msg)
{
	if (!m_editor->CallTipActive()) {
		String text = msg;
		int startLine = m_editor->LineFromPosition(start);
		int endLine = m_editor->LineFromPosition(end);
		if (startLine != endLine) {
			text += _T(":\n") + _GetAndFormatLine(m_editor, startLine) + _T("\n...\n") +
				_GetAndFormatLine(m_editor, endLine);
		}
		else
			text += _T(": ") + m_editor->GetTextRange(start, end);
		
		m_editor->CallTipShow(start, text);
		m_tooltipCancelTimer.Start(5000, wxTIMER_ONE_SHOT);
	}
}

//**********************************************************************

void Editor::cancelDwellInfo (void)
{
	m_editor->GetLangIface()->CancelDisplayedInfo();

	if (m_editor->CallTipActive()) {
		sigEditMouseOnSymbolCancel(this);
		m_editor->CallTipCancel();
	}
	m_tooltipCancelTimer.Stop();
}

//**********************************************************************

void Editor::textAddedOrRemoved (int pos, int length, int direction, int linesAdded)
{
#ifndef INCREMENTAL_EDITING
	m_editor->GetLangIface()->ContentOffsetChanged(pos, direction * length);
	m_timer.Start(100, wxTIMER_ONE_SHOT);
#else

	if (direction < 0)
		m_editor->GetLangIface()->ContentDeleted(pos, length, -linesAdded);
	else
		m_editor->GetLangIface()->ContentAdded(pos, length, linesAdded);

#endif // ! INCREMENTAL_EDITING

	if (linesAdded && !m_editor->IsReloading())
		sigEditLinesChangedBy(this, m_editor->LineFromPosition(pos) + 1, linesAdded);
	sigEditTextChanged(pos, length, direction >= 0);
}

//**********************************************************************

void Editor::toggleVisualizeContent (void)
{
	GUI_SCOPED_FREEZE(this);

	if (this->IsSplit()) {

		wxWindow* oldVisWindow = this->GetWindow2();
		this->Unsplit(oldVisWindow);
		m_editor->GetLangIface()->DestroyContentVisualization();
	}
	else {

		wxWindow* visWindow = m_editor->GetLangIface()->VisualizeContent(this);
		if (visWindow)
			this->SplitHorizontally(m_editor, visWindow);
	}
}

//**********************************************************************

void Editor::CancelFileModificationWatcher(void)
{
	if (m_changeWatcher)
		util::FileChangeWatcher::Instance().Cancel(GetURI(), m_changeWatcher);
	m_editor->ResetInternalSaveCounter();
}

//**********************************************************************

void Editor::OnFileModification (void)
{
	if (m_editor->GetInternalSaveCounter())
		m_editor->DecrementInternalSaveCounter();
	else {
		boost::mutex::scoped_lock lock(m_reloadMutex);
		if (!m_reloadPending) {
			m_reloadPending = true;
			timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(&Editor::AskForReload, this));
		}
	}
}

//**********************************************************************

void Editor::AskForReload (void)
{
	assert(m_reloadPending);
	const String message = _("This file has been modified outside from the source editor.");
	const String question = IsModified() ?
		_T("Do you want to reload it and lose the changes made in the source editor?"):
		_T("Do you want to reload it?");
	if (gui::getConfirmation(this, _T("Sparrow"), GetURI() + _T("\n\n") + message + _T("\n") + question) && Reload())
		sigFileNameChanged(this, GetURI());

	{
		boost::mutex::scoped_lock lock(m_reloadMutex);
		m_reloadPending = false;
	}
}

//**********************************************************************

bool Editor::Reload (void)
{
	if (!m_editor->Reload())
		return false;
	else {
		SetLanguageSettings(GetURI());
		return true;
	}
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
