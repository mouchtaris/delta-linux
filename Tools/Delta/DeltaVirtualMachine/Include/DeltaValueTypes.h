// DeltaValueTypes.h
// Types that are involved in a DeltaValue.
// These are all forward declarations.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, January 2010 (appeared from refactoring).
//

#ifndef	DELTAVALUETYPES_H
#define	DELTAVALUETYPES_H

#include "DeltaStdDefs.h"
#include "ustrings.h"

//////////////////////////////////////////////////////////////////

class DVM_CLASS DeltaValue;
class DVM_CLASS	DeltaClosureVarProxy;
class DVM_CLASS	DeltaClosure;
class DVM_CLASS	DeltaClosureCreator;
class DVM_CLASS	DeltaClosureHandling;
class DVM_CLASS DeltaString;
class DVM_CLASS	DeltaTable;
class DVM_CLASS	DeltaObject;
class DVM_CLASS DeltaExternId;
class DVM_CLASS DeltaExternIdFactory;
class DVM_CLASS DeltaExternIdFieldGetter;
class DVM_CLASS DeltaVirtualMachine;
class DVM_CLASS DeltaLibraryFuncArgsBinder;
class			DeltaRelationalErrorHandler;
class			DeltaArithmeticErrorHandler;

typedef void (*DeltaLibraryFunc) (DeltaVirtualMachine* vm);

//////////////////////////////////////////////////////////////////

enum DeltaRelOpType {
	DeltaRelOp_EQ				= 0,
	DeltaRelOp_NE				= 1,
	DeltaRelOp_GT				= 2,
	DeltaRelOp_GE				= 3,
	DeltaRelOp_LT				= 4,
	DeltaRelOp_LE				= 5,
	DeltaRelOp_INVALID			= 6
};

#define	TOTAL_DELTAOBJECT_RELOP_TYPES	6

//////////////////////////////////////////////////////////////////

enum DeltaArithOpType {
	DeltaArithOp_ADD			= 0,
	DeltaArithOp_SUB			= 1,
	DeltaArithOp_MUL			= 2,
	DeltaArithOp_DIV			= 3,
	DeltaArithOp_MOD			= 4,
	DeltaArithOp_INVALID		= 5
};

#define	TOTAL_DELTAOBJECT_ARITHOP_TYPES	5

//////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
