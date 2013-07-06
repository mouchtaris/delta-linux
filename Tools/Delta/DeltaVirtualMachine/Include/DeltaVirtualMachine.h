// DeltaVirtualMachine.h
// The Delta Virtual Machine API.
// Script Fighter Project.
// A. Savidis, October 1999 (original version).
// Latest updates, November 2009.
// Many changes took place, mostly documented in various files 
// and design documents.
//

#ifndef	DELTAVIRTUALMACHINE_H
#define	DELTAVIRTUALMACHINE_H

#include "DeltaValue.h"
#include "DeltaFunc.h"
#include "utypes.h"
#include "ufunctors.h"
#include "ucallbacks.h"
#include "DeltaConstantProgramValues.h"
#include "DeltaDebugExtensionsSuper.h"
#include "BlockIdStacks.h"
#include "GenericReader.h"
#include "uvalidatable.h"
#include "uptr.h"
#include "uvector.h"
#include <list>
#include <map>
#include <stack>
#include <string>

///////////////////////////////////////////////////////

#define	DELTA_DEFAULT_STACK_MINSIZE		4096
#define	DELTA_DEFAULT_VM_ID				"Delta VM"

#define	DELTAVM_VGET_INST(val)			VGET_INST(DeltaVirtualMachine, val, DELTA_VM_TYPE_STR)

///////////////////////////////////////////////////////

#define	DELTA_RETURN_VOID

#define	DELTA_RECORD_VM_SERIALNO(vm)													\
		util_ui32 savedSerialNo_##vm = DPTR(vm)->GetSerialNo()

#define	DELTA_RECORD_VM_ID(vm)															\
		std::string savedVMId_##vm(DPTR(vm)->Id())

#define	DELTA_GET_RECORDED_VM_ID(vm)													\
			(savedVMId_##vm)

#define	DELTA_GET_RECORDED_VM_SERIALNO(vm)												\
			(savedSerialNo_##vm)

#define	DELTA_IF_INVALID_VM_SERIALNO(vm)												\
		if (!ValidatableHandler::Validate(vm, savedSerialNo_##vm))

#define	DELTA_VALIDATE_VM_SERIALNO(vm, ret)												\
		if (!ValidatableHandler::Validate(vm, savedSerialNo_##vm))						\
			return ret; else

#define	DELTA_RECORD_VM_SERIALNO_EX(vm)													\
		util_ui32 savedSerialNo_##vm = DPTR(vm)->GetSerialNo();							\
		DPTR(vm)->PushSerialNoPointer(&savedSerialNo_##vm)

#define	DELTA_VALIDATE_VM_SERIALNO_NOREMOVE_EX(vm, ret)									\
		if (!savedSerialNo_##vm)														\
			return ret;	else
		
#define	DELTA_VALIDATE_VM_SERIALNO_EX(vm, ret)											\
		if (!savedSerialNo_##vm)														\
			return ret;																	\
		else if (true) {																\
			DASSERT(ValidatableHandler::Validate(vm, savedSerialNo_##vm));				\
			DPTR(vm)->PopSerialNoPointer(&savedSerialNo_##vm);							\
		} else

#define DELTA_VALIDATE_VM_SERIALNO_EX_RETURN_VOID(vm)									\
		DELTA_VALIDATE_VM_SERIALNO_EX(vm, DELTA_RETURN_VOID)

#define	DELTA_RETURN_IF_PRODUCEDERROR(vm, ret)											\
		if (DPTR(vm)->HasProducedError())												\
			return ret; else

#define DELTA_RETURN_IF_PRODUCEDERROR_RETURN_VOID(vm)									\
		DELTA_RETURN_IF_PRODUCEDERROR(vm, DELTA_RETURN_VOID)

#define	DELTA_RECORD_EXECUTION_LOOP_PASSPORT(vm)										\
		const util_ui32* execLoop_##vm = DPTR(vm)->GetCurrentExecutionLoopPassport()	

#define	DELTA_GET_RECORDED_EXECUTION_LOOP_PASSPORT(vm)									\
			(execLoop_##vm)

#define	DELTA_VALIDATE_EXECUTION_LOOP_PASSPORT(vm, ret)									\
		if (!DeltaVirtualMachine::IsValidExecutionLoopPassport(*execLoop_##vm))			\
			return ret; else

#define	DELTA_VALIDATE_EXECUTION_LOOP_PASSPORT_RETURN_VOID(vm)							\
		DELTA_VALIDATE_EXECUTION_LOOP_PASSPORT(vm, DELTA_RETURN_VOID)

///////////////////////////////////////////////////////
// This macro is used in Delta embedding calls, normally Run / ExtCall<type> /
// DeltaValue() invocations, in order to make the failing vms (but the primary 
// failing one) workable again.

#define	DELTA_NO_VM_CALLER_EXISTS	(!DeltaVirtualMachine::TotalExecutionLoops())
#define	DELTA_NO_VM_CALLER_FAILED	(!DeltaVirtualMachine::HasAnyExecutionLoopFailed())

#define	DELTA_VM_CALL_AND_RESET_ERRORS(vm, call, cond)									\
	if (true) {																			\
		util_ui32 sn = DPTR(vm)->GetSerialNo();											\
		DPTR(vm)->PushSerialNoPointer(&sn);												\
		call;																			\
		if (sn) {																		\
			DPTR(vm)->PopSerialNoPointer(&sn);											\
			if (DPTR(vm)->HasProducedError() && (cond))	{								\
				DeltaVirtualMachine::ResetRunTimeErrors();								\
			}																			\
			UERROR_CLEAR();																\
		} else unullify(vm); /* vm died, so make the caller aware of it */				\
	} else

///////////////////////////////////////////////////////

class DVM_CLASS DeltaVirtualMachine : public Validatable {

	#include "DeltaVirtualMachineP.h"

	///////////////////////////////////////////////////////////////

	private:
	class ArgumentsPreserver {	// Used to preserve arguments for method invocations.
		private:
		std::list<DeltaValue*>	args;
		util_ui16				n;
		DeltaVirtualMachine*	vm;
		bool					copied;

		public:
		void	Copy (void);
		void	Invalidate (void);
		void	Restore (void);
		ArgumentsPreserver (DeltaVirtualMachine* _vm); 
		~ArgumentsPreserver();
	};
	
	///////////////////////////////////////////////////////////////

	public:
	class DVM_CLASS StdUtilities {	// Reserved. To avoid dependency on the std library.
		public:
		typedef std::list<DeltaValue>	ValList;
		typedef uvector<DeltaValue>		ValVector;

		virtual bool						IsList					(const DeltaValue* value)	const = 0;
		virtual const ValList&				GetList					(const DeltaValue* value)	const = 0;
		virtual ValList&					GetList					(DeltaValue* value)			const = 0;
		virtual CollectableContainer*		GetListContainer		(DeltaValue* value)			const = 0;
		virtual const CollectableContainer*	GetListContainer		(const DeltaValue* value)	const = 0;
		virtual void						MakeList				(DeltaValue* at)			const = 0;
		virtual bool						IsVector				(const DeltaValue* value)	const = 0;
		virtual const ValVector&			GetVector				(const DeltaValue* value)	const = 0;
		virtual ValVector&					GetVector				(DeltaValue* value)			const = 0;
		virtual CollectableContainer*		MakeVector				(DeltaValue* at)			const = 0;
		virtual DeltaExternId*				MakeExternId			(DeltaVirtualMachine* vm)	const = 0;
		virtual void						LinkWithLibraryFunctions(DeltaVirtualMachine* vm)	const = 0;
		virtual ~StdUtilities(){};
	};
	
	class DVM_CLASS TableIteratorHandler {	// Reserved. To avoid dependency on the std table iterator library.
		public:
		virtual void Make (DeltaValue* at) const = 0;
		virtual ~TableIteratorHandler(){};
	};

	struct DVM_CLASS CallInformation {
		util_ui32				tag;
		DeltaValue				func;
		std::string				name;
		DeltaVirtualMachine*	vm;
		util_ui32				defLine;

		UASSIGN_OPERATOR_RETURN_THIS(CallInformation)

		CallInformation (void) : 
			tag		(0),
			vm		((DeltaVirtualMachine*) 0),
			defLine	(DELTA_CANTBE_A_SOURCE_LINE){}

		CallInformation (const CallInformation& info) :
			tag		(info.tag),
			func	(info.func),
			name	(info.name),
			vm		(info.vm),
			defLine	(info.defLine){}
	};

	enum ErrorCause { 
		CallerFailed	= 0, 
		CurrentFailed	= 1, 
		NoneFailed		= 2 
	};

	///////////////////////////////////////////////////////////////

	protected:
	DeltaDebugExtensionsSuper*	
						debugExtensions;			// API debug extensions.
	void				SetDebugger (DeltaDebugExtensionsSuper* dbg);
	static DeltaDebugExtensionsSuper* 
						NoDebugExtensionsConstructor (DeltaVirtualMachine* unused);
	
	///////////////////////////////////////////////////////////////
	// Instructions and debug information.

	std::string			srcFile;					// From which the byte code file it was loaded (can be dynamic).
	DeltaInstruction*	code;
	util_ui16			codeSize;
	bool				hasDebugInfo;
	DeltaDebugProgramInfo	
						debugInfo;

	typedef	void (*OnLoadCallback)(DeltaVirtualMachine*, void*);

	static ucallbackwithclosure<OnLoadCallback> 
						onLoadCallback;				// Instructions and loading callback.

	bool				IsValidTargetLabel (util_ui32 n) const
							{ return n <= GetCodeSize(); }

	///////////////////////////////////////////////////////////////
	// Registers.
	//
	util_ui32			ax;
	util_ui32			bx;
	util_ui32			cx;
	util_ui32			top;
	util_ui32			topsp;
	util_ui32			pc;
	bool				retain_pc;
	bool				isRunning;
	void				SetRetainPC (void)	{ DASSERT(!retain_pc); retain_pc = true; }
	void				ResetRetainPC(void) { DASSERT(retain_pc); retain_pc = false; }

	struct				ExecutionTerminationException {};

	///////////////////////////////////////////////////////////////
	// Tables for function and global variables.

	typedef std::map<std::string, DeltaStdFuncInfo*>		FuncByNameMap;
	typedef std::map<DeltaCodeAddress, DeltaStdFuncInfo*>	FuncByAddrMap;
	typedef std::stack<const DeltaStdFuncInfo*>				FuncInfoStack;
	typedef std::pair<std::string, util_ui16>				StaticVar;
	typedef std::list<StaticVar>							StaticsList;

	DeltaStdFuncInfo*	userDefinedFuncs;
	util_ui16			totalUserDefinedFuncs;
	FuncByNameMap		funcByName;
	FuncByAddrMap		funcByAddr;
	FuncInfoStack		funcStack;					// Stack of called functions for this VM.
	const DeltaStdFuncInfo*	
						currFunc;

	void				PushFunc (const DeltaStdFuncInfo* f = (DeltaStdFuncInfo*) 0);
	void				PopFunc (void);

	DeltaTable*			globalFunctionsTable;		// Functions indexed by their names.
	util_ui32			globalFunctionsTableSerialNo;
	DeltaTable*			staticsTable;				// Statics indexed by their names.
	StaticsList			staticsList;				// As a sequence of name and offset.
	DeltaExternId*		selfAsExternId;				// Unique extern id initially created.

	bool				IsValidFuncInfo (const DeltaStdFuncInfo* info) const {
							return uptrinside(
										info, 
										userDefinedFuncs, 
										userDefinedFuncs + totalUserDefinedFuncs - 1
									); 
						}

	void				ProduceStaticsList (void);
	void				ClearStaticsTable (void);
	void				UnreferenceStaticsTable (void);
	void				UnreferenceFunctionsTable (void);
	void				UndefineAllReferrers (void);
	
	///////////////////////////////////////////////////////////////
	// Call traces to support inspecting the call stack using the VM.

	struct DVM_CLASS CallTrace {
		DeltaVirtualMachine*	vm;
		util_ui32				vmSerialNo;			// To validate the vm (check for destruction).
		const void*				func;
		DeltaCodeAddress		pc;
		util_ui32				top;
		util_ui32				topsp;
		util_ui32				blockIdStackIndex;	// Stack index of the top block of the call.
		util_ui32				callLine;			// Line in which call is made.

		UASSIGN_OPERATOR_RETURN_THIS(CallTrace)

		bool			MatchesRegisters (DeltaCodeAddress _pc, util_ui32 _top, util_ui32 _topsp) const
							{ return pc == _pc && top == _top && topsp == _topsp; }
		void			Set (
							DeltaVirtualMachine*	_vm, 
							const void*				_func, 
							DeltaCodeAddress		_pc,
							util_ui32				_top, 
							util_ui32				_topsp, 
							util_ui32				_blockIdStackIndex,
							util_ui32				_callLine
						){ udestructor_invocation(this); new (this) CallTrace(_vm, _func, _pc, _top, _topsp, _blockIdStackIndex, _callLine); }
		
		void			Reset (void)
							{ ureset_via_default_ctor(this); }

		bool			IsValid (void) const
							{ return vm || vmSerialNo || func || topsp || top; }

		bool			IsCallAtGlobalCode (void) const;
		bool			IsCallAtLibraryFunc (void) const;
		bool			IsCallAtProgramOrMethodFunc (void) const;

		CallTrace (void) : 
			vm					((DeltaVirtualMachine*) 0),
			vmSerialNo			(VALIDATABLE_CANTHAVE_SERIALNO),
			func				((void*) 0),
			pc					(DELTA_CANTBE_A_FUNCTION_ADDRESS),
			top					(DELTA_TOP_TOPSP_OVERFLOW_VALUE),
			topsp				(DELTA_TOP_TOPSP_OVERFLOW_VALUE),
			blockIdStackIndex	(DELTA_MAIN_BLOCK_VALUE),
			callLine			(DELTA_CANTBE_A_SOURCE_LINE){}

		CallTrace (
			DeltaVirtualMachine*	_vm, 
			const void*				_func, 
			DeltaCodeAddress		_pc,
			util_ui32				_top, 
			util_ui32				_topsp, 
			util_ui32				_blockIdStackIndex,
			util_ui32				_callLine
		) :	vm					(_vm),
			vmSerialNo			(_vm->GetSerialNo()),
			func				(_func),
			pc					(_pc),
			top					(_top),
			topsp				(_topsp),
			blockIdStackIndex	(_blockIdStackIndex),
			callLine			(_callLine){}

		CallTrace (const CallTrace& trace) :
			vm			(trace.vm),
			vmSerialNo	(trace.vmSerialNo),
			func		(trace.func),
			topsp		(trace.topsp){}
	};

	//***************************

	typedef uvector<CallTrace>			CallTraceStack;
	typedef std::list<util_ui32>		SnList;		// VM serial no list.

	static CallTraceStack*	
						callTraceStack;				// Global, across all VMs.
	static CallTrace*	topCallTrace;				// Always top call trace.
	static util_ui32	callTraceStackTotal;		// Number of elems in the stack.
	static std::string*	stackTrace;					// In case of error.
	static SnList*		vmNesting;					// Will need one stack per execution thread if threads are supported. 
	
	void				RemoveAllMyCallTracess (void);
	void				PushCallTrace (const void* info, DeltaCodeAddress _pc, util_ui32 _top, util_ui32 _topsp, util_ui32 _blocksIndex);
	static void			PopCallTrace (void);
	bool				IsFoundInAnyCallTrace (void) const;
	void				UpdateCallLineAndPCOfTopCallTraceBeingAtGlobalCode (void);
	void				UpdateTopCallTrace (util_ui32 _top, util_ui32 _topsp);
	static DeltaVirtualMachine*
						GetTopVMFromCallTraceStack (void);
	static util_ui32	GetTotalCallTraces (void);
	static CallTrace&	GetCallTrace (util_ui32 i);
	static CallTrace&	GetTopCallTrace (void);
	static CallTrace&	GetCallerOfTopCallTrace (void);

	static void			MakeCallInformationFromCallTrace (
							CallTrace&			call, 
							CallInformation*	info, 
							util_ui32			tag
						);

	///////////////////////////////////////////////////////////////
	// Handling of execution loops and zombi loops.

	typedef std::list<util_ui32> ExecutionLoopPassports;

	ExecutionLoopPassports	
						executionLoopPassports;
	void				EnterExecutionLoop (void);
	void				ExitExecutionLoop (void);
	const util_ui32*	GetCurrentExecutionLoopPassport (void) const;
	void				ExpireAllExecutionLoopPassports(void);
	static bool			IsValidExecutionLoopPassport (util_ui32 passport);
	bool				AllPreceedingPassportsHaveExpired (const ExecutionLoopPassports::iterator& i) const;

	///////////////////////////////////////////////////////////////
	// Array of constant values. Prebinding uses the
	// arrays of constant values.
	//
	DeltaConstantArraysReader 
						constArrays;

	void				PrebindStringConsts (DeltaInstruction* instr);
	void				PrebindStringConst(DeltaOperand& operand);
	void				PrebindNumericConsts(DeltaInstruction* instr);
	void				PrebindNumericConst(DeltaOperand& operand);
	void				PrebindLibraryFuncs (DeltaInstruction* instr);
	void				PrebindLibraryFunc(DeltaOperand& operand);
	void				PrebindFunctionsOrMethods (DeltaInstruction* instr);
	void				PrebindFunctionOrMethod(DeltaOperand& operand);

	///////////////////////////////////////////////////////////////
	// Specific for calling external library functions.
	//
	util_ui16			callingLibraryFunction;

	void				DoExternFuncEnterCode (DeltaLibraryFunc func);
	void				DoExternFuncReturnCode (DeltaLibraryFunc func);
	void				CallLibraryFunc (const char* name, util_ui16 totalActuals);
	void				CallPreboundLibraryFunc (DeltaLibraryFunc func, util_ui16 totalActuals);
	void				SetCreationInfoIfLibraryFuncCreatedAnExternId (DeltaLibraryFunc func);

	///////////////////////////////////////////////////////////////
	// Runtime stack and access-acceleration variables for globals,
	// locals and actual arguments.
	//
	DeltaValue*			stack;
	util_ui32			stackSize;
	DeltaValue*			globals;					// Where globals start.
	util_ui16			totalActiveGlobals;			// Number of active globals.
	util_ui16			totalGlobals;				// Only those at block 0.
	DeltaBlockIdStacks	blockIdStacks;
	DeltaValue*			actuals;					// Where the actuals of the current function start.
	DeltaValue*			locals;						// Where the locals of the current function start.
	DeltaValue*			retval;						// Return value object, address is always the same.
	
	void				SetActualsAndLocalsPointers (void);
	void				SetGlobalsAndStackPointers (void);
	util_ui32			GetEnvironmentValue (util_i16 offset) const;
	util_ui16			GetTotalActualsForCurrFunction (void) const;
	DeltaTable*			GetSelfForCurrMethod (void);
	DeltaValue*			Locals (void) 
							{ return locals; }
	DeltaBlockIdStacks&	GetBlocks (void) 
							{ return blockIdStacks; }
	util_ui32			TotalLocals (void) const 
							{ return topsp - top; }

	///////////////////////////////////////////////////////////////
	// Operand to Delta object translation.

	DeltaValue*			GetGlobal (util_ui16 offset);
	DeltaValue*			GetLocal (util_ui16 offset);
	DeltaValue*			GetFormal (util_ui16 offset);
	
	typedef DeltaValue* (DeltaVirtualMachine::*StorageConverterFunc)(const DeltaOperand& src);

	DeltaValue*			FromLocalToStorage (const DeltaOperand& src);
	DeltaValue*			FromFormalToStorage (const DeltaOperand& src);
	DeltaValue*			FromGlobalToStorage (const DeltaOperand& src);
	DeltaValue*			FromClosureVarToStorage (const DeltaOperand& src);

	StorageConverterFunc	
						storageConverterFuncs[DELTA_TOTAL_STORAGE_TYPES];

	typedef DeltaValue* (DeltaVirtualMachine::*OperandConverterFunc)(const DeltaOperand& src, DeltaValue* dest);

	DeltaValue*			FromOperandToStorage (const DeltaOperand& src, DeltaValue* dest);
	DeltaValue*			FromOperandToLabel (const DeltaOperand& src, DeltaValue* dest);
	DeltaValue*			FromOperandToProgramFunc (const DeltaOperand& src, DeltaValue* dest);
	DeltaValue*			FromOperandToLibFunc (const DeltaOperand& src, DeltaValue* dest);
	DeltaValue*			FromOperandToMethodFunc (const DeltaOperand& src, DeltaValue* dest);
	DeltaValue*			FromOperandToConstNumber (const DeltaOperand& src, DeltaValue* dest);
	DeltaValue*			FromOperandToConstString (const DeltaOperand& src, DeltaValue* dest);
	DeltaValue*			FromOperandToConstBool (const DeltaOperand& src, DeltaValue* dest);
	DeltaValue*			FromOperandToConstNil (const DeltaOperand& src, DeltaValue* dest);
	DeltaValue*			FromOperandToLambda (const DeltaOperand& src, DeltaValue* dest);
	DeltaValue*			FromOperandToNewSelf (const DeltaOperand& src, DeltaValue* dest);
	DeltaValue*			FromOperandToNotUsed (const DeltaOperand& src, DeltaValue* dest);
	void				InstallOperandConverters (void);

	OperandConverterFunc	
						operandConverterFuncs[DELTA_TOTAL_OPERAND_TYPES];

	///////////////////////////////////////////////////////////////
	// Function call support.
	// This is the list of actual arguments, which is eventually pushed
	// on the runtime stack upon function call.
	//
	typedef std::list<ArgumentsPreserver*> ArgsPreservers;

	std::list<DeltaValue*>	
						actualArguments;
	util_ui16			actualArgsPushed;
	ArgsPreservers		argsPreservers;
	static StdUtilities*	
						stdUtilities;
	static TableIteratorHandler*	
						tableIteratorHandler;

	static StdUtilities*	
						GetStdUtilities (void);

	static TableIteratorHandler*	
						GetTableIteratorHandler (void);

	void				SetArgumentsPreserver (ArgumentsPreserver* p)
							{ argsPreservers.push_front(p); }

	void				ResetArgumentsPreserver (ArgumentsPreserver* p) {
							DASSERT(p == argsPreservers.front());
							argsPreservers.pop_front();
						}

	void				InvokeArgumentsPreserverIfWasSet (void) {
							if (!argsPreservers.empty())
								argsPreservers.front()->Copy(); 
						}

	void				InvalidateArgumentsPreservers (void) {
							std::for_each(
								argsPreservers.begin(), 
								argsPreservers.end(), 
								std::mem_fun(&ArgumentsPreserver::Invalidate)
							);
						}

	void				PushArgumentTable (DeltaTable* table);
	void				PushArgumentList (const std::list<DeltaValue>& args);
	void				PushArgumentVector (const uvector<DeltaValue>& args);

	DeltaValue*			NewActualArgument (void);

	bool				PushUserArgumentsOnly (util_ui32 extraTotalArgs = 0);
	bool				PushUserArgumentsOnly (std::list<DeltaValue*>& userArguments, util_ui32 extraTotalArgs = 0);
	bool				PushUserArgumentsAndArgumentsVector (util_ui32 extraTotalArgs = 0);
	bool				PushUserArgumentsAndArgumentsVector (std::list<DeltaValue*>& userArguments, util_ui32 extraTotalArgs = 0);

	void				PushSelfArgument (DeltaTable* self) 
							{ DASSERT(top); stack[top].FromTable(self); --top; }

	const std::list<DeltaValue*>&
						GetActualArguments (void) const 
							{ return actualArguments; }

	util_ui16			GetTotalActualArgs (void) const	// Returns those explicitly pushed.
							{ return actualArgsPushed; }

	void				RestoreArguments (const std::list<DeltaValue*>& args, util_ui16 n) {
							DASSERT(actualArguments.empty() && !actualArgsPushed); 
							actualArguments = args; 
							actualArgsPushed = n; 
						}

	void				ResetTotalActualArgs (void)
							{ actualArgsPushed = 0; }

	void				ResetArguments (void)
							{ actualArguments.clear(); ResetTotalActualArgs(); }

	void				CallFunction (DeltaValue* functor);

	///////////////////////////////////////////////////////////////
	// Garbage collection.
	//
	void				CollectGarbage (void);
	void				GlobalGarbageCollection (void);

	///////////////////////////////////////////////////////////////
	// Error management.
	
	static void			(*notifyError) (const char*);
	static void			(*notifyWarning) (const char*);
	static bool			somewhereWeHadAnError;			// There is a vm which produced an error.
	static bool			anyErrorInvalidatesAll;			// Do we invalidate all in case one fails?
	static DeltaVirtualMachine*
						primaryFailing;					// The primary failing vm posting an error.
	bool				executionError;					// An error locally in this vm.
	ErrorCause			errorCause;						// Error cause for current vm (if error).
	std::string			errorCode;						// Optional error code set with primary errors.

	//**************************

	void				SetExecutionError (void)
							{ DASSERT(!executionError); executionError = true; }
	void				ResetExecutionError (void)
							{ DASSERT(executionError); executionError = false; }
	static void			RunTimeError (const char* format,...);
	static void			RunTimeWarning (const char* format,...);

	static bool			IsValidErrorCause (ErrorCause cause)
							{ return cause == CallerFailed || cause == CurrentFailed; }
	void				ResetErrorCause (void) 
							{ errorCause = NoneFailed; }
	void				SetErrorCause (ErrorCause cause) 
							{ DASSERT(IsValidErrorCause(cause));  errorCause = cause;  }
	bool				IsErrorCauseReset (void) const
							{ return errorCause == NoneFailed; }

	//**************************

	static DeltaVirtualMachine*	
						GetPrimaryFailing (void);			// The vm causing the initial error.
	void				SetAsPrimaryFailing (void)
							{ DASSERT(!primaryFailing); primaryFailing = this; }
	static void			ResetPrimaryFailing (void)
							{ unullify(primaryFailing); }
	bool				IsPrimaryFailing (void) const
							{ return this == GetPrimaryFailing(); }

	void				ForceCompleteExecutionByError (void);
	bool				AreRegistersResetDueToError (void) const;

	void				InvalidateExecution (ErrorCause cause = CurrentFailed);
	void				InvalidateEverySecondaryFailingAndPerformClearActions (void);
	bool				IsInvalidationCommitted (void) const;
	void				CommitInvalidation (void);
	
	///////////////////////////////////////////////////////////////
	// Execution loop management.

	static DeltaVirtualMachine* 
						callingVM;						// The VM that executed the most recent VM instruction.
	std::string			id;
	util_ui32			currLine;						// Source line for error reporting.
	enum				ExecutorType { WithDebugger = 0, WithoutDebugger = 1};
	ExecutorType		currExecutorType;
	void				(DeltaVirtualMachine::*currExecutor)(util_ui32, const util_ui32*);
	void				(DeltaVirtualMachine::*currUnwindingChecker)(void);

	static void			SetCallingVM (DeltaVirtualMachine* vm)
							{ callingVM = vm; }

	void				PrepareForExecution (void);		// Initialise execution parameters. 
	void				StartUp (void);					// Start up instructions.
	void				Continue (void);				// Continue execution from current instruction.
	void				ExecuteCurrInstruction (util_ui32, const util_ui32*);	// Execute current instruction when debugger is on.
	void				ExecuteCurrInstructionWithoutDebugger (util_ui32, const util_ui32*);	// Execute current instruction when debugger is not on.

	void				UnwindingCheckerEmpty (void)
							{}
	void				UnwindingChecker (void);
	void				SetUnwindingChecker (void)
							{ currUnwindingChecker = &DeltaVirtualMachine::UnwindingChecker; }
	void				ResetUnwindingChecker (void)
							{ currUnwindingChecker = &DeltaVirtualMachine::UnwindingCheckerEmpty; }
	bool				IsUnwindingCheckerReset (void) const
							{ return currUnwindingChecker == &DeltaVirtualMachine::UnwindingCheckerEmpty; }

	bool				PCAtLastInstruction (void) const;	
	bool				PCIsInvalid (void) const;
	bool				PCAtNormalProgramEnd (void) const;

	///////////////////////////////////////////////////////////////
	// Handling of current object construction stack. This is used
	// to supply owner object for methods.

	std::list<DeltaObject*>	
						objectConstructionStack;
	DeltaObject*		currConstructedObject;

	void				PushConstructedObject (DeltaObject* obj)
							{ objectConstructionStack.push_back(currConstructedObject = obj); }

	void				PopConstructedObject (void) { 
							DASSERT(!objectConstructionStack.empty());
							objectConstructionStack.pop_back();
							if (objectConstructionStack.size())
								currConstructedObject = *(objectConstructionStack.rbegin());
							else
								unullify(currConstructedObject);
						}

	///////////////////////////////////////////////////////////////
	// User defined function enter / return sequences. Those functions are
	// used ONLY for runtime execution of CALLFUNC instructions. The CallFuncPrepare()
	// is the function that prepares a call either of an extern function, or of a
	// user defined function.
	//
	bool				currFuncReturnsValue;
	DeltaClosure*		userFuncClosure;		// Closure of user-defined function (null for lib funcs).

	bool				SetUserFuncClosure (DeltaClosure* closure, const DeltaStdFuncInfo* func);
	void				ResetUserFuncClosure (void)
							{ unullify(userFuncClosure); }
	DeltaClosure*		GetUserFuncClosure (void) const
							{ return userFuncClosure; }

	void				SetCurrFuncReturnsValue (void)
							{ currFuncReturnsValue = true; }
	void				ResetCurrFuncReturnsValue (void)
							{ currFuncReturnsValue = false; }
	bool				IsCurrFuncReturningValue (void) const
							{ return currFuncReturnsValue; }

	bool				ExtCallGlobalFuncCommit (
							DeltaCodeAddress	funcAddr, 
							util_ui16			stdCallArguments
						);
	bool				ExtCallFunctionCommit (DeltaValue* functor);
	void				ExecuteCallProgramFunc (
							DeltaVirtualMachine*	caller,
							DeltaCodeAddress		addr, 
							util_ui16				totalActuals, 
							util_ui16				stdCallArguments
						);

	void				CallFuncPrepare (
							DeltaVirtualMachine*	caller,
							util_ui32				totalActuals, 
							util_ui32				stdCallArguments
						);
	void				DoLocalFuncReturnCode (void);
	void				DoLocalFuncEnterCode (const DeltaStdFuncInfo* funcInfo);

	void				BlockEnterImpl (util_ui16 blockId, util_ui16 totalLocals, bool isMainProgramBlock);
	void				BlockEnterImplWithoutDebugger (util_ui16 blockId, util_ui16 totalLocals, bool isMainProgramBlock);
	void				BlockExitImpl (util_ui16 blockId, util_ui16 totalLocals, bool isMainProgramBlock);
	void				BlockExitImplWithoutDebugger (util_ui16 blockId, util_ui16 totalLocals, bool isMainProgramBlock);

	bool				DoEnterBlock (util_ui16 totalLocals);
	void				DoExitBlock (util_ui16 totalLocals);
	void				DoExitBlocksUntil (util_ui16 blockId);
	void				DoExitMultipleBlocks (util_ui16 blockId);
	void				DoExitAllBlocksOfCurrentContext (void);
	bool				AreNewBlockLocalsUndefined (util_ui16 totalLocals);

	//--------------------------------------------------------------
	// PUBLIC API STARTS HERE.

	public:
	DeltaDebugExtensionsSuper*	
						GetDebugger (void)		
							{ return debugExtensions; }
	DeltaDebugExtensionsSuper&	
						GetDebuggerRef (void)	
							{ DASSERT(debugExtensions); return *DPTR(debugExtensions); }

	// Set this if you wish to be notified when a virtual
	// machine gets loaded (after loading completes). Single
	// registration scheme.

	static void			SetOnLoadCallback (OnLoadCallback f, void* c = (void*) 0)
							{ onLoadCallback.set(f, c); }

	// Those registration functions are reserved by the std library. Calling those
	// explicitly will result into an assertion.

	static void			SetStdUtilities (StdUtilities* handler);
	static void			ResetStdUtilities (void);

	static void			SetTableIteratorHandler (TableIteratorHandler* handler);
	static void			ResetTableIteratorHandler (void);

	//--------------------------------------------------------------
	// Embedding API. Should be used only by VM clients for
	// implementing library functions or for invoking Delta functions
	// inside native code.
	//
	void				PushActualArg (const DeltaValue&);
	void				PushActualArg (const DeltaValue*);
	util_ui16			TotalActualArgs (void);
	DeltaValue*			GetActualArg (util_ui16 argNo);

	void				SetReturnValue (const DeltaValue& val) 
							{ retval->Undefine(); retval->Assign(val); }
	void				SetReturnValue (const DeltaValue* val) 
							{ retval->Undefine(); retval->Assign(val); }
	void				GetReturnValue (DeltaValue* val) const
							{ val->Assign(retval); }
	DeltaValue&			GetReturnValue (void) 
							{ return *retval; }
	const DeltaValue&	GetReturnValue (void) const 
							{ return *retval; }

	void				ExtCallGlobalFunc (const char* name);
	void				ExtCallGlobalFunc (DeltaCodeAddress funcAddr);
	void				ExtCallMethodFunc (DeltaCodeAddress funcAddr, DeltaTable* table);
	void				ExtCallFunction (DeltaValue* functor);

	bool				GlobalFuncExists (const char* name);
	bool				GlobalFuncExists (DeltaCodeAddress funcAddr);
	DeltaCodeAddress	GlobalFuncAddress (const char* name);
	const char*			GetFuncName (DeltaCodeAddress addr) const;

	DeltaCodeAddress	ValidateFuncAddress (DeltaCodeAddress funcAddr, bool isMethod = false);
	DeltaValue*			ValidateStackValuePtr (DeltaValue* val);
	DeltaValue*			ValidateClosureValuePtr (DeltaValue* val);

	///////////////////////////////////////////////////////////
	// These are normally used by the debugger backend and you 
	// will not bother unless your are building a new backend.

	DeltaObject*		GetCurrConstructedObject (void) const
							{ return currConstructedObject; }
	const DeltaStdFuncInfo*	
						GetFuncInfo (DeltaCodeAddress funcAddr) const;
	const DeltaStdFuncInfo*	
						GetCurrFuncInfo (void) const					// For currently called function.			
							{ return currFunc; }

	typedef std::list<std::pair<std::string, DeltaValue> > FuncList;

	void				GetGlobalFunctions (FuncList* funcs);
	void				GetGlobalFunctionsTable (DeltaValue* funcs);	// Set as an object value.
	DeltaTable*			GetStaticsTable (void);			

	DeltaExternId*		GetExternId (void)
							{ return selfAsExternId; }
	const DeltaExternId*
						GetExternId (void) const
							{ return selfAsExternId; }

	///////////////////////////////////////////////////////////
	// Stack trace creation functions.
	// You should invoke the methods for extracting call information
	// only when HasAnyCalls() returns true. Nomrally, you will not
	// use these functions for embedded execution.

	static bool			HasAnyCalls (CallInformation* info)
							{ return callTraceStackTotal != 0; }
	static util_ui32	GetTotalCalls (void)
							{ return callTraceStackTotal; }
	static void			GetCall (util_ui32 i, CallInformation* info);
	static void			GetCurrentCall (CallInformation* info);
	static bool			IsValidCallTag (util_ui32 tag);		// Allows checking call tags.
	static bool			GetNextCall (util_ui32 tag, CallInformation* info);	// Must have a valid tag.
	static void			CreateStackTrace (util_ui32 maxTraces = 0);
	static const std::string	
						GetStackTrace (void) 					
							{ return *DPTR(stackTrace); }

	///////////////////////////////////////////////////////////
	// Error reporting and management. Callbacks have a single registration scheme,
	// so when setting a callback you should check if there is another, that you should
	// also manually invoke.
	DeltaVirtualMachine*
						SetErrorCode (const std::string& _errorCode)
							{ errorCode = _errorCode; return this; }
	void				PrimaryError (const char* format,...);			// It must be called for the first reporting of an error.
	void				PossiblePrimaryError (const char* format,...);	// Posts primary if no primary was set before.
	void				Error (const char* format,...);					// It is called when the error is caused by an already existing error.
	void				Warning (const char* format,...);
	
	typedef void (*ErrorWarningCallback) (const char*);
	static void			SetErrorCallback (ErrorWarningCallback f);
	static void			SetWarningCallback (ErrorWarningCallback f);
	static ErrorWarningCallback
						GetErrorCallback (void);
	static ErrorWarningCallback
						GetWarningCallback (void);

	void				ResetError (void);						// Resets error only on the current vm.
	static void			ResetRunTimeErrors (void);				// Resets erros in all vms.
	bool				HasProducedError (void) const;			// If caller vm is in error state.
	bool				HasExecutionCompleted (void) const;		// Gracefully or not.
	static bool			RunTimeErrorsExist (void);				// If there is a vm in error state.

	static void			SetAnyErrorInvalidatesAll (bool val);	// If a vm raises an error it propages to its caller.
	static bool			GetAnyErrorInvalidatesAll (void) ;

	//**************************

	const char*			Id (void) const							
							{ return id.c_str(); }	
	const char*			Source (void) const						
							{ return srcFile.c_str(); } 
	util_ui32			Line (void) const 						
							{ return currLine; }
	util_ui32			GetInstructionLine (void) const			
							{ DASSERT(!PCIsInvalid()); return code[pc].line; }
	util_ui32			GetInstructionLine (util_ui32 i) const	
							{ DASSERT(i < codeSize); return code[i].line; }
	const DeltaInstruction&
						GetInstruction (void) const				
							{ DASSERT(!PCIsInvalid()); return code[pc]; }
	const DeltaInstruction&
						GetInstruction (util_ui32 i) const		
							{ DASSERT(i < codeSize); return code[i]; }
	DeltaValue*			GetStorageObject (const DeltaOperand& src);
	DeltaValue*			TranslateOperand (const DeltaOperand& src, DeltaValue* dest);

	DeltaCodeAddress	PC (void) const 						
							{ return pc; }
	util_ui32			GetCodeSize (void) const				
							{ return codeSize; }
	util_ui32			GetStackSize (void) const				
							{ return stackSize; }

	//**************************

	static void			Initialise (void);
	static void			CleanUp (void);

	bool				Load (const char* file);
	bool				Load (GenericReader& reader);
	void				Clear (void);						// Clears everything, as if not loaded. Can load again.
	bool				HasCode (void) const
							{ return !!code; }
	bool				HasDebugInfo (void) const			
							{ return hasDebugInfo; }
	const DeltaDebugProgramInfo&
						GetDebugInfo (void) const
							{ return debugInfo; }
	bool				HasRun (void) const
							{ return isRunning; }
	bool				HasNotRunYet (void) const
							{ return !isRunning; }
	void				Run (void);							// Execution loop.
	static util_ui32	TotalExecutionLoops (void);			// How many outer (caller) execution loops.
	static bool			HasAnyExecutionLoopFailed (void);	// If a VM of an outer execution loop has produced an error.

	static DeltaVirtualMachine* 
						GetCallingVM (void)					// In holds the VM of the most recently executed instruction.
							{ return callingVM; }
	bool				IsCallingLibraryFunction (void) const
							{ return callingLibraryFunction; }

	///////////////////////////////////////////////////////////

	const char*			GetExtClassString (void) const		
								{ return DELTA_VM_TYPE_STR; }
	DYNAMIC_CAST_NOBASE_INLINE_IMPL(DELTA_VM_TYPE_STR)

	DeltaVirtualMachine (
		const char* _id			= DELTA_DEFAULT_VM_ID, 
		util_ui32 _stackSize	= DELTA_DEFAULT_STACK_MINSIZE
	);
	virtual ~DeltaVirtualMachine();
};

///////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.