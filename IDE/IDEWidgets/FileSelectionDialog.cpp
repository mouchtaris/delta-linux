/**
 *	FileSelectionDialog.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "FileSelectionDialog.h"
#include "CommonGUIUtils.h"
#include "ComponentRegistry.h"
#include "Algorithms.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

FileSelectionDialog::FileSelectionDialog (
		wxWindow*		parent,
		const String&	msg,
		const String&	dir,
		const String&	wildCard,
		long			style
	) : wxFileDialog(parent, msg, dir, wxEmptyString, wildCard, style)
{

}

//**********************************************************************

////////////////////////////////////////////////////////////////////////

} // namespace ide
