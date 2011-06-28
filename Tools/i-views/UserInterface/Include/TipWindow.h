///////////////////////////////////////////////////////////////////////////////
// Name:        wx/tipwin.h
// Purpose:     wxTipWindow is a window like the one typically used for
//              showing the tooltips
// Author:      Vadim Zeitlin
// Modified by:
// Created:     10.09.00
// RCS-ID:      $Id: tipwin.h 53135 2008-04-12 02:31:04Z VZ $
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef	TIP_WINDOW_H
#define TIP_WINDOW_H

//you need to add defs with tipwind. Wx problem
#include <wx/defs.h>
#include <wx/tipwin.h>
#include <wx/popupwin.h>

namespace iviews {

class wxTipWindowView;

class TipWindow : public wxPopupTransientWindow {
public:
	virtual ~TipWindow ();
	TipWindow (
		wxWindow*		parent, 
		const wxString& text, 
		wxCoord			maxLength	= 100, 
		TipWindow**		windowPtr	= NULL, 
		wxRect *		rectBounds	= NULL
	);

    
    void				SetTipWindowPtr (TipWindow** windowPtr) { m_windowPtr = windowPtr; }
    void				SetBoundingRect (const wxRect& rectBound);
    void				Close (void);

protected:
    bool				CheckMouseInBounds(const wxPoint& pos);
    void				OnMouseClick(wxMouseEvent& event);
    virtual void		OnDismiss();

private:
    wxArrayString		m_textLines;
    wxCoord				m_heightLine;

    wxTipWindowView*	m_view;
    TipWindow**			m_windowPtr;
    wxRect				m_rectBound;


    DECLARE_EVENT_TABLE()
    friend class wxTipWindowView;
    DECLARE_NO_COPY_CLASS(TipWindow)
};






}	//namespace iviews 




#endif	//TIP_WINDOW_H
