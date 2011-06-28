/**
 *	ExportedFunctionSetup.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_EXPORTEDFUNCTIONSETUP_H
#define IDE_EXPORTEDFUNCTIONSETUP_H

#include "Common.h"

namespace ide {

////////////////////////////////////////////////////////////////////////
// Maximum arguments supported by exported functions
//
#define IDE_EXPORTEDFUNCTION_MAX_ARGS 6

////////////////////////////////////////////////////////////////////////
// Common functionality used in stubs and skeletons
//
#define IDE_VALUE_TYPE(varType) \
	typename boost::remove_const< typename boost::remove_reference<varType>::type >::type

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_EXPORTEDFUNCTIONSETUP_H
