/**
 *	Editor.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_EDITOR_H
#define IDE_EDITOR_H

#include "Common.h"
#include "Component.h"
#include "ComponentBuilder.h"
#include "LibraryAPIDecls.h"
#include "EditorInterfaces/LanguageModuleIface.h"

#include <boost/thread/mutex.hpp>
#include <wx/splitter.h>
#include <wx/timer.h>

class wxScintillaEvent;

namespace editor {
	class EditorWindow;
}

namespace ide {

////////////////////////////////////////////////////////////////////////

class Editor : public wxSplitterWindow, public Component {
	DECLARE_IDE_COMPONENT(Editor);

public:
	Editor (void);
	~Editor (void);

	DECLARE_EXPORTED_STATIC(void, Initialize, (void));
	DECLARE_EXPORTED_STATIC(void, CleanUp, (void));

	//******************************************************************
	// Content setup
	//
	DECLARE_EXPORTED_MEMBER_(void, Open, (const String& uri),
		_("Closes the current file, if any, and opens a file in the editor"));

	DECLARE_EXPORTED_MEMBER_(void, Close, (void),
		_("Closes the current editor"));

	DECLARE_EXPORTED_MEMBER(const String&,	GetURI, (void));
	DECLARE_EXPORTED_MEMBER(void, SetURI, (const String& uri));
	DECLARE_EXPORTED_MEMBER(void, SetLanguageSettings, (const String& uri));

	DECLARE_EXPORTED_MEMBER(bool, Save, (void));
	DECLARE_EXPORTED_MEMBER(bool, SaveAs, (const String& uri));
	DECLARE_EXPORTED_MEMBER(bool, SaveToDisk, (const String& uri));

	DECLARE_EXPORTED_MEMBER(bool, IsTemp, (void));

	DECLARE_EXPORTED_MEMBER(void, SetAssociatedComponent, (const Handle& resource));
	DECLARE_EXPORTED_MEMBER(const Handle&, GetAssociatedComponent, (void));

	//******************************************************************
	// Text retrieval and modification
	//
	DECLARE_EXPORTED_MEMBER(const String,	GetText, (void));
	DECLARE_EXPORTED_MEMBER(const String,	GetSelectedText, (void));
	DECLARE_EXPORTED_MEMBER(const String,	GetLineText, (int line));
	DECLARE_EXPORTED_MEMBER(const String,	GetTextRange, (int startPos, int endPos));

	DECLARE_EXPORTED_MEMBER(void, SetText, (const String& text));
	DECLARE_EXPORTED_MEMBER(void, InsertText, (int pos, const String& text));
	DECLARE_EXPORTED_MEMBER(void, AppendText, (const String& text));
	DECLARE_EXPORTED_MEMBER(void, ClearText, (void));
	DECLARE_EXPORTED_MEMBER(void, ReplaceSelection, (const String& text));

	DECLARE_EXPORTED_MEMBER(bool, IsReadOnly, (void));
	DECLARE_EXPORTED_MEMBER(void, SetReadOnly, (bool value));

	//******************************************************************
	// Search and replace using the target
	// Return -1 if nothing is found
	//
	DECLARE_EXPORTED_MEMBER(int, SearchNext, (int flags, const String& text));
	DECLARE_EXPORTED_MEMBER(int, SearchPrev, (int flags, const String& text));

	//******************************************************************
	// Cut, copy and paste
	//
	DECLARE_EXPORTED_MEMBER(void, Cut, (void));
	DECLARE_EXPORTED_MEMBER(void, Copy, (void));
	DECLARE_EXPORTED_MEMBER(void, Paste, (void));
	DECLARE_EXPORTED_MEMBER(void, Clear, (void));
	DECLARE_EXPORTED_MEMBER(bool, CanPaste, (void));

	DECLARE_EXPORTED_MEMBER(void, CopyRange, (int startPos, int endPos));
	DECLARE_EXPORTED_MEMBER(void, CopyText, (const String& text));

	//******************************************************************
	// Undo and Redo
	//
	DECLARE_EXPORTED_MEMBER(void, SetModified, (bool val));
	DECLARE_EXPORTED_MEMBER(bool, IsModified, (void));

	DECLARE_EXPORTED_MEMBER(void, Undo, (void));
	DECLARE_EXPORTED_MEMBER(bool, CanUndo, (void));
	DECLARE_EXPORTED_MEMBER(void, Redo, (void));
	DECLARE_EXPORTED_MEMBER(bool, CanRedo, (void));

	DECLARE_EXPORTED_MEMBER(void, BeginUndoAction, (void));
	DECLARE_EXPORTED_MEMBER(void, EndUndoAction, (void));
	DECLARE_EXPORTED_MEMBER(void, ClearUndoHistory, (void));

	//******************************************************************
	// Selection and information
	//
	DECLARE_EXPORTED_MEMBER(void,	GotoPos, (int pos));
	DECLARE_EXPORTED_MEMBER(int,	GetCurrentPos, (void));

	DECLARE_EXPORTED_MEMBER(void,	GotoLine, (int line));
	DECLARE_EXPORTED_MEMBER(int,	GetCurrentLine, (void));

	DECLARE_EXPORTED_MEMBER(int,	GetLineFromPos, (int pos));

	DECLARE_EXPORTED_MEMBER(void,	GotoColumn, (int column));
	DECLARE_EXPORTED_MEMBER(int,	GetCurrentColumn, (void));

	DECLARE_EXPORTED_MEMBER(void,	EnsureLineVisible, (int line));

	DECLARE_EXPORTED_MEMBER(bool,	GetOvertype, (void));

	DECLARE_EXPORTED_MEMBER(const SerializedEditorAST,	GetAST, (void));

	DECLARE_EXPORTED_MEMBER(void,	SharedWriteAST, (void));

	DECLARE_EXPORTED_MEMBER(int,	GetLineCount, (void));
	DECLARE_EXPORTED_MEMBER(void,	SelectAll, (void));

	DECLARE_EXPORTED_MEMBER(void,	SelectRange, (int start, int end));

	DECLARE_EXPORTED_MEMBER(void,	ShowTooltip, (int start, const String& text));

	DECLARE_EXPORTED_MEMBER(void,	UserListShow, (int listType, const StringList& items));

	DECLARE_EXPORTED_MEMBER(void,	SetExtraLibraryDefinitions, (const StringList& keywords));
	DECLARE_EXPORTED_MEMBER(void,	SetByteCodeLoadingPath, (const String& path));

	//******************************************************************
	// Bookmarks, breakpoints, goto defintion and bpt arrow support
	//
	DECLARE_EXPORTED_MEMBER(void,	InsertBreakpoint, (int line, bool enabled));
	DECLARE_EXPORTED_MEMBER(void,	RemoveBreakpoint, (int line, bool enabled));

	DECLARE_EXPORTED_MEMBER(void,	InsertBookmark, (int line));
	DECLARE_EXPORTED_MEMBER(void,	RemoveBookmark, (int line));
	DECLARE_EXPORTED_MEMBER(void,	ToggleBookmark, (int line));
	DECLARE_EXPORTED_MEMBER(bool,	HasBookmark, (int line));
	DECLARE_EXPORTED_MEMBER(int,	NextBookmarkFromLine, (int line));
	DECLARE_EXPORTED_MEMBER(int,	PrevBookmarkFromLine, (int line));
	DECLARE_EXPORTED_MEMBER(void,	ClearBookmarks, (void));

	DECLARE_EXPORTED_MEMBER(const std::vector<int>, GetAllEnabledBreakpoints, (int fromLine));
	DECLARE_EXPORTED_MEMBER(const std::vector<int>, GetAllDisabledBreakpoints, (int fromLine));
	DECLARE_EXPORTED_MEMBER(const std::vector<int>, GetAllBookmarks, (int fromLine));

	DECLARE_EXPORTED_MEMBER(void,	SetBreakpointArrow, (int line));
	DECLARE_EXPORTED_MEMBER(void,	RemoveBreakpointArrow, (void));

	DECLARE_EXPORTED_MEMBER(void,	SetTraceArrow, (int line));
	DECLARE_EXPORTED_MEMBER(void,	RemoveTraceArrow, (void));

	DECLARE_EXPORTED_MEMBER(bool,	GotoGlobalDefinition, (const String& id));

	//******************************************************************
	// User commands
	//
	DECLARE_EXPORTED_MEMBER(void, ToggleContentView, (void));
	DECLARE_EXPORTED_MEMBER(void, ToggleLineNumbers, (void));
	DECLARE_EXPORTED_MEMBER(void, PromptGotoLine, (void));
	DECLARE_EXPORTED_STATIC(void, Configure, (void));

	//******************************************************************
	// Slots
	//
	DECLARE_EXPORTED_MEMBER(void, OnFileRenamed, (const Handle& file, const String& oldUri, const String& newUri));

	//******************************************************************
	// Component virtual members implementation
	//
	virtual wxWindow* GenerateWindow (wxWindow* parent);
	virtual wxWindow* GetWindow (void);

protected:

	//******************************************************************
	// Component signals
	//
	virtual void ComponentDestroyed (void);
	virtual void ComponentAppliedChangedProperties (const conf::PropertyTable& old, const conf::PropertyIdVec& changed);

private:

	//******************************************************************
	// wx Signals
	//
	DECLARE_EVENT_TABLE();

	void onUpdateUI (wxScintillaEvent& event);
	void onSavePointReached (wxScintillaEvent& event);
	void onLeftSavePoint (wxScintillaEvent& event);
	void onCharAdded (wxScintillaEvent& event);
	void onTextChanged (wxScintillaEvent& event);
	void onMarginClicked (wxScintillaEvent& event);
	void onDwellStart (wxScintillaEvent& event);
	void onDwellEnd (wxScintillaEvent& event);
	void onHotspotClicked (wxScintillaEvent& event);
	void onUserListSelection (wxScintillaEvent& event);
	void onMouseLeftUp (wxMouseEvent& event);
	void onMouseMove (wxMouseEvent& event);
	void onTimerExpired (wxTimerEvent& event);
	void onTooltipCancelTimerExpired (wxTimerEvent& event);
	void onAcceleratorCommand (wxCommandEvent& event);

	void displayDwellInfo (int start, int end, int type, const String& msg);
	void cancelDwellInfo (void);
	void textAddedOrRemoved (int pos, int length, int direction, int linesAdded);

	void toggleVisualizeContent (void);

	void CancelFileModificationWatcher (void);
	void OnFileModification (void);
	void AskForReload (void);
	bool Reload (void);
	
	static editor::LanguageModuleIface::GotoDefinitionResult 
					GotoGlobalDefinitionOfFile (const std::string& file, const std::string& id, uint stage, void* closure);
	static void		GotoPosition (int pos, void* closure);

	editor::EditorWindow*	m_editor;
	wxTimer					m_timer;
	wxTimer					m_tooltipCancelTimer;

	int						m_prevStartPos;
	int						m_prevEndPos;
	int						m_marginDragStartPos;
	bool					m_temp;
	Handle					m_workspaceResource;
	unsigned long long		m_changeWatcher;
	bool					m_reloadPending;
	boost::mutex			m_reloadMutex;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_EDITOR_H
