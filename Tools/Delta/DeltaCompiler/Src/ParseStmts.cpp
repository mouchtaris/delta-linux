// ParseStmts.cpp.
// Translation rules for common statements.
// ScriptFighter Project.
// A. Savidis, October 1999 (original 1st version).
// Re-factored last August 2006.
//

#include <string.h>
#include <limits.h>
#include <math.h>

#include "DDebug.h"
#include "ParseActions.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaCompErrorMsg.h"
#include "Symbol.h"
#include "TypeCheck.h"
#include "LocalDataHandling.h"
#include "SelectiveStepInPreparator.h"
#include "LibraryNamespaceHolder.h"
#include "FunctionReturnTypesManager.h"
#include "DeltaCompErrorDefs.h"
#include "Unparsable.h"
#include "CompilerAPI.h"
#include "ustrings.h"
#include "ufiles.h"
#include "uerrorclass.h"

//------------------------------------------------------------------
// ASSERT, BREAK & CONTINUE.

void Translate_ASSERT (DeltaExpr* expr) {

	if (expr) {
		DeltaExpr* exprStr = DeltaExpr::MakeConst(expr->GetUnparsed());
		QUADS.Emit(
			DeltaIC_ASSERT, 
			exprStr, 
			NIL_EXPR, 
			DPTR(expr)->AdaptIfBool()
		);
	}
}

///////////////////////////////////////////////////////////////////

Stmt* Translate_BREAK (void) {

	if (!ParseParms::InLoop()) {
		DELTACOMP_ERROR_NOT_ALLOWED_OUTSIDE_LOOPS("break");
		return NIL_STMT;
	}

	QUADS.Emit(DeltaIC_JUMP, NIL_EXPR, NIL_EXPR, NIL_EXPR);
	Stmt* st = DNEW(Stmt);
	DPTR(st)->breakList = QUADS.MakeList(QUADS.CurrQuadNo());

	return st;
}

///////////////////////////////////////////////////////////////////

Stmt* Translate_CONTINUE (void) {

	if (!ParseParms::InLoop()) {
		DELTACOMP_ERROR_NOT_ALLOWED_OUTSIDE_LOOPS("continue");
		return NIL_STMT;
	}

	QUADS.Emit(DeltaIC_JUMP, NIL_EXPR, NIL_EXPR, NIL_EXPR);
	Stmt* st = DNEW(Stmt);
	DPTR(st)->contList = QUADS.MakeList(QUADS.CurrQuadNo());

	return st;
}

///////////////////////////////////////////////////////////////////

void Translate_UsingNamespace (const NameList& ns) {
	DASSERT(ParseParms::InGlobalScope() && ns.size() > 1);
	DELTANAMESPACES.Open(ns);
}

void Translate_UsingNamespace (const std::string& ns)
	{ DELTANAMESPACES.Open(ns); }

///////////////////////////////////////////////////////////////////

void Translate_UsingNamespace (const std::string& id, bool hasNamespacePath) {

	DASSERT(ParseParms::InGlobalScope());

	if (hasNamespacePath) {
		ParseParms::AppendToNamespacePath(id);
		DELTANAMESPACES.Open(ParseParms::GetNamespacePath());
	}
	else
		DELTANAMESPACES.Open(id);
	
	ParseParms::ClearNamespacePath();
}

///////////////////////////////////////////////////////////////////

void Translate_UsingByteCodeLibrary (const std::string& id, util_ui16 line) {

	DASSERT(ParseParms::InGlobalScope());
	std::string file = id + ".dbc";
	if (DeltaSymbol* lib = DELTASYMBOLS.Lookup(id))
		DELTACOMP_ERROR_BYTECODE_LIBRARY_REDEFINITION(id);
	else
	if (DeltaLibraryNamespace* ns = DELTANAMESPACES.LookupNamespace(id))
		DELTACOMP_ERROR_BYTECODE_LIBRARY_NAMESPACE_CONFLICT(id, ns->GetFullyQualifiedName());
	else {
		// Resolve byte code file paths.
		const std::string& byteCodePath	= DeltaCompiler::GetByteCodePath();
		if (byteCodePath.empty())
			{ DELTACOMP_ERROR_NO_BYTECODE_LOADING_PATH(file); return; }

		ustrlist fullPaths = ufileresolveallwithpaths(file, utokenizestr(byteCodePath, ";,"), false);
		if (fullPaths.empty())
			{ DELTACOMP_ERROR_BYTECODE_LIBRARY_NOT_FOUND(file, byteCodePath); return; }

		// If more than one byte code file name is ambiguous.
		if (fullPaths.size() > 1)
			{ DELTACOMP_ERROR_BYTECODE_LIBRARY_AMBIGUOUS(file, ustrconcat(fullPaths, ";")); return; }

		// Open.
		std::string fullPath = fullPaths.front();
		FILE* fp = ubinaryfileopen(fullPath, "r");
		if (!fp)
			{ DELTACOMP_ERROR_BYTECODE_LIBRARY_OPEN_FAILED(fullPath); return; }

		// Read (skip) version info and source file.
		BinFileReader reader(fp);
		std::string sourceFile;
		if (!DeltaReadVersionInformation(reader) || !reader.read(sourceFile, false)) 
			{ DELTACOMP_ERROR_BYTECODE_LIBRARY_READ_ERROR(fullPath, UERROR_GETREPORT()); UERROR_CLEAR();  fclose(fp); return; }

		// Read function table.
		util_ui16			total	= 0;
		DeltaStdFuncInfo*	funcs	= DeltaStdFuncInfo::ReadFunctionTable(reader, &total);
		DASSERT(funcs || !total);
		if (!funcs && UERROR_ISRAISED())	// Func table can be legally null when having no visibal global functions.
			{ DELTACOMP_ERROR_BYTECODE_LIBRARY_READ_ERROR(fullPath, UERROR_GETREPORT()); UERROR_CLEAR();  fclose(fp); return; }

		// Create the library var and emit the vm loading quads.
		fclose(fp);
		lib = DELTASYMBOLS.NewSymbol(id);
		DPTR(lib)->SetAsImportedLibVar();
		DeltaExpr* vmVar = DeltaExpr::MakeInternalVar(lib);
		DPTR(vmVar)->SetTypeTag(TagExternId);
		DeltaExpr* temp = DeltaExpr::MakeInternalVar(DELTASYMBOLS.NewTemp());

#define	VM_ID		DeltaExpr::MakeConst(id)
#define	VM_FILE		DeltaExpr::MakeConst(file)
#define	VM_LOAD		DeltaExpr::MakeConst("std::vmload")
#define	VM_RUN		DeltaExpr::MakeConst("std::vmrun")
#define	VM_GET		DeltaExpr::MakeConst("std::vmget")

		// An already loaded library?
		QUADS.Emit(DeltaIC_PUSHARG,		VM_ID,		NIL_EXPR,	NIL_EXPR);
		QUADS.SetQuadLine(QUADS.CurrQuadNo(), line);
		QUADS.Emit(DeltaIC_CALLEXTFUNC,	NIL_EXPR,	NIL_EXPR,	VM_GET);	// GET
		QUADS.Emit(DeltaIC_GETRETVAL,	NIL_EXPR,	NIL_EXPR,	temp);
		QUADS.Emit(DeltaIC_ASSIGN,		temp,		NIL_EXPR,	vmVar);
		QUADS.Emit(DeltaIC_JNE,			vmVar,		TRUE_EXPR,	NIL_EXPR, QUADS.NextQuadNoPlus(2));		//	| 	vm not found jump (load)
		QUADS.Emit(DeltaIC_JUMP,		NIL_EXPR,	NIL_EXPR,	NIL_EXPR, QUADS.NextQuadNoPlus(7));		//	|	+1	| 	vm found jump
		QUADS.Emit(DeltaIC_PUSHARG,		VM_ID,		VM_FILE,	NIL_EXPR);	// LOAD						<---	+2	|	+1
		QUADS.Emit(DeltaIC_CALLEXTFUNC, NIL_EXPR,	NIL_EXPR,	VM_LOAD);									//		|	+2
		QUADS.Emit(DeltaIC_GETRETVAL,	NIL_EXPR,	NIL_EXPR,	temp);										//		|	+3
		QUADS.Emit(DeltaIC_ASSIGN,		temp,		NIL_EXPR,	vmVar);										//		|	+4
		QUADS.Emit(DeltaIC_PUSHARG,		vmVar,		NIL_EXPR,	NIL_EXPR);									//		|	+5
		QUADS.Emit(DeltaIC_CALLEXTFUNC, NIL_EXPR,	NIL_EXPR,	VM_RUN);									//		|	+6
																											// <---		+7	1st quad after vm run
		// Make the function vars and emit their assigment quads.
		if (funcs) {
			for (DeltaStdFuncInfo* p = funcs; total; --total, ++p)
				if (p->IsExported()) {	// Only exported functions are included.
					DASSERT(p->IsNamedFunction() && p->IsGlobal());
					DeltaSymbol* func = DELTASYMBOLS.NewSymbol(DeltaSymbol::MakeImportedFuncVarName(id, p->GetName()));
					DPTR(func)->SetAsImportedFunctionVar(QUADS.NextQuadNo(), id, p->GetName());
					DPTR(DPTR(lib)->GetImportedLibVar())->Add(p->GetName(), func);

					// Add explicitly the signature for arguments and return type checking.
					DPTR(func)->funcSig	= DNEWCLASS(DeltaSymbol::FuncSig, (true));	// Exported always.
					const ustrlist& args = p->GetFormals();
					for (ustrlist::const_iterator i = args.begin(); i != args.end(); ++i)
						func->AddFormal(*i);

					if (p->HasVarArgs())
						DPTR(func)->SetHasVarArgs();

					DELTARETURNTYPES.Convert(	// Create return types liost.
						p->GetReturnTypes(),
						func->GetFunctionSignature()->GetReturnTypesPtr()
					);

					DeltaExpr* funcVar = DeltaExpr::MakeInternalVar(func);
					DPTR(funcVar)->SetTypeTag(TagFunction);

					QUADS.Emit(DeltaIC_BOUNDEDOBJGET, vmVar, DeltaExpr::MakeConst(p->GetName()), funcVar);
				}
			DDELARR(funcs);
		}
	}

#undef	VM_ID	
#undef	VM_FILE	
#undef	VM_LOAD	
#undef	VM_RUN	
#undef	VM_GET	

}

///////////////////////////////////////////////////////////////////

Stmt* Translate_ExprListStmt (DeltaExpr* elist) {
	if (elist) {
		while (elist) {
			QUADS.Backpatch(DPTR(elist)->trueList, QUADS.NextQuadNo());
			QUADS.Backpatch(elist->falseList, QUADS.NextQuadNo());
			elist = elist->next;
		}
		return NEW_STMT;
	}
	else 
		return NIL_STMT;
}

///////////////////////////////////////////////////////////////////

Stmt* Translate_Stmts (Stmt* stmt) 
	{ DELTASYMBOLS.ResetTemp(); return stmt; }

Stmt* Translate_Stmts (Stmt* stmts, Stmt* stmt) {
	if (!stmts)
		return stmt;
	else
	if (!stmt)
		return stmts;
	else {
		DPTR(stmts)->breakList	= QUADS.Merge( DPTR(stmts)->breakList, DPTR(stmt)->breakList);
		DPTR(stmts)->contList	= QUADS.Merge( DPTR(stmts)->contList,  DPTR(stmt)->contList);

		DDELETE(stmt);
		return stmts;
	}
}

///////////////////////////////////////////////////////////////////

void Translate_CompoundBegin (void) {
	LocalDataHandler::OnBlockBegin(); 
	ParseParms::CurrScope().enter();
}

void Translate_CompoundEnd (void) {
	LocalDataHandler::OnBlockEnd();
	DELTASYMBOLS.InactivateSymbols(ParseParms::CurrScope().value());
	ParseParms::CurrScope().exit();
}

// The formal arguments are inactivated explicitly in case
// of an empty compound.
//
void Translate_EmptyCompound (void) {
	DELTASYMBOLS.InactivateSymbols(
		ParseParms::CurrScope().value() + 1
	);
}

///////////////////////////////////////////////////////////////////

void Translate_BasicStmt (util_ui32 line) 
	{  SelectiveStepInPreparator::OnStmtDone(line); }

//------------------------------------------------------------------

