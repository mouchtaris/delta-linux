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

#define	PRIVATE_ParseParms_MEMBERS												\
	DeltaSymbol*				currFunctionStack[DELTA_MAXNESTED_FUNCTIONS];	\
	util_ui32					currLine;										\
	util_i16					topFunc;										\
	util_ui16					globalDataSize;									\
	util_ui16					globalBlocks;									\
	bool						inFormalArgs;									\
	util_ui16					currFuncSerial;									\
	util_i8						currLevel;										\
	NestedState					inTableExpr;									\
	NestedState					inAssertStmt;									\
	NestedState					currScope;										\
	NestedState					inLoop;											\
	std::list<util_ui32>*		unindexedElemOrder;								\
	std::list<DeltaExpr*>*		newTableStack;									\
	std::list<DeltaSymboList>*	controlFlowInitialisationsStack;				\
	std::list<std::string>*		namespacePath;									\
	void EnteringControlFlowStmt (void);										\
	void ExitingControlFlowStmt (bool pop, bool reset, bool possibly = true);	\

#endif	// Do not add stuff beyond this point.
