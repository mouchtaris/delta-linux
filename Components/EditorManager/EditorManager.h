/**
 *	EditorManager.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_EDITORMANAGER_H
#define IDE_EDITORMANAGER_H

#include "Common.h"
#include "DockableComponent.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"
#include "NotebookCnt.h"
#include "LibraryAPIDecls.h"

#include <vector>

namespace ide {

////////////////////////////////////////////////////////////////////////

class EditorManager : public NotebookCnt, public DockableComponent {
	DECLARE_IDE_COMPONENT(EditorManager);

public:
	EditorManager (void);
	~EditorManager (void);

	DECLARE_EXPORTED_MEMBER(const Handle, OpenDocument, (const String& uri));
	DECLARE_EXPORTED_MEMBER(void, ViewDocument, (const String& uri));
	DECLARE_EXPORTED_MEMBER(void, CloseDocument, (const String& uri));
	DECLARE_EXPORTED_MEMBER(void, FocusDocument, (const String& uri));
	DECLARE_EXPORTED_MEMBER(void, GotoDocument, (const String& uri, int line));

	DECLARE_EXPORTED_MEMBER(const Handle&,		NewEditor, (void));
	DECLARE_EXPORTED_MEMBER(void,				CloseEditor, (const Handle& editor));
	DECLARE_EXPORTED_MEMBER(void,				FocusEditor, (const Handle& editor));
	DECLARE_EXPORTED_MEMBER(const HandleVec&,	GetEditors, (void));
	DECLARE_EXPORTED_MEMBER(const Handle&,		GetEditor, (const String& uri));
	DECLARE_EXPORTED_MEMBER(const Handle&,		GetFocusedEditor, (void));

	DECLARE_EXPORTED_MEMBER(bool,				CanDestroy, (void));

	//******************************************************************
	// User commands
	//
	DECLARE_EXPORTED_MEMBER_(void, NewDocument, (void),
		_("Create a new empty document"));

	DECLARE_EXPORTED_MEMBER(void, PromptOpenDocument, (void));

	DECLARE_EXPORTED_MEMBER(void, CloseMenu, (void));
	DECLARE_EXPORTED_MEMBER(void, CloseCtx, (void));
	DECLARE_EXPORTED_MEMBER(void, Close, (void));

	DECLARE_EXPORTED_MEMBER(void, CloseAll, (void));

	DECLARE_EXPORTED_MEMBER_(void, SaveMenu, (void),
		_("Save the currently open document"));
	DECLARE_EXPORTED_MEMBER_(void, SaveToolbar, (void),
		_("Save the currently open document"));
	DECLARE_EXPORTED_MEMBER_(void, Save, (void),
		_("Save the currently open document"));
	DECLARE_EXPORTED_MEMBER_(void, SaveAs, (void),
		_("Save the currently open document with a new name"));

	DECLARE_EXPORTED_MEMBER_(void, SaveAllMenu, (void),
		_("Save all open documents"));
	DECLARE_EXPORTED_MEMBER_(void, SaveAllToolbar, (void),
		_("Save all open documents"));
	DECLARE_EXPORTED_MEMBER_(void, SaveAll, (void),
		_("Save all open documents"));

	DECLARE_EXPORTED_MEMBER(void, ToggleBookmark, (void));
	DECLARE_EXPORTED_MEMBER_(void, NextBookmark, (void),
		_("Navigate to the next bookmark"));
	DECLARE_EXPORTED_MEMBER_(void, PreviousBookmark, (void),
		_("Navigate to the previous bookmark"));
	DECLARE_EXPORTED_MEMBER(void, ClearBookmarks, (void));

	DECLARE_EXPORTED_MEMBER(void, FocusNextDocument, (void));
	DECLARE_EXPORTED_MEMBER(void, FocusPreviousDocument, (void));

	DECLARE_EXPORTED_MEMBER(void, CloseAllButFocusedEditor, (void));
	DECLARE_EXPORTED_MEMBER(void, CopyFocusedEditorFullPath, (void));
	DECLARE_EXPORTED_MEMBER(void, OpenFocusedEditorContainingFolder, (void));

	//******************************************************************
	// Slots
	//
	DECLARE_EXPORTED_MEMBER(void, OnSavePointReached, (const Handle& editor));
	DECLARE_EXPORTED_MEMBER(void, OnLeftSavePoint, (const Handle& editor));

	DECLARE_EXPORTED_MEMBER(void, OnNameChanged, (const Handle& editor, const String& uri));
	DECLARE_EXPORTED_MEMBER(void, OnCursorChanged, (const Handle& editor, int line, int col, bool ovr));

	DECLARE_EXPORTED_MEMBER(void, OnInsertBreakpoint, (const std::string& classId, const String& uri, const String& symbolic, int line, const String& condition));
	DECLARE_EXPORTED_MEMBER(void, OnRemoveBreakpoint, (const std::string& classId, const String& uri, const String& symbolic, int line, bool enabled));
	DECLARE_EXPORTED_MEMBER(void, OnEnableBreakpoint, (const std::string& classId, const String& uri, const String& symbolic, int line));
	DECLARE_EXPORTED_MEMBER(void, OnDisableBreakpoint, (const std::string& classId, const String& uri, const String& symbolic, int line));
	DECLARE_EXPORTED_MEMBER(void, OnHitBreakpoint, (const std::string& classId, const String& uri, const String& symbolic, int line));

	DECLARE_EXPORTED_MEMBER(void, OnDebugStopped, (const std::string& classId, const String& uri));
	DECLARE_EXPORTED_MEMBER(void, OnDebugResumed, (const std::string& classId));

	//******************************************************************
	// Component virtual members implementation
	//
	virtual wxWindow* GenerateWindow (wxWindow* parent);
	virtual wxWindow* GetWindow (void);

protected:
	virtual void ChildDestroyed (Component* editor);

	virtual void ChildAddedAsFirstOfItsKind (Component* component);
	virtual void ChildRemovedAsLastOfItsKind (Component* component);

private:
	DECLARE_EVENT_TABLE();

	const Handle& newEditor (const std::string& id);

	HandleVec::iterator getEditor (uint serial);
	HandleVec::iterator getEditor (const String& uri);
	HandleVec::iterator getEditor (const Handle& editor);

	size_t getEditorPos (HandleVec::const_iterator iter) const;

	void onDragMotion			(wxAuiNotebookEvent& event);
	void onEditorChanged		(wxAuiNotebookEvent& event);
	void onEditorClosed			(wxAuiNotebookEvent& event);
#if wxCHECK_VERSION(2, 8, 5)
	void onEditorTabRightDown	(wxAuiNotebookEvent& event);
	void onEditorTabRightUp		(wxAuiNotebookEvent& event);
#else
	void onContextMenu			(wxContextMenuEvent& event);
#endif
	void onMenuCommand			(wxCommandEvent& event);

	void RemoveArrowsFromAllEditors(void);
	bool EditorSaveAsDialog(const Handle& editor, bool *userCanceled = (bool *) 0);

	HandleVec	m_editors;
	size_t		m_currEditor;

	ComponentMenu *menu;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_EDITORMANAGER_H
