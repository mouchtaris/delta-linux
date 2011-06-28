// ParseActions.h
// Translation schemes for the DeltaCompiler.
// ScriptFighter Project.
// A. Savidis, October 1999 (original version).
// Last update (attributes added), January 2009.
//

#ifndef	PARSEACTIONS_H
#define	PARSEACTIONS_H

#include "DDebug.h"
#include "InterCode.h"
#include "DeltaByteCodeTypes.h"
#include "Symbol.h"
#include "Expr.h"
#include "AutoCollection.h"
#include "Unparsable.h"
#include "utypes.h"
#include "ufunctors.h"
#include "ParseTableConstruction.h"
#include "ASTNode.h"

//////////////////////////////////////////////////////

struct Stmt :	public AutoCollectable,
				public Unparsable {

	DeltaQuadAddress	breakList;
	DeltaQuadAddress	contList;
	void*				userData;	// Anything exrta we may carry as part of a stmt.

	Stmt (void) {
		breakList	=  contList	= DELTA_NIL_QUAD_LABEL;
		unullify(userData);
	}
	~Stmt(){}
};

//////////////////////////////////////////////////////

#define	NULL_EXPR_CHECK(e)				\
	if (!e)								\
		return (DeltaExpr*) 0;

#define	NULL_EXPR_PAIR_CHECK(e1, e2)	\
	if (!e1 || !e2)						\
		return (DeltaExpr*) 0;

#define	NIL_STMT	(Stmt*) 0
#define	NEW_STMT	DNEW(Stmt)

//////////////////////////////////////////////////////

inline void DELTA_EXPR_EMIT_BOOL_TEST (DeltaExpr* expr, DeltaExpr* result, bool trueTest) {
	QUADS.Emit(trueTest ? DeltaIC_JTRUETEST : DeltaIC_JFALSETEST, expr, NIL_EXPR, NIL_EXPR);
	DPTR(result)->trueList = QUADS.MakeList(QUADS.CurrQuadNo());
	QUADS.Emit(DeltaIC_JUMP, NIL_EXPR, NIL_EXPR, NIL_EXPR);
	DPTR(result)->falseList = QUADS.MakeList(QUADS.CurrQuadNo());
}

//////////////////////////////////////////////////////

typedef std::list<std::string>	NameList;

extern DeltaExpr*		Translate_StringConst (const char* str, DeltaExpr* s = NIL_EXPR);
extern DeltaExpr*		Translate_StringifyDottedIdents (const char* id, DeltaExpr* prefix = NIL_EXPR);
extern DeltaExpr*		Translate_StringifyNamespaceIdent (const char* id);
extern DeltaExpr*		Translate_StringifyNamespaceIdent (const NameList& namespacePath);
extern DeltaExpr*		Translate_ConstValue (const std::string& s);
extern DeltaExpr*		Translate_ConstValue (DeltaNumberValueType num);
inline DeltaExpr*		Translate_ConstValue (const char* val) { return Translate_ConstValue(std::string(val)); }
extern DeltaExpr*		Translate_ConstValue (bool val);
extern DeltaExpr*		Translate_ConstValue (void);

extern DeltaExpr*		Translate_AttrLvalue (const std::string& id);

extern DeltaSymbol*		Translate_UseClosureFunctionVar (DeltaSymbol* func, DeltaSymbol* clientFunc);
extern DeltaExpr*		Translate_Lvalue (const std::string& id);
extern DeltaExpr*		Translate_NamespaceLvalue (
							const std::string&	id,
							std::string*		ns
						);
extern DeltaExpr*		Translate_NamespaceLvalue (
							const NameList& namespacePath,
							const std::string&	id,
							std::string*		ns
						);
extern DeltaExpr*		Translate_NamespaceLvalueAsLibraryFunctionOrConst (
							const NameList&		namespacePath,
							const std::string&	id, 
							std::string*		ns, 
							bool				atGlobalNamespace = false
						);

extern DeltaExpr*		Translate_StaticLvalue (const std::string& id);
extern DeltaExpr*		Translate_Lvalue (const std::string& id, util_i16 scopeOffset);
extern DeltaExpr*		Translate_SELF (void);
extern DeltaExpr*		Translate_NEWSELF (void);
extern DeltaExpr*		Translate_LAMBDA_REF (DeltaSymbol* func = NIL_SYMBOL);
extern DeltaExpr*		Translate_ARGUMENTS (void);
extern DeltaExpr*		Translate_TRIPLE_DOT (void);

extern void				Translate_ASSERT (DeltaExpr* expr);

extern DeltaExpr*		Translate_TablePrefix (void);
extern DeltaExpr*		Translate_TableContent (DeltaExpr* table, const char* index);
extern DeltaExpr*		Translate_TableContent (DeltaExpr* table, DeltaExpr* index);
extern DeltaExpr*		Translate_BoundedTableContent (DeltaExpr* table, const char* index);
extern DeltaExpr*		Translate_BoundedTableContent (DeltaExpr* table, DeltaExpr* index);
extern DeltaExpr*		Translate_ExpressionListItem (DeltaExpr* expr);
extern DeltaExpr*		Translate_ExpressionList (DeltaExpr* elist, DeltaExpr* expr);
extern DeltaExpr*		Translate_FunctionCall (DeltaExpr* func, DeltaExpr* actualArgs);
extern void				Translate_IdentList (const char* id);
extern void				Translate_VarArgs (void);

extern DeltaSymbol*		Translate_Function (const char* id, util_ui8 funcClass, util_ui8 linkage = DELTA_FUNCTION_DEFAULT_EXPORT);
extern DeltaSymbol*		Translate_Function (DeltaSymbol* func, Stmt* block, util_ui32 skipFunctionQuad);
extern void				Translate_FunctionHeader (DeltaSymbol* func);
extern DeltaExpr*		Translate_FunctionParenthesisForm (DeltaSymbol* func);
extern DeltaExpr*		Translate_FunctionExpresssion  (DeltaSymbol* func);

extern TableElements*	Translate_IndexedValues (DeltaExpr* indices, DeltaExpr* values);
extern TableElements*	Translate_IdentIndexElement (
							const char*			index, 
							DeltaExpr*			value, 
							DeltaQuadAddress	Mquad, 
							util_ui32			line
						);
extern TableElements*	Translate_TableElement (DeltaExpr* expr);
extern TableElements*	Translate_TableElements (TableElements* t1, TableElements* t2);
extern DeltaExpr*		Translate_TableConstructor (DeltaExpr* table, TableElements* p =(TableElements*) 0);
extern TableElements*	Translate_UnindexedValue (
							DeltaExpr*			expr,
							DeltaQuadAddress	startQuad,
							util_ui32			lastLine
						);

extern DeltaExpr*		Translate_TermLvalue (DeltaExpr* lvalue, bool postop, DeltaICOpcode op);
extern DeltaExpr*		Translate_UMINUSExpression (DeltaExpr* expr);

extern DeltaExpr*		Translate_NOTExpression (DeltaExpr* expr);
extern DeltaExpr*		Translate_ArithmeticExpression (DeltaExpr*, DeltaICOpcode, DeltaExpr*, const char* opStr);
extern DeltaExpr*		Translate_ExprORExpr (DeltaExpr* e1, DeltaQuadAddress Mquad, DeltaExpr* e2);
extern DeltaExpr*		Translate_ExprANDExpr (DeltaExpr* e1, DeltaQuadAddress Mquad, DeltaExpr* e2);
extern DeltaExpr*		Translate_RelationalExpr (DeltaExpr* e1, DeltaExpr* e2, DeltaICOpcode relOp, const char* opStr);

extern DeltaExpr*		Translate_ConstAssignExpression (const char* id, DeltaExpr* expr);
extern DeltaExpr*		Translate_AssignExpr (DeltaExpr* lvalue, DeltaExpr* expr);
extern DeltaExpr*		Translate_AssignArithExpr (DeltaExpr* lvalue, DeltaExpr* expr, DeltaICOpcode arithOp, const char* opStr);

extern DeltaExpr*		Translate_Condition (DeltaExpr* cond);
extern DeltaQuadAddress	Translate_N (void);

extern Stmt*			Translate_ExprListStmt (DeltaExpr* elist);
extern void				Translate_IfStmtPrefix (void); 
extern void				Translate_ElseStmtPrefix (void); 
extern void				Translate_IfStmt (DeltaExpr* cond, DeltaQuadAddress Mquad);
extern void				Translate_IfElseStmt (
							DeltaExpr*			cond, 
							DeltaQuadAddress	M1quad, 
							DeltaQuadAddress	Nquad, 
							DeltaQuadAddress	M2quad
						);

extern void				Translate_WhilePrefix (void);
extern Stmt*			Translate_WhileStmt (
							DeltaExpr*			cond, 
							DeltaQuadAddress	M1quad,
							DeltaQuadAddress	M2quad,
							Stmt*				stmt,
							util_ui32			stmtLine
						);

extern void				Translate_ForOpening (void);
extern void				Translate_ForPrefix (
							DeltaQuadAddress	initListQuad,
							util_ui32			initListLine
						);
extern Stmt*			Translate_ForStmt (
							DeltaExpr*			cond, 
							DeltaQuadAddress	M1quad, 
							DeltaQuadAddress	M2quad, 
							DeltaQuadAddress	M3quad,
							DeltaQuadAddress	N1quad,
							Stmt*				stmt
						);

extern Stmt*			Translate_ForeachPrefix (
							DeltaExpr*			foreachVar,
							DeltaExpr*			foreachIndex,
							DeltaExpr*			foreachContainer
						);
extern Stmt*			Translate_ForeachStmt (
							Stmt*				foreachPrefix, 
							Stmt*				foreachStmt, 
							DeltaQuadAddress	Mquad
						);

extern Stmt*			Translate_RETURN (DeltaExpr* expr = NIL_EXPR);
extern Stmt*			Translate_BREAK (void);
extern Stmt*			Translate_CONTINUE (void);
extern void				Translate_UsingNamespace (const std::string& id, bool hasNamespacePath);
extern void				Translate_UsingByteCodeLibrary (const std::string& id, util_ui16 line);
extern void				Translate_UsingNamespace (const NameList& ns);
extern void				Translate_UsingNamespace (const std::string& ns);
extern Stmt*			Translate_Stmts (Stmt* stmts, Stmt* stmt);
extern Stmt*			Translate_Stmts (Stmt* stmt);
extern void				Translate_BasicStmt (util_ui32 line);
extern void				Translate_CompoundBegin (void);
extern void				Translate_CompoundEnd (void);
extern void				Translate_EmptyCompound (void);

extern DeltaExpr*		Translate_Ternary (
							DeltaExpr*			cond,
							DeltaExpr*			expr1,
							DeltaExpr*			expr2,
							DeltaQuadAddress	M1quad,
							DeltaQuadAddress	M2quad,
							DeltaQuadAddress	Nquad
						);

extern Stmt*			Translate_THROW (DeltaExpr* expr);
extern DeltaQuadAddress	Translate_TRY (void);
extern DeltaQuadAddress	Translate_TRAP (void);
extern DeltaQuadAddress	Translate_TrapJumpOver (void);
extern	void			Translate_TrapStart (
							util_ui32	enable, 
							util_ui32	disable, 
							DeltaExpr*	exceptionVar	
						);
extern void				Translate_TrapEnd (util_ui32 jumpOver);

extern DeltaExpr*		Translate_AttributeMethod (
							DeltaExpr*			f,
							DeltaQuadAddress	startQuad,
							util_ui32			lastLine
						);
extern DeltaExpr*		Translate_NewAttribute (
							const char*		id,
							DeltaExpr*		set,
							DeltaExpr*		get
						);

extern char*			Translate_StringWithLateDestruction (char* s);

//////////////////////////////////////////////////////

extern void				ParseActions_Initialise (void);
extern void				ParseActions_CleanUp (void);
extern void				ParseActions_SingletonCreate (void);
extern void				ParseActions_SingletonDestroy (void);
extern void				ParseActions_MakeReservedGlobalSymbols (void);

//////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
