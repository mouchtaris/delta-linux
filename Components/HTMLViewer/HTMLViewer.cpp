/**
 *	HTMLViewer.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "HTMLViewer.h"
#include "ExtProperties.h"
#include "Streams.h"

namespace ide {

////////////////////////////////////////////////////////////////////////
// HTMLViewer class
//
COMPONENT_METADATA(
	HTMLViewer,
	_("HTMLViewer"),
	_("Displays stuff that have html tags"),
	_T("Giannis Georgalis <jgeorgal@ics.forth.gr>"),
	_T("0.1a")
);
IMPLEMENT_WX_COMPONENT(HTMLViewer, HTMLWindowExt);

COMPONENT_SET_PROPERTIES_FUNCTION(HTMLViewer, table)
{
	using namespace conf;
}

//**********************************************************************

BEGIN_EVENT_TABLE(HTMLViewer, HTMLWindowExt)
END_EVENT_TABLE();

//**********************************************************************

EXPORTED_SIGNAL(HTMLViewer, FileNameChanged, (const String& newUri));
EXPORTED_SIGNAL(HTMLViewer, FileOpened, (const String& uri));
EXPORTED_SIGNAL(HTMLViewer, EditCursorChanged, (int line, int column, bool overtype));

//**********************************************************************

EXPORTED_MEMBER(HTMLViewer, void, Open, (const String& uri))
{
	m_uri = uri;
	this->LoadPage(uri);
	sigFileNameChanged(this, uri);
	sigFileOpened(this, uri);
}

EXPORTED_MEMBER(HTMLViewer, void, Close, (void))
	{ this->Component::Destroy(); }

EXPORTED_MEMBER(HTMLViewer, const String&, GetURI, (void))
	{ return m_uri; }

EXPORTED_MEMBER(HTMLViewer, bool, Save, (void))
	{ return this->SaveAs(m_uri); }

EXPORTED_MEMBER(HTMLViewer, bool, SaveAs, (const String& uri))
{
	if (!this->IsModified())
		return true;

	std::ofstream output(m_uri.c_str(), std::ios_base::out | std::ios_base::binary);
	if (output.is_open()) {
		output << this->GetText();

		if (m_uri != uri) {
			m_uri = uri;
			sigFileNameChanged(this, uri);
		}
		return true;
	}
	return false;
}

//**********************************************************************

EXPORTED_MEMBER(HTMLViewer, const String, GetText, (void))
	{ return this->GetPage(); }

EXPORTED_MEMBER(HTMLViewer, const String, GetSelectedText, (void))
	{ return this->SelectionToText(); }

EXPORTED_MEMBER(HTMLViewer, const String, GetLineText, (int line))
	{ return String(); }

EXPORTED_MEMBER(HTMLViewer, const String, GetTextRange, (int startPos, int endPos))
	{ return String(); }

EXPORTED_MEMBER(HTMLViewer, void, SetText, (const String& text))
	{ this->SetPage(text); }

EXPORTED_MEMBER(HTMLViewer, void, InsertText, (int pos, const String& text))
	{}

EXPORTED_MEMBER(HTMLViewer, void, AppendText, (const String& text))
	{}

EXPORTED_MEMBER(HTMLViewer, void, ClearText, (void))
	{ this->SetPage(String()); }

EXPORTED_MEMBER(HTMLViewer, void, ReplaceSelection, (const String& text))
	{}

EXPORTED_MEMBER(HTMLViewer, bool, IsReadOnly, (void))
	{ return true; }

EXPORTED_MEMBER(HTMLViewer, void, SetReadOnly, (bool value))
	{}

//**********************************************************************

EXPORTED_MEMBER(HTMLViewer, int, SearchNext, (int flags, const String& text))
	{ return -1; }

EXPORTED_MEMBER(HTMLViewer, int, SearchPrev, (int flags, const String& text))
	{ return -1; }

//**********************************************************************

EXPORTED_MEMBER(HTMLViewer, void, Cut, (void)) {}
EXPORTED_MEMBER(HTMLViewer, void, Copy, (void)) {}
EXPORTED_MEMBER(HTMLViewer, void, Paste, (void)) {}
EXPORTED_MEMBER(HTMLViewer, void, Clear, (void)) {}
EXPORTED_MEMBER(HTMLViewer, bool, CanPaste, (void)) { return false; }

EXPORTED_MEMBER(HTMLViewer, void, CopyRange, (int startPos, int endPos))
	{}

EXPORTED_MEMBER(HTMLViewer, void, CopyText, (const String& text))
	{}

//**********************************************************************

EXPORTED_MEMBER(HTMLViewer, bool, IsModified, (void)) { return false; }
EXPORTED_MEMBER(HTMLViewer, void, Undo, (void)) {}
EXPORTED_MEMBER(HTMLViewer, bool, CanUndo, (void)) { return false; }
EXPORTED_MEMBER(HTMLViewer, void, Redo, (void)) {}
EXPORTED_MEMBER(HTMLViewer, bool, CanRedo, (void)) { return false; }

EXPORTED_MEMBER(HTMLViewer, void, BeginUndoAction, (void)) {}
EXPORTED_MEMBER(HTMLViewer, void, EndUndoAction, (void)) {}

//**********************************************************************

EXPORTED_MEMBER(HTMLViewer, void, GotoPos, (int pos))
{
}

EXPORTED_MEMBER(HTMLViewer, int, GetCurrentPos, (void))
	{ return -1; }

//**********************************************************************

EXPORTED_MEMBER(HTMLViewer, void, GotoLine, (int line))
{
}

EXPORTED_MEMBER(HTMLViewer, int, GetCurrentLine, (void))
	{ return -1; }

//**********************************************************************

EXPORTED_MEMBER(HTMLViewer, void, GotoColumn, (int column))
{}

EXPORTED_MEMBER(HTMLViewer, int, GetCurrentColumn, (void))
	{ return -1; }

//**********************************************************************

EXPORTED_MEMBER(HTMLViewer, bool, GetOvertype, (void))
	{ return false; }

EXPORTED_MEMBER(HTMLViewer, int, GetLineCount, (void))
	{ return 10; }

EXPORTED_MEMBER(HTMLViewer, void, SelectAll, (void))
	{}

//**********************************************************************

wxWindow* HTMLViewer::GenerateWindow (wxWindow* parent)
{
	this->Create(parent);
	return this;
}

//**********************************************************************

wxWindow* HTMLViewer::GetWindow (void)
{
	return this;
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
