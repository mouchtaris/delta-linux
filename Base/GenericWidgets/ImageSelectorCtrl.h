/**
 *	ImageSelectorCtrl.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_TEXTCTRL_H
#define IDE_TEXTCTRL_H

#include "Common.h"
#include "LibraryAPIDecls.h"

#include <wx/bmpcbox.h>
class wxImageList;
class wxBitmap;

namespace ide {

////////////////////////////////////////////////////////////////////////

#define IDE_IMAGESELECTORCTRL_FLAGS \
	wxCB_READONLY | wxCB_SIMPLE

////////////////////////////////////////////////////////////////////////

class _BASE_API ImageSelectorCtrl : public wxBitmapComboBox {
public:
	ImageSelectorCtrl (void) {}
	ImageSelectorCtrl (
			wxWindow*		parent,
			long			flags = IDE_IMAGESELECTORCTRL_FLAGS,
			const wxSize&	size = wxDefaultSize
	);
	~ImageSelectorCtrl (void);

	bool Create (
			wxWindow*		parent,
			long			flags = IDE_IMAGESELECTORCTRL_FLAGS,
			const wxSize&	size = wxDefaultSize
	);

	//******************************************************************

	void ShowImages (const wxImageList* images, const String* captions);
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_TEXTCTRL_H
