// Optimizer.h
// The optimizer of Delta intermediate code.
// ScriptFigher Project.
// A. Savidis, February 2006.
//

#ifndef	OPTIMIZER_H
#define	OPTIMIZER_H

#include "DDebug.h"
#include "CompilerComponentDirectory.h"
#include "InterCode.h"
#include "Symbol.h"
#include "UnusedTempAssignElimination.h"

#include <list>
#include <algorithm>
#include <functional>
#include <map>

class Optimizer {
	
	USE_COMPILER_COMPONENT_DIRECTORY();

	private:
	////////////////////////////////////////////////////

	UnusedTempAssignElimination unsedTempAssigns;
	std::list<util_ui32>	excludedFromTempElimination;	
	std::list<util_ui32>	excludedFromCopyPropagation;

	////////////////////////////////////////////////////

	typedef std::pair<util_ui32, util_ui32> BasicBlock;	// Start and end quads.
	std::list<BasicBlock>	basicBlocks;
	bool*					isLeaderTable; // Marks leader quads, size equal to total quads.

	const std::string		OptimizerOutput (void) const;

	void					IdentifyBasicBlocksByLeaders (DeltaQuadManager& quadManager); // Produces basic blocks consisting only of leaders.
	void					AddNewBasicBlock (util_ui32 leader);
	void					ExpandBasicBlocksTillNextLeader (DeltaQuadManager& quadManager);

	////////////////////////////////////////////////////
	// Carries<x,y>:	most recent variable y assign to x, meaning x carries y's value.
	// Carried<y,L>:	in L we have all variables x that carry y.
	//
	typedef std::map<DeltaSymbol*, DeltaSymbol*>				CarriesMap;
	typedef std::map<DeltaSymbol*, std::list<DeltaSymbol*> >	CarriedByMap;

	CarriesMap				carries;
	CarriedByMap			carriedBy;

	struct ClearCarriesFunctor : public std::binary_function<DeltaSymbol*, CarriesMap*, void> {
		void operator()(DeltaSymbol* var, CarriesMap* m) const {
			CarriesMap::iterator i = m->find(var);
			if (i != m->end())
				m->erase(i);
		}
	};

	void					ClearCarriedBy (DeltaSymbol* var);
	void					ClearCarries (DeltaSymbol* var)
								{ ClearCarriesFunctor()(var, &carries); }

	//**************************

	DeltaExpr*				CopyPropagate (DeltaExpr* expr, util_ui32 quadNo, FILE* fp);

	void					DoCopyPropagation (
								DeltaQuadManager&		quadManager,
								const BasicBlock&	block,
								FILE*				fp
							);
	
	void					HandleAssign (DeltaSymbol* lvalue, DeltaSymbol* rvalue, util_ui32 quadNo);
	void					HandleBlockExit (util_ui32 blockId);

	////////////////////////////////////////////////////

	void					DoResultTempAssignOptimisation (
								DeltaQuadManager&		quadManager,
								const BasicBlock&	block,
								FILE*				fp
							);

	////////////////////////////////////////////////////

	public:
	bool					IsExcludedFromTempElimination (util_ui32 quadNo) const;
	void					ExcludeFromTempElimination (util_ui32 quadNo);

	bool					IsExcludedFromCopyPropagation (util_ui32 quadNo) const;
	void					ExcludeFromCopyPropagation (util_ui32 quadNo);

	void					OptimizeOverBasicBlocks (DeltaQuadManager& quadManager);
	void					EliminateJumpsToNextInstruction (DeltaQuadManager& quadManager);
	void					RetargetJumpsToDroppedQuads (DeltaQuadManager& quadManager);
	void					Optimize (void);

	////////////////////////////////////////////////////

	Optimizer (void);
	~Optimizer();
};

////////////////////////////////////////////

#define OPTIMIZER_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), Optimizer)))

#define OPTIMIZER	OPTIMIZER_EX(COMPONENT_DIRECTORY())

////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.