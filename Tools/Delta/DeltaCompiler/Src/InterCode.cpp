// InterCode.cpp
// Intermediate code generation in the Delta compiler.
// ScriptFighter Project.
// A. Savidis, original version October 1999.
// Most changes concerned new types of instructions.
//

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "DDebug.h"
#include "Symbol.h"
#include "DeltaByteCodeTypes.h"
#include "InterCode.h"
#include "ParseParms.h"
#include "DeltaCompErrorMsg.h"
#include "ufunctors.h"

//------------------------------------------------------------------

static const char* quadOpcodes[DELTA_TOTAL_ICINSTRUCTIONS] = {
	"ADD",
	"OBJGET",
	"OBJNEW",
	"OBJNEWSET",
	"OBJNEWEMPTY",
	"OBJSET",
	"ASSIGN",
	"ASSIGNTEMPVAR",
	"CALLFUNC",
	(const char*) 0,	//	"CALLPROGRAMFUNC",
	(const char*) 0,	//	"CALLLAMBDAPROGRAMFUNC",
	(const char*) 0,	//	"CALLLAMBDAMETHODFUNC",
	"CALLEXTFUNC",
	(const char*) 0,	//  "CALLEXTPREBOUNDFUNC",
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

////////////////////////////////////////////////////////////////

void DeltaQuad::WriteText (util_ui32 quadNo, FILE* fp) {

	DASSERT(opcode < uarraysize(quadOpcodes));

	fprintf(fp,"%u(%d)%s:\t", quadNo, line, dropped? "OFF" : "");
	fprintf(fp,"%s ", DNULLCHECK(quadOpcodes[opcode]));
	fprintf(fp,"%s ", DeltaExpr::Handle(arg1).c_str());
	fprintf(fp,"%s ", DeltaExpr::Handle(arg2).c_str());
	fprintf(fp,"%s ", DeltaExpr::Handle(result).c_str());
	fprintf(fp,"%u\n", label);
}

//------------------------------------------------------------------

DeltaQuadManager* DeltaQuadManager::singletonPtr = (DeltaQuadManager*) 0;

void DeltaQuadManager::SingletonCreate (void) {
	if (!singletonPtr)
		singletonPtr = DNEW(DeltaQuadManager);
}

void DeltaQuadManager::SingletonDestroy (void) {
	if (singletonPtr) {
		DDELETE(singletonPtr);
		singletonPtr = (DeltaQuadManager*) 0;
	}
}

//------------------------------------------------------------------

DeltaQuadManager::DeltaQuadManager (void) {

	Reset();
	size	= (allocationsMade = 1) * DELTA_QUAD_ALLOCATION_SIZE;
	quads	= (DeltaQuad*) DNEWARR(DeltaQuad, size);
}

////////////////////////////////////////////////////////////////
// Simply destroy the quads memory, if any.
//
DeltaQuadManager::~DeltaQuadManager () {
	DASSERT(quads);
	DDELARR(quads);
}

////////////////////////////////////////////////////////////////

void DeltaQuadManager::Reset (void) {
	currQuad		= DELTA_START_QUAD;
	iterCounter		= 0;
	quadsEmitted	= false;
	usedLibraryConsts.clear();
}

////////////////////////////////////////////////////////////////

DeltaQuadAddress DeltaQuadManager::CurrQuadNoPlus (util_ui32 n) {
	DASSERT(n);
	if (!quadsEmitted)	// The current quad no is also the next quad no (initial case).
		--n;
	while (currQuad + n >= size)
			Expand();
	return currQuad + n;
}

////////////////////////////////////////////////////////////////

void DeltaQuadManager::Expand (void) {
	DASSERT(currQuad <= size);
	util_ui32 oldSize = size;
	size = (++allocationsMade) * DELTA_QUAD_ALLOCATION_SIZE;
	DeltaQuad* newQuads = DNEWARR(DeltaQuad, size);
	memcpy(newQuads, quads, oldSize * sizeof(DeltaQuad));
	DDELARR(quads);
	quads = newQuads;
}

////////////////////////////////////////////////////////////////

void DeltaQuad::operator=(const DeltaQuad& quad) {
	opcode			= quad.opcode;
	arg1			= quad.arg1;
	arg2			= quad.arg2;
	result			= quad.result;
	label			= quad.label;
	line			= quad.line;
	funcSerial		= quad.funcSerial;
	dropped			= quad.dropped;
	userData		= quad.userData;
}

void DeltaQuadManager::Append (DeltaQuad& quad) {

	if (quadsEmitted)
		++currQuad;
	else
		quadsEmitted = true;
	
	if (currQuad == size)
		Expand();

	quads[currQuad] = quad;
}

////////////////////////////////////////////////////////////////

bool DeltaQuadManager::Iterate (DeltaQuad* quad) {
	if (!quadsEmitted)				// No quads yet.
		return false;
	else
	if (iterCounter > currQuad)		// Last emitted quad reached.
		return false;
	else {
		memcpy(quad, quads + iterCounter, sizeof(DeltaQuad));
		++iterCounter;
		return true;
	}
}

////////////////////////////////////////////////////////////////

void DeltaQuadManager::Emit (
		DeltaICOpcode		opcode,
		DeltaExpr*			arg1,
		DeltaExpr*			arg2,
		DeltaExpr*			result,
		DeltaQuadAddress	label
	) {

	DeltaQuad quad;
	quad.opcode		= opcode;
	quad.arg1		= arg1;
	quad.arg2		= arg2;
	quad.result		= result;
	quad.label		= label;
	quad.line		= 0;
	quad.dropped	= ParseParms::InProductionMode() && ParseParms::InAssertStmt().inside();
	quad.funcSerial = ParseParms::InsideFuncSerial();
	
	Append(quad);
}

//------------------------------------------------------------------

DeltaQuadAddress DeltaQuadManager::MakeList (DeltaQuadAddress i) {
	
	quads[i].label = DELTA_NIL_QUAD_LABEL;
	return i;
}

////////////////////////////////////////////////////////////////

DeltaQuadAddress DeltaQuadManager::Merge (DeltaQuadAddress list1, DeltaQuadAddress list2) {

	if (list1 == DELTA_NIL_QUAD_LABEL)
		return list2;
	else
	if (list2 == DELTA_NIL_QUAD_LABEL)
		return list1;
	else {
		DeltaQuadAddress i;
		for (i = list1; quads[i].label != DELTA_NIL_QUAD_LABEL; i = quads[i].label)
			;

		quads[i].label = list2;
		return list1;
	}
}

////////////////////////////////////////////////////////////////

void DeltaQuadManager::Patch(DeltaQuadAddress quadNo, DeltaQuadAddress label) {
	DASSERT(quadNo <= currQuad);
	quads[quadNo].label = label;
}

//***************************

void DeltaQuadManager::Backpatch (DeltaQuadAddress qlist, DeltaQuadAddress label) {

	while (qlist != DELTA_NIL_QUAD_LABEL) {

		DASSERT(qlist <= currQuad);
		DASSERT(IsJump(qlist));

		DeltaQuadAddress temp	= quads[qlist].label;
		quads[qlist].label		= label;
		qlist					= temp;
	}
}

//***************************
// For every JUMP instruction in 'qlist', it scans all quads
// in-between the 'target' quad, and computes the number of
// block exits that will be bypassed due to the jump. The direction
// of scanning (i.e. upwards or downwards) is defined by the
// three supplied functions.
//
void DeltaQuadManager::BackpatchBlockExits (
		DeltaQuadAddress	qlist,
		DeltaQuadAddress	target,
		void				(*quadStepFunc)(util_ui32*),
		void				(*onBlockExitFunc)(util_i8*),
		void				(*onBlockEnterFunc)(util_i8*)
	) {

	DASSERT(qlist != target);

	while (qlist != DELTA_NIL_QUAD_LABEL) {
	
		DASSERT(quads[qlist].opcode == DeltaIC_JUMP);

		util_i8	blockExitCounter = 0;

		for (util_ui32 quadNo = qlist; quadNo != target; (*quadStepFunc)(&quadNo))
			if (quads[quadNo].opcode == DeltaIC_BLOCKEXIT)
				(*onBlockExitFunc)(&blockExitCounter);
			else
			if (quads[quadNo].opcode == DeltaIC_BLOCKENTER)
				(*onBlockEnterFunc)(&blockExitCounter);

		DASSERT(blockExitCounter >= 0);
		quads[qlist].result = DeltaExpr::Make(blockExitCounter);
		qlist				= quads[qlist].label;
	}
}

//***************************

void DeltaQuadManager::BackpatchBlockExitsUp (
		DeltaQuadAddress qlist, 
		DeltaQuadAddress target
	) { 
	BackpatchBlockExits(
		qlist, 
		target, 
		ustep<util_ui32>::dec, 
		ustep<util_i8>::dec, 
		ustep<util_i8>::inc
	);
}

//***************************

void DeltaQuadManager::BackpatchBlockExitsDown (
		DeltaQuadAddress qlist, 
		DeltaQuadAddress target
	){ 
	BackpatchBlockExits(
		qlist, 
		target, 
		ustep<util_ui32>::inc, 
		ustep<util_i8>::inc, 
		ustep<util_i8>::dec
	);
}

//***************************
// Expressions 'x;' 'local x;' 'static x;' do not
// generate a quad. Hence, in this case quadNo will be
// currQuad + 1, that is Total(). Also, the JUMP preceeding
// function definitions should never get a line number,
// as also the FUNSTART that follows.
//
void  DeltaQuadManager::SetQuadLine (DeltaQuadAddress quadNo, util_ui16 line, bool isExprStmt) {
	
	// The main block of a function can never be given a line number.
	DASSERT(quads[quadNo].opcode != DeltaIC_FUNCENTER);

	if (quadNo < Total()) {

		if (quads[quadNo].opcode == DeltaIC_JUMP	&& 
			quadNo + 1 < Total()					&&
			quads[quadNo + 1].opcode == DeltaIC_FUNCENTER) {

			if (isExprStmt) {											// Check if a function def used directly

				DeltaQuadAddress endQuad = CurrQuadNo();				// That's exactly the ending quad
				DASSERT(quadNo < endQuad - 1);							// Can't be equal since there is func def in between

				if (quads[endQuad].opcode != DeltaIC_FUNCRET) {			// The function def may be directly used

					while (quads[endQuad].opcode != DeltaIC_FUNCRET)	// Find the func end quad
						{ DASSERT(quadNo < endQuad); --endQuad; }
					
					// Now move to the next quad. 
					// That's the first quad of the func use expression

					++endQuad; 
					quads[endQuad].SetLine(line);
				}
			}
		}
		else 
			quads[quadNo].SetLine(line);
	}
	else
		DASSERT(quadNo == Total());
}

//------------------------------------------------------------------


void DeltaQuadManager::WriteText (const char* file) {

	if (FILE* fp = fopen(file, "wt")) {
		if (quadsEmitted)
			for (util_ui32 i = DELTA_START_QUAD; i < Total(); ++i)
				quads[i].WriteText(i, fp);
		fclose(fp);	
	}
	else
		DeltaCompMsg(
			DELTA_COMPILER_FAILED_PREFIX " to open file '%s' for writing intermediate code (%s).\n", 
			file, 
			strerror(errno)
		);
}

//------------------------------------------------------------------

bool DeltaQuadManager::JumpDoesNotCrossCurrentFunction (DeltaQuadAddress jump, DeltaQuadAddress target) const {

	DASSERT(jump != target && DeltaIsBranchOpcode(quads[jump].opcode));
	util_ui32 funcNesting = 0;

	while (jump < target) {
		++jump;
		const DeltaQuad& q = quads[jump];
		if (q.opcode == DeltaIC_FUNCENTER)
			if (jump == target)		// jump to an inner function enter!
				return false; 
			else
				++funcNesting;
		else
		if (q.opcode == DeltaIC_FUNCRET)
			if (jump == target)
				if (!funcNesting)
					return true;
				else
					return false;	// jump at the end of an inner function!
			else
				if (!funcNesting)	// passed the end of the current function!
					return false;
				else
					--funcNesting;
		else { } // "use-up" "free" "if-", avoid warning for ambiguous "else"
	}
	return true;
}

///////////////////////////////////////////////////////////////////

void DeltaQuadManager::ValidateIfJump (const DeltaQuad& q, DeltaQuadAddress i) const {

	if (DeltaIsBranchOpcode(q.opcode)) {

		DASSERT(q.label);				// 0 is no mans land.
		DASSERT(q.label <= Total());	// Allow passed the end, but no more.

		if (q.label < Total()) {
			DASSERT(JumpDoesNotCrossCurrentFunction(i, q.label));
			DASSERT(q.opcode != DeltaIC_FUNCENTER);
		}
	}
}

///////////////////////////////////////////////////////////////////

void DeltaQuadManager::BeforeOptimizationValidation (void) const {
	for (util_ui32 i = DELTA_START_QUAD; i < Total(); ++i)
		ValidateIfJump(quads[i], i);
}

///////////////////////////////////////////////////////////////////

void DeltaQuadManager::AfterOptimizationValidation (void) const {

	util_ui32 objCreation = 0;

	for (util_ui32 i = DELTA_START_QUAD; i < Total(); ++i) {

		const DeltaQuad& q = quads[i];

		ValidateIfJump(q, i);

		// Retval as an rvalue should be used exclusively as 'temp = retval'.
		DASSERT(
			!q.arg1 ||
			(	q.arg1 != DeltaExpr::GetReturnValue() || 
				(DPTR(q.result)->IsTemp() && q.opcode == DeltaIC_ASSIGN)	)
		);
		DASSERT(!q.arg2 || q.arg2 != DeltaExpr::GetReturnValue());

		if (q.opcode == DeltaIC_OBJNEW)
			++objCreation;
		else
		if (q.opcode == DeltaIC_OBJDONE)
			{ DASSERT(objCreation); --objCreation; }
	}

	DASSERT(!objCreation);	// Must have balanced appearence of new / done.
}

//------------------------------------------------------------------
