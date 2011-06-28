/**
 *	TextCtrl.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_TEXTCTRL_H
#define IDE_TEXTCTRL_H

#include "Common.h"
#include "LibraryAPIDecls.h"

#include <wx/textctrl.h>

namespace ide {

////////////////////////////////////////////////////////////////////////

class _BASE_API TextCtrl : public wxTextCtrl {
public:
	TextCtrl (void) {}
	TextCtrl (
			wxWindow*		parent,
			long			flags = 0,
			const wxSize&	size = wxDefaultSize
	);
	~TextCtrl (void);

	bool Create (
			wxWindow*		parent,
			long			flags = 0,
			const wxSize&	size = wxDefaultSize
	);
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_TEXTCTRL_H
