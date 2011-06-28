// DeltaAssertExtensions.h
// Extensions regarding the assert functionality.
// Script Fighter Project.
// A. Savidis, June 2006.
//
#ifndef	DELTAASSERTEXTENSIONS_H
#define	DELTAASSERTEXTENSIONS_H

#include "utypes.h"
#include "DDebug.h"
#include "DeltaStdDefs.h"

#define	ASSERT_ABORT	0
#define	ASSERT_CONTINUE	1
#define	ASSERT_DEBUG	2

/////////////////////////////////////////////////////////////

class DeltaVirtualMachine;

class DVM_CLASS DeltaAssertExtensions {

	public:
	typedef bool (*ErrorInterfaceCallback)(const std::string&, void*);
	typedef util_ui32 (*AssertInterfaceCallback)(DeltaVirtualMachine*, const std::string& expr, void*);
	typedef void (*AbortCallback)(void);

	private:
	static ErrorInterfaceCallback	errorInterfaceCallback;
	static void*					errorClosure;
	static void*					assertClosure;
	static AssertInterfaceCallback	assertInterfaceCallback;
	static AbortCallback			abortCallback;

	static bool			DefaultErrorInterfaceCallback (const std::string&, void* closure);
	static util_ui32	DefaultAssertInterfaceCallback (DeltaVirtualMachine* vm, const std::string& expr, void* closure);
	static void			DefaultAbortCallback (void);

	//***************************

	public:
	static void			SetAssertInterfaceCallback (AssertInterfaceCallback f, void* c = (void*) 0);
	static void			SetErrorInterfaceCallback (ErrorInterfaceCallback f, void* c = (void*) 0);
	static void			SetAbortCallback (AbortCallback f);

	static AssertInterfaceCallback
						GetAssertInterfaceCallback (void);
	static ErrorInterfaceCallback
						GetErrorInterfaceCallback (void);
	static AbortCallback
						GetAbortCallback (void);

	static bool			ErrorInterfaceResponse (const std::string& msg);
	static util_ui32	AssertInterfaceResponse (DeltaVirtualMachine* vm, const std::string& expr);
	static void			Abort (void);
};

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
