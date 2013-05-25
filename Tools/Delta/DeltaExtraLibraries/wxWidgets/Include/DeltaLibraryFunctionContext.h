// DeltaLibraryFunctionContext.h
// Provides a library function context to avoid the use
// of macros.
// wxWidgets DELTA wrapper.
// A. Maragudakis, May 2013.
//

#ifndef DELTA_LIBRARY_FUNCTION_CONTEXT
#define DELTA_LIBRARY_FUNCTION_CONTEXT

#include "DeltaVirtualMachine.h"
#include "utypes.h"
#include <string>

////////////////////////////////////////////////////////////////

struct DeltaLibraryFunctionContext {
	DeltaVirtualMachine*	vm;
	const char*				CURR_FUNC;
	util_ui32				_argNo;
	std::string				_sig1, _sig2;

	void					UpdateLocals
							(
								DeltaVirtualMachine** _vm,
								const char** _CURR_FUNC,
								util_ui32* __argNo,
								std::string* __sig1,
								std::string* __sig2
							);

	void					UpdateContext
							(
								util_ui32 __argNo,
								const std::string __sig1,
								const std::string& __sig2
							);

	DeltaLibraryFunctionContext
	(
		DeltaVirtualMachine* _vm, const char* funcName,
		util_ui32 __argNo, const std::string __sig1,
		const std::string& __sig2
	);
};

////////////////////////////////////////////////////////////////

#define WX_CREATE_LIBFUNC_LOCALS		\
	DeltaVirtualMachine* vm		= NULL;	\
	const char *CURR_FUNC		= "";	\
	util_ui32 _argNo			= 0;	\
	std::string _sig1, _sig2;

#define WX_CREATE_CONTEXT(contextName)											\
	DeltaLibraryFunctionContext contextName(vm, CURR_FUNC, _argNo, _sig1, _sig2)

////////////////////////////////////////////////////////////////

#endif