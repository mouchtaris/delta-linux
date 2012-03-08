// DeltaStddefs.h
// Common Delta definitions.
// ScriptFighter Project.
// A. Savidis, August 2005 (moved in a single file).
//

#ifndef	DELTASTDDEFS_H
#define	DELTASTDDEFS_H

#include "utypes.h"

//-----------------------------------------------------------

#define	DELTA_NUMBER_DOUBLE

#if		defined(DELTA_NUMBER_FLOAT) && defined(DELTA_NUMBER_DOUBLE)
#error	"Cannot define both DELTA_NUMBER_DOUBLE and DELTA_NUMBER_FLOAT."
#endif

#if	!defined(DELTA_NUMBER_FLOAT) && !defined(DELTA_NUMBER_DOUBLE)
#error "Should define either DELTA_NUMBER_FLOAT or DELTA_NUMBER_DOUBLE."
#endif

#ifdef	DELTA_NUMBER_FLOAT
typedef	float DeltaNumberValueType;
#endif

#ifdef	DELTA_NUMBER_DOUBLE
typedef	double DeltaNumberValueType;
#endif

//-----------------------------------------------------------

#define	DELTA_MAXNESTED_FUNCTIONS				32
#define	DELTA_FUNCSERIAL_OF_GLOBAL_CODE			0
#define	DELTA_MAX_STRING_CONSTS					1024
#define	DELTA_MAX_EXTERN_FUNCS					1024
#define	DELTA_MAX_NUMERIC_CONSTS				1024

typedef util_ui16		DeltaCodeAddress;
typedef util_ui16		DeltaMemAddress;

struct	DeltaSymbol;

//-----------------------------------------------------------
// Debug-time information for current function in call stack.
// Support for library / user function resolution.
//
#define	DELTA_MAXLIBFUNC_INDEX					0xffff				// At most 65536 library functions allowed.
#define	DELTA_MAXINSTR_INDEX					0xffff				// At most 65536 target instructions allowed.
#define	DELTA_MAIN_BLOCK_VALUE					0					// This is always the main block.
#define	DELTA_BLOCKIDSTACKS_RESERVED			1					// Number of reserved blockid stacks (for global).
#define	DELTA_GLOBALBLOCKIDSTACK_SIZE			1					// Size of global block id stack (one block).
#define	DELTA_IGNORE_BLOCKID_VALUE				0xFFFF				// A block id signifying that it is to be ignored.

//-----------------------------------------------------------
// Those macros are strongly related to the code generation policy.
// Each time something is changed there, we need to update those
// as needed, or severe run-time errors will be generated.
//
#define	DELTA_SAVED_ENVIRONMENT_SIZE			4					// Nunber of std items stored in function calls.
#define	DELTA_NIL_CODE_LABEL					0					
#define	DELTA_FUNCCALL_PC_OFFSET				1					// Offset to return upon function call.
#define	DELTA_TOTAL_REGS						9
#define	DELTA_RETVALUE_OFFSET					0					// At global position 0.
#define	DELTA_FUNC_LEVEL_UKNOWN					0					// When the function level is 0.
#define	DELTA_RESERVED_GLOBAL_ITEMS				1					// The return value
#define	DELTA_CANTBE_A_FUNCTION_ADDRESS			0					// 0 address is reserved
#define	DELTA_CANTBE_A_SOURCE_LINE				0					// 0 line impossible anyway
#define	DELTA_FUNCENTER_USER_INSTR_OFFSET		+1					// Offset of user instruction from FUNCENTER.
#define	DELTA_STATICS_OWNING_BLOCKID			0					// All static variables are assumed for block 0
#define	DELTA_USER_CODE_START					1					// The instruction index from which user code can start (0 is reserved).

#define	DELTA_CALL_MAX_STD_ARGUMENTS			2					// 'arguments' and 'self'
#define	DELTA_METHOD_STD_ARGUMENTS				2					// 'arguments' and 'self'
#define	DELTA_PROGFUNC_STD_ARGUMENTS			1					// 'arguments'

#define	DELTA_NEWSELF_POINTER_ID				"@self"				// The currently constructed object.
#define	DELTA_SELF_POINTER_ID					"self"				// An artifical formal argument.
#define	DELTA_STATICVAR_NAME_PREFIX				"static."
#define	DELTA_HIDDEN_SYMBOL_ID_PREFIX			"_"
#define	DELTA_TEMPVAR_NAME_PREFIX				DELTA_HIDDEN_SYMBOL_ID_PREFIX "t"
#define	DELTA_ANONYMOUS_FUNC_NAME_PREFIX		DELTA_HIDDEN_SYMBOL_ID_PREFIX "f"
#define	DELTA_ANONYMOUS_METHOD_NAME_PREFIX		DELTA_HIDDEN_SYMBOL_ID_PREFIX "m"
#define	DELTA_IMPORTEDFUNCVAR_NAME_PREFIX		DELTA_HIDDEN_SYMBOL_ID_PREFIX "l"
#define	DELTA_HIDDENVAR_ID_PREFIX				DELTA_HIDDEN_SYMBOL_ID_PREFIX[0]
#define	DELTA_HIDDENCONSTVAR_NAME_PREFIX		"__"
#define	DELTA_IMPORTEDFUNCVAR_NAME_FORMAT		DELTA_IMPORTEDFUNCVAR_NAME_PREFIX "_%s_%s"	// _l_<lib_name>_<func_name>

#define	DELTA_ARGUMENTS_POINTER_ID				"arguments"			// An artifical formal argument.
#define	DELTA_TABLEARGUMENTS_TOTAL				"total"				// In dynamic arguments allows define total
#define	DELTA_TABLEARGUMENTS_START				"start"				// and from where to start counting.
#define	DELTA_TABLEARGUMENTS_MAXTOTAL			256

#define	DELTA_LINE_NOT_VISIBLE					0xFFFF
#define	DELTA_VM_TYPE_STR						"std::vm"
#define	DELTA_TABLE_CLASS_ID					"Table"
#define	DELTA_OBJECT_CLASS_ID					"Object"

//-----------------------------------------------------------
// Overloaded operators.
//
#define	DELTA_OPERATOR_OBJECT_FUNCTIONCALL		"()"
#define	DELTA_OPERATOR_OBJECT_SET				".="
#define	DELTA_OPERATOR_OBJECT_GET				"."
#define	DELTA_OPERATOR_OBJECT_ASSIGN			"="
#define	DELTA_OPERATOR_OBJECT_TYPECAST			"=()"
#define	DELTA_OPERATOR_OBJECT_TOSTRING			"tostring()"

//-----------------------------------------------------------
// Type strings.
//
#define	DELTA_NUMBER_TYPESTRING					"Number"
#define	DELTA_STRING_TYPESTRING					"String"
#define	DELTA_TABLE_TYPESTRING					"Table"
#define	DELTA_OBJECT_TYPESTRING					"Object"
#define	DELTA_PROGRAMFUNC_TYPESTRING			"ProgramFunc"
#define	DELTA_LIBRARYFUNC_TYPESTRING			"LibraryFunc"
#define	DELTA_BOOL_TYPESTRING					"Bool"
#define	DELTA_EXTERNID_TYPESTRING				"ExternId"
#define	DELTA_NIL_TYPESTRING					"Nil"
#define	DELTA_METHODFUNC_TYPESTRING				"MethodFunc"
#define	DELTA_UNDEFINED_TYPESTRING				"Undefined"

//-----------------------------------------------------------
// Error tags for runtime error exceptions.
//

#define	DELTA_GENERAL_RUNTIME_ERROR						"DeltaGeneralRuntimeError"
#define	DELTA_ARITH_OPERATOR_ERROR						"DeltaArithmeticOperatorError"
#define	DELTA_RELAT_OPERATOR_ERROR						"DeltaRelationalOperatorError"
#define	DELTA_NOT_CALLABLE_ERROR						"DeltaNotCallableError"
#define	DELTA_FORMAL_ARG_WITHOUT_ACTUAL_VALUE_ERROR		"DeltaFormalArgWithoutActualValueError"
#define	DELTA_UNRESOLVED_LIBFUNC_ERROR					"DeltaUnresolvedLibraryFuncError"
#define	DELTA_LIBFUNC_NATIVE_EXCEPTION_ERROR			"DeltaLibraryFuncNativeExceptionError"
#define	DELTA_UNRESOLVED_GLOBALFUNC_ERROR				"DeltaUnresolvedGlobalFuncError"
#define	DELTA_SUICIDAL_VM_ERROR							"DeltaVirtualMachineSelfDestructionError"
#define	DELTA_ZOMBI_VM_ERROR							"DeltaVirtualMachineCallerDestructionError"
#define	DELTA_INVALID_SUPPLIED_CLOSURE_ERROR			"DeltaInvalidSuppliedClosureError"
#define	DELTA_NO_SUPPLIED_CLOSURE_ERROR					"DeltaNoSuppliedClosureError"
#define	DELTA_FUNCTOR_SLOT_MISSING_ERROR				"DeltaFunctorSlotMissingError"
#define	DELTA_STACK_OVERFLOW_ERROR						"DeltaStackOverflowError"
#define	DELTA_FOREACH_MISSING_METHODS_ERROR				"DeltaForeachMissingMethodsError"
#define	DELTA_FOREACH_INVALID_ITERATOR_ERROR			"DeltaForeachInvalidIteratorError"
#define	DELTA_FOREACH_INVALID_CONTAINER_ERROR			"DeltaForeachInvalidContainerError"
#define	DELTA_VM_NOT_RUN_ERROR							"DeltaVirtualMachineNotRunError"
#define	DELTA_FUNCTOR_OVERLOADING_DISABLED_ERROR		"DeltaFunctorOverloadingDisabledError"
#define	DELTA_OPERATOR_OVERLOADING_DISABLED_ERROR		"DeltaOperatorOverloadingDisabledError"
#define	DELTA_FUNCTOR_CYCLIC_REFERENCE_ERROR			"DeltaFunctorCyclicReferenceError"
#define	DELTA_OPERATOR_OVERLOADING_ERROR				"DeltaOperatorOverloadingError"
#define	DELTA_OPERATOR_SLOT_NOT_CALLABLE_ERROR			"DeltaOperatorSlotNotCallableError"
#define	DELTA_ILL_FORMED_TABLE_IN_DYNAMIC_ARGS_ERROR	"DeltaIllFormedTableInDynamicArgsError"
#define	DELTA_INVALID_VALUE_IN_DYNAMIC_ARGS_ERROR		"DeltaInvalidValueInDynamicArgsError"
#define	DELTA_EXTERNID_MISSING_USERDATA_ERROR			"DeltaExternIdMissingUserDataError"
#define	DELTA_SET_METHOD_SELF_INVALID_VALUE_ERROR		"DeltaSetMethodSelfInvalidValueError"
#define	DELTA_INVALID_METHOD_VALUE_INDEXING				"DeltaInvalidMethodValueIndexingError"
#define	DELTA_SET_METHOD_SELF_NEEDS_CONST_INDEX_ERROR	"DeltaSetMethodSelfNeedsConstIndexError"
#define	DELTA_INDEXING_INVALID_VALUE_ERROR				"DeltaIndexingInvalidValueError"
#define	DELTA_OBJECT_ATTRIBUTE_OPERATION_ERROR			"DeltaObjectAttributeOperationError"
#define	DELTA_USER_RUNTIME_ERROR						"DeltaUserRuntimeError"

//-----------------------------------------------------------
// The offsets for the various items saved on the stack, once an activation
// record has been pushed entirely. Those are needed for the return sequence.
// The activation record structrue follows:
// |ArgN|...|Arg1|Arguments|[self]|#Agruments|PC|top|topsp|Local vars|
//

#define	DELTA_TOP_TOPSP_OVERFLOW_VALUE			0
#define	DELTA_SAVED_TOPSP_OFFSET				+1
#define	DELTA_SAVED_TOP_OFFSET					+2
#define	DELTA_SAVED_PC_OFFSET					+3
#define	DELTA_SAVED_NUMACTUALS_OFFSET			+4
#define	DELTA_PREVIOUS_TOP_EXTRA_OFFSET			+2

///////////////////////////////////////////////////////////////////
// NATIVE LIBRARIES.

#define	DELTA_LIBRARYNAMESPACE_SEPARATOR		"::"
#define	DELTA_LIBRARYNAMESPACE_COMMENT			"#"
#define	DELTA_LIBRARYNAMESPACE_ENTER			"@enter"
#define	DELTA_LIBRARYNAMESPACE_EXIT				"@exit"
#define	DELTA_LIBRARYNAMESPACE_TYPE				"@type"
#define	DELTA_LIBRARYNAMESPACE_CONST			"@const"

#define	DELTA_LIBRARYNAMESPACE_AUTOCOMP_LIBNAME	"#libname"	// Always the first token in a user library
#define	DELTA_LIBRARYNAMESPACE_AUTOCOMP_GROUP	"#group"
#define	DELTA_LIBRARYNAMESPACE_AUTOCOMP_GROUPEX	"#groupex"
#define	DELTA_LIBRARYNAMESPACE_AUTOCOMP_NOGROUP	"_"

#define	DELTA_ENVIRONMENT_VAR_BYTECODE_PATH		"DELTA_BYTECODE_PATH"
#define	DELTA_ENVIRONMENT_VAR_DLL_PATH			"DELTA_DLL_PATH"

#define	DELTA_DEPENDENCY_NOT_FOUND				"not_found"
#define	DELTA_DEPENDENCY_ONE_FOUND				"one_found"
#define	DELTA_DEPENDENCY_MANY_FOUND				"many_found"

///////////////////////////////////////////////////////////////////
// DEBUGGER.
// Those macros will be used by the debugger interface to check the return
// value of strings which are supposed to represent a name or a value.
//
#define	DBG_NO_INFO								"_<NODBGINFO>_"			// When returned on requests means no debug info was included.
#define	DBG_ID_NOTFOUND							"_<NOID>_"				// When returned on requests means the item required is invalid.
#define	DBG_ASSSIGN_ERROR						"_<ERRASSIGN>_"			// When returned on assignments as the new value, means error expr and value not changed.
#define	DBG_DYNAMIC_SOURCE						"_<DYNAMICSOURCE>_"		// When the source file is dynamic.
#define	DBG_GRAPH_START_NODE					"_<STARTNODE>_"			// The first node when requesting an object graph
#define	DBG_GRAPH_START_NODE_ABSREF				"_<STARTNODEABSREF>_"	// Slot key to find the absref in the start node of the graph
#define	DBG_GETVARIABLES_NAME_IS_GROUP			"_<VARGROUP>_"

#define	DBG_ANY_BLOCK							0xFFFF					// When sending a request for a variable's block id
#define	DBG_LINEVALUE_IS_INVALID_BPT			0
#define	DBG_LINEVALUE_IS_CANTCHECK_BPT			0xffffffff
#define	DBG_GLOBAL_CODE_DESCRIPTION				"global code"
#define	DBG_NO_LINE_INFO						0
#define	DBG_GETVARIABLES_VAR_FORMAT				"%s:%u"					// <name>:<line>, <name>: ident | arguments[%u] (see below)
#define	DBG_GETVARIABLES_EXTRA_ACTUAL_FORMAT	DELTA_ARGUMENTS_POINTER_ID "[%u]"
#define	DBG_NO_ACTUAL_VALUE_FOR_FORMAL			"_<NO ACTUAL VALUE>_"

//-----------------------------------------------------------
// VM and debugger backend exporting.

#if defined(DVM_EXPORT) && defined(DVM_IMPORT)
#error "Can't define both DVM_EXPORT and DVM_IMPORT!"
#endif

#if	defined(DVM_EXPORT)
#define	DVM_CLASS	UDLL_EXPORT_CPP_CLASS
#define	DVM_FUNC	UDLL_EXPORT_CPP
#define	DVM_CFUNC	UDLL_EXPORT_C
#elif defined(DVM_IMPORT)
#define	DVM_CLASS	UDLL_IMPORT_CPP_CLASS
#define	DVM_FUNC	UDLL_IMPORT_CPP
#define	DVM_CFUNC	UDLL_IMPORT_C
#else
#define	DVM_CLASS
#define	DVM_FUNC extern
#define	DVM_CFUNC extern
#endif

#if defined(DVMDEBUG_EXPORT) && defined(DVMDEBUG_IMPORT)
#error "Can't define both DVMDEBUG_EXPORT and DVMDEBUG_IMPORT!"
#endif

#if	defined(DVMDEBUG_EXPORT)
#define	DVMDEBUG_CLASS	UDLL_EXPORT_CPP_CLASS
#define	DVMDEBUG_FUNC	UDLL_EXPORT_CPP
#define	DVMDEBUG_CFUNC	UDLL_EXPORT_C
#elif defined(DVMDEBUG_IMPORT)
#define	DVMDEBUG_CLASS	UDLL_IMPORT_CPP_CLASS
#define	DVMDEBUG_FUNC	UDLL_IMPORT_CPP
#define	DVMDEBUG_CFUNC	UDLL_IMPORT_C
#else
#define	DVMDEBUG_CLASS
#define	DVMDEBUG_FUNC extern
#define	DVMDEBUG_CFUNC extern
#endif

//-----------------------------------------------------------
// Bytecode structure exporting.

#if defined(DBYTECODE_EXPORT) && defined(DBYTECODE_IMPORT)
#error "Can't define both DBYTECODE_EXPORT and DBYTECODE_IMPORT!"
#endif

#if	defined(DBYTECODE_EXPORT)
#define	DBYTECODE_CLASS	UDLL_EXPORT_CPP_CLASS
#define	DBYTECODE_FUNC	UDLL_EXPORT_CPP
#define	DBYTECODE_CFUNC	UDLL_EXPORT_C
#elif defined(DBYTECODE_IMPORT)
#define	DBYTECODE_CLASS	UDLL_IMPORT_CPP_CLASS
#define	DBYTECODE_FUNC	UDLL_IMPORT_CPP
#define	DBYTECODE_CFUNC	UDLL_IMPORT_C
#else
#define	DBYTECODE_CLASS
#define	DBYTECODE_FUNC extern
#define	DBYTECODE_CFUNC extern
#endif

//-----------------------------------------------------------

#endif	// Do not ad stuff beyond this point.

