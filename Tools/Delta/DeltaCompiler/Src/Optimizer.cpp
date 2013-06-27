// Optimizer.cpp
// The optimizer of Delta intermediate code.
// ScriptFigher Project.
// A. Savidis, February 2006.
//

#include "Optimizer.h"
#include "InterCode.h"
#include "CompileOptions.h"
#include "ParseActions.h"

//	#define	OPTIMIZER_DUMP_OUTPUT

#ifdef	OPTIMIZER_DUMP_OUTPUT
#define	OPT_CODE(code)		code
#define	OPT_FP(fp)			fp
#else
#define	OPT_CODE(code)
#define	OPT_FP(fp)			(FILE*) 0
#endif

#define	OPT_SAFE_FPRINTF	if (fp) fprintf
#define	OPT_SAFE_FCLOSE		if (fp) fclose

const std::string Optimizer::OptimizerOutput (void) const {
	const std::string s = COMPOPTIONS.GetSourceFile();
	return s.substr(0, s.length() - 4) + ".opt";
}

#define	OPTIMIZER_OUTPUT	OptimizerOutput().c_str()

///////////////////////////////////////////////////////////////////

Optimizer::Optimizer (ucomponentdirectory* directory) :
	ucomponentdirectoryclient(directory), isLeaderTable((bool*) 0) {}

Optimizer::~Optimizer() {}

///////////////////////////////////////////////////////////////////

void Optimizer::Optimize (void) {
	OptimizeOverBasicBlocks(QUADS);
	unsedTempAssigns.PerformOptimization(QUADS, excludedFromTempElimination, OPTIMIZER_OUTPUT);
	EliminateJumpsToNextInstruction(QUADS);
	RetargetJumpsToDroppedQuads(QUADS);

	// The default is to clean-up, but we may keep the structures
	// for further processing.
	
	DDELARR(isLeaderTable);
	unullify(isLeaderTable);
	basicBlocks.clear();
	excludedFromTempElimination.clear();
	excludedFromCopyPropagation.clear();
}

////////////////////////////////////////////////////////////
// Explicitly exclude  some quads, as those
// will erroneously stripped-off by the optimizaitons algorithm.
// A quad may be excluded multiple times.
//
void Optimizer::ExcludeFromTempElimination (util_ui32 quadNo) {
	if (!IsExcludedFromTempElimination(quadNo))
		excludedFromTempElimination.push_back(quadNo);
}

void Optimizer::ExcludeFromCopyPropagation (util_ui32 quadNo) {
	if (!IsExcludedFromCopyPropagation(quadNo))
		excludedFromCopyPropagation.push_back(quadNo);
}

bool Optimizer::IsExcludedFromTempElimination (util_ui32 quadNo) const {
	return	std::find(
				excludedFromTempElimination.begin(),
				excludedFromTempElimination.end(),
				quadNo
			) != excludedFromTempElimination.end();
}

bool Optimizer::IsExcludedFromCopyPropagation (util_ui32 quadNo) const {
	return	std::find(
				excludedFromCopyPropagation.begin(),
				excludedFromCopyPropagation.end(),
				quadNo
			) != excludedFromCopyPropagation.end();
}

///////////////////////////////////////////////////////////////////
//	In this case we want to compact two very common quads of the form:
//	't1 = a op b' and 'x = t1' 
//	to 'x = a op b' and ' t1 = x'.
//	This transformation precedes the elimination of temporaries.
//
void Optimizer::DoResultTempAssignOptimisation (
		DeltaQuadManager&	quadManager,
		const BasicBlock&	block,
		FILE*				fp
	) {

	OPT_CODE(OPT_SAFE_FPRINTF(
		fp, 
		"Checking Block[%d, %d].\n", 
		block.first, block.second
	));

	for (util_ui32 i = block.first; i<= block.second; ++i) {

		DeltaQuad& curr = quadManager.GetQuad(i);

		// Operation with result on a temp var (except simple assignment)?
		// t = a op b.

		if (DeltaIsModifyingOpcode(curr.opcode) && DPTR(DPTR(curr.result)->sym)->IsTempVar()) {
		
			DeltaQuad& next = quadManager.GetQuad(i+1);

			// Then next quad assigns the temp to the program var?
			// x = t.

			if (next.opcode == DeltaIC_ASSIGN			&&
				!DPTR(next.result->sym)->IsTempVar()	&&
				DPTR(next.arg1)->sym == DPTR(curr.result)->sym) {

				curr.result = next.result;	// 1st becomes 'x = a op b.'
				next.result = next.arg1;	// 2nd becomes 't = t'

				// The next is necessary for code equivalence: following code
				// may still use the temp value which now carries x, not the opposite
				// as it was before.

				next.arg1	= curr.result;	// 2nd becomes 't = x' finally.

				OPT_CODE(OPT_SAFE_FPRINTF(fp, "Optimized result temp copy in instruction %d.\n", i+1));
				++i;	// No need to check again the second quad.
			}
		}
	}
}

///////////////////////////////////////////////////////////////////

void Optimizer::EliminateJumpsToNextInstruction (DeltaQuadManager& quadManager) {
	
	OPT_CODE(FILE* fp = fopen(OPTIMIZER_OUTPUT, "at"));
	OPT_CODE(OPT_SAFE_FPRINTF(fp, "***Eliminating jumps to next instruction: Starting...\n"));

	util_ui32 totalDropped = 0;
	for (util_ui32 i = 0, n = quadManager.Total(); n; --n, ++i) {
		DeltaQuad& q = quadManager.GetQuad(i);
		if (!q.IsDropped() && q.opcode == DeltaIC_JUMP && q.label == i + 1) {
			q.MarkAsDropped();
			++totalDropped;
			OPT_CODE(OPT_SAFE_FPRINTF(fp, "Instruction %d (jump next) dropped.\n", i));
		}	
	}

	OPT_CODE(OPT_SAFE_FPRINTF(
		fp,
		"***Eliminating jumps to next instruction:Done...\nTotal %d out of %d instructions dropped.\n", 
		totalDropped, quadManager.Total()
	));

	OPT_CODE(OPT_SAFE_FCLOSE(fp));
}

///////////////////////////////////////////////////////////////////

void Optimizer::RetargetJumpsToDroppedQuads (DeltaQuadManager& quadManager) {

	OPT_CODE(FILE* fp = fopen(OPTIMIZER_OUTPUT, "at"));

	util_ui32 n = quadManager.Total();
	do {
		DeltaQuad& q = quadManager.GetQuad(--n);
		if (!q.IsDropped() && DeltaIsBranchOpcode(q.opcode))
			while (q.label != quadManager.Total() && quadManager.GetQuad(q.label).IsDropped()) {
				OPT_CODE(OPT_SAFE_FPRINTF(fp,"Instruction %d is a jump to %d that is dropped.\n", n, q.label));
				++q.label;
			}
			
	} while (n);

	OPT_CODE(OPT_SAFE_FCLOSE(fp));
}

///////////////////////////////////////////////////////////////////

void Optimizer::AddNewBasicBlock (util_ui32 leader) {

	if (!isLeaderTable[leader]) {	// We need to avoid multiple entries.
		isLeaderTable[leader] = true;
		basicBlocks.push_back(BasicBlock(leader, leader));
	}
}
	
//**************************

void Optimizer::IdentifyBasicBlocksByLeaders (DeltaQuadManager& quadManager) {

	util_ui32 n = quadManager.Total();

	DASSERT(!isLeaderTable);
	isLeaderTable = DNEWARR(bool, n);
	memset(isLeaderTable, 0, n * sizeof(bool));

	AddNewBasicBlock(0);	// 1st quad is a leader.

	for (util_ui32 i = 1; i < n; ++i) {

		DeltaQuad& quad = quadManager.GetQuad(i);

		if (DeltaIsBranchOpcode(quad.opcode)		|| 
			quad.opcode == DeltaIC_FOREACHCHECKEND	|| 
			quad.opcode == DeltaIC_FOREACHFWD) {
			AddNewBasicBlock(quad.label);	// Every branch target is a leader.
			if (i != n -1 )
				AddNewBasicBlock(i + 1);	// ..and the quad that follows it.
		}
	}
}

//**************************

void Optimizer::ExpandBasicBlocksTillNextLeader (DeltaQuadManager& quadManager) {

	util_ui32 n = quadManager.Total() - 1;

	for	(	std::list<BasicBlock>::iterator i = basicBlocks.begin();
			i != basicBlocks.end();
			++i	)

			// Expand basic block till next leader.

			while (i->second < n && !isLeaderTable[i->second + 1])
				++(i->second);

}

///////////////////////////////////////////////////////////////////
// COPY PROPAGATION.
//

void Optimizer::ClearCarriedBy (DeltaSymbol* var) {

	CarriedByMap::iterator i = carriedBy.find(var);

	if (i != carriedBy.end()) {
		std::for_each(
			i->second.begin(),
			i->second.end(),
			std::bind2nd(ClearCarriesFunctor(), &carries)
		);

		i->second.clear();
		carriedBy.erase(i);
	}
}

//**************************
// Substitutes an X variable expr with a new Y variable expr
// only when X is recored to carry the Y value.
//
DeltaExpr* Optimizer::CopyPropagate (DeltaExpr* expr, util_ui32 quadNo, FILE* fp) {

	if (!expr || !expr->sym || IsExcludedFromCopyPropagation(quadNo))
		return expr;
	else {
		DeltaSymbol* rvalue = DPTR(expr)->sym;
		CarriesMap::iterator i = carries.find(rvalue);

		if (i != carries.end()) {
			OPT_CODE(OPT_SAFE_FPRINTF(
				fp, 
				"In instruction %d, %s -> %s.\n",
				quadNo,
				DPTR(expr->sym)->GetName().c_str(),
				DPTR(i->second)->GetName().c_str()
			));
			return EXPRFACTORY.MakeInternalVar(i->second);
		}
		else
			return expr;
	}
}

//**************************
// All vars carrying other vars (that they may optimize out
// due to copy propagation) must be cancelled out of their block.

void Optimizer::HandleBlockExit (util_ui32 blockId) {
	
	std::list<DeltaSymbol*> inactive;
	for (CarriesMap::iterator i = carries.begin(); i != carries.end(); ++i)
		if (i->second->blockId == blockId)	// An out of scope symbol can't be carried.
			inactive.push_back(i->second);

	for (std::list<DeltaSymbol*>::iterator i = inactive.begin(); i != inactive.end(); ++i)
		ClearCarriedBy(*i);	// Clear its carried by entry too.
}

//**************************

void  Optimizer::HandleAssign (DeltaSymbol* lvalue, DeltaSymbol* rvalue, util_ui32 quadNo) {

	DASSERT(
		lvalue != TRANSLATOR.GetReturnValue()->sym && 
		rvalue != TRANSLATOR.GetReturnValue()->sym
	);

	// Since lvalue is assigned, it is no longer carried
	// by the variables that used to carry it till this point.

	ClearCarriedBy(lvalue);
	if (!IsExcludedFromCopyPropagation(quadNo))	// If lvalue not protected from copy propagation.
		carries[lvalue] = rvalue;	// Then the lvalue carries its rvalue
	else
		ClearCarries(lvalue);		// Else the lvalue carries nothing (we need previous carries).
	carriedBy[rvalue].push_back(lvalue);	
}

//**************************

void Optimizer::DoCopyPropagation (
		DeltaQuadManager&	quadManager,
		const BasicBlock&	block,
		FILE*				fp
	) {

	OPT_CODE(OPT_SAFE_FPRINTF(
		fp, 
		"Checking Block[%d, %d].\n", 
		block.first, block.second
	));

	for (util_ui32 i = block.first; i <= block.second; ++i) {

		DeltaQuad& quad = quadManager.GetQuad(i);

		// Are there any rvalues in this quad that we can be optimised
		// via copy propagation?
		//
		quad.arg1 = CopyPropagate(quad.arg1, i, fp);
		quad.arg2 = CopyPropagate(quad.arg2, i, fp);

		// In the following instructions the result is implicitly 
		// an rvalue as well. So we copy propagate for this too.
		//
		if (quad.opcode == DeltaIC_PUSHARG			|| 
			quad.opcode == DeltaIC_BOUNDEDOBJSET	|| 
			quad.opcode == DeltaIC_OBJNEWSET		|| 
			quad.opcode == DeltaIC_OBJSET			|| 
			quad.opcode == DeltaIC_OBJSETATTR)
			quad.result = CopyPropagate(quad.result, i, fp);
		
		// In case of any sort of mutating instruction 'x= y op x'
		// except 'x = y', x is neither carried nor carries.
		// 
		if (DeltaIsModifyingOpcode(quad.opcode) || quad.opcode == DeltaIC_FOREACHGETVAL || quad.opcode == DeltaIC_FOREACHGETINDEX) {
			ClearCarries(quad.result->sym);
			ClearCarriedBy(quad.result->sym);
		}
		else
		if (quad.opcode == DeltaIC_ASSIGN) {
			if (quad.arg1->GetType() != DeltaExprVar)	{ // Assigning from const value.
				ClearCarries(quad.result->sym);
				ClearCarriedBy(quad.result->sym);
			}
			else
			if (quad.arg1 == quad.result)
				quad.MarkAsDropped();
			else
				HandleAssign(quad.result->sym, quad.arg1->sym, i);
		}
		else
		if (quad.opcode == DeltaIC_GETRETVAL) {
			DASSERT(quad.result->IsTemp());
			ClearCarries(quad.result->sym);
			ClearCarriedBy(quad.result->sym);
		}
		else
		if (quad.opcode == DeltaIC_BLOCKEXIT)
			HandleBlockExit((util_ui32) quad.DELTA_BLOCK_OPERAND_BLOCKID->numConst);
	}

	carries.clear();
	carriedBy.clear();
}

///////////////////////////////////////////////////////////////////

void Optimizer::OptimizeOverBasicBlocks (DeltaQuadManager& quadManager) {

	OPT_CODE(FILE* fp = fopen(OPTIMIZER_OUTPUT, "wt"));

	IdentifyBasicBlocksByLeaders(quadManager);
	ExpandBasicBlocksTillNextLeader(quadManager);

	for (	std::list<BasicBlock>::iterator i = basicBlocks.begin();
			i != basicBlocks.end();
			++i	) {

			DoResultTempAssignOptimisation(quadManager, *i, OPT_FP(fp));
			DoCopyPropagation(quadManager, *i, OPT_FP(fp));
		}

	OPT_CODE(OPT_SAFE_FCLOSE(fp));
}

///////////////////////////////////////////////////////////////////