// ParseActions.h
// Translation schemes for the DeltaCompiler.
// ScriptFighter Project.
// A. Savidis, October 1999 (original version).
// Last update (attributes added), January 2009.
//

#ifndef	PARSEACTIONS_H
#define	PARSEACTIONS_H

#include "DDebug.h"
#include "CompilerComponentDirectory.h"
#include "InterCode.h"
#include "DeltaByteCodeTypes.h"
#include "Symbol.h"
#include "Expr.h"
#include "AutoCollection.h"
#include "Unparsable.h"
#include "utypes.h"
#include "ufunctors.h"
#include "ASTNode.h"
#include "ParseParms.h"

//////////////////////////////////////////////////////

struct Stmt :	public AutoCollectable,
				public Unparsable {

	DFRIENDDESTRUCTOR()
	friend class AutoCollectableFactory<Stmt>;

	DeltaQuadAddress	breakList;
	DeltaQuadAddress	contList;
	void*				userData;	// Anything extra we may carry as part of a stmt.

	private:
	Stmt (AutoCollector* collector) : AutoCollectable(collector) {
		breakList	=  contList	= DELTA_NIL_QUAD_LABEL;
		unullify(userData);
	}
	~Stmt(){}
};

//////////////////////////////////////////////////////

typedef AutoCollectableFactory<Stmt> DeltaStmtFactory;

#define STMTFACTORY_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), DeltaStmtFactory)))

#define STMTFACTORY	STMTFACTORY_EX(GET_COMPONENT_DIRECTORY())

//////////////////////////////////////////////////////

#define	NULL_EXPR_CHECK(e)				\
	if (!e)								\
		return (DeltaExpr*) 0;

#define	NULL_EXPR_PAIR_CHECK(e1, e2)	\
	if (!e1 || !e2)						\
		return (DeltaExpr*) 0;

#define	NIL_STMT	(Stmt*) 0
#define	NEW_STMT	STMTFACTORY.New()

//////////////////////////////////////////////////////

typedef std::list<std::string>	NameList;

class LocalDataHandler;
struct TableElements;

class Translator : public ucomponentdirectoryclient {

	private:

	DeltaExpr*				retValueExpr;
	DeltaExpr*				lambdaValueExpr;

	//////////////////////////////////////////////////////

	//Arithmetic
	DeltaNumberValueType CheckDivisionByZero (DeltaNumberValueType& num);

	//ControlFlowStmts
	void				BackpatchLoop (Stmt* stmt, DeltaQuadAddress contQuad, DeltaQuadAddress breakQuad, DeltaQuadAddress stmtBegin);

	//Lvalues
	DeltaExpr*			Translate_ByteCodeLibraryFunctionLvalue (const std::string& libName, const std::string& funcName, bool* wasLib);
	DeltaExpr*			SetLvalueTypeInformation (DeltaSymbol* sym, DeltaExpr* lvalue);
	bool				HasConflictWithLibraryFunctionOrConst (DeltaSymbol* sym);
	bool				Translate_ShoudlUseFunctionViaClosureFunctionVar (DeltaSymbol* func, DeltaSymbol* clientFunc);
	DeltaSymbol*		Translate_UseClosureFunctionVar(DeltaSymbol* funcVar, DeltaSymbol* func, DeltaSymbol* clientFunc);
	DeltaSymbol*		Translate_ClosureVar (DeltaSymbol* sym, const std::string& id);

	//FunctionsAndCalls
	void				Translate_HandleCreateFunctionVar (DeltaSymbol* func);
	void				Translate_HandleAssignFunctionVar (DeltaSymbol* func);
	void				Translate_HandleAllInnerUses (DeltaSymbol* func);
	DeltaQuadAddress	Translate_FunctionEnd (DeltaSymbol* func, util_ui32 startQuad);

	//TableConstruction
	TableElements*		CreateTableElements(void) const;

	public:
	void				DELTA_EXPR_EMIT_BOOL_TEST (DeltaExpr* expr, DeltaExpr* result, bool trueTest);

	DeltaExpr*			Translate_StringConst (const std::string& str, DeltaExpr* s = NIL_EXPR);
	DeltaExpr*			Translate_StringifyDottedIdents (const std::string& id, DeltaExpr* prefix = NIL_EXPR);
	DeltaExpr*			Translate_StringifyNamespaceIdent (const std::string& id);
	DeltaExpr*			Translate_StringifyNamespaceIdent (const NameList& namespacePath);
	DeltaExpr*			Translate_ConstValue (const std::string& s);
	DeltaExpr*			Translate_ConstValue (DeltaNumberValueType num);
	DeltaExpr*			Translate_ConstValue (const char* val) { return Translate_ConstValue(std::string(val)); }
	DeltaExpr*			Translate_ConstValue (bool val);
	DeltaExpr*			Translate_ConstValue (void);

	DeltaExpr*			Translate_AttrLvalue (const std::string& id);

	DeltaSymbol*		Translate_UseClosureFunctionVar (DeltaSymbol* func, DeltaSymbol* clientFunc);
	DeltaExpr*			Translate_Lvalue (const std::string& id);
	DeltaExpr*			Translate_NamespaceLvalue (const std::string& id, std::string* ns);
	DeltaExpr*			Translate_NamespaceLvalue (
						const NameList& namespacePath,
						const std::string&	id,
						std::string*		ns
					);
	DeltaExpr*			Translate_NamespaceLvalueAsLibraryFunctionOrConst (
							const NameList&		namespacePath,
							const std::string&	id, 
							std::string*		ns, 
							bool				atGlobalNamespace = false
						);

	DeltaExpr*			Translate_StaticLvalue (const std::string& id);
	DeltaExpr*			Translate_Lvalue (const std::string& id, util_i16 scopeOffset);
	DeltaExpr*			Translate_SELF (void);
	DeltaExpr*			Translate_NEWSELF (void);
	DeltaExpr*			Translate_LAMBDA_REF (DeltaSymbol* func = NIL_SYMBOL);
	DeltaExpr*			Translate_ARGUMENTS (void);
	DeltaExpr*			Translate_TRIPLE_DOT (void);

	void				Translate_ASSERT (DeltaExpr* expr);

	DeltaExpr*			Translate_TablePrefix (void);
	DeltaExpr*			Translate_TableContent (DeltaExpr* table, const char* index);
	DeltaExpr*			Translate_TableContent (DeltaExpr* table, DeltaExpr* index);
	DeltaExpr*			Translate_BoundedTableContent (DeltaExpr* table, const char* index);
	DeltaExpr*			Translate_BoundedTableContent (DeltaExpr* table, DeltaExpr* index);
	DeltaExpr*			Translate_ExpressionListItem (DeltaExpr* expr);
	DeltaExpr*			Translate_ExpressionList (DeltaExpr* elist, DeltaExpr* expr);
	DeltaExpr*			Translate_FunctionCall (DeltaExpr* func, DeltaExpr* actualArgs, DeltaSymbol* explicitTemp = NIL_SYMBOL);
	void				Translate_IdentList (const char* id);
	void				Translate_VarArgs (void);

	DeltaSymbol*		Translate_Function (const char* id, util_ui8 funcClass, util_ui8 linkage = DELTA_FUNCTION_DEFAULT_EXPORT);
	DeltaSymbol*		Translate_Function (DeltaSymbol* func, Stmt* block, util_ui32 skipFunctionQuad, util_ui32 orphanMethodLine);
	void				Translate_FunctionHeader (DeltaSymbol* func);
	DeltaExpr*			Translate_FunctionParenthesisForm (DeltaSymbol* func);
	DeltaExpr*			Translate_FunctionExpresssion  (DeltaSymbol* func);

	TableElements*		Translate_IndexedValues (DeltaExpr* indices, DeltaExpr* values);
	TableElements*		Translate_IdentIndexElement (
							const std::string& 	index, 
							DeltaExpr*			value, 
							DeltaQuadAddress	Mquad, 
							util_ui32			line
						);
	TableElements*		Translate_TableElement (DeltaExpr* expr);
	TableElements*		Translate_TableElements (TableElements* t1, TableElements* t2);
	DeltaExpr*			Translate_TableConstructor (DeltaExpr* table, TableElements* p = (TableElements*) 0);
	TableElements*		Translate_UnindexedValue (DeltaExpr* expr, DeltaQuadAddress	startQuad, util_ui32 lastLine);

	DeltaExpr*			Translate_TermLvalue (DeltaExpr* lvalue, bool postop, DeltaICOpcode op);
	DeltaExpr*			Translate_UMINUSExpression (DeltaExpr* expr);

	DeltaExpr*			Translate_NOTExpression (DeltaExpr* expr);
	DeltaExpr*			Translate_ArithmeticExpression (DeltaExpr*, DeltaICOpcode, DeltaExpr*, const char* opStr);
	DeltaExpr*			Translate_ExprORExpr (DeltaExpr* e1, DeltaQuadAddress Mquad, DeltaExpr* e2);
	DeltaExpr*			Translate_ExprANDExpr (DeltaExpr* e1, DeltaQuadAddress Mquad, DeltaExpr* e2);
	DeltaExpr*			Translate_RelationalExpr (DeltaExpr* e1, DeltaExpr* e2, DeltaICOpcode relOp, const char* opStr);

	DeltaExpr*			Translate_ConstAssignExpression (const std::string& id, DeltaExpr* expr);
	DeltaExpr*			Translate_AssignExpr (DeltaExpr* lvalue, DeltaExpr* expr);
	DeltaExpr*			Translate_AssignArithExpr (DeltaExpr* lvalue, DeltaExpr* expr, DeltaICOpcode arithOp, const char* opStr);

	DeltaExpr*			Translate_Condition (DeltaExpr* cond);
	DeltaQuadAddress	Translate_N (void);

	Stmt*				Translate_ExprListStmt (DeltaExpr* elist);
	void				Translate_IfStmtPrefix (void); 
	void				Translate_ElseStmtPrefix (void); 
	void				Translate_IfStmt (DeltaExpr* cond, DeltaQuadAddress Mquad);
	void				Translate_IfElseStmt (
							DeltaExpr*			cond, 
							DeltaQuadAddress	M1quad, 
							DeltaQuadAddress	Nquad, 
							DeltaQuadAddress	M2quad
						);

	void				Translate_WhilePrefix (void);
	Stmt*				Translate_WhileStmt (
							DeltaExpr*			cond, 
							DeltaQuadAddress	M1quad,
							DeltaQuadAddress	M2quad,
							Stmt*				stmt,
							util_ui32			stmtLine
						);

	void				Translate_ForOpening (void);
	void				Translate_ForPrefix (DeltaQuadAddress initListQuad, util_ui32 initListLine);
	Stmt*				Translate_ForStmt (
							DeltaExpr*			cond, 
							DeltaQuadAddress	M1quad, 
							DeltaQuadAddress	M2quad, 
							DeltaQuadAddress	M3quad,
							DeltaQuadAddress	N1quad,
							Stmt*				stmt
						);

	Stmt*				Translate_ForeachPrefix (
							DeltaExpr*			foreachVar,
							DeltaExpr*			foreachIndex,
							DeltaExpr*			foreachContainer
						);
	Stmt*				Translate_ForeachStmt (
							Stmt*				foreachPrefix, 
							Stmt*				foreachStmt, 
							DeltaQuadAddress	Mquad
						);

	Stmt*				Translate_RETURN (DeltaExpr* expr = NIL_EXPR);
	Stmt*				Translate_BREAK (void);
	Stmt*				Translate_CONTINUE (void);
	void				Translate_UsingNamespace (const std::string& id, bool hasNamespacePath);
	void				Translate_UsingByteCodeLibrary (const std::string& id, util_ui16 line);
	void				Translate_UsingNamespace (const NameList& ns);
	void				Translate_UsingNamespace (const std::string& ns);
	Stmt*				Translate_Stmts (Stmt* stmts, Stmt* stmt);
	Stmt*				Translate_Stmts (Stmt* stmt);
	void				Translate_BasicStmt (util_ui32 line);
	void				Translate_CompoundBegin (void);
	void				Translate_CompoundEnd (void);
	void				Translate_EmptyCompound (void);

	DeltaExpr*			Translate_Ternary (
							DeltaExpr*			cond,
							DeltaExpr*			expr1,
							DeltaExpr*			expr2,
							DeltaQuadAddress	M1quad,
							DeltaQuadAddress	M2quad,
							DeltaQuadAddress	Nquad
						);

	Stmt*				Translate_THROW (DeltaExpr* expr);
	DeltaQuadAddress	Translate_TRY (void);
	DeltaQuadAddress	Translate_TRAP (void);
	DeltaQuadAddress	Translate_TrapJumpOver (void);
	void				Translate_TrapStart (
							util_ui32	enable, 
							util_ui32	disable, 
							DeltaExpr*	exceptionVar	
						);
	void				Translate_TrapEnd (util_ui32 jumpOver);

	DeltaExpr*			Translate_AttributeMethod (
							DeltaExpr*			f,
							DeltaQuadAddress	startQuad,
							util_ui32			lastLine
						);
	DeltaExpr*			Translate_NewAttribute (
							const std::string&	id,
							DeltaExpr*			set,
							DeltaExpr*			get
						);

	//////////////////////////////////////////////////////

	DeltaExpr*			GetReturnValue	(void) const { return retValueExpr;		}
	DeltaExpr*			GetLambdaRef	(void) const { return lambdaValueExpr;	}

	void				Initialise (void);
	void				CleanUp (void);
	void				MakeReservedGlobalSymbols (void);

	//////////////////////////////////////////////////////

	Translator (ucomponentdirectory* directory) :
		ucomponentdirectoryclient(directory), retValueExpr((DeltaExpr*) 0), lambdaValueExpr((DeltaExpr*) 0) {}
	~Translator () {}
};

//////////////////////////////////////////////////////

#define TRANSLATOR_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), Translator)))

#define TRANSLATOR	TRANSLATOR_EX(GET_COMPONENT_DIRECTORY())

//////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
