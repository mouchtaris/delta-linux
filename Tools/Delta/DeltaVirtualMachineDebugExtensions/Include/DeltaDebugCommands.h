// DeltaDebugCommands.h
// Contains the message types that are to be communicated between
// the debug client and the debug server.
// ScriptFighter Project.
// A. Savidis, December 1999 (original version).
// Extended to support multiple expr requests with asingle message (July 2008).
// Extended to support type data information for expression (August 2008).
//
#ifndef	DELTADEBUGCOMMANDS_H
#define	DELTADEBUGCOMMANDS_H

#include "utypes.h"
#include "ufunctors.h"
#include <list>
#include <map>

enum DebugClientToServerRequest {

	// Those are execution control commands. They are
	// available only when the execution is in trace mode
	// (stopped at a breakpoint).
	//
	Debug_Go							= 0,		// Continue execution from current point.
	Debug_StepOver						= 1,		// Next stmt, not entering func calls.
	Debug_StepIn						= 2,		// Next stmt in code, entering func calls.
	Debug_SelectiveStepIn				= 3,		// Selective stepping into a call .
	Debug_RunTo							= 4,		// Continue execution until target line met.
	Debug_StepOut						= 5,		// Exit current call context.

	// Tose are context control commands. They affect the current context 
	// of the executing VM, in the call stack, in which any requests for formal 
	// or local arguments are to be made. Available only in trace mode.
	// Used to extract call stack and set as context a record from the call stack.
	//
	Debug_StackUp						= 6,		// Move to next activation record.
	Debug_StackDown						= 7,		// Move to previous activation record.
	Debug_MultipleStackUp				= 8,		// Move to next activation record n times.
	Debug_MultipleStackDown				= 9,		// Move to previous activation record n times.

	// Value request commands. Asking for the content of run-time variables. 
	// Execution should be in trace mode.
	//
	Debug_GetLocal						= 10,		// Get value of a local variable (name, scope).
	Debug_GetGlobal						= 11,		// Get value of global (name).
	Debug_GetActual						= 12,		// Get value of an actual (name).
	Debug_GetExpr						= 13,		// Get the value of an expression.
	Debug_GetExprMany					= 14,		// Get the value of many expressions (list).
	Debug_GetExprTypeData				= 15,		// Get structured information of an expression.
	Debug_GetBinaryData					= 16,		// Get binary data (if any) for an extern id.
	Debug_GetExprTypeDataMany			= 17,		// Get structured information of many expressions.
	Debug_SetObjectGraphConfiguration	= 18,		// Set config parms of GetObjectGraph
	Debug_GetObjectGraph				= 19,		// Get an object graph from an object expression.
	Debug_GetVariables					= 20,		// Get all local variable at current context.
	Debug_GetAllPossibleCalls			= 21,		// Get all calls at current stmt that may be executed.

	// Break point control. Those may be called at any point in time.
	//
	Debug_AddBreakPoint					= 22,	// Add a break-point.
	Debug_RemoveBreakPoint				= 23,	// Remove a break-point.
	Debug_EnableBreakPoint				= 24,	// Enables a break-point.
	Debug_ChangeBreakPointCondition		= 25,	// Enables a break-point.
	Debug_DisableBreakPoint				= 26,	// Disables a break-point.
	Debug_EnableAllBreakPoints			= 27,	// Enables all break-points.
	Debug_DisableAllBreakPoints			= 28,	// Disables all break-points.
	Debug_RemoveAllBreakPoints			= 29,	// Delete all break-points.

	Debug_Start							= 30,	// Start execution (optional).
	Debug_Stop							= 31,	// Stop execution and exit.
	Debug_BreakExecution				= 32,	// Break execution as soon as possible.
	Debug_GetDynamicCode				= 33,	// Requesting dynamic source.
	Debug_SetToStringMaxLength			= 34,	// Settting max length for string conversion of values.
	Debug_SetBreakOnThrownException		= 35,	// Settting max length for string conversion of values.

	Debug_RequestInvalid				= 36	// Invalid request.
};

///////////////////////////////////////////////////////////////////
// All Sync requests below are allowed ONLY WHEN we are in breakpoint,
// meaning after a stop point message is received.

enum DebugServerToClientResponse {

	Debug_InfoStopPoint					= 0,	// Async.	Stop in a break point. 
	Debug_InfoInvalidBreakPoint			= 1,	// Async.	The recently added break point was invalid. 
	Debug_InfoValidBreakPoint			= 2,	// Async.	The recently added break point was valid. 
	Debug_InfoGlobalContext				= 3,	// Sync.	A context control command (normally previous rec) reached global level.
	Debug_InfoTopContext				= 4,	// Sync.	A context control command (normally next rec) cannot go higher to the call stack. 
	Debug_InfoCurrFunction				= 5,	// Sync.	The current activation record (func, def line, call line, scope, call signature). 
	Debug_InfoValue						= 6,	// Sync.	Returns the content of a variable or expression result. 
	Debug_InfoBinaryData				= 7,	// Sync.	Returns the binary data of an extern id.
	Debug_InfoValueMany					= 8,	// Sync.	Returns the value of many expressions. 
	Debug_InfoExprTypeData				= 9,	// Sync.	Returns the type data of an expression. 
	Debug_InfoExprTypeDataMany			= 10,	// Sync.	Returns the type data of many expressios.
	Debug_InfoObjectGraph				= 11,	// Sync.	Returns the requested object graph.
	Debug_InfoErrorValue				= 12,	// Sync.	Signifies an error during expression evaluation.
	Debug_InfoStop						= 13,	// Async.	The running application is about to exit.
	Debug_InfoError						= 14,	// Async.	A runtime error reported.
	Debug_InfoBreakPointConditionError	= 15,	// Async.	The break point condition evaluation had errors.
	Debug_InfoMostRecentFuncResults		= 16,	// Sync.	After a stop in a break point. 
	Debug_InfoWarning					= 17,	// Async.	Runtime warning.
	Debug_InfoVariables					= 18,	// Sync.	After a request for variables. 
	Debug_InfoAllPossibleCalls			= 19,	// Sync.	After a request for all possible calls. 
	Debug_InfoDynamicCode				= 20,	// Async.	Dynamic source content.
	Debug_InfoBreakPointError			= 21,	// Async	The last request for breakpoints concerned an invalid breakpoint. 
	Debug_ResponseInvalid				= 22	// Invalid response.
};

////////////////////////////////////////////////////////

#define	DEBUG_COMMAND_GETOBJECTGRAPH_DEPTH_UNLIMITED	0
#define	DEBUG_COMMAND_ADDBREAKPOINT_NO_CONDITION		""
#define	DEBUG_CLIENT_NEGOTIATIONPORT_CMMDARG			"--negotiationport"

////////////////////////////////////////////////////////

namespace DeltaDebugger {
	typedef uquintuple<std::string, bool, std::string, bool, std::string>	ObjectSlot;
	typedef std::list<ObjectSlot>											ObjectNode;
	typedef std::map<std::string,  ObjectNode>								ObjectGraph;
}

////////////////////////////////////////////////////////

#define	DEBUG_OBJECT_ABSREF_PREFIX						"_2object_"
#define	DEBUG_EXTERNID_ABSREF_PREFIX					"_2externid_"
#define	DEBUG_METHODFUNC_ABSREF_PREFIX					"_2method_"
#define	DEBUG_PROGRAMFUNC_ABSREF_PREFIX					"_2function_"
#define	DEBUG_LIBRARYFUNC_ABSREF_PREFIX					"_2libfunc_"

#define	DEBUG_KEYTYPE_OBJECT_PSEUDO_KEY					"OBJECT_PSEUDO_KEY"
#define	DEBUG_KEYTYPE_OBJECT_INHERITANCE_KEY			"OBJECT_INHERITANCE_PSEUDO_KEY"
#define	DEBUG_KEYTYPE_OBJECT_DELEGATION_KEY				"OBJECT_DELEGATION_PSEUDO_KEY"
#define	DEBUG_KEYTYPE_OBJECT_ATTRIBUTE_KEY				"OBJECT_ATTRIBUTE_KEY"
#define	DEBUG_KEYTYPE_OBJECT_OPERATOR_KEY				"OBJECT_OPERATOR_KEY"
#define	DEBUG_KEYTYPE_EXTERNID_PSEUDO_KEY				"EXTERNID_PSEUDO_KEY"
#define	DEBUG_KEYTYPE_EXTERNID_USER_PSEUDO_KEY			"EXTERNID_USER_PSEUDO_KEY"
#define	DEBUG_KEYTYPE_AGGREGATE_PSEUDO_KEY				"AGGREGATE_PSEUDO_KEY"
#define	DEBUG_KEYTYPE_NONE								""

////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
