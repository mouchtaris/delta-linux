/**
 *	RichTextCtrl.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "RichTextCtrl.h"

namespace ide {

////////////////////////////////////////////////////////////////////////
// RichTextCtrl class
//
RichTextCtrl::RichTextCtrl (wxWindow* parent, long flags, const wxSize& size) :
	wxRichTextCtrl(parent, wxID_ANY, String(), wxDefaultPosition, size, flags)
{
}

//**********************************************************************

RichTextCtrl::~RichTextCtrl (void)
{

}

//**********************************************************************

bool RichTextCtrl::Create (wxWindow* parent, long flags, const wxSize& size)
{
	if (
		!this->wxRichTextCtrl::Create(
			parent, wxID_ANY, String(), wxDefaultPosition, size, flags
		)
	) {
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////

} // namespace talkie
