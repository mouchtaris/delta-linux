/**
 *	SparrowLibCommon.h
 *
 *	-- Sparrow IDE extension for Delta --
 *
 *	Common include file for the sparrow library
 *  function implementation.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	January 2009
 */
#ifndef SPARROW_LIB_FUNC_COMMON_H_
#define SPARROW_LIB_FUNC_COMMON_H_

#include "DeltaVirtualMachine.h"
#include "SparrowLibFuncNames.h"

#define ARG(x) boost::get<x>(args)

#define RUNTIME_ERROR(msg, func)														\
	do {																				\
		DPTR(vm)->PrimaryError((std::string(func) + ": " + std::string(msg)).c_str());	\
		DPTR(vm)->GetReturnValue().FromNil();											\
		return;																			\
	} while (false)

#define INSTALL_LIBRARY_FUNCTOR(f) \
	DeltaLibFuncBinder::Add((std::string(SPW_NAMESPACE_PREFIX) + f().Id()).c_str(), &Perform<f>)

namespace ide
{
	//-------------------------------------------------------//
	//---- Template Library Function Performer --------------//

	template<class Functor> static void Perform(DeltaVirtualMachine* vm)
	{
		const Functor f;
		if (!f.HasValidArgCount(DPTR(vm)->TotalActualArgs()))
			RUNTIME_ERROR(f.ArgCountError(), f.Id());

		DeltaArgumentVec arguments;
		for (uint i=0; i < DPTR(vm)->TotalActualArgs(); ++i)
			arguments.push_back(DPTR(vm)->GetActualArg(i));

		try { f(vm, arguments); }
		catch (std::exception& e) { RUNTIME_ERROR(e.what(), f.Id()); }
	}
}

#endif	///< SPARROW_LIB_FUNC_COMMON_H_
