// MiscLib.cpp.
// Standard library functions for the DeltaVirtualMachine.
// ScriptFighter Project.
// A. Savidis, November 1999.
// Refactored December 2007, encompassed in the 'std' API object. 
// Yannis Lilis added dllimport, dllinvoke (July 2008), slight changes by A. Savidis.
// Added similar support for UNIX variants, June 2009.
// Refactored, June 2009, moved dynamic library loading to Util.
//

#include "DeltaLibraryCreators.h"
#include <stdio.h>
#include "MiscLib.h"
#include "VMRegLib.h"
#include "usystem.h"
#include "RcLoaderAPI.h"
#include "RcLoaderErrorMsg.h"
#include "DeltaStdLib.h"
#include "DeltaStdDefs.h"
#include "OperatorOverloading.h"
#include "ListLib.h"
#include "ufiles.h"
#include "usystem.h"
#include "ustrings.h"

//------------------------------------------------------------------

static void print_LibFunc (DeltaVirtualMachine*);
static void callglobal_LibFunc (DeltaVirtualMachine* vm);
static void typeof_LibFunc (DeltaVirtualMachine* vm);
static void iscallable_LibFunc (DeltaVirtualMachine* vm);
static void equal_LibFunc (DeltaVirtualMachine* vm);
static void tostring_LibFunc (DeltaVirtualMachine* vm);
static void isundefined_LibFunc (DeltaVirtualMachine* vm);
static void isnil_LibFunc (DeltaVirtualMachine* vm);
static void undefined_LibFunc (DeltaVirtualMachine* vm);
static void externidtype_LibFunc (DeltaVirtualMachine* vm);
static void externiduserdata_LibFunc (DeltaVirtualMachine* vm);
static void taggedvalue_LibFunc (DeltaVirtualMachine* vm);
static void getvaluetag_LibFunc (DeltaVirtualMachine* vm);
static void iserrortag_LibFunc (DeltaVirtualMachine* vm);

static void isoverloadableoperator_LibFunc (DeltaVirtualMachine* vm);
static void currenttime_LibFunc (DeltaVirtualMachine* vm);
static void getenvironmentvar_LibFunc (DeltaVirtualMachine* vm);
static void error_LibFunc (DeltaVirtualMachine* vm);

static void setarithmeticoperator_LibFunc  (DeltaVirtualMachine* vm);
static void setrelationaloperator_LibFunc  (DeltaVirtualMachine* vm);
static void setassignmentoperator_LibFunc  (DeltaVirtualMachine* vm);
static void settypecastingoperator_LibFunc  (DeltaVirtualMachine* vm);
static void getarithmeticoperator_LibFunc  (DeltaVirtualMachine* vm);
static void getrelationaloperator_LibFunc  (DeltaVirtualMachine* vm);
static void getassignmentoperator_LibFunc  (DeltaVirtualMachine* vm);
static void gettypecastingoperator_LibFunc  (DeltaVirtualMachine* vm);

static void libfuncget_LibFunc (DeltaVirtualMachine* vm);
static void libfuncisbound_LibFunc (DeltaVirtualMachine* vm);
static void libfuncbind_LibFunc (DeltaVirtualMachine* vm);
static void libfuncunbound_LibFunc (DeltaVirtualMachine* vm);
static void libfuncgetboundargs_LibFunc (DeltaVirtualMachine* vm);

static void rcload_LibFunc (DeltaVirtualMachine* vm);
static void rcsetpreprocessor_LibFunc (DeltaVirtualMachine* vm);
static void rcparse_LibFunc (DeltaVirtualMachine* vm);
static void rcloadgeterror_LibFunc (DeltaVirtualMachine* vm);
static void rcstore_LibFunc (DeltaVirtualMachine* vm);

static void dllimport_LibFunc (DeltaVirtualMachine* vm);
static void dllimportdeltalib_LibFunc (DeltaVirtualMachine* vm);
static void dllimportaddpath_LibFunc (DeltaVirtualMachine* vm);
static void dllinvoke_LibFunc (DeltaVirtualMachine* vm);
static void dllhasfunction_LibFunc (DeltaVirtualMachine* vm);
static void dllunimport_LibFunc (DeltaVirtualMachine* vm);
static void dllunimportdeltalib_LibFunc (DeltaVirtualMachine* vm);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {

	{ "print",					print_LibFunc					},
	{ "callglobal",				callglobal_LibFunc				},
	{ "typeof",					typeof_LibFunc					},
	{ "iscallable",				iscallable_LibFunc				},
	{ "equal",					equal_LibFunc					},
	{ "tostring",				tostring_LibFunc				},
	{ "isundefined",			isundefined_LibFunc 			},
	{ "isnil",					isnil_LibFunc 					},
	{ "undefined",				undefined_LibFunc				},
	{ "externidtype",			externidtype_LibFunc			},
	{ "externiduserdata",		externiduserdata_LibFunc		},

	{ "taggedvalue",			taggedvalue_LibFunc 			},
	{ "getvaluetag",			getvaluetag_LibFunc 			},
	{ "iserrortag",				iserrortag_LibFunc 				},

	{ "isoverloadableoperator", isoverloadableoperator_LibFunc	},
	{ "currenttime",			currenttime_LibFunc				},
	{ "getenvironmentvar",		getenvironmentvar_LibFunc		},
	{ "error",					error_LibFunc					},

	{ "setarithmeticoperator",	setarithmeticoperator_LibFunc	},  
	{ "setrelationaloperator",	setrelationaloperator_LibFunc	},  
	{ "setassignmentoperator",	setassignmentoperator_LibFunc	},  
	{ "settypecastingoperator", settypecastingoperator_LibFunc	}, 
	{ "getarithmeticoperator",	getarithmeticoperator_LibFunc	},  
	{ "getrelationaloperator",	getrelationaloperator_LibFunc	},  
	{ "getassignmentoperator",	getassignmentoperator_LibFunc	},  
	{ "gettypecastingoperator", gettypecastingoperator_LibFunc	}, 

	{ "libfuncget",				libfuncget_LibFunc				},
	{ "libfuncisbound",			libfuncisbound_LibFunc			},
	{ "libfuncbind",			libfuncbind_LibFunc				},
	{ "libfuncunbound",			libfuncunbound_LibFunc			},
	{ "libfuncgetboundargs",	libfuncgetboundargs_LibFunc		},

	{ "rcload",					rcload_LibFunc					},
	{ "rcsetpreprocessor",		rcsetpreprocessor_LibFunc		},
	{ "rcparse",				rcparse_LibFunc					},
	{ "rcloadgeterror",			rcloadgeterror_LibFunc			},
	{ "rcstore",				rcstore_LibFunc					},

	{ "dllimport",				dllimport_LibFunc				},
	{ "dllimportdeltalib",		dllimportdeltalib_LibFunc		},
	{ "dllinvoke",				dllinvoke_LibFunc				},
	{ "dllhasfunction",			dllhasfunction_LibFunc			},
	{ "dllunimport",			dllunimport_LibFunc				},
	{ "dllunimportdeltalib",	dllunimportdeltalib_LibFunc		},
	{ "dllimportaddpath",		dllimportaddpath_LibFunc		}
};

///////////////////////////////////////////////

static const char* errorTags[] = {
	DELTA_GENERAL_RUNTIME_ERROR,					
	DELTA_ARITH_OPERATOR_ERROR,					
	DELTA_RELAT_OPERATOR_ERROR,					
	DELTA_NOT_CALLABLE_ERROR,					
	DELTA_FORMAL_ARG_WITHOUT_ACTUAL_VALUE_ERROR,	
	DELTA_UNRESOLVED_LIBFUNC_ERROR,				
	DELTA_LIBFUNC_NATIVE_EXCEPTION_ERROR,		
	DELTA_UNRESOLVED_GLOBALFUNC_ERROR,			
	DELTA_SUICIDAL_VM_ERROR,						
	DELTA_ZOMBI_VM_ERROR,
	DELTA_INVALID_SUPPLIED_CLOSURE_ERROR,
	DELTA_NO_SUPPLIED_CLOSURE_ERROR,		
	DELTA_FUNCTOR_SLOT_MISSING_ERROR,
	DELTA_STACK_OVERFLOW_ERROR,
	DELTA_FOREACH_MISSING_METHODS_ERROR,
	DELTA_FOREACH_INVALID_ITERATOR_ERROR,
	DELTA_FOREACH_INVALID_CONTAINER_ERROR,
	DELTA_VM_NOT_RUN_ERROR,
	DELTA_FUNCTOR_OVERLOADING_DISABLED_ERROR,
	DELTA_FUNCTOR_CYCLIC_REFERENCE_ERROR,
	DELTA_OPERATOR_OVERLOADING_DISABLED_ERROR,
	DELTA_OPERATOR_OVERLOADING_ERROR,
	DELTA_OPERATOR_SLOT_NOT_CALLABLE_ERROR,
	DELTA_ILL_FORMED_TABLE_IN_DYNAMIC_ARGS_ERROR,
	DELTA_INVALID_VALUE_IN_DYNAMIC_ARGS_ERROR,
	DELTA_EXTERNID_MISSING_USERDATA_ERROR,
	DELTA_SET_METHOD_SELF_INVALID_VALUE_ERROR,
	DELTA_INVALID_METHOD_VALUE_INDEXING,
	DELTA_SET_METHOD_SELF_NEEDS_CONST_INDEX_ERROR,
	DELTA_INDEXING_INVALID_VALUE_ERROR,
	DELTA_OBJECT_ATTRIBUTE_OPERATION_ERROR,
	DELTA_USER_RUNTIME_ERROR
};

#define	TOTAL_ERROR_TAGS uarraysize(errorTags)

///////////////////////////////////////////////

DVM_FUNC bool DeltaExternId_GetType (void* val, DeltaValue* at) { 
	DeltaExternId* externId = DELTA_EXTERNID_VGET_INST(val);
	DASSERT(externId);
	at->FromString(DPTR(externId)->GetTypeStr()); 
	return true; 
}

DVM_FUNC bool DeltaExternId_GetUserData (void* val, DeltaValue* at) {
	DeltaExternId* externId = DELTA_EXTERNID_VGET_INST(val);
	DASSERT(externId);
	at->Assign(DPTR(externId)->GetUserData()); 
	return true; 
}

///////////////////////////////////////////////

VCLASSID_DEFINE(DLL_CID, DLL_TYPE_STR)
typedef ValidatableValue<udynamiclibloader, DLL_CID> ValidatableDll;

static void DestroyValidatableDll (const std::pair<ValidatableDll*, bool>& p) {
	DDELETE(DPTR(p.first)->GetValue());
	DDELETE(p.first);
}

typedef std::pair<ValidatableDll*, util_ui32>	DllEntry;
typedef std::map<std::string, DllEntry>			DllPathMap;
typedef ubag<ValidatableDll*>					DllMap;

static DllMap*			allDlls				= (DllMap*) 0; 
static DllPathMap*		dllPaths			= (DllPathMap*) 0; 
static upathmanager*	pathManager			= (upathmanager*) 0;

static void AddEnvironmentLoadingPaths (void);

static void Initialise (void)	{
	unew(allDlls);
	unew(dllPaths);
	unew(pathManager);
	AddEnvironmentLoadingPaths();
}

static void CleanUp (void) {
	std::for_each(
		DPTR(allDlls)->begin(),
		DPTR(allDlls)->end(),
		&DestroyValidatableDll
	);
	udelete(dllPaths);
	udelete(allDlls); 
	udelete(pathManager);
}

DELTA_LIBRARY_SUBAPI_INSTALLER_EX(DeltaMisc, "misc", DELTA_STDLIB_NAMESPACE_PREFIX, Initialise(), CleanUp())

//------------------------------------------------------------------

void DeltaAdd_dllimport_Path (const std::string& path, bool prioritised) 
	{ DPTR(pathManager)->add(path, prioritised); }

static void AddEnvironmentLoadingPaths (void) 
	{ DPTR(pathManager)->add_environment_path(DELTA_ENVIRONMENT_VAR_DLL_PATH, false); }

static const std::string MakeLoadingPath (void) 
	{ return DPTR(pathManager)->make_entire_path(); }

#define	LOADING_RESOLVE_DLL_FILE(_full_path, _file, _reset)									\
	std::string _full_path = DPTR(pathManager)->resolve(_file);								\
	if (_full_path.empty()) {																\
		DPTR(vm)->PrimaryError(																\
			"in %s dll file '%s' was not found in cwd and '%s' dll loading path!",			\
			CURR_FUNC,																		\
			_file.c_str(),																	\
			MakeLoadingPath().c_str()														\
		);																					\
		_reset;																				\
	} else

//------------------------------------------------------------------
// Local variables declared are: dllVal, dllValTypeStr, dll.
//
#define	GET_DLL(argno, retvalreset, func)										\
																				\
	DeltaValue* argObj;															\
	GET_EXTERNID(dllVal, func, retvalreset, argno);								\
	if (DLL_TYPE_STR != dllValTypeStr) {										\
		DeltaErrorExternId(argno, DLL_TYPE_STR, dllValTypeStr.c_str(), func);	\
		retvalreset;															\
	}																			\
	ValidatableDll* vdll = VGET_VALUE(udynamiclibloader, DLL_CID, dllVal);		\
	if (!vdll) {																\
		DPTR(vm)->PrimaryError(													\
			"invalid dll handle 0x%x, probably closed!",						\
			(util_ui32) dllVal													\
		);																		\
		retvalreset;															\
	}																			\
	udynamiclibloader* dll = vdll->GetValue();									\
	DASSERT(allDlls);															\
	DllMap::iterator dllIter = DPTR(allDlls)->find(vdll);						\
	DllPathMap::iterator pathIter = DPTR(dllPaths)->find(DPTR(dll)->GetPath());	\
	DASSERT(dllIter != DPTR(allDlls)->end());									\
	DASSERT(pathIter != DPTR(dllPaths)->end());

//------------------------------------------------------------------
// udynamiclibloader class to be used for dll libfuncs.
//

static void Dll2String (DeltaString* at, void* val) {
	
	ValidatableDll* vdll = VGET_VALUE(udynamiclibloader, DLL_CID, val);

	if (!vdll)
		DPTR(at)->Add(
			uconstructstr("DLL(handle 0x%x, closed!)", (util_ui32) vdll)
		);
	else {
		DllMap::iterator dllIter = DPTR(allDlls)->find(vdll);
		DASSERT(dllIter != DPTR(allDlls)->end());
		DPTR(at)->Add(
			uconstructstr(
				"DLL(0x%x)[path '%s']", 
				DPTR(vdll)->GetSerialNo(),
				DPTR(vdll)->GetValue()->GetPath().c_str()
			)
		);
	}
}

//------------------------------------------------------------------

static void console_OutputFunc (const char* msg) 
	{ fprintf(stdout,"%s", msg); }

static void (*print_OutputFunc) (const char*) = &console_OutputFunc;

void DeltaSet_print_OutputFunc (void (*outputFunc) (const char*)) {
	if (outputFunc) 
		print_OutputFunc = outputFunc; 
	else 
		print_OutputFunc = console_OutputFunc;
}

///////////////////////////////////////////////
// print(x1,...,xn).
// Prints on stdout. Used with console I/O.
//
static void print_LibFunc (DeltaVirtualMachine* vm) {

	// Get actual args. Then, print each actual arg
	// one-by-one.
	//
	for (util_ui16 n = DPTR(vm)->TotalActualArgs(), i = 0; i < n; i++) {
		DeltaValue* arg = DPTR(vm)->GetActualArg(i);
		DASSERT(arg);
		(*print_OutputFunc)(arg->ConvertToString().c_str());

		if (DPTR(vm)->HasProducedError() || UERROR_ISRAISED()) {
			DPTR(vm)->PossiblePrimaryError(
				"'print' failed after argument #%d of type '%s'",
				i,
				arg->TypeStr()
			);
			return;
		}
	}
}

//------------------------------------------------------------------
// callglobal(f,arg1,...,argN).
// Calls a user defined global function. Its name
// should be the first argument, while its arguments
// should follow.
//
static void callglobal_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("callglobal");
	DeltaAtLeastTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DeltaValue* argObj;
	GET_STRING(funcName, CURR_FUNC, RESET_NIL_RETURNVALUE, 0);

	if (!DPTR(vm)->GlobalFuncExists(funcName.c_str())) {
		DPTR(vm)->PrimaryError(
			"In 'callglobal', func '%s' not found",
			funcName.c_str()
		);
		RESET_NIL_RETURNVALUE;
	}

	// Then, push the actual arguments one by
	// one on the stack in the reverse order.
	//
	for (util_ui16 argNo = DPTR(vm)->TotalActualArgs() - 1; argNo; --argNo)
		DPTR(vm)->PushActualArg(
			DPTR(vm)->GetActualArg(argNo)
		);

	DPTR(vm)->ExtCallGlobalFunc(	// Call by name. Return value set implicitly.
		funcName.c_str()
	);
}

//------------------------------------------------------------------
// typeof(x).
// Returns the type string of the argument object.
//
static void typeof_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("typeof");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DPTR(vm)->GetReturnValue().FromString(
		DPTR(vm)->GetActualArg(0)->TypeStr()
	);
}

//------------------------------------------------------------------
// Bool iscallable(x).
// Returns the true if callable.
//
static void iscallable_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("iscallable");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_FALSE_RETURNVALUE);

	DPTR(vm)->GetReturnValue().FromBool(
		DPTR(vm)->GetActualArg(0)->IsCallable()
	);
}

//------------------------------------------------------------------
// Bool isundefined(x).
// Returns true if the type is still undefined (i.e.
// not assigned to something initialised).
//
static void isundefined_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("isundefined");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_FALSE_RETURNVALUE);

	DPTR(vm)->GetReturnValue().FromBool(
		DPTR(vm)->GetActualArg(0)->Type() ==  DeltaValue_Undefined
	);
}

//------------------------------------------------------------------
// Bool isnil(x).
// Returns true if the type is nil.
//
static void isnil_LibFunc (DeltaVirtualMachine* vm) {
	ISSUE_FUNC("isnil");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_FALSE_RETURNVALUE);
	DPTR(vm)->GetReturnValue().FromBool(DPTR(vm)->GetActualArg(0)->IsNil());
}

//------------------------------------------------------------------
// Undef undefined().
// Returns an undefined value.
//
static void undefined_LibFunc (DeltaVirtualMachine* vm) {
	ISSUE_FUNC("undefined");
	DeltaTotalArgsCheck(0, CURR_FUNC, RESET_UNDEFINED_RETURNVALUE);
	DPTR(vm)->GetReturnValue().Undefine();
}

//------------------------------------------------------------------
// Bool equal(x,y).
// Returns native equality test (not overloaded for tables).
//
static void equal_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("equal");
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_FALSE_RETURNVALUE);

	DPTR(vm)->GetReturnValue().FromBool(
		DPTR(vm)->GetActualArg(0)->Equal(
			DPTR(vm)->GetActualArg(1)
		)
	);
}

//------------------------------------------------------------------
// String externidtype(ExternId(x)).
// Returns the specific type of an extern id.
//
static void externidtype_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("externidtype");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DeltaValue* argObj;
	GET_EXTERNID(val, CURR_FUNC, RESET_NIL_RETURNVALUE, 0);

	DPTR(vm)->GetReturnValue().FromString(valTypeStr);
}

//------------------------------------------------------------------
// Object externiduserdata(ExternId(x)).
// Returns the userdata of an extern id.
//
static void externiduserdata_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("externiduserdata");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DeltaValue* argObj;
	GET_EXTERNID(val, CURR_FUNC, RESET_NIL_RETURNVALUE, 0);
	GET_USERDATA(userData);
	
	if (!userData)
		DPTR(vm)->GetReturnValue().FromNil();
	else
		DPTR(vm)->GetReturnValue().FromTable(userData);
}

//------------------------------------------------------------------
// bool isoverloadableoperator(x).
// Returns true if a string representing an overloadable operator.
//
static void isoverloadableoperator_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("isoverloadableoperator");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_FALSE_RETURNVALUE);

	DeltaValue* arg = DPTR(vm)->GetActualArg(0);
	DPTR(vm)->GetReturnValue().FromBool(
		arg->Type() == DeltaValue_String && DeltaTable::IsOverloadableOperator(arg->ToString())
	);
}

//------------------------------------------------------------------
// Number curenttime().
// Returns the current time value in milliseconds.
//
static void currenttime_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("currenttime");
	DeltaTotalArgsCheck(0, CURR_FUNC, RESET_ZERO_RETURNVALUE);

	DPTR(vm)->GetReturnValue().FromNumber((DeltaNumberValueType) ugettime());
}

//------------------------------------------------------------------

DLIB_FUNC_START(getenvironmentvar, 1, Nil)
DLIB_ARG(const char*, id)
std::string val = ugetenvironmentvar(DLIB_ARGVAL(id));
if (val.empty())
	DLIB_RETVAL_REF.FromNil();
else
	DLIB_RETVAL_REF.FromString(val);
DLIB_FUNC_END

//------------------------------------------------------------------

static void error_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("error");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_EMPTY);

	DeltaValue* argObj;
	GET_STRING(msg, CURR_FUNC, RESET_EMPTY, 0);
	DPTR(vm)->SetErrorCode(DELTA_USER_RUNTIME_ERROR)->PrimaryError("%s", msg.c_str());
}

//------------------------------------------------------------------
// Result libfuncget(String(name)).
//
static void libfuncget_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("libfuncget");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DeltaValue* argObj;
	GET_STRING(name, CURR_FUNC, RESET_EMPTY, 0);
	DeltaLibraryFunc func = DeltaLibFuncBinder::Get(name.c_str());
	if (!func)
		DPTR(vm)->GetReturnValue().FromNil();
	else
		DPTR(vm)->GetReturnValue().FromLibraryFunc(func);
}

//------------------------------------------------------------------
// Bool libfuncisbound(Library function (f)).
//
static void libfuncisbound_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("libfuncisbound");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_FALSE_RETURNVALUE);

	DeltaValue* argObj;
	GET_LIBFUNC(func, CURR_FUNC, RESET_FALSE_RETURNVALUE, 0);

	DPTR(vm)->GetReturnValue().FromBool(!!funcBinder);
}

//------------------------------------------------------------------
// LibraryFunc libfuncbind(Library function (f), ...).
//
static void libfuncbind_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("libfuncbind");
	DeltaAtLeastTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DeltaValue* argObj;
	GET_LIBFUNC(func, CURR_FUNC, RESET_NIL_RETURNVALUE, 0);

	util_ui16 n = DPTR(vm)->TotalActualArgs() - 1;

	if (!n && !funcBinder)		// No bound arguments and the lib func was not also bound?
		DPTR(vm)->GetReturnValue().FromLibraryFunc(func);	// Then a simple lib func value.
	else {
		DeltaLibraryFuncArgsBinder* binder = DeltaLibraryFuncArgsBinder::New(funcBinder);
		for (util_ui16 i = 1; i <= n; i++)
			DPTR(binder)->BindArg(*DPTR(vm)->GetActualArg(i));
		DPTR(vm)->GetReturnValue().FromLibraryFunc(func, binder);
	}
}

//------------------------------------------------------------------
// LibraryFunc libfuncunbound(LibraryFunc(f)).

static void libfuncunbound_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("libfuncunbound");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DeltaValue* argObj;
	GET_LIBFUNC(func, CURR_FUNC, RESET_NIL_RETURNVALUE, 0);
	DPTR(vm)->GetReturnValue().FromLibraryFunc(func);
}

//------------------------------------------------------------------
// List / Nil libfuncgetboundargs(LibraryFunc(f)).

static void libfuncgetboundargs_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("libfuncgetboundargs");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DeltaValue* argObj;
	GET_LIBFUNC(func, CURR_FUNC, RESET_NIL_RETURNVALUE, 0);

	if (DeltaLibraryFuncArgsBinder* binder = argObj->GetLibraryFuncArgsBinder())
		DPTR(vm)->GetReturnValue().Assign(binder->GetListValue());
	else
		DPTR(vm)->GetReturnValue().FromNil();
}

//------------------------------------------------------------------
// String tostring(x).
//
static void tostring_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("tostring");

	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DPTR(vm)->GetReturnValue().FromString(
		DPTR(vm)->GetActualArg(0)->ConvertToString()
	);
}

//------------------------------------------------------------------

static void rc_create_handler (
		const char*				func, 
		DeltaVirtualMachine*	vm, 
		DeltaTable* (*loader)(const std::string&)
	) {

	ISSUE_FUNC(func);
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);
	DeltaValue* argObj;
	GET_STRING(str, CURR_FUNC, RESET_ZERO_RETURNVALUE, 0);

	DeltaTable* r = (*loader)(str.c_str());
	if (r) {
		DASSERT(!ResourceLoaderErrorsExist());
		DPTR(vm)->GetReturnValue().FromTable(r);
		DPTR(r)->DecRefCounter((DeltaValue*) 0);	// It was increased by default.
	}
	else	
		DPTR(vm)->GetReturnValue().FromNil();
}

//------------------------------------------------------------------
// Table rcparse(string buffer).
// Parses resources from a string buffer. Resources are returned
// within a dynamically created table.

void rcparse_LibFunc (DeltaVirtualMachine* vm) 
	{ rc_create_handler("rcparse", vm, &ResourceLoader::LoadFromString); }

//------------------------------------------------------------------
// Table rcload(path).
// Load resources from path.

void rcload_LibFunc (DeltaVirtualMachine* vm)
	{ rc_create_handler("rcload", vm, &ResourceLoader::Load); }

//------------------------------------------------------------------
// String rcloadgeterror().
//
void rcloadgeterror_LibFunc (DeltaVirtualMachine* vm) {
	ISSUE_FUNC("rcloadgeterror");
	DeltaTotalArgsCheck(0, CURR_FUNC, RESET_NIL_RETURNVALUE);
	DPTR(vm)->GetReturnValue().FromString(ResourceLoaderGetError());
}

DLIB_FUNC_START(rcsetpreprocessor, 2, Empty)
DLIB_ARG(const char*,	cpp)
DLIB_ARG(const char*,	include)
ResourceLoader::SetPreprocessor(DLIB_ARGVAL(cpp), DLIB_ARGVAL(include));
DLIB_FUNC_END

//------------------------------------------------------------------
// Bool rcstore(t, path).
// Store resources to a file. Resources are stored in a table.
//
void rcstore_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("rcstore");
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_FALSE_RETURNVALUE);
	DeltaValue* argObj;
	GET_TABLE(rc, CURR_FUNC, RESET_ZERO_RETURNVALUE, 0);
	GET_STRING(path, CURR_FUNC, RESET_ZERO_RETURNVALUE, 1);

	DPTR(vm)->GetReturnValue().FromBool(
		ResourceLoader::Write(path.c_str(), rc)
	);
}

//------------------------------------------------------------------

static void dllimport_get_func (
		DeltaVirtualMachine*	vm, 
		std::string&			at, 
		const char*				curr_func, 
		bool					is_delta, 
		bool*					result
	) {

	ISSUE_FUNC(curr_func);
	if (is_delta) 
		{ at = DELTA_DLLIMPORT_INSTALL_NAME; *result = true; }
	else {
		DeltaValue* argObj;
		GET_STRING(func, CURR_FUNC, RESET_NIL_RETURNVALUE, 1);
		at = func;
		*result = true;
	}
}

///////////////////////////////////////////////////////////////////////

#define	IF_DELTA_DLL_ERROR_ELSE(format, dll_arg)													\
	if (DPTR(vm)->HasProducedError()) /* The DLL may load and execute scripts causing errors. */		\
		return;																						\
	else																							\
	if (UERROR_ISRAISED())	/* But not all errors are caused by scripts (thus known by vms).*/		\
		DPTR(vm)->PrimaryError(																		\
			"in '%s(" format ",...) func '%s' caused some errors!",									\
			CURR_FUNC,																				\
			dll_arg,																				\
			func.c_str()																			\
		);																							\
	else

static void dllimport_handler (DeltaVirtualMachine* vm, const char* funcName, bool is_delta) {

	ISSUE_FUNC(funcName);
	DeltaTotalArgsCheck(is_delta ? 1 : 2, CURR_FUNC, RESET_NIL_RETURNVALUE);
	DeltaValue* argObj;
	GET_STRING(path, CURR_FUNC, RESET_NIL_RETURNVALUE, 0);

#ifndef NDEBUG
	path = path.substr(0, path.find_last_of('.')) + "D" + path.substr(path.find_last_of('.'));
#endif

	LOADING_RESOLVE_DLL_FILE(fullPath, path, RESET_NIL_RETURNVALUE);

	std::string func;
	bool ok = false;
	dllimport_get_func(vm, func, CURR_FUNC, is_delta, &ok);
	if (!ok)
		return;

	DllPathMap::iterator i = DPTR(dllPaths)->find(fullPath);

	if (i != DPTR(dllPaths)->end()) {

		DllEntry& pathEntry (i->second);
		DASSERT(pathEntry.second);

		ValidatableDll* vdll = DPTR(pathEntry.first);
		++pathEntry.second;	// inc ref counter

		DPTR(vm)->GetReturnValue().FromExternId(
			(void*) DPTR(vdll)->GetSerialNo(),	// Handle.
			DeltaExternId_NonCollectable,
			Dll2String,
			DLL_TYPE_STR,
			DELTA_EXTERNID_NO_FIELD_GETTER
		);
	}
	else {

		udynamiclibloader* dll = DNEWCLASS(udynamiclibloader, (fullPath));

		if (!*dll) {
			DPTR(vm)->Warning(
				"in '%s(%s,%s)': %s!",
				CURR_FUNC,
				path.c_str(), 
				func.c_str(),
				dll->GetError().c_str()
			);
			DDELETE(dll);
			RESET_NIL_RETURNVALUE;
		}

		udynamiclibloader::ResultType result = DPTR(dll)->Call(func);
		if(result.first != UTIL_DLLFUNC_TRUSTED && result.first != UTIL_DLLFUNC_TRUSTED_SETCLEANUP) {
			DDELETE(dll);
			DPTR(vm)->PrimaryError(
				"in '%s(path %s, func %s)': %s!",
				CURR_FUNC,
				path.c_str(),
				func.c_str(),
				DNULLCHECK(udynamiclibloader::GetErrorString(result.first))
			);
			RESET_NIL_RETURNVALUE;
		}

		ValidatableDll* vdll = DNEWCLASS(ValidatableDll, (dll));
		DPTR(allDlls)->insert(vdll);
		(*DPTR(dllPaths))[fullPath] = DllEntry(vdll, 1);

		// Check if the result is also an inquiry to set a cleanup function.
		if (result.first == UTIL_DLLFUNC_TRUSTED_SETCLEANUP)
			dll->SetCleanUp(result.second);

		IF_DELTA_DLL_ERROR_ELSE("path %s", path.c_str())
			DPTR(vm)->GetReturnValue().FromExternId(
				(void*) DPTR(vdll)->GetSerialNo(),	// Handle.
				DeltaExternId_NonCollectable,
				Dll2String,
				DLL_TYPE_STR,
				DELTA_EXTERNID_NO_FIELD_GETTER
			);
	}
}

///////////////////////////////////////////////////////////////////////
// ExternId dllimport(path, initFunc)
void dllimport_LibFunc (DeltaVirtualMachine* vm) 
	{ dllimport_handler(vm, "dllimport", false); }

// ExternId dllimportdeltalib(path)
void dllimportdeltalib_LibFunc (DeltaVirtualMachine* vm) 
	{ dllimport_handler(vm, "dllimportdeltalib", true); }

//------------------------------------------------------------------
// [{.succeeded: <Boolean>}, {.value: <String>} ] dllinvoke(dll, String(func)).
//

static void dll_invoke_handler (
		DeltaVirtualMachine*	vm, 
		udynamiclibloader*		dll, 
		const std::string&		func, 
		const char*				CURR_FUNC
	) {

	udynamiclibloader::ResultType result = DPTR(dll)->Call(func);

	if(result.first != UTIL_DLLFUNC_TRUSTED && result.first != UTIL_DLLFUNC_ERROR) {
		DPTR(vm)->PrimaryError(
			"in '%s(dll 0x%x, func %s)': %s!",
			CURR_FUNC,
			(util_ui32) dll,
			func.c_str(),
			DNULLCHECK(udynamiclibloader::GetErrorString(result.first))
		);
		RESET_FALSE_RETURNVALUE;
	}

	IF_DELTA_DLL_ERROR_ELSE("dll 0x%x", (util_ui32) dll)
		DPTR(vm)->GetReturnValue().FromTable(
			DELTATUPLE(
				DELTATUPLE_ID(succeeded),	DELTATUPLE_VALUE(result.first == UTIL_DLLFUNC_TRUSTED),
				DELTATUPLE_ID(value),		DELTATUPLE_VALUE(result.second)
			)
		);
}

//------------------------------------------------------------------

static void dllinvoke_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("dllinvoke");
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_FALSE_RETURNVALUE);
	GET_DLL(0, RESET_EMPTY, CURR_FUNC);
	GET_STRING(func, CURR_FUNC, RESET_FALSE_RETURNVALUE, 1);

	dll_invoke_handler(vm, dll, func, CURR_FUNC);
}

//------------------------------------------------------------------
// Bool dllhasfunction(dll, String(func)).
//
static void dllhasfunction_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("dllhasfunction");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_FALSE_RETURNVALUE);

	GET_DLL(0, RESET_EMPTY, CURR_FUNC);
	GET_STRING(func, CURR_FUNC, RESET_FALSE_RETURNVALUE, 1);

	DPTR(vm)->GetReturnValue().FromBool(dll->Has(func));
}

//------------------------------------------------------------------
// dllunimport(dll)
// dll is a udynamiclibloader* carried as an externid type.
//

#define	UNIMPORT_IMPL(_handler)								\
	if (true) {												\
	DllEntry& pathEntry (pathIter->second);					\
	DASSERT(pathEntry.second);	/* positive ref counter */	\
	if (!--pathEntry.second) {								\
		_handler;											\
		DPTR(allDlls)->remove(dllIter);						\
		DPTR(dllPaths)->erase(pathIter);					\
		DDELETE(dll);										\
		DDELETE(vdll);										\
	}														\
	} else		

static void dllunimport_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("dllunimport");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_EMPTY);
	GET_DLL(0, RESET_EMPTY, CURR_FUNC);

	UNIMPORT_IMPL(UEMPTY);
}

//------------------------------------------------------------------
// dllunimportdeltalib(dll)
// dll is a udynamiclibloader* carried as an externid type.
//
static void dllunimportdeltalib_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("dllunimportdeltalib");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_EMPTY);
	GET_DLL(0, RESET_EMPTY, CURR_FUNC);

	UNIMPORT_IMPL(dll_invoke_handler(vm, dll, DELTA_DLLIMPORT_CLEANUP_NAME, CURR_FUNC));
}

DLIB_FUNC_START(dllimportaddpath, 1, Empty)
DLIB_ARG(const char*, path)
DeltaAdd_dllimport_Path(DLIB_ARGVAL(path), false);
DLIB_FUNC_END

//------------------------------------------------------------------
// Global operator overloading.

#define	CHECK_ARITHMETIC_OPERATOR()														\
	DeltaArithOpType arithOp = OperatorOverloading::GetArithmeticOp(DLIB_ARGVAL(op));	\
	DLIB_ERROR_CHECK(																	\
		arithOp == DeltaArithOp_INVALID,												\
		uconstructstr(																	\
			"in '%s' illegal arithmetic operator '%s'",									\
			CURR_FUNC, DLIB_ARGVAL(op)													\
		)																				\
	)

#define	CHECK_OPERATOR_FUNCTION(optype)								\
	DLIB_ERROR_CHECK(												\
		!DLIB_ARGVAL(func)->IsCallable(),							\
		uconstructstr(												\
			"in '%s' for %s operator '%s' callable required",		\
			CURR_FUNC, optype, DLIB_ARGVAL(op)						\
		)															\
	)

#define	CHECK_RELATIONAL_OPERATOR()													\
	DeltaRelOpType relOp = OperatorOverloading::GetRelationalOp(DLIB_ARGVAL(op));	\
	DLIB_ERROR_CHECK(																\
		relOp == DeltaRelOp_INVALID,												\
		uconstructstr(																\
			"in '%s' illegal relational operator '%s'",								\
			CURR_FUNC, DLIB_ARGVAL(op)												\
		)																			\
	)

////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(setarithmeticoperator, 2, Empty)
DLIB_ARG(const char*,	op)
DLIB_ARG(DeltaValue*,	func)
CHECK_ARITHMETIC_OPERATOR()
CHECK_OPERATOR_FUNCTION("arithmetic")	
OperatorOverloading::SetGlobalArithmetic(arithOp, DLIB_ARGVAL(func));
DLIB_FUNC_END

DLIB_FUNC_START(getarithmeticoperator, 1, Nil)
DLIB_ARG(const char*,	op)
CHECK_ARITHMETIC_OPERATOR()	
DLIB_RETVAL_REF.Assign(OperatorOverloading::GetGlobalArithmetic(arithOp));
DLIB_FUNC_END

////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(setrelationaloperator, 2, Empty)
DLIB_ARG(const char*,	op)
DLIB_ARG(DeltaValue*,	func)
CHECK_RELATIONAL_OPERATOR()
CHECK_OPERATOR_FUNCTION("relational")
OperatorOverloading::SetGlobalRelational(relOp, DLIB_ARGVAL(func));
DLIB_FUNC_END

DLIB_FUNC_START(getrelationaloperator, 1, Nil)
DLIB_ARG(const char*,	op)
CHECK_RELATIONAL_OPERATOR()
DLIB_RETVAL_REF.Assign(OperatorOverloading::GetGlobalRelational(relOp));
DLIB_FUNC_END

////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(setassignmentoperator, 1, Empty)
DLIB_ARG(DeltaValue*,	func)
DLIB_ERROR_CHECK(
	!DLIB_ARGVAL(func)->IsCallable(), 
	uconstructstr("in '%s' callable required", CURR_FUNC)
)
OperatorOverloading::SetGlobalAssign(DLIB_ARGVAL(func));
DLIB_FUNC_END

DLIB_FUNC_START(getassignmentoperator, 0, Nil)
DLIB_RETVAL_REF.Assign(OperatorOverloading::GetGlobalAssign());
DLIB_FUNC_END

////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(settypecastingoperator, 1, Empty)
DLIB_ARG(DeltaValue*,	func)
DLIB_ERROR_CHECK(
	!DLIB_ARGVAL(func)->IsCallable(), 
	uconstructstr("in '%s' callable required", CURR_FUNC)
)
OperatorOverloading::SetGlobalTypeCasting(DLIB_ARGVAL(func));
DLIB_FUNC_END

DLIB_FUNC_START(gettypecastingoperator, 0, Nil)
DLIB_RETVAL_REF.Assign(OperatorOverloading::GetGlobalTypeCasting());
DLIB_FUNC_END

////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(taggedvalue, 2, Nil)
DLIB_ARG(DeltaValue*,	val)
DLIB_ARG(const char*,	tag)
DLIB_RETVAL_REF.Assign(DLIB_ARGVAL(val));
DLIB_RETVAL_REF.SetTypeTag(DLIB_ARGVAL(tag));
DLIB_FUNC_END

DLIB_FUNC_START(getvaluetag, 1, Nil)
DLIB_ARG(DeltaValue*,	val)
DLIB_RETVAL_REF.FromString(DLIB_ARGVAL(val)->GetTypeTag());
DLIB_FUNC_END

////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(iserrortag, 1, Nil)
DLIB_ARG(const char*, tag)
DLIB_RETVAL_REF.FromBool(
	ustrpos(
		errorTags, 
		TOTAL_ERROR_TAGS, 
		DLIB_ARGVAL(tag)
	) != TOTAL_ERROR_TAGS
);
DLIB_FUNC_END

//------------------------------------------------------------------
