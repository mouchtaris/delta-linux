// TargetCode.cpp
// Traget code generation for Delta compiler.
// ScriptFighter Project.
// A. Savidis, October 1999 (original version).
// Latest editing May 2009.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "DDebug.h"
#include "Symbol.h"
#include "DeltaByteCodeTypes.h"
#include "ParseParms.h"
#include "ParseActions.h"
#include "InterCode.h"
#include "TargetCode.h"
#include "DeltaCompErrorMsg.h"
#include "ustrings.h"
#include "ExprCleaner.h"
#include "DebugNamingForStaticVars.h"
#include "CompilerAPI.h"
#include "DeltaVersionDefs.h"
#include "ufiles.h"
#include "SelectiveStepInPreparator.h"
#include "Symbol.h"
#include "DeltaFunc.h"
#include "LibraryNamespaceHolder.h"
#include "FunctionReturnTypesManager.h"
#include "ulexutil.h"
#include "usystem.h"

#include <list>
#include <new>

//	#define	AUTO_ASSIGN_ZERO_NUMBERED_LINES
#define	NO_DEBUG_INFORMATION_IS_PRODUCED(sym)

//-----------------------------------------------------------------

std::list<DeltaStdFuncInfo>*		DeltaCodeGenerator::funcTable = (std::list<DeltaStdFuncInfo>*) 0;
DeltaCodeGenerator*					DeltaCodeGenerator::singletonPtr = (DeltaCodeGenerator*) 0;

void DeltaCodeGenerator::SingletonCreate (void) {
	unew(funcTable);
	DASSERT(!singletonPtr);
	singletonPtr = DNEW(DeltaCodeGenerator);
}

void DeltaCodeGenerator::SingletonDestroy (void) {
	udelete(singletonPtr);
	udelete(funcTable);
}

/////////////////////////////////////////////

DeltaCodeGenerator::DeltaCodeGenerator (void) {

	code = (DeltaInstruction*) 0;

	generateFuncs[DeltaIC_ADD]						= &Generate_ADD;
	generateFuncs[DeltaIC_OBJGET]					= &Generate_OBJGET;
	generateFuncs[DeltaIC_OBJNEW]					= &Generate_OBJNEW;
	generateFuncs[DeltaIC_OBJNEWSET]				= &Generate_OBJNEWSET;
	generateFuncs[DeltaIC_OBJNEWEMPTY]				= &Generate_OBJNEWEMPTY;
	generateFuncs[DeltaIC_OBJSET]					= &Generate_OBJSET;
	generateFuncs[DeltaIC_ASSIGN]					= &Generate_ASSIGN;
	generateFuncs[DeltaIC_ASSIGNTEMPVAR]			= &Generate_ASSIGNTEMPVAR;
	generateFuncs[DeltaIC_CALLEXTFUNC]				= &Generate_CALLEXTFUNC;
	generateFuncs[DeltaIC_CALLFUNC]					= &Generate_CALLFUNC;
	generateFuncs[DeltaIC_CALLOBJGETMETHOD]			= &Generate_CALLOBJGETMETHOD;
	generateFuncs[DeltaIC_CALLOBJBOUNDEDGETMETHOD]	= &Generate_CALLOBJBOUNDEDGETMETHOD;

	generateFuncs[DeltaIC_DIV]						= &Generate_DIV;
	generateFuncs[DeltaIC_FUNCRET]					= &Generate_FUNCRET;
	generateFuncs[DeltaIC_FUNCENTER]				= &Generate_FUNCENTER;
	generateFuncs[DeltaIC_JUMP]						= &Generate_JUMP;
	generateFuncs[DeltaIC_JEQ]						= &Generate_JEQ;
	generateFuncs[DeltaIC_JNE]						= &Generate_JNE;
	generateFuncs[DeltaIC_JGE]						= &Generate_JGE;
	generateFuncs[DeltaIC_JGT]						= &Generate_JGT;
	generateFuncs[DeltaIC_JLE]						= &Generate_JLE;
	generateFuncs[DeltaIC_JLT]						= &Generate_JLT;
	generateFuncs[DeltaIC_JTRUETEST]				= &Generate_JTRUETEST;
	generateFuncs[DeltaIC_JFALSETEST]				= &Generate_JFALSETEST;

	generateFuncs[DeltaIC_MOD]						= &Generate_MOD;
	generateFuncs[DeltaIC_MUL]						= &Generate_MUL;
	generateFuncs[DeltaIC_PUSHARG]					= &Generate_PUSHARG;
	generateFuncs[DeltaIC_SUB]						= &Generate_SUB;
	generateFuncs[DeltaIC_OBJDONE]					= &Generate_OBJDONE;
	generateFuncs[DeltaIC_PUSHLATEBOUNDARG]			= &Generate_PUSHLATEBOUNDARG;
	generateFuncs[DeltaIC_BLOCKENTER]				= &Generate_BLOCKENTER;
	generateFuncs[DeltaIC_BLOCKEXIT]				= &Generate_BLOCKEXIT;
	generateFuncs[DeltaIC_BOUNDEDOBJGET]			= &Generate_BOUNDEDOBJGET;
	generateFuncs[DeltaIC_BOUNDEDOBJSET]			= &Generate_BOUNDEDOBJSET;
	generateFuncs[DeltaIC_ASSERT]					= &Generate_ASSERT;
	generateFuncs[DeltaIC_TRAPENABLE]				= &Generate_TRAPENABLE;
	generateFuncs[DeltaIC_TRAPDISABLE]				= &Generate_TRAPDISABLE;
	generateFuncs[DeltaIC_TRAP]						= &Generate_TRAP;
	generateFuncs[DeltaIC_THROW]					= &Generate_THROW;

	generateFuncs[DeltaIC_OBJNEWATTR]				= &Generate_OBJNEWATTR;
	generateFuncs[DeltaIC_OBJSETATTR]				= &Generate_OBJSETATTR;
	generateFuncs[DeltaIC_OBJGETATTR]				= &Generate_OBJGETATTR;

	generateFuncs[DeltaIC_FOREACHBEGIN]				= &Generate_FOREACHBEGIN;	
	generateFuncs[DeltaIC_FOREACHCHECKEND]			= &Generate_FOREACHCHECKEND;	
	generateFuncs[DeltaIC_FOREACHGETVAL]			= &Generate_FOREACHGETVAL;	
	generateFuncs[DeltaIC_FOREACHGETINDEX]			= &Generate_FOREACHGETINDEX;	
	generateFuncs[DeltaIC_FOREACHFWD]				= &Generate_FOREACHFWD;		
	generateFuncs[DeltaIC_FOREACHEND]				= &Generate_FOREACHEND;	
	generateFuncs[DeltaIC_RETURNVAL]				= &Generate_RETURNVAL;		
	generateFuncs[DeltaIC_RETURN]					= &Generate_RETURN;		
	generateFuncs[DeltaIC_GETRETVAL]				= &Generate_GETRETVAL;	
	
	generateFuncs[DeltaIC_NOP]						= &Generate_NOP;

	Reset();
}

/////////////////////////////////////////////

DeltaCodeGenerator::~DeltaCodeGenerator () {
	if (code)
		DDELARR(code);
	unfinishedJumps.clear();
}

/////////////////////////////////////////////

void DeltaCodeGenerator::Generate_Instruction (
		DeltaVMOpcode	opcode,
		DeltaQuad&		quad,
		util_ui32		quadNo
	) {

	DeltaInstruction instr;
	instr.opcode	= opcode;
	instr.line		= quad.line;

	CODEGENERATOR.MakeOperand(quad.arg1,	&instr.arg1);
	CODEGENERATOR.MakeOperand(quad.arg2,	&instr.arg2);
	CODEGENERATOR.MakeOperand(quad.result,	&instr.result);

	CODEGENERATOR.Emit(&instr);
	QUADS.SetQuadTargetLabel(quadNo, CODEGENERATOR.CurrInstructionNo());
}

/////////////////////////////////////////////
// When initialised, previously allocated code is not deleted.
// Nor the memory allocated to hold jumps. The indexes are just
// reset to be ready for new code entrance.
//
void DeltaCodeGenerator::Reset (void) {

	currInstr				= 0;
	currSize				= 0;
	codeEmitted				= false;
	unfinishedJumps.clear();
}

/////////////////////////////////////////////

void DeltaCodeGenerator::CleanUp (void) {

	DPTR(funcTable)->clear();
	ureset_via_default_ctor(&debugInfo);
	Reset();
}

//-----------------------------------------------------------------
// USER-DEFINED FUNCTION TABLE FOR CODE GENERATION.
//

DeltaStdFuncInfo* DeltaCodeGenerator::GetFuncInfo (util_ui32 serial) {

	for (	std::list<DeltaStdFuncInfo>::iterator i = funcTable->begin();
			i != funcTable->end();
			++i	)

			if (i->GetSerial() == serial)
				return &*i;

	return (DeltaStdFuncInfo*) 0;
}

//-----------------------------------------------------------------

#define	DEFAULT_GENERATE_FUNC(mnemonic)													\
void DeltaCodeGenerator::Generate_##mnemonic (DeltaQuad &quad, util_ui32 quadNo) {		\
	DASSERT(quad.opcode == DeltaIC_##mnemonic);											\
	Generate_Instruction(DeltaVM_##mnemonic, quad, quadNo);								\
}

// ARITHMETIC.

DEFAULT_GENERATE_FUNC(ADD)
DEFAULT_GENERATE_FUNC(DIV)
DEFAULT_GENERATE_FUNC(MUL)
DEFAULT_GENERATE_FUNC(MOD)
DEFAULT_GENERATE_FUNC(SUB)

//-----------------------------------------------------------------
// OBJECT MANAGEMENT.

DEFAULT_GENERATE_FUNC(OBJGET)
DEFAULT_GENERATE_FUNC(BOUNDEDOBJGET)
DEFAULT_GENERATE_FUNC(OBJNEW)
DEFAULT_GENERATE_FUNC(OBJNEWSET)
DEFAULT_GENERATE_FUNC(OBJNEWEMPTY)
DEFAULT_GENERATE_FUNC(OBJDONE)
DEFAULT_GENERATE_FUNC(OBJSET)

DEFAULT_GENERATE_FUNC(BOUNDEDOBJSET)
DEFAULT_GENERATE_FUNC(OBJNEWATTR)
DEFAULT_GENERATE_FUNC(OBJSETATTR)
DEFAULT_GENERATE_FUNC(OBJGETATTR)

//-----------------------------------------------------------------
// MISCELLANEOUS.

void DeltaCodeGenerator::Generate_ASSIGNTEMPVAR (DeltaQuad &quad, util_ui32 quadNo) 
	{ DASSERT(false); }

void DeltaCodeGenerator::Generate_ASSIGN (DeltaQuad &quad, util_ui32 quadNo) {
	DASSERT(quad.opcode == DeltaIC_ASSIGN);
	Generate_Instruction(
		quad.DELTA_ASSIGN_OPERAND_LVALUE->IsTemp() ? DeltaVM_ASSIGNTEMPVAR : DeltaVM_ASSIGN, 
		quad, 
		quadNo
	);
}

DEFAULT_GENERATE_FUNC(RETURNVAL)
DEFAULT_GENERATE_FUNC(RETURN)
DEFAULT_GENERATE_FUNC(GETRETVAL)
DEFAULT_GENERATE_FUNC(NOP)
DEFAULT_GENERATE_FUNC(ASSERT)

////////////////////////////////////////////////////////////////////

#define	GENERATE_BLOCK(_opcode)												\
	DeltaInstruction instr;													\
	instr.opcode	= _opcode;												\
	instr.line		= quad.line;											\
	CODEGENERATOR.MakeInternal(quad.arg1,	&instr.arg1);					\
	CODEGENERATOR.MakeOperand(quad.arg2,	&instr.arg2);					\
	CODEGENERATOR.MakeInternal(quad.result,	&instr.result);					\
	CODEGENERATOR.Emit(&instr);												\
	QUADS.SetQuadTargetLabel(quadNo, CODEGENERATOR.CurrInstructionNo());

void DeltaCodeGenerator::Generate_BLOCKENTER (DeltaQuad &quad, util_ui32 quadNo) 
	{ GENERATE_BLOCK(DeltaVM_BLOCKENTER); }

void DeltaCodeGenerator::Generate_BLOCKEXIT (DeltaQuad &quad, util_ui32 quadNo) 
	{ GENERATE_BLOCK(DeltaVM_BLOCKEXIT); }

//-----------------------------------------------------------------
// FUNCTION CALLS.

void DeltaCodeGenerator::Generate_ActualArgument (DeltaVMOpcode opcode, DeltaQuad &quad, util_ui32 quadNo) {

	// Notice that all argument push instructions mapping to actual args of a function
	// call, get the same line number. This will also be the line of the real call.
	// A break point may stop at the first PUSHARG instruction, always after the instructions
	// actually evaluating the arguments, but then it cannot stop in between as all take the same line.
	//
	DeltaInstruction instr;
	instr.opcode	= opcode;
	instr.line		= quad.line;

	CODEGENERATOR.MakeOperand(quad.arg1, &instr.arg1);

	if (quad.arg2)
		CODEGENERATOR.MakeOperand(quad.arg2, &instr.arg2);

	if (quad.result)
		CODEGENERATOR.MakeOperand(quad.result, &instr.result);

	CODEGENERATOR.Emit(&instr);
	QUADS.SetQuadTargetLabel(quadNo, CODEGENERATOR.CurrInstructionNo());
}

//////////////////////////////////////////////////////////////////
// PUSHARG ARG.
//
void DeltaCodeGenerator::Generate_PUSHARG(DeltaQuad &quad, util_ui32 quadNo) {
	DASSERT(quad.opcode == DeltaIC_PUSHARG);
	Generate_ActualArgument(DeltaVM_PUSHARG,quad, quadNo);
}

//////////////////////////////////////////////////////////////////
// PUSHLATEBOUNDARG ARG.
//
void DeltaCodeGenerator::Generate_PUSHLATEBOUNDARG(DeltaQuad &quad, util_ui32 quadNo) {
	DASSERT(quad.opcode == DeltaIC_PUSHLATEBOUNDARG);
	Generate_ActualArgument(DeltaVM_PUSHLATEBOUNDARG, quad, quadNo);
}

//////////////////////////////////////////////////////////////////
// This will handle both user-defined functions as well as external
// functions. External functions are considered at run-time as first-class
// values, carrying the external function name. Hence, they can be also
// assigned and called. Hence, in the call we can have three types of arguments:
// 
//	1.	Directly external function names (CALLEXTFUNC).
//	2.	Directly local function addresses (CALLFUNC).
//	3.	A variable carrying a user-defined function address (CALLFUNC).
//	4.	A variable carrying an external function name (CALLFUNC).
//
void DeltaCodeGenerator::Generate_FunctionCall (
		DeltaVMOpcode	opcode, 
		DeltaQuad&		quad, 
		util_ui32		quadNo
	) {

	DeltaInstruction instr;
	instr.opcode	= opcode;
	instr.line		= quad.line;

	// |CALLFUNC / CALLEXTFUNC |UNUSED|UNUSED|RESULT(FUNC)|.
	//
	DeltaCodeAddress beginInstrNo = CODEGENERATOR.NextInstructionNo();
	CODEGENERATOR.MakeOperand(quad.result, &instr.result);
	CODEGENERATOR.Emit(&instr);

	QUADS.SetQuadTargetLabel(quadNo, beginInstrNo);
}

//////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::Generate_CALLEXTFUNC(DeltaQuad &quad, util_ui32 quadNo) {
	DASSERT(quad.opcode == DeltaIC_CALLEXTFUNC);
	Generate_FunctionCall(DeltaVM_CALLEXTFUNC, quad, quadNo);	
}

void DeltaCodeGenerator::Generate_CALLFUNC(DeltaQuad &quad, util_ui32 quadNo) {

	DASSERT(quad.opcode == DeltaIC_CALLFUNC);

	DeltaVMOpcode opcode;

	switch ((util_ui32) quad.GetUserData()) {
		case DELTA_NONRECURSIVE_FUNC_USERDATA: {
			DASSERT(quad.result->GetType() != DeltaExprLambda);
			if (quad.result->GetType() == DeltaExprProgramFunction)
				opcode = DeltaVM_CALLPROGRAMFUNC;
			else
				opcode = DeltaVM_CALLFUNC;
			break;
		}
		case DELTA_RECURSIVE_PROGRAMFUNC_USERDATA	: opcode = DeltaVM_CALLLAMBDAPROGRAMFUNC;	break;
		case DELTA_RECURSIVE_METHODFUNC_USERDATA	: opcode = DeltaVM_CALLLAMBDAMETHODFUNC;	break;
		default: DASSERT(false);
	}

	Generate_FunctionCall(opcode, quad, quadNo);	
}

//////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::Generate_MethodCall (
		DeltaVMOpcode	opcode, 
		DeltaQuad&		quad, 
		util_ui32		quadNo
	) {

	DeltaInstruction instr;
	instr.opcode	= opcode;
	instr.line		= quad.line;

	DeltaCodeAddress beginInstrNo = CODEGENERATOR.NextInstructionNo();
	CODEGENERATOR.MakeOperand(quad.arg1, &instr.arg1);
	CODEGENERATOR.MakeOperand(quad.arg2, &instr.arg2);
	CODEGENERATOR.Emit(&instr);

	QUADS.SetQuadTargetLabel(quadNo, beginInstrNo);
}

//////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::Generate_CALLOBJGETMETHOD (DeltaQuad &quad, util_ui32 quadNo) {
	DASSERT(quad.opcode == DeltaIC_CALLOBJGETMETHOD);
	Generate_MethodCall(DeltaVM_CALLOBJGETMETHOD, quad, quadNo);	
}

//////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::Generate_CALLOBJBOUNDEDGETMETHOD (DeltaQuad &quad, util_ui32 quadNo) {
	DASSERT(quad.opcode == DeltaIC_CALLOBJBOUNDEDGETMETHOD);
	Generate_MethodCall(DeltaVM_CALLOBJBOUNDEDGETMETHOD, quad, quadNo);	
}

//////////////////////////////////////////////////////////////////
	
void DeltaCodeGenerator::ProduceFuncInfo (const DeltaSymbol* func, DeltaStdFuncInfo* info) {
	
	// Make std information for functions.

	udestructor_invocation(info);

	new (info) DeltaStdFuncInfo(
		func->GetFunctionReadableName(),
		func->IsFunctionExported(),
		func->IsAnonymousFunction(),
		func->GetAddress(),
		func->HasVarArgs(),
		func->serial,
		func->GetFormals(),
		func->IsMethod(),
		!func->scope	// Global
	);

	if (DPTR(func)->HasClosure()) {

		DeltaStdFuncInfo::ClosureVarsInfo	varsInfo;
		const std::list<DeltaSymbol*>&		vars = DPTR(func)->GetClosureVars();

		// Produce information for closure variables with the exact order of appearence.
		for (std::list<DeltaSymbol*>::const_iterator i = vars.begin(); i != vars.end(); ++i) {

			DASSERT(DPTR(*i)->IsClosureVarAccess() && DPTR(*i)->GetClosureVarAccessed());

			util_ui16						offset = DPTR(*i)->GetClosureVarAccessed()->GetOffset();	// Offset of referred var
			DeltaClosureVarInfo::VarType	type;
			
			if (!DPTR(*i)->IsDirectClosureVarAccess()) {
				DASSERT(DPTR(*i)->GetClosureVarAccessed()->GetClosureVarAccessed());					// Refers to closure var
				type = DeltaClosureVarInfo::VarTypeClosure;
			}
			else {
				DeltaSymbol* var = DPTR(*i)->GetClosureVarAccessed();
				DASSERT(!DPTR(var)->GetClosureVarAccessed());											// Can't be a closure var too

				if (DPTR(var)->IsFormal())
					{ DASSERT(var->myFunc); type = DeltaClosureVarInfo::VarTypeFormal; }
				else
				if (DPTR(var)->IsLocal())
					{ DASSERT(var->myFunc); type = DeltaClosureVarInfo::VarTypeLocal; }
				else {
					DASSERT(var->IsGlobal() && !var->IsTopGlobal());
					type = DeltaClosureVarInfo::VarTypeGlobal;				
				}
			}

			varsInfo.push_back(
				DeltaClosureVarInfo(
					offset, 
					type,
					DPTR(*i)->GetName(),
					DPTR(*i)->GetLine()
				)
			);
		}

		DASSERT(!varsInfo.empty());
		info->SetClosureVarsInfo(varsInfo);
	}

	// Produce function return types information.
	if (info->IsGlobal() && info->IsNamedFunction())
		DELTARETURNTYPES.Convert(
			func->GetFunctionSignature().GetReturnTypes(),
			info->GetReturnTypesPtr()
		);
}

//////////////////////////////////////////////////////////////////
//	|FUNCENTER / FUNCENTER |ARG1(funcInfo)|ARG2(#locals)|RESULT(blockId)|

#define	EMIT_FUNC_ENTER_OR_EXIT_INSTRUCTION(_opcode)										\
	if (true) {																				\
	DeltaInstruction instr;																	\
	instr.opcode	= DeltaVM_##_opcode;													\
	instr.line		= DELTA_LINE_NOT_VISIBLE;												\
	instr.DELTA_FUNCENTER_OPERAND_FUNCINFO.SetType(											\
		func->IsMethod() ? DeltaOperand_MethodFunc : DeltaOperand_ProgramFunc				\
	);																						\
	instr.DELTA_FUNCENTER_OPERAND_FUNCINFO.SetValue(										\
		DPTR(func)->GetAddress()															\
	);																						\
	if (true) {\
	CODEGENERATOR.MakeInternal(quad.arg2,	&instr.DELTA_FUNCENTER_OPERAND_TOTALLOCALS);	\
	CODEGENERATOR.MakeInternal(quad.result,	&instr.DELTA_FUNCENTER_OPERAND_BLOCKID);		\
	} \
	CODEGENERATOR.Emit(&instr);																\
	QUADS.SetQuadTargetLabel(quadNo, CODEGENERATOR.CurrInstructionNo());					\
	} else 

void DeltaCodeGenerator::Generate_FUNCENTER (DeltaQuad &quad, util_ui32 quadNo) {

	DASSERT(quad.opcode == DeltaIC_FUNCENTER);
	DASSERT(DPTR(quad.DELTA_FUNCENTER_OPERAND_FUNCINFO)->GetType() == DeltaExprProgramFunction);

	// Set the function address for the function symbol.
	DeltaSymbol* func = DPTR(quad.DELTA_FUNCENTER_OPERAND_FUNCINFO)->sym;
	DPTR(func)->SetAddress(CODEGENERATOR.NextInstructionNo());

	EMIT_FUNC_ENTER_OR_EXIT_INSTRUCTION(FUNCENTER);
	
	// Add the entry in the function table.
	DeltaStdFuncInfo funcInfo;
	ProduceFuncInfo(func, &funcInfo);
	funcTable->push_back(funcInfo);
}

//////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::Generate_FUNCRET (DeltaQuad &quad, util_ui32 quadNo) {

	DASSERT(quad.opcode == DeltaIC_FUNCRET);
	DASSERT(DPTR(quad.DELTA_FUNCENTER_OPERAND_FUNCINFO)->GetType() == DeltaExprProgramFunction);

	DeltaSymbol* func = quad.DELTA_FUNCENTER_OPERAND_FUNCINFO->sym;
	EMIT_FUNC_ENTER_OR_EXIT_INSTRUCTION(FUNCRET);
}

//-----------------------------------------------------------------
// JUMP INSTRUCTIONS.
//
void DeltaCodeGenerator::Generate_JUMP (DeltaQuad &quad, util_ui32 quadNo) {

	DASSERT(quad.opcode == DeltaIC_JUMP);

	DeltaInstruction instr;
	instr.opcode	= DeltaVM_JUMP;
	instr.line		= quad.line;

	// If jumping before, no need to consider it as an unfinished jump.
	//
	if (quad.label < quadNo)
		CODEGENERATOR.MakeLabel(&instr.result, QUADS.GetQuadTargetLabel(quad.label));
	else { 
		CODEGENERATOR.MakeLabel(&instr.result, quad.label);
		CODEGENERATOR.AddUnfinishedJump(CODEGENERATOR.CurrInstructionNo() + 1);
	}

	DASSERT(quad.arg1 == NIL_EXPR || quad.arg1->GetType() == DeltaExprNumber);
	DASSERT(quad.arg2 == NIL_EXPR);
	DASSERT(quad.result == NIL_EXPR || quad.result->GetType() == DeltaExprNumber);

	if (quad.result)	// Block exit information at 'arg1' of the instruction.
		CODEGENERATOR.MakeInternal(quad.result, &instr.DELTA_JUMP_OPERAND_EXIT_BLOCKS_TOTAL);

	if (quad.arg1)		// Trap disable information at 'arg2' of the instruction.
		CODEGENERATOR.MakeInternal(quad.arg1, &instr.DELTA_JUMP_OPERAND_TRAP_DISABLE_TOTAL);

	CODEGENERATOR.Emit(&instr);
	QUADS.SetQuadTargetLabel(quadNo, CODEGENERATOR.CurrInstructionNo());
}

//////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::Generate_TRAPENABLE (DeltaQuad &quad, util_ui32 quadNo) {

	DeltaInstruction instr;
	instr.opcode	= DeltaVM_TRAPENABLE;
	instr.line		= quad.line;

	CODEGENERATOR.MakeLabel(&instr.result, quad.label);
	CODEGENERATOR.AddUnfinishedJump(CODEGENERATOR.CurrInstructionNo()+1);

	CODEGENERATOR.Emit(&instr);
	QUADS.SetQuadTargetLabel(quadNo, CODEGENERATOR.CurrInstructionNo());
}

//*********************

void DeltaCodeGenerator::Generate_TRAPDISABLE (DeltaQuad &quad, util_ui32 quadNo) {

	DeltaInstruction instr;
	instr.opcode	= DeltaVM_TRAPDISABLE;
	instr.line		= quad.line;

	CODEGENERATOR.MakeLabel(&instr.result, quad.label);
	CODEGENERATOR.AddUnfinishedJump(CODEGENERATOR.CurrInstructionNo()+1);

	CODEGENERATOR.Emit(&instr);
	QUADS.SetQuadTargetLabel(quadNo, CODEGENERATOR.CurrInstructionNo());
}

//*********************

void DeltaCodeGenerator::Generate_TRAP (DeltaQuad &quad, util_ui32 quadNo) {
	DeltaInstruction instr;
	instr.opcode	= DeltaVM_TRAP;
	instr.line		= quad.line;

	CODEGENERATOR.MakeInternal(quad.arg1,	&instr.arg1);
	CODEGENERATOR.MakeOperand(quad.result,	&instr.result);

	CODEGENERATOR.Emit(&instr);
	QUADS.SetQuadTargetLabel(quadNo, CODEGENERATOR.CurrInstructionNo());
}

//*********************

void DeltaCodeGenerator::Generate_THROW (DeltaQuad &quad, util_ui32 quadNo) 
	{ Generate_Instruction(DeltaVM_THROW, quad, quadNo); }

//////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::Generate_Relational (
		DeltaQuad&			quad,
		DeltaVMOpcode		opcode,
		DeltaQuadAddress	quadNo
	) {

	DeltaInstruction instr;
	instr.opcode	= opcode;
	instr.line		= quad.line;
	CODEGENERATOR.MakeOperand(quad.arg1, &instr.arg1);
	CODEGENERATOR.MakeOperand(quad.arg2, &instr.arg2);
	CODEGENERATOR.MakeLabel(&instr.result, quad.label);
	CODEGENERATOR.Emit(&instr);

	QUADS.SetQuadTargetLabel(quadNo, CODEGENERATOR.CurrInstructionNo());
	CODEGENERATOR.AddUnfinishedJump(CODEGENERATOR.CurrInstructionNo());
}

//////////////////////////////////////////////////////////////////

#define	JUMP_GENERATE_FUNC(mnemonic)													\
	void DeltaCodeGenerator::Generate_##mnemonic (DeltaQuad &quad, util_ui32 quadNo) {	\
	DASSERT(quad.opcode == DeltaIC_##mnemonic);											\
	Generate_Relational(quad, DeltaVM_##mnemonic, quadNo);								\
}

JUMP_GENERATE_FUNC(JEQ)
JUMP_GENERATE_FUNC(JNE)
JUMP_GENERATE_FUNC(JGE)
JUMP_GENERATE_FUNC(JGT)
JUMP_GENERATE_FUNC(JLT)
JUMP_GENERATE_FUNC(JLE)
JUMP_GENERATE_FUNC(JTRUETEST)
JUMP_GENERATE_FUNC(JFALSETEST)

//-----------------------------------------------------------------

void DeltaCodeGenerator::Generate_FOREACHBEGIN (DeltaQuad &quad, util_ui32 quadNo) 
	{ Generate_Instruction(DeltaVM_FOREACHBEGIN, quad, quadNo); }

void DeltaCodeGenerator::Generate_FOREACHCHECKEND (DeltaQuad &quad, util_ui32 quadNo)
	{ Generate_Relational(quad,  DeltaVM_FOREACHCHECKEND, quadNo); }

void DeltaCodeGenerator::Generate_FOREACHGETVAL (DeltaQuad &quad, util_ui32 quadNo)
	{ Generate_Instruction(DeltaVM_FOREACHGETVAL, quad, quadNo); }

void DeltaCodeGenerator::Generate_FOREACHGETINDEX (DeltaQuad &quad, util_ui32 quadNo)
	{ Generate_Instruction(DeltaVM_FOREACHGETINDEX, quad, quadNo); }

void DeltaCodeGenerator::Generate_FOREACHFWD (DeltaQuad &quad, util_ui32 quadNo) 
	{ Generate_Relational(quad,  DeltaVM_FOREACHFWD, quadNo); }

void DeltaCodeGenerator::Generate_FOREACHEND (DeltaQuad &quad, util_ui32 quadNo)
	{ Generate_Instruction(DeltaVM_FOREACHEND, quad, quadNo); }

//-----------------------------------------------------------------

void DeltaCodeGenerator::ExpandCode (void) {

#define	NEW_SIZE (currSize+DELTA_CODE_ALLOCATION_SIZE)

	DeltaInstruction* newCode = DNEWARR(DeltaInstruction,NEW_SIZE);

	if (code) {
		memcpy(DPTR(newCode), DPTR(code), currSize*sizeof(DeltaInstruction));
		DDELARR(code);
	}

	code = newCode;
	currSize += DELTA_CODE_ALLOCATION_SIZE;

#undef	NEW_SIZE
}

//-----------------------------------------------------------------

// 'instr' is not a dynamic entry. It is the address of an auto variable.
//
void DeltaCodeGenerator::Emit (DeltaInstruction* instr) {

	if (currInstr >= DELTA_MAXINSTR_INDEX) {
		DeltaCompError(
			"Source is too large (max #instructions %u exceeded): split in more files)!",
			DELTA_MAXINSTR_INDEX
		);
		return;
	}

#ifdef	AUTO_ASSIGN_ZERO_NUMBERED_LINES

	// Adjust line, so that 0 line numbers get the same as
	// the previous line. This holds for instr number > 0.
	// Also, max line numbers are automatically set to 0.
	// Those lines are not "visible" during execution, meaning
	// break points are not allowed to stop at them.
	//
	if (!instr->line && currInstr)
		instr->line = code[currInstr].line;
#else
	// In case of function calls we need to make sure that getreval that is executed
	// after returning to function has a line number, including the instruction
	// that follows it since getretval may be optimized out.

	if (!instr->line && currInstr && code[currInstr].GetOpCode() == DeltaVM_GETRETVAL)
		instr->line	= code[currInstr].line = currLine;
#endif

	// Max line numbers are automatically set to 0.
	// Those lines are not "visible" during execution, meaning
	// break points are not allowed to stop at them.
	//
	if (instr->line == DELTA_LINE_NOT_VISIBLE)
		instr->line = 0;

	if (instr->line)
		currLine = instr->line;

	if (!codeEmitted)
		codeEmitted = true;
	else
		++currInstr;

	if (currSize == currInstr)
		ExpandCode();

	memcpy(
		code + currInstr, 
		instr, 
		sizeof(DeltaInstruction)
	);
}

//-----------------------------------------------------------------
// OPERAND CREATION.

void DeltaCodeGenerator::MakeOperand (DeltaExpr* expr, DeltaOperand* operand) {
	if (!expr) {
		operand->SetType(DeltaOperand_NotUsed);
		operand->SetValue(0xffffffff);	// Guard.
	}
	else
	if (DPTR(expr)->GetType() == DeltaExprNumber) {
		operand->SetType(DeltaOperand_ConstNumber);
		operand->SetValue(constArrays.AddNumericConst(DPTR(expr)->numConst));
	}
	else
	if (DPTR(expr)->GetType() == DeltaExprString) {
		operand->SetType(DeltaOperand_ConstString);
		operand->SetValue(constArrays.AddStringConst(DPTR(expr)->strConst.c_str()));
	}
	else
	if (DPTR(expr)->GetType()==DeltaExprBoolean)
		MakeBool(operand, expr->boolConst);
	else
	if (DPTR(expr)->GetType() == DeltaExprNil) {
		operand->SetType(DeltaOperand_ConstNil);
		operand->SetValue(0xffffffff);
	}
	else
	if (DPTR(expr)->GetType() == DeltaExprLibraryFunction) {
		operand->SetType(DeltaOperand_LibFunc);
		operand->SetValue(constArrays.AddExternFuncUsed(DPTR(expr->sym)->GetName().c_str()));
	}
	else
	if (DPTR(expr)->GetType() == DeltaExprProgramFunction) {

		DeltaStdFuncInfo* info = GetFuncInfo(expr->sym->serial);
		DASSERT(info);

		if (DPTR(expr)->sym->funcClass == DELTA_FUNCCLASS_METHOD)
			operand->SetType(DeltaOperand_MethodFunc);
		else {
			DASSERT(
				(expr)->sym->funcClass == DELTA_FUNCCLASS_PROGRAMFUNCTION ||
				(expr)->sym->funcClass == DELTA_FUNCCLASS_ONEVENT
			);
					
			operand->SetType(DeltaOperand_ProgramFunc);
		}

		operand->SetValue(info->GetAddress());
	}
	else	// An implicit or explicit storage expression type.
	if (DPTR(expr)->GetType() == DeltaExprTableConstruction		|| 
		DPTR(expr)->GetType() == DeltaExprTableElement			||
		DPTR(expr)->GetType() == DeltaExprArithmetic			||
		DPTR(expr)->GetType() == DeltaExprLogical				||
		DPTR(expr)->GetType() == DeltaExprVar					||
		DPTR(expr)->GetType() == DeltaExprCall					)

		if (DPTR(expr)->GetType() == DeltaExprTableElement && !DPTR(expr)->sym) {	// The only case we have no symbol.
			operand->SetType(DeltaOperand_NewSelf);
			operand->SetValue(0xffffffff);	// Guard.
		}
		else {
			// The offset translation is a matter of the virtual machine.
			// We also put information if the variable is a temporary one.
			//
			DeltaSymbol* var = DNULLCHECK(DPTR(expr)->sym);

			if (DPTR(var)->IsClosureVarAccess()) {
				DASSERT(DPTR(var)->myFunc);
				operand->SetType(DeltaOperand_ClosureVar);
				operand->SetVarValue(false, DPTR(var)->GetOffset());
			}
			else {
				operand->SetVarValue(DPTR(var)->IsTempVar(), DPTR(var)->GetOffset());

				if (DPTR(var)->IsGlobal() || DPTR(var)->IsStatic())
					operand->SetType(DeltaOperand_Global);
				else {
					DASSERT(DPTR(var)->myFunc);
					if (DPTR(var)->IsFormal())
						operand->SetType(DeltaOperand_Formal);
					else {
						DASSERT(DPTR(var)->IsLocal());
						operand->SetType(DeltaOperand_Local);
					}
				}
			}
	}
	else
	if (DPTR(expr)->GetType() == DeltaExprLambda) {
		operand->SetType(DeltaOperand_Lambda);
		operand->SetValue(0xffffffff);	// Guard.
	}
	else
	if (DPTR(expr)->GetType() == DeltaExprNewSelf) {
		operand->SetType(DeltaOperand_NewSelf);
		operand->SetValue(0xffffffff);	// Guard.
	}
	else {
		DASSERT(!"Invalid type for expression in making operand!");
	}
}

////////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::MakeInternal (DeltaExpr* expr, DeltaOperand* operand) {
	DASSERT(expr->GetType() == DeltaExprNumber);
	operand->SetType(DeltaOperand_Internal);
	operand->SetValue((util_ui32) expr->numConst);
}

////////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::MakeLabel(DeltaOperand* operand, util_ui32 label) {
	operand->SetType(DeltaOperand_Label);
	operand->SetValue(label);
}

////////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::MakeNotUsed(DeltaOperand* operand) {
	operand->SetType(DeltaOperand_NotUsed);
	operand->SetValue(0);
}

////////////////////////////////////////////////////////////////////

// This concerns only making constant numbers.
//
void DeltaCodeGenerator::MakeNumber (DeltaOperand* operand, DeltaNumberValueType val) {

	operand->SetType(DeltaOperand_ConstNumber);
	operand->SetValue(constArrays.AddNumericConst(val));
}

////////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::MakeBool (DeltaOperand* operand, bool val) {
	operand->SetType(DeltaOperand_ConstBool);
	operand->SetValue(val ? 1 : 0);
}

//-----------------------------------------------------------------
// Code layout is the following:
//
// |Version number|Source file|Total globals|
// |Function table|
// |Arrays of constant values| 
// |Debug information|
// |Instructions|
//
bool DeltaCodeGenerator::WriteBinaryCode (GenericWriter& writer, bool includeDebugInfo) {

	if (!codeEmitted)
		return false;

	DeltaWriteVersionInformation(writer);
	if (DeltaCompiler::IsDynamicCode())
		writer.write(DBG_DYNAMIC_SOURCE);
	else
		writer.write(DeltaCompiler::GetSourceFile());
	WriteFunctionTable(writer);
	writer.write((util_ui16) ParseParms::GlobalDataSize());
	constArrays.Write(writer);
	WriteDebugInfo(writer, includeDebugInfo);
	WriteInstructions(writer);

	return true;
}

//////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::GenerateCode (void) {

	// Firstly, provide offsets to static variables (at block 0) and
	// shift all globals (execept the retval) after the static vars.

	if (DELTASYMBOLS.GetTotalStaticVars()) {
		DELTASYMBOLS.GiveOffsetsToStaticVariables(ParseParms::GlobalDataSize());
		DELTASYMBOLS.ShiftOffsetsOfGlobalVariablesAfterStatics();
	}

	// Check if currSize is 0, meaning no code memory yet. If yes,
	// allocate memory for the code generation. This is set initially
	// as the number of quads.

	if (!currSize) {
		currSize = QUADS.Total();
		if (code)
			DDELARR(code);
		code = DNEWARR(DeltaInstruction, currSize);
	}

	EmitStartUpInstruction();

	// Iterate through quads and call the respective generator.

	DeltaQuad quad;
	QUADS.StartIteration();

	util_ui32 quadNo;
	util_ui32 droppedLine = 0;
	currLine = 0;

	for (quadNo = DELTA_START_QUAD; QUADS.Iterate(&quad); ++quadNo) {

		// Because of optimization we may drop quads that
		// actually carry line information. As this is not
		// correct, we wrap-around by storing the non-zero line
		// of a dropped quad and assigning it to the first
		// quad that has a zero line.

		if (quad.IsDropped()) {
			if (quad.line)
				droppedLine = quad.line;
			continue;
		}
		else
		if (!quad.line && droppedLine) {
			quad.line = droppedLine;
			droppedLine = 0;
		}

		// In each quad, we keep information regarding the current
		// quad line and function serial. This is put in the debug info.

		(*generateFuncs[quad.opcode])(quad, quadNo);

		if (DeltaCompErrorsExist())
			return;

		if (quad.line)
			debugInfo.GetLines().AddLine(quad.line, quad.funcSerial);
	} 

	DASSERT(
		quadNo == QUADS.Total() || 	// Either we passed all instructions.
		(quadNo == DELTA_START_QUAD && QUADS.CurrQuadNo() == DELTA_START_QUAD)	// O rhave no instructions
	);

	EmitProgramEndInstruction();
	PatchUnfinishedJumps();
	SelectiveStepInPreparator::OnTargetCodeProduced();
}

//-----------------------------------------------------------------

// Start-up instruction. We give it line number 0 so that
// no break point can be set before execution starts.
//
void DeltaCodeGenerator::EmitStartUpInstruction (void) {
	
	DeltaInstruction instr;
	instr.opcode	= DeltaVM_PROGRAMBEGIN;
	instr.line		= 0;
	Emit(&instr);
}

void DeltaCodeGenerator::EmitProgramEndInstruction (void) {
	
	DeltaInstruction instr;
	instr.opcode	= DeltaVM_PROGRAMEND;
	instr.line		= 0;
	Emit(&instr);
}

//////////////////////////////////////////////////////////////////
// Patch the unfinished jumps. In case of jumps right
// passed the end of the code, we give the value of
// the last+1 instruction number.
// 
void DeltaCodeGenerator::PatchUnfinishedJumps (void) {
	
	for (	std::list<DeltaCodeAddress>::iterator i = unfinishedJumps.begin();
			i != unfinishedJumps.end();
			++i	) {

		DASSERT(*i < currInstr);

		DeltaOperand& label= code[*i].result;
		DASSERT(label.GetType() == DeltaOperand_Label);

		// If passed the end of code, the same for target
		// code.
		//
		if (label.GetValue() == QUADS.Total())
			label.SetValue(Total());
		else
			label.SetValue(
				QUADS.GetQuadTargetLabel(label.GetValue())
			);
	}
}

//////////////////////////////////////////////////////////////////
// Write number of user-defined functions, followed
// by the table entry for each function.
//
void DeltaCodeGenerator::WriteFunctionTable (GenericWriter& writer) {

	writer.write((util_ui16) funcTable->size());

	for (	std::list<DeltaStdFuncInfo>::iterator i = funcTable->begin();
			i != funcTable->end();
			++i	)
			i->Write(writer);
}

//////////////////////////////////////////////////////////////////
// Write firstly number of instructions, followed by
// the instructions.
//
void DeltaCodeGenerator::WriteInstructions (GenericWriter& writer) {

	writer.write((util_ui16) (currInstr + 1));

	for (DeltaCodeAddress instrNo = 0; instrNo <= currInstr; ++instrNo)
		code[instrNo].Write(writer);
}

//-----------------------------------------------------------------

void DeltaCodeGenerator::ProduceDebugInfoForCodeLineChunks (void) {

	DeltaCodeLineChunk		currChunk;
	std::list<util_ui32>	funcSerialStack;
	
	funcSerialStack.push_front(DELTA_FUNCSERIAL_OF_GLOBAL_CODE);

	for (util_ui32 i = 0; i < QUADS.Total(); ++i) {

		DeltaQuad& quad = QUADS.GetQuad(i);
		if (quad.GetOpCode() == DeltaIC_FUNCENTER)
			funcSerialStack.push_front(DPTR(quad.GetArg1()->sym)->GetSerial() + 1);
		else
		if (quad.GetOpCode() == DeltaIC_FUNCRET)
			funcSerialStack.pop_front();
		else
		if (const util_ui32 line = quad.GetLine()) {

			const util_ui32 funcSerial = funcSerialStack.front();

			if (!currChunk.start) {								// Chunk is used for the first time.
				currChunk.start	= currChunk.end	 = line;
				currChunk.funcSerial = funcSerial;
			}
			else
			if (currChunk.funcSerial == funcSerial) {			// Same func serial, update end line.
				if (line >= currChunk.end)
					currChunk.end = line;
			}
			else {
				debugInfo.GetChunks().AppendChunk(currChunk);	// Add existing chunk and initalise a new one.
				currChunk.start	= currChunk.end	= line;
				currChunk.funcSerial = funcSerial;
			}
		}
	}
}

//-----------------------------------------------------------------
// Generation of debug information.
//

void DeltaCodeGenerator::ProduceDebugInfoForFunction (DeltaSymbol* sym) {

	DeltaDebugFuncInfo* func = DNULLCHECK(debugInfo.GetFunc(sym->serial));

	if (!func->IsDone()) {
		func->SetInformation(
			sym->GetFunctionReadableName(), 
			DEBUGSTATICS.FullFunctionName(sym),
			sym->addr,
			sym->line, 
			sym->scope,
			sym->myFunc? sym->myFunc->addr : DELTA_CANTBE_A_FUNCTION_ADDRESS
		);
		func->Locals().SetTotalBlocks(DPTR(sym)->totalBlocks);
		func->SetTotalFormals(DPTR(sym)->GetTotalFormals());
		func->SetDone();
	}
}

///////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::ProduceDebugInfoForStaticVar (DeltaSymbol* sym) {
	debugInfo.GetGlobals().AddVar(
		DEBUGSTATICS.GetDebugName(sym).c_str(),
		DELTA_STATICS_OWNING_BLOCKID,
		sym->GetOffset(),
		sym->GetLine()
	);
}

///////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::ProduceDebugInfoForGlobalVar (DeltaSymbol* sym) {
	debugInfo.GetGlobals().AddVar(
		sym->GetName().c_str(),
		sym->GetBlockId(), 
		sym->GetOffset(),
		sym->GetLine()
	);
}

///////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::ProduceDebugInfoForUsedLibraryConsts (void) {

	const ubag<DeltaSymbol*>& b = QUADS.GetUsedLibraryConsts();

	for (ubag<DeltaSymbol*>::const_iterator i = b.begin(); i != b.end(); ++i) {

		DeltaSymbol* sym = i->first;
		DASSERT(DPTR(sym)->IsLibraryConst() && !DPTR(sym)->myFunc);

		DeltaExpr* e = DNULLCHECK(sym->GetConst());

		switch (DPTR(e)->GetType()) {
			case DeltaExprNumber: debugInfo.AddUsedLibraryConst(sym->GetName(), e->numConst); break;
			case DeltaExprString: debugInfo.AddUsedLibraryConst(sym->GetName(), e->strConst); break;
			default: DASSERT(false);
		}
	}
}

///////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::ProduceDebugInfoForFormalArgOrLocalVar (DeltaSymbol* sym) {

	DASSERT(sym->myFunc);
	ProduceDebugInfoForFunction(sym->myFunc);
	DeltaDebugFuncInfo* func = debugInfo.GetFunc(sym->myFunc->serial);
	DASSERT(func);

	if (sym->IsFormal())										// FORMAL
		func->AddFormal(sym->GetName().c_str(), sym->GetOffset());
	else
	if (!sym->IsUserDefinedConst() && !sym->IsLibraryConst())	// LOCAL
		func->Locals().AddVar(
			sym->GetName().c_str(), 
			sym->GetBlockId(), 
			sym->GetOffset(), 
			sym->GetLine()
		);
}

///////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::WriteDebugInfo (GenericWriter& writer, bool includeDebugInfo) {
	writer.write(includeDebugInfo);
	if (includeDebugInfo) {
		ProduceDebugInfo();
		debugInfo.Write(writer);
	}
}

///////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::ProduceDebugInfoForSymbols (void) {

	for (	std::list<DeltaSymbol*>::iterator i = DELTASYMBOLS.allSymbols.begin(); 
			i != DELTASYMBOLS.allSymbols.end(); 
			++i	) {

		DeltaSymbol* sym = *i;
		if (DPTR(sym)->IsImportedLibVar())										// USED BYTECODE LIBRARY (extra)
			debugInfo.AddUsedByteCodeLibrary(sym->GetName());

		if (DPTR(sym)->IsUserDefinedFunction())								// USER FUNCTION
			ProduceDebugInfoForFunction(sym);
		else
		if (!sym->IsLibraryFunction() && !sym->IsClosureVarAccess()) {
			if (sym->IsStatic())											// STATIC VAR
				ProduceDebugInfoForStaticVar(sym);
			else
			if (!sym->GetMyFunction()) {									
				if (!sym->IsUserDefinedConst() && !sym->IsLibraryConst())	// GLOBAL VAR
					ProduceDebugInfoForGlobalVar(sym);
				else											
					NO_DEBUG_INFORMATION_IS_PRODUCED(sym);					// NAMED CONSTANT
			}
			else
				ProduceDebugInfoForFormalArgOrLocalVar(sym);				// FORMAL / LOCAL
		}
	}
}

///////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::ProduceDebugInfoForOpenedNamespaces(void) {

	typedef DeltaLibraryNamespaceHolder::Namespaces Namespaces;
	const Namespaces& namespaces = DELTANAMESPACES.GetOpened();

	for (Namespaces::const_iterator i = namespaces.begin(); i != namespaces.end(); ++i)
		if (i->second != DELTANAMESPACES.GetGlobalNamespace())
			debugInfo.AddOpenedNamespace(i->first);
}

///////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::ProduceDebugInfo (void) {

	if (DeltaCompiler::IsDynamicCode())
		debugInfo.SetDynamicCode(DeltaCompiler::GetDynamicCode());

	debugInfo.GetGlobals().SetTotalBlocks(ParseParms::GetTotalGlobalBlocks());
	debugInfo.SetTotalFuncs(ParseParms::CurrFuncSerial());

	ProduceDebugInfoForOpenedNamespaces();
	ProduceDebugInfoForUsedLibraryConsts();
	ProduceDebugInfoForSymbols();
	ProduceDebugInfoForCodeLineChunks();
	debugInfo.SetCalls(*DPTR(SelectiveStepInPreparator::GetAllStmts()));
}

///////////////////////////////////////////////////////////////////

static const char* instrOpcodes[] = {
	"ADD",
	"OBJGET",
	"OBJNEW",
	"OBJNEWSET",
	"OBJNEWEMPTY",
	"OBJSET",
	"ASSIGN",
	"ASSIGNTEMPVAR",
	"CALLFUNC",
	"CALLPROGRAMFUNC",
	"CALLLAMBDAPROGRAMFUNC",
	"CALLLAMBDAMETHODFUNC",
	"CALLEXTFUNC",
	"CALLEXTPREBOUNDFUNC",
	"DIV",
	"JUMP",
	"JEQ",
	"JNE",
	"JGE",
	"JGT",
	"JLE",
	"JLT",
	"JTRUETEST",
	"JFALSETEST",
	"MOD",
	"MUL",
	"SUB",
	"PUSHARG",
	"FUNCRET",
	"FUNCENTER",
	"PROGRAMBEGIN",
	"OBJDONE",
	"PUSHLATEBOUNDARG",
	"BLOCKENTER",
	"BLOCKEXIT",
	"BOUNDEDOBJGET",
	"BOUNDEDOBJSET",
	"ASSERT",
	"TRAPENABLE",
	"TRAPDISABLE",
	"TRAP",
	"THROW",
	"PROGRAMEND",
	"OBJNEWATTR",
	"OBJSETATTR",
	"OBJGETATTR",
	"CALLOBJGETMETHOD",
	"CALLOBJBOUNDEDGETMETHOD",
	"FOREACHBEGIN",
	"FOREACHCHECKEND",	
	"FOREACHGETVAL",	
	"FOREACHGETINDEX",	
	"FOREACHFWD",	
	"FOREACHEND",
	"GETRETVAL",
	"RETURNVAL",
	"RETURN",
	"NOP"
};

//////////////////////////////////////////////////////////////////

const std::string DeltaCodeGenerator::Handle (DeltaOperand* operand) {
	switch (operand->GetType()) {
		case DeltaOperand_NotUsed		: return uconstructstr("_");
		case DeltaOperand_ConstNumber	: return uconstructstr("%f", constArrays.GetNumericConst((util_ui16) operand->GetValue()));
		case DeltaOperand_ConstString	: return std::string("'") + uunparseescapesequences(constArrays.GetStringConst((util_ui16) operand->GetValue())) + "'";
		case DeltaOperand_ConstBool		: return ubool2conststr(!!operand->GetValue());
		case DeltaOperand_ConstNil		: return "nil";
		case DeltaOperand_LibFunc		: return constArrays.GetExternFuncUsed((util_ui16) operand->GetValue());
		case DeltaOperand_ProgramFunc	: return uconstructstr("func(%u)", operand->GetValue());
		case DeltaOperand_MethodFunc	: return uconstructstr("method(%u)", operand->GetValue());
		case DeltaOperand_Label			: return uconstructstr("label(%u)", operand->GetValue());
		case DeltaOperand_Global		: return uconstructstr("global(%u)", operand->GetValue());
		case DeltaOperand_Local			: return uconstructstr("local(%u)", operand->GetValue());
		case DeltaOperand_Formal		: return uconstructstr("formal(%u)", operand->GetValue());
		case DeltaOperand_ClosureVar	: return uconstructstr("closure(%u)", operand->GetValue());
		case DeltaOperand_Lambda		: return "lambda";
		case DeltaOperand_NewSelf		: return "newself";
		case DeltaOperand_Internal		: return uconstructstr("%u", operand->GetValue());
		default							: DASSERT(false); return "";
	}
}

//////////////////////////////////////////////////////////////////

void DeltaCodeGenerator::WriteInstructionText (
		DeltaCodeAddress	addr,
		DeltaInstruction&	instr, 
		FILE*				fp
	) {
	
	DASSERT(addr < Total() && instr.opcode < uarraysize(instrOpcodes));

	fprintf(
		fp,
		"%u(%u): %s ",
		addr, 
		instr.line == DELTA_LINE_NOT_VISIBLE ? DELTA_CANTBE_A_SOURCE_LINE : instr.line,
		instrOpcodes[instr.opcode]
	);

	// We need separate fprintf since Handle returns the address of a static
	// string buffer.
	//
	fprintf(fp,"%s ",	Handle(&instr.arg1).c_str());
	fprintf(fp,"%s ",	Handle(&instr.arg2).c_str());
	fprintf(fp,"%s\n",	Handle(&instr.result).c_str());
}

//////////////////////////////////////////////////////////////////

bool DeltaCodeGenerator::WriteTextCode (const char* file) {

	if (!codeEmitted)
		return false;

	FILE* fp = fopen(file, "wt");
	if (!fp) {
		DeltaCompMsg(DELTA_COMPILER_FAILED_PREFIX  " to open file '%s' to write target code in text format (%s)", file, strerror(errno));
		return false;
	}

	fprintf(fp, "***SOURCE FILE***\n%s\n", DeltaCompiler::GetSourceFile());
	fprintf(fp, "\n***TOTAL GLOBALS***\n");
	fprintf(fp, "%d\n", ParseParms::GlobalDataSize());
	
	if (DPTR(funcTable)->size()) {
		fprintf(fp,"\n***FUNCTION TABLE***\n");
		std::for_each(
			DPTR(funcTable)->begin(),
			DPTR(funcTable)->end(),
			std::bind2nd(std::mem_fun_ref(&DeltaStdFuncInfo::WriteText), fp)
		 );
	}

	fprintf(fp, "\n***CONSTANTS***\n");			
	constArrays.WriteText(fp);

	debugInfo.WriteText(fp);					

	fprintf(fp, "\n***PROGRAM CODE***\n");
	for (util_ui32 i = 0; i < Total(); ++i)
		WriteInstructionText(i, code[i], fp);

	fclose(fp);
	return true;
}

//-----------------------------------------------------------------
