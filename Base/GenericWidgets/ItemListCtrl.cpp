/**
 *	ItemListCtrl.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ItemListCtrl.h"

namespace ide {

////////////////////////////////////////////////////////////////////////
// ItemListCtrl class
//
ItemListCtrl::ItemListCtrl (wxWindow* parent, long flags, const wxSize& size) :
	wxListCtrl(parent, wxID_ANY, wxDefaultPosition, size, flags)
{
	this->setPlatformSpecificOptions();
}

//**********************************************************************

ItemListCtrl::~ItemListCtrl (void)
{

}

//**********************************************************************

bool ItemListCtrl::Create (wxWindow* parent, long flags, const wxSize& size)
{
	if (!this->wxListCtrl::Create(parent, wxID_ANY, wxDefaultPosition, size, flags))
		return false;
	this->setPlatformSpecificOptions();
	return true;
}

//**********************************************************************

void ItemListCtrl::setPlatformSpecificOptions (void)
{
	//const long LVS_EX_LABELTIP = 16384;

	//long style = SendMessage(this->GetHandle(), LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
	//style |= LVS_EX_LABELTIP;
	//SendMessage(this->GetHandle(), LVM_SETEXTENDEDLISTVIEWSTYLE, 0, style);
}

////////////////////////////////////////////////////////////////////////

} // namespace talkie
