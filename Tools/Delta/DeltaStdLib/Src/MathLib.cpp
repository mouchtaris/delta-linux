// MathLib.cpp.
// Mathematical library functions for the DeltaVirtualMachine.
// ScriptFighter Project.
// A. Savidis, December 1999.
// Refactoring, mainly restructing, August 2007.
// Refactored December 2007, encompassed in the 'std' API object. 
//

#include "DeltaLibraryCreators.h"
#include "MathLib.h"
#include "ugeometry.h"

#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <stdlib.h>

#define	DELTA_PI				3.1415926535
#define	DELTA_TO_RADS(theta)	(((theta)  * DELTA_PI) / 180.0)

//------------------------------------------------------------------

static void sqr_LibFunc (DeltaVirtualMachine*);
static void sqrt_LibFunc (DeltaVirtualMachine* vm);
static void cos_LibFunc (DeltaVirtualMachine* vm);
static void sin_LibFunc (DeltaVirtualMachine* vm);
static void tan_LibFunc (DeltaVirtualMachine* vm);
static void abs_LibFunc (DeltaVirtualMachine* vm);
static void max_LibFunc (DeltaVirtualMachine* vm);
static void min_LibFunc (DeltaVirtualMachine* vm);
static void random_LibFunc (DeltaVirtualMachine* vm);
static void randomise_LibFunc (DeltaVirtualMachine* vm);
static void power_LibFunc (DeltaVirtualMachine* vm);
static void pi_LibFunc (DeltaVirtualMachine*);
static void fractionalpart_LibFunc (DeltaVirtualMachine*);
static void integerpart_LibFunc (DeltaVirtualMachine*);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "sqr",			sqr_LibFunc				},
	{ "sqrt",			sqrt_LibFunc			},
	{ "cos",			cos_LibFunc 			},
	{ "sin",			sin_LibFunc 			},
	{ "tan",			tan_LibFunc 			},
	{ "abs",			abs_LibFunc 			},
	{ "max",			max_LibFunc 			},
	{ "min",			min_LibFunc 			},
	{ "random",			random_LibFunc			},
	{ "randomise",		randomise_LibFunc		},
	{ "power",			power_LibFunc			},
	{ "pi",				pi_LibFunc				},
	{ "fractionalpart",	fractionalpart_LibFunc	},
	{ "integerpart",	integerpart_LibFunc		}

};

DELTA_LIBRARY_SUBAPI_INSTALLER(DeltaMath, "math", DELTA_STDLIB_NAMESPACE_PREFIX)

//------------------------------------------------------------------

// sqr(x).
// x should be a number. Returns square as a real number.
//
void sqr_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("sqr");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_ZERO_RETURNVALUE);

	DeltaValue*	argObj;
	GET_NUMBER(val, CURR_FUNC, RESET_ZERO_RETURNVALUE, 0);
	DPTR(vm)->GetReturnValue().FromNumber(val*val);
}

//------------------------------------------------------------------
// sqrt(x).
// x should be a number. Returns square root as a real number.
// If x<0, run-time error.
//
void sqrt_LibFunc (DeltaVirtualMachine* vm) { 

	ISSUE_FUNC("sqr");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_ZERO_RETURNVALUE);

	DeltaValue*	argObj;
	GET_NUMBER(val, CURR_FUNC, RESET_ZERO_RETURNVALUE, 0);

	if (val < 0) {
		DPTR(vm)->PrimaryError("in '%s', negative value %f passed!",
			CURR_FUNC,
			val
		);
		RESET_ZERO_RETURNVALUE;
	}
	else
		DPTR(vm)->GetReturnValue().FromNumber(sqrt(val));
}

//------------------------------------------------------------------

#define	TRIG_FUNC(name, call)												\
	ISSUE_FUNC("cos");														\
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_ZERO_RETURNVALUE);				\
	DeltaValue*	argObj;														\
	GET_NUMBER(theta, CURR_FUNC, RESET_ZERO_RETURNVALUE, 0);				\
	if (theta < 0) {														\
		DPTR(vm)->PrimaryError(												\
			DEFORMAT_NEGATIVE_ANGLE,										\
			theta,															\
			CURR_FUNC														\
		);																	\
		RESET_ZERO_RETURNVALUE;												\
	}																		\
	else																	\
		DPTR(vm)->GetReturnValue().FromNumber(call(DELTA_TO_RADS(theta)))

//****************************
// cos(x).
void cos_LibFunc (DeltaVirtualMachine* vm) 
	{ TRIG_FUNC("cos", cos); }

//****************************
// sin(x).
void sin_LibFunc (DeltaVirtualMachine* vm)
	{ TRIG_FUNC("sin", sin); }

//****************************
// tan(x).
void tan_LibFunc (DeltaVirtualMachine* vm) 
	{ TRIG_FUNC("tan", tan); }

//------------------------------------------------------------------
// abs(x).
// Number passed, turned to real.
//
void abs_LibFunc (DeltaVirtualMachine* vm) { 

	ISSUE_FUNC("abs");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_ZERO_RETURNVALUE);
	DeltaValue* argObj;
	GET_NUMBER(num, CURR_FUNC, RESET_ZERO_RETURNVALUE, 0);

	DPTR(vm)->GetReturnValue().FromNumber(uabs(num));
}

//------------------------------------------------------------------

#define	MINMAX_IMPL(cmp)													\
	DeltaAtLeastTotalArgsCheck(1, CURR_FUNC, RESET_ZERO_RETURNVALUE);		\
	DeltaNumberValueType result;											\
	bool firstDone = false;													\
	for (util_ui32 n = DPTR(vm)->TotalActualArgs(), i = 0; i<n; ++i) {		\
		DeltaValue* argObj;													\
		GET_NUMBER(x, CURR_FUNC,RESET_ZERO_RETURNVALUE, i);					\
		if (!firstDone)														\
			result = x, firstDone = true;									\
		else																\
			result = cmp(result, x);										\
	}																		\
	DPTR(vm)->GetReturnValue().FromNumber(result);

// max(x1,...,xn).
// Returns the maximum from a series of numbers.
// If non-numbers are supplied, run-time errors are reported.
//
void max_LibFunc (DeltaVirtualMachine* vm) { 
	ISSUE_FUNC("max");
	MINMAX_IMPL(umax);
}

//------------------------------------------------------------------
// min(x1,...,xn).
// Returns the minimum from a series of numbers.
// If non-numbers are supplied, run-time errors are reported.
//
void min_LibFunc (DeltaVirtualMachine* vm) { 
	ISSUE_FUNC("min");
	MINMAX_IMPL(umin);
}

//------------------------------------------------------------------

// random(n).
// Returns random number in 0...n-1.
// n should be positive integer.
//
void random_LibFunc (DeltaVirtualMachine* vm) { 

	ISSUE_FUNC("random");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_ZERO_RETURNVALUE);

	DeltaValue* argObj;
	GET_UNSIGNEDINT(n, CURR_FUNC, RESET_ZERO_RETURNVALUE, 0);

	DPTR(vm)->GetReturnValue().FromNumber(
		n == 0 ? 0 : ((DeltaNumberValueType) (rand() % ((util_ui32) n)))
	);
}

//------------------------------------------------------------------
// randomise().
// Initialises the random number generator with the current time.
//
void randomise_LibFunc (DeltaVirtualMachine* vm) {
	ISSUE_FUNC("randomise");
	DeltaTotalArgsCheck(0, CURR_FUNC, RESET_EMPTY);
	srand((util_ui32) time((time_t*) 0));
}

//------------------------------------------------------------------
// power(x,y).
// x^y.
//
void power_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("power");
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_ZERO_RETURNVALUE);

	DeltaValue* argObj;
	GET_NUMBER(x, CURR_FUNC, RESET_ZERO_RETURNVALUE, 0);
	GET_NUMBER(y, CURR_FUNC, RESET_ZERO_RETURNVALUE, 1);

	DPTR(vm)->GetReturnValue().FromNumber(
		pow(x, y)
	);
}

//------------------------------------------------------------------

// pi().
// Returns the PI number.
//
void pi_LibFunc (DeltaVirtualMachine* vm) {
	
	ISSUE_FUNC("pi");
	DeltaTotalArgsCheck(0, CURR_FUNC, RESET_ZERO_RETURNVALUE);

	DPTR(vm)->GetReturnValue().FromNumber(DELTA_PI);
}

//------------------------------------------------------------------

#define	FRACTIONAL_OR_INTEGER_PART(func, call)						\
void func##_LibFunc (DeltaVirtualMachine* vm) {						\
	ISSUE_FUNC(#func);												\
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_ZERO_RETURNVALUE);		\
	DeltaValue* argObj;												\
	GET_NUMBER(x, CURR_FUNC, RESET_ZERO_RETURNVALUE, 0);			\
	DPTR(vm)->GetReturnValue().FromNumber(call(x));					\
}

// fractionalpart(x).
FRACTIONAL_OR_INTEGER_PART(fractionalpart,	ufractionalpart)

// integerpart(x).
FRACTIONAL_OR_INTEGER_PART(integerpart,		uintegerpart)

//------------------------------------------------------------------
