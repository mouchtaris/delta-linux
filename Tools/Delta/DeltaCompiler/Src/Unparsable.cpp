// Unparsable.cpp
// Simple interface for parsed items capable to carry their unparsed form.
/// ScriptFighter Project.
// A. Savidis, September 2008.
//

#include "Unparsable.h"
#include "Expr.h"
#include "ParseActions.h"
#include "ASTNode.h"
#include "DeltaSyntax.h"
#include "ustrings.h"
#include "ulexutil.h"
#include "Unparse.h"

///////////////////////////////////////////////////////////////////

#define	SCHAR(s)	std::string(s)
#define	SPACE		SCHAR(" ")

///////////////////////////////////////////////////////////////////

#define	ERROR_RET(e)			if (!e) return
#define	ERROR_RET2(e1,e2)		ERROR_RET(e1);  ERROR_RET(e2)
#define	ERROR_RET3(e1,e2,e3)	ERROR_RET2(e1,e2); ERROR_RET(e2)
#define	ERROR_RET4(e1,e2,e3,e4)	ERROR_RET3(e1,e2,e3); ERROR_RET(e4)

#define	ERROR_RET_EX(e)				if (!e) return ""
#define	ERROR_RET2_EX(e1,e2)		ERROR_RET_EX(e1);  ERROR_RET_EX(e2)
#define	ERROR_RET3_EX(e1,e2,e3)		ERROR_RET2_EX(e1,e2); ERROR_RET_EX(e3)
#define	ERROR_RET4_EX(e1,e2,e3,e4)	ERROR_RET3_EX(e1,e2,e3); ERROR_RET_EX(e4)

///////////////////////////////////////////////////////////////////

void Unparse_ConstValue (DeltaExpr* e) {
	ERROR_RET(e);
	switch (DPTR(e)->GetType()) {
		case DeltaExprNil		:	DPTR(e)->SetUnparsed(Unparse_Nil());						break;
		case DeltaExprNumber	:	DPTR(e)->SetUnparsed(Unparse_Number(DPTR(e)->numConst));	break;
		case DeltaExprBoolean	:	DPTR(e)->SetUnparsed(Unparse_Bool(DPTR(e)->boolConst));		break;
		case DeltaExprString	:	DPTR(e)->SetUnparsed(Unparse_StringConst(e->strConst));		break;
		default: DASSERT(false);
	}
}

///////////////////////////////////////////////////////////////////

void Unparse_ConstDef (DeltaSymbol* var, DeltaExpr* e) {
	ERROR_RET(e);
	DPTR(e)->SetUnparsed(
		Unparse_ConstDef(DPTR(var)->GetName(), DPTR(var)->GetConst()->GetUnparsed())
	);
}

///////////////////////////////////////////////////////////////////

void Unparse_Stringify (DeltaExpr* e) {
	ERROR_RET(e);
	DASSERT(DPTR(e)->GetType() == DeltaExprString);
	DPTR(e)->SetUnparsed(Unparse_Token(STRINGIFY) + e->strConst);		
}

///////////////////////////////////////////////////////////////////

void Unparse_Var (DeltaExpr* e, const std::string& id, util_ui32 qualifier) {
	ERROR_RET(e);
	DPTR(e)->SetUnparsed(Unparse_Var(id, qualifier));
}

void Unparse_Var (DeltaExpr* e, const std::string& id) 
	{ ERROR_RET(e); DPTR(e)->SetUnparsed(Unparse_Var(id)); }

void Unparse_BuiltIn (DeltaExpr* e, util_ui32 qualifier)
	{ ERROR_RET(e); DPTR(e)->SetUnparsed(Unparse_BuiltIn(qualifier)); }

///////////////////////////////////////////////////////////////////

void Unparse_BinaryOp (DeltaExpr* e, DeltaExpr* e1, util_ui32 op, DeltaExpr* e2) {
	ERROR_RET3(e, e1, e2);
	DPTR(e)->SetUnparsed(Unparse_BinaryOp(DPTR(e1)->GetUnparsed(), op, DPTR(e2)->GetUnparsed()));
}

///////////////////////////////////////////////////////////////////

void Unparse_UnaryOp (DeltaExpr* e, util_ui32 op, DeltaExpr* e1, const std::string& infix) {
	ERROR_RET2(e, e1);
	DPTR(e)->SetUnparsed(Unparse_UnaryOp(op, DPTR(e1)->GetUnparsed(), infix));
}

void Unparse_UnaryOp (DeltaExpr* e, DeltaExpr* e1, util_ui32 op) {
	ERROR_RET2(e, e1);
	DPTR(e)->SetUnparsed(Unparse_UnaryOp(op, DPTR(e1)->GetUnparsed()));
}

void Unparse_UnaryIncDec (DeltaExpr* e, util_ui32 op, DeltaExpr* e1, bool after) {
	ERROR_RET2(e, e1); 
	DPTR(e)->SetUnparsed(Unparse_UnaryIncDec(op, e1->GetUnparsed(), after));
}

///////////////////////////////////////////////////////////////////

void Unparse_TernaryOp (DeltaExpr* e, DeltaExpr* cond, DeltaExpr* e1, DeltaExpr* e2) {
	ERROR_RET4(e, cond, e1, e2);
	DPTR(e)->SetUnparsed(
		Unparse_TernaryOp(DPTR(cond)->GetUnparsed(), DPTR(e1)->GetUnparsed(), DPTR(e2)->GetUnparsed())
	);
}

///////////////////////////////////////////////////////////////////

const std::string Unparse_ExprList (DeltaExpr* e) {	
	ERROR_RET_EX(e);
	std::list<std::string> l;
	while (e) {	
		l.push_front(DPTR(e)->GetUnparsed()); // It is in reverse order.
		e = e->next;
	}
	return Unparse_ExprList(l);
}

void Unparse_LateBound (DeltaExpr* e, DeltaExpr* arg) {
	ERROR_RET2(e, arg);
	DPTR(e)->SetUnparsed(Unparse_LateBound(arg->GetUnparsed()));
}

///////////////////////////////////////////////////////////////////

void Unparse_ParenthesisedExpr (DeltaExpr* e) {
	ERROR_RET(e);
	DPTR(e)->SetUnparsed(Unparse_ParenthesisedExpr(DPTR(e)->GetUnparsed()));
}

///////////////////////////////////////////////////////////////////

void Unparse_IndexedValues (TableElements* elems) {
	ERROR_RET(elems);
	DPTR(elems)->SetUnparsed(
		Unparse_IndexedValues(
			Unparse_ExprList(DPTR(DPTR(elems)->indices)->head),
			Unparse_ExprList(DPTR(DPTR(elems)->indexedValues)->head)
		)
	);
}

void Unparse_UnindexedValue (TableElements* elem, DeltaExpr* e) {
	ERROR_RET2(elem, e);
	DPTR(elem)->SetUnparsed(Unparse_UnindexedValue(DPTR(e)->GetUnparsed()));
}

void Unparse_IdentIndexElement (TableElements* elem, const char* index, DeltaExpr* e) {
	ERROR_RET2(elem, e);
	DPTR(elem)->SetUnparsed(Unparse_IdentIndexElement(index, DPTR(e)->GetUnparsed()));
}

///////////////////////////////////////////////////////////////////

void Unparse_DottedIdent (DeltaExpr* e, const char* id) {
	ERROR_RET(e);
	DPTR(e)->SetUnparsed(Unparse_DottedIdent(id));
}

void Unparse_OperatorIndex (DeltaExpr* e, const char* op) {
	ERROR_RET(e);
	DPTR(e)->SetUnparsed(Unparse_OperatorIndex(op));
}

///////////////////////////////////////////////////////////////////

const std::string Unparse_TableElements (TableElements* elems, TableElements* elem) {
	ERROR_RET2_EX(elems, elem);
	return Unparse_TableElements(DPTR(elems)->GetUnparsed(), DPTR(elem)->GetUnparsed());
}

const std::string Unparse_TableConstructor (TableElements* elems) {
	ERROR_RET_EX(elems);
	return Unparse_TableConstructor(elems ? DPTR(elems)->GetUnparsed() : "");
}

///////////////////////////////////////////////////////////////////
// FIXME: better have dot double dot bracket double bracket here.
// Change the DeltaParser.y as needed.

void Unparse_TableContentDot (DeltaExpr* e, DeltaExpr* t, const char* index) {
	ERROR_RET2(e, t);
	DPTR(e)->SetUnparsed(Unparse_TableContentDot(DPTR(t)->GetUnparsed(), index));
}

void Unparse_TableContentDoubleDot (DeltaExpr* e, DeltaExpr* t, const char* index) {
	ERROR_RET2(e, t);
	DPTR(e)->SetUnparsed(Unparse_TableContentDoubleDot(DPTR(t)->GetUnparsed(), index));
}

void Unparse_TableContentBracket (DeltaExpr* e, DeltaExpr* t, const char* index) {
	ERROR_RET2(e, t);
	DPTR(e)->SetUnparsed(Unparse_TableContentBracket(DPTR(t)->GetUnparsed(), index));
}

void Unparse_TableContentDoubleBracket (DeltaExpr* e, DeltaExpr* t, const char* index) {
	ERROR_RET2(e, t);
	DPTR(e)->SetUnparsed(Unparse_TableContentDoubleBracket(DPTR(t)->GetUnparsed(), index));
}

void Unparse_TableContentBracket (DeltaExpr* e, DeltaExpr* t, DeltaExpr* index) {
	ERROR_RET2(e, t);
	DPTR(e)->SetUnparsed(Unparse_TableContentBracket(DPTR(t)->GetUnparsed(), index->GetUnparsed()));
}

void Unparse_TableContentDoubleBracket (DeltaExpr* e, DeltaExpr* t, DeltaExpr* index) {
	ERROR_RET2(e, t);
	DPTR(e)->SetUnparsed(Unparse_TableContentDoubleBracket(DPTR(t)->GetUnparsed(), index->GetUnparsed()));
}

///////////////////////////////////////////////////////////////////

const std::string Unparse_Stmts (Stmt* stmts, Stmt* stmt) {
	ERROR_RET2_EX(stmts, stmt);
	return Unparse_Stmts(DPTR(stmts)->GetUnparsed(), DPTR(stmt)->GetUnparsed());
}

void Unparse_ExprListStmt (Stmt* stmt, DeltaExpr* elist){
	ERROR_RET2(stmt, elist); 
	DPTR(stmt)->SetUnparsed(Unparse_ExprListStmt(Unparse_ExprList(elist)));
}

void Unparse_ExprStmt (Stmt* stmt, util_ui32 act, DeltaExpr* e) {
	ERROR_RET2(stmt, e); 
	DPTR(stmt)->SetUnparsed(Unparse_ExprStmt(act, DPTR(e)->GetUnparsed()));
}

void Unparse_BuiltInStmt (Stmt* stmt, util_ui32 act) {
	ERROR_RET(stmt); 
	DPTR(stmt)->SetUnparsed(Unparse_BuiltInStmt(act));
}

///////////////////////////////////////////////////////////////////

const std::string Unparse_TryTrap (Stmt* trySection, Stmt* trapSection, DeltaExpr* exceptionVar) {
	ERROR_RET2_EX(trySection, trapSection); 
	return	Unparse_TryTrap(
				DPTR(trySection)->GetUnparsed(),
				DPTR(trapSection)->GetUnparsed(),
				DPTR(exceptionVar)->GetUnparsed()
			);
}

///////////////////////////////////////////////////////////////////

void Unparse_If (Stmt* stmt, DeltaExpr* cond, Stmt* ifSection) {
	ERROR_RET3(stmt, cond, ifSection); 
	DPTR(stmt)->SetUnparsed(
		Unparse_If(DPTR(cond)->GetUnparsed(), DPTR(ifSection)->GetUnparsed())
	);
}

const std::string Unparse_IfElse (DeltaExpr* cond, Stmt* ifSection, Stmt* elseSection) {
	ERROR_RET3_EX(cond, ifSection, elseSection); 
	return	Unparse_IfElse(
				DPTR(cond)->GetUnparsed(), 
				DPTR(ifSection)->GetUnparsed(), 
				DPTR(elseSection)->GetUnparsed()
			);
}

const std::string Unparse_While (DeltaExpr* cond, Stmt* whileSection) {
	ERROR_RET2_EX(cond, whileSection); 
	return	Unparse_While(DPTR(cond)->GetUnparsed(), DPTR(whileSection)->GetUnparsed());
}

///////////////////////////////////////////////////////////////////

void Unparse_ForInitList (Stmt* stmt, DeltaExpr* elist) {
	ERROR_RET(stmt);
	DPTR(stmt)->SetUnparsed(Unparse_ForInitList(Unparse_ExprList(elist)));
}

///////////////////////////////////////////////////////////////////

const std::string Unparse_For (Stmt* initSection, DeltaExpr* cond, Stmt* closureSection, Stmt* forSection) {
	ERROR_RET4_EX(initSection, cond, closureSection, forSection); 
	return	Unparse_For(
				initSection->GetUnparsed(),
				cond->GetUnparsed(),
				closureSection->GetUnparsed(),
				forSection->GetUnparsed()
			);
}

///////////////////////////////////////////////////////////////////

const std::string Unparse_ForeachPrefix (DeltaExpr* foreachVar, DeltaExpr* foreachIndex, DeltaExpr* foreachContainer) {
	ERROR_RET2_EX(foreachVar, foreachContainer); 
	if (!foreachIndex)
		return Unparse_ForeachPrefix(foreachVar->GetUnparsed(), "", foreachContainer->GetUnparsed());
	else
		return Unparse_ForeachPrefix(foreachVar->GetUnparsed(), foreachIndex->GetUnparsed(), foreachContainer->GetUnparsed());
}

const std::string Unparse_ForeachStmt (Stmt* foreachPrefix, Stmt* foreachSection) {
	ERROR_RET2_EX(foreachPrefix, foreachSection); 
	return	Unparse_ForeachStmt(foreachPrefix->GetUnparsed(), foreachSection->GetUnparsed());
}

///////////////////////////////////////////////////////////////////

void Unparse_Compound (Stmt* compound) {
	ERROR_RET(compound);
	DPTR(compound)->SetUnparsed(Unparse_Compound(compound->GetUnparsed()));
}

///////////////////////////////////////////////////////////////////

const std::string Unparse_Function (DeltaSymbol* func, Stmt* compound) {

	ERROR_RET2_EX(func, compound); 

	return	Unparse_Function(
				Unparse_FunctionPrefix(!func->IsAnonymousFunction()? func->GetName() : "", func->funcClass, func->IsFunctionExported()),
				Unparse_FunctionFormals(func->GetFormals(), func->funcClass),
				compound->GetUnparsed()
			);
}

const std::string Unparse_LambdaFunction (DeltaSymbol* func,  Stmt* stmt) {
	ERROR_RET2_EX(func, stmt); 
	DASSERT(func->funcClass == DELTA_FUNCCLASS_PROGRAMFUNCTION);
	return	Unparse_LambdaFunction(
				Unparse_FunctionFormals(func->GetFormals(), DELTA_FUNCCLASS_PROGRAMFUNCTION),
				stmt->GetUnparsed()
			);
}

void Unparse_LambdaStmt (Stmt* stmt, DeltaExpr* expr) {
	ERROR_RET2(stmt, expr);  
	DPTR(stmt)->SetUnparsed(Unparse_LambdaStmt(DPTR(expr)->GetUnparsed()));
}

///////////////////////////////////////////////////////////////////

void Unparse_FunctionDef (Stmt* stmt, DeltaSymbol* func) {
	ERROR_RET2(stmt, func);
	DPTR(stmt)->SetUnparsed(Unparse_FunctionDef(DPTR(func)->sourceCode));
}

void Unparse_FunctionDef (DeltaExpr* expr, DeltaSymbol* func) {
	ERROR_RET2(expr, func);
	DPTR(expr)->SetUnparsed(Unparse_FunctionDef(DPTR(func)->sourceCode));
}

const std::string Unparse_FunctionCall (DeltaExpr* func, DeltaExpr* args) { // args being null means no arguments
	ERROR_RET_EX(func);
	return Unparse_FunctionCall(func->GetUnparsed(), Unparse_ExprList(args));
}

void Unparse_FunctionParenthesisForm (DeltaExpr* e, DeltaSymbol* func) {
	ERROR_RET2(e, func);
	DPTR(e)->SetUnparsed(Unparse_FunctionParenthesisForm(DPTR(func)->sourceCode));
}

///////////////////////////////////////////////////////////////////

 void Upnparse_NewAttributeSet (
		DeltaExpr*			attr,
		const std::string&	id,
		DeltaExpr*			set,
		DeltaExpr*			get
	) {
	DASSERT(attr &&  set && get);

	DPTR(attr)->SetUnparsed(
		Unparse_NewAttributeSet(id, DPTR(set)->GetUnparsed(), DPTR(get)->GetUnparsed())
	);
 }

 ///////////////////////////////////////////////////////////////////
