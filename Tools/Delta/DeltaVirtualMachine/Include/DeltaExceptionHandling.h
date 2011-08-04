// DeltaExceptionHandling.h
// Script Fighter Project.
// A. Savidis, June 2006.
// Extended May 2009 to support native code trap and throw
// Delta exceptions.
//

#ifndef	DELTAEXCEPTIONHANDLING_H
#define	DELTAEXCEPTIONHANDLING_H

#include "DDebug.h"
#include "utypes.h"
#include "ustlhelpers.h"
#include "DeltaStdDefs.h"
#include "DeltaValue.h"
#include "DeltaVirtualMachine.h"
#include "ufunctors.h"
#include "ustrings.h"
#include "uerrorclass.h"
#include "usingleton.h"

#include <string>
#include <list>
#include <algorithm>
#include <functional>

#define	DELTA_EXCEPTIONS_NATIVECODE_ADDRESS	0
#define	DELTA_EXCEPTIONS_NATIVECODE_VM		((DeltaVirtualMachine*) 0)

//-----------------------------------------------------------------
// It has to be a singleton. Should be turned to normal class
// when having multiple processes.
//
class DVM_CLASS DeltaExceptionHandling {

	public:
	enum Context {	
		InLibraryFunction	=	0,
		InDeltaFunction		=	1,
		InDeltaGlobalCode	=	2,
		InNativeCode		=	3
	};

	/////////////////////////////////////////////////////////////////
	private:
	static DeltaExceptionHandling* singletonPtr;

	// We keep two stacks, in accordance to the global call-stack.
	// The trap-blocks of each function call and the respective VMs,
	// with a value indicating if the context is a library function, a program 
	// function or global porgram context.
	//
	union ContextInfo {
		
		util_ui32			pc;
		DeltaLibraryFunc	func;
		bool				IsEmpty (void) const				{ return !pc && !func; }
		operator			bool()								{ return pc || func; }

		UASSIGN_OPERATOR_RETURN_THIS(ContextInfo)

		ContextInfo (void)										{ memset(this, 0, sizeof(ContextInfo)); }
		ContextInfo (const ContextInfo& info)					{ memcpy(this, &info, sizeof(ContextInfo)); }
		ContextInfo (util_ui32 _pc) : pc(_pc)					{}
		ContextInfo (DeltaLibraryFunc _func) : func(_func)		{}
	};

	typedef std::list<DeltaCodeAddress>							TrapBlockList;
	typedef utriple<DeltaVirtualMachine*, Context, ContextInfo>	CallContext;
	typedef std::list<CallContext>								CallStack;
	typedef std::list<TrapBlockList*>							TrapBlockStack;

	TrapBlockStack					trapBlockStack;			// Trap block lists aligned to the call stack.
	CallStack						callStack;				// The call stack of respective VM's.
	TrapBlockList*					currList;				// The trap blocks of the function at the top of the stack.
	DeltaVirtualMachine*			currVM;
	Context							currContext;
	DeltaLibraryFunc				currFunc;
	DeltaValue						exceptionValue;
	util_ui32						expectedNativeTrapDisableCalls;

	bool							isUnwinding;
	bool							shouldUnwind;
	bool							isUnwindingFromNativeCode;

						// Empty => !currList, !currList => Empty, !Empty => Top == currList
	bool				Invariant (void) const {
							return	(!trapBlockStack.empty() || !currList) && 
									(currList || trapBlockStack.empty())   &&
									(!trapBlockStack.empty()  || callStack.empty()) &&
									(trapBlockStack.empty() || currList == trapBlockStack.front());
						}
	
	bool				IsValidCallContext (const CallContext& c) const;

	bool				IsNativeTryBlockContext (Context context) const
							{ return context == InNativeCode; }
	bool				IsLibraryFunctionContext (Context context) const
							{ return context == InLibraryFunction; }
	bool				IsDeltaCodeContext (Context context) const
							{ return context == InDeltaFunction || context == InDeltaGlobalCode; }

	void				SetUnwinding (bool val) 
							{ isUnwinding = val; }
	bool				ShouldUnwind (void) const
							{ return shouldUnwind; }
	void				SwitchShouldUnwind (void)
							{ shouldUnwind = !shouldUnwind; }
	void				Unwind (DeltaVirtualMachine* caller);
	bool				UnwindDeltaFunction (void);		// Returns true to continue the unwinding loop.
	bool				UnwindLibraryFunction (void);	// Returns true to continue the unwinding loop.
	bool				UnwindGlobalCode (void);		// Returns true to continue the unwinding loop.
	bool				UnwindNativeCode (void);		// Returns true to continue the unwinding loop.

	void				SetException (const DeltaValue& e);
	bool				IsThereAnyTrapBlock (void) const;
	bool				HasTrapBlockAtThisCall (void) const;
	void				Trap (void);

	/////////////////////////////////////////////////////////////////

	public:
	void				PushFunc (DeltaVirtualMachine* vm, Context context);
	void				PushFunc (DeltaVirtualMachine* vm, Context context, util_ui32 pc);
	void				PushFunc (DeltaVirtualMachine* vm, Context context, DeltaLibraryFunc func);
	void				PopFunc (void);	
	bool				Empty (void) const 
							{ DASSERT(Invariant()); return trapBlockStack.empty(); }

	void				PopCallsAffectedByError (DeltaVirtualMachine* vm);
	void				RemoveAllCalls (DeltaVirtualMachine* vm);
	void				SetUnwindingCheckers (void) const;
				
	/////////////////////////////////////////////////////////////////

	void				TrapEnable (DeltaCodeAddress addr);
	void				TrapDisable (DeltaCodeAddress addr);
	void				Trap (DeltaVirtualMachine* vm, DeltaValue* lvalue, util_ui16 trapBlockId);
	void				Throw (DeltaVirtualMachine* vm, const DeltaValue& exception);
	bool				NativeTrap (DeltaValue* lvalue);		// Always returns true
	void				NativeTrapDisable (void);				// When no native exception is caught.
	bool				NativeTry (void);						// Always returns true
	void				NativeThrow (const std::string& from, util_ui32 line, const DeltaValue& exception);

	/////////////////////////////////////////////////////////////////

	DeltaValue&			GetException (void) 
							{ DASSERT(Invariant()); return exceptionValue; }

	void				ResetException (void) 
							{ DASSERT(Invariant()); exceptionValue.Undefine(); }

	//******************************

	bool				PerformUnwindingTests (DeltaVirtualMachine* vm);
	bool				IsUnwinding (void) const
							{ return isUnwinding; }

	/////////////////////////////////////////////////////////////////

	USINGLETON_APISTYLE_DECLARE_GETTER(DeltaExceptionHandling)
	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS

	DeltaExceptionHandling (void);
	~DeltaExceptionHandling();
};

/////////////////////////////////////////////////////////////////

#define	EXCEPTION_HANDLERS	\
	DPTR(DeltaExceptionHandling::GetSingletonPtr())

//-----------------------------------------------------------------

// Use this explicitly inside lib functions that call Delta functions to
// directly unwind.
//
#define	DELTA_ON_UNWINDING_RETURN_TEST(vm, ret)		\
	if (EXCEPTION_HANDLERS->IsUnwinding()) {		\
		vm->GetReturnValue().Undefine();			\
		return ret;									\
	} else

#define	DELTA_ON_UNWINDING_RETURN_TEST_NO_VM(ret)	\
	if (EXCEPTION_HANDLERS->IsUnwinding()) {		\
		return ret;									\
	} else

#define	UNWIND_EMPTY

//////////////////////////////////////////////////////////////////
// Use this when calling directly Delta functions from within
// C++ code (embedding), not being a lib fubnc per se,
// to check if exception handling is under way
// (if no error occured). In case it returns true, it means some
// Delta code is the caller of the C++ so we have to allow
// it unwind further.
//
inline bool DELTA_SHOULD_UNWIND_DUE_TO_EXCEPTION (void)
	{ return EXCEPTION_HANDLERS->IsUnwinding(); }

//////////////////////////////////////////////////////////////////
// Support trying and trapping Delta exceptions from native code (used
// when invocing Delta from C++) and throwing Delta exceptions from native 
// code (used in C++ code invoked from Delta code). Uses as:
//
// DELTA_EXCEPTIONS_NATIVE_TRY				stmt
// DELTA_EXCEPTIONS_NATIVE_TRAP(exception)	stmt (exception visible in stmt as a local DeltaValue)
//

#define	DELTA_EXCEPTIONS_NATIVE_TRY									\
		if (!EXCEPTION_HANDLERS->NativeTry())						\
			DASSERT(false);											\
		else

#define	DELTA_EXCEPTIONS_NATIVE_TRAP(exception)						\
	if		(!EXCEPTION_HANDLERS->IsUnwinding())					\
			EXCEPTION_HANDLERS->NativeTrapDisable();				\
	else if	(DeltaValue exception = false)							\
			DASSERT(false);											\
	else if (!EXCEPTION_HANDLERS->NativeTrap(&exception))			\
			DASSERT(false);											\
	else

#define	DELTA_EXCEPTIONS_NATIVE_THROW(from, exception)				\
	if (EXCEPTION_HANDLERS->IsUnwinding())							\
		UPRIMARYERROR(												\
			"Exception handling: native throw at '%s' line '%d'"	\
			"  from native code '%s' during stack unwinding",		\
			__FILE__, __LINE__, ucstringarg(from)					\
		);															\
	else															\
		EXCEPTION_HANDLERS->NativeThrow(from, __LINE__, exception)

#define	DELTA_EXCEPTIONS_THROW_INSIDE_LIBRARY_FUNC(exception)		\
	if (true) {														\
		if (EXCEPTION_HANDLERS->IsUnwinding())						\
			DPTR(vm)->PrimaryError(									\
				"Native throw at '%s' line '%d'"					\
				" inside lib function '%s' during stack unwinding",	\
				__FILE__, __LINE__, CURR_FUNC						\
			);														\
		else														\
			EXCEPTION_HANDLERS->Throw(vm, DeltaValue(exception));	\
		vm->GetReturnValue().Undefine();							\
		return;														\
	} else

//-----------------------------------------------------------------

#endif	// Do not add stuff beyond this point.



