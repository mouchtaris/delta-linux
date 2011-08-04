// DeltaDebugCallStackInspector.h
// Debugger's call stack.
// Debug support extensions.
// ScriptFighter Project.
// A. Savidis, June 2006 (original).
// RF to a separate class (July 2008).
// Repaired a bug with getting call line, July 2009.
//

#ifndef	DELTADEBUGCALLSTACKINSPECTOR_H
#define	DELTADEBUGCALLSTACKINSPECTOR_H

#include "DeltaDebugExtensionsStdDefs.h"
#include "DeltaValueTypes.h"
#include "utypes.h"
#include "uvector.h"

/////////////////////////////////////////////////////////////////

class DeltaDebugCallStackInspector {
	
	DFRIENDDESTRUCTOR()

	private:
	typedef DeltaDebugCallStackFuncValue	FuncValue;
	typedef DeltaDebugCallStackEntryType	EntryType;

	struct Entry {

		DeltaVirtualMachine*			vm;
		const DeltaClosure*				closure;
		DeltaDebugCallStackEntryType	type;
		DeltaDebugCallStackFuncValue	func;
		DeltaCodeAddress				pc;
		util_ui32						top;
		util_ui32						topsp;
		util_ui32						blocksIndex;
		util_ui16						callLine;

		UOVERLOADED_VOID_ASSIGN_VIA_COPY_CONSTRUCTOR(Entry)

		bool	IsValid (void) const;
		void	Reset (void) { ureset_via_default_ctor(this); }

		Entry (
			DeltaVirtualMachine*			_vm, 
			const DeltaClosure*				_closure,
			DeltaDebugCallStackEntryType	_type,
			DeltaDebugCallStackFuncValue	_func, 
			DeltaCodeAddress				_pc,
			util_ui32						_top, 
			util_ui32						_topsp,
			util_ui32						_blocksIndex,
			util_ui16						_callLine
		) :
		vm				(_vm), 
		closure			(_closure),
		type			(_type),
		func			(_func), 
		pc				(_pc),
		top				(_top), 
		topsp			(_topsp), 
		blocksIndex		(_blocksIndex),
		callLine		(_callLine) 
		{}

		Entry (void) :	
			vm			((DeltaVirtualMachine*) 0), // Default state signifies an illegal entry.
			closure		((const DeltaClosure*) 0), 
			type		(DeltaDebugCallStackEntry_Undef),
			func		(DELTA_CANTBE_A_FUNCTION_ADDRESS),
			pc			(DELTA_CANTBE_A_FUNCTION_ADDRESS),
			top			(0), 
			topsp		(0), 
			blocksIndex	(0),
			callLine	(DELTA_CANTBE_A_SOURCE_LINE)
			{}
	};

	//****************************

	typedef	uvector<Entry>	CallStack;
	CallStack				callStack;
	util_i32				callStackTop;	// Top called function. It is a legal index.
	util_i32				callStackCurr;	// Current debug context (used on break points).

	/////////////////////////////////////////////////////////////////

	public:
	void					AtTopCall (void);
	void					VerifyCalls(void) const;
	void					RetrieveCurrCallValues (util_ui32* a_top, util_ui32* a_topsp) const;
	void					RetrieveCallValues (util_ui16 call, util_ui32* a_top, util_ui32* a_topsp) const;
	DeltaCodeAddress		RetrieveCurrCallPC (void) const;
	DeltaCodeAddress		RetrieveCallPC (util_ui16 call) const;

	bool					IsCurrCallValid (void) const;
	bool					IsAtFunctionCall (void) const; 
	bool					IsAtGlobalCode (void) const;
	bool					IsTopCallAtGlobalCode (void) const;
	bool					IsAtGlobalCodeWithNoPreviousCallOnTheStack (void) const;
	bool					IsBottomLevelReached (void) const;
	bool					IsTopLevelReached (void) const;

	//****************************

	util_ui16				GetTotal (void) const;
	util_ui16				GetCurr (void) const;
	util_ui16				GetCalleeOf (util_ui16 call) const;
	util_ui16				GetCallerOf (util_ui16 call) const;

	//****************************

	DeltaVirtualMachine*	GetCurrCallerVM (void);			// The caller of the current call (one position down).
	DeltaVirtualMachine*	GetCurrCalleeVM (void);			// The callee of the current call (one position up). 
	DeltaVirtualMachine*	GetVM (util_ui16 call);
	util_ui16				GetCallLine (util_ui16 call);
	DeltaVirtualMachine*	GetTopVM (void);

	//****************************

	DeltaVirtualMachine*	GetCurrVM (void);
	const DeltaClosure*		GetCurrClosure (void) const;
	FuncValue				GetCurrFunction (void) const;
	EntryType				GetCurrCallType (void) const;
	util_ui32				GetCurrBlocksIndex (void) const;
	util_ui16				GetCurrCallLine (void) const;
	util_ui32				GetCurrCallIndex (void) const;

	void					Down (void);
	void					Up (void);
	void					Push (
								DeltaVirtualMachine*			vm,
								const DeltaClosure*				closure,
								DeltaDebugCallStackEntryType	type,
								FuncValue						func, 
								DeltaCodeAddress				pc,
								util_ui32						top, 
								util_ui32						topsp,
								util_ui32						blocksIndex,
								util_ui16						line
							);
	void					Pop (void);
	void					ChangeTopGlobalCode (util_ui32 newLine, DeltaCodeAddress pc);
	void					ChangeTopStackPointers (util_ui32 top, util_ui32 topsp);
	void					RemoveAllCalls (DeltaVirtualMachine* vm);

	DeltaDebugCallStackInspector (void);
	~DeltaDebugCallStackInspector();
};

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
