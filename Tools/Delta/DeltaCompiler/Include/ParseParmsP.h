// ParseParmsP.hh.
// Private members for ParseParms class.
// ScriptFigher Project.
// A. Savidis, October 1999.
//

#ifndef	PARSEPARMSP_HH
#define	PARSEPARMSP_HH

#include "DeltaByteCodeTypes.h"
#include "Symbol.h"
#include "unestedstate.h"
#include "Expr.h"

typedef unestedstate<DELTA_MAXNESTED_FUNCTIONS + 1> NestedState;

#define	PRIVATE_ParseParms_MEMBERS													\
	virtual void F(void) =0 ;														\
	static DeltaSymbol*					currFunctionStack[DELTA_MAXNESTED_FUNCTIONS];	\
	static util_i16						topFunc;										\
	static util_ui16					globalDataSize;									\
	static util_ui16					globalBlocks;									\
	static bool							inFormalArgs;									\
	static util_ui16					currFuncSerial;									\
	static util_i8						currLevel;										\
	static bool							productionMode;									\
	static NestedState					inTableExpr;									\
	static NestedState					inAssertStmt;									\
	static NestedState					currScope;										\
	static NestedState					inLoop;											\
	static std::list<util_ui32>*		unindexedElemOrder;								\
	static std::list<DeltaExpr*>*		newTableStack;									\
	static std::list<DeltaSymboList>*	controlFlowInitialisationsStack;				\
	static std::list<std::string>*		namespacePath;									\
	static void EnteringControlFlowStmt (void);											\
	static void ExitingControlFlowStmt (bool pop, bool reset, bool possibly = true);	\

#endif	// Do not add stuff beyond this point.
