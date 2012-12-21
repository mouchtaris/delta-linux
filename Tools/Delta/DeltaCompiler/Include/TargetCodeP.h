// Members of the DeltaCodeGenerator class.
// To be included directly in the class body.
// ScriptFighter Project.
// A. Savidis, October 1999 (original version).
//

//-------------------------------------------------------------------

void Generate_Instruction (
		DeltaVMOpcode	opcode,
		DeltaQuad&		quad,
		util_ui32		quadNo
	);

void Generate_ADD (DeltaQuad &quad, util_ui32 quadNo);
void Generate_DIV (DeltaQuad &quad, util_ui32 quadNo);
void Generate_MOD (DeltaQuad &quad, util_ui32 quadNo);
void Generate_MUL (DeltaQuad &quad, util_ui32 quadNo);
void Generate_SUB (DeltaQuad &quad, util_ui32 quadNo);

void Generate_OBJGET (DeltaQuad &quad, util_ui32 quadNo);
void Generate_BOUNDEDOBJGET (DeltaQuad &quad, util_ui32 quadNo);
void Generate_OBJNEW (DeltaQuad &quad, util_ui32 quadNo);
void Generate_OBJNEWSET (DeltaQuad &quad, util_ui32 quadNo);
void Generate_OBJNEWEMPTY (DeltaQuad &quad, util_ui32 quadNo);
void Generate_OBJDONE (DeltaQuad &quad, util_ui32 quadNo);
void Generate_OBJSET (DeltaQuad &quad, util_ui32 quadNo);
void Generate_BOUNDEDOBJSET (DeltaQuad &quad, util_ui32 quadNo);
void Generate_ASSIGN (DeltaQuad &quad, util_ui32 quadNo);
void Generate_ASSIGNTEMPVAR (DeltaQuad &quad, util_ui32 quadNo);
void Generate_NOP (DeltaQuad &quad, util_ui32 quadNo);
void Generate_OBJNEWATTR (DeltaQuad &quad, util_ui32 quadNo);
void Generate_OBJSETATTR (DeltaQuad &quad, util_ui32 quadNo);
void Generate_OBJGETATTR (DeltaQuad &quad, util_ui32 quadNo);
void Generate_CALLOBJGETMETHOD (DeltaQuad &quad, util_ui32 quadNo);
void Generate_CALLOBJBOUNDEDGETMETHOD (DeltaQuad &quad, util_ui32 quadNo);

void Generate_ActualArgument (DeltaVMOpcode opcode, DeltaQuad &quad, util_ui32 quadNo);
void Generate_PUSHARG(DeltaQuad &quad, util_ui32 quadNo);
void Generate_PUSHLATEBOUNDARG(DeltaQuad &quad, util_ui32 quadNo);
void Generate_FunctionCall (DeltaVMOpcode opcode, DeltaQuad &quad, util_ui32 quadNo);
void Generate_MethodCall (DeltaVMOpcode opcode, DeltaQuad &quad, util_ui32 quadNo);
void Generate_CALLEXTFUNC (DeltaQuad &quad, util_ui32 quadNo);
void Generate_CALLFUNC (DeltaQuad &quad, util_ui32 quadNo);
void Generate_FUNCRET (DeltaQuad &quad, util_ui32 quadNo);
void Generate_FUNCENTER (DeltaQuad &quad, util_ui32 quadNo);

void Generate_JUMP(DeltaQuad &quad, util_ui32 quadNo);
void Generate_Relational (
			DeltaQuad&			quad,
			DeltaVMOpcode		opcode,
			DeltaQuadAddress	quadNo
		);
void Generate_JEQ(DeltaQuad &quad, util_ui32 quadNo);
void Generate_JNE(DeltaQuad &quad, util_ui32 quadNo);
void Generate_JGE(DeltaQuad &quad, util_ui32 quadNo);
void Generate_JGT(DeltaQuad &quad, util_ui32 quadNo);
void Generate_JLE(DeltaQuad &quad, util_ui32 quadNo);
void Generate_JLT(DeltaQuad &quad, util_ui32 quadNo);
void Generate_JTRUETEST(DeltaQuad &quad, util_ui32 quadNo);
void Generate_JFALSETEST(DeltaQuad &quad, util_ui32 quadNo);

void Generate_BLOCKENTER (DeltaQuad &quad, util_ui32 quadNo);
void Generate_BLOCKEXIT (DeltaQuad &quad, util_ui32 quadNo);
void Generate_ASSERT (DeltaQuad &quad, util_ui32 quadNo);
void Generate_TRAPENABLE (DeltaQuad &quad, util_ui32 quadNo);
void Generate_TRAPDISABLE (DeltaQuad &quad, util_ui32 quadNo);
void Generate_TRAP (DeltaQuad &quad, util_ui32 quadNo);
void Generate_THROW (DeltaQuad &quad, util_ui32 quadNo);

void Generate_FOREACHBEGIN (DeltaQuad &quad, util_ui32 quadNo);
void Generate_FOREACHCHECKEND (DeltaQuad &quad, util_ui32 quadNo);
void Generate_FOREACHGETVAL (DeltaQuad &quad, util_ui32 quadNo);
void Generate_FOREACHGETINDEX (DeltaQuad &quad, util_ui32 quadNo);
void Generate_FOREACHFWD	(DeltaQuad &quad, util_ui32 quadNo);	
void Generate_FOREACHEND	(DeltaQuad &quad, util_ui32 quadNo);	
void Generate_RETURNVAL	(DeltaQuad &quad, util_ui32 quadNo);
void Generate_RETURN	(DeltaQuad &quad, util_ui32 quadNo);	
void Generate_GETRETVAL	(DeltaQuad &quad, util_ui32 quadNo);	