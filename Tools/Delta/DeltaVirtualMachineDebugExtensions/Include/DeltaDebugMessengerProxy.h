// DeltaDebugMessengerProxy.h
// Proxy to the module posting messages to the debugger User Interface.
// Debug support extensions.
// Script Fighter Project.
// A. Savidis, June 2006 (original).
// RF to separate class (July 2008).
//

#ifndef	DELTADEBUGMESSENGERPROXY_H
#define	DELTADEBUGMESSENGERPROXY_H

#include "utypes.h"
#include "uthreadlib.h"
#include <list>

/////////////////////////////////////////////////////////////////

class DeltaDebugMessengerProxy {

	public:
	typedef	void	(*InvalidBreakPointCallback)(
						const char*	source, 
						util_ui32	line, 
						util_ui32	newLine,
						const char*	cond
					);

	typedef	void	(*ValidBreakPointCallback)(
						const char*	source, 
						util_ui32	line, 
						const char*	cond
					);

	typedef InvalidBreakPointCallback InvalidCondCallback;

	typedef void	(*MostRecentFuncResultsCallback) (
						const std::list< std::pair<std::string, std::string> >& results
					);

	typedef void	(*StopPointCallback) (
						const char*		source, 
						util_ui32		line, 
						bool			isGlobal,
						const char*		cond
					);

	typedef void	(*SendCurrFunctionCallback) (
						const char*		func,	
						util_ui32		defLine,	// If 0, it means it is an extern function.	
						util_ui32		callLine,
						util_ui32		scope,
						const char*		call
					);

	typedef void	(*SendDynamicCodeCallback) (const char* code);

	private:
	mutable umutex					mutex;
	bool							isConnected;
	InvalidBreakPointCallback		onInvalidBreakPoint;
	ValidBreakPointCallback			onValidBreakPoint;
	InvalidCondCallback				onInvalidCond;
	MostRecentFuncResultsCallback	onMostRecentFuncResults;
	StopPointCallback				onStopPoint;
	SendCurrFunctionCallback		onSendCurrFunction;
	SendDynamicCodeCallback			onSendDynamicCode;

	/////////////////////////////////////////////////////////////////

	public:
	void	NotifyInvalidBreakPoint(
				const char*	source, 
				util_ui32	line, 
				util_ui32	newLine,
				const char*	cond
			) const;

	void	NotifyValidBreakPoint(
				const char*	source, 
				util_ui32	line, 
				const char*	cond
			) const;

	void	NotifyInvalidCondition(
				const char*	source, 
				util_ui32	line, 
				util_ui32	newLine,
				const char*	cond
			) const;

	void	NotifyMostRecentFuncResults (
				const std::list< std::pair<std::string, std::string> >& results
			) const;

	void	NotifyStopPoint (
				const char*		source, 
				util_ui32		line, 
				bool			isGlobal,
				const char*		cond
			) const;

	void	NotifySendCurrFunction(
				const char*		func,	
				util_ui32		defLine,
				util_ui32		callLine,
				util_ui32		scope,
				const char*		call
			) const;

	void	NotifyDynamicCode (const char* code) const;

	//******************************

	void	SetOnInvalidBreakPoint (InvalidBreakPointCallback f)
				{ LOCK_BLOCK(mutex); onInvalidBreakPoint = f; }
	void	ResetOnInvalidBreakPoint (void)
				{ LOCK_BLOCK(mutex); unullify(onInvalidBreakPoint); }

	void	SetOnValidBreakPoint (ValidBreakPointCallback f)
				{ LOCK_BLOCK(mutex); onValidBreakPoint = f; }
	void	ResetOnValidBreakPoint (void)
				{ LOCK_BLOCK(mutex); unullify(onValidBreakPoint); }

	void	SetOnInvalidCondition (InvalidCondCallback f)
				{ LOCK_BLOCK(mutex); onInvalidCond = f; }
	void	ResetOnInvalidCondition (void)
				{ LOCK_BLOCK(mutex); unullify(onInvalidCond); }

	void	SetOnMostRecentFuncResults (MostRecentFuncResultsCallback f)
				{ LOCK_BLOCK(mutex); onMostRecentFuncResults = f; }
	void	ResetOnMostRecentFuncResults (void)
				{ LOCK_BLOCK(mutex); unullify(onMostRecentFuncResults); }
	
	void	SetOnStopPoint (StopPointCallback f)
				{ LOCK_BLOCK(mutex); onStopPoint = f; }
	void	ResetOnStopPoint (void)
				{ LOCK_BLOCK(mutex); unullify(onStopPoint); }

	void	SetOnSendCurrFunction (SendCurrFunctionCallback f)
				{ LOCK_BLOCK(mutex); onSendCurrFunction = f; }
	void	ResetOnSendCurrFunction (void)
				{ LOCK_BLOCK(mutex); unullify(onSendCurrFunction); }

	void	SetOnSendDynamicCode (SendDynamicCodeCallback f)
				{ LOCK_BLOCK(mutex); onSendDynamicCode = f; }
	void	ResetOnSendDynamicCode (void)
				{ LOCK_BLOCK(mutex); unullify(onSendDynamicCode); }

	void	SetIsConnected (void) 
				{ LOCK_BLOCK(mutex); isConnected = true; }
	void	ResetIsConnected (void) 
				{ LOCK_BLOCK(mutex); isConnected = false; }
	bool	IsConnected (void) const
				{ LOCK_BLOCK(mutex); return isConnected; }

	//******************************

	DeltaDebugMessengerProxy (void);
	~DeltaDebugMessengerProxy();
};

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
