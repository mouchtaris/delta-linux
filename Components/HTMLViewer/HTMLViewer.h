/**
 *	HTMLViewer.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_HTMLVIEWER_H
#define IDE_HTMLVIEWER_H

#include "Common.h"
#include "Component.h"
#include "ComponentBuilder.h"
#include "LibraryAPIDecls.h"
#include "HTMLExt.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

class HTMLViewerWindow;

////////////////////////////////////////////////////////////////////////

class HTMLViewer : public HTMLWindowExt, public Component {
	DECLARE_IDE_COMPONENT(HTMLViewer);

public:
	HTMLViewer (void) {}
	~HTMLViewer (void) {}

	//******************************************************************
	// Content setup
	//
	DECLARE_EXPORTED_MEMBER_(void, Open, (const String& uri),
		_("Closes the current file, if any, and opens a file in the HTMLViewer"));

	DECLARE_EXPORTED_MEMBER_(void, Close, (void),
		_("Closes the current HTMLViewer"));

	DECLARE_EXPORTED_MEMBER(const String&,	GetURI, (void));

	DECLARE_EXPORTED_MEMBER(bool, Save, (void));
	DECLARE_EXPORTED_MEMBER(bool, SaveAs, (const String& uri));

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
	DECLARE_EXPORTED_MEMBER(bool, IsModified, (void));
	DECLARE_EXPORTED_MEMBER(void, Undo, (void));
	DECLARE_EXPORTED_MEMBER(bool, CanUndo, (void));
	DECLARE_EXPORTED_MEMBER(void, Redo, (void));
	DECLARE_EXPORTED_MEMBER(bool, CanRedo, (void));

	DECLARE_EXPORTED_MEMBER(void, BeginUndoAction, (void));
	DECLARE_EXPORTED_MEMBER(void, EndUndoAction, (void));

	//******************************************************************
	// Selection and information
	//
	DECLARE_EXPORTED_MEMBER(void,	GotoPos, (int pos));
	DECLARE_EXPORTED_MEMBER(int,	GetCurrentPos, (void));

	DECLARE_EXPORTED_MEMBER(void,	GotoLine, (int line));
	DECLARE_EXPORTED_MEMBER(int,	GetCurrentLine, (void));

	DECLARE_EXPORTED_MEMBER(void,	GotoColumn, (int column));
	DECLARE_EXPORTED_MEMBER(int,	GetCurrentColumn, (void));

	DECLARE_EXPORTED_MEMBER(bool,	GetOvertype, (void));

	DECLARE_EXPORTED_MEMBER(int,	GetLineCount, (void));
	DECLARE_EXPORTED_MEMBER(void,	SelectAll, (void));

	//******************************************************************
	// Component virtual members implementation
	//
	virtual wxWindow* GenerateWindow (wxWindow* parent);
	virtual wxWindow* GetWindow (void);

private:
	DECLARE_EVENT_TABLE();

	String m_uri;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_HTMLVIEWER_H
