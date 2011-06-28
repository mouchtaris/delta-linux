#include "CorbaCommonLib.h"

#include "ToCorbaConverter.h"
#include "ToDeltaConverter.h"
#include "ConversionException.h"

#include "DeltaLibraryCreators.h"

//**********************************************************************
// API, CREATION.
//
static void corba_init_LibFunc (DeltaVirtualMachine*);
static void corba_cleanup_LibFunc (DeltaVirtualMachine*);
static void corba_enum_LibFunc (DeltaVirtualMachine*);
static void corba_union_LibFunc (DeltaVirtualMachine*);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "init",		corba_init_LibFunc		},
	{ "cleanup",	corba_cleanup_LibFunc	},
	{ "enum",		corba_enum_LibFunc		},
	{ "union",		corba_union_LibFunc		}
};

/////////////////////////////////////////////////////////////////////

static void corba_init_LibFunc(DeltaVirtualMachine* vm) {
	//Manual initialization as we don't want a fixed number of arguments
	ISSUE_FUNC("corba_init");
	DeltaFalseReset reseter(vm);
	if (corba::IsInitialized()) {
		DPTR(vm)->PrimaryError("corba library is already initialized");																				\
		DLIB_RESET_RETURN;
	}	
	else {
		std::list<std::string> initRefs;
		for (util_ui16 n = DPTR(vm)->TotalActualArgs(), i = 0; i < n; i++) {
			DeltaValue* arg = DPTR(vm)->GetActualArg(i);
			DASSERT(arg);
			if (arg->Type() != DeltaValue_String) {
				DPTR(vm)->PrimaryError(DEFORMAT_ARGTYPE, i, "String", arg->TypeStr(), CURR_FUNC);
				DLIB_RESET_RETURN;
			}
			else
				initRefs.push_back(arg->ToString());
		}
		if (!corba::Initialize(initRefs))
			{ DLIB_RESET_RETURN; }
		else
			DLIB_RETVAL_REF.FromBool(true);
	}
}

/////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(corba_cleanup, 0, Empty)
	ERROR_IF_CORBA_NOT_INITIALIZED;
	corba::Cleanup();
DLIB_FUNC_END

/////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(corba_enum, 1, Nil)
	DLIB_ARG(const char*, value);
	DLIB_RETVAL_REF.FromString(DLIB_ARGVAL(value));
	DLIB_RETVAL_REF.SetTypeTag("enum");
DLIB_FUNC_END

/////////////////////////////////////////////////////////////////////

static void corba_union_LibFunc(DeltaVirtualMachine* vm) {
	//Manual initialization as we don't want a fixed number of arguments
	ISSUE_FUNC("corba_union");
	const util_ui16 n = DPTR(vm)->TotalActualArgs();
	if (n == 0)	//undefined union structure with an empty table
		DLIB_RETVAL_REF.FromTable(DeltaObject::NewObject());
	else if (n == 1) {	//default index and value given in the first arg
		DeltaTable* table = DeltaObject::NewObject();
		DLIB_RETVAL_REF.FromTable(table);
		table->Set("value", *DPTR(vm)->GetActualArg(0));
	}
	else if (n == 2) {	//normal union with type (1st arg) and value (2nd arg) indices
		DeltaValue type = *DPTR(vm)->GetActualArg(0);
		if (type.Type() == DeltaValue_String) {
			DeltaTable* table = DeltaObject::NewObject();
			DLIB_RETVAL_REF.FromTable(table);
			table->Set("type", type);
			table->Set("value", *DPTR(vm)->GetActualArg(1));
		}
		else {
			DPTR(vm)->PrimaryError(DEFORMAT_ARGTYPE, 0, "String", type.TypeStr(), CURR_FUNC);
			RESET_NIL_RETURNVALUE;
		}
	}
	else {
		DPTR(vm)->PrimaryError(DEFORMAT_LESSARGSNO, 2, n, CURR_FUNC);
		RESET_NIL_RETURNVALUE;
	}
}

/////////////////////////////////////////////////////////////////////

static unsigned initCounter = 0;

static DeltaObject* corbaApi = (DeltaObject*) 0;
DELTA_LIBRARY_GETAPI_IMPL(corba_getapi,corbaApi)
CORBACOMMON_FUNC DeltaObject * GetCorbaAPI (void) { return corbaApi; }

static DeltaObject* commonApi = (DeltaObject*) 0;

void Install_DeltaCorbaCommon_Lib (void) {
	if (++initCounter == 1) {
		corbaApi = DeltaLibraryObjectCreator::CreateAPI();
		DeltaLibFuncBinder::Add("corba_getapi", corba_getapi_LibFunc);
		ToCorbaConverter::Initialize();
		ToDeltaConverter::Initialize();
		commonApi = DeltaLibraryObjectCreator::CreateAPI(funcs, corbaApi, "common", CORBA_NAMESPACE_PREFIX);
	}
}

void CleanUp_DeltaCorbaCommon_Lib (void) {
	if (--initCounter == 0) {
		DeltaLibraryObjectCreator::DestroyAPI(commonApi);
		ToCorbaConverter::Cleanup();
		ToDeltaConverter::Cleanup();
		DeltaLibraryObjectCreator::DestroyAPI(corbaApi);
	}
}