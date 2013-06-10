// DeltaLibraryFunctionContext.cpp
// Provides a library function context to avoid the use
// of macros.
// wxWidgets DELTA wrapper.
// A. Maragudakis, May 2013.
//

#include "DeltaLibraryFunctionContext.h"

////////////////////////////////////////////////////////////////

void DeltaLibraryFunctionContext::UpdateLocals
	(
		DeltaVirtualMachine** _vm,
		const char** _CURR_FUNC,
		util_ui32* __argNo,
		std::string* __sig1,
		std::string* __sig2
	)
{
	if (_vm)		*_vm		= vm;
	if (_CURR_FUNC)	*_CURR_FUNC	= CURR_FUNC;
	if (__argNo)	*__argNo	= _argNo;
	if (__sig1)		*__sig1		= _sig1;
	if (__sig2)		*__sig2		= _sig2;
}

void DeltaLibraryFunctionContext::UpdateContext
	(
		util_ui32 __argNo,
		const std::string __sig1,
		const std::string& __sig2
	)
{
	_argNo				= __argNo;
	_sig1				= __sig1;
	_sig2				= __sig2;
}

////////////////////////////////////////////////////////////////

DeltaLibraryFunctionContext::DeltaLibraryFunctionContext
	(
		DeltaVirtualMachine* _vm, const char* funcName,
		util_ui32 __argNo, const std::string __sig1,
		const std::string& __sig2
	) :
		vm(_vm), CURR_FUNC(funcName), _argNo(__argNo),
		_sig1(__sig1), _sig2(__sig2)
		{ }

////////////////////////////////////////////////////////////////
