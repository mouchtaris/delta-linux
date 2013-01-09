// TypeCheck.h
// Basic type checking for the Delta compiler.
// Although Delta is a strongly typed language, there
// cases where the type is known at compile time. In this
// case we should provide error messages to avoid 
// run-time type conflicts or "shadow conversions".
// ScriptFighter Project.
// A. Savidis, October 1999.
// Revised June 2005, adding more clever compile-time
// type checking.
//

#ifndef	TYPECHECK_H
#define	TYPECHECK_H

#include "CompilerComponentDirectory.h"
#include "InterCode.h"
#include "TypeTag.h"
#include "LibraryTypes.h"
#include "Symbol.h"
#include "utypes.h"
#include "Expr.h"
#include <list>

typedef std::list<DeltaLibraryTypeInfo> CallSig;
typedef std::list<DeltaLibraryTypeInfo> DefSig;
typedef	std::list<std::string>			IdSig;

/////////////////////////////////////////////////////////////

class TypeChecker : public ucomponentdirectoryclient {

	public:
	void	Check_ProgramFunctionExactMissingArguments (DeltaSymbol* func,  util_ui32 n);
	void	Check_NumberOfLibraryFunctionActualArguments (DeltaSymbol* func, util_ui32 n);
	bool	Check_LibFunctionArguments (DeltaSymbol* func, const CallSig& c_sig);
	bool	Check_FunctionCall (DeltaExpr* func);
	bool	Check_Table (DeltaExpr* table);
	bool	Check_LateBoundArg (DeltaExpr* table);
	bool	Check_TableIndex (DeltaExpr* index);
	bool	Check_UseAsBoolean (DeltaExpr* expr);
	bool	Check_InRelational (DeltaExpr* expr, bool onlyEquality);
	bool	Check_InRelational (DeltaExpr* e1, DeltaExpr* e2, DeltaICOpcode relOp);
	bool	Check_InArithmetic (DeltaExpr* expr, DeltaICOpcode relOp, const char* opStr, bool alwaysError = true);
	bool	Check_Assign (DeltaExpr* lvalue);
	bool	Check_ForeachContainer (DeltaExpr* container);

	TypeChecker (ucomponentdirectory* directory) : ucomponentdirectoryclient(directory) {}
	~TypeChecker() {}
};

/////////////////////////////////////////////////////////////

#define TYPECHECKER_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), TypeChecker)))

#define TYPECHECKER	TYPECHECKER_EX(GET_COMPONENT_DIRECTORY())

/////////////////////////////////////////////////////////////

#endif	// Do not ad stuff beyond this point.