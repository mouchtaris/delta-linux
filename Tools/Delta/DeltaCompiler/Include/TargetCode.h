// TargetCode.h
// Target code generation for the Delta compiler.
// ScriptFighter Project.
// A. Savidis, October 1999.
// Revised many times later.
//

#ifndef	TARGETCODE_H
#define	TARGETCODE_H

#include <limits.h>
#include <string.h>
#include <stdio.h>
#include "DeltaByteCodeTypes.h"
#include "Expr.h"
#include "DeltaConstantProgramValues.h"
#include "InterCode.h"
#include <list>
#include <map>

#define	DELTA_CODE_ALLOCATION_SIZE	4096
#define	DELTA_JUMP_ALLOCATION_SIZE	1024

class DeltaCodeGenerator : public ucomponentdirectoryclient {

	DFRIENDDESTRUCTOR()

	#include "TargetCodeP.h"
	private:

	//-------------------------------------------------------------------

	typedef void (DeltaCodeGenerator::*GenerateFunc)(DeltaQuad&, util_ui32);

	std::list<DeltaStdFuncInfo>*		funcTable;
	DeltaInstruction*					code;
	std::list<DeltaCodeAddress>			unfinishedJumps;
	util_ui32							currSize;
	DeltaCodeAddress					currInstr;
	bool								codeEmitted;
	DeltaConstantArraysWriter			constArrays;	// Arrays of constant values.
	DeltaDebugProgramInfo				debugInfo;
	std::map<util_ui32, GenerateFunc>	generateFuncs;
	util_ui32							currLine;

	//-------------------------------------------------------------------

	void					ExpandCode (void);
	void					ProduceDebugInfo (void);
	void					ProduceDebugInfoForSymbols (void);
	void					ProduceDebugInfoForOpenedNamespaces(void);
	void					ProduceDebugInfoForFunction (DeltaSymbol* sym);
	void					ProduceDebugInfoForStaticVar (DeltaSymbol* sym);
	void					ProduceDebugInfoForGlobalVar (DeltaSymbol* sym);
	void					ProduceDebugInfoForFormalArgOrLocalVar (DeltaSymbol* sym);
	void					ProduceDebugInfoForUsedLibraryConsts(void);
	void					ProduceDebugInfoForCodeLineChunks (void);

	void					WriteFunctionTable (GenericWriter& writer);
	void					WriteInstructions (GenericWriter& writer);
	void					WriteDebugInfo (GenericWriter& writer, bool includeDebugInfo);

	void					WriteInstructionText (
								DeltaCodeAddress	addr, 
								DeltaInstruction&	instr, 
								FILE*				fp
							);

	const std::string		Handle (DeltaOperand* operand);

	//////////////////////////////////////////////////////////////////////

	DeltaStdFuncInfo*		GetFuncInfo (util_ui32 serial);
	void					ProduceFuncInfo (const DeltaSymbol* func, DeltaStdFuncInfo* info);

	void					AddUnfinishedJump (DeltaCodeAddress jumpInstr)
								{ unfinishedJumps.push_back(jumpInstr); }
	void					PatchUnfinishedJumps (void);

	util_ui32				Total (void) const
								{ return currInstr+1; }
	DeltaCodeAddress		CurrInstructionNo (void) const
								{ return currInstr; }
	DeltaCodeAddress		NextInstructionNo (void) const
								{ return codeEmitted ? currInstr+1 : currInstr; }

	//-------------------------------------------------------------------

	void					MakeOperand (DeltaExpr* expr, DeltaOperand* operand);
	void					MakeInternal (DeltaExpr* expr, DeltaOperand* operand);
	void					MakeNotUsed(DeltaOperand* operand);
	void					MakeNumber (DeltaOperand* operand, DeltaNumberValueType	value);
	void					MakeLabel(DeltaOperand* operand, util_ui32 label);
	void					MakeBool(DeltaOperand* operand, bool val);

	void					EmitStartUpInstruction (void);
	void					EmitProgramEndInstruction (void);
	void					Emit (DeltaInstruction* instr);
	void					Reset (void);

	//-------------------------------------------------------------------

	public:
	void					GenerateCode (void);

	bool					WriteBinaryCode (const char* file, bool includeDebugInfo = false);
	bool					WriteBinaryCode (GenericWriter& writer, bool includeDebugInfo = false);
	bool					WriteTextCode (const char* file);

	void					CleanUp (void);

	////////////////////////////////////////////

	DeltaCodeGenerator (ucomponentdirectory* directory);
	~DeltaCodeGenerator();
};

////////////////////////////////////////////

#define CODEGENERATOR_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), DeltaCodeGenerator)))

#define CODEGENERATOR	CODEGENERATOR_EX(GET_COMPONENT_DIRECTORY())

////////////////////////////////////////////

#endif	// Do not ad stuff beyond this point.