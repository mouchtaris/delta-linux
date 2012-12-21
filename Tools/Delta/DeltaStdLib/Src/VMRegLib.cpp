// VMRegLib.cpp
// Manipulation of multiple virtual machines.
// Get and Call functions.
// ScriptFigher Project.
// A. Savidis, December 1999.
// Updated November 2006 (getting all VM functions).
// Refactored December 2007, encompassed in the 'std' API object. 
// Extended March 2009, added dynamic compilation  support.
//

#include "DeltaLibraryCreators.h"
#include "VMRegLib.h"
#include "DeltaStdLibFuncNames.h"
#include "DeltaStdClassNames.h"
#include "BufferLib.h"
#include "ReaderWriterLib.h"
#include "DeltaStdLib.h"
#include "Execute.h"
#include "DeltaLibFuncOverloading.h"
#include "ListLib.h"
#include "ufiles.h"
#include "usystem.h"
#include "ustrings.h"

/////////////////////////////////////////////////////

static DeltaExternIdFieldGetter* vmGetter = (DeltaExternIdFieldGetter*) 0;

static bool GetKeys (void* val, DeltaValue* at) 
	{ return DELTAVM_VGET_INST(val) ? DPTR(vmGetter)->GetAllKeys(val, at) : false; }

static bool GetId (void* val, DeltaValue* at) 
	{ at->FromString(DELTAVM_VGET_INST(val)->Id()); return true; }

static bool GetSource (void* val, DeltaValue* at) 
	{ at->FromString(DELTAVM_VGET_INST(val)->Source()); return true; }

static bool GetPC (void* val, DeltaValue* at) 
	{ at->FromNumber(DELTAVM_VGET_INST(val)->PC()); return true; }

static bool GetLine (void* val, DeltaValue* at) 
	{ at->FromNumber(DELTAVM_VGET_INST(val)->Line()); return true; }

static bool GetStatics (void* val, DeltaValue* at) {
	DeltaTable* t = DELTAVM_VGET_INST(val)->GetStaticsTable();
	if (!t)
		at->FromString("can't get, no debug information!");
	else
		at->FromTable(t); 
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry vmGetters[] = {
	{ "keys",		&GetKeys,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "id",			&GetId,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "source",		&GetSource,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "line",		&GetLine,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "pc",			&GetPC,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "statics",	&GetStatics,	DELTA_GETBYSTRING_NO_PRECOND	}
};

//-----------------------------------------------------------------------

static void vmget_LibFunc (DeltaVirtualMachine*);
static void vmcall_LibFunc (DeltaVirtualMachine*);
static void vmfuncs_LibFunc (DeltaVirtualMachine*);
static void vmfuncaddr_LibFunc (DeltaVirtualMachine*);
static void vmof_LibFunc (DeltaVirtualMachine*);
static void vmthis_LibFunc (DeltaVirtualMachine* vm);
static void vmid_LibFunc (DeltaVirtualMachine*);
static void vmcurrcall_LibFunc (DeltaVirtualMachine*);
static void vmnextcall_LibFunc (DeltaVirtualMachine*);
static void vmgetstacktrace_LibFunc (DeltaVirtualMachine*);

static void vmload_LibFunc (DeltaVirtualMachine* vm);
static void vmloadaddpath_LibFunc (DeltaVirtualMachine* vm);
static void vmloadfrominputbuffer_LibFunc (DeltaVirtualMachine* vm);
static void vmloadfromreader_LibFunc (DeltaVirtualMachine* vm);

static void vmrun_LibFunc (DeltaVirtualMachine* vm);
static void vmunload_LibFunc (DeltaVirtualMachine* vm);
static void vmseterrorinvalidatesall_LibFunc (DeltaVirtualMachine* vm);
static void vmisvalid_LibFunc (DeltaVirtualMachine*);
static void vmhaserror_LibFunc (DeltaVirtualMachine* vm);
static void vmhaserrorsomewhere_LibFunc (DeltaVirtualMachine* vm);
static void vmreseterror_LibFunc (DeltaVirtualMachine* vm);
static void vmresetallerrors_LibFunc (DeltaVirtualMachine* vm);
static void vmgeterrorreport_LibFunc (DeltaVirtualMachine* vm);

static void vmregistercopiedlib_LibFunc (DeltaVirtualMachine* vm);
static void vmregistersharedlib_LibFunc (DeltaVirtualMachine* vm);
static void vmunregisterlib_LibFunc (DeltaVirtualMachine* vm);
static void vmisregisteredcopiedlib_LibFunc (DeltaVirtualMachine* vm);
static void vmisregisteredsharedlib_LibFunc (DeltaVirtualMachine* vm); 
static void vmimportlib_LibFunc (DeltaVirtualMachine* vm);
static void vmunimportlib_LibFunc (DeltaVirtualMachine* vm);

DELTALIBFUNC_DECLARECONSTS(1, 8, "call", "unload")

#define	LP										\
			DELTA_LIBRARY_RETAINNAME_PREFIX		\
			DELTA_STDLIB_NAMESPACE_PREFIX		\
			DELTA_STDLIB_LIBS_NAMESPACE_PREFIX

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "get",						vmget_LibFunc						},

	{ "call",						vmcall_LibFunc						},
	{ "funcs",						vmfuncs_LibFunc						},
	{ "funcaddr",					vmfuncaddr_LibFunc					},
	{ "run",						vmrun_LibFunc						},
	{ "id",							vmid_LibFunc						},
	{ "reseterror",					vmreseterror_LibFunc				},
	{ "haserror",					vmhaserror_LibFunc					},
	{ "unload",						vmunload_LibFunc					},

	{ "haserrorsomewhere",			vmhaserrorsomewhere_LibFunc			},
	{ "resetallerrors",				vmresetallerrors_LibFunc			},
	{ "this",						vmthis_LibFunc						},
	{ "load",						vmload_LibFunc						},
	{ "loadaddpath",				vmloadaddpath_LibFunc				},
	{ "loadfrominputbuffer",		vmloadfrominputbuffer_LibFunc		},
	{ "loadfromreader",				vmloadfromreader_LibFunc			},

	{ "seterrorinvalidatesall", 	vmseterrorinvalidatesall_LibFunc	},
	{ "of",							vmof_LibFunc						},
	{ "currcall",					vmcurrcall_LibFunc					},
	{ "nextcall",					vmnextcall_LibFunc					},
	{ "getstacktrace",				vmgetstacktrace_LibFunc				},

	{ "isvalid",					vmisvalid_LibFunc					},
	{ "geterrorreport",				vmgeterrorreport_LibFunc			},

	{ LP "registercopied",			vmregistercopiedlib_LibFunc			},
	{ LP "registershared",			vmregistersharedlib_LibFunc			},
	{ LP "unregister",				vmunregisterlib_LibFunc				},
	{ LP "isregisteredcopied",		vmisregisteredcopiedlib_LibFunc		},
	{ LP "isregisteredshared",		vmisregisteredsharedlib_LibFunc		},
	{ LP "import",					vmimportlib_LibFunc					},
	{ LP "unimport",				vmunimportlib_LibFunc				}
};

static  bool ValidateValue (void* val) 
	{ return !!DELTAVM_VGET_INST(val); }

inline bool VAL_IS_VM (const DeltaValue& val)
	{ return val.Type() == DeltaValue_ExternId && val.GetExternIdTypeString() == DELTA_VM_TYPE_STR; }

DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(VAL_IS_VM(val), DELTA_VM_TYPE_STR)

namespace libs {
	static void Initialise (void);
	static void CleanUp(void);
	static bool WasImported (DeltaVirtualMachine* vm);
}

/////////////////////////////////////////////////////////////////////////

typedef std::map<std::string, std::string>	RunContextMap;
typedef std::list<std::string>				LoadPaths;

static ubag<DeltaVirtualMachine*>*		vms							= (ubag<DeltaVirtualMachine*>*) 0;
static ubag<std::string>*				sourcesWithRunInProgress	= (ubag<std::string>*) 0;
static RunContextMap*					sourceRunContenxts			= (RunContextMap*) 0;
static upathmanager*					pathManager					= (upathmanager*) 0;

/////////////////////////////////////////////////////////////////////////

extern void DeltaVM_AddLoadingPath (const std::string& path, bool prioritised)
	{ DPTR(pathManager)->add(path, prioritised); }

static void AddEnvironmentLoadingPaths (void)
	{ DPTR(pathManager)->add_environment_path(DELTA_ENVIRONMENT_VAR_BYTECODE_PATH, false); }

extern const std::string DeltaVM_MakeLoadingPath (void)
	{ return DPTR(pathManager)->make_entire_path(); }

#define	LOADING_RESOLVE_BYTECODE_FILE(_full_path, _file, _reset)							\
	std::string _full_path = DPTR(pathManager)->resolve(_file);								\
	if (_full_path.empty()) {																\
		DPTR(vm)->PrimaryError(																\
			"in %s byte code file '%s' was not found in cwd and '%s' vm loading path!",		\
			CURR_FUNC,																		\
			_file.c_str(),																	\
			DeltaVM_MakeLoadingPath().c_str()												\
		);																					\
		_reset;																				\
	} else

/////////////////////////////////////////////////////////////////////////

#define	CHECK_RECURSIVE_RUN_ERROR(theVM, reset)											\
	if (DPTR(sourcesWithRunInProgress)->in(theVM->Source())) {							\
		RunContextMap::iterator i = DPTR(sourceRunContenxts)->find(theVM->Source());	\
		DASSERT(i != DPTR(sourceRunContenxts)->end());									\
		DPTR(vm)->PrimaryError(															\
			"in '%s' recursive run for source '%s' (started at %s)!",					\
			CURR_FUNC,																	\
			theVM->Source(),															\
			i->second.c_str()															\
		);																				\
		reset																			\
	} else

#define	RECORD_RUN_CONTEXT(theVM)														\
	if (true) {																			\
	DPTR(sourcesWithRunInProgress)->insert(theVM->Source());							\
	(*DPTR(sourceRunContenxts))[theVM->Source()] = uconstructstr(						\
										"source '%s' vm id '%s' func '%s' line %u",		\
										vm->Source(),									\
										CURR_FUNC,										\
										vm->Id(),										\
										vm->Line()										\
									);													\
	} else

#define	RUN_VM(theVM)																	\
	if (true) {																			\
	DELTA_RECORD_VM_SERIALNO_EX(vm);													\
	DPTR(theVM)->Run();																	\
	DPTR(sourcesWithRunInProgress)->remove(theVM->Source());							\
	DPTR(sourceRunContenxts)->erase(theVM->Source());									\
	DELTA_VALIDATE_VM_SERIALNO_EX(vm, UEMPTY);											\
	} else

/////////////////////////////////////////////////////////////////////////

static void Initialise (void) {
	
	DASSERT(!vmGetter);
	unew(vms);
	unew(sourcesWithRunInProgress);
	unew(sourceRunContenxts);
	unew(pathManager);
	AddEnvironmentLoadingPaths();

	vmGetter = DNEWCLASS(DeltaExternIdFieldGetter, (&ValidateValue));
	DPTR(vmGetter)->SetGetByString(vmGetters, uarraysize(vmGetters));
	DPTR(vmGetter)->SetKeyToGetKeys("keys");
	DELTALIBFUNC_CREATE_METHODS_TABLE();
	libs::Initialise();
}

static void CleanUp(void) {
	udelete(vms); 
	udelete(sourcesWithRunInProgress);
	udelete(sourceRunContenxts);
	udelete(vmGetter); 
	udelete(pathManager);

	DELTALIBFUNC_DESTROY_METHODS_TABLE(); 
	libs::CleanUp(); 
}

DELTA_LIBRARY_SUBAPI_INSTALLER_EX(DeltaVM, "vm", DELTA_STDLIB_NAMESPACE_PREFIX "vm", Initialise(), CleanUp())

//-----------------------------------------------------------------------

DVM_FUNC void DeltaVM_ToString (DeltaString* at, void* val) {

	DeltaVirtualMachine* vm = DELTAVM_VGET_INST(val);
	DASSERT(vm);

	DPTR(vm)->Validate();
	DPTR(at)->Add(
		uconstructstr(
			"%s(0x%x)[id '%s', source '%s', line %d]",
			DPTR(vm)->GetExtClassString(),
			DPTR(vm)->GetSerialNo(),
			DPTR(vm)->Id(),
			DPTR(vm)->Source(),
			DPTR(vm)->Line()
		)
	);
}

//-----------------------------------------------------------------------

DeltaExternId* DeltaStdUtilities::MakeExternId (DeltaVirtualMachine* vm) const {

	DASSERT(!DeltaExternId::Get(DELTA_VM_TYPE_STR, (void*) DPTR(vm)->GetSerialNo()));
	DASSERT(!DPTR(vm)->GetExternId());

	return	DeltaExternIdFactory::New(
				(void*) DPTR(vm)->GetSerialNo(),
				DeltaExternId_NonCollectable,
				DeltaVM_ToString,
				DELTA_VM_TYPE_STR,
				vmGetter
			);
}

/////////////////////////////////////////////////////////////////////////

void DeltaStdUtilities::LinkWithLibraryFunctions (DeltaVirtualMachine* vm) const {

	DeltaExternId* externId = DeltaExternId::Get(DELTA_VM_TYPE_STR, (void*) DPTR(vm)->GetSerialNo());
	DASSERT(externId && DPTR(vm)->GetExternId() == externId);

	DELTALIBFUNC_DELEGATE_METHODS(externId);

	// Populate userData with all global functions.

	DeltaTable* userData	= DNULLCHECK(DPTR(externId)->GetUserData());
	DeltaTable* extra		= DeltaObject::NativeCodeHelpers::NewObject();

	typedef std::list<std::pair<std::string, DeltaValue> > FuncList;
	FuncList funcs;
	DPTR(vm)->GetGlobalFunctions(&funcs);

	for (FuncList::iterator i = funcs.begin(); i != funcs.end(); ++i) {

		DeltaValue index(i->first);

		if (i->first == DELTA_VM_USERDATA_EXTRA_FIELD_KEY)	// Func name conficts with the 'extra' slot.
			DPTR(extra)->Set(index, i->second);						// Add it to the extra table of functions.
		else {
			DeltaValue content;
			bool result = DPTR(userData)->Get(index, &content);	
			DASSERT(result);
			if (content.Type() != DeltaValue_Nil)					// Func conflicts with a bound lib function or extra slot.
				DPTR(extra)->Set(index, i->second);					// Add it to the extra table of functions.
			else													// No conflict.
				DPTR(userData)->Set(index, i->second);				// Add it to the userData table.
		}
	}

	if (!extra->Total())
		DeltaObject::NativeCodeHelpers::Delete(extra);
	else {
		DPTR(userData)->Set(DELTA_VM_USERDATA_EXTRA_FIELD_KEY, extra);
		DeltaObject::NativeCodeHelpers::GiveUp(extra);
	}
}

/////////////////////////////////////////////////////////////////////////

DVM_FUNC void DeltaVM_Make (DeltaValue* at, DeltaVirtualMachine* vm) {
	DeltaExternId* externId = DeltaExternId::Get(DELTA_VM_TYPE_STR, (void*) DPTR(vm)->GetSerialNo());
	DASSERT(externId && DPTR(vm)->GetExternId() == externId);
	at->FromExternIdBySerialNo(DPTR(externId)->GetSerialNo());
}

DVM_FUNC DeltaVirtualMachine* DeltaVM_Get (const DeltaValue& val) {
	if (val.Type() != DeltaValue_ExternId)
		return (DeltaVirtualMachine*) 0;
	return DELTAVM_VGET_INST(const_cast<DeltaValue&>(val).ToExternId());
}

/////////////////////////////////////////////////////////////////////////

#define	DLIB_GETVM	\
	DLIB_GET_VALIDATABLE_ARG(vmVal, vmVar, DeltaVirtualMachine, DELTA_VM_TYPE_STR, DELTAVM_VGET_INST)

#define	DLIB_HANDLE_NEW_VM(newVM)				\
	if (true) {									\
		DeltaVM_Make(DLIB_RETVAL_PTR, newVM);	\
		DPTR(vms)->insert(newVM);				\
		DLIB_SET_CREATOR_INFORMATION();			\
	} else

//-----------------------------------------------------------------------
// ExternId(VM) vmget(id).
// Returns the virtual machine from an id.

DLIB_FUNC_START(vmget, 1, Nil)
DLIB_ARG(const char*, id)
if (DeltaVirtualMachine* result = VMRegistry().Get(DLIB_ARGVAL(id)))
	DeltaVM_Make(DLIB_RETVAL_PTR, result);
else
	DLIB_RETVAL_REF.FromNil();
DLIB_FUNC_END

//-----------------------------------------------------------------------
// Table[] vmfuncs(id or VM).
// Returns the a table with all gobal functions as elements
// indexed by their name.
//
static void vmfuncs_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC(DELTA_STDLIB_NAMESPACE_PREFIX "vmfuncs");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DeltaVirtualMachine*	targetVM	= (DeltaVirtualMachine*) 0;
	DeltaValue*				argObj		= DPTR(vm)->GetActualArg(0);

	if (argObj->Type() == DeltaValue_String) {
		std::string id = argObj->ToString();
		if (!(targetVM = VMRegistry().Get(id.c_str()))) {
			DPTR(vm)->PrimaryError(
				DEFORMAT_INVALID_VMID,
				id.c_str(),
				CURR_FUNC
			);
			RESET_NIL_RETURNVALUE
		}
	}
	else
	if (argObj->Type() == DeltaValue_ExternId) {
		GET_VM(theVM, RESET_NIL_RETURNVALUE, 0); 
		targetVM = theVM;
	}
	else {
		DPTR(vm)->PrimaryError(DEFORMAT_VMID_OR_VM_EXPECTED, CURR_FUNC);
		RESET_NIL_RETURNVALUE
	}

	if (DPTR(targetVM)->HasNotRunYet()) {
		DPTR(vm)->PrimaryError(DEFORMAT_VM_NOT_RUN, DPTR(targetVM)->Id(), CURR_FUNC);
		RESET_NIL_RETURNVALUE;
	}
	else
		DPTR(targetVM)->GetGlobalFunctionsTable(
			&DPTR(vm)->GetReturnValue()
		);
}

//-----------------------------------------------------------------------
// ProgramFunc vmfuncaddr(ExternId(vm), String(func)).

DLIB_FUNC_START(vmfuncaddr, 2, Nil)
DLIB_GETVM
DLIB_ERROR_CHECK(
	DPTR(vmVar)->HasNotRunYet(), 
	uconstructstr(DEFORMAT_VM_NOT_RUN, DPTR(vmVar)->Id(), CURR_FUNC)
)
DLIB_ARG(const char*, func)
DeltaCodeAddress addr = DPTR(vmVar)->GlobalFuncAddress(DLIB_ARGVAL(func));
if (addr != DELTA_CANTBE_A_FUNCTION_ADDRESS)
	DLIB_RETVAL_REF.FromProgramFunc(addr, vmVar);
else
	DLIB_RETVAL_REF.FromNil();
DLIB_FUNC_END

//-----------------------------------------------------------------------
// string vmid(ExternId(vm)).

DLIB_FUNC_START(vmid, 1, Nil)
DLIB_GETVM
DLIB_RETVAL_REF.FromString(DPTR(vmVar)->Id());
DLIB_FUNC_END

//-----------------------------------------------------------------------

#define	VMCALL_NUM_STDARGS	2

// vmcall(vm,func,arg1,...,argN).
// vm can be either string or extern id.
// func must be string.
// arg1,...,argN can be whatever (optional).
//
void vmcall_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC(DELTA_STDLIB_NAMESPACE_PREFIX "vmcall");
	DeltaAtLeastTotalArgsCheck(VMCALL_NUM_STDARGS, CURR_FUNC, RESET_NIL_RETURNVALUE)

	// Firstly check for a legal VM and extract the
	// virual machine instance. VM can be defined either via an id,
	// or directly with a VM ptr.
	//
	DeltaValue*				argObj		= vm->GetActualArg(0);
	DeltaVirtualMachine*	targetVM	= (DeltaVirtualMachine*) 0;

	if (argObj->Type() == DeltaValue_String) {
		std::string id = argObj->ToString();
		targetVM = VMRegistry().Get(id.c_str());

		if (!targetVM) {
			DPTR(vm)->PrimaryError("In 'vmcall', vm '%s' not found",
				id.c_str()
			);
			RESET_NIL_RETURNVALUE;
		}
	}
	else
	if (argObj->Type() == DeltaValue_ExternId) {
		GET_VM(theVM, RESET_NIL_RETURNVALUE, 0); 
		targetVM = theVM;
	}
	else {
		DPTR(vm)->PrimaryError(
			"String or ExternId expected (%s passed) as 1st arg to 'vmcall'",
			argObj->TypeStr()
		);
		RESET_NIL_RETURNVALUE;
	}

	if (DPTR(targetVM)->HasNotRunYet()) {
		DPTR(vm)->PrimaryError(DEFORMAT_VM_NOT_RUN, DPTR(targetVM)->Id(), CURR_FUNC);
		RESET_NIL_RETURNVALUE;
	}

	// Now check for a string or func addr argument, which should be a
	// global function for the target VM.
	//
	argObj = vm->GetActualArg(1);
	DeltaCodeAddress funcAddr = 0;

	if (argObj->Type()==DeltaValue_String) {

		// Check if global function.

		std::string funcName = argObj->ToString();

		if (!targetVM->GlobalFuncExists(funcName.c_str())) {
			DPTR(vm)->PrimaryError("In 'vmcall', '%s' func not found in vm '%s' (0x%x)",
				funcName.c_str(),
				targetVM->Id(),
				(util_ui32) targetVM
			);
			RESET_NIL_RETURNVALUE;
		}
		else
			funcAddr = targetVM->GlobalFuncAddress(funcName.c_str());
	}
	else {
		DPTR(vm)->PrimaryError(
			"String 2nd arg expected (%s passed) to 'vmcall'",
			argObj->TypeStr()
		);
		RESET_NIL_RETURNVALUE;
	}

	// We may invoke a vm that already produced an error.

	if (DPTR(targetVM)->HasProducedError()) {
		DPTR(vm)->PrimaryError(
			"calling a function of vm '%s' which produced an error!", 
			DPTR(targetVM)->Id()
		);
		RESET_NIL_RETURNVALUE;
	}

	// Make the call.
	std::list<DeltaValue*> args;
	for (util_ui16 i = vm->TotalActualArgs() - 1; i >= VMCALL_NUM_STDARGS; --i)
		args.push_back(vm->GetActualArg(i));
	DeltaValue func(funcAddr, targetVM);
	func(args, &vm->GetReturnValue());
}

//-----------------------------------------------------------------------
// ExternId(VM*) vmthis().
// Return current VM as an extern id.

DLIB_FUNC_START(vmthis, 0, Nil)
DeltaVM_Make(DLIB_RETVAL_PTR, vm);
DLIB_FUNC_END

#define	DLIB_LOADING_NEW_VM(newVM, from, id)						\
		if (!DPTR(newVM)->Load(from)){								\
			DDELETE(newVM);											\
			DPTR(vm)->PrimaryError(									\
				"in '%s'(vm id '%s') could not load byte code!",	\
				CURR_FUNC,											\
				ucstringarg(id)										\
			);														\
			DLIB_RETVAL_REF.FromNil();								\
		} else DLIB_HANDLE_NEW_VM(newVM)

//-----------------------------------------------------------------------
// ExternId(VM*) vmload(String(file), String(VmId));
// Loads compiled code into a virtual machine.
//
static void vmload_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("vmload");
	DeltaTotalArgsCheck(2, "vmload", RESET_NIL_RETURNVALUE);
	DeltaValue*	argObj;
	GET_STRING(file, CURR_FUNC, RESET_NIL_RETURNVALUE, 0);
	GET_STRING(vmId, CURR_FUNC, RESET_NIL_RETURNVALUE, 1);

	if (VMRegistry().Get(vmId.c_str())) {
		DPTR(vm)->PrimaryError(
			DEFORMAT_VM_ALREADY_LOADED,
			CURR_FUNC,
			vmId.c_str()
		);
		RESET_NIL_RETURNVALUE;
	}
	
	LOADING_RESOLVE_BYTECODE_FILE(fullPath, file, RESET_NIL_RETURNVALUE);
	DeltaVirtualMachine* newVM = DNEWCLASS(DeltaVirtualMachine, (vmId.c_str()));
	DLIB_LOADING_NEW_VM(newVM, fullPath.c_str(), vmId);
}

/////////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(vmloadaddpath, 1, Empty)
DLIB_ARG(const char*, path)
DeltaVM_AddLoadingPath(DLIB_ARGVAL(path), false);
DLIB_FUNC_END

/////////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(vmloadfrominputbuffer, 2, Nil)
DLIB_GET_INPUTBUFFER
DLIB_ARG(const char*,	id)
DLIB_ERROR_CHECK(
	VMRegistry().Get(DLIB_ARGVAL(id)), 
	uconstructstr("a vm with id '%s' already exists", DLIB_ARGVAL(id))
)

DeltaVirtualMachine* newVM = DNEWCLASS(DeltaVirtualMachine, (DLIB_ARGVAL(id)));
PortableBufferReader reader(DPTR(ib));
DLIB_LOADING_NEW_VM(newVM, reader, DLIB_ARGVAL(id));
DLIB_FUNC_END

/////////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(vmloadfromreader, 2, Nil)
DLIB_GET_READER
DLIB_ARG(const char*, id)
DLIB_ERROR_CHECK(
	VMRegistry().Get(DLIB_ARGVAL(id)), 
	uconstructstr("a vm with id '%s' already exists", DLIB_ARGVAL(id))
)

DeltaVirtualMachine* newVM = DNEWCLASS(DeltaVirtualMachine, (DLIB_ARGVAL(id)));
DLIB_LOADING_NEW_VM(newVM, *DPTR(reader), DLIB_ARGVAL(id));
DLIB_FUNC_END

//-----------------------------------------------------------------------
// bool vmrun(ExternId(vm)).

static void vmrun_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC(DELTA_STDLIB_NAMESPACE_PREFIX "vmrun");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_FALSE_RETURNVALUE);

	DeltaValue*	argObj;
	GET_VM(theVM, RESET_EMPTY, 0);

	CHECK_RECURSIVE_RUN_ERROR(theVM, RESET_FALSE_RETURNVALUE);
	RECORD_RUN_CONTEXT(theVM);

	std::string src = DPTR(theVM)->Source(), id = DPTR(theVM)->Id();
	RUN_VM(theVM);

	if (DeltaVirtualMachine::RunTimeErrorsExist()) {
		DPTR(vm)->PossiblePrimaryError(
			"in '%s' for vm source '%s' id '%s', errors occured!",
			CURR_FUNC,
			src.c_str(),
			id.c_str()
		);
		RESET_FALSE_RETURNVALUE;
	}
		
	// Fallback successful case.
	DPTR(vm)->GetReturnValue().FromBool(true);
}

//-----------------------------------------------------------------------
// vmunload(ExternId(vm)).

DLIB_FUNC_START(vmunload, 1, Empty)
DLIB_GETVM
DLIB_ERROR_CHECK(
	libs::WasImported(vmVar),
	uconstructstr("the vm '%s' was imported (not created with vmload)!", DPTR(vmVar)->Id())
)
DLIB_ERROR_CHECK(
	!DPTR(vms)->in(vmVar),
	uconstructstr("the vm '%s' was created internally (not via vmload)!", DPTR(vmVar)->Id())
)
DPTR(vms)->remove(vmVar);
DDELETE(vmVar);
DLIB_FUNC_END

//-----------------------------------------------------------------------
// vmseterrorinvalidatesall(Bool flag).

DLIB_FUNC_START(vmseterrorinvalidatesall, 1, Empty)
DLIB_ARG(bool, flag)
DeltaVirtualMachine::SetAnyErrorInvalidatesAll(DLIB_ARGVAL(flag));
DLIB_FUNC_END

//-----------------------------------------------------------------------
// vmof(Callable(f)).
//
static void vmof_LibFunc (DeltaVirtualMachine* vm) {
	ISSUE_FUNC(DELTA_STDLIB_NAMESPACE_PREFIX "vmof");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DeltaValue* f = DPTR(vm)->GetActualArg(0);

	if (f->Type() == DeltaValue_ProgramFunc || f->Type() == DeltaValue_MethodFunc) {
		DeltaVirtualMachine*	theVM;
		DeltaCodeAddress		addr;
		if (f->Type() == DeltaValue_ProgramFunc)
			f->ToProgramFunc(&addr, &theVM);
		else 
			{ DeltaTable* t; f->ToMethodFunc(&addr, &t, &theVM); }
		if (theVM)
			DeltaVM_Make(DLIB_RETVAL_PTR, theVM);
		else
			vm->GetReturnValue().FromNil();			
	}
	else
		DPTR(vm)->PrimaryError("in %s(), illegal arg type %s!", f->TypeStr());
}

//-----------------------------------------------------------------------
// Bool vmisvalid(ExternId(vm)).

DLIB_FUNC_START(vmisvalid, 1, False)
DLIB_ARGEX_JUSTVALUE(val)
DLIB_RETVAL_REF.FromBool(!!DELTAVM_VGET_INST(DLIB_ARGVAL(val)));
DLIB_FUNC_END

//-----------------------------------------------------------------------
// Bool vmhaserror(vm).

DLIB_FUNC_START(vmhaserror, 1, True)
DLIB_GETVM
DLIB_RETVAL_REF.FromBool(DPTR(vmVar)->HasProducedError());
DLIB_FUNC_END

//-----------------------------------------------------------------------
// Bool vmhaserrorsomewhere().

DLIB_FUNC_START(vmhaserrorsomewhere, 0, True)
DLIB_RETVAL_REF.FromBool(DeltaVirtualMachine::RunTimeErrorsExist());
DLIB_FUNC_END

//-----------------------------------------------------------------------
// vmreseterror(vm).

DLIB_FUNC_START(vmreseterror, 1, Empty)
DLIB_GETVM
if (DPTR(vmVar)->HasProducedError())
	DPTR(vmVar)->ResetError();
DLIB_FUNC_END

//-----------------------------------------------------------------------
// vmresetallerrors().

DLIB_FUNC_START(vmresetallerrors, 0, Empty)
DeltaVirtualMachine::ResetRunTimeErrors();
DLIB_FUNC_END

//-----------------------------------------------------------------------
// String vmgeterrorreport()

DLIB_FUNC_START(vmgeterrorreport, 0, Nil)
if (UERROR_ISRAISED())
	DLIB_RETVAL_REF.FromString(uerror::GetSingleton().getreport());
else
	DLIB_RETVAL_REF.FromNil();
DLIB_FUNC_END

//-----------------------------------------------------------------------

#define	RETURN_CALL_INFORMATION()														\
if (true) {																				\
DeltaValue vmObj(DeltaValue::Nil);														\
if (info.vm)																			\
	DeltaVM_Make(&vmObj, info.vm);														\
DLIB_RETVAL_REF.FromTable(																\
	DELTATUPLE(																			\
		DELTATUPLE_ID(tag),		DELTATUPLE_VALUE((DeltaNumberValueType) info.tag),		\
		DELTATUPLE_ID(func),	info.func,												\
		DELTATUPLE_ID(name),	DELTATUPLE_VALUE(info.name),							\
		DELTATUPLE_ID(vm),		vmObj													\
	)																					\
); } else

DLIB_FUNC_START(vmcurrcall, 0, Nil)
DeltaVirtualMachine::CallInformation info;
DeltaVirtualMachine::GetCurrentCall(&info);
DASSERT(info.name == std::string(DELTA_STDLIB_NAMESPACE_PREFIX) + CURR_FUNC);	// This is the current library function actually.
bool result = DeltaVirtualMachine::GetNextCall(info.tag, &info);
DASSERT(result);					// No way the lib func is the bottom call.
RETURN_CALL_INFORMATION();
DLIB_FUNC_END

DLIB_FUNC_START(vmnextcall, 1, Nil)
DLIB_ARG(util_ui32,	tag)
DLIB_ERROR_CHECK(
	!DeltaVirtualMachine::IsValidCallTag(DLIB_ARGVAL(tag)),
	uconstructstr("invalid call tag 0x%xs supplied", DLIB_ARGVAL(tag))
)
DeltaVirtualMachine::CallInformation info;
if (DeltaVirtualMachine::GetNextCall(DLIB_ARGVAL(tag), &info))
	RETURN_CALL_INFORMATION();
else
	DLIB_RETVAL_REF.FromNil();	// Bottom call.
DLIB_FUNC_END

DLIB_FUNC_START(vmgetstacktrace, 0, Nil)
DLIB_RETVAL_REF.FromString(DeltaVirtualMachine::GetStackTrace());
DLIB_FUNC_END

//-----------------------------------------------------------------------

namespace libs {

typedef uquadruple<std::string, CollectableSimple*, bool, std::string> LibInfoBase;

struct LibInfo : public LibInfoBase  {

	util_ui32 totalLoaders;

	LibInfo (
		const std::string&	path, 
		CollectableSimple*	ib, 
		bool				isShared, 
		const std::string&	id
	) : LibInfoBase(path, ib, isShared, id),  totalLoaders(0)  
		{ if (ib)  ib->IncRefCounter(); }

	LibInfo (void) :  LibInfoBase("", (CollectableSimple*) 0, false, "") , totalLoaders(0)
		{}

	LibInfo (const LibInfo& info) : LibInfoBase(info) 
		{ if (second)  second->IncRefCounter();  }

	~LibInfo() { if (second) second->DecRefCounter(); }

	void				IncLoaders (void) 
							{ ++totalLoaders; }
	void				DecLoaders (void) 
							{ DASSERT(totalLoaders); --totalLoaders; }
	util_ui32			GetTotalLoaders (void) const 
							{ return totalLoaders; }

	const std::string	ToString (void) const {
							if (second)
								return	uconstructstr(
											"%s sn(0x%x)", 
											second->GetExtClassString(), 
											second->GetSerialNo()
										);
							else
								return first;
						}

	UOVERLOADED_ASSIGN_VIA_COPY_CONSTRUCTOR(LibInfo)
};

/////////////////////////////////////////////////////////////////////////

typedef std::pair<DeltaVirtualMachine*, util_ui32>	VMInfo;
typedef std::list<VMInfo>							VMList;
typedef std::map<std::string, VMList>				ImportMap;
typedef std::map<DeltaVirtualMachine*, std::string>	VMMap;
typedef std::map<std::string, LibInfo>				InfoMap;

static ImportMap*	imported	= (ImportMap*) 0;
static InfoMap*		info		= (InfoMap*) 0;
static VMMap*		vms			= (VMMap*) 0;

static void Initialise (void) 
	{ unew(imported); unew(info); unew(vms); }

static void CleanUp (void) 
	{ udelete(imported); udelete(info); udelete(vms); }

/////////////////////////////////////////////////////////////////////////

#define	CHECK_NOT_REGISTERED()															\
DLIB_ERROR_CHECK(																		\
	DPTR(info)->find(DLIB_ARGVAL(id)) != DPTR(info)->end(),								\
	uconstructstr("a lib with id '%s' is already registered", DLIB_ARGVAL(id))			\
)

#define	CHECK_IS_REGISTERED()															\
DLIB_ERROR_CHECK(																		\
	DPTR(info)->find(DLIB_ARGVAL(id)) == DPTR(info)->end(),								\
	uconstructstr("a lib with id '%s' no registered", DLIB_ARGVAL(id))					\
)

typedef std::pair<std::string, CollectableSimple*> GetResult;

static const GetResult GetPathOrInputBuffer (DeltaVirtualMachine* vm, DeltaValue* arg, const char* func) {
	if (arg->Type() == DeltaValue_String)
		return GetResult(arg->ToString(), (CollectableSimple*) 0);
	else {
		DeltaValue actual;
		if (!arg->ConvertToExternId(INPUTBUFFER_TYPE_STR, &actual))	{
			DPTR(vm)->PrimaryError(DEFORMAT_ERROR_WHILE_TRYING_TO_MATCH_EXTERNID_TYPE, 1, INPUTBUFFER_TYPE_STR, func);
			return GetResult("", (CollectableSimple*) 0);
		}
		else
		if (actual.IsUndefined()) {
			DPTR(vm)->PrimaryError(DEFORMAT_NO_TYPE_MATCHING_FOR_EXTERNID, 1, INPUTBUFFER_TYPE_STR, func);
			return GetResult("", (CollectableSimple*) 0);
		}
		else {
			DASSERT(actual.Type() == DeltaValue_ExternId && actual.GetExternIdTypeString() == INPUTBUFFER_TYPE_STR);
			CollectableSimple* c = VGET_INST(CollectableSimple, actual.ToExternId(), INPUTBUFFER_TYPE_STR);
			DASSERT(c);
			return GetResult("", c);
		}
	}
}

/////////////////////////////////////////////////////////////////////////

#define	REGISTER_IMPL(name, is_shared)														\
DLIB_FUNC_START(name, 2, Empty)																\
DLIB_ARG(const char*, id)																	\
DLIB_ARG(DeltaValue*, x)																	\
GetResult r = GetPathOrInputBuffer(vm, DLIB_ARGVAL(x), #name);								\
if (r.first.empty() && !r.second)															\
	{ DLIB_RESET_RETURN; }																	\
CHECK_NOT_REGISTERED()																		\
(*DPTR(info))[DLIB_ARGVAL(id)] = LibInfo(r.first, r.second, is_shared, DLIB_ARGVAL(id));	\
DLIB_FUNC_END

REGISTER_IMPL(registercopied, false)
REGISTER_IMPL(registershared, true)

/////////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(unregister, 1, Empty)
DLIB_ARG(const char*, id)																
CHECK_IS_REGISTERED()
DPTR(info)->erase(DLIB_ARGVAL(id));
DLIB_FUNC_END

DLIB_FUNC_START(isregisteredcopied, 1, False)
DLIB_ARG(const char*, id)
InfoMap::iterator i = DPTR(info)->find(DLIB_ARGVAL(id));
DLIB_RETVAL_REF.FromBool(i != DPTR(info)->end() && !i->second.third);
DLIB_FUNC_END

DLIB_FUNC_START(isregisteredshared, 1, False)
DLIB_ARG(const char*, id)
InfoMap::iterator i = DPTR(info)->find(DLIB_ARGVAL(id));
DLIB_RETVAL_REF.FromBool(i != DPTR(info)->end() && i->second.third);
DLIB_FUNC_END

/////////////////////////////////////////////////////////////////////////

static void import_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("import");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);
	DeltaValue*	argObj;
	GET_STRING(id, CURR_FUNC, RESET_NIL_RETURNVALUE, 0);

	InfoMap::iterator i = DPTR(info)->find(id);
	if (i == DPTR(info)->end()) {
		DPTR(vm)->PrimaryError(
			DEFORMAT_VM_LIB_NOT_REGISTERED,
			CURR_FUNC,
			id.c_str()
		);
		RESET_NIL_RETURNVALUE;
	}

	DeltaVirtualMachine*	loadedVM	= (DeltaVirtualMachine*) 0;
	ImportMap::iterator		j			= DPTR(imported)->find(id);
	bool					nowCreated	= false;

	if (i->second.third && j != DPTR(imported)->end()) { // It is a shared library already imported.
		DASSERT(j->second.size() == 1);	// Shared libs require one instance.
		loadedVM = j->second.front().first;
		DASSERT(loadedVM == DELTAVM_VGET_INST(j->second.front().second));
	}
	else {
		std::string vmId =	i->second.third ? 
								(id + "(shared)") : 
								uconstructstr("%s%u", id.c_str(), j != DPTR(imported)->end() ? j->second.size() : 0);

		loadedVM = DNEWCLASS(DeltaVirtualMachine, (vmId.c_str()));
		nowCreated = true;

		bool loadResult = false;
		if (i->second.first.empty()) {
			DASSERT(i->second.second);
			ubinaryio::InputBuffer* ib = (ubinaryio::InputBuffer*) i->second.second->GetValue();
			PortableBufferReader reader(DPTR(ib));
			loadResult = DPTR(loadedVM)->Load(reader);
		}
		else {
			DASSERT(!i->second.second);
			LOADING_RESOLVE_BYTECODE_FILE(fullPath, i->second.first, RESET_NIL_RETURNVALUE);
			loadResult = DPTR(loadedVM)->Load(fullPath.c_str());
		}

		if (!loadResult) {
			udelete(loadedVM); 
			DPTR(vm)->PrimaryError(
				DEFORMAT_VM_LIB_IMPORT_ERROR,
				CURR_FUNC,
				id.c_str(),
				i->second.ToString().c_str()
			);
			RESET_NIL_RETURNVALUE;
		}
		else {

			CHECK_RECURSIVE_RUN_ERROR(loadedVM, RESET_NIL_RETURNVALUE);
			RECORD_RUN_CONTEXT(loadedVM);
			RUN_VM(loadedVM);

			if (DeltaVirtualMachine::RunTimeErrorsExist()) {
				DPTR(vm)->PossiblePrimaryError(
					DEFORMAT_VM_LIB_RUN_ERROR,
					CURR_FUNC,
					id.c_str()
				);
				RESET_NIL_RETURNVALUE
			}
			
		}
	}

	if (loadedVM) {
		DeltaVM_Make(DLIB_RETVAL_PTR, loadedVM);
		if (nowCreated) {
			DLIB_SET_CREATOR_INFORMATION();
			(*DPTR(imported))[id].push_back(VMInfo(loadedVM, loadedVM->GetSerialNo()));
		}
		i->second.IncLoaders();
		(*DPTR(vms))[loadedVM] = id;
	}
}

/////////////////////////////////////////////////////////////////////////

static bool WasImported (DeltaVirtualMachine* vm) 
	{ return DPTR(vms)->find(vm) != DPTR(vms)->end(); } 

static void unimport_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("unimport");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_EMPTY);
	DeltaValue*	argObj;
	GET_VM(theVM, RESET_EMPTY, 0);

	VMMap::iterator	i = DPTR(vms)->find(theVM);
	if (i == DPTR(vms)->end())
		DPTR(vm)->PrimaryError(
			DEFORMAT_VM_LIB_WAS_NOT_IMPORTED,
			CURR_FUNC,
			theVM->Id()
		);
	else {
		ImportMap::iterator	j = DPTR(imported)->find(i->second);
		DASSERT(j != DPTR(imported)->end());

		InfoMap::iterator k = DPTR(info)->find(i->second);
		DASSERT(k != DPTR(info)->end());

		k->second.DecLoaders();
		if (!k->second.third || !k->second.GetTotalLoaders()) {	// Copied or shared and no more loaders remain.
			DPTR(vms)->erase(i);
			VMList::iterator q  =	std::find(
										j->second.begin(), 
										j->second.end(), 
										VMInfo(theVM, theVM->GetSerialNo())
									);
			DASSERT(q != j->second.end());
			j->second.erase(q);
			DDELETE(theVM);
		}		
	}
}

}	// libs namespace closing

/////////////////////////////////////////////////////////////////////////

static void vmregistercopiedlib_LibFunc (DeltaVirtualMachine* vm) 
	{ libs::registercopied_LibFunc(vm); }

static void vmregistersharedlib_LibFunc (DeltaVirtualMachine* vm) 
	{ libs::registershared_LibFunc(vm); }

static void vmunregisterlib_LibFunc (DeltaVirtualMachine* vm) 
	{ libs::unregister_LibFunc(vm); }

static void vmimportlib_LibFunc (DeltaVirtualMachine* vm) 
	{ libs::import_LibFunc(vm); }

static void vmisregisteredcopiedlib_LibFunc (DeltaVirtualMachine* vm) 
	{ libs::isregisteredcopied_LibFunc(vm); }

static void vmisregisteredsharedlib_LibFunc (DeltaVirtualMachine* vm) 
	{ libs::isregisteredshared_LibFunc(vm); }

static void vmunimportlib_LibFunc (DeltaVirtualMachine* vm) 
	{ libs::unimport_LibFunc(vm); }

//-----------------------------------------------------------------------
