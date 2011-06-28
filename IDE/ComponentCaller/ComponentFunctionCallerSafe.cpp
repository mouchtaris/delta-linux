/**
 *	ComponentFunctionCallerSafe.cpp
 *
 *	-- Component Function Caller --
 *
 *	A special caller class that checks the existance
 *	of a component instance before it calls its function.
 *	If no instance is available the function is not invoked
 *	and no error conditions are triggered.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	July 2007
 */
#include "ComponentFunctionCallerSafe.h"
#include "ComponentRegistry.h"

namespace ide
{
	//-------------------------------------------------------//
	//---- class ComponentFunctionCallerSafe ----------------//

	bool ComponentFunctionCallerSafe::Invoke(ide::Message& result, const Buffer& args) const
	{
		try {
			ComponentFunctionCaller::Invoke(result, args);
			return true;
		}
		catch (CallException&) {
			// TODO: Log exception, but otherwise ignore
			return false;
		}
	}

	//-----------------------------------------------------------------------
}
