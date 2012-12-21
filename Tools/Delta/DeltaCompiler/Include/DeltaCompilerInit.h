// DeltaCompilerInit.h
// Global program start-up and ending actions.
// ScriptFigher Project.
// A. Savidis, May 2003.
//

#ifndef	DELTACOMPILERINIT_H
#define	DELTACOMPILERINIT_H

#include "DeltaCompilerDefs.h"

namespace DeltaCompilerInit {

	struct VMOptions {
		bool		debug;
		util_ui32	negotiationPort;

		VMOptions (void) : debug(false), negotiationPort(0) {}
	};

	DCOMPLIB_FUNC void Initialise (const VMOptions& vmOptions = VMOptions());
	DCOMPLIB_FUNC void CleanUp (void);
}

#endif	// Do not add stuff beyond this point.