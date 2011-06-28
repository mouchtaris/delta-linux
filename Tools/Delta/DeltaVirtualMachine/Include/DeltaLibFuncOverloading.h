// DeltaLibFuncOverloading.h
// A simple and fast way to support string-signature based overloading 
// of user-defined library functions, calling the 1st alternative
// whos signature has as prefix the actual arguments signature.
// ScriptFigher Project.
// A. Savidis, February 2008.
//

#ifndef	DELTALIBFUNCOVERLOADING_H
#define	DELTALIBFUNCOVERLOADING_H

#include "DeltaVirtualMachine.h"
#include "DeltaValue.h"
#include "utypes.h"
#include <string>

////////////////////////////////////////////////////////////////////

class DVM_CLASS DeltaLibFuncOverloading {

	private:
	static std::string Signature (DeltaVirtualMachine* vm);

	public:
	struct DVM_CLASS Entry {
		const char*			sig;
		DeltaLibraryFunc	func;
	};
	
	static void Dispatch (
				DeltaVirtualMachine*	vm, 
				Entry*					funcs, 
				util_ui32				n,
				const char*				funcName
		);
};

////////////////////////////////////////////////////////////////////

#define	DELTA_OVERLOADLIBFUNC_START(func) \
	static DeltaLibFuncOverloading::Entry func##Overloading[] = { \

#define	DELTA_OVERLOADLIBFUNC_VERSION(sig, func) \
	{ sig, func },

#define	DELTA_OVERLOADLIBFUNC_FALLBACK(func) \
	{ (char*) 0, func },

#define	DELTA_OVERLOADLIBFUNC_END \
	{ (char*) 0, (DeltaLibraryFunc) 0 } };

#define	DELTA_OVERLOADLIBFUNC_DISPATCH(func) \
	DeltaLibFuncOverloading::Dispatch( \
		vm, \
		func##Overloading, \
		uarraysize(func##Overloading) - 1, \
		#func \
	);

////////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.