/**
 *	EditorManager.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "EditorManager.h"

#include "ConsoleHost.h"
#include "StringUtils.h"
#include "ComponentFactory.h"
#include "ComponentRegistry.h"
#include "ComponentMenu.h"
#include "CommonGUIUtils.h"
#include "DefaultGUIGenerator.h"
#include "FileSelectionDialog.h"
#include "GenericDialogs.h"
#include "PropertyUtils.h"
#include "Call.h"
#include "DelayedCaller.h"
#include "Undo.h"
#include "Streams.h"
#include "IDEDialogs.h"
#include "InternalDataObject.h"

#include <boost/foreach.hpp>
#include <wx/dnd.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/textbuf.h>
#include <wx/clipbrd.h>
#include <wx/utils.h>

#ifndef NO_VLD
#include <vld.h>
#endif

#include "Icons/save.xpm"
#include "Icons/save_all.xpm"
#include "Icons/book_toggle.xpm"
#include "Icons/book_next.xpm"
#include "Icons/book_prev.xpm"
#include "Icons/book_clear.xpm"

namespace ide {

////////////////////////////////////////////////////////////////////////
// EditorManager class
//
COMPONENT_METADATA(
	EditorManager,
	_("Editor Manager"),
	_("Manages text editors and other stuff that shall be unnamed"),
	_T("Giannis Georgalis <jgeorgal@ics.forth.gr>"),
	_T("0.1a")
);
IMPLEMENT_WX_COMPONENT_(EditorManager, NotebookCnt, DockableComponent);

COMPONENT_SET_PROPERTIES_FUNCTION(EditorManager, table)
{
	conf::EnumStringProperty* docking = const_cast<conf::EnumStringProperty*>(
		conf::safe_prop_cast<const conf::EnumStringProperty>(
			table.GetProperty("docking")
		)
	);
	if (docking)
		docking->SetOption(6);
}

//**********************************************************************

BEGIN_EVENT_TABLE(EditorManager, NotebookCnt)
	EVT_AUINOTEBOOK_DRAG_MOTION(wxID_ANY, EditorManager::onDragMotion)
	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, EditorManager::onEditorChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, EditorManager::onEditorClosed)
#if wxCHECK_VERSION(2, 8, 5)
	EVT_AUINOTEBOOK_TAB_RIGHT_DOWN(wxID_ANY, EditorManager::onEditorTabRightDown)
	EVT_AUINOTEBOOK_TAB_RIGHT_UP(wxID_ANY, EditorManager::onEditorTabRightUp)
#else
	EVT_CONTEXT_MENU(EditorManager::onContextMenu)
#endif
	EVT_MENU(wxID_ANY, EditorManager::onMenuCommand)
END_EVENT_TABLE();

//**********************************************************************

EXPORTED_SIGNAL(EditorManager, AllEditorsClosed, (void));
EXPORTED_SIGNAL(EditorManager, FocusedEditorChanged, (const Handle& focusedEditor));
EXPORTED_SIGNAL(EditorManager, FocusedEditorCursorChanged, (int line, int column, bool overtype));

//**********************************************************************

EditorManager::EditorManager (void) : m_currEditor(0), menu(0)
{

}

//**********************************************************************

EditorManager::~EditorManager (void)
{
	if (menu) {
		delete menu;
		menu = 0;
	}
}

//**********************************************************************

EXPORTED_MEMBER(EditorManager, const Handle, OpenDocument, (const String& uri))
{
	Handle editor;
	const String normalizedUri = util::normalizepath(uri);
	if (editor = this->GetEditor(normalizedUri))
		this->FocusEditor(editor);
	else {
		Component* shell = ComponentRegistry::Instance().GetFocusedInstance("Shell");
		if(shell) {
			GUI_SCOPED_FREEZE(shell->GetWindow());
			if (editor = this->NewEditor()) {
				Call<void (const String&)>(this, editor, "Open")(normalizedUri);
				Undo<void (String)>(this, "CloseDocument")(normalizedUri);
			}
		}
	}
	return editor;
}

//**********************************************************************

EXPORTED_MEMBER(EditorManager, void, ViewDocument, (const String& uri))
{
	const String normalizedUri = util::normalizepath(uri);
	if (const Handle& editor = this->GetEditor(normalizedUri))
		this->FocusEditor(editor);
	else
		Call<void (const String&)>(this, this->newEditor("HTMLViewer"), "Open")(normalizedUri);
}

//**********************************************************************

EXPORTED_MEMBER(EditorManager, void, CloseDocument, (const String& uri))
{
	this->CloseEditor(this->GetEditor(util::normalizepath(uri)));
}

//**********************************************************************

EXPORTED_MEMBER(EditorManager, void, FocusDocument, (const String& uri))
{
	this->FocusEditor(this->GetEditor(util::normalizepath(uri)));
}

//**********************************************************************

EXPORTED_MEMBER(EditorManager, void, GotoDocument, (const String& uri, int line))
{
	const Handle editor = this->OpenDocument(util::normalizepath(uri));
	Call<void (int)>(this, editor, "GotoLine")(line);
	Call<void (int)>(this, editor, "EnsureLineVisible")(line);
}

//**********************************************************************

EXPORTED_MEMBER(EditorManager, const Handle&, NewEditor, (void))
	{ return this->newEditor("Editor"); }

//**********************************************************************

EXPORTED_MEMBER(EditorManager, void, CloseEditor, (const Handle& editor))
{
	if (editor) {
		const String uri = Call<String (void)>(this, editor, "GetURI")();
		if (Call<bool (void)>(this, editor, "IsModified")()) {
			String name = wxFileName(uri).GetFullName();
			if (name.empty()) {
				HandleVec::iterator iter = std::find(m_editors.begin(), m_editors.end(), editor);
				name = iter != m_editors.end() ?
					GetPageText(std::distance(m_editors.begin(), iter)) : _T("New File");
			}
			int ret = gui::getConfirmationOption(0,	_("EditorManager"),
				String(_("Do you want to save the changes to ")) + name + _("?")
			);
			if (ret == wxID_YES) {
				bool saved = Call<bool (void)>(this, editor, "IsTemp")() ? EditorSaveAsDialog(editor) :
					Call<bool (void)>(this, editor, "Save")();
				if (!saved && !gui::getConfirmation(this, _("Error"), _("Cannot save file! Do you still want to close it?")))
					return;
			}
			else if (ret == wxID_CANCEL)
				return;
		}
		Call<void (void)>(this, editor, "Close")();
		Undo<void (String)>(this, "OpenDocument")(uri);
	}
}

//**********************************************************************

EXPORTED_MEMBER(EditorManager, void, FocusEditor, (const Handle& editor))
{
	HandleVec::iterator iter = std::find(m_editors.begin(), m_editors.end(), editor);
	if (iter != m_editors.end()) {
		this->SetSelection(std::distance(m_editors.begin(), iter));
		if (Component* comp = iter->Resolve()) {
			wxWindow* window = comp->GetWindow();
			assert(window);
			window->SetFocus();
		}
	}
}

//**********************************************************************

EXPORTED_MEMBER(EditorManager, const HandleVec&, GetEditors, (void))
{
	return m_editors;
}

//**********************************************************************

EXPORTED_MEMBER(EditorManager, const Handle&, GetEditor, (const String& uri))
{
	HandleVec::iterator iter = this->getEditor(util::normalizepath(uri));
	if (iter != m_editors.end())
		return *iter;
	return nullHandle;
}

//**********************************************************************

EXPORTED_MEMBER(EditorManager, const Handle&, GetFocusedEditor, (void))
{
	if (m_editors.empty())
		return nullHandle;
	return m_editors[m_currEditor];
}

//**********************************************************************

EXPORTED_MEMBER(EditorManager, bool, CanDestroy, (void))
{
	HandleVec modified;
	BOOST_FOREACH(const Handle& editor, m_editors)
		if (Call<bool (void)>(this, editor, "IsModified")())
			modified.push_back(editor);

	if (!modified.empty()) {
		int ret = gui::getConfirmationOption(0,	_("EditorManager"),
			_("There are unsaved files. Do you want to save changes?")
		);
		if (ret == wxID_YES) {
			bool result = true;
			BOOST_FOREACH(const Handle& editor, m_editors)
				result = Call<bool (void)>(this, editor, "Save")() && result;

			if (!result)
				return gui::getConfirmation(this, _("Error"),
					_("Failed to save some files! Do you still want to close the EditorManager?"));
		}
		else if (ret == wxID_CANCEL)
			return false;
	}
	return true;
}

//**********************************************************************

EXPORTED_CMD_MEMBER(EditorManager, NewDocument, _("/File/{100}--New/{10}File...\tCtrl-N"), MT_MAIN, "")
{
	this->NewEditor();
}

//**********************************************************************

EXPORTED_CMD_MEMBER(EditorManager, PromptOpenDocument, _("/File/{110}Open--/{10}File...\tCtrl-O"), MT_MAIN, "")
{
	FileSelectionDialog fsd(this);
	if (fsd.ShowModal() == wxID_OK)
		this->OpenDocument(fsd.GetPath());
}

//**********************************************************************

EXPORTED_CMD_MEMBER(EditorManager, CloseMenu, _("/File/{120}--Close--/Current File\tCtrl-F4"), MT_MAIN, "")
	{ Close(); }
EXPORTED_CMD_MEMBER(EditorManager, CloseCtx, _("/{20}Close Current File"), MT_CTX, "")
	{ Close(); }
EXPORTED_MEMBER(EditorManager, void, Close, (void))
{
	this->CloseEditor(this->GetFocusedEditor());
}

//**********************************************************************

EXPORTED_MEMBER(EditorManager, void, CloseAll, (void))
{
	while (!m_editors.empty())
		this->CloseEditor(m_editors.back());
}

//**********************************************************************

EXPORTED_IMAGE(EditorManager, "save", save_xpm);
EXPORTED_CMD_MEMBER(EditorManager, SaveMenu, _("/File/{130}--Save/{10}Current File\tCtrl-S"), MT_MAIN, "save")
	{ Save(); }
EXPORTED_CMD_MEMBER(EditorManager, SaveToolbar, _("/{10}Save Current File"), MT_NMAIN, "save")
	{ Save(); }
EXPORTED_MEMBER(EditorManager, void, Save, (void))
{
	const Handle& editor = this->GetFocusedEditor();
	if (editor) {
		bool error = false;
		if (Call<bool (void)>(this, editor, "IsTemp")()) {
			bool userCanceled;
			error = !EditorSaveAsDialog(editor, &userCanceled) && !userCanceled;
		}
		else if(!Call<bool (void)>(this, editor, "Save")())
			error = true;
		if (error)
			gui::displayMessage(this, _("Error"), _("Cannot save file!"));
	}
}

//**********************************************************************

EXPORTED_CMD_MEMBER(EditorManager, SaveAs, _("/File/{135}Save File As..."), MT_NTOOLBAR, "save")
{
	const Handle& editor = this->GetFocusedEditor();
	bool userCanceled;
	if (editor && !EditorSaveAsDialog(editor, &userCanceled) && !userCanceled)
		gui::displayMessage(this, _("Error"), _("Cannot save file!"));
}

//**********************************************************************

EXPORTED_IMAGE(EditorManager, "save_all", save_all_xpm);
EXPORTED_CMD_MEMBER(EditorManager, SaveAllMenu, _("/File/{140}Save All--/{10}Files\tCtrl-Shift-S"), MT_MAIN, "save_all")
	{ SaveAll(); }
EXPORTED_CMD_MEMBER(EditorManager, SaveAllToolbar, _("/Save All Files"), MT_TOOLBAR, "save_all")
	{ SaveAll(); }
EXPORTED_MEMBER(EditorManager, void, SaveAll, (void))
{
	bool result = true;
	for (uint i = 0; i < m_editors.size(); ++i)
		result = Call<bool (void)>(this, m_editors[i], "Save")() && result;

	if (!result)
		gui::displayMessage(this, _("Error"), _("Failed to save some files"));
}

//**********************************************************************

EXPORTED_IMAGE(EditorManager, "book_toggle", book_toggle_xpm);
EXPORTED_CMD_MEMBER(EditorManager, ToggleBookmark, _("/{1}Edit/{10}Bookmarks--/{10}Toggle Bookmark\tCtrl+F2--"), MT_MAIN, "book_toggle")
{
	if (const Handle& editor = this->GetFocusedEditor()) {
		EnsureVisibility("EditorManager");
		int line = Call<int (void)>(this, editor, "GetCurrentLine")();
		Call<void (int)>(this, editor, "ToggleBookmark")(line);
	}
}

//**********************************************************************

EXPORTED_IMAGE(EditorManager, "book_next", book_next_xpm);
EXPORTED_CMD_MEMBER(EditorManager, NextBookmark, _("/{1}Edit/{10}Bookmarks/{20}Next Bookmark\tF2"), MT_NCTX, "book_next")
{
	if (m_editors.empty())
		return;

	size_t index = m_currEditor, start = m_currEditor;
	int startLine = Call<int (void)>(this, m_editors[index], "GetCurrentLine")();

	if (Call<bool (int)>(this, m_editors[index], "HasBookmark")(startLine))
		++startLine;

	int line = startLine;
	while (true) {
		if (!(line = Call<int (int)>(this, m_editors[index], "NextBookmarkFromLine")(line))) {
			index = (index + 1) % m_editors.size();
			line = 1;
			if (index == start) {
				if (startLine == 1)
					break;
				startLine = 1;
			}
		}
		else {
			EnsureVisibility("EditorManager");
			Call<void (int)>(this, m_editors[index], "GotoLine")(line);
			this->FocusEditor(m_editors[index]);
			break;
		}
	}
}

//**********************************************************************

EXPORTED_IMAGE(EditorManager, "book_prev", book_prev_xpm);
EXPORTED_CMD_MEMBER(EditorManager, PreviousBookmark, _("/{1}Edit/{10}Bookmarks/{30}Previous Bookmark\tShift+F2--"), MT_NCTX, "book_prev")
{
	if (m_editors.empty())
		return;

	size_t index = m_currEditor, start = m_currEditor;
	int startLine = Call<int (void)>(this, m_editors[index], "GetCurrentLine")();

	if (Call<bool (int)>(this, m_editors[index], "HasBookmark")(startLine))
		--startLine;

	int line = startLine;
	while (true) {
		if (!(line = Call<int (int)>(this, m_editors[index], "PrevBookmarkFromLine")(line))) {
			index = index > 0 ? index - 1 : m_editors.size() - 1;
			line = Call<int (void)>(this, m_editors[index], "GetLineCount")();
			if (index == start) {
				if (startLine == line)
					break;
				startLine = line;
			}
		}
		else {
			EnsureVisibility("EditorManager");
			Call<void (int)>(this, m_editors[index], "GotoLine")(line);
			this->FocusEditor(m_editors[index]);
			break;
		}
	}
}

//**********************************************************************

EXPORTED_IMAGE(EditorManager, "book_clear", book_clear_xpm);
EXPORTED_CMD_MEMBER(EditorManager, ClearBookmarks, _("/{1}Edit/{10}Bookmarks/{40}--Clear Bookmarks"), MT_MAIN, "book_clear")
{
	BOOST_FOREACH(const Handle& editor, m_editors)
		Call<void (void)>(this, editor, "ClearBookmarks")();
}

//**********************************************************************

EXPORTED_CMD_MEMBER(EditorManager, FocusNextDocument, _("/{1}Edit/{18}Next Document\tCtrl+Tab--"), MT_MAIN, "")
{
	if (m_editors.empty())
		return;

	size_t index = (m_currEditor + 1) % m_editors.size();
	this->FocusEditor(m_editors[index]);
}

//**********************************************************************

EXPORTED_CMD_MEMBER(EditorManager, FocusPreviousDocument, _("/{1}Edit/{16}--Previous Document\tCtrl+Shift+Tab"), MT_MAIN, "")
{
	if (m_editors.empty())
		return;

	size_t index = m_currEditor > 0 ? m_currEditor - 1 : m_editors.size() - 1;
	this->FocusEditor(m_editors[index]);
}

//**********************************************************************

EXPORTED_CMD_MEMBER(EditorManager, CloseAllButFocusedEditor, _("/{30}Close All But This--"), MT_CTX, "")
{
	// Don't use a Handle& as it will be invalidated after closing other editors
	if (const Handle editor = this->GetFocusedEditor()) {
		SCOPED_BATCH_UNDO(s_classId);
		Undo<void (const Handle&)>(this, "FocusEditor")(editor);
		while(m_editors.front() != editor)
			this->CloseEditor(m_editors.front());
		while(m_editors.back() != editor)
			this->CloseEditor(m_editors.back());
	}
}

//**********************************************************************

EXPORTED_CMD_MEMBER(EditorManager, CopyFocusedEditorFullPath, _("/{40}--Copy Full Path"), MT_CTX, "")
{
#if wxUSE_CLIPBOARD
	if (const Handle& editor = this->GetFocusedEditor())
		if (wxTheClipboard->Open()) {
			const String& uri = Call<const String&(void)>(this, editor, "GetURI")();
			wxTheClipboard->UsePrimarySelection(false);
			wxTheClipboard->SetData(new wxTextDataObject(uri));
			wxTheClipboard->Close();
		}
#endif	// wxUSE_CLIPBOARD
}

//**********************************************************************

EXPORTED_CMD_MEMBER(EditorManager, OpenFocusedEditorContainingFolder, _("/{50}Open Containing Folder"), MT_CTX, "")
{
	if (const Handle& editor = this->GetFocusedEditor()) {
		const String& uri = Call<const String&(void)>(this, editor, "GetURI")();
		if (!wxFile::Exists(uri))
			gui::displayMessage(this, _("Error"), _("Failed to open containing folder because file does not exist!"));
		else {
			const std::string path = util::str2std(wxFileName(uri).GetPath());
#ifdef WIN32
			util::ConsoleHost().Execute("explorer.exe " + path);
#endif	// WIN32
		}
	}
}

//**********************************************************************

EXPORTED_SLOT_MEMBER(EditorManager, void, OnSavePointReached, (const Handle& editor),
	"FileSavePointReached")
{
	HandleVec::const_iterator iter = this->getEditor(editor);
	if (iter != m_editors.end()) {
		size_t pos = this->getEditorPos(iter);
		const String prevTitle = this->GetPageText(pos);
		const String title = prevTitle.substr(0, prevTitle.size() - 1);
		this->SetPageText(pos, title);
	}
}

//**********************************************************************

EXPORTED_SLOT_MEMBER(EditorManager, void, OnLeftSavePoint, (const Handle& editor),
	"FileLeftSavePoint")
{
	HandleVec::const_iterator iter = this->getEditor(editor);
	if (iter != m_editors.end()) {
		size_t pos = this->getEditorPos(iter);
		const String title = this->GetPageText(pos) + _T("*");
		this->SetPageText(pos, title);
	}
}

//**********************************************************************

EXPORTED_SLOT_MEMBER(EditorManager, void, OnNameChanged, (const Handle& editor, const String& uri),
	"FileNameChanged")
{
	HandleVec::const_iterator iter = this->getEditor(editor);
	if (iter != m_editors.end()) {
		size_t pos = this->getEditorPos(iter);	
		const String title = wxFileName(uri).GetFullName();
		this->SetPageText(pos, title);
	}
}

//**********************************************************************

EXPORTED_SLOT_MEMBER(EditorManager, void, OnCursorChanged, (const Handle& editor, int line, int col, bool ovr),
	"EditCursorChanged")
{
	if (editor == this->GetFocusedEditor())
		sigFocusedEditorCursorChanged(this, line, col, ovr);
}

//**********************************************************************

EXPORTED_SLOT_MEMBER(EditorManager, void, OnInsertBreakpoint,
	(const std::string& classId, const String& uri, const String& symbolic, int line, const String& condition),
	"BreakpointAdded")
{
	if (const Handle& editor = this->GetEditor(uri))
		Call<void (int, bool)>(this, editor, "InsertBreakpoint")(line, true);
}

//**********************************************************************

EXPORTED_SLOT_MEMBER(EditorManager, void, OnRemoveBreakpoint,
	(const std::string& classId, const String& uri, const String& symbolic, int line, bool enabled),
	"BreakpointRemoved")
{
	if (const Handle& editor = this->GetEditor(uri))
		Call<void (int, bool)>(this, editor, "RemoveBreakpoint")(line, enabled);
}

//**********************************************************************

EXPORTED_SLOT_MEMBER(EditorManager, void, OnEnableBreakpoint,
	(const std::string& classId, const String& uri, const String& symbolic, int line),
	"BreakpointEnabled")
{
	if (const Handle& editor = this->GetEditor(uri)) {
		Call<void (int, bool)>(this, editor, "RemoveBreakpoint")(line, false);
		Call<void (int, bool)>(this, editor, "InsertBreakpoint")(line, true);
	}
}

//**********************************************************************

EXPORTED_SLOT_MEMBER(EditorManager, void, OnDisableBreakpoint,
	(const std::string& classId, const String& uri, const String& symbolic, int line),
	"BreakpointDisabled")
{
	if (const Handle& editor = this->GetEditor(uri)) {
		Call<void (int, bool)>(this, editor, "RemoveBreakpoint")(line, true);
		Call<void (int, bool)>(this, editor, "InsertBreakpoint")(line, false);
	}
}

//**********************************************************************

EXPORTED_SLOT_MEMBER(EditorManager, void, OnHitBreakpoint,
	(const std::string& classId, const String& uri, const String& symbolic, int line),
	"BreakpointHit")
{
	Handle editor = this->GetEditor(uri);
	if (!editor)
		editor = this->OpenDocument(uri);
	if (editor) {
		Focus();
		Call<void (int)>(this, editor, "SetBreakpointArrow")(line);
		FocusEditor(editor);
	}
}

//**********************************************************************

EXPORTED_SLOT_MEMBER(EditorManager, void, OnDebugStopped, (const std::string& classId, const String& uri),
	"DebugStopped")
{
	RemoveArrowsFromAllEditors();
}

//**********************************************************************

EXPORTED_SLOT_MEMBER(EditorManager, void, OnDebugResumed, (const std::string& classId), "DebugResumed")
{
	RemoveArrowsFromAllEditors();
}

//**********************************************************************

wxWindow* EditorManager::GenerateWindow (wxWindow* parent)
{
	this->Create(parent);
	menu = GetUserCommands().GenerateMenu(new ComponentMenu);
	return this;
}

//**********************************************************************

wxWindow* EditorManager::GetWindow (void)
{
	return this;
}

//**********************************************************************

void EditorManager::ChildDestroyed (Component* editor)
{
	const Handle handle(editor);
	HandleVec::iterator iter = std::find(m_editors.begin(), m_editors.end(), handle);
	assert(iter != m_editors.end());
	size_t pos = std::distance(m_editors.begin(), iter);
	m_editors.erase(iter);
	this->RemovePage(pos);

	if (m_editors.empty())
		sigAllEditorsClosed(this);
}

//**********************************************************************

void EditorManager::ChildAddedAsFirstOfItsKind (Component* component)
	{ this->MergeUserCommands(component->GetUserCommands()); }

void EditorManager::ChildRemovedAsLastOfItsKind (Component* component)
	{ this->UnMergeUserCommands(component->GetUserCommands()); }

//**********************************************************************

const Handle& EditorManager::newEditor (const std::string& id)
{
	if (Component* editor = ComponentFactory::Instance().CreateComponent(id)) {
		editor->SetParent(this);

		m_editors.push_back(Handle(editor));
		this->AddPage(editor->GenerateWindow(this), _("New file"), true);

		return m_editors.back();
	}
	return nullHandle;
}

//**********************************************************************

HandleVec::iterator EditorManager::getEditor (uint serial)
	{ return std::find(m_editors.begin(), m_editors.end(), serial); }

HandleVec::iterator EditorManager::getEditor (const String& uri)
{
	for (HandleVec::iterator i = m_editors.begin(); i != m_editors.end(); ++i)
		if (wxFileName(Call<const String& (void)>(this, *i, "GetURI")()).SameAs(uri))
			return i;
	return m_editors.end();
}

HandleVec::iterator EditorManager::getEditor (const Handle& editor)
	{ return std::find(m_editors.begin(), m_editors.end(), editor); }

//**********************************************************************

size_t EditorManager::getEditorPos (HandleVec::const_iterator iter) const
	{ return std::distance(m_editors.begin(), iter); }

//**********************************************************************

void EditorManager::onDragMotion (wxAuiNotebookEvent& event)
{
	if(!this->GetScreenRect().Inflate(20, 20).Contains(::wxGetMousePosition())) {
		// Custom AuiNotebook handling to release the mouse, remove the docking hints and reset the cursor.
		// Do not invoke wxAuiNotebook::OnTabEndDrag as it will also split the page tab.
		this->m_mgr.HideHint();
		if (wxAuiTabCtrl* ctrl = (wxAuiTabCtrl*)event.GetEventObject()) {
			if (ctrl->GetCapture() == ctrl)
				ctrl->ReleaseMouse();
	       ctrl->SetCursor(wxCursor(wxCURSOR_ARROW));
		}

		const Handle& editor = this->GetFocusedEditor();
		wxDropSource dragSource(this);
		gui::InternalDataObject object(_T("Handle"), comm::encoder().encode(editor).str());
		dragSource.SetData(object);
		dragSource.DoDragDrop();
	}
	else
		event.Skip();
}

//**********************************************************************

void EditorManager::onEditorChanged (wxAuiNotebookEvent& event)
{
	m_currEditor = event.GetSelection();
	const Handle& newlyfocused = m_editors[m_currEditor];
	newlyfocused->Focus();

	sigFocusedEditorChanged(this, newlyfocused);

	int line = Call<int (void)>(this, newlyfocused, "GetCurrentLine")();
	int column = Call<int (void)>(this, newlyfocused, "GetCurrentColumn")();
	bool ovr = Call<bool (void)>(this, newlyfocused, "GetOvertype")();
	sigFocusedEditorCursorChanged(this, line, column, ovr);
}

//**********************************************************************

void EditorManager::onEditorClosed (wxAuiNotebookEvent& event)
{
	size_t editorIndex = event.GetSelection();
	assert(editorIndex < m_editors.size());
	this->CloseEditor(m_editors[editorIndex]);
	event.Veto();
}

//**********************************************************************

#if wxCHECK_VERSION(2, 8, 5)
void EditorManager::onEditorTabRightDown (wxAuiNotebookEvent& event)
	{ SetSelection(event.GetSelection()); }

void EditorManager::onEditorTabRightUp (wxAuiNotebookEvent& event)
	{ PopupMenu(menu); }
#else
void EditorManager::onContextMenu (wxContextMenuEvent& event)
{
	const wxPoint p = event.GetPosition() - this->GetScreenPosition();
	wxAuiTabCtrl* ctrl = static_cast<wxAuiTabCtrl*>(event.GetEventObject());
	wxWindow *window;
	assert(ctrl);
	if (ctrl->TabHitTest(p.x, p.y, &window)) {
		ctrl->SetActivePage(window);
		window->SetFocus();
		PopupMenu(menu);
	}
}
#endif

//**********************************************************************

static void _callComponentCallback(const UserCommand::Callback& action)
	{ action(); }

void EditorManager::onMenuCommand(wxCommandEvent& event)
{
	timer::DelayedCaller::Instance().PostDelayedCall(
		boost::bind(_callComponentCallback, menu->GetAction(event.GetId()))
	);
}

//**********************************************************************

void EditorManager::RemoveArrowsFromAllEditors(void)
{
	for (uint i = 0; i < m_editors.size(); ++i) {
		Call<void (void)>(this, m_editors[i], "RemoveBreakpointArrow")();
		Call<void (void)>(this, m_editors[i], "RemoveTraceArrow")();
	}
}

//**********************************************************************

bool EditorManager::EditorSaveAsDialog(const Handle& editor, bool *userCanceled) {
	FileDialog dlg(0, _("Save File As"), _T("Default DIR"), String(), String(), 
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dlg.ShowModal() == wxID_OK)
		return Call<bool (const String&)>(this, editor, "SaveAs")(dlg.GetPath());
	else {
		if (userCanceled)
			*userCanceled = true;
		return false;
	}
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
