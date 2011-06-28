/////////////////////////////////////////////////////////
// This file is directly included in DeltaVirtualMachine
// body. In only contains friend classes and functions.
// Script Fighter Project.
// A. Savidis, October 1999 (original version)
//

/////////////////////////////////////////////////////////

friend class	DeltaValue;
friend class	DeltaClosureCreator;
friend class	DeltaClosureHandling;
friend struct	DeltaValue::Operations;
friend class	OperatorOverloading;
friend class	DeltaExceptionHandling;
friend class	DeltaDebugExtensions;
friend class	DeltaLibraryFuncArgsBinder;
friend class	ActualArgsPreserver;

template <class OpTraits> friend struct GetElemTemplate;
template <class OpTraits> friend struct SetElemTemplate;

/////////////////////////////////////////////////////////

friend void Execute_PROGRAMBEGIN (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_PROGRAMEND (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_ASSERT (DeltaInstruction* instr, DeltaVirtualMachine* vm);

friend void Execute_OBJNEW (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_OBJNEWSET (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_OBJNEWEMPTY (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_OBJDONE (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_OBJGET (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_BOUNDEDOBJGET (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_OBJSET (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_BOUNDEDOBJSET (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_OBJNEWATTR (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_OBJSETATTR (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_OBJGETATTR (DeltaInstruction*, DeltaVirtualMachine*);

friend void Execute_PUSHLATEBOUNDARG (DeltaInstruction* instr, DeltaVirtualMachine* vm, util_ui32 sn, const util_ui32* passport);
friend void Execute_PUSHARG (DeltaInstruction* instr, DeltaVirtualMachine* vm, util_ui32 sn, const util_ui32* passport);
friend void Execute_PUSHARG (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_PUSHLATEBOUNDARG (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_CALLFUNC (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_CALLPROGRAMFUNC (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_CALLLAMBDAPROGRAMFUNC (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_CALLLAMBDAMETHODFUNC (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_GETFUNCLEVEL (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_CALLEXTFUNC (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_CALLEXTPREBOUNDFUNC (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_CALLOBJGETMETHOD (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_CALLOBJBOUNDEDGETMETHOD (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_CALLOBJGETMETHOD (DeltaInstruction* instr, DeltaVirtualMachine* vm, util_ui32 sn, const util_ui32* passport);
friend void Execute_CALLOBJBOUNDEDGETMETHOD (DeltaInstruction* instr, DeltaVirtualMachine* vm, util_ui32 sn, const util_ui32* passport);
friend void Execute_BLOCKENTER (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_BLOCKEXIT (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_BLOCKENTERWithoutDebugger (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_BLOCKEXITWithoutDebugger (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_FUNCRET (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_FUNCENTER (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_FUNCRETWithoutDebugger (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_FUNCENTERWithoutDebugger (DeltaInstruction* instr, DeltaVirtualMachine* vm);

friend void Execute_JUMP (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_JEQ (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_JNE (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_JGE (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_JGT (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_JLE (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_JLT (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_JTRUETEST (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_JFALSETEST (DeltaInstruction*, DeltaVirtualMachine*);

friend void Execute_ASSIGN (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_ASSIGNTEMPVAR (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_RETURNVAL (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_RETURN (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_GETRETVAL (DeltaInstruction*, DeltaVirtualMachine*);

friend void Execute_ADD (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_SUB (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_MUL (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_DIV (DeltaInstruction*, DeltaVirtualMachine*);
friend void Execute_MOD (DeltaInstruction*, DeltaVirtualMachine*);

friend void Execute_TRAPENABLE (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_TRAPDISABLE (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_THROW (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_TRAP (DeltaInstruction* instr, DeltaVirtualMachine* vm);

friend void Execute_FOREACHBEGIN (DeltaInstruction* instr, DeltaVirtualMachine* vm);	
friend void Execute_FOREACHCHECKEND (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_FOREACHGETVAL (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_FOREACHGETINDEX (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_FOREACHFWD (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void Execute_FOREACHEND (DeltaInstruction* instr, DeltaVirtualMachine* vm);

/////////////////////////////////////////////////////////
// Optimized table get instructions.

friend void TableGetConstStringUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableBoundedGetConstStringUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableGetConstStringUserVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableBoundedGetConstStringUserVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableGetConstStringTempVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableBoundedGetConstStringTempVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableGetConstStringTempVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableBoundedGetConstStringTempVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableGetConstNumberUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableBoundedGetConstNumberUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableGetConstNumberUserVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableBoundedGetConstNumberUserVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableGetConstNumberTempVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableBoundedGetConstNumberTempVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableGetConstNumberTempVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableBoundedGetConstNumberTempVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);

friend void ExternIdGetConstStringUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdBoundedGetConstStringUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdGetConstStringUserVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdBoundedGetConstStringUserVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdGetConstStringTempVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdBoundedGetConstStringTempVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdGetConstStringTempVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdBoundedGetConstStringTempVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdGetConstNumberUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdBoundedGetConstNumberUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdGetConstNumberUserVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdBoundedGetConstNumberUserVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdGetConstNumberTempVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdBoundedGetConstNumberTempVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdGetConstNumberTempVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdBoundedGetConstNumberTempVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);

/////////////////////////////////////////////////////////
// Optimized table set instructions.

friend void TableSetConstStringContentVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableBoundedSetConstStringContentVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableSetConstStringContentNotVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableBoundedSetConstStringContentNotVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableSetConstNumberContentVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableBoundedSetConstNumberContentVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableSetConstNumberContentNotVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableBoundedSetConstNumberContentNotVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);

friend void ExternIdSetConstStringContentVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdBoundedSetConstStringContentVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdSetConstStringContentNotVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdBoundedSetConstStringContentNotVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdSetConstNumberContentVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdBoundedSetConstNumberContentVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdSetConstNumberContentNotVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdBoundedSetConstNumberContentNotVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);

friend void MethodSetSelfConstStringIndexCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void MethodSetSelfUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void MethodBoundedSetSelfConstStringIndexCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void MethodBoundedSetSelfUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);

/////////////////////////////////////////////////////////
// Optimized method invocation instructions.

friend void TableCallGetMethod (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void TableCallBoundedGetMethod (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdCallGetMethod (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void ExternIdCallBoundedGetMethod (DeltaInstruction* instr, DeltaVirtualMachine* vm);

friend void MethodGetSelfConstStringIndexCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void MethodGetSelfUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void MethodBoundedGetSelfConstStringIndexCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);
friend void MethodBoundedGetSelfUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm);

/////////////////////////////////////////////////////////
// The following macros rely on the standard offsets defined in delta types.
//
#define	DELTA_TOPSP_TOTALARGS_OFFSET		DELTA_SAVED_NUMACTUALS_OFFSET
#define	DELTA_TOPSP_PC_OFFSET				DELTA_SAVED_PC_OFFSET
#define	DELTA_TOPSP_OLDTOP_OFFSET			DELTA_SAVED_TOP_OFFSET
#define	DELTA_TOPSP_OLDTOPSP_OFFSET			DELTA_SAVED_TOPSP_OFFSET
#define	DELTA_TOPSP_FIRSTARG_OFFSET			(DELTA_SAVED_ENVIRONMENT_SIZE+1)
#define	DELTA_TOPSP_SELF_OFFSET				DELTA_TOPSP_FIRSTARG_OFFSET

//***************************
// The return value is a reserved stack object, which can
// be taken from the top of the stack (first global variable).
//
#define	DELTA_RETVALPOS						(stackSize - 1 - DELTA_RETVALUE_OFFSET)
#define	DELTA_RETVAL_VAR					stack[DELTA_RETVALPOS]

/////////////////////////////////////////////////////////
