// Block.cpp
// Block entry / exit management. The information
// is used only when we have a debugger linked.
// A. Savidis, February 2005.
//

#include "Block.h"
#include "DeltaVirtualMachine.h"
#include "Execute.h"
#include "ContainerDelayedCollector.h"
#include "DeltaClosureHandling.h"

////////////////////////////////////////////////////////////////////////////////

void DeltaVirtualMachine::BlockEnterImpl (util_ui16 blockId, util_ui16 totalLocals, bool isMainProgramBlock) {
	GetBlocks().EnterBlock(blockId, totalLocals);
	if (DoEnterBlock(totalLocals))
		GetDebuggerRef().OnBlockEnter(isMainProgramBlock, blockId);
}

//************************************

void DeltaVirtualMachine::BlockEnterImplWithoutDebugger (util_ui16 blockId, util_ui16 totalLocals, bool isMainProgramBlock) {
	GetBlocks().EnterBlock(blockId, totalLocals);
	DoEnterBlock(totalLocals);
}

//************************************

void DeltaVirtualMachine::BlockExitImpl (util_ui16 blockId, util_ui16 totalLocals, bool isMainProgramBlock) {
	if (isMainProgramBlock) 	// We never exit the the main program block.
		DASSERT(blockId == DELTA_MAIN_BLOCK_VALUE);
	else {
		GetBlocks().ExitBlock(blockId, totalLocals);
		DoExitBlock(totalLocals);
	}

	GetDebuggerRef().OnBlockExit(isMainProgramBlock, blockId);
}

//************************************

void DeltaVirtualMachine::BlockExitImplWithoutDebugger (util_ui16 blockId, util_ui16 totalLocals, bool isMainProgramBlock) {
	if (isMainProgramBlock) 	// We never exit the main program block.
		DASSERT(blockId == DELTA_MAIN_BLOCK_VALUE);
	else {
		GetBlocks().ExitBlock(blockId, totalLocals);
		DoExitBlock(totalLocals);
	}
}

////////////////////////////////////////////////////////////////////////////////

#define	BLOCK_ACTIONS(_call)																	\
	DASSERT(																					\
		instr->DELTA_BLOCK_OPERAND_TOTAL_LOCALS.IsInternal()							&&		\
		instr->DELTA_BLOCK_OPERAND_IS_MAIN_BLOCK.GetType() ==  DeltaOperand_ConstBool	&&		\
		instr->DELTA_BLOCK_OPERAND_BLOCKID.IsInternal()											\
	);																							\
	DPTR(vm)->_call(																			\
		(util_ui16) instr->DELTA_BLOCK_OPERAND_BLOCKID.GetValue(),								\
		(util_ui16) instr->DELTA_BLOCK_OPERAND_TOTAL_LOCALS.GetValue(),							\
		instr->DELTA_BLOCK_OPERAND_IS_MAIN_BLOCK.GetValue() != 0								\
	);																							\

//************************************

void Execute_BLOCKENTER (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ BLOCK_ACTIONS(BlockEnterImpl); }

void Execute_BLOCKENTERWithoutDebugger (DeltaInstruction* instr, DeltaVirtualMachine* vm)
	{ BLOCK_ACTIONS(BlockEnterImplWithoutDebugger); }

void Execute_BLOCKEXIT (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ BLOCK_ACTIONS(BlockExitImpl); }

void Execute_BLOCKEXITWithoutDebugger (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ BLOCK_ACTIONS(BlockExitImplWithoutDebugger); }

////////////////////////////////////////////////////////////////////////////////

bool DeltaVirtualMachine::AreNewBlockLocalsUndefined (util_ui16 totalLocals) {
	DeltaValue* s_top = stack + top;
	while (totalLocals--)
		if ((s_top--)->Type() != DeltaValue_Undefined)
			return false;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Empty blocks are safely ingored.

bool DeltaVirtualMachine::DoEnterBlock (util_ui16 totalLocals) {
	if (!totalLocals)
		return true;
	else
	if (top <= totalLocals)
		{ PrimaryError("Stack overflow in entering block!"); return false; }
	else {
		DASSERT(AreNewBlockLocalsUndefined(totalLocals));
		top -= totalLocals;
		UpdateTopCallTrace(top, topsp);
		if (GetBlocks().HasOnlyGlobalBlocks())
			totalActiveGlobals += totalLocals;
		return true;
	}
}

////////////////////////////////////////////////////////////////////////////////

void DeltaVirtualMachine::DoExitBlock (util_ui16 totalLocals) {
	if (!totalLocals)
		return;
	DELTA_COLLECTOR_ENTER();
	DeltaValue* stop = stack + top;
	CLOSURE_HANDLERS->PopBlock(this, stop + 1, totalLocals);
	for (util_ui32	n = totalLocals; n--;)
		(++stop)->CollectGarbage();
	DELTA_COLLECTOR_EXIT();

	top += totalLocals;
	DASSERT(top <= DELTA_TOP_AFTERGLOBALS_VALUE);
	
	UpdateTopCallTrace(top, topsp);
	if (GetBlocks().HasOnlyGlobalBlocks())
		{ DASSERT(totalActiveGlobals > totalLocals); totalActiveGlobals -= totalLocals; }
}

////////////////////////////////////////////////////////////////////////////////

void DeltaVirtualMachine::DoExitBlocksUntil (util_ui16 blockId) 
	{ DoExitBlock(GetBlocks().ExitBlocksUntil(blockId)); }

void DeltaVirtualMachine::DoExitMultipleBlocks (util_ui16 n)
	{ DoExitBlock(GetBlocks().ExitMultipleBlocks(n)); }

void DeltaVirtualMachine::DoExitAllBlocksOfCurrentContext (void)
	{ DoExitBlock(GetBlocks().ExitAllBlocksOfCurrentContext()); }

////////////////////////////////////////////////////////////////////////////////

