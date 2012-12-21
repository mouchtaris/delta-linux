// ASTVisitorLib.cpp
// Visitor for ast traversal.
// ScriptFighter Project.
// Y. Lilis, August 2011.
//

#include "DeltaLibraryCreators.h"
#include "MiscLib.h"
#include "ASTVisitorLib.h"
#include "ASTLib.h"
#include "TreeVisitor.h"

#define	ASTVISITOR_VAL(val) \
			DPTR(VGET_INST(TreeVisitorWrapper, val, ASTVISITOR_TYPE_STR))

/////////////////////////////////////////////////////

static DeltaExternIdFieldGetter* getter = (DeltaExternIdFieldGetter*) 0;

static bool GetKeys (void* val, DeltaValue* at) 
	{ return DPTR(getter)->GetAllKeys(val, at); }

//************************

static bool GetHandlersByTag (DeltaValue* at, TreeVisitorWrapper* wrapper, const std::string& tag) {
	const TreeVisitorWrapper::HandlerMap& handlers = DPTR(wrapper)->GetHandlers();
	TreeVisitorWrapper::HandlerMap::const_iterator begin = handlers.lower_bound(tag);
	TreeVisitorWrapper::HandlerMap::const_iterator end = handlers.upper_bound(tag);
	DASSERT(begin != handlers.end());
	
	DeltaObject* object = DeltaObject::NewObject();
	at->FromTable(object);
	for (unsigned i = 0; begin != end; ++begin, ++i)
		object->Set((DeltaNumberValueType) i, begin->second->first);
	return true;
}

//************************

static bool GetHandlers (void* val, DeltaValue* at) 
	{ return GetHandlersByTag(at, ASTVISITOR_VAL(val), HANDLER_TAG); }

static bool GetContextHandlers (void* val, DeltaValue* at)
	{ return GetHandlersByTag(at, ASTVISITOR_VAL(val), CONTEXT_HANDLER_TAG); }

static bool GetDefaultHandler (void* val, DeltaValue* at)
	{ return GetHandlersByTag(at, ASTVISITOR_VAL(val), DEFAULT_HANDLER_TAG); }

//************************

static bool GetState (void* val, DeltaValue* at) {
	TreeVisitor* visitor = ASTVISITOR_VAL(val)->GetVisitor();
	const char* states[] = { "stop", "leave", "prune", "continue" };
	int state = (int) visitor->GetState();
	DASSERT(state >= 1 && state <= 4);
	at->FromString(states[state - 1]);
	return true;
}

//************************

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ HANDLER_TAG,			&GetHandlers,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ CONTEXT_HANDLER_TAG,	&GetContextHandlers,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ DEFAULT_HANDLER_TAG,	&GetDefaultHandler,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "state",				&GetState,				DELTA_GETBYSTRING_NO_PRECOND	}
};

/////////////////////////////////////////////////////

static void astvisitor_new_LibFunc (DeltaVirtualMachine*);
static void astvisitor_set_handler_LibFunc (DeltaVirtualMachine*);
static void astvisitor_set_context_handler_LibFunc (DeltaVirtualMachine*);
static void astvisitor_set_default_handler_LibFunc (DeltaVirtualMachine*);
static void astvisitor_stop_LibFunc (DeltaVirtualMachine*);
static void astvisitor_leave_LibFunc (DeltaVirtualMachine*);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "new",					astvisitor_new_LibFunc					},
	{ "set_handler",			astvisitor_set_handler_LibFunc			},
	{ "set_context_handler",	astvisitor_set_context_handler_LibFunc	},
	{ "set_default_handler",	astvisitor_set_default_handler_LibFunc	},
	{ "stop",					astvisitor_stop_LibFunc					},
	{ "leave",					astvisitor_leave_LibFunc				}
};

inline bool VAL_IS_ASTVISITOR (const DeltaValue& val)
	{ return val.Type() == DeltaValue_ExternId && val.GetExternIdTypeString() == ASTVISITOR_TYPE_STR; }

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "set_handler", "leave")

DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(VAL_IS_ASTVISITOR(val), ASTVISITOR_TYPE_STR)

DLIB_INITIALIZE_CLEANUP_FUNCS_EX(
	ASTVisitor, 
	"astvisitor",
	DELTA_STDLIB_NAMESPACE_PREFIX "astvisitor_",
	DELTALIBFUNC_CREATE_METHODS_TABLE();,
	DELTALIBFUNC_DESTROY_METHODS_TABLE();
)

static void ASTVisitor_Make (DeltaValue& val) {
	DNEW(TreeVisitorWrapper)->ToValue(&val);
	DELTALIBFUNC_DELEGATE_METHODS(val);
}

void TreeVisitorWrapper::ToValue (DeltaValue* at) {
	at->FromExternId(
		(void*) GetSerialNo(),
		DeltaExternId_CollectableContainer,
		DELTA_EXTERNID_TOSTRINGNULL,
		ASTVISITOR_TYPE_STR,
		getter
	);
}

/////////////////////////////////////////////////////

#define DLIB_ARG_HANDLER(tag, closure)								\
	DLIB_ARG(DeltaValue*, handler)									\
	DLIB_ERROR_CHECK(												\
		!DLIB_ARGVAL(handler)->IsCallable(),							\
		uconstructstr("callable required for the handler argument")	\
	);																\
	TreeVisitorWrapper::Handler* closure = DNEWCLASS(				\
		TreeVisitorWrapper::Handler,								\
		(*DLIB_ARGVAL(handler), visitor)							\
	);																\
	DPTR(visitor)->AddHandler(tag, closure);

static void HandlerWrapper(TreeNode* node, const std::string& childId, bool entering, void* closure) {
	TreeVisitorWrapper::Handler* handler = (TreeVisitorWrapper::Handler*) closure;
	DeltaValue deltaNode;
	DeltaAST_Make(&deltaNode, node);
	//TODO: maybe should also check about exceptions
	DPTR(handler)->first(deltaNode, DeltaValue(childId), DeltaValue(entering));
	if (DeltaVirtualMachine::RunTimeErrorsExist())
		DPTR(DPTR(DPTR(handler)->second)->GetVisitor())->Stop();
}

/////////////////////////////////////////////////////

DLIB_FUNC_START(astvisitor_new, 0, Empty)
ASTVisitor_Make(DLIB_RETVAL_REF);
DLIB_SET_CREATOR_INFORMATION();
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(astvisitor_set_handler, 3, Empty)
_DLIB_GET_ASTVISITOR_WRAPPER_EX_(visitor)
DLIB_ARG(const char*, tag)
DLIB_ARG_HANDLER(HANDLER_TAG, closure)
DPTR(DPTR(visitor)->GetVisitor())->SetHandler(DLIB_ARGVAL(tag), HandlerWrapper, closure);
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(astvisitor_set_context_handler, 4, Empty)
_DLIB_GET_ASTVISITOR_WRAPPER_EX_(visitor)
DLIB_ARG(const char*, parentTag)
DLIB_ARG(const char*, childId)
DLIB_ARG_HANDLER(CONTEXT_HANDLER_TAG, closure)
DPTR(DPTR(visitor)->GetVisitor())->SetContextDependentHandler(
	DLIB_ARGVAL(parentTag),
	DLIB_ARGVAL(childId),
	HandlerWrapper,
	closure
);
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(astvisitor_set_default_handler, 2, Empty)
_DLIB_GET_ASTVISITOR_WRAPPER_EX_(visitor)
DLIB_ARG_HANDLER(DEFAULT_HANDLER_TAG, closure)
DPTR(visitor->GetVisitor())->SetDefaultHandler(HandlerWrapper, closure);
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(astvisitor_stop, 1, Empty)
DLIB_GET_ASTVISITOR
DPTR(visitor)->Stop();
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(astvisitor_leave, 1, Empty)
DLIB_GET_ASTVISITOR
DPTR(visitor)->Leave();
DLIB_FUNC_END

//////////////////////////////////////////////////////
