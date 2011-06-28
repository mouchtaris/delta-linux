// DebugClient.h
// The API to be used for the implementation of the debugger
// interface which serves as the debug client (communicating
// remotely via sockets with the debug server).
// ScriptFighter Project.
// A. Savidis, February 2002.
//

#ifndef	DELTADEBUGCLIENT_H
#define	DELTADEBUGCLIENT_H

#include "utypes.h"
#include "ufunctors.h"
#include "DeltaDebugCommands.h"
#include "DeltaDebuggerFrontendDefs.h"
#include "DeltaStdDefs.h"
#include <string>
#include <list>
#include <functional>

#define	DS2CR \
	DebugServerToClientResponse

///////////////////////////////////////////////////////////////////
// This is the class that the debugger interface will use to 
// implement all the various debug functions.
//
class DVMDEBUGFRONTEND_CLASS DeltaDebugClient {

	private:
	static void PostAllBreakoints (void);

	static bool	GetResponse (
					DebugServerToClientResponse cmmd,
					std::string*				content
				);

	static bool	GetResponse (
					DebugServerToClientResponse cmmd,
					std::list< std::pair<std::string, std::string> >& contents
				);

	static bool GetResponse (
					DebugServerToClientResponse cmmd,
					std::list< std::pair<std::string, bool> >& contents
				);

	/////////////////////////////////////////////////////////////////////

	public:
	struct DVMDEBUGFRONTEND_CLASS BrokenConnection {};

	// Start-up, connection and clean-up (auto disconnects).
	//
	static void			Initialise (void);
	static bool			Connect (const std::string& host, util_ui32 port);
	static void			CleanUp (void);

	class DVMDEBUGFRONTEND_CLASS ServerPortNegotiation {
		public:
		static util_ui32			Start (void);				// Returns the negotiation port.
		static util_ui32			Finish (void);				// Returns the debug server port.
		static const std::string	ToArg (util_ui32 port);		// For addition as a cmmd line arg.
	};

	// Trace control.
	static void			DoGo (void);		
	static void			DoStepOver (void);	
	static void			DoStepIn (void);	
	static void			DoGetAllPossibleCalls (void);
	static void			DoSelectiveStepIn (util_ui32 callOrder);	
	static void			DoRunTo (util_ui32 line);		
	static void			DoStepOut (void);
	static void			DoStart (void);	
	static void			DoStop (void);	
	static void			DoBreakExecution (void);	

	// Variable / expression value request and string conversion size.
	static void			DoGetGlobal (const std::string& name);
	static void			DoGetLocal (const std::string& name);
	static void			DoGetActual (const std::string& name);
	static void			DoGetExpr (const std::string& expr);
	static void			DoGetExprMany (const std::list<std::string>& exprs);
	static void			DoGetExprTypeData (const std::string& formatId, const std::string& expr);
	static void			DoGetExprTypeDataMany (const std::string& formatId, const std::list<std::string>& exprs);
	static void			DoGetBinaryData (const std::string& expr);
	static void			DoSetObjectGraphConfiguration (
							bool	expandLibraryFuncs,
							bool	expandMethodFuncs,
							bool	expandProgramFuncs,
							bool	expandFuncVMs
						);
	static void			DoGetObjectGraph (const std::string& expr, util_ui32 depth = DEBUG_COMMAND_GETOBJECTGRAPH_DEPTH_UNLIMITED, bool directed = true);
	static void			DoGetVariables (void);
	static void			DoSetToStringMaxLength (util_ui32 maxLen);
	static void			DoSetBreakOnThrownException (bool val);

	// Dynamic code management.
	static void			DoGetDynamicCode (void);

	// Context control.
	static void			DoStackUp (void);
	static void			DoStackDown (void);	
	static void			DoMultipleStackUp (util_ui32 n);
	static void			DoMultipleStackDown (util_ui32 n);	

	// Break point control.
	static void			DoAddBreakPoint (const std::string& source, util_ui32 line, const std::string& condition = DEBUG_COMMAND_ADDBREAKPOINT_NO_CONDITION);
	static void			DoChangeBreakPointCondition (const std::string& source, util_ui32 line, const std::string& condition);
	static void			DoRemoveBreakPoint (const std::string& source, util_ui32 line);		
	static void			DoEnableBreakPoint (const std::string& source, util_ui32 line);		
	static void			DoDisableBreakPoint (const std::string& source, util_ui32 line);
	static void			DoEnableAllBreakPoints (const std::string& source);	
	static void			DoDisableAllBreakPoints (const std::string& source);	
	static void			DoRemoveAllBreakPoints (const std::string& source);	


	// Checking for response availability, doing local receipt and querying type.
	static bool			IsResponsePending (void);	// Check this first.
	static bool			ReceiveResponse (void);		// Then receive response locally.
	static DS2CR		GetResponseType (void);		// Query response always after receiving.
	static void			ResponseProcessed (void);	// Call this at the end, after GetInfo called.

	// Response specific receive functions. Return true if
	// message type received matches the function call, else
	// returns false. For some message types that do not
	// include data, there is no get function.
	//
	static bool			GetInfoStopPoint (
							std::string*	source,		
							util_ui32*		line, 
							bool*			isGlobal,
							std::string*	cond
						);

	#define	GetInfoInfoBreakPointConditionError \
						GetInfoInvalidBreakPoint
	
	static bool			GetInfoInvalidBreakPoint (
							std::string*	source,
							util_ui32*		line,		// The break point requested.
							util_ui32*		newLine,	
							std::string*	cond		// Condition, if "" no condition.
						);
	enum				InvalidBreakPointReason {
							LineWasInvalidButFailedToReposition			= 0,
							LineWasInvalidButSuccceededToReposition		= 1,
							ConditionWasInvalid							= 2
						};
	static InvalidBreakPointReason						// Use this function to identify the reason.
						GetInvalidBreakPointReason (util_ui32 line, util_ui32 newLine);

	static bool			GetInfoValidBreakPoint (		// Returns the original data supplied when adding.
							std::string*	source,
							util_ui32*		line,
							std::string*	cond
						);

	static bool			IsLineOfLibraryFunction (util_ui32 defLine)
							{ return defLine == DBG_NO_LINE_INFO; }

	static bool			GetInfoCurrFunction (
							std::string*	func,	
							util_ui32*		defLine,
							util_ui32*		callLine,
							util_ui32*		scope,
							std::string*	call
						);

	static bool			GetInfoValue (std::string* content);
	static bool			GetInfoBinaryData (void** data, util_ui32* n);
	static void			ClearBinaryData (void** data, util_ui32* n);
	static bool			GetInfoDynamicCode (std::string* source);
	static const std::string
						GetDynamicCodeVirtualPath (const std::string& vmId);
	static bool			GetInfoValueMany (std::list< std::pair<std::string, bool> >& contents);
	static bool			GetInfoExprTypeData (std::string* content);

	static bool			GetInfoExprTypeDataMany (std::list< std::pair<std::string, bool> >& contents);

	typedef DeltaDebugger::ObjectSlot	ObjectSlot;
	typedef DeltaDebugger::ObjectNode	ObjectNode;
	typedef DeltaDebugger::ObjectGraph	ObjectGraph;

	static bool			GetInfoObjectGraph (ObjectGraph& graph);

	static bool			GetInfoErrorValue (std::string* error);
	static bool			GetInfoError (std::string* error);
	static bool			GetInfoBreakPointError (std::string* error);
	static bool			GetInfoWarning (std::string* error);

	static bool			GetInfoVariables (
							std::list< std::pair<std::string, std::string> >& vars
						);
	static bool			GetInfoAllPossibleCalls (
							std::list< std::pair<std::string, std::string> >& calls
						);
	static bool			GetMostRecentFuncResults (
							std::list< std::pair<std::string, std::string> >& results
						);

	// Mischellaneous.
	static void			ConsumeUntilReceive (DebugServerToClientResponse m);
	static void			WaitAnyMessage (void);
	static bool			IsConnectionBroken (void);
};

#endif	// Do not add stuff beyond this point.
