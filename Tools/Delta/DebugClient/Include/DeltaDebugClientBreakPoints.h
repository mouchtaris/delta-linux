// DeltaDebugClientBreakPoints.h
// Breakpoint holder instance for the debugger frontend.
// ScriptFighter Project.
// A. Savidis, December 2008.
//

#ifndef	DELTADEBUGCLIENTBREAKPOINTS_H
#define	DELTADEBUGCLIENTBREAKPOINTS_H

#include "DeltaStdDefs.h"
#include "DebugBreakPointHolder.h"
#include "DeltaDebuggerFrontendDefs.h"
#include "usingleton.h"

UGLOBALINSTANCE_CLASS_DEF(
	DVMDEBUGFRONTEND_CLASS DeltaDebugClientBreakPoints, 
	DebugBreakPointHolder
)

#endif	// Do not add stuff beyond this point.