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

//////////////////////////////////////////////////////

#define	DELTA_GLOBAL_SCOPE				0
#define	DELTA_TOSTRING_SYMBOLIC_NAME	"@"

//////////////////////////////////////////////////////

class ParseParms {
	private:
	PRIVATE_ParseParms_MEMBERS

	public:
	util_ui32		GetLine (void);
	util_ui32		SetLine (util_ui32 line);
	void			NextLine (void);

	bool			InGlobalScope	(void) const	{ return !currScope.inside(); }
	NestedState&	CurrScope		(void)			{ return currScope; }
	NestedState&	InAssertStmt	(void)			{ return inAssertStmt; }
	NestedState&	InTableExpr		(void)			{ return inTableExpr; }

	util_ui16		CurrFuncSerial (void) const;
	util_ui16		InsideFuncSerial(void) const;
	void			IncFuncSerial (void);

	void			PushFunction (DeltaSymbol* func);
	void			PopFunction (void);
	DeltaSymbol*	CurrFunction (void) const;
	bool			IsOuterFunction (DeltaSymbol* func) const;
	bool			InFunction (void) const;
	bool			InMethod (void) const;
	static bool		IsOperator (const std::string& id);

	util_ui16		GlobalDataSize (void) const;
	void			SetGlobalDataSize (util_ui16 n);
	util_ui16		IncGlobalBlocks (void);
	util_ui16		GetTotalGlobalBlocks (void) const;

	bool			InFormalArgs (void) const;
	void			SetInFormalArgs (bool val);

	enum ConditionValue { CondFalse = 1, CondTrue = 0, CondCantTell = 2 };

	bool			InLoop (void) const;
	void			EnteringLoop (void);
	void			ExitingLoop (ConditionValue condValue = CondCantTell);

	void			EnteringIf (void);
	void			ExitingIf (ConditionValue cond);
	void			ExitingIfFollowedByElse (void);
	void			EnteringElse (void);
	void			ExitingElse (ConditionValue cond);

	void			AssumeToBeInitialised (DeltaSymbol* var);

	void			IncUnindexedElementOrder (void);
	util_ui32		GetUnindexedElementOrder (void);
	void			EnteringTableConstructor (DeltaExpr* table);
	void			ExitingTableConstructor (void);
	DeltaExpr*		GetCurrConstructedTable (void) const;

	typedef std::list<std::string> NameList;
	void			AppendToNamespacePath (const std::string& id);
	void			ClearNamespacePath (void);
	const NameList&	GetNamespacePath (void) const;

	void			Initialise (void);
	void			CleanUp (void);

	ParseParms(void);
	~ParseParms();

};

//////////////////////////////////////////////////////

#define PARSEPARMS_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), ParseParms)))

#define PARSEPARMS	PARSEPARMS_EX(GET_COMPONENT_DIRECTORY())

//////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
