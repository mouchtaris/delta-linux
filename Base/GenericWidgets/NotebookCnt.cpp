/**
 *	NotebookCnt.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "NotebookCnt.h"

namespace ide {

////////////////////////////////////////////////////////////////////////
// NotebookCnt class
//
NotebookCnt::NotebookCnt (wxWindow* parent, long flags, const wxSize& size) :
	wxAuiNotebook(parent, wxID_ANY, wxDefaultPosition, size, flags)
{
}

//**********************************************************************

NotebookCnt::~NotebookCnt (void)
{

}

//**********************************************************************

bool NotebookCnt::Create (wxWindow* parent, long flags, const wxSize& size)
{
	if (!this->wxAuiNotebook::Create(parent, wxID_ANY, wxDefaultPosition, size, flags)) {
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////

} // namespace talkie
