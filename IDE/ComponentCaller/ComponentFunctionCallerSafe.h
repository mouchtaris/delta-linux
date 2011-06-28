/**
 *	ComponentFunctionCallerSafe.h
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
#ifndef IDE_COMPONENT_FUNCTION_CALLER_SAFE_H_
#define IDE_COMPONENT_FUNCTION_CALLER_SAFE_H_

#include "Common.h"
#include "IDECore.h"
#include "Message.h"
#include "ComponentFunctionCaller.h"

namespace ide
{
	class Component;
	class Handle;

	//----------------------------
	//-- class ComponentFunctionCallerSafe

	class _IDE_API ComponentFunctionCallerSafe :
		public ComponentFunctionCaller
	{
	public:
		///--- constructors / destructor
		ComponentFunctionCallerSafe(void) {}

		ComponentFunctionCallerSafe(Component* caller, const Handle& handle, const std::string& function)
			: ComponentFunctionCaller(caller, handle, function) {}

		ComponentFunctionCallerSafe(Component* caller, const std::string& classId, const std::string& function)
			: ComponentFunctionCaller(caller, classId, function) {}

		ComponentFunctionCallerSafe(const std::string& callerId, const Handle& handle, const std::string& function)
			: ComponentFunctionCaller(callerId, handle, function) {}

		ComponentFunctionCallerSafe(const std::string& callerId, const std::string& classId, const std::string& function)
			: ComponentFunctionCaller(callerId, classId, function) {}

		~ComponentFunctionCallerSafe(void) {}

		bool Invoke(Message& result, const Buffer& args) const;
	};

	typedef ComponentFunctionCallerSafe SafeCall;
}

#endif	//IDE_COMPONENT_FUNCTION_CALLER_SAFE_H_
