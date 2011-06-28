// DeltaDebugTraceCommandHandler.h
// Trace command management for the debugger.
// Debug support extensions.
// Script Fighter Project.
// A. Savidis, June 2006 (original).
// RF to a separate class (July 2008).
// Added selective step-in and finnaly made the dispatcher (September 2008).
//

#ifndef	DELTADEBUGTRACECOMMANDHANDLER_H
#define	DELTADEBUGTRACECOMMANDHANDLER_H

#include "DeltaVirtualMachine.h"
#include "DeltaDebugLinesWithCodeHolder.h"
#include "utypes.h"

/////////////////////////////////////////////////////////////////

#define	TOTAL_TRACE_COMMANDS	None

class DeltaDebugTraceCommandHandler {

	private:
	enum CommandType {
		Step			= 0,
		StepIn			= 1,
		SelectiveStepIn	= 2,
		StepOut			= 3,
		RunTo			= 4,
		Continue		= 5,
		None			= 6
	};

	typedef DeltaDebugLinesWithCodeHolder LinesHolder;
	typedef void (DeltaDebugTraceCommandHandler::*TraceFunc)(DeltaVirtualMachine*, const LinesHolder&);
	static TraceFunc traceFuncs[TOTAL_TRACE_COMMANDS];

	///////////////////////////////////////////////////////

	class CommandData {

		private:
		CommandType				cmmd;
		util_i16				callNesting;
		bool					done;
		DeltaVirtualMachine*	postedBy;

		union {
			struct Step					{}							step;
			struct StepIn				{}							stepIn;
			struct SelectiveStepInIn	{ DeltaCodeAddress pc; }	selectiveStepIn;
			struct StepOut				{}							stepOut;
			struct RunTo				{ util_ui16 line; }			runTo;
		} data;

		//*******************************

		void	Set (DeltaVirtualMachine* vm,  CommandType _cmmd) {
					cmmd		= _cmmd; 
					callNesting	= 0;
					postedBy	= vm;
					done		= false;
				}

		///////////////////////////////////////////////////////

		public:
		void					SetStep (DeltaVirtualMachine* vm) 
									{ Set(vm, Step); }

		void					SetStepIn (DeltaVirtualMachine* vm) 
									{ Set(vm, StepIn); }

		void					SetSelectiveStepIn (DeltaVirtualMachine* vm, DeltaCodeAddress pc) 
									{ Set(vm, SelectiveStepIn); data.selectiveStepIn.pc = pc; }

		void					SetStepOut (DeltaVirtualMachine* vm) 
									{ Set(vm, StepOut); }

		void					SetRunTo (DeltaVirtualMachine* vm, util_ui16 line) { 
									Set(vm, RunTo);
									data.runTo.line	= line;
								}
		
		void					SetContinue (DeltaVirtualMachine* vm)
									{ Set(vm, Continue); }

		//************************

		bool					IsCompleted (void) const
									{ return done && HasCommand(); }
		
		void					SetCompleted (void)
									{ done = true; }
		
		void					Reset (void) 
									{ cmmd = None; callNesting = 0; unullify(postedBy); done = false; }

		bool					HasCommand (void) const
									{ return cmmd != None; }

		CommandType				GetCommand (void) const 
									{ return cmmd; }

		util_ui16				GetRunToLine (void) const
									{ DASSERT(cmmd == RunTo); return data.runTo.line; }

		DeltaCodeAddress		GetSelectiveStepInTarget (void) const
									{ DASSERT(cmmd == SelectiveStepIn); return data.selectiveStepIn.pc; }

		//************************

		void					IncCallNesting (void) 
									{ ++callNesting; }

		void					DecCallNesting (void) 
									{ --callNesting; }

		util_i16				GetCallNesting (void) const 
									{ return callNesting; }

		DeltaVirtualMachine*	GetPostedBy (void) 
									{ return postedBy; }

		CommandData (void) : cmmd(None), callNesting(0), done(false){}
		~CommandData(){}
	};

	///////////////////////////////////////////////////////

	void		HandleStep				(DeltaVirtualMachine* vm, const LinesHolder& lines);
	void		HandleStepIn			(DeltaVirtualMachine* vm, const LinesHolder& lines);
	void		HandleSelectiveStepIn	(DeltaVirtualMachine* vm, const LinesHolder& lines);
	void		HandleStepOut			(DeltaVirtualMachine* vm, const LinesHolder& lines);
	void		HandleRunTo				(DeltaVirtualMachine* vm, const LinesHolder& lines);
	void		HandleContinue			(DeltaVirtualMachine* vm, const LinesHolder& lines);

	CommandData	debugCmmd;	
	
	///////////////////////////////////////////////////////

	public:

	void		OnFunctionEnter (void);
	void		OnFunctionExit (void);
	void		OnStopPoint (void);

	void		StartStep (DeltaVirtualMachine* vm);
	void		StartStepIn (DeltaVirtualMachine* vm);
	void		StartSelectiveStepIn (DeltaVirtualMachine* vm, DeltaCodeAddress pc);
	void		StartStepOut (DeltaVirtualMachine* vm);
	void		StartRunTo (DeltaVirtualMachine* vm, util_ui16 line);
	void		StartContinue (DeltaVirtualMachine* vm);
	void		Reset (void);

	void		HandleAnyTraceCommand (
					DeltaVirtualMachine*					vm,
					const DeltaDebugLinesWithCodeHolder&	lines
				);
	bool		IsTraceCommandCompleted (void);
	void		BusyWaitForTraceCommand (util_ui32 sleepTime);

	DeltaDebugTraceCommandHandler (void);
	~DeltaDebugTraceCommandHandler();
};

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
