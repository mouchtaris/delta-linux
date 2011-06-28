/**
 *	HTMLWindow.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#include "HTMLWindow.h"

#if _MSC_VER == 1600 && defined (NO_THIRD_PARTY_BASE)
#	include <wx/mimetype.h>
#	define _EXECUTE_LINK ExecuteLink
static bool ExecuteLink (const wxString &link) {
    wxString mimetype = wxEmptyString;
    if (link.StartsWith (_T("http://"))) {
        mimetype = _T("text/html");
    }else if (link.StartsWith (_T("ftp://"))) {
        mimetype = _T("text/html");
    }else if (link.StartsWith (_T("mailto:"))) {

#ifdef __WXMSW__
		wxRegKey key(wxRegKey::HKCR, _T("mailto\\shell\\open\\command"));
		wxString command = _T("%1");
		key.QueryValue(wxEmptyString, command);
		command.Replace(_T("%1"), link.c_str());
		command.Replace(_T("%l"), link.c_str());
		::wxExecute(command);
		return true;
#else
        mimetype = _T("message/rfc822");
#endif
    }else{
        return false;
    }
    wxFileType *filetype = wxTheMimeTypesManager->GetFileTypeFromMimeType (mimetype);
    if (filetype) {
        wxString cmd;
        if (filetype->GetOpenCommand (&cmd, wxFileType::MessageParameters (link))) {
            cmd.Replace(_T("file://"), wxEmptyString);
            ::wxExecute(cmd);
            return true;
        }
        delete filetype;
    }
    return false;
}

#else
#	include "wxHyperlink.h"
#	define _EXECUTE_LINK wxHyperLink::ExecuteLink
#endif

#include <wx/html/htmprint.h>
#include <wx/html/htmlcell.h>
#include <wx/filedlg.h>

namespace gui {

////////////////////////////////////////////////////////////////////////

HTMLWindow::HTMLWindow (wxWindow* parent) :
	wxHtmlWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO)
{
	static int defaultSizes[] = {
		6, 7, 8, 9, 10, 11, 12
	};
	this->SetFonts(String(), String(), defaultSizes);
}

//**********************************************************************

HTMLWindow::~HTMLWindow (void)
{
}

//**********************************************************************

bool HTMLWindow::Create (wxWindow* parent)
{
	return this->wxHtmlWindow::Create(
			parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO
		);
}

//void HTMLWindow::Zoom (float factor)
//{
//	static int defaultSizes[] = {
//		wxHTML_FONT_SIZE_1, wxHTML_FONT_SIZE_2, wxHTML_FONT_SIZE_3, wxHTML_FONT_SIZE_4,
//		wxHTML_FONT_SIZE_5, wxHTML_FONT_SIZE_6, wxHTML_FONT_SIZE_7
//	};
//	factor = util::clamp(factor, 0.5F, 6.5F);
//
//	int sizes[7];
//	for (int i = 0; i < 7; ++i)
//		sizes[i] = defaultSizes[i] * factor;
//	this->SetFonts(String(), String(), sizes);
//
//	if (!m_data.empty())
//		this->SetPage(m_data);
//}

//**********************************************************************

bool HTMLWindow::SetPage (const String& page)
{
	assert(!page.empty());

	m_data = page;
	return this->wxHtmlWindow::SetPage(page);
}

//**********************************************************************

void HTMLWindow::AppendToPage (const String& fragment)
{
	this->wxHtmlWindow::AppendToPage(fragment);
	String::size_type pos = m_data.rfind(_T("</body>"));
	assert(pos != String::npos);
	m_data.insert(pos, fragment);
}

//**********************************************************************

bool HTMLWindow::LoadPage (const String& location)
	{ return this->LoadFile(location); }

bool HTMLWindow::LoadFile (const String& location)
{
	if (this->wxHtmlWindow::LoadPage(location)) {
		//util::File file(location, "r");
		//m_data = file.GetContents();
		//return file;
		return true;
	}
	return false;
}

//**********************************************************************

int HTMLWindow::GetContentWidth (void) const
	{ return this->GetInternalRepresentation()->GetWidth(); }

int HTMLWindow::GetContentHeight (void) const
	{ return this->GetInternalRepresentation()->GetHeight(); }

//**********************************************************************

bool HTMLWindow::Print (void) const
{
	static std::auto_ptr<wxHtmlEasyPrinting> ptr(new wxHtmlEasyPrinting());
	return ptr->PrintText(m_data);
}

//**********************************************************************

bool HTMLWindow::SaveToFile (const String& PORT_UNUSED_PARAM(file))
{
	//util::File dest(file, "w");
	//dest.WriteText(m_data);
	//return bool(dest);
	return false;
}

//**********************************************************************

bool HTMLWindow::SaveToFile (void)
{
	//FileDialog* dialog = new FileDialog(
	//	this, _("Select a file"), String(), String(), _T("*.html"),
	//	wxSAVE | wxOVERWRITE_PROMPT
	//);

	//bool result = false;
	//int answer = DialogDisplayer::ShowModal(dialog);
	//if (DialogDisplayer::IsModalAlive(dialog) && answer == wxID_OK) {
	//	const String& filename = dialog->GetPath();
	//	if (!filename.empty()) {
	//		PRINT_DEBUG("Saving to: \"%s\"", filename.c_str());
	//		result = this->SaveToFile(filename);
	//	}
	//}
	//DialogDisplayer::DestroyModal(dialog);
	//return result;

	return false;
}

//**********************************************************************

const String HTMLWindow::GetBodyStart (const String& title)
{
	// utf8
	//
	return _T("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 3.2//EN\"> <html> <head>")
		_T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf8\">")
		_T("<title>") + title + _T("</title> </head> <body>");

}

//**********************************************************************

const String HTMLWindow::GetBodyEnd (void)
{
	return _T("</body> </html>");
}

//**********************************************************************

void HTMLWindow::OnLinkClicked (const wxHtmlLinkInfo& link)
{
	if (!_EXECUTE_LINK(link.GetHref()))
		this->LoadPage(link.GetHref());
}

////////////////////////////////////////////////////////////////////////
// HTML Toolbar implementation
//
enum ToolIds { ID_SAVE, ID_PRINT, ID_CLOSE };

//**********************************************************************

//BEGIN_EVENT_TABLE(HTMLToolbar, Toolbar)
//	EVT_TOOL(ID_SAVE, HTMLToolbar::onSave)
//	EVT_TOOL(ID_PRINT, HTMLToolbar::onPrint)
//	EVT_TOOL(ID_CLOSE, HTMLToolbar::onClose)
//END_EVENT_TABLE();
//
////**********************************************************************
//
//HTMLToolbar::HTMLToolbar (wxWindow* parent, HTMLWindow* html) :
//	Toolbar(parent), m_html(html)
//{
//	this->createTools();
//	this->Realize();
//}
//
////**********************************************************************
//
//HTMLToolbar::~HTMLToolbar (void)
//{
//}
//
////**********************************************************************
//
//void HTMLToolbar::PerformZoom (void)
//{
//	this->Freeze();
//	this->ClearTools();
//
//	this->createTools();	
//
//	this->Realize();
//	this->GetParent()->Layout();
//	this->Thaw();
//}
//
////**********************************************************************
//
//void HTMLToolbar::createTools (void)
//{
//	this->SetToolBitmapSize(wxSize(p_toolSize, p_toolSize));
//	const String sizeStr(String::Format("%d", p_toolSize));
//
//	this->AddTool(ID_SAVE, _("Save"), wxBitmap(_T("save") + sizeStr),
//		_("Save this page"));
//
//	this->AddSeparator();
//	this->AddTool(ID_PRINT, _("Print"), wxBitmap(_T("print") + sizeStr),
//		_("Print this page"));
//
//	if (m_html->HasOnClose()) {
//		this->AddSeparator();
//		this->AddTool(ID_CLOSE, _("Close"), wxBitmap(_T("no") + sizeStr),
//			_("Close this page"));
//	}
//}
//
////**********************************************************************
//
//void HTMLToolbar::onSave (wxCommandEvent& event) { m_html->SaveToFile(); }
//void HTMLToolbar::onPrint (wxCommandEvent& event) { m_html->Print(); }
//void HTMLToolbar::onClose (wxCommandEvent& event) { m_html->CloseWindow(); }

////////////////////////////////////////////////////////////////////////

} // namespace gui
