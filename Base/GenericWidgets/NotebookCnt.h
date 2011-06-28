/**
 *	NotebookCnt.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_NOTEBOOKCNT_H
#define IDE_NOTEBOOKCNT_H

#include "Common.h"
#include "LibraryAPIDecls.h"
#include "UndefineAllWin32Shit.h"

#include <wx/aui/auibook.h>

namespace ide {

////////////////////////////////////////////////////////////////////////

#define IDE_NOTEBOOKCNT_FLAGS	wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_CLOSE_BUTTON | wxAUI_NB_WINDOWLIST_BUTTON

////////////////////////////////////////////////////////////////////////

class _BASE_API NotebookCnt : public wxAuiNotebook {
public:
	NotebookCnt (void) {}
	NotebookCnt (
		wxWindow*		parent,
		long			flags = IDE_NOTEBOOKCNT_FLAGS,
		const wxSize&	size = wxDefaultSize
	);
	~NotebookCnt (void);

	bool Create (
		wxWindow*		parent,
		long			flags = IDE_NOTEBOOKCNT_FLAGS,
		const wxSize&	size = wxDefaultSize
	);
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_NOTEBOOKCNT_H
