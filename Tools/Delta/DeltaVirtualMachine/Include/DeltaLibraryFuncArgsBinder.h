// DeltaLibraryFuncArgsBinder.h
// A class for wrapping calls to library functions with extra
// arguments (binding) that are accessed first in the list of arguments
// (i.e. they are the last to push on the stack).
// ScriptFigher Project.
// A. Savidis, September 2008.
// Extended, April 2010, supported the treatment of lib functions through
// the binder as methods, so that method suites can be made, used via delegation.
//

#ifndef	DELTALIBRARYFUNCARGSBINDER_H
#define	DELTALIBRARYFUNCARGSBINDER_H

#include "DDebug.h"
#include "DeltaValue.h"
#include "DeltaVirtualMachine.h"
#include "DeltaLibraryObjectCreator.h"
#include "DeltaCollectableContainer.h"

#include <string>
#include <list>

////////////////////////////////////////////////////////////

#define	DELTALIBFUNCARGSBINDER_CLASSID			"DeltaLibraryFuncArgsBinder"
#define	DELTALIBFUNCARGSBINDER_VGET_INST(val)	VGET_INST(DeltaLibraryFuncArgsBinder, (val), DELTALIBFUNCARGSBINDER_CLASSID)

// Requires:
// METHODS_START_INDEX	evaluating to util_ui32
// METHODS_TOTAL		evaluating to util_ui32
// METHODS_FIRST		evaluating to const char*
// METHODS_LAST			evaluating to const char*
//
#define	DELTALIBFUNC_BIND_ALL(funcs, obj)												\
	if (true) {																			\
	DASSERT(!strcmp(funcs[METHODS_START_INDEX].name, METHODS_FIRST));					\
	DASSERT(!strcmp(funcs[METHODS_START_INDEX+METHODS_TOTAL-1].name, METHODS_LAST));	\
	DeltaLibraryFuncArgsBinder::BindAll(												\
		funcs,																			\
		METHODS_START_INDEX,															\
		METHODS_START_INDEX + METHODS_TOTAL,											\
		obj																				\
	);																					\
	} else

// Use this when the previous required consts must be defined as consts.
// Usually when included in namespaces if multiple verisons are needed.
//
#define	DELTALIBFUNC_DECLARECONSTS(start_index, total, first, last) \
	static const util_ui32	METHODS_START_INDEX		= start_index;	\
	static const util_ui32	METHODS_TOTAL			= total;		\
	static const char*		METHODS_FIRST			= first;		\
	static const char*		METHODS_LAST			= last;

////////////////////////////////////////////////////////////

// The checkExpr should assume an argument const DeltaValue& val.
#define	DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(checkExpr, api)			\
	static DeltaObject* methods = (DeltaObject*) 0;										\
	static const char* apiClassName = api;												\
	static bool CallerChecker (const DeltaValue& val)									\
		{ return checkExpr; }

// Requires: DeltaTable* methods, bool CallerChecker(const DeltaValue&).
#define	DELTALIBFUNC_INSTALL_METHODS(funcs)												\
	if (true) {																			\
	DASSERT(!strcmp(funcs[METHODS_START_INDEX].name, METHODS_FIRST));					\
	DASSERT(!strcmp(funcs[METHODS_START_INDEX+METHODS_TOTAL-1].name, METHODS_LAST));	\
	DeltaLibraryFuncArgsBinder::InstallLibraryMethods(									\
		funcs,																			\
		METHODS_START_INDEX,															\
		METHODS_START_INDEX + METHODS_TOTAL,											\
		&CallerChecker,																	\
		methods																			\
	);																					\
	}  else

// Use this macro to set creation information of the method's table object.
#define	DELTALIBFUNC_SET_METHODS_TABLE_CREATION_INFO(methods, apiName)					\
	methods->GetCreationInfo().SetCreatorInformation(									\
		std::string(apiName) + "(library)",												\
		DELTA_CREATION_LINE_HIDDEN,														\
		"is library method table (delegate)"											\
	)																					\

// Requires: funcs, apiName, and what DELTALIBFUNC_INSTALL_METHODS requires.
#define	DELTALIBFUNC_CREATE_METHODS_TABLE()												\
	if (true) {																			\
	methods = DeltaObject::NativeCodeHelpers::NewObject();								\
	DELTALIBFUNC_SET_METHODS_TABLE_CREATION_INFO(methods, apiClassName);				\
	DELTALIBFUNC_INSTALL_METHODS(funcs);												\
	} else

// Requires: DeltaTable* methods.
#define	DELTALIBFUNC_DESTROY_METHODS_TABLE()											\
	DeltaObject::NativeCodeHelpers::Delete(methods)

// Requires: DeltaTable* methods.
#define	DELTALIBFUNC_DELEGATE_METHODS(externId)											\
	DeltaLibraryFuncArgsBinder::DelegateLibraryMethods(externId, methods)

#define	DELTALIBFUNC_DELEGATE_METHODS_EX(externId, methods)								\
	DeltaLibraryFuncArgsBinder::DelegateLibraryMethods(externId, methods)

////////////////////////////////////////////////////////////

class DVM_CLASS DeltaLibraryFuncArgsBinder : public CollectableContainer {

	public:
	typedef bool (*CallerCheckerFunc)(const DeltaValue& val);

	private:
	std::list<DeltaValue>*	listPtr;			// List native pointer value.
	DeltaValue				listVal;			// The original value that is contained.
	CollectableContainer*	listCont;			// The collectable container.
	CallerCheckerFunc		callerChecker;		// Caller checker when invocation as method is supported.

	void							ConvertToStringHandler (
										DeltaString* at,
										void (DeltaValue::*tostring) (DeltaString* at) const
									);

	class CallerCheckerHandler {
		
		private:
		CallerCheckerFunc			f;
		DeltaValue					caller;
		bool						matched;

		public:
		bool				HasCallerChecker (void) const 
								{ return !!f; }
		void 				Set (DeltaObject* val) 
								{ caller.FromTable(val); matched = true; }
		void 				Set (DeltaExternId* val)  
								{ caller.FromExternIdDirectly(val); matched = true; }
		void 				Set (const DeltaValue& val)
								{ caller.Assign(val); matched = true; }
		bool				Check (const DeltaValue& val) 
								{ if ((*f)(val)) { Set(val); return true; } else return false; }
		bool				Matched (void) const 
								{ return matched; }
		const DeltaValue&	GetCaller (void) const 
								{ DASSERT(Matched()); return caller; }

		CallerCheckerHandler (CallerCheckerFunc _f) : f(_f), matched(false) {}
	};

	static bool						CallerCheckerPred (DeltaObject* obj, void* closure);
	CallerCheckerFunc				GetCallerChecker (void) const 
										{ return callerChecker; }
	bool							MatchesCaller (const DeltaValue& val, CallerCheckerHandler* data) const;
	bool							MatchesCallerDirectly (const DeltaValue& val) const;

	DeltaLibraryFuncArgsBinder (const DeltaLibraryFuncArgsBinder& binder);
	DeltaLibraryFuncArgsBinder (CallerCheckerFunc f = (CallerCheckerFunc) 0);

	////////////////////////////////////////////////////////////

	public:

	//**************************
	// Support for bound args.

	util_ui16						GetTotalArgs (void) const 
										{ return (util_ui16) DNULLCHECK(listPtr)->size(); }
	const std::list<DeltaValue>&	GetListNativeValue (void) const
										{ return *DNULLCHECK(listPtr); }
	const DeltaValue&				GetListValue (void) const 
										{ return listVal; }
	void							PushArgs (DeltaVirtualMachine* vm) const;

	void							BindArg (const DeltaValue& arg);
	static void						BindAll (
										 DeltaLibraryObjectCreator::FuncEntry*	funcs,
										 util_ui16								from,
										 util_ui16								to,
										 DeltaValue&							externId
									);

	static void						BindAll (
										 DeltaLibraryObjectCreator::FuncEntry*	funcs,
										 util_ui16								from,
										 util_ui16								to,
										 DeltaExternId*							externId
									);

	static DeltaLibraryFuncArgsBinder*
									New (const DeltaLibraryFuncArgsBinder*	binder = (DeltaLibraryFuncArgsBinder*) 0);

	//**************************
	// Support for treating the associated lib function as a method.

	static bool						PushCallerIfLibraryMethod (
										const DeltaValue&		caller,
										const DeltaValue&		method, 
										DeltaVirtualMachine*	vm,
										bool					bounded
									);
	static bool						IsLibraryMethodMatchingCaller (
										const DeltaValue&		caller,
										const DeltaValue&		method,
										DeltaValue*				actualCaller,
										bool					bounded = false
									);
	static void						InstallLibraryMethods (
										 DeltaLibraryObjectCreator::FuncEntry*	funcs,
										 util_ui16								from,
										 util_ui16								to,
										 CallerCheckerFunc						checker,
										 DeltaTable*							owner
									);

	static void						DelegateLibraryMethods (
										DeltaValue&								externId,
										DeltaTable*								owner
									);
	
	static void						DelegateLibraryMethods (
										DeltaExternId*							externId,
										DeltaTable*								owner
									);

	static DeltaLibraryFuncArgsBinder*
									New (CallerCheckerFunc checker);

	//**************************
	// Virtual functions required for CollectableContainer base class.

	virtual void					ConvertToStringBySkippingUserCode (DeltaString* at, void* closure = (void*) 0);
	virtual void					ConvertToString (DeltaString* at, void* closure = (void*) 0);
	virtual void					ExplicitDestructor (void);
	virtual void					ToValue (DeltaValue* at) 
										{ at->Undefine(); }
	virtual void					DeleteMemory (void) { DDELETE(this); }

	//**************************
	// Virtual functions required for Validatable base class.
	
	virtual const char*				GetExtClassString (void) const 
										{ return DELTALIBFUNCARGSBINDER_CLASSID; }

	DYNAMIC_CAST_SINGLEBASE_INLINE_IMPL(CollectableContainer, DELTALIBFUNCARGSBINDER_CLASSID)
	virtual ~DeltaLibraryFuncArgsBinder();
};

////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
