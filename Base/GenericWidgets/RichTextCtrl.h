/**
 *	RichTextCtrl.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_RICHTEXTCTRL_H
#define IDE_RICHTEXTCTRL_H

#include "Common.h"
#include "LibraryAPIDecls.h"

#pragma warning(disable: 4267)

#include <wx/richtext/richtextctrl.h>

namespace ide {

////////////////////////////////////////////////////////////////////////

class _BASE_API RichTextCtrl : public wxRichTextCtrl {
public:
	RichTextCtrl (void) {}
	RichTextCtrl (
			wxWindow*		parent,
			long			flags = wxRE_MULTILINE,
			const wxSize&	size = wxDefaultSize
	);
	~RichTextCtrl (void);

	bool Create (
			wxWindow*		parent,
			long			flags = wxRE_MULTILINE,
			const wxSize&	size = wxDefaultSize
	);
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_RICHTEXTCTRL_H
