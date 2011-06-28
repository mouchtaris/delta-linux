// DebugExprParseActions.h
// Parse actions for the debug expression evaluator.
// ScriptFighter Project.
// A. Savidis, February 2002.
//

#ifndef	DEBUGEXPRPARSEACTIONS_H
#define	DEBUGEXPRPARSEACTIONS_H

#include "DeltaVirtualMachine.h"
#include "DeltaValue.h"
#include "DebugEvaluatedExpr.h"

#include <map>
#include <string>

#define	CHECK_ERROR_EXIT_RET(ret)				\
	if (HasError())								\
		return ret;

#define	CHECK_ERROR_EXIT()						\
	CHECK_ERROR_EXIT_RET((DebugEvaluatedExpr*) 0)

#define	CHECK_DEBUGINFO_INCLUDED()				\
	if (!DPTR(targetVM)->HasDebugInfo()) {		\
		Error("No debug information!");			\
		return (DebugEvaluatedExpr*) 0;			\
	}

/////////////////////////////////////////////////////////////

#define	DEBUG_LIBRARYFUNC_2VM					"_2vm_"
#define	DEBUG_LIBRARYFUNC_GETVALUETAG			"_getvaluetag_"
#define	DEBUG_LIBRARYFUNC_EXTERNIDTYPE			"_externidtype_"
#define	DEBUG_LIBRARYFUNC_ISUNDEFINED			"_isundefined_"
#define	DEBUG_LIBRARYFUNC_TYPEOF				"_typeof_"

/////////////////////////////////////////////////////////////

enum DebugExprRelationalOp {

	DebugExprRelOp_GT = 0,
	DebugExprRelOp_LT = 1,
	DebugExprRelOp_GE = 2,
	DebugExprRelOp_LE = 3,
	DebugExprRelOp_EQ = 4,
	DebugExprRelOp_NE = 5
};

/////////////////////////////////////////////////////////////

enum DebugExprArithmeticOp {
	DebugExpArithOp_ADD = 0,
	DebugExpArithOp_SUB = 1,
	DebugExpArithOp_DIV = 2,
	DebugExpArithOp_MUL = 3,
	DebugExpArithOp_MOD = 4
};

/////////////////////////////////////////////////////////////

class DebugExprParseActions {

	private:
	typedef DeltaValue* (*ReservedIndexGetter)(DeltaValue* table);

	static DeltaVirtualMachine*							targetVM;
	static char											errorText[1024];
	static bool											hasError;
	static DebugEvaluatedExpr*							resultExpr;
	static std::list<DeltaValue*>*						tempObjects;

	//**********************************

	typedef std::map<std::string, ReservedIndexGetter>	ReservedAttrMap;
	static ReservedAttrMap*								reservedAttrs;

	static void			InstallReservedTableAttributeGetters(void);
	static void			CleanUpReservedTableAttributeGetters(void);

	static DeltaValue*	GetBases (DeltaValue* table);
	static DeltaValue*	GetDerived (DeltaValue* table);
	static DeltaValue*	GetMostDerived (DeltaValue* table);
	static DeltaValue*	GetDerivedHandler (DeltaValue* table, DeltaObject* (DeltaObject::*f)(void));

	static DeltaValue*	GetDelegates (DeltaValue* table);
	static DeltaValue*	GetDelegators (DeltaValue* table);
	static DeltaValue*	GetOverloadingEnabled (DeltaValue* table);
	static DeltaValue*	GetMadeIn (DeltaValue* table);
	static DeltaValue*	GetDefinedIn (DeltaValue* func);
	static DeltaValue*	GetClosure (DeltaValue* func);
	static DeltaValue*	GetDelegationHandler (DeltaValue* table, DeltaTable* (DeltaTable::*f)(void));

	static DeltaValue*	GetLength (DeltaValue* table);
	static DeltaValue*	GetIndices (DeltaValue* table);
	static DeltaValue*	GetVM (DeltaValue* func);
	static DeltaValue*	GetBoundArgs (DeltaValue* func);

	//**********************************

	typedef DeltaValue* (*UnaryLibraryFunc)(DebugExprList* args);
	typedef std::map<std::string, UnaryLibraryFunc>		LibraryFuncMap;
	static LibraryFuncMap*							libFuncs;

	static void			InstallUnaryLibraryFunctions (void);
	static void			CleanUpUnaryLibraryFunctions (void);
	static DeltaValue*	CallTypeOf (DebugExprList* args);
	static DeltaValue*	CallIsUndefined(DebugExprList* args);
	static DeltaValue*	CallExternIdType (DebugExprList* args);
	static DeltaValue*	CallGetValueTag (DebugExprList* args);

	static DeltaValue*	CallToObject (DebugExprList* args);
	static DeltaValue*	CallToVM (DebugExprList* args);
	static DeltaValue*	CallToMethod (DebugExprList* args);
	static DeltaValue*	CallToFunction (DebugExprList* args);
	static DeltaValue*	CallToExternId (DebugExprList* args);
	static DeltaValue*	CallToLibraryFunc (DebugExprList* args);

	//**********************************
	
	typedef std::map<std::string, DeltaValue*>		MonitorMap;
	static MonitorMap*								monitors;

	static void					InitialiseMonitors (void);
	static void					CleanUpMonitors (void);
	static DebugEvaluatedExpr*	NewMonitor (const std::string& id, DebugEvaluatedExpr* expr);
	static DebugEvaluatedExpr*	DeleteMonitor (const std::string& id);
	static DebugEvaluatedExpr*	GetMonitor (const std::string& id);

	//**********************************

	static DeltaValue*			NewTempValue (void);
	static void					ClearTempObjects (void);

	static bool					IsSingletonCreated (void) 
									{ return tempObjects && reservedAttrs && libFuncs; }

	public:

	/////////////////////////////////////////////////////////////
	// Initialise is called at the ebeginning with the target VM
	// where the named objects will looked.
	//
	static void					Initialise (DeltaVirtualMachine* vm);							
	static void					CleanUp (void);

	static void					SingletonCreate (void);
	static void					SingletonDestroy (void);

	/////////////////////////////////////////////////////////////

	static void					Error (const char* format,...);							
	static bool					HasError (void);						
	static const std::string	GetLastError (void);

	/////////////////////////////////////////////////////////////

	static bool					Manage_RelationalExprForTables (
									DeltaValue*			arg1,
									DeltaValue*			arg2, 
									DebugExprRelationalOp	op
								);

	static DebugEvaluatedExpr*	Manage_RelationalExpr (
									DebugEvaluatedExpr*		arg1,
									DebugEvaluatedExpr*		arg2, 
									DebugExprRelationalOp	op
								);

	static DebugEvaluatedExpr*	Manage_ExprANDExpr (
									DebugEvaluatedExpr*	arg1,
									DebugEvaluatedExpr*	arg2
								);

	static DebugEvaluatedExpr*	Manage_ExprORExpr (
									DebugEvaluatedExpr*	arg1,
									DebugEvaluatedExpr*	arg2
								);

	/////////////////////////////////////////////////////////////

	static DebugEvaluatedExpr*	Manage_ArithmeticExpr (
									DebugEvaluatedExpr*		arg1,
									DebugEvaluatedExpr*		arg2,
									DebugExprArithmeticOp	op
								);

	static DebugEvaluatedExpr*	Manage_UMINUSExpression (DebugEvaluatedExpr* arg);
	static DebugEvaluatedExpr*	Manage_NOTExpression (DebugEvaluatedExpr* arg);

	/////////////////////////////////////////////////////////////

	static DebugEvaluatedExpr*	Manage_ConstValue (DeltaNumberValueType val);
	static DebugEvaluatedExpr*	Manage_ConstValue (void);	// Nil
	static DebugEvaluatedExpr*	Manage_ConstValue (bool val);
	static DebugEvaluatedExpr*	Manage_ConstValue (char* val);
	static char*				Manage_Name (char* s1, char* s2);
	static char*				Manage_AttributeIndex (char* s);

	/////////////////////////////////////////////////////////////

	static DebugExprList*		Manage_ExpressionList (DebugEvaluatedExpr* expr);
	static DebugExprList*		Manage_ExpressionList (DebugExprList* elist, DebugEvaluatedExpr* expr);
	static DebugEvaluatedExpr*	Manage_UnaryLibraryFuncCall (const char* func, DebugExprList* args);

	static DebugEvaluatedExpr*	Manage_AssignLvalue (DebugEvaluatedExpr* lvalue, DebugEvaluatedExpr* rvalue);
	static DebugEvaluatedExpr*	Manage_AnyLvalue (const std::string& id);
	static DebugEvaluatedExpr*	Manage_LocalLvalue (const std::string& id);
	static DebugEvaluatedExpr*	Manage_FormalLvalue (const std::string& id);
	static DebugEvaluatedExpr*	Manage_GlobalLvalue (const std::string& id, bool isStatic);
	static DeltaValue*			Manage_GlobalFunction (DeltaVirtualMachine* vm, const std::string& id);

	static DebugEvaluatedExpr*	Manage_TableContent_ReservedAttr (DebugEvaluatedExpr* table, const char* index);
	static DebugEvaluatedExpr*	Manage_TableContent (DebugEvaluatedExpr* table, const char* index, bool isLocal);
	static DebugEvaluatedExpr*	Manage_TableContent (DebugEvaluatedExpr* table, DebugEvaluatedExpr* index, bool isLocal);
	static bool					Manage_ExternIdContent (DeltaValue* val, const DeltaValue& index, DeltaValue* result);

	static DebugEvaluatedExpr*	Manage_NewMonitor (const std::string& id, DebugEvaluatedExpr* expr)
									{ return NewMonitor(id, expr); }
	static DebugEvaluatedExpr*	Manage_DeleteMonitor (const std::string& id)
									{ return DeleteMonitor(id); }
	static DebugEvaluatedExpr*	Manage_MONITOR (const std::string& id)
									{ return GetMonitor(id); }

	static DebugEvaluatedExpr*	Manage_ATTRIBUTE (const std::string& id);
	static DebugEvaluatedExpr*	Manage_LAMBDA (void);

	/////////////////////////////////////////////////////////////

	static void					SetResultExpression (DebugEvaluatedExpr* obj);
	static DebugEvaluatedExpr*	GetResultExpression (void);

	/////////////////////////////////////////////////////////////
};

#endif	// Do not add stuff beyond this point.
