// UnusedTempAssignElimination.cpp
// Optimization support.
// ScriptFigher Project.
// A. Savidis, February 2006.
//

#include "UnusedTempAssignElimination.h"

//	#define	OPTIMIZER_DUMP_OUTPUT

#ifdef	OPTIMIZER_DUMP_OUTPUT
#define	OPT_CODE(code)		code
#else
#define	OPT_CODE(code)
#endif

#define	OPT_SAFE_FPRINTF	if (fp) fprintf
#define	OPT_SAFE_FCLOSE		if (fp) fclose

////////////////////////////////////////////////////////////

UnusedTempAssignElimination::UnusedTempAssignElimination(void) {

	dispatcher[DeltaIC_ADD]						= &UnusedTempAssignElimination::OnArithmetic;
	dispatcher[DeltaIC_SUB]						= &UnusedTempAssignElimination::OnArithmetic;
	dispatcher[DeltaIC_MUL]						= &UnusedTempAssignElimination::OnArithmetic;
	dispatcher[DeltaIC_DIV]						= &UnusedTempAssignElimination::OnArithmetic;
	dispatcher[DeltaIC_MOD]						= &UnusedTempAssignElimination::OnArithmetic;

	dispatcher[DeltaIC_OBJNEW]					= &UnusedTempAssignElimination::OnObjNew;
	dispatcher[DeltaIC_OBJNEWEMPTY]				= &UnusedTempAssignElimination::OnObjNew;
	dispatcher[DeltaIC_OBJGET]					= &UnusedTempAssignElimination::OnTableGet;		
	dispatcher[DeltaIC_OBJSET]					= &UnusedTempAssignElimination::OnTableSet;
	dispatcher[DeltaIC_OBJNEWSET]				= &UnusedTempAssignElimination::OnObjNewSet;
	dispatcher[DeltaIC_BOUNDEDOBJGET]			= &UnusedTempAssignElimination::OnTableGet;	
	dispatcher[DeltaIC_BOUNDEDOBJSET]			= &UnusedTempAssignElimination::OnTableSet;
	dispatcher[DeltaIC_OBJGETATTR]				= &UnusedTempAssignElimination::OnTableGet;		
	dispatcher[DeltaIC_OBJSETATTR]				= &UnusedTempAssignElimination::OnTableSet;
	dispatcher[DeltaIC_OBJNEWATTR]				= &UnusedTempAssignElimination::OnObjNewAttr;
	dispatcher[DeltaIC_ASSIGN]  				= &UnusedTempAssignElimination::OnAssign;
	dispatcher[DeltaIC_CALLFUNC]				= &UnusedTempAssignElimination::OnCallFunc;			
	dispatcher[DeltaIC_CALLOBJGETMETHOD]		= &UnusedTempAssignElimination::OnCallMethod;			
	dispatcher[DeltaIC_CALLOBJBOUNDEDGETMETHOD]	= &UnusedTempAssignElimination::OnCallMethod;			
	dispatcher[DeltaIC_PUSHARG]					= &UnusedTempAssignElimination::OnPushArg;
	dispatcher[DeltaIC_PUSHLATEBOUNDARG]		= &UnusedTempAssignElimination::OnPushArg;

	dispatcher[DeltaIC_JEQ]						= &UnusedTempAssignElimination::OnBranch;
	dispatcher[DeltaIC_JNE]						= &UnusedTempAssignElimination::OnBranch;
	dispatcher[DeltaIC_JGE]						= &UnusedTempAssignElimination::OnBranch;
	dispatcher[DeltaIC_JGT]						= &UnusedTempAssignElimination::OnBranch;
	dispatcher[DeltaIC_JLE]						= &UnusedTempAssignElimination::OnBranch;
	dispatcher[DeltaIC_JLT]						= &UnusedTempAssignElimination::OnBranch;
	dispatcher[DeltaIC_JTRUETEST]				= &UnusedTempAssignElimination::OnBranch;
	dispatcher[DeltaIC_JFALSETEST]				= &UnusedTempAssignElimination::OnBranch;

	dispatcher[DeltaIC_ASSERT]					= &UnusedTempAssignElimination::OnAssert;	
	dispatcher[DeltaIC_THROW]					= &UnusedTempAssignElimination::OnThrow;

	dispatcher[DeltaIC_FOREACHBEGIN]			= &UnusedTempAssignElimination::OnForeachBegin;
	dispatcher[DeltaIC_FOREACHCHECKEND]			= &UnusedTempAssignElimination::OnForeachCheckEnd;
	dispatcher[DeltaIC_FOREACHGETVAL]			= &UnusedTempAssignElimination::OnForeachGetValOrIndex;
	dispatcher[DeltaIC_FOREACHGETINDEX]			= &UnusedTempAssignElimination::OnForeachGetValOrIndex;
	dispatcher[DeltaIC_FOREACHFWD]				= &UnusedTempAssignElimination::OnForeachFwd;
	dispatcher[DeltaIC_FOREACHEND]				= &UnusedTempAssignElimination::OnForeachEnd;
	dispatcher[DeltaIC_GETRETVAL]				= &UnusedTempAssignElimination::OnGetRetVal;
	dispatcher[DeltaIC_RETURNVAL]				= &UnusedTempAssignElimination::OnRetVal;
}

////////////////////////////////////////////////////////////
// A variable is assumed useful for preceeding quads if it is used in
// a subsequent quads in the same basic block.
//
void UnusedTempAssignElimination::AssumeUsefulVar (DeltaSymbol* sym) {
	if (sym && DPTR(sym)->IsTempVar() && !IsUsefulVar(sym))
		usefulVars.push_back(sym);
}

////////////////////////////////////////////////////////////
// We remove all sorts of statements which produce a result to
// a temporary variable that is never actually used.
//
void UnusedTempAssignElimination::PerformOptimization (
		DeltaQuadManager&				quadManager, 
		const std::list<util_ui32>&		excluded,
		const char*						outputFile
	) {

	util_ui32				totalQuads		= quadManager.Total();
	util_ui32				totalDropped	= 0;

	OPT_CODE(FILE* fp	= fopen(outputFile, "at"));
	OPT_CODE(OPT_SAFE_FPRINTF(fp, "***Unused temporary assign elimination: Starting...\n"));

	do {

		DeltaQuad&	quad	= quadManager.GetQuad(--totalQuads);
		std::map<util_ui32, Handler>::iterator i = dispatcher.find(quad.opcode);
		if (i != dispatcher.end()) {
			(this->*i->second)(
				quad, 
				std::find(excluded.begin(), excluded.end(), totalQuads) != excluded.end()
			);

			if (quad.IsDropped()) {
				OPT_CODE(OPT_SAFE_FPRINTF(fp, "Instruction %d dropped.\n", totalQuads));
				++totalDropped;
			}
		}
	}
	while (totalQuads);

	usefulVars.clear();

	OPT_CODE(OPT_SAFE_FPRINTF(
		fp,
		"***Unused temporary assign elimination:Done...\nTotal %d out of %d instructions dropped.\n", 
		totalDropped, quadManager.Total()
	));
	OPT_CODE(OPT_SAFE_FCLOSE(fp));
}

////////////////////////////////////////////////////////////

void UnusedTempAssignElimination::OnAssign (DeltaQuad& q, bool exclude) {

	DeltaSymbol* rvalue = q.arg1->sym;
	DeltaSymbol* lvalue = q.result->sym;

	DASSERT(lvalue);

	if (lvalue == rvalue)
		q.MarkAsDropped(); 
	else {
		if (!exclude)	
			if (DPTR(lvalue)->IsTempVar()) {
				VarIter i = FindUsefulVar(lvalue);
				if (i == usefulVars.end())	// Assign to a not useful temp ?
					{ q.MarkAsDropped(); return; } // Neither rvalue is useful.
				else 
					usefulVars.erase(i);
			}
		AssumeUsefulVar(rvalue);
	}
}

////////////////////////////////////////////////////////////

void UnusedTempAssignElimination::OnGetRetVal (DeltaQuad& q,  bool exclude) {

	DASSERT(!exclude);
	DeltaSymbol* lvalue = DNULLCHECK(q.result->sym);
	DASSERT(DPTR(lvalue)->IsTempVar());

	VarIter i = FindUsefulVar(lvalue);
	if (i == usefulVars.end())		// Assign to a not useful temp ?
		q.MarkAsDropped();			// Drop it.
	else 
		usefulVars.erase(i);
}

////////////////////////////////////////////////////////////

void UnusedTempAssignElimination::OnRetVal (DeltaQuad& q,  bool exclude) {

	DASSERT(!exclude);
	if (DeltaSymbol* rvalue = q.result->sym)
		AssumeUsefulVar(rvalue);
}

////////////////////////////////////////////////////////////

void UnusedTempAssignElimination::OnForeachBegin (DeltaQuad& q, bool exclude) {

	DASSERT(!exclude);

	DeltaSymbol* cont = q.arg1->sym;
	DeltaSymbol* iter = q.result->sym;
	DASSERT(cont && iter && DPTR(iter)->IsTempVar());

	VarIter i = FindUsefulVar(iter);
	DASSERT(i != usefulVars.end());		// Foreach always uses its iterator.

	usefulVars.erase(i);				// We used here the temp.
	AssumeUsefulVar(cont);
}

//******************************

void UnusedTempAssignElimination::OnForeachCheckEnd (DeltaQuad& q,  bool exclude) {

	DASSERT(!exclude);

	DeltaSymbol* cont = q.arg1->sym;
	DeltaSymbol* iter = q.arg2->sym;
	DASSERT(cont && iter && DPTR(iter)->IsTempVar());

	AssumeUsefulVar(iter);
	AssumeUsefulVar(cont);
}

//******************************

void UnusedTempAssignElimination::OnForeachGetValOrIndex (DeltaQuad& q,  bool exclude) {
	
	DASSERT(!exclude);

	DeltaSymbol* iter	= q.arg1->sym;
	DeltaSymbol* result = q.result->sym;
	DASSERT(iter && result && DPTR(iter)->IsTempVar());

	AssumeUsefulVar(iter);

	if (result->IsTempVar()) {
		VarIter i = FindUsefulVar(result);
		if (i == usefulVars.end())		// Assign to a not useful temp ?
			q.MarkAsDropped();			// Drop it.
		else 
			usefulVars.erase(i);
	}
}

//******************************

void UnusedTempAssignElimination::OnForeachFwd (DeltaQuad& q,  bool exclude) {

	DASSERT(!exclude);

	DeltaSymbol* iter = q.arg1->sym;
	DASSERT(iter && DPTR(iter)->IsTempVar());

	AssumeUsefulVar(iter);
}

//******************************

void UnusedTempAssignElimination::OnForeachEnd (DeltaQuad& q,  bool exclude) 
	{ OnForeachFwd(q, exclude); }	// Because of similar coed generation.

////////////////////////////////////////////////////////////

void UnusedTempAssignElimination::OnArithmetic (DeltaQuad& q, bool exclude){ 

	DeltaSymbol* lvalue = q.result->sym;
	DeltaSymbol* rvalue1 = q.arg1->sym;
	DeltaSymbol* rvalue2 = q.arg2->sym;
	DASSERT(lvalue);

	if (!exclude)
		if (DPTR(lvalue)->IsTempVar()) {
			VarIter i = FindUsefulVar(lvalue);
			if (i == usefulVars.end())	// Assign to a not useful temp ?
				{ q.MarkAsDropped(); return; } // Neither rvalues are useful.
			else
				usefulVars.erase(i);
		}

	AssumeUsefulVar(rvalue1);
	AssumeUsefulVar(rvalue2);
}

////////////////////////////////////////////////////////////

void UnusedTempAssignElimination::OnPushArg (DeltaQuad& q, bool exclude) { 

	AssumeUsefulVar(q.arg1->sym);
	if (q.arg2)
		AssumeUsefulVar(q.arg2->sym);
	if (q.result)
		AssumeUsefulVar(q.result->sym);
}

////////////////////////////////////////////////////////////

void UnusedTempAssignElimination::OnThrow (DeltaQuad& q, bool exclude) 
	{ AssumeUsefulVar(q.result->sym); }

////////////////////////////////////////////////////////////

void UnusedTempAssignElimination::OnTableGet (DeltaQuad& q, bool exclude) { 

	DeltaSymbol* table	 = q.arg1->sym;
	DeltaSymbol* index	 = q.arg2->sym;
	DeltaSymbol* content = q.result->sym;

	DASSERT((table || DPTR(q.arg1)->IsTableElementOnNewSelf()) && content);

	if (!exclude) {
		if (DPTR(content)->IsTempVar()) {
			VarIter i = FindUsefulVar(content);
			if (i == usefulVars.end())
				{ q.MarkAsDropped(); return; } // Neither index is useful.
			else
				usefulVars.erase(i); // After that it is not useful.
		}
		else
		if (content->IsImportedFunctionVar()) {	// Was autogenerated tabget to extract the function due to using directive.
			DASSERT(!index && q.arg2->GetType() == DeltaExprString);
			if (!DPTR(content)->GetImportedFuncVar()->IsUsed())
				{ q.MarkAsDropped(); return; } // index is useful.
		}
	}

	AssumeUsefulVar(index);
	AssumeUsefulVar(table);
}

////////////////////////////////////////////////////////////

void UnusedTempAssignElimination::OnObjNewSet (DeltaQuad& q,  bool exclude) {

	DeltaSymbol* content = q.arg1->sym;
	DeltaSymbol* index	 = q.arg2->sym;
	DASSERT(!q.result);

	AssumeUsefulVar(index);
	AssumeUsefulVar(content);
}

////////////////////////////////////////////////////////////

void UnusedTempAssignElimination::OnObjNewAttr (DeltaQuad& q,  bool exclude) {

	DeltaSymbol* setter = q.arg1->sym;
	DeltaSymbol* getter	= q.arg2->sym;

	AssumeUsefulVar(getter);
	AssumeUsefulVar(setter);
}

////////////////////////////////////////////////////////////

void UnusedTempAssignElimination::OnTableSet (DeltaQuad& q, bool exclude){

	DeltaSymbol* content = q.arg1->sym;
	DeltaSymbol* index	 = q.arg2->sym;
	DeltaSymbol* table	 = q.result->sym;

	DASSERT(table || DPTR(q.arg1)->IsTableElementOnNewSelf());

	AssumeUsefulVar(table);
	AssumeUsefulVar(index);
	AssumeUsefulVar(content);
}

////////////////////////////////////////////////////////////

void UnusedTempAssignElimination::OnObjNew (DeltaQuad& q,  bool exclude) {
	
	DeltaSymbol* obj = q.result->sym;
	DASSERT(obj);

	if (DPTR(obj)->IsTempVar()) {
		VarIter i = FindUsefulVar(obj);
		if (i != usefulVars.end())	// Since assigned at this point no more useful.
			usefulVars.erase(i);
	}
}

////////////////////////////////////////////////////////////

void UnusedTempAssignElimination::OnCallFunc (DeltaQuad& q, bool exclude) {
	DeltaSymbol* func = q.result->sym;
	AssumeUsefulVar(func);	
}

void UnusedTempAssignElimination::OnCallMethod (DeltaQuad& q, bool exclude) {
	DeltaSymbol* obj	= q.arg1->sym;
	DeltaSymbol* index	= q.arg2->sym;
	AssumeUsefulVar(obj);	
	AssumeUsefulVar(index);	
}

////////////////////////////////////////////////////////////

void UnusedTempAssignElimination::OnBranch (DeltaQuad& q, bool exclude){ 

	DeltaSymbol* arg1 = q.arg1->sym;
	AssumeUsefulVar(arg1);

	if (q.arg2) {
		DeltaSymbol* arg2 = q.arg2->sym;
		AssumeUsefulVar(arg2);
	}
	else
		DASSERT(q.GetOpCode() == DeltaIC_JTRUETEST || q.GetOpCode() == DeltaIC_JFALSETEST);
}

////////////////////////////////////////////////////////////

void UnusedTempAssignElimination::OnAssert (DeltaQuad& q, bool exclude){ 
	DeltaSymbol* result	 = q.result->sym;
	AssumeUsefulVar(result);
}

////////////////////////////////////////////////////////////
