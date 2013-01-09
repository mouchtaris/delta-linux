// InterCode.h
// Intermediate code generation in the Delta compiler.
// Three address code implementation, with opcodes same as
// those of the target code (VM). Essentially we have the following
// relationship: [i-code quad => t-code instruction] VIA [expr => operand].
// ScriptFighter Project.
// A. Savidis, October 1999.
//

#ifndef	INTERCODE_H
#define	INTERCODE_H

#include <stdio.h>
#include "CompilerComponentDirectory.h"
#include "DeltaByteCodeTypes.h"
#include "utypes.h"
#include "usystem.h"
#include "Expr.h"
#include <list>
#include <string>

////////////////////////////////////////////

typedef util_ui8 DeltaICOpcode;

#define DeltaIC_ADD						DeltaVM_ADD						
#define DeltaIC_OBJGET					DeltaVM_OBJGET					
#define DeltaIC_OBJNEW					DeltaVM_OBJNEW
#define DeltaIC_OBJNEWSET				DeltaVM_OBJNEWSET
#define DeltaIC_OBJNEWEMPTY				DeltaVM_OBJNEWEMPTY
#define DeltaIC_OBJSET					DeltaVM_OBJSET					
#define DeltaIC_ASSIGN					DeltaVM_ASSIGN					
#define DeltaIC_ASSIGNTEMPVAR			DeltaVM_ASSIGNTEMPVAR
#define DeltaIC_CALLEXTFUNC				DeltaVM_CALLEXTFUNC			
#define DeltaIC_CALLPROGRAMFUNC			DeltaVM_CALLPROGRAMFUNC				
#define	DeltaIC_CALLLAMBDAPROGRAMFUNC	DeltaVM_CALLLAMBDAPROGRAMFUNC
#define	DeltaIC_CALLLAMBDAMETHODFUNC	DeltaVM_CALLLAMBDAMETHODFUNC
#define DeltaIC_CALLFUNC				DeltaVM_CALLFUNC				
#define DeltaIC_DIV						DeltaVM_DIV						
#define DeltaIC_FUNCRET					DeltaVM_FUNCRET					
#define DeltaIC_FUNCENTER				DeltaVM_FUNCENTER				
#define DeltaIC_JUMP					DeltaVM_JUMP					
#define DeltaIC_JEQ						DeltaVM_JEQ						
#define DeltaIC_JNE						DeltaVM_JNE						
#define DeltaIC_JGE						DeltaVM_JGE						
#define DeltaIC_JGT						DeltaVM_JGT						
#define DeltaIC_JLE						DeltaVM_JLE						
#define DeltaIC_JLT						DeltaVM_JLT			
#define DeltaIC_JTRUETEST				DeltaVM_JTRUETEST
#define DeltaIC_JFALSETEST				DeltaVM_JFALSETEST
#define DeltaIC_MOD						DeltaVM_MOD						
#define DeltaIC_MUL						DeltaVM_MUL						
#define DeltaIC_PUSHARG					DeltaVM_PUSHARG					
#define DeltaIC_SUB						DeltaVM_SUB						
#define	DeltaIC_OBJDONE					DeltaVM_OBJDONE					
#define DeltaIC_PUSHLATEBOUNDARG		DeltaVM_PUSHLATEBOUNDARG			
#define DeltaIC_BLOCKENTER				DeltaVM_BLOCKENTER				
#define DeltaIC_BLOCKEXIT				DeltaVM_BLOCKEXIT				
#define DeltaIC_BOUNDEDOBJGET			DeltaVM_BOUNDEDOBJGET			
#define DeltaIC_BOUNDEDOBJSET			DeltaVM_BOUNDEDOBJSET			
#define DeltaIC_ASSERT					DeltaVM_ASSERT					
#define DeltaIC_TRAPENABLE				DeltaVM_TRAPENABLE				
#define DeltaIC_TRAPDISABLE				DeltaVM_TRAPDISABLE				
#define DeltaIC_TRAP					DeltaVM_TRAP					
#define DeltaIC_THROW					DeltaVM_THROW					
#define	DeltaIC_OBJNEWATTR				DeltaVM_OBJNEWATTR				
#define	DeltaIC_OBJSETATTR				DeltaVM_OBJSETATTR				
#define	DeltaIC_OBJGETATTR				DeltaVM_OBJGETATTR				
#define	DeltaIC_CALLOBJGETMETHOD		DeltaVM_CALLOBJGETMETHOD		
#define	DeltaIC_CALLOBJBOUNDEDGETMETHOD	DeltaVM_CALLOBJBOUNDEDGETMETHOD	
#define	DeltaIC_FOREACHBEGIN			DeltaVM_FOREACHBEGIN			
#define	DeltaIC_FOREACHCHECKEND			DeltaVM_FOREACHCHECKEND			
#define	DeltaIC_FOREACHGETVAL			DeltaVM_FOREACHGETVAL			
#define	DeltaIC_FOREACHGETINDEX			DeltaVM_FOREACHGETINDEX			
#define	DeltaIC_FOREACHFWD				DeltaVM_FOREACHFWD				
#define	DeltaIC_FOREACHEND				DeltaVM_FOREACHEND				
#define	DeltaIC_PROGRAMBEGIN			DeltaVM_PROGRAMBEGIN
#define	DeltaIC_PROGRAMEND				DeltaVM_PROGRAMEND
#define	DeltaIC_GETRETVAL				DeltaVM_GETRETVAL
#define	DeltaIC_RETURNVAL				DeltaVM_RETURNVAL
#define	DeltaIC_RETURN					DeltaVM_RETURN
#define DeltaIC_NOP						DeltaVM_NOP						

#define	DELTA_TOTAL_ICINSTRUCTIONS		DELTA_TOTAL_VMINSTRUCTIONS

inline bool DeltaIsComparisonOpcode (DeltaICOpcode op) {
	return	op==DeltaIC_JGE		||
			op==DeltaIC_JGT		||
			op==DeltaIC_JLE		||
			op==DeltaIC_JLT;
}

inline bool DeltaIsArithOpOpcode (DeltaICOpcode op) {
	return	op==DeltaIC_ADD		||
			op==DeltaIC_SUB		||
			op==DeltaIC_MUL		||
			op==DeltaIC_DIV		||
			op==DeltaIC_MOD;
}

inline bool DeltaIsModifyingOpcode (DeltaICOpcode op) {	// Excluding direct assigmenmt.
	return	DeltaIsArithOpOpcode(op)		||
			op == DeltaIC_OBJNEW			||
			op == DeltaIC_OBJNEWEMPTY		||
			op == DeltaIC_OBJGET			||
			op == DeltaIC_BOUNDEDOBJGET		||
			op == DeltaIC_OBJGETATTR		||
			op == DeltaIC_FOREACHGETVAL;
}

inline bool DeltaIsBranchOpcode (DeltaICOpcode op) {
	return	DeltaIsComparisonOpcode(op) ||
			op==DeltaIC_JEQ				||
			op==DeltaIC_JNE				||
			op==DeltaIC_JUMP			||
			op==DeltaIC_JTRUETEST		||
			op==DeltaIC_JFALSETEST;		
}

////////////////////////////////////////////

#define	DELTA_QUAD_ALLOCATION_SIZE					4096u
#define	DELTA_NIL_QUAD_LABEL						0
#define	DELTA_START_QUAD							1
#define	DELTA_ORPHAN_METHOD_FUNCENTER_OFFSET		+1

#define	DELTA_RECURSIVE_PROGRAMFUNC_USERDATA		1
#define	DELTA_RECURSIVE_METHODFUNC_USERDATA			2
#define	DELTA_NONRECURSIVE_FUNC_USERDATA			0

////////////////////////////////////////////

typedef util_ui32 DeltaQuadAddress;

struct DeltaQuad {

	DeltaICOpcode		opcode;
	DeltaExpr*			arg1;
	DeltaExpr*			arg2;
	DeltaExpr*			result;
	DeltaQuadAddress	label;			// Label of goto in icode.

	DeltaMemAddress		targetLabel;	// Address in target code.

	util_ui16			line;			// Source code line.
	util_ui16			funcSerial;		// Increased by 1, 0 meaning global.
	bool				dropped;		// If dropped due to optimization.
	void*				userData;		// Extra user-specific information.

	void				MarkAsDropped (void) 
							{ dropped = true; }
	bool				IsDropped (void) const 
							{ return dropped; }
	void				MarkAsUsed (void) 
							{ dropped = false; }
	void				SetUserData (void* p) 
							{ userData = p; }
	void*				GetUserData (void) const 
							{ return userData; }

	DeltaICOpcode		GetOpCode (void) const
							{ return opcode; }
	const DeltaExpr*	GetArg1 (void) const 
							{ return arg1; }
	DeltaExpr*			GetArg1 (void)
							{ return arg1; }
	const DeltaExpr*	GetArg2 (void) const 
							{ return arg2; }
	DeltaExpr*			GetArg2 (void)
							{ return arg2; }
	const DeltaExpr*	GetResult (void) const 
							{ return result; }
	DeltaExpr*			GetResult (void)
							{ return result; }
	DeltaQuadAddress	GetLabel (void) const 
							{ return label; }
	util_ui32			GetLine (void) const
							{ return line; }

	void				SetOpCode (DeltaICOpcode _opcode)
							{ DASSERT(_opcode < DELTA_TOTAL_ICINSTRUCTIONS); opcode = _opcode; }
	void				SetArg1 (DeltaExpr* e)		
							{ arg1 = e; }
	void				SetArg2 (DeltaExpr* e)		
							{ arg2 = e; }
	void				SetResult (DeltaExpr* e)	
							{ result = e; }
	void				SetLabel (DeltaQuadAddress l)
							{ label = l; }
	void				SetLine (util_ui16 _line)
							{ DASSERT(_line); if (!line) line = _line; }

	void				WriteText (util_ui32 quadNo, FILE* fp);

	void				operator=(const DeltaQuad& quad);

	DeltaQuad (void) {
		opcode		= DeltaIC_NOP;
		arg1		= 
		arg2		= 
		result		= NIL_EXPR;
		label		= DELTA_NIL_QUAD_LABEL;
		targetLabel = 
		line		= 0;		// 0 for line means retain previous value.
		dropped		= false;
		userData	= (void*) DELTA_NONRECURSIVE_FUNC_USERDATA;
	}
};

////////////////////////////////////////////

class DeltaQuadManager : public ucomponentdirectoryclient {

	////////////////////////////////////////////////////

	private:
	DeltaQuadAddress	currQuad;
	DeltaQuadAddress	iterCounter;
	DeltaQuad*			quads;
	util_ui16			allocationsMade;
	util_ui32			size;
	bool				quadsEmitted;
	ubag<DeltaSymbol*>	usedLibraryConsts;

	bool				IsJump (util_ui16 quad) const {
							return	quads[quad].opcode == DeltaIC_JUMP		||
									quads[quad].opcode == DeltaIC_JUMP		||
									quads[quad].opcode == DeltaIC_JEQ		||	
									quads[quad].opcode == DeltaIC_JNE		||	
									quads[quad].opcode == DeltaIC_JGE		||	
									quads[quad].opcode == DeltaIC_JGT		||	
									quads[quad].opcode == DeltaIC_JLE		||	
									quads[quad].opcode == DeltaIC_JLT		||
									quads[quad].opcode == DeltaIC_JTRUETEST	||
									quads[quad].opcode == DeltaIC_JFALSETEST;
						}

	void				Append (DeltaQuad& quad);
	void				Reset (void);
	void				Expand (void);
	DeltaQuadAddress	CurrQuadNoPlus (util_ui32 n);

	////////////////////////////////////////////////////

	public:
	DeltaQuadAddress	CurrQuadNo (void)
							{ DASSERT(quadsEmitted); return currQuad; }

	DeltaQuadAddress	NextQuadNo (void)
							{ return CurrQuadNoPlus(1); }

	DeltaQuadAddress	NextQuadNoPlus (util_ui32 n) 
							{ return CurrQuadNoPlus(n + 1); }

	DeltaQuadAddress	Total (void) const
							{ return currQuad+1; }

	DeltaICOpcode		WhichSetOpcode (const DeltaExpr* e) const {
							DASSERT(DPTR(e)->GetType()== DeltaExprTableElement);
							return	DPTR(e)->IsAttribute()?	DeltaIC_OBJSETATTR :
									DPTR(e)->IsBoundedElement() ?	DeltaIC_BOUNDEDOBJSET : DeltaIC_OBJSET;
						}

	DeltaICOpcode		WhichGetOpcode (const DeltaExpr* e) const {
							DASSERT(DPTR(e)->GetType()== DeltaExprTableElement);
							return	DPTR(e)->IsAttribute()?	DeltaIC_OBJGETATTR :
									DPTR(e)->IsBoundedElement() ?	DeltaIC_BOUNDEDOBJGET : DeltaIC_OBJGET;
						}

	//**************************

	void				AddUsedLibraryConst (DeltaSymbol* sym)
							{ usedLibraryConsts.insert(sym); }
	const ubag<DeltaSymbol*>&		
						GetUsedLibraryConsts (void) const			
							{ return usedLibraryConsts; }

	//**************************

	void				StartIteration (void)
							{ iterCounter = DELTA_START_QUAD; }
	bool				Iterate (DeltaQuad* quad);

	// Sets the target label of a quad. We need to know the
	// corresponding target instruction for each quad, so
	// that upon code generation we can patch jumps.
	//
	void				SetQuadTargetLabel (util_ui32 quadNo, util_ui32 targetLabel) {
							DASSERT(quadNo < Total());
							quads[quadNo].targetLabel = targetLabel;
						}

	util_ui32			GetQuadTargetLabel (util_ui32 quadNo) {
							DASSERT(quadNo < Total());
							return quads[quadNo].targetLabel; 
						}

	void				SetQuadLine (DeltaQuadAddress quadNo, util_ui16 line, bool isExprStmt = false);

	const DeltaQuad&	GetQuad (util_ui32 quadNo) const {
							DASSERT(quadNo < Total());
							return quads[quadNo]; 
						}

	const DeltaQuad&	GetCurrQuad (void) const 
							{ return quads[currQuad];  }

	DeltaQuad&			GetQuad (util_ui32 quadNo) {
							DASSERT(quadNo < Total());
							return quads[quadNo]; 
						}

	DeltaQuad&			GetCurrQuad (void)
							{ return quads[currQuad];  }

	util_ui16			GetQuadLine (DeltaQuadAddress quadNo) {
							DASSERT(quadNo < Total());
							return quads[quadNo].line;
						}

	//**************************

	void				Emit (
							DeltaICOpcode		opcode,
							DeltaExpr*			arg1,
							DeltaExpr*			arg2,
							DeltaExpr*			result,
							DeltaQuadAddress	label = DELTA_NIL_QUAD_LABEL
						);

	//**************************

	DeltaQuadAddress	MakeList (DeltaQuadAddress i);
	DeltaQuadAddress	Merge (DeltaQuadAddress qlist1, DeltaQuadAddress qlist2);
	void				Backpatch (DeltaQuadAddress qlist, DeltaQuadAddress target);
	void				Patch (DeltaQuadAddress quad, DeltaQuadAddress label);

	//**************************
	void				BackpatchExplicitTrapDisables (
							DeltaQuadAddress	qlist,
							DeltaQuadAddress	loopStmtBegin
						);

	void				BackpatchBlockExits (
							DeltaQuadAddress	qlist, 
							DeltaQuadAddress	target,
							void				(*quadStepFunc)(util_ui32*),
							void				(*onBlockExitFunc)(util_i8*),
							void				(*onBlockEnterFunc)(util_i8*)
						);

	void				BackpatchBlockExitsUp (
							DeltaQuadAddress qlist, 
							DeltaQuadAddress target
						);

	void				BackpatchBlockExitsDown (
							DeltaQuadAddress qlist, 
							DeltaQuadAddress target
						);

	//**************************

	bool				JumpDoesNotCrossCurrentFunction (DeltaQuadAddress jump, DeltaQuadAddress target) const;
	void				ValidateIfJump (const DeltaQuad& q, DeltaQuadAddress i) const;
	void				BeforeOptimizationValidation (void) const;
	void				AfterOptimizationValidation (void) const;
	void				WriteText (const char* file);
	void				CleanUp (void) { Reset(); }

	////////////////////////////////////////////////////

	DeltaQuadManager (ucomponentdirectory* directory);
	~DeltaQuadManager();
};

////////////////////////////////////////////

#define QUADS_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), DeltaQuadManager)))

#define QUADS	QUADS_EX(GET_COMPONENT_DIRECTORY())

////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
