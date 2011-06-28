/**
 *	ItemListCtrl.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_ITEMLISTCTRL_H
#define IDE_ITEMLISTCTRL_H

#include "Common.h"
#include "LibraryAPIDecls.h"

#include <wx/listctrl.h>

namespace ide {

////////////////////////////////////////////////////////////////////////

#define IDE_ITEMLISTCTRL_FLAGS \
	wxLC_ICON | wxLC_AUTOARRANGE

////////////////////////////////////////////////////////////////////////

class _BASE_API ItemListCtrl : public wxListCtrl {
public:
	ItemListCtrl (void) {}
	ItemListCtrl (
		wxWindow*		parent,
		long			flags = IDE_ITEMLISTCTRL_FLAGS,
		const wxSize&	size = wxDefaultSize
	);
	~ItemListCtrl (void);

	bool Create (
		wxWindow*		parent,
		long			flags = IDE_ITEMLISTCTRL_FLAGS,
		const wxSize&	size = wxDefaultSize
	);

private:
	void setPlatformSpecificOptions (void);
};

////////////////////////////////////////////////////////////////////////

} // namespace talkie

#endif // IDE_ITEMLISTCTRL_H
