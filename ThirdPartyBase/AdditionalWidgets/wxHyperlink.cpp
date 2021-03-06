//////////////////////////////////////////////////////////////////////////////
// File:        hyperlink.cpp
// Purpose:     wxHyperLink control
// Maintainer:  Wyo
// Created:     2003-04-07
// RCS-ID:      $Id: hyperlink.cpp,v 1.5 2005/09/06 19:21:54 wyo Exp $
// Copyright:   (c) 2004 wxCode
// Licence:     wxWindows
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// information
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// headers
//----------------------------------------------------------------------------

#pragma warning (disable:4996)

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all 'standard' wxWidgets headers)
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

// wxWidgets headers
#include <wx/mimetype.h> // mimetype support

#ifdef __WXMSW__
    #include "wx/msw/registry.h"
#endif

// hyperlink headers
#include "wxHyperlink.h"   // wxHyperLink control

//----------------------------------------------------------------------------
// resources
//----------------------------------------------------------------------------


//============================================================================
// declarations
//============================================================================


//============================================================================
// implementation
//============================================================================

//----------------------------------------------------------------------------
// wxHyperLink
//----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS (wxHyperLink, wxControl)

BEGIN_EVENT_TABLE    (wxHyperLink, wxControl)
    EVT_LEFT_DCLICK  (wxHyperLink::OnLinkActivate)
    EVT_LEFT_DOWN    (wxHyperLink::OnLinkActivate)
    EVT_PAINT        (wxHyperLink::OnPaint)
    EVT_ENTER_WINDOW (wxHyperLink::OnWindowEnter)
    EVT_LEAVE_WINDOW (wxHyperLink::OnWindowLeave)
END_EVENT_TABLE()

bool wxHyperLink::Create (wxWindow *parent,
                          wxWindowID id,
                          const wxString &label,
                          const wxPoint &pos,
                          const wxSize &size,
                          long style,
                          const wxString &name) {
    bool okay = FALSE;

    // create static text
    okay = wxControl::Create (parent, id, pos, size, style|wxNO_BORDER, wxDefaultValidator, name);
    wxASSERT_MSG (okay, _T("Failed to create wxControl, needed by wxHyperLink!"));

    // initialize variables
    SetLabel (label);
    m_URL = wxEmptyString;
    m_Visited = false;
    m_HoverCursor = wxCursor (wxCURSOR_HAND);
    m_HoverColour = wxColour (_T("RED"));
    m_NormalColour = wxColour (_T("BLUE"));
    m_VisitedColour = wxColour (_T("PURPLE"));

    // set foreground colour
    SetForegroundColour (m_NormalColour);
    wxFont font = GetFont();
    font.SetUnderlined (true);
    SetFont (font);

    // get background colour
    m_BackgroundColour = GetBackgroundColour ();

    // determin and set minimal size
    wxClientDC dc (this);
    dc.SetFont(GetFont());
    dc.GetTextExtent (GetLabel(), &m_width, &m_height);
    SetBestSize (DoGetBestSize());

    return okay;
} // Create

//----------------------------------------------------------------------------
// event handlers

void wxHyperLink::OnLinkActivate (wxMouseEvent &WXUNUSED(event)) {
    m_Visited = TRUE;
    SetForegroundColour (m_VisitedColour);
    SetBackgroundColour (m_BackgroundColour);
    Refresh();
    if (m_URL.IsEmpty()) {
       ExecuteLink (GetLabel());
    }else{
       ExecuteLink (m_URL);
    }
}

void wxHyperLink::OnPaint (wxPaintEvent &WXUNUSED(event)) {
    wxPaintDC dc(this);
#if !wxCHECK_VERSION(2, 9, 0)
    dc.BeginDrawing();
#endif
    dc.SetFont (GetFont());
    dc.SetTextForeground (GetForegroundColour());
    dc.DrawText (GetLabel(), 0, 0);
#if !wxCHECK_VERSION(2, 9, 0)
    dc.EndDrawing();
#endif
}

void wxHyperLink::OnWindowEnter (wxMouseEvent &WXUNUSED(event)) {
    SetCursor (m_HoverCursor);
    SetForegroundColour (m_HoverColour);
    Refresh();
}

void wxHyperLink::OnWindowLeave (wxMouseEvent &WXUNUSED(event)) {
    SetCursor (wxNullCursor);
    SetForegroundColour (!m_Visited? m_NormalColour: m_VisitedColour);
    Refresh();
}

//----------------------------------------------------------------------------
// size functions

wxSize wxHyperLink::DoGetBestSize() const {
    return wxSize (m_width, m_height);
}

//----------------------------------------------------------------------------
// settings functions

wxCursor wxHyperLink::GetHoverCursor () {
    return m_HoverCursor;
}

void wxHyperLink::SetHoverCursor (wxCursor cursor) {
    m_HoverCursor = cursor;
}

wxColour wxHyperLink::GetHoverColour () {
    return m_HoverColour;
}

void wxHyperLink::SetHoverColour (wxColour colour) {
    m_HoverColour = colour;
}

wxColour wxHyperLink::GetNormalColour () {
    return m_NormalColour;
}

void wxHyperLink::SetNormalColour (wxColour colour) {
    m_NormalColour = colour;
    if (!m_Visited) {
        SetForegroundColour (m_NormalColour);
        Refresh();
    }
}

wxColour wxHyperLink::GetVisitedColour () {
    return m_VisitedColour;
}

void wxHyperLink::SetVisitedColour (wxColour colour) {
    m_VisitedColour = colour;
    if (m_Visited) {
        SetForegroundColour (m_VisitedColour);
        Refresh();
    }
}

wxString wxHyperLink::GetURL () {
    return m_URL;
}

void wxHyperLink::SetURL (const wxString &url) {
    m_URL = url;
}

//----------------------------------------------------------------------------
// static functions

bool wxHyperLink::ExecuteLink (const wxString &link) {
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

