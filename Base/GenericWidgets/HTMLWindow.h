/**
 *	HTMLWindow.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#ifndef GUI_HTMLWINDOW_H
#define GUI_HTMLWINDOW_H

#include "Common.h"
#include "LibraryAPIDecls.h"

#include <wx/html/htmlwin.h>

class wxHtmlLinkInfo;

namespace gui {

////////////////////////////////////////////////////////////////////////

class _BASE_API HTMLWindow : public wxHtmlWindow {
public:
	HTMLWindow (void) {}
	HTMLWindow (wxWindow* parent);
	~HTMLWindow (void);

	bool Create (wxWindow* parent);

	bool SetPage (const String& page);
	void AppendToPage (const String& fragment);
	bool LoadPage (const String& location);
	bool LoadFile (const String& location);

	const String& GetPage (void) const { return m_data; }

	//******************************************************************

	int GetContentWidth (void) const;
	int GetContentHeight (void) const;

	//******************************************************************

	bool Print (void) const;
	bool SaveToFile (const String& file);
	bool SaveToFile (void);

	//******************************************************************

	static const String GetBodyStart (const String& title);
	static const String GetBodyEnd (void);	

protected:
	virtual void OnLinkClicked (const wxHtmlLinkInfo& link);

private:
	String m_data;
};

////////////////////////////////////////////////////////////////////////

//class HTMLToolbar : public Toolbar {
//public:
//	HTMLToolbar (wxWindow* parent, HTMLWindow* html);
//	~HTMLToolbar (void);
//
//private:
//	DECLARE_EVENT_TABLE();
//
//	void createTools (void);
//
//	void onSave (wxCommandEvent& event);
//	void onPrint (wxCommandEvent& event);
//	void onClose (wxCommandEvent& event);
//
//	HTMLWindow* m_html;
//};

////////////////////////////////////////////////////////////////////////

} // namespace gui

#endif // GUI_HTMLWINDOW_H
