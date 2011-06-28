// DeltaClosureHandling.h
// Global handling of closures for the VM.
// Delta VirtualMachine implementation.
// ScriptFighter Project.
// A. Savidis, September 2010.
//

#ifndef	DELTACLOSUREHANDLING_H
#define	DELTACLOSUREHANDLING_H

#include "DeltaClosureCreator.h"
#include "usingleton.h"
#include <list>
#include <functional>

//-----------------------------------------------------------

class DVM_CLASS DeltaClosureHandling {

	friend class DVM_CLASS DeltaClosureCreator;

	private:
	typedef std::list<DeltaClosureCreator>					CreatorStack;
	typedef std::pair<DeltaVirtualMachine*, DeltaClosure*>	ClosureEntry;
	typedef std::list<ClosureEntry>							ClosureStack;
	
	static DeltaClosureHandling* singletonPtr;

	CreatorStack			creatorStack;	// Used to create closures for the current top function.
	ClosureStack			closureStack;	// Used to access closure vars from the current top function.

	DeltaClosureCreator&	GetCreator (void) 
								{ DASSERT(!creatorStack.empty()); return creatorStack.front(); }
	DeltaClosure*			GetTopClosurePrivileged (void) const;	// Non-const value returned.

	/////////////////////////////////////////////////////////////

	public:
	void					PushFunc (
								DeltaVirtualMachine*	vm, 
								DeltaClosure*			closure
							);
	void					PopFunc (DeltaVirtualMachine* vm);	// This assumes all stack vars are popped.
	void					PopFunc (							// Inclusively actuals[0]...actuals[n - 1]
								DeltaVirtualMachine*	vm, 
								DeltaValue*				sp,		// Inclusively sp[0]...sp[n - 1]
								util_ui32				n
							);
	void					PopBlock (							// Inclusively block[0]...block[n - 1]
								DeltaVirtualMachine*	vm, 
								DeltaValue*				block, 
								util_ui32				n
							);
	DeltaClosure*			MakeClosureIfNeeded (				// Creates closure if function uses closure vars.
								DeltaVirtualMachine*	vm,
								const DeltaStdFuncInfo*	func
							);
	const DeltaClosure*		GetTopClosure (void) const;
	const DeltaClosure*		GetClosure (util_ui32 stackIndex) const;

	void					PopCallsAffectedByError (DeltaVirtualMachine* vm);
	void					RemoveAllCalls (DeltaVirtualMachine* vm);

	USINGLETON_APISTYLE_DECLARE_GETTER(DeltaClosureHandling)
	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS

	DeltaClosureHandling();
	~DeltaClosureHandling();
};

/////////////////////////////////////////////////////////////

#define	CLOSURE_HANDLERS	\
	DPTR(DeltaClosureHandling::GetSingletonPtr())

//-----------------------------------------------------------

#endif	// Do not ad stuff beyond this point.
