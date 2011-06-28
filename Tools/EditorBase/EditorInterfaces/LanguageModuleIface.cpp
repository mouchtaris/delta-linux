/**
 *	LanguageModuleIface.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 *	Goto Definition additions, A. Savidis, March 2011.
 */

#include "LanguageModuleIface.h"

namespace editor {

////////////////////////////////////////////////////////////////////////
// LanguageModuleIface class
//
LanguageModuleIface::LanguageModuleIface (void) : 
	m_editor(0),
	gotoGlobalDefinitionCallback(0), 
	gotoGlobalDefinitionClosure(0),
	gotoPositionCallback(0), 
	gotoPositionClosure(0)
	{}

//**********************************************************************

LanguageModuleIface::~LanguageModuleIface (void)
{

}

////////////////////////////////////////////////////////////////////////

} // namespace editor
