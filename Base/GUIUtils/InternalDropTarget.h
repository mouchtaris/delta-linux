/**
 *	InternalDropTarget.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef GUI_INTERNALDROPTARGET_H
#define GUI_INTERNALDROPTARGET_H

#include "Common.h"
#include "LibraryAPIDecls.h"

#include <wx/dnd.h>

namespace gui {

////////////////////////////////////////////////////////////////////////

class _BASE_API InternalDropTarget : public wxDropTarget {
public:
	InternalDropTarget (const tchar* format);
	~InternalDropTarget (void);

	virtual bool DataDropped (wxCoord x, wxCoord y, const std::string& data) = 0;

protected:
	virtual wxDragResult	OnEnter (wxCoord x, wxCoord y, wxDragResult def);
	virtual wxDragResult	OnDragOver (wxCoord x, wxCoord y, wxDragResult def);
	virtual void			OnLeave (void);

	virtual wxDragResult	OnData (wxCoord x, wxCoord y, wxDragResult def);
};

////////////////////////////////////////////////////////////////////////

} // namespace gui

#endif // GUI_INTERNALDROPTARGET_H
