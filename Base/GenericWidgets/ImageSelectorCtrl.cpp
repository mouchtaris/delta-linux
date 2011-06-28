/**
 *	ImageSelectorCtrl.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ImageSelectorCtrl.h"
#include <wx/imaglist.h>

namespace ide {

////////////////////////////////////////////////////////////////////////
// ImageSelectorCtrl class
//
ImageSelectorCtrl::ImageSelectorCtrl (wxWindow* parent, long flags, const wxSize& size) :
	wxBitmapComboBox(parent, wxID_ANY, String(), wxDefaultPosition, size, 0, NULL, flags)
{
}

//**********************************************************************

ImageSelectorCtrl::~ImageSelectorCtrl (void)
{

}

//**********************************************************************

bool ImageSelectorCtrl::Create (wxWindow* parent, long flags, const wxSize& size)
{
	if (
		!this->wxBitmapComboBox::Create(
			parent, wxID_ANY, String(), wxDefaultPosition, size, 0, NULL, flags
		)
	) {
		return false;
	}
	return true;
}

//**********************************************************************

void ImageSelectorCtrl::ShowImages (const wxImageList* images, const String* captions)
{
	for (int i = 0; i < images->GetImageCount(); ++i)
		this->Append(
			captions != 0 ? captions[i] : String::Format(_("Image %d"), i + 1),
			images->GetBitmap(i)
		);
	this->SetSelection(0);
}

////////////////////////////////////////////////////////////////////////

} // namespace talkie
