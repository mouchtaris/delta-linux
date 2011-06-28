/**
 *	InternalDropTarget.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "InternalDropTarget.h"
#include "InternalDataObject.h"
#include "Streams.h"

#include <string.h>

namespace gui {

////////////////////////////////////////////////////////////////////////
// InternalDropTarget class
//

InternalDropTarget::InternalDropTarget (const tchar* format) :
	wxDropTarget(new InternalDataObject(format))
{

}

//**********************************************************************

InternalDropTarget::~InternalDropTarget (void)
{

}

//**********************************************************************

wxDragResult InternalDropTarget::OnEnter (wxCoord x, wxCoord y, wxDragResult def)
{
	DBGOUT << "ON ENTER: " << x << ", " << y << DBGENDL;

	return this->OnDragOver(x, y, def);
}

//**********************************************************************

wxDragResult InternalDropTarget::OnDragOver (wxCoord PORT_UNUSED_PARAM(x), wxCoord PORT_UNUSED_PARAM(y), wxDragResult def)
{
	return def;
}

//**********************************************************************

void InternalDropTarget::OnLeave (void)
{
	DBGOUT << "ON LEAVE" << DBGENDL;
}

//**********************************************************************

wxDragResult InternalDropTarget::OnData (wxCoord x, wxCoord y, wxDragResult def)
{
	if (!this->GetData())
		return wxDragNone;

	InternalDataObject* object = static_cast<InternalDataObject*>(this->GetDataObject());
	return this->DataDropped(x, y, object->GetData()) ? def : wxDragNone;
}

////////////////////////////////////////////////////////////////////////

} // namespace gui
