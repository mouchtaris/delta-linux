// UnusedTempAssignElimination.h
// Optimization support.
// ScriptFigher Project.
// A. Savidis, February 2006.
//

#ifndef	UNUSEDTEMPASSIGNELIMINATION_H
#define	UNUSEDTEMPASSIGNELIMINATION_H

#include "DDebug.h"
#include "InterCode.h"
#include "Symbol.h"

#include <list>
#include <algorithm>
#include <functional>
#include <map>

class UnusedTempAssignElimination {

	private:
	std::list<DeltaSymbol*>	usefulVars;
	typedef std::list<DeltaSymbol*>::iterator VarIter;
	
	typedef void (UnusedTempAssignElimination::*Handler)(DeltaQuad&, bool);

	void	OnForeachBegin (DeltaQuad& q,  bool exclude);
	void	OnForeachCheckEnd (DeltaQuad& q,  bool exclude);
	void	OnForeachGetValOrIndex (DeltaQuad& q,  bool exclude);
	void	OnForeachFwd (DeltaQuad& q,  bool exclude);
	void	OnForeachEnd (DeltaQuad& q,  bool exclude);
	void	OnAssign (DeltaQuad& q,  bool exclude);
	void	OnGetRetVal (DeltaQuad& q,  bool exclude);
	void	OnRetVal (DeltaQuad& q,  bool exclude);
	void	OnArithmetic (DeltaQuad& q,  bool exclude);
	void	OnPushArg (DeltaQuad& q,  bool exclude);
	void	OnTableGet (DeltaQuad& q,  bool exclude);
	void	OnTableSet (DeltaQuad& q,  bool exclude);
	void	OnObjNewSet (DeltaQuad& q,  bool exclude);
	void	OnObjNewAttr (DeltaQuad& q,  bool exclude);
	void	OnObjNew (DeltaQuad& q,  bool exclude);
	void	OnCallFunc (DeltaQuad& q,  bool exclude);
	void	OnCallMethod (DeltaQuad& q,  bool exclude);
	void	OnBranch (DeltaQuad& q,  bool exclude);
	void	OnAssert (DeltaQuad& q,  bool exclude);
	void	OnThrow (DeltaQuad& q,  bool exclude);

	std::map<util_ui32, Handler> dispatcher;

	VarIter	FindUsefulVar (DeltaSymbol* sym) 
				{ return std::find(usefulVars.begin(), usefulVars.end(), sym); }

	bool	IsUsefulVar (DeltaSymbol* sym)
				{ return FindUsefulVar(sym) != usefulVars.end(); }

	void	AssumeUsefulVar (DeltaSymbol* sym);

	//////////////////////////////////////////////////////////////

	public:
	void	PerformOptimization (
				DeltaQuadManager&				quadManager, 
				const std::list<util_ui32>&		excluded,
				const char*						outputFile
			);

	UnusedTempAssignElimination (void);
	~UnusedTempAssignElimination(){}
};

#endif	// Do not add stuff beyond this point.