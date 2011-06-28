// Unparsable.h
// Simple interface for parsed items capable to carry their unparsed form.
// ScriptFighter Project.
// A. Savidis, September 2008.
//

#ifndef	UNPARSABLE_H
#define	UNPARSABLE_H

#include "utypes.h"
#include "usingleton.h"
#include "Symbol.h"
#include <string>
#include <map>

//////////////////////////////////////////////////////


class Unparsable {

	protected:
	std::string unparsed;
	
	public:
	const std::string			GetUnparsed (void) const { return unparsed; }
	void						SetUnparsed (const std::string& s) { unparsed = s; }

	Unparsable(void){}
	virtual ~Unparsable(){}
};

#define	UNPARSABLE_GET(call) \
	std::string unparsed = call

#define	UNPARSABLE_SET(var) \
	if (var) DPTR(var)->SetUnparsed(unparsed)

#define	SAFE_GETUNPARSED(e) \
	((e) ? DPTR(e)->GetUnparsed() : std::string())

//////////////////////////////////////////////////////

struct DeltaExpr;

extern void					Unparse_ConstValue (DeltaExpr* e);
extern void					Unparse_ConstDef (DeltaSymbol* var, DeltaExpr* e);
extern void					Unparse_Stringify (DeltaExpr* e);
extern void					Unparse_Var (DeltaExpr* e, const std::string& id, util_ui32 qualifier);
extern void					Unparse_Var (DeltaExpr* e, const std::string& id);
extern void					Unparse_BuiltIn (DeltaExpr* e, util_ui32 qualifier);
extern void					Unparse_BinaryOp (DeltaExpr* e, DeltaExpr* e1, util_ui32 op, DeltaExpr* e2);
extern void					Unparse_UnaryOp (DeltaExpr* e, util_ui32 op, DeltaExpr* e1, const std::string& infix = "");
extern void					Unparse_UnaryOp (DeltaExpr* e, DeltaExpr* e1, util_ui32 op);
extern void					Unparse_UnaryIncDec (DeltaExpr* e, util_ui32 op, DeltaExpr* e1, bool after);
extern void					Unparse_TernaryOp (DeltaExpr* e, DeltaExpr* cond, DeltaExpr* e1, DeltaExpr* e2);
extern const std::string	Unparse_ExprList (DeltaExpr* e);
extern void					Unparse_LateBound (DeltaExpr* e, DeltaExpr* arg);
extern void					Unparse_ParenthesisedExpr (DeltaExpr* e);

struct TableElements;
extern void					Unparse_IndexedValues (TableElements* elems);
extern void					Unparse_UnindexedValue (TableElements* elem, DeltaExpr* e);
extern void					Unparse_IdentIndexElement (TableElements* elem, const char* index, DeltaExpr* expr);
extern void					Unparse_DottedIdent (DeltaExpr* e, const char* id);
extern void					Unparse_OperatorIndex (DeltaExpr* e, const char* op);
extern const std::string	Unparse_TableElements (TableElements* elems, TableElements* elem);
extern const std::string	Unparse_TableConstructor (TableElements* elems = (TableElements*) 0);

extern void					Unparse_TableContentDot (DeltaExpr* e, DeltaExpr* t, const char* index);
extern void					Unparse_TableContentDoubleDot (DeltaExpr* e, DeltaExpr* t, const char* index);
extern void					Unparse_TableContentBracket (DeltaExpr* e, DeltaExpr* t, const char* index);
extern void					Unparse_TableContentDoubleBracket (DeltaExpr* e, DeltaExpr* t, const char* index);
extern void					Unparse_TableContentBracket (DeltaExpr* e, DeltaExpr* t, DeltaExpr* index);
extern void					Unparse_TableContentDoubleBracket (DeltaExpr* e, DeltaExpr* t, DeltaExpr* index);

struct Stmt;
extern const std::string	Unparse_Stmts (Stmt* stmts, Stmt* stmt);
extern void					Unparse_ExprListStmt (Stmt* stmt, DeltaExpr* elist);
extern void					Unparse_ExprStmt (Stmt* stmt, util_ui32 act, DeltaExpr* e);
extern void					Unparse_BuiltInStmt (Stmt* stmt, util_ui32 act);
extern const std::string	Unparse_TryTrap (Stmt* trySection, Stmt* trapSection, DeltaExpr* exceptionVar);
extern const std::string	Unparse_IfElse (DeltaExpr* cond, Stmt* ifSection, Stmt* elseSection);
extern void					Unparse_If (Stmt* stmt, DeltaExpr* cond, Stmt* ifSection);
extern const std::string	Unparse_While (DeltaExpr* cond, Stmt* whileSection);
extern void					Unparse_ForInitList (Stmt* stmt, DeltaExpr* elist = (DeltaExpr*) 0);
extern const std::string	Unparse_For(Stmt* initSection, DeltaExpr* cond, Stmt* closureSection, Stmt* forSection);
extern const std::string	Unparse_ForeachPrefix (DeltaExpr* foreachVar, DeltaExpr* foreachIndex, DeltaExpr* foreachContainer);
extern const std::string	Unparse_ForeachStmt (Stmt* foreachPrefix, Stmt* foreachSection);
extern void					Unparse_Compound (Stmt* compound);

struct DeltaSymbol;
extern const std::string	Unparse_Function (DeltaSymbol* func, Stmt* compound);
extern const std::string	Unparse_LambdaFunction (DeltaSymbol* func, Stmt* compound);
extern void					Unparse_LambdaStmt (Stmt* stmt, DeltaExpr* expr);
extern const std::string	Unparse_FunctionCall (DeltaExpr* func, DeltaExpr* args);
extern void					Unparse_FunctionDef (Stmt* stmt, DeltaSymbol* func);
extern void					Unparse_FunctionDef (DeltaExpr* expr, DeltaSymbol* func);
extern void					Unparse_FunctionParenthesisForm (DeltaExpr* e, DeltaSymbol* func);

extern void					Upnparse_NewAttributeSet (
								DeltaExpr*			e,
								const std::string&	id,
								DeltaExpr*			set,
								DeltaExpr*			get
							);

//////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
