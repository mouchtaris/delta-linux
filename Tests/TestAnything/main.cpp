/**
 *	main.cpp
 *
 */
#include "Common.h"

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <iostream>
#include <algorithm>

////////////////////////////////////////////////////////////////////////

template<typename TExpr>
bool uf (TExpr e) { return !!e; }

////////////////////////////////////////////////////////////////////////

template<class Signature>
class function_caller_t;

template<class R>
struct function_caller_t<R (void)> {
	function_caller_t (R (*_func) ()) : func(_func) {}
	bool operator() (void) const { return !!func(); }
	R (*func)();
};

template<>
struct function_caller_t<void (void)> {
	function_caller_t (void (*_func) ()) : func(_func) {}
	bool operator() (void) const { func(); return true; }
	void (*func)();
};

template<class R>
inline function_caller_t<R ()> function_caller (R (*func) ())
	{ return function_caller_t<R ()>(func); }

template<>
inline function_caller_t<void ()> function_caller (void (*func) ())
	{ return function_caller_t<void ()>(func); }

////////////////////////////////////////////////////////////////////////

void foo (void) {}
double lala (void) { return 0.0; }

////////////////////////////////////////////////////////////////////////

int main (void)
{
	std::cout << "Hello world" << std::endl;

	std::cout << function_caller(foo)() << std::endl;
	std::cout << function_caller(lala)() << std::endl;

	uf(lala());

	return 0;
}

////////////////////////////////////////////////////////////////////////
