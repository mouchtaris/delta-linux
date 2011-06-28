// DebugServer.h
// The debug server is an independent thread initiated together with the
// running Delta main program, serving all network requests.
// ScriptFighter Project.
// A. Savidis, February 2002.
// Updated (re-factored) February 2005.
//

#ifndef	DELTADEBUGSERVER_H
#define	DELTADEBUGSERVER_H

#include "DeltaDebugCommands.h"
#include "utypes.h"
#include "ufunctors.h"
#include <map>
#include <list>
#include <functional>
#include <string>

#define	DEBUG_SERVER_BREAKPOINTS		"Breakpoints.txt"
#define	DEBUG_SERVER_PORT				0	// Auto assign.
#define	DEBUG_SERVER_PORT_INFO_FILE		"DebugServerPort.txt"

#define	DC2SR \
	DebugClientToServerRequest

/////////////////////////////////////////////////////////////

class DeltaVirtualMachine;

class DeltaDebugServer {

	/////////////////////////////////////////////////////////////

	private:

	// This flag indicates if we have stopped inside a break point.
	// It is used to judge if we can serve execution control commands
	// and display requests.
	//
	static bool	insideBreakPoint;

	//////////////////////////////////////////////////////////////////
	// The go function allows the debug server to give external control
	// to a debug loader to run excplicitly the VM at start-up.
	//
	static bool startRequestReceived;
	static void DefaultOnStart (void);
	static void	(*startFunc)(void);

	//************************
	// Content extraction functions.
	//
	static bool GetUnsignedInt (DebugClientToServerRequest cmmd, util_ui32* val);
	static bool GetStringUnsignedInt (DebugClientToServerRequest cmmd, char** str, util_ui32* val);
	static bool GetStringString (DebugClientToServerRequest cmmd, char** str1, char** str2);
	static bool GetStringUnsignedIntString (DebugClientToServerRequest cmmd, char** str1, util_ui32* val, char** str2);
	static bool GetString (DebugClientToServerRequest cmmd, char** str);

	//************************
	// Dispatch (handler) functions.
	//
	typedef void (*RequestHandler) (void);
	typedef std::map<DebugClientToServerRequest, RequestHandler> HandlerMap;

	static HandlerMap* handlers;
	static void InstallRequestHandlers (void);

	static void HandleGo (void);
	static void HandleStepOver (void);	
	static void HandleStepIn (void);	
	static void HandleSelectiveStepIn (void);	
	static void HandleRunTo (void);		
	static void HandleStepOut (void);	
	static void HandleStackUp (void);		
	static void HandleStackDown (void);		
	static void HandleMultipleStackUp (void);		
	static void HandleMultipleStackDown (void);		
	static void HandleGetLocal (void);		
	static void HandleGetGlobal (void);		
	static void HandleGetActual (void);	
	static void HandleGetExpr (void);		
	static void HandleGetExprMany (void);		
	static void HandleGetExprTypeData (void);		
	static void HandleGetExprTypeDataMany (void);		
	static void HandleGetBinaryData (void);		
	static void HandleGetObjectGraph (void);		
	static void HandleSetObjectGraphConfiguration (void);		
	static void HandleGetVariables (void);		
	static void HandleGetDynamicCode (void);		
	static void HandleGetAllPossibleCalls (void);
	static void HandleAddBreakPoint (void);			
	static void HandleRemoveBreakPoint (void);		
	static void HandleChangeBreakPointCondition (void);		
	static void HandleEnableBreakPoint (void);		
	static void HandleDisableBreakPoint (void);		
	static void HandleEnableAllBreakPoints (void);	
	static void HandleDisableAllBreakPoints (void);	
	static void HandleRemoveAllBreakPoints (void);	
	static void HandleStart (void);
	static void HandleStop (void);	
	static void HandleBreakExecution (void);	
	static void HandleQueryBreakPoint (void);
	static void HandleSetToStringMaxLength (void);
	static void HandleSetBreakOnThrownException (void);

	static void	DispatchRequest (DebugClientToServerRequest request);
	static bool	IsClientDisconnectedAndStartedRetryingReconnection (void);

	static DeltaVirtualMachine*
				GetFirstMatchingVM (const char* source);

	static void	DoResponse (
					DebugServerToClientResponse cmmd,
					const char*					content
				);

	static void	DoResponse (
					DebugServerToClientResponse								cmmd,
					const std::list< std::pair<std::string, std::string> >& contents
				);

	static void	DoResponse (
					DebugServerToClientResponse							cmmd,
					const std::list< std::pair<std::string, bool> >&	contents
				);

	//************************
	// Callabcks to be set on debug extensions.

	static void		DoInvalidBreakPointAdapter(
						const char*	source, 
						util_ui32	line, 
						util_ui32	newLine,
						const char*	cond
					); 

	static void		DoInvalidConditionAdapter(
						const char*	source, 
						util_ui32	line, 
						util_ui32	newLine,
						const char*	cond
					); 

	static void		DoInfoStopPointAdapter (
						const char*		source, 
						util_ui32		line, 
						bool			isGlobal,
						const char*		cond
					);

	//************************
	// Request processing functions.
	//
	static bool		IsRequestPending (void);	// Check this first.
	static bool		IsConnectionBroken (void);	// Socket no more opened!
	static bool		ReceiveRequest (void);		// Then receive request locally.
	static DC2SR	GetRequestType (void);		// Query request always after receiving.
	static void		RequestProcessed (void);	// Call this at the end.

	static bool		GetSelectiveStepIn (util_ui32* callOrder);
	static bool		GetRunTo (util_ui32* line);
	static bool		GetSetToStringMaxLength (util_ui32* maxLen);
	static bool		GetSetBreakOnThrownException (bool* val);
	static bool		GetAddBreakPoint (char** source, util_ui32* line, char** cond);
	static bool		GetChangeBreakPointCondition (char** source, util_ui32* line, char** cond);
	static bool		GetRemoveBreakPoint (char** source, util_ui32* line);		
	static bool		GetEnableBreakPoint (char** source, util_ui32* line);		
	static bool		GetDisableBreakPoint (char** source, util_ui32* line);

	static bool		GetGetGlobal (char** name);
	static bool		GetGetLocal (char** name);
	static bool		GetGetActual (char** name);
	static bool		GetGetExpr (char** expr);
	static bool		GetGetExprMany (std::list<std::string>& exprs);
	static bool		GetGetExprTypeData (char** formatId, char** expr);
	static bool		GetGetExprTypeDataMany (char** formatId, std::list<std::string>& exprs);
	static bool		GetGetBinaryData (char** expr);

	static bool		GetSetObjectGraphConfiguration (
						bool*	expandLibraryFuncs,
						bool*	expandMethodFuncs,
						bool*	expandProgramFuncs,
						bool*	expandFuncVMs
					);
	static bool		GetGetObjectGraph (char** expr, util_ui32* depth, bool* directed);

	static bool		GetRemoveAllBreakPoints (char** source);		
	static bool		GetEnableAllBreakPoints (char** source);		
	static bool		GetDisableAllBreakPoints (char** source);

	static bool		InsideBreakPoint (void);
	static void		ResetInsideBreakPoint (void);
	static bool		ReceiveAllBreakPoints (void);

	//************************
	// Mischellaneous functions.
	//
	static void		NotifyExit (void);

	static void		DebugServiceLoopThread (void* unused);
	static void		TryEstablishClientConnection (void* unused = (void*) 0);

	static bool		IsNegotiationWithClientSuccessful (int argc, char** argv);
	static void		UserMessageAboutServerPort (void);
	static bool		TryAcceptClient (void);
	static bool		InitialiseAfterClientConnection (void);
	static void		CleanUpForClientConnection (void);


	/////////////////////////////////////////////////////////////

	public:
	static bool			Initialise (util_ui32 port, int argc, char** argv);
	static void			CleanUp (void);
	static bool			IsClientConnected (void);	// If a client was connected.
	static bool			ClientWasAttached (void);
	static util_ui32	GetPort (void);
	static void			CloseClientConnection (void);
	static bool			IsDebugStopped (void);	// If it was running and was stopped explicitly (just before exit).

	//************************
	// Response functions.
	//
	static void		DoInfoStopPoint (
						const char*		source, 
						util_ui32		line, 
						bool			isGlobal,
						const char*		cond
					);

	static void		DoInfoGlobalContext (void);		
	static void		DoInfoTopContext (void);	

	static void		DoInfoCurrFunction (
						const char*		func,	
						util_ui32		defLine,	// If 0, it means it is an extern function.	
						util_ui32		callLine,
						util_ui32		scope,
						const char*		call
					);

	static void		DoInfoValue (const char* content);
	static void		DoInfoValueMany (
						const std::list< std::pair<std::string, bool> >& contents
					);
	static void		DoInfoExprTypeData (const char* content);
	static void		DoInfoExprTypeDataMany(
						const std::list< std::pair<std::string, bool> >& contents
					);
	static void		DoInfoBinaryData (void* data, util_ui32 n);

	typedef DeltaDebugger::ObjectSlot	ObjectSlot;
	typedef DeltaDebugger::ObjectNode	ObjectNode;
	typedef DeltaDebugger::ObjectGraph	ObjectGraph;

	static void		DoInfoObjectGraph (const ObjectGraph& graph);

	static void		DoInfoErrorValue (const char* error);
	static void		DoInfoStop (void);

	static void		DoInfoMostRecentFuncResults (
						const std::list< std::pair<std::string, std::string> >& results
					);

	static void		DoInfoVariables (
						const std::list< std::pair<std::string, std::string> >& vars
					);

	static void		DoInfoDynamicCode (const char* src);

	static void		DoInfoAllPossibleCalls (
						const std::list< std::pair<std::string, std::string> >& calls
					);

	static void		DoInfoError (const char* error);
	static void		DoInfoBreakPointError (const char* error);
	static void		DoInfoWarning (const char* error);

	static void		DoInfoInvalidBreakPoint (
						DebugServerToClientResponse cmmd,
						const char*					source, 
						util_ui32					line, 
						util_ui32					newLine,
						const char*					cond
					);

	static void		DoInfoValidBreakPoint (
						const char*					source, 
						util_ui32					line, 
						const char*					cond
					);

	static void		SetExitServeFunction (void (*exitFunc) (void));
	static void		SetStartServeFunction (void (*startFunc) (void));
};

#endif	// Do not add stuff beyond this point.
