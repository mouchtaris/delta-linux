/**
 *	FileSelectionDialog.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_FILESELECTIONDIALOG_H
#define IDE_FILESELECTIONDIALOG_H

#include "Common.h"
#include "IDECommon.h"

#include <wx/filedlg.h>

namespace ide {

////////////////////////////////////////////////////////////////////////

class _IDE_API FileSelectionDialog : public wxFileDialog {
public:
	FileSelectionDialog (
		wxWindow*		parent,
		const String&	msg = _("Choose a file"),
		const String&	dir = String(),
		const String&	wildCard = _T("*.*"),
		long			style = wxFD_OPEN | wxFD_FILE_MUST_EXIST
	);
	~FileSelectionDialog (void) {}

private:
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_FILESELECTIONDIALOG_H
