// DeltaLibFuncOverloading.cpp
// Implementation of lib func overloading functions.
// ScriptFigher Project.
// A. Savidis, February 2008.
//

#include "DeltaLibFuncOverloading.h"
#include "DeltaRuntimeError.h"
#include "DeltaValue.h"
#include "utypes.h"
#include "DDebug.h"

////////////////////////////////////////////////////////////////////

std::string DeltaLibFuncOverloading::Signature (DeltaVirtualMachine* vm) {

	std::string s;

	for (util_ui32 i = 0; i < DPTR(vm)->TotalActualArgs(); ++i) {

		DeltaValue* arg = vm->GetActualArg(i);

		if (arg->Type() == DeltaValue_ExternId) {
			std::string t = arg->GetExternIdTypeString();
			if (t != DELTA_EXTERNID_TYPESTR_UNDEF) 
				{ s.append(t); continue; }
		}

		s.append(arg->TypeStr());
	}

	return s.empty() ? "void" : s;
}

////////////////////////////////////////////////////////////////////

void DeltaLibFuncOverloading::Dispatch (
		DeltaVirtualMachine*	vm, 
		Entry*					funcs, 
		util_ui32				n,
		const char*				funcName
	) {

	std::string			sig = Signature(vm);
	DeltaLibraryFunc	func = (DeltaLibraryFunc) 0;

	while (n--) {
		if ((funcs->sig)) {
			if (ustrprefix(sig.c_str(), funcs->sig))
				{ func = funcs->func; break; }
			else
				func = funcs->func; // Fallback.
		}
		++funcs;
	}

	if (!func)
		vm->SetErrorCode(DELTA_UNRESOLVED_LIBFUNC_ERROR)->PrimaryError(
			"in call to overloaded %s(), no match for signature '%s'!",
			funcName,
			sig.c_str()
		);
	else
		(*func)(vm);
}

////////////////////////////////////////////////////////////////////
