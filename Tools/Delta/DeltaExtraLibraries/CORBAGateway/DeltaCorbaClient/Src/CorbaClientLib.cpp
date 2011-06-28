#include "CorbaClientLib.h"
#include "CorbaCommonLib.h"

#include "CorbaClientWrapper.h"
#include "CorbaDeferredCall.h"
#include "ToCorbaConverter.h"
#include "ToDeltaConverter.h"
#include "ConversionException.h"

#include "DeltaLibraryCreators.h"

#define CATCH_CORBA_EXCEPTION_INSIDE_LIBRARY_FUNC													\
		catch(CORBA::UnknownUserException& e) {														\
			DeltaValue exception;																	\
			try { ToDeltaConverter::convert(&exception, &e.exception()); }							\
			catch(const ConversionException&) {														\
				DPTR(vm)->PrimaryError("cannot convert corba exception of type %s", e._rep_id());	\
				DLIB_RESET_RETURN;																	\
			}																						\
			DELTA_EXCEPTIONS_THROW_INSIDE_LIBRARY_FUNC(exception);									\
		}																							\
		catch(const CORBA::UserException& e) {														\
			DeltaTable* table = DeltaObject::NewObject();										\
			DeltaValue exception;																	\
			exception.FromTable(table);																\
			table->Set("id", e._rep_id());															\
			table->Set("name", e._name());															\
			DELTA_EXCEPTIONS_THROW_INSIDE_LIBRARY_FUNC(exception);									\
		}																							\
		catch(const CORBA::Exception& e) {															\
			DeltaValue exception;																	\
			CORBA::Any any;																			\
			any <<= e;																				\
			try { ToDeltaConverter::convert(&exception, &any); }									\
			catch(const ConversionException&) {														\
				DPTR(vm)->PrimaryError("cannot convert corba exception of type %s", e._rep_id());	\
				DLIB_RESET_RETURN;																	\
			}																						\
			DELTA_EXCEPTIONS_THROW_INSIDE_LIBRARY_FUNC(exception);									\
		}


//**********************************************************************
// FIELD GETTER
//
static DeltaExternIdFieldGetter* getter = (DeltaExternIdFieldGetter*) 0;

static bool GetKeys (void* val, DeltaValue* at) 
	{ return CORBA_CLIENT_VAL(val)? DPTR(getter)->GetAllKeys(val, at) : false; }

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys", &GetKeys,	DELTA_GETBYSTRING_NO_PRECOND }
};

//**********************************************************************
// API, CREATION, BINDERS.
//
static void corba_readonly_attribute_set_LibFunc (DeltaVirtualMachine*);
static void corba_attribute_set_LibFunc (DeltaVirtualMachine*);
static void corba_attribute_get_LibFunc (DeltaVirtualMachine*);

static void corba_deferred_isfinished_LibFunc (DeltaVirtualMachine*);
static void corba_deferred_getresponse_LibFunc (DeltaVirtualMachine*);

static void corba_connect_LibFunc (DeltaVirtualMachine*);
static void corba_connect_by_ior_LibFunc (DeltaVirtualMachine*);
static void corba_resolve_initial_references_LibFunc (DeltaVirtualMachine*);
static void corba_invoke_LibFunc (DeltaVirtualMachine*);
static void corba_invoke_deferred_LibFunc (DeltaVirtualMachine*);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "connect",					corba_connect_LibFunc						},
	{ "connect_by_ior",				corba_connect_by_ior_LibFunc				},
	{ "resolve_initial_references",	corba_resolve_initial_references_LibFunc	},
	{ "invoke",						corba_invoke_LibFunc						},
	{ "invoke_deferred",			corba_invoke_deferred_LibFunc				}
};

//**********************************************************************

static bool FromObjectClientConversion(DeltaValue* output, CORBA::Object_ptr obj);
static bool ToObjectClientConversion(CORBA::Any* output, DeltaValue* input);

static void Initialize (void) {
	DeltaLibFuncBinder::AddInternal(corba_readonly_attribute_set_LibFunc);
	DeltaLibFuncBinder::AddInternal(corba_attribute_set_LibFunc);
	DeltaLibFuncBinder::AddInternal(corba_attribute_get_LibFunc);
	DeltaLibFuncBinder::AddInternal(corba_deferred_isfinished_LibFunc);
	DeltaLibFuncBinder::AddInternal(corba_deferred_getresponse_LibFunc);
	ToCorbaConverter::AddObjectConversion(ToObjectClientConversion);
	ToDeltaConverter::SetClientObjectConversion(FromObjectClientConversion);
}

static void Cleanup (void) {
	ToCorbaConverter::RemoveObjectConversion(ToObjectClientConversion);
	DeltaLibFuncBinder::RemoveInternal(corba_readonly_attribute_set_LibFunc);
	DeltaLibFuncBinder::RemoveInternal(corba_attribute_set_LibFunc);
	DeltaLibFuncBinder::RemoveInternal(corba_attribute_get_LibFunc);
	DeltaLibFuncBinder::RemoveInternal(corba_deferred_isfinished_LibFunc);
	DeltaLibFuncBinder::RemoveInternal(corba_deferred_getresponse_LibFunc);
}

inline bool VAL_IS_CORBA_CLIENT (const DeltaValue& val)
	{ return val.Type() == DeltaValue_ExternId && val.GetExternIdTypeString() == CORBA_CLIENT_TYPE_STR; }

DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(VAL_IS_CORBA_CLIENT(val), CORBA_NAMESPACE_PREFIX CORBA_CLIENT_TYPE_STR)

DELTALIBFUNC_DECLARECONSTS(3, uarraysize(funcs) - 3, "invoke", "invoke_deferred")

DLIB_INITIALIZE_CLEANUP_FUNCS_EX(
	CorbaClient, 
	"client", 
	CORBA_NAMESPACE_PREFIX,
	Initialize(); DELTALIBFUNC_CREATE_METHODS_TABLE();,
	Cleanup(); DELTALIBFUNC_DESTROY_METHODS_TABLE();
)

DLIB_TOSTRING_FUNC(CorbaClientWrapper, CORBA_CLIENT_TYPE_STR, CORBA_CLIENT_VAL)
DLIB_TOEXTERNID_FUNC_WITH_LIBRARY_METHODS(CorbaClientWrapper, CORBA_CLIENT_TYPE_STR, ToCorbaClient)

//**********************************************************************

static void bind_corba_methods(DeltaValue& externId, CORBA::InterfaceDef_ptr def) {
	DeltaTable*	userData = externId.GetExternIdUserData();
	DASSERT(userData);

	CORBA::InterfaceDef::FullInterfaceDescription *fid = def->describe_interface();
	for (CORBA::ULong i = 0; i < fid->operations.length (); ++i) {
		const std::string name = fid->operations[i].name.in();
		CORBA::ContainedSeq_var ctd_seq = def->lookup_name(name.c_str(), -1, CORBA::dk_Operation, false);
		if (!ctd_seq || ctd_seq->length() != 1)
			continue;	//should probably assert false here but describe_interface is not always consistent with lookup_name.

		DeltaLibraryFuncArgsBinder* binder = DeltaLibraryFuncArgsBinder::New();
		binder->BindArg(externId);
		binder->BindArg(name);
	
		DeltaValue function(corba_invoke_LibFunc, binder);
		bool result = DPTR(userData)->Set(name, function);
		DASSERT(result);
	}

	for (CORBA::ULong i = 0; i < fid->attributes.length (); ++i) {
		const std::string attrName = fid->attributes[i].name.in();
		CORBA::ContainedSeq_var ctd_seq = def->lookup_name(attrName.c_str(), -1, CORBA::dk_Attribute, false);
		if (!ctd_seq || ctd_seq->length() != 1)
			continue;	//should probably assert false here but describe_interface is not always consistent with lookup_name.

		DeltaValue name(attrName);
		DeltaLibraryFuncArgsBinder* setBinder = DeltaLibraryFuncArgsBinder::New();
		setBinder->BindArg(externId);
		setBinder->BindArg(name);
		DeltaValue set(
			fid->attributes[i].mode == CORBA::ATTR_READONLY ? 
				corba_readonly_attribute_set_LibFunc : corba_attribute_set_LibFunc,
			setBinder
		);

		DeltaLibraryFuncArgsBinder* getBinder = DeltaLibraryFuncArgsBinder::New();
		getBinder->BindArg(externId);
		getBinder->BindArg(name);
		DeltaValue get(corba_attribute_get_LibFunc, getBinder);

		userData->NewAttribute(name.ToString(), set, get);
		get(); //get initial value for visualization in the debugger
		if (DELTA_SHOULD_UNWIND_DUE_TO_EXCEPTION())
			return;
	}

	try {
		DeltaValue metadata;
		ToDeltaConverter::convert(&metadata, *def);
		userData->Set("metadata", metadata);
	}
	catch(const ConversionException&) { DASSERT(false); }	//should never have a conversion exception here
}

//**********************************************************************

static bool ToObjectClientConversion(CORBA::Any* output, DeltaValue* input) {
	std::string type;
	void *val;
	CorbaClientWrapper *corbaClient;
	if (input->Type() == DeltaValue_ExternId && (val = input->ToExternId(type)) &&
		type == CORBA_CLIENT_TYPE_STR && (corbaClient = CORBA_CLIENT_VAL(val))
	) {
		*output <<= corbaClient->GetCorbaObject();
		return true;
	}
	else
		return false;
}

static bool FromObjectClientConversion(DeltaValue* output, CORBA::Object_ptr obj) {
	if (CORBA::is_nil(obj))
		output->FromNil();
	else {
		DeltaValue corbaObject;
		ToCorbaClient(&corbaObject, DNEWCLASS(CorbaClientWrapper, (obj)));
		const std::string id = obj->_is_local() ? obj->_interface_repository_id() : obj->_repository_id();
		if (CORBA::InterfaceDef_ptr interface = corba::GetInterface(id.c_str()))
			bind_corba_methods(corbaObject, interface);
		output->Assign(corbaObject);
	}
	return true;
}

/////////////////////////////////////////////////////////////////////
// CORBA Attribute internal libfuncs

DLIB_FUNC_START(corba_readonly_attribute_set, 3, Empty)
	DLIB_GET_CORBA_CLIENT;
	DLIB_ARG(const char*, attribute);
	DeltaString* str = DeltaString::New();
	ToString(str, DLIB_ARGVAL(corbaClientVal));
	DPTR(vm)->PrimaryError("%s: Cannot assign to readonly attribute %s", str->Content().c_str(), DLIB_ARGVAL(attribute));
	DeltaString::Delete(str);
DLIB_FUNC_END

//**********************************************************************

DLIB_FUNC_START(corba_attribute_set, 3, Empty)
	DLIB_GET_CORBA_CLIENT;
	DLIB_ARG(const char*, attribute);
	const char* attributeName = DLIB_ARGVAL(attribute);

	try {
		CORBA::Object_ptr target = corbaClientWrapper->GetCorbaObject();
		DASSERT(target);

		CORBA::InterfaceDef_ptr intf_def = corba::GetInterface(target->_repository_id());
		DASSERT(intf_def);

		CORBA::ContainedSeq_var ctd_seq = intf_def->lookup_name(attributeName, -1, CORBA::dk_Attribute, false);
		DASSERT(ctd_seq && ctd_seq->length() == 1);

		CORBA::AttributeDef_ptr attr_def = CORBA::AttributeDef::_narrow(ctd_seq.in()[0]);
		DASSERT(attr_def);

		CORBA::TypeCode_ptr tc = attr_def->type();
		DASSERT(tc->kind() != CORBA::tk_void);

		const std::string operation = std::string("_set_") + attributeName;
		CORBA::Request_ptr request = target->_request(operation.c_str());
		CORBA::Any& any = request->add_in_arg();
		ToCorbaConverter::convert(&any, DPTR(vm)->GetActualArg(2), tc);
		request->invoke();

		// Set the value to have updated data for the visualization in the debugger
		DeltaTable*	userData = DPTR(vm)->GetActualArg(0)->GetExternIdUserData();
		DASSERT(userData);
		userData->SetAttribute(attributeName, *DPTR(vm)->GetActualArg(2));
	}
	catch(const ConversionException& e) {
		DPTR(vm)->PrimaryError("%s", e.what());
		DLIB_RESET_RETURN;
	}
	CATCH_CORBA_EXCEPTION_INSIDE_LIBRARY_FUNC
DLIB_FUNC_END

//**********************************************************************

DLIB_FUNC_START(corba_attribute_get, 2, Nil)
	DLIB_GET_CORBA_CLIENT;
	DLIB_ARG(const char*, attribute);
	const char* attributeName = DLIB_ARGVAL(attribute);

	try {
		CORBA::Object_ptr target = corbaClientWrapper->GetCorbaObject();
		DASSERT(target);

		CORBA::InterfaceDef_ptr intf_def = corba::GetInterface(target->_repository_id());
		DASSERT(intf_def);

		CORBA::ContainedSeq_var ctd_seq = intf_def->lookup_name(attributeName, -1, CORBA::dk_Attribute, false);
		DASSERT(ctd_seq && ctd_seq->length() == 1);

		CORBA::AttributeDef_ptr attr_def = CORBA::AttributeDef::_narrow(ctd_seq.in()[0]);
		DASSERT(attr_def);

		const std::string operation = std::string("_get_") + attributeName;
		CORBA::Request_ptr request = target->_request(operation.c_str());
		request->set_return_type(attr_def->type());
		request->invoke();

		DeltaValue value;
		try { ToDeltaConverter::convert(&value, request->result()->value()); }
		catch(const ConversionException&) { throw CORBA::MARSHAL(0, CORBA::COMPLETED_YES); }

		// Set the value to have updated data for the visualization in the debugger
		DeltaTable*	userData = DPTR(vm)->GetActualArg(0)->GetExternIdUserData();
		DASSERT(userData);
		userData->SetAttribute(attributeName, value);

		DLIB_RETVAL_REF.Assign(value);
	}
	CATCH_CORBA_EXCEPTION_INSIDE_LIBRARY_FUNC
DLIB_FUNC_END

/////////////////////////////////////////////////////////////////////
// CORBA Client libfuncs

static void GetCorbaObject(DeltaVirtualMachine *vm, const char *CURR_FUNC, CORBA::Object_ptr (*convertFunc)(const char *)) {
	util_ui32 _argNo = 0;
	std::string _sig1, _sig2;
	DeltaNilReset reseter(vm);

	DLIB_ARG(const char*, str);
	ERROR_IF_CORBA_NOT_INITIALIZED;
	try {
		CORBA::Object_ptr obj = (*convertFunc)(DLIB_ARGVAL(str));
		try {
			DeltaValue value;
			ToDeltaConverter::convert(&value, obj);
			DLIB_RETVAL_REF.Assign(value);
		}
		catch(const ConversionException&) { DASSERT(false); }	//should never have a conversion exception here
	}
	CATCH_CORBA_EXCEPTION_INSIDE_LIBRARY_FUNC
}

static void corba_connect_LibFunc(DeltaVirtualMachine* vm)
	{ GetCorbaObject(vm, "corba_connect", &corba::Resolve); }

static CORBA::Object_ptr GetCorbaObjectFromIOR(const char *ior)
	{ return corba::GetORB()->string_to_object(ior); }

static void corba_connect_by_ior_LibFunc(DeltaVirtualMachine* vm)
	{ GetCorbaObject(vm, "corba_connect_by_ior", &GetCorbaObjectFromIOR); }

static void corba_resolve_initial_references_LibFunc(DeltaVirtualMachine* vm)
	{ GetCorbaObject(vm, "corba_resolve_initial_references", &corba::ResolveInitialReferences); }

//**********************************************************************

static void HandleOutAndInoutParameters(CORBA::NVList_ptr arglist, const std::list<DeltaValue>& outAndInoutArgs) {
	std::list<DeltaValue>::const_iterator iter = outAndInoutArgs.begin();
	for (CORBA::ULong i = 0; i < arglist->count(); ++i) {
		CORBA::NamedValue_ptr nv = arglist->item(i);
		if (nv->flags() == CORBA::ARG_OUT || nv->flags() == CORBA::ARG_INOUT) {
			DASSERT(iter != outAndInoutArgs.end());
			DeltaValue object = *iter++;
			DASSERT(object.Type() == DeltaValue_Table);

			CORBA::Any *arg = nv->value();
			DeltaValue value;
			try { ToDeltaConverter::convert(&value, arg); }
			catch(const ConversionException&) { throw CORBA::MARSHAL(0, CORBA::COMPLETED_YES); }
			object.ToTable()->Set((DeltaNumberValueType) 0, value);
		}
	}
	DASSERT(iter == outAndInoutArgs.end());
}

static void HandleResult(DeltaValue *result, CORBA::Request_ptr request) {
	try {
		DeltaValue value;
		ToDeltaConverter::convert(&value, request->result()->value());
		result->Assign(value);
	}
	catch(const ConversionException&) { throw CORBA::MARSHAL(0, CORBA::COMPLETED_YES); }
}

static void PerformInvocation(DeltaVirtualMachine* vm, const char *CURR_FUNC, bool deferedCall) {
	util_ui32 _argNo = 0;
	std::string _sig1, _sig2;
	DeltaNilReset reseter(vm);

	if (DPTR(vm)->TotalActualArgs() < 2) {
		DPTR(vm)->PrimaryError(DEFORMAT_MOREARGSNO, 2, DPTR(vm)->TotalActualArgs(), CURR_FUNC);
		DLIB_RESET_RETURN;
	}

	DLIB_GET_CORBA_CLIENT;
	DLIB_ARG(const char*, function);
	ERROR_IF_CORBA_NOT_INITIALIZED;

	const char* functionName = DLIB_ARGVAL(function);

	try {
		CORBA::ORB_ptr orb = corba::GetORB();
		CORBA::Object_ptr target = corbaClientWrapper->GetCorbaObject();
		DASSERT(target);

		CORBA::InterfaceDef_ptr intf_def = corba::GetInterface(target->_repository_id());
		DASSERT(intf_def);

		CORBA::ContainedSeq_var ctd_seq = intf_def->lookup_name(functionName, -1, CORBA::dk_Operation, false);
		if (!ctd_seq || ctd_seq->length() != 1) {
			DeltaString* str = DeltaString::New();
			ToString(str, DLIB_ARGVAL(corbaClientVal));
			DPTR(vm)->PrimaryError("%s does not have a method '%s'", str->Content().c_str(), functionName);
			DeltaString::Delete(str);
			DLIB_RESET_RETURN;
		}
		CORBA::OperationDef_var op_def = CORBA::OperationDef::_narrow(ctd_seq.in()[0]);
		DASSERT(op_def);

		CORBA::NVList_var arglist;
		orb->create_operation_list(op_def.in(), arglist.out());

		std::list<DeltaValue> outAndInoutArgs;
		const CORBA::ULong totalFormals = arglist->count();
		for (CORBA::ULong i = 0; i < totalFormals; ++i) {
			CORBA::NamedValue_ptr nv = arglist->item(i);
			if (i + 2 >= DPTR(vm)->TotalActualArgs()) {
				DPTR(vm)->PrimaryError(DEFORMAT_MOREARGSNO, totalFormals, DPTR(vm)->TotalActualArgs() - 2, functionName);
				DLIB_RESET_RETURN;
			}
			DeltaValue object = *DPTR(vm)->GetActualArg(i + 2);
			if (nv->flags() == CORBA::ARG_OUT) {
				if (object.Type() != DeltaValue_Table) {
					DPTR(vm)->PrimaryError("corba out " DEFORMAT_ARGTYPE, i, object.TypeStr(), "Table", functionName);
					DLIB_RESET_RETURN;
				}
				else {
					outAndInoutArgs.push_back(object);
					continue;
				}
			}
			else if (nv->flags() == CORBA::ARG_INOUT) {
				if (object.Type() != DeltaValue_Table) {
					DPTR(vm)->PrimaryError("corba inout " DEFORMAT_ARGTYPE, i, object.TypeStr(), "Table", functionName);
					DLIB_RESET_RETURN;
				}
				outAndInoutArgs.push_back(object);
				object.ToTable()->Get((DeltaNumberValueType) 0, &object);
			}
			CORBA::Any *arg = nv->value();
			CORBA::TypeCode_ptr tc = arg->type();
			try { ToCorbaConverter::convert(arg, &object, tc); }
			catch(const ConversionException& e) {
				DPTR(vm)->PrimaryError("arg %d cannot be converted to %s in %s", i, e.what(), functionName);
				DLIB_RESET_RETURN;
			}
		}

		CORBA::NamedValue_var ret_nv;
		orb->create_named_value(ret_nv.out());

		CORBA::ExceptionList_var exc_list;
		CORBA::ExceptionDefSeq_var exc_seq = op_def->exceptions();
		if (exc_seq->length() > 0) {
			orb->create_exception_list (exc_list.out());
			for (CORBA::ULong i = 0; i < exc_seq->length(); ++i) {
				CORBA::TypeCode_var exc_tc = exc_seq[i]->type();
				exc_list->add(exc_tc.in());
			}
		}

		CORBA::Request_var request;
		target->_create_request(CORBA::Context::_nil(), functionName, arglist.in(), ret_nv.in(),
			exc_list.in(), CORBA::ContextList::_nil(),	request.out(), 0);

		bool isVoid = op_def->result()->kind() == CORBA::tk_void;
		if (!isVoid)
			request->set_return_type(op_def->result());

		if (op_def->mode() == CORBA::OP_ONEWAY)
			request->send_oneway();
		else if(deferedCall) {
			request->send_deferred();
			CorbaDeferredCall *call = DNEWCLASS(CorbaDeferredCall,
				(CORBA::Request::_duplicate(request.in()), outAndInoutArgs, isVoid));
			DeltaValue externId;
			call->ToValue(&externId);
			DeltaTable *userData = externId.GetExternIdUserData();

			DeltaLibraryFuncArgsBinder* isFinishedBinder = DeltaLibraryFuncArgsBinder::New();
			isFinishedBinder->BindArg(externId);
			DeltaValue isfinished(corba_deferred_isfinished_LibFunc, isFinishedBinder);
			userData->Set("isfinished", isfinished);
			
			DeltaLibraryFuncArgsBinder* getResponseBinder = DeltaLibraryFuncArgsBinder::New();
			getResponseBinder->BindArg(externId);
			DeltaValue getresponse(corba_deferred_getresponse_LibFunc, getResponseBinder);
			userData->Set("getresponse", getresponse);

			DLIB_RETVAL_REF.Assign(externId);
			return;
		}
		else
			request->invoke();

		HandleOutAndInoutParameters(arglist, outAndInoutArgs);
		if (!isVoid)
			HandleResult(DLIB_RETVAL_PTR, request.in());
	}
	CATCH_CORBA_EXCEPTION_INSIDE_LIBRARY_FUNC
}

static void corba_invoke_LibFunc(DeltaVirtualMachine* vm)
	{ PerformInvocation(vm, "corba_invoke", false); }

static void corba_invoke_deferred_LibFunc(DeltaVirtualMachine* vm)
	{ PerformInvocation(vm, "corba_invoke_deferred", true); }

/////////////////////////////////////////////////////////////////////
// CORBA Deferred Call internal libfuncs

DLIB_FUNC_START(corba_deferred_isfinished, 1, Nil)
	DLIB_GET_CORBA_DEFERRED_CALL;
	try {
		CORBA::Request_ptr request = call->GetRequest();
		if (call->IsFinished())
			DLIB_RETVAL_REF.FromBool(true);
		else if (request->poll_response()) {
			call->SetFinished();
			DLIB_RETVAL_REF.FromBool(true);
		}
		//else if (request->env()->exception())
		//BUG: We should be able to check that an exception occured using the above test.
		//Unfortunately, TAO neither provides the env method nor handles exceptions during deferred calls.
		//This means that there will never be a response for a deferred call causing an exception so the
		//client program will probably block trying to check for finished or trying to get the response.
		else
			DLIB_RETVAL_REF.FromBool(false);
	}
	CATCH_CORBA_EXCEPTION_INSIDE_LIBRARY_FUNC
DLIB_FUNC_END

//**********************************************************************

DLIB_FUNC_START(corba_deferred_getresponse, 1, Nil)
	DLIB_GET_CORBA_DEFERRED_CALL;
	try {
		CORBA::Request_ptr request = call->GetRequest();
		request->get_response();
		HandleOutAndInoutParameters(request->arguments(), call->GetOutAndInoutArgs());
		if (!call->IsVoid())
			HandleResult(DLIB_RETVAL_PTR, request);
	}
	CATCH_CORBA_EXCEPTION_INSIDE_LIBRARY_FUNC
DLIB_FUNC_END