// udebugsupport.h
// This file has been reduced to contain only runtime tracing
// macros that may also be used in normal prorgam mode.
// ScriptFighter Project.
// A. Savidis, July 2002.
//

#ifndef	UDEBUGSUPPORT_H
#define	UDEBUGSUPPORT_H

//---------------------------------------------------------------
// Use this macro to call in a function, which IS NOT time critical,
// code to be executed upon exit. The EX version allows linkage to enclosing
// block data via a user-defined closure type.
//
#define	UTIL_ONFUNC_RETURN(func, call)	\
class _TempClass##func {				\
	public:								\
	_TempClass##func(void){}			\
	~_TempClass##func() { call }		\
} _tempInst##func##__LINE__

#define	UTIL_ONFUNC_RETURN_EX(func, call, closure)	\
class _TempClass##func {							\
	private:										\
	void* p;										\
	public:											\
	void* Closure (void) { return p; }				\
	_TempClass##func(void* c) : p(c) {}				\
	~_TempClass##func() { call }					\
	} _tempInst##func##__LINE__(closure)

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.

