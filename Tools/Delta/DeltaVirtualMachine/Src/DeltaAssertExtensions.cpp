// DeltaAssertExtensions.cpp
// Extensions regarding the assert functionality.
// Script Fighter Project.
// A. Savidis, June 2006.
//
#include "DeltaAssertExtensions.h"
#include "DeltaVirtualMachine.h"

///////////////////////////////////////////////////////////////////////

DeltaAssertExtensions::ErrorInterfaceCallback 
		DeltaAssertExtensions::errorInterfaceCallback = &DefaultErrorInterfaceCallback;

DeltaAssertExtensions::AssertInterfaceCallback 
		DeltaAssertExtensions::assertInterfaceCallback = &DefaultAssertInterfaceCallback;

DeltaAssertExtensions::AbortCallback 
	DeltaAssertExtensions::abortCallback = &DefaultAbortCallback;

void* DeltaAssertExtensions::assertClosure = (void*) 0;
void* DeltaAssertExtensions::errorClosure = (void*) 0;

///////////////////////////////////////////////////////////////////////

util_ui32 DeltaAssertExtensions::DefaultAssertInterfaceCallback (DeltaVirtualMachine* vm, const std::string& expr, void* unused) {
	DPTR(vm)->Error("assertion '%s' failed, aborting!", expr.c_str());
	return ASSERT_ABORT; 
}

bool DeltaAssertExtensions::DefaultErrorInterfaceCallback (const std::string& msg, void* unused) 
	{ return false; }

void DeltaAssertExtensions::DefaultAbortCallback (void) 
	{ abort(); }

///////////////////////////////////////////////////////////////////////

void DeltaAssertExtensions::SetAssertInterfaceCallback (AssertInterfaceCallback f, void* c)
	{ assertInterfaceCallback = f; assertClosure = c; }

void DeltaAssertExtensions::SetAbortCallback (AbortCallback f)
	{ abortCallback = f; }

void DeltaAssertExtensions::SetErrorInterfaceCallback (ErrorInterfaceCallback f, void* c)
	{ errorInterfaceCallback = f; errorClosure = c; }

DeltaAssertExtensions::AssertInterfaceCallback DeltaAssertExtensions::GetAssertInterfaceCallback (void)	
	{ return assertInterfaceCallback; }

DeltaAssertExtensions::ErrorInterfaceCallback DeltaAssertExtensions::GetErrorInterfaceCallback (void)		
	{ return errorInterfaceCallback; }

DeltaAssertExtensions::AbortCallback DeltaAssertExtensions::GetAbortCallback (void)
	{ return abortCallback; }

///////////////////////////////////////////////////////////////////////

util_ui32 DeltaAssertExtensions::AssertInterfaceResponse (DeltaVirtualMachine* vm, const std::string& expr)
	{ DASSERT(assertInterfaceCallback); return (*assertInterfaceCallback)(vm, expr, assertClosure); }

void DeltaAssertExtensions::Abort (void)
	{ DASSERT(abortCallback); (*abortCallback)(); }

bool DeltaAssertExtensions::ErrorInterfaceResponse (const std::string& msg) {
	std::string nl = msg[msg.length() - 1] == '\n' ? "" : "\n";
	return (*errorInterfaceCallback)(msg + nl + "Debug program?", errorClosure);
}

///////////////////////////////////////////////////////////////////////
