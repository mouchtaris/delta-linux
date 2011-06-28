// ParseParms.h
// Parsing parameters for Delta compiler.
// ScriptFigher Project.
// A. Savidis, October 1999.
//

#ifndef	PARSEPARMS_H
#define	PARSEPARMS_H

#include "utypes.h"
#include "Symbol.h"
#include "ParseParmsP.h"

#define	DELTA_GLOBAL_SCOPE				0
#define	DELTA_TOSTRING_SYMBOLIC_NAME	"@"

class ParseParms {

	public:
	static util_ui16		CurrLine (void);

	static bool				InGlobalScope (void)	{ return !currScope.inside(); }
	static NestedState&		CurrScope (void)		{ return currScope; }
	static NestedState&		InAssertStmt (void)		{ return inAssertStmt; }
	static NestedState&		InTableExpr(void)		{ return inTableExpr; }

	static util_ui16		CurrFuncSerial (void);
	static util_ui16		InsideFuncSerial(void);
	static void				IncFuncSerial (void);

	static void				PushFunction (DeltaSymbol* func);
	static void				PopFunction (void);
	static DeltaSymbol*		CurrFunction (void);
	static bool				IsOuterFunction (DeltaSymbol* func);
	static bool				InFunction (void);
	static bool				InMethod (void);
	static bool				IsOperator (const std::string& id);

	static util_ui16		GlobalDataSize (void);
	static void				SetGlobalDataSize (util_ui16 n);
	static util_ui16		IncGlobalBlocks (void);
	static util_ui16		GetTotalGlobalBlocks (void);

	static bool				InFormalArgs (void);
	static void				SetInFormalArgs (bool val);

	static bool				InProductionMode (void);
	static void				SetInProductionMode (bool val);

	enum ConditionValue { CondFalse = 1, CondTrue = 0, CondCantTell = 2 };

	static bool				InLoop (void);
	static void				EnteringLoop (void);
	static void				ExitingLoop (ConditionValue condValue = CondCantTell);

	static void				EnteringIf (void);
	static void				ExitingIf (ConditionValue cond);
	static void				ExitingIfFollowedByElse (void);
	static void				EnteringElse (void);
	static void				ExitingElse (ConditionValue cond);

	static void				AssumeToBeInitialised (DeltaSymbol* var);

	static void				IncUnindexedElementOrder (void);
	static util_ui32		GetUnindexedElementOrder (void);
	static void				EnteringTableConstructor (DeltaExpr* table);
	static void				ExitingTableConstructor (void);
	static DeltaExpr*		GetCurrConstructedTable (void);

	typedef std::list<std::string> NameList;
	static void				AppendToNamespacePath (const std::string& id);
	static void				ClearNamespacePath (void);
	static const NameList&	GetNamespacePath (void);

	static void				Initialise (void);
	static void				CleanUp (void);

	private:
	PRIVATE_ParseParms_MEMBERS
};

#endif	// Do not add stuff beyond this point.
