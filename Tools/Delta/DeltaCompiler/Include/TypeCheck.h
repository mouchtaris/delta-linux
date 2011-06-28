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

extern void TypeCheck_ProgramFunctionExactMissingArguments (DeltaSymbol* func,  util_ui32 n);
extern void TypeCheck_NumberOfLibraryFunctionActualArguments (DeltaSymbol* func, util_ui32 n);
extern bool TypeCheck_LibFunctionArguments (DeltaSymbol* func, const CallSig& c_sig);
extern bool	TypeCheck_FunctionCall (DeltaExpr* func);
extern bool	TypeCheck_Table (DeltaExpr* table);
extern bool	TypeCheck_LateBoundArg (DeltaExpr* table);
extern bool	TypeCheck_TableIndex (DeltaExpr* index);
extern bool	TypeCheck_UseAsBoolean (DeltaExpr* expr);
extern bool	TypeCheck_InRelational (DeltaExpr* expr, bool onlyEquality);
extern bool	TypeCheck_InRelational (DeltaExpr* e1, DeltaExpr* e2, DeltaICOpcode relOp);
extern bool	TypeCheck_InArithmetic (DeltaExpr* expr, DeltaICOpcode relOp, const char* opStr);
extern bool TypeCheck_Assign (DeltaExpr* lvalue);
extern bool TypeCheck_ForeachContainer (DeltaExpr* container);

#endif	// Do not ad stuff beyond this point.