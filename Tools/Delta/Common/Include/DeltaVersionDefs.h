// DeltaVersionDefs.h
// Definitions (magic number and version tracking) for
// VM byte code.
// ScriptFighter Project.
// A. Savidis, February 2008.
//

#ifndef	DELTAVERSIONDEFS_H
#define	DELTAVERSIONDEFS_H

#include <stdio.h>
#include "utypes.h"
#include "ufiles.h"
#include "GenericWriter.h"

//-----------------------------------------------------------

#define	DELTA_MAGIC_NO			99512117	// 32 bits
#define	DELTA_VERSION_MAJOR_NO	5			// 16 bits
#define	DELTA_VERSION_MINOR_NO	27			// 16 bits

/////////////////////////////////////////////////////////////

#define	DELTA_ISVALID_MAGICNO(magic) \
	(magic) == DELTA_MAGIC_NO

#define	DELTA_IS_CURRENT_VERSION(major, minor) \
	((major) == DELTA_VERSION_MAJOR_NO && (minor) == DELTA_VERSION_MINOR_NO)

#define	DELTA_IS_NEWER_VERSION(major, minor) \
	((major) > DELTA_VERSION_MAJOR_NO || \
	 ((major) == DELTA_VERSION_MAJOR_NO && (minor) > DELTA_VERSION_MINOR_NO))

#define	DELTA_IS_OLDER_VERSION(major, minor) \
	((major) < DELTA_VERSION_MAJOR_NO || \
	 ((major) == DELTA_VERSION_MAJOR_NO && (minor) < DELTA_VERSION_MINOR_NO))

//-----------------------------------------------------------

#endif	// Do not ad stuff beyond this point.
