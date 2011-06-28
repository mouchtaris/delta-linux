///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/tipwin.cpp
// Purpose:     implementation of TipWindow
// Author:      Vadim Zeitlin
// Modified by:
// Created:     10.09.00
// RCS-ID:      $Id: tipwin.cpp 43033 2006-11-04 13:31:10Z VZ $
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include <wx/dc.h>
#include <wx/timer.h>
#include <wx/dcclient.h>
#include <wx/settings.h>


#include "TipWindow.h"

namespace iviews {


static const wxCoord TEXT_MARGIN_X = 3;
static const wxCoord TEXT_MARGIN_Y = 3;



class wxTipWindowView : public wxWindow {
public:
    wxTipWindowView(wxWindow *parent);

    // event handlers
    void 		OnPaint(wxPaintEvent& event);
    void 		OnMouseClick(wxMouseEvent& event);
    void 		OnMouseMove(wxMouseEvent& event);
    void 		Adjust(const wxString& text, wxCoord maxLength);

private:
    TipWindow*	m_parent;

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxTipWindowView)
};


BEGIN_EVENT_TABLE(TipWindow, wxPopupTransientWindow)
    EVT_LEFT_DOWN	(TipWindow::OnMouseClick)
    EVT_RIGHT_DOWN	(TipWindow::OnMouseClick)
    EVT_MIDDLE_DOWN	(TipWindow::OnMouseClick)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxTipWindowView, wxWindow)
    EVT_PAINT		(wxTipWindowView::OnPaint)
    EVT_LEFT_DOWN	(wxTipWindowView::OnMouseClick)
    EVT_RIGHT_DOWN	(wxTipWindowView::OnMouseClick)
    EVT_MIDDLE_DOWN	(wxTipWindowView::OnMouseClick)
    EVT_MOTION		(wxTipWindowView::OnMouseMove)
END_EVENT_TABLE()

//-----------------------------------------------------------------------

TipWindow::TipWindow(
	wxWindow *		parent,
    const wxString& text,
    wxCoord			maxLength,
    TipWindow**		windowPtr,
    wxRect *		rectBounds
) : wxPopupTransientWindow(parent) {
    SetTipWindowPtr(windowPtr);
    if ( rectBounds )
        SetBoundingRect(*rectBounds);

    // set colours
    SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));

    // set size, position and show it
    m_view = new wxTipWindowView(this);
    m_view->Adjust(text, maxLength);
    m_view->SetFocus();

    int x, y;
    wxGetMousePosition(&x, &y);

    // we want to show the tip below the mouse, not over it
    //
    // NB: the reason we use "/ 2" here is that we don't know where the current
    //     cursors hot spot is... it would be nice if we could find this out
    //     though
    y += wxSystemSettings::GetMetric(wxSYS_CURSOR_Y) / 2;

    Position(wxPoint(x, y), wxSize(0,0));
    Popup(m_view);
}

//-----------------------------------------------------------------------

TipWindow::~TipWindow() {
    if ( m_windowPtr )
		*m_windowPtr = NULL;
}

//-----------------------------------------------------------------------

void TipWindow::OnMouseClick(wxMouseEvent& event) { 
	wxWindow * parent = GetParent();

	Close(); 
	if (event.RightIsDown())
		parent->ProcessEvent(event);
}

//-----------------------------------------------------------------------

void TipWindow::OnDismiss() 
	{ Close(); }

//-----------------------------------------------------------------------

void TipWindow::SetBoundingRect(const wxRect& rectBound)
	{ m_rectBound = rectBound; }

//-----------------------------------------------------------------------

void TipWindow::Close() {
    if ( m_windowPtr ) {
		*m_windowPtr = NULL;
		m_windowPtr = NULL;
    }

    Show(false);
    Destroy();
}

//-----------------------------------------------------------------------

wxTipWindowView::wxTipWindowView(wxWindow *parent)
:	wxWindow(
		parent, 
		wxID_ANY,
		wxDefaultPosition, 
		wxDefaultSize,
		wxNO_BORDER
) {
    // set colours
    SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
    m_parent = (TipWindow*)parent;
}

//-----------------------------------------------------------------------

void wxTipWindowView::Adjust(const wxString& text, wxCoord maxLength)
{
    wxClientDC dc(this);
    dc.SetFont(GetFont());

    // calculate the length: we want each line be no longer than maxLength
    // pixels and we only break lines at words boundary
    wxString current;
    wxCoord height, 
			width,
            widthMax		= 0;
    m_parent->m_heightLine	= 0;

    bool breakLine = false;
    for (const wxChar *p = text.c_str(); ; p++ ){
        if ( *p == _T('\n') || *p == _T('\0') ) {
            dc.GetTextExtent(current, &width, &height);
            if ( width > widthMax )
                widthMax = width;

            if ( height > m_parent->m_heightLine )
                m_parent->m_heightLine = height;

            m_parent->m_textLines.Add(current);

            if (!*p)
                break;	// end of text


            current.clear();
            breakLine = false;
        }
        else 
		if ( breakLine && (*p == _T(' ') || *p == _T('\t')) ) {
            // word boundary - break the line here
            m_parent->m_textLines.Add(current);
            current.clear();
            breakLine = false;
        }
		else { // line goes on
            current += *p;
            dc.GetTextExtent(current, &width, &height);
            if (width > maxLength)
                breakLine = true;

            if (width > widthMax)
                widthMax = width;

            if (height > m_parent->m_heightLine)
                m_parent->m_heightLine = height;
        }
    }

    // take into account the border size and the margins
    width  = 2*(TEXT_MARGIN_X + 1) + widthMax;
    height = 2*(TEXT_MARGIN_Y + 1) + wx_truncate_cast(wxCoord, m_parent->m_textLines.GetCount())*m_parent->m_heightLine;
    m_parent->SetClientSize(width, height);
    SetSize(0, 0, width, height);
}

//-----------------------------------------------------------------------

void wxTipWindowView::OnPaint(wxPaintEvent& WXUNUSED(event)) {
    wxPaintDC dc(this);

    wxRect rect;
    wxSize size = GetClientSize();
    rect.width = size.x;
    rect.height = size.y;

    // first filll the background
    dc.SetBrush(wxBrush(GetBackgroundColour(), wxSOLID));
    dc.SetPen( wxPen(GetForegroundColour(), 1, wxSOLID) );
    dc.DrawRectangle(rect);

    // and then draw the text line by line
    dc.SetTextBackground(GetBackgroundColour());
    dc.SetTextForeground(GetForegroundColour());
    dc.SetFont(GetFont());

    wxPoint pt;
    pt.x = TEXT_MARGIN_X;
    pt.y = TEXT_MARGIN_Y;
    size_t count = m_parent->m_textLines.GetCount();
    
	for ( size_t n = 0; n < count; n++ ) {
        dc.DrawText(m_parent->m_textLines[n], pt);
        pt.y += m_parent->m_heightLine;
    }
}

//-----------------------------------------------------------------------

void wxTipWindowView::OnMouseClick(wxMouseEvent& event){ 
	if (event.RightIsDown())
		GetParent()->ProcessEvent(event);
	else
		m_parent->Close(); 
}

//-----------------------------------------------------------------------

void wxTipWindowView::OnMouseMove(wxMouseEvent& event) {
    const wxRect& rectBound = m_parent->m_rectBound;

    if (rectBound.width &&
        !rectBound.Contains(ClientToScreen(event.GetPosition())) 
		)
        m_parent->Close();	        // mouse left the bounding rect, disappear
    else
        event.Skip();
}


}	//namespace iviews