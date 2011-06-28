/**
 *	TextCtrl.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "TextCtrl.h"

namespace ide {

////////////////////////////////////////////////////////////////////////
// TextCtrl class
//
TextCtrl::TextCtrl (wxWindow* parent, long flags, const wxSize& size) :
	wxTextCtrl(parent, wxID_ANY, String(), wxDefaultPosition, size, flags)
{
}

//**********************************************************************

TextCtrl::~TextCtrl (void)
{

}

//**********************************************************************

bool TextCtrl::Create (wxWindow* parent, long flags, const wxSize& size)
{
	if (
		!this->wxTextCtrl::Create(
			parent, wxID_ANY, String(), wxDefaultPosition, size, flags
		)
	) {
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////

} // namespace talkie
