// BitOpLib.cpp.
// Bit operators' library functions for the DeltaVirtualMachine.
// ScriptFighter Project.
// A. Savidis, December 1999.
// Refactored December 2007, encompassed in the 'std' API object. 
//
#include "DeltaLibraryCreators.h"
#include "BitOpLib.h"

//------------------------------------------------------------------

static void bitand_LibFunc (DeltaVirtualMachine*);
static void bitor_LibFunc (DeltaVirtualMachine*);
static void bitnot_LibFunc (DeltaVirtualMachine*);
static void bitxor_LibFunc (DeltaVirtualMachine*);
static void bitshright_LibFunc (DeltaVirtualMachine*);
static void bitshleft_LibFunc (DeltaVirtualMachine*);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "bitand",		bitand_LibFunc		},
	{ "bitor",		bitor_LibFunc		},
	{ "bitnot",		bitnot_LibFunc		},
	{ "bitxor",		bitxor_LibFunc		},
	{ "bitshright", bitshright_LibFunc	},
	{ "bitshleft",	bitshleft_LibFunc	}
};

DELTA_LIBRARY_SUBAPI_INSTALLER(DeltaBitOp, "binary", DELTA_STDLIB_NAMESPACE_PREFIX)

//------------------------------------------------------------------

#define	BINARY_BITOP(func,op)											\
	ISSUE_FUNC(func);													\
	DeltaTotalArgsCheck(2, func, RESET_ZERO_RETURNVALUE)				\
	DeltaValue* argObj;													\
	GET_NUMBER(x, CURR_FUNC, RESET_ZERO_RETURNVALUE, 0);				\
	GET_NUMBER(y, CURR_FUNC, RESET_ZERO_RETURNVALUE, 1);				\
	vm->GetReturnValue().FromNumber(									\
		(DeltaNumberValueType) (((util_ui32) x) op ((util_ui32) y))		\
	);

#define	VARIADIC_BITOP(func,op)											\
	ISSUE_FUNC(func);													\
	DeltaAtLeastTotalArgsCheck(2, func, RESET_ZERO_RETURNVALUE)			\
	DeltaValue* argObj;													\
	GET_NUMBER(arg0, CURR_FUNC, RESET_ZERO_RETURNVALUE, 0);				\
	util_ui32 retval = (util_ui32) arg0;								\
	for (util_ui16 n = DPTR(vm)->TotalActualArgs(), i = 1; i < n; i++) {\
		GET_NUMBER(argi, CURR_FUNC, RESET_ZERO_RETURNVALUE, i);			\
		retval op##= (util_ui32) argi;									\
	}																	\
	vm->GetReturnValue().FromNumber((DeltaNumberValueType) retval);

//------------------------------------------------------------------

void bitand_LibFunc (DeltaVirtualMachine* vm) 
	{ VARIADIC_BITOP("bitand", &); }

void bitor_LibFunc (DeltaVirtualMachine* vm) 
	{  VARIADIC_BITOP("bitor", |); }

void bitxor_LibFunc (DeltaVirtualMachine* vm) 
	{  VARIADIC_BITOP("bitxor", ^); }

void bitshright_LibFunc (DeltaVirtualMachine* vm) 
	{  BINARY_BITOP("bitshright", >>); }

void bitshleft_LibFunc (DeltaVirtualMachine* vm) 
	{  BINARY_BITOP("bitshleft", <<); }

void bitnot_LibFunc (DeltaVirtualMachine* vm) { 
	
	ISSUE_FUNC("bitnot");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_FALSE_RETURNVALUE);

	DeltaValue* argObj; 
	GET_NUMBER(x, CURR_FUNC, RESET_ZERO_RETURNVALUE, 0);
	vm->GetReturnValue().FromNumber((DeltaNumberValueType) (util_ui32) ~((util_ui32) x));
}

//------------------------------------------------------------------
