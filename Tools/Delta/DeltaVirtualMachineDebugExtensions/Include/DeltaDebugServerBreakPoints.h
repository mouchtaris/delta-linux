// DeltaDebugServerBreakPoints.h
// Breakpoint holder instance for the debugger backend.
// ScriptFighter Project.
// A. Savidis, December 2008.
//

#ifndef	DELTADEBUGSERVERBREAKPOINTS_H
#define	DELTADEBUGSERVERBREAKPOINTS_H

#include "DeltaStdDefs.h"
#include "DebugBreakPointHolder.h"
#include "usingleton.h"

UGLOBALINSTANCE_CLASS_DEF(
	DVM_CLASS DeltaDebugServerBreakPoints, 
	DebugBreakPointHolder
)

#endif	// Do not add stuff beyond this point.