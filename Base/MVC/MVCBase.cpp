/**
 *	MVCBase.cpp
 *
 *	-- ide application --
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "MVCBase.h"
#include <wx/window.h>

namespace ide {

////////////////////////////////////////////////////////////////////////

void ViewBase::ResetModel (void)
{
	this->OnResetModel();
	this->GetControllerBase()->ResetMonitoredModel();
}

//**********************************************************************

void ViewBase::FreezeUpdates (void) { this->GetWindow()->Freeze(); }
void ViewBase::ThawUpdates (void) { this->GetWindow()->Thaw(); }

////////////////////////////////////////////////////////////////////////

void ControllerBase::ResetMonitoredModel (void)
{
	this->OnResetMonitoredModel();
}

////////////////////////////////////////////////////////////////////////

} // namespace deltaide
