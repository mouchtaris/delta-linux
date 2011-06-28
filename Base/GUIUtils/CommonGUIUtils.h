/**
 *	CommonGUIUtils.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef UTIL_COMMONGUIUTILS_H
#define UTIL_COMMONGUIUTILS_H

#include "Common.h"
#include <wx/window.h>

namespace gui {

////////////////////////////////////////////////////////////////////////

class scoped_freeze {
public:
	scoped_freeze (wxWindow* win) : m_win(win) { m_win->Freeze(); }
	~scoped_freeze (void) throw() { m_win->Thaw(); }

private:
	wxWindow* m_win;
};

//**********************************************************************

#define GUI_SCOPED_FREEZE(WIN) gui::scoped_freeze raii_gui_freezer(WIN)

////////////////////////////////////////////////////////////////////////

} // namespace gui

#endif // UTIL_COMMONGUIUTILS_H
