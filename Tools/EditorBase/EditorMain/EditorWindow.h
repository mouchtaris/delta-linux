/**
 *	EditorWindow.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef EDITOR_EDITORWINDOW_H
#define EDITOR_EDITORWINDOW_H

#include "EditorUtils/EditorCommon.h"
#include "WXScintilla/wxscintilla.h"

#include <vector>

namespace editor {

////////////////////////////////////////////////////////////////////////

class LanguageModuleIface;

////////////////////////////////////////////////////////////////////////

class _EDITOR_API EditorWindow : public wxScintilla {
public:
	enum MarginType {
		MARGIN_LINENUMBER	= 0,
		MARGIN_EDITORMARKS	= 1,
		MARGIN_FOLDING		= 2,
	};

	EditorWindow (void);
	EditorWindow (wxWindow* parent);
	~EditorWindow (void);

	bool Create (wxWindow* parent);

	//******************************************************************

	wxScintilla*	CreateOtherView (wxWindow* parent);
	void			DestroyOtherView (void);

	//******************************************************************

	bool Open (const String& uri);
	bool Reload (void);
	bool IsReloading (void) const { return m_duringReload; }
	bool Save (void);
	bool SaveAs (const String& uri);
	bool SaveToDisk (const String& uri);

	//******************************************************************

	void	IncrementInternalSaveCounter (void)	{ ++m_internalSaves; }
	void	DecrementInternalSaveCounter (void)	{ --m_internalSaves; assert(m_internalSaves >= 0); }
	int		GetInternalSaveCounter (void) const	{ assert(m_internalSaves >= 0); return m_internalSaves; }
	void	ResetInternalSaveCounter (void) { m_internalSaves = 0; }

	//******************************************************************

	void LoadLanguageModule (const String& moduleNamePath);
	void UnLoadLanguageModule (void);

	//******************************************************************

	void					SetURI (const String& uri) { m_uri = uri; }

	const String&			GetURI (void) const { return m_uri; }
	const String&			GetLangModulePath (void) const { return m_langModPath; }
	LanguageModuleIface*	GetLangIface (void) { return m_langIface; }

	//******************************************************************

	void SetModified (bool val);
	bool IsModified	(void);

	//******************************************************************
	// Utility functions
	//
	void UpdateMatchedBrace (void);

	void ShowFoldingView (bool value = true);
	void RequestToggleFoldingAtPos (int pos);

	void AddIndicator (int start, int end, int mask);
	void RemoveIndicator (int start, int end, int mask);

	//******************************************************************

	void InsertEnabledBreakpointToLine (int line);
	void RemoveEnabledBreakpointFromLine (int line);

	void InsertDisabledBreakpointToLine (int line);
	void RemoveDisabledBreakpointFromLine (int line);

	void InsertErrorToLine (int line);
	void RemoveErrorFromLine (int line);

	void InsertBookmarkToLine (int line);
	void RemoveBookmarkFromLine (int line);
	void ToggleBookmarkAtLine (int line);
	bool HasBookmarkAtLine (int line);

	int NextBookmarkFromLine (int line);
	int PrevBookmarkFromLine (int line);

	void ClearBookmarks (void);

	const std::vector<int> GetAllBookmarksFromLine (int line);
	const std::vector<int> GetAllEnabledBreakpointsFromLine (int line);
	const std::vector<int> GetAllDisabledBreakpointsFromLine (int line);

	//******************************************************************

	void SetYellowArrowToLine (int line);
	void RemoveYellowArrow (void);

	void SetGreenArrowToLine (int line);
	void RemoveGreenArrow (void);

private:
	DECLARE_EVENT_TABLE();

	bool save (const String& uri);

	inline void insert		(int line, int type);
	inline void remove		(int line, int type);
	inline void toggle		(int line, int type);
	inline void removeall	(int type);

	inline const std::vector<int> getAll (int line, int type);

	void onSavePointReached (wxScintillaEvent& event);
	void onLeftSavePoint (wxScintillaEvent& event);
	void onAutoCompleteSelection (wxScintillaEvent& event);
	void onKillFocus (wxFocusEvent& event);

	String					m_uri;
	wxScintilla*			m_otherView;

	String					m_langModPath;
	LanguageModuleIface*	m_langIface;

	int m_yellowArrowLine;
	int m_greenArrowLine;
	int m_internalSaves;
	bool m_modified;
	bool m_duringReload;
};

////////////////////////////////////////////////////////////////////////

class ScopedBusyCursor {
public:
	ScopedBusyCursor (EditorWindow* win) : m_win(win), m_prevType(win->GetCursorType())
		{ win->SetCursorType(wxSCI_CURSORWAIT); }
	~ScopedBusyCursor (void) { m_win->SetCursorType(m_prevType); }

private:
	EditorWindow*	m_win;
	int				m_prevType;
};

//**********************************************************************

#define SCOPED_BUSY(editorWin) editor::ScopedBusyCursor _scoped_##editorWin(editorWin)

////////////////////////////////////////////////////////////////////////

} // namespace editor

#endif // EDITOR_EDITORWINDOW_H
