/**
 *	HTMLExt.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_HTMLEXT_H
#define IDE_HTMLEXT_H

#include "Common.h"
#include "IDECommon.h"
#include "HTMLWindow.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

class _IDE_API HTMLWindowExt : public gui::HTMLWindow {
public:
	HTMLWindowExt (void) {}
	HTMLWindowExt (wxWindow* parent);
	~HTMLWindowExt (void) {}

	bool Create (wxWindow* parent);
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_HTMLEXT_H
