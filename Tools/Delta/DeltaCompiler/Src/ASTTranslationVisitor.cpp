// ASTTranslationVisitor.cpp
// Tree visitor for translation to intermediate code.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#include "ASTTranslationVisitor.h"
#include "ASTValidationVisitor.h"
#include "ASTCreationActions.h"
#include "ParseActions.h"
#include "ParseTableConstruction.h"
#include "CompilerAPI.h"
#include "GlobalData.h"
#include "ASTTags.h"
#include "ASTMetaTags.h"
#include "Unparsable.h"
#include "DeltaCompErrorDefs.h"
#include "Unparse.h"
#include "DeltaSyntax.h"
#include "ufunctors.h"
#include "uerrorclass.h"
#include "ustlhelpers.h"

#include "ASTEscapeTranslationVisitor.h"
#include "ASTUnparseVisitor.h"
#include "DeltaStdClassNames.h"

///////////////////////////////////////////////////////////

#define VISITOR			DNULLCHECK((AST::TranslationVisitor*) closure)

#undef COMPMESSENGER
#define COMPMESSENGER	COMPMESSENGER_EX(VISITOR->GET_COMPONENT_DIRECTORY())

#undef GLOBALDATA
#define GLOBALDATA		GLOBALDATA_EX(VISITOR->GET_COMPONENT_DIRECTORY())

#undef PARSEPARMS
#define PARSEPARMS		PARSEPARMS_EX(VISITOR->GET_COMPONENT_DIRECTORY())

#undef TRANSLATOR
#define TRANSLATOR		TRANSLATOR_EX(VISITOR->GET_COMPONENT_DIRECTORY())

#undef DELTASYMBOLS
#define DELTASYMBOLS	DELTASYMBOLS_EX(VISITOR->GET_COMPONENT_DIRECTORY())

#undef QUADS
#define QUADS			QUADS_EX(VISITOR->GET_COMPONENT_DIRECTORY())

#undef STMTFACTORY
#define STMTFACTORY		STMTFACTORY_EX(VISITOR->GET_COMPONENT_DIRECTORY())

///////////////////////////////////////////////////////////

#define Rule				AST::TranslationVisitor::EvaluationStack::TopRule
#define	EvalStack()			VISITOR->GetEvalStack()
#define	yysetsourceinfo()	SetSourceInfo(AST_VISITOR_ACTUALS)
#define	yyrule				yysetsourceinfo();						\
							Rule yy(EvalStack().GetValues());		\
							yy = Rule::RuleSize()
#define	yyapply				yy.Apply(__LINE__, node->GetTag().c_str())
#define	yypreserve			yy.Preserve()
#define	yypush				EvalStack().Set(__LINE__, node->GetTag().c_str()).Push
#define	yytop				EvalStack().Top()
#define	yv					yy[0]
#define	M()					if (true) { yyrule;	yv = QUADS.NextQuadNo();		yyapply; } else
#define	N()					if (true) { yyrule;	yv = TRANSLATOR.Translate_N();	yyapply; } else
#define LINE(node)			DPTR((AST::Node*)node)->GetLine()

///////////////////////////////////////////////////////////

#define	_H(name, tag)		\
		SetHandler(tag, &Handle_##name, this)

#define	_C(name, tag, id)	\
		SetContextDependentHandler(tag, id, &Handle_##name, this)

#define	SET_QUAD_LINE(yypos, is_expr)							\
		const util_ui32 line = LINE(node);						\
		DASSERT(line);											\
		QUADS.SetQuadLine(yy[yypos].quadNo(), line, is_expr)	\

///////////////////////////////////////////////////////////

AST::TranslationVisitor::TranslationVisitor (ucomponentdirectory* directory) :
	ucomponentdirectoryclient(directory), quasiQuoteOnErrorFunc(0)
{
	// Context independent handlers.
	_H(Program,							AST_TAG_PROGRAM);
	_H(Stmts,							AST_TAG_STMTS);
	_H(Stmt,							AST_TAG_STMT);
	_H(BasicStmt,						AST_TAG_BASIC_STMT);
	_H(Name,							AST_TAG_NAME);
	_H(Function,						AST_TAG_FUNCTION);
	_H(LambdaFunction,					AST_TAG_LAMBDA_FUNCTION);
	_H(FunctionStmt,					AST_TAG_FUNCTION_STMT);
	_H(FormalArgs,						AST_TAG_FORMAL_ARGS);
	_H(Compound,						AST_TAG_COMPOUND);
	_H(ExpressionListStmt,				AST_TAG_EXPRLIST_STMT);
	_H(EmptyStmt,						AST_TAG_EMPTY_STMT);
	_H(AssertStmt,						AST_TAG_ASSERT);
	_H(Return,							AST_TAG_RETURN);
	_H(UsingNamespace,					AST_TAG_USING_NAMESPACE);
	_H(UsingByteCodeLibrary,			AST_TAG_USING_BYTECODE_LIBRARY);
	_H(ConstDefExpression,				AST_TAG_CONSTDEF);
	_H(AssignExpr,						AST_TAG_ASSIGN);
	_H(AssignArithExpr,					AST_TAG_OP_ADD_ASSIGN);
	_H(AssignArithExpr,					AST_TAG_OP_SUB_ASSIGN);
	_H(AssignArithExpr,					AST_TAG_OP_MUL_ASSIGN);
	_H(AssignArithExpr,					AST_TAG_OP_DIV_ASSIGN);
	_H(AssignArithExpr,					AST_TAG_OP_MOD_ASSIGN);
	_H(RelationalExpr,					AST_TAG_OP_GT);
	_H(RelationalExpr,					AST_TAG_OP_LT);
	_H(RelationalExpr,					AST_TAG_OP_GE);
	_H(RelationalExpr,					AST_TAG_OP_LE);
	_H(RelationalExpr,					AST_TAG_OP_EQ);
	_H(RelationalExpr,					AST_TAG_OP_NE);
	_H(ExprANDExpr,						AST_TAG_AND);
	_H(ExprORExpr,						AST_TAG_OR);
	_H(ArithmeticExpression,			AST_TAG_OP_ADD);
	_H(ArithmeticExpression,			AST_TAG_OP_SUB);
	_H(ArithmeticExpression,			AST_TAG_OP_MUL);
	_H(ArithmeticExpression,			AST_TAG_OP_DIV);
	_H(ArithmeticExpression,			AST_TAG_OP_MOD);
	_H(NOTExpression,					AST_TAG_NOT);
	_H(UMINUSExpression,				AST_TAG_UMINUS);
	_H(TermLvalueArith,					AST_LVALUE_PLUSPLUS);
	_H(TermLvalueArith,					AST_LVALUE_MINUSMINUS);
	_H(TermLvalueArith,					AST_PLUSPLUS_LVALUE);
	_H(TermLvalueArith,					AST_MINUSMINUS_LVALUE);
	_H(Ternary,							AST_TAG_TERNARY);
	_H(PrimaryExpression,				AST_TAG_PRIMARY_EXPRESSION);
	_H(Expr,							AST_TAG_EXPR);
	_H(ParenthesisedExpr,				AST_TAG_PARENTHESISED_EXPR);
	_H(ExprList,						AST_TAG_EXPR_LIST);
	_H(ExpressionListItem,				AST_TAG_EXPRLIST_ITEM);
	_H(NumConst, 						AST_TAG_NUM_CONST);
	_H(NilConst, 						AST_TAG_NIL);
	_H(BoolConst, 						AST_TAG_BOOL_CONST);
	_H(StringConst, 					AST_TAG_STRING_CONST);
	_H(StringifyDottedIdents, 			AST_TAG_STRINGIFY_DOTTED_IDENTS);
	_H(StringifyNamespaceIdent,			AST_TAG_STRINGIFY_NAMESPACE_IDENT);
	_H(NamespaceLvalue,					AST_TAG_LVALUE_NAMESPACE_IDENT);
	_H(SELF,							AST_TAG_SELF);
	_H(ARGUMENTS,						AST_TAG_ARGUMENTS);
	_H(TRIPLE_DOT,						AST_TAG_TRIPLE_DOT);
	_H(LAMBDA_REF,						AST_TAG_LAMBDA_REF);
	_H(NEWSELF,							AST_TAG_LVALUE_NEWSELF);
	_H(BREAK,							AST_TAG_BREAK);
	_H(CONTINUE,						AST_TAG_CONTINUE);
	_H(FunctionParenthesisForm,			AST_TAG_FUNCTION_PARENTHESIS);
	_H(Argument,						AST_TAG_ARGUMENT);
	_H(LateBoundArgument,				AST_TAG_LATEBOUND_ARGUMENT);
	_H(FunctionCall,					AST_TAG_FUNCTION_CALL);
	_H(Lvalue,							AST_TAG_LVALUE_IDENT);
	_H(AttrLvalue,						AST_TAG_LVALUE_ATTRIBUTE);
	_H(StaticLvalue,					AST_TAG_LVALUE_STATIC_IDENT);
	_H(LocalLvalue,						AST_TAG_LVALUE_LOCAL_IDENT);
	_H(DotIndexIdent,					AST_TAG_DOTINDEX_IDENT);
	_H(DotIndexOpString,				AST_TAG_DOTINDEX_OPSTRING);
	_H(DotIndexStringConst,				AST_TAG_DOTINDEX_STRINGCONST);
	_H(BracketIndexOperatorMethod,		AST_TAG_BRACKETINDEX_OPMETHOD);
	_H(TableContentDot,					AST_TAG_TABLE_CONTENT_DOT);
	_H(BoundedTableContentDot,			AST_TAG_TABLE_CONTENT_DOUBLE_DOT);
	_H(TableContentBracket,				AST_TAG_TABLE_CONTENT_BRACKET);
	_H(BoundedTableContentBracket,		AST_TAG_TABLE_CONTENT_DOUBLE_BRACKET);
	_H(TableConstructor,				AST_TAG_TABLE_CONSTRUCTOR);
	_H(UnindexedValue,					AST_TAG_TABLE_UNINDEXED_ELEM);
	_H(IdentIndexElement,				AST_TAG_TABLE_IDENTINDEX_ELEM);
	_H(FunctionExpression,				AST_TAG_FUNCTION_EXPR);
	_H(DottedIdent,						AST_TAG_TABLE_DOTTED_IDENT);
	_H(OperatorIndex,					AST_TAG_TABLE_OPERATOR_INDEX);
	_H(TableValues,						AST_TAG_TABLE_VALUES);
	_H(TableIndices,					AST_TAG_TABLE_INDICES);
	_H(IndexedValues,					AST_TAG_TABLE_INDEXED_VALUES);
	_H(NewAttribute,					AST_TAG_TABLE_NEW_ATTRIBUTE);
	_H(IfStmt,							AST_TAG_IF);
	_H(IfElseStmt,						AST_TAG_IF_ELSE);
	_H(WhileStmt,						AST_TAG_WHILE);
	_H(ForStmt,							AST_TAG_FOR);
	_H(ForeachStmt,						AST_TAG_FOREACH);
	_H(THROW,							AST_TAG_THROW);
	_H(ExceptionVar,					AST_TAG_EXCEPTION_VAR);
	_H(Exception,						AST_TAG_TRYTRAP);
	_H(QuasiQuotes,						AST_TAG_QUASI_QUOTES);
	_H(Escape,							AST_TAG_ESCAPE);
	_H(Inline,							AST_TAG_INLINE);
	_H(Execute,							AST_TAG_EXECUTE);

	// Context dependent handlers.
	_C(AdaptTableContent,	AST_TAG_TABLE_CONTENT_DOT,				AST_CHILD_TABLE);
	_C(AdaptTableContent,	AST_TAG_TABLE_CONTENT_DOUBLE_DOT,		AST_CHILD_TABLE);
	_C(AdaptTableContent,	AST_TAG_TABLE_CONTENT_BRACKET,			AST_CHILD_TABLE);
	_C(AdaptTableContent,	AST_TAG_TABLE_CONTENT_DOUBLE_BRACKET,	AST_CHILD_TABLE);

	_C(AdaptBoolExpression, AST_TAG_OP_ADD, 						AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_SUB, 						AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_MUL, 						AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_DIV, 						AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_MOD, 						AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_GT,							AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_LT,							AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_GE,							AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_LE,							AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_EQ,							AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_NE,							AST_CHILD_LEFT);

	_C(AdaptBoolExpression, AST_TAG_OP_ADD, 						AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_SUB, 						AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_MUL, 						AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_DIV, 						AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_MOD, 						AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_GT,							AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_LT,							AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_GE,							AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_LE,							AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_EQ,							AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_NE,							AST_CHILD_RIGHT);

	_C(ElsePrefix,			AST_TAG_IF_ELSE,						AST_CHILD_ELSE);	//Must be before the ScopedStmts
				
	_C(ScopedStmt,			AST_TAG_TRYTRAP,						AST_CHILD_TRY);
	_C(ScopedStmt,			AST_TAG_TRYTRAP,						AST_CHILD_TRAP);
	_C(ScopedStmt,			AST_TAG_FOR,							AST_CHILD_STMT);
	_C(ScopedStmt,			AST_TAG_FOREACH,						AST_CHILD_STMT);
	_C(ScopedStmt,			AST_TAG_WHILE,							AST_CHILD_STMT);
	_C(ScopedStmt,			AST_TAG_IF_ELSE,						AST_CHILD_IF);
	_C(ScopedStmt,			AST_TAG_IF_ELSE,						AST_CHILD_ELSE);
	_C(ScopedStmt,			AST_TAG_IF,								AST_CHILD_STMT);
	
	_C(IfCondition,			AST_TAG_IF,								AST_CHILD_COND);
	_C(IfCondition,			AST_TAG_IF_ELSE,						AST_CHILD_COND);
	_C(WhileCondition,		AST_TAG_WHILE,							AST_CHILD_COND);
	_C(ForCond,				AST_TAG_FOR,							AST_CHILD_COND);
	_C(ForeachContainer,	AST_TAG_FOREACH,						AST_CHILD_CONTAINER);
	_C(ExpressionANDOR,		AST_TAG_AND,							AST_CHILD_LEFT);
	_C(ExpressionANDOR,		AST_TAG_OR,								AST_CHILD_LEFT);
	_C(TernaryCondition,	AST_TAG_TERNARY,						AST_CHILD_COND);
	_C(TernaryLeftExpr,		AST_TAG_TERNARY,						AST_CHILD_LEFT);
	_C(TernaryRightExpr,	AST_TAG_TERNARY,						AST_CHILD_RIGHT);
	_C(ForInit,				AST_TAG_FOR,							AST_CHILD_INIT);
	_C(ForSuffix,			AST_TAG_FOR,							AST_CHILD_SUFFIX);
	_C(LambdaStmt,			AST_TAG_LAMBDA_FUNCTION,				AST_CHILD_EXPR);

	_C(AttributeMethod,		AST_TAG_TABLE_NEW_ATTRIBUTE,			AST_CHILD_SET);
	_C(AttributeMethod,		AST_TAG_TABLE_NEW_ATTRIBUTE,			AST_CHILD_GET);

	GetEvalStack().Initialise();
}

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::operator()(AST::Node* root){
	if (root) {
		DPTR(root)->AcceptPreOrder(this);
		COMPMESSENGER_EX(GET_COMPONENT_DIRECTORY()).SetSourceReferences();
	}
}

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_Program (AST_VISITOR_ARGS){ 
	if (entering)
		GLOBALDATA.Start();
	else {
		GLOBALDATA.End();
		DASSERT(TRANSLATOR.GetReturnValue()->sym->GetOffset() == DELTA_RETVALUE_OFFSET);
		DASSERT(TRANSLATOR.GetReturnValue()->sym->GetBlockId() == DELTA_MAIN_BLOCK_VALUE);
	}
}

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_Name (AST_VISITOR_ARGS)
	{ if (!entering) yypush(NAME(node).c_str()); }

//*************************
// Function: M(1.quadNo) Class Function(2.sym) Name(3.str) FormalArgs Compound(4.stmt)
//
void AST::TranslationVisitor::Handle_FunctionBasic (AST_VISITOR_ARGS, const std::string (*unparser)(DeltaSymbol*, Stmt*)) {

	if (entering) {

		M();

		yyrule;

		const char* name = (const char*) 0;
		if (TreeNode* funcName = DPTR(node)->GetChild(AST_CHILD_NAME))
			name = NAME(funcName).c_str();

		util_ui8 funcClass = AST::GetFuncClass((AST::Node*) node);
		util_ui8 funcLinkage = AST::GetFuncLinkage((AST::Node*) node);
		yv = TRANSLATOR.Translate_Function(name, funcClass, funcLinkage);

		yyapply;

	} else {
		const util_ui32 n = DPTR(node)->GetTotalChildren();
		DASSERT(n == 2 || n == 3);
		yyrule(n + 1);

		UNPARSABLE_GET((*unparser)(yy[2].sym(), yy[n + 1].stmt()));
		if (yv = TRANSLATOR.Translate_Function(yy[2].sym(), yy[n + 1].stmt(), yy[1].quadNo(), LINE(node)))
			DPTR(yv.sym())->SetSourceCode(unparsed);

		yyapply;
	}
}

//*************************

void AST::TranslationVisitor::Handle_Function (AST_VISITOR_ARGS)
	{ Handle_FunctionBasic(AST_VISITOR_ACTUALS, &Unparse_Function); }

void AST::TranslationVisitor::Handle_LambdaFunction (AST_VISITOR_ARGS){
	Handle_FunctionBasic(AST_VISITOR_ACTUALS, &Unparse_LambdaFunction);
	Handle_FunctionExpression(AST_VISITOR_ACTUALS);
}

//*************************
// LambdaStmt: M(1.quadNo) Expr(2.expr)

void AST::TranslationVisitor::Handle_LambdaStmt (AST_VISITOR_ARGS) {
	if (entering) {
		yysetsourceinfo();
		TRANSLATOR.Translate_CompoundBegin();
		M();
	}
	else {
		yyrule(2);

		DeltaExpr* expr = yy[2].expr();
		DASSERT(expr);

		Unparse_LambdaStmt(yv = TRANSLATOR.Translate_RETURN(expr), expr);
		TRANSLATOR.Translate_CompoundEnd();
		SET_QUAD_LINE(1, true);
		yyapply;
	}
}

//*************************

void AST::TranslationVisitor::Handle_FunctionStmt (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(1);
		Unparse_FunctionDef(yv = NEW_STMT, yy[1].sym());
		yyapply;
	}
}

//*************************
// FormalArgs: IdentList (i.str).
//
void AST::TranslationVisitor::Handle_FormalArgs (AST_VISITOR_ARGS){

	if (!entering) {
		{
			const util_ui32 n = DPTR(node)->GetTotalChildren();
			yyrule(n);
			for (util_ui32 i = 1; i <= n; ++i)
				if (!strcmp(yy[i].str(), AST_VALUE_VARARGS_FORMAL_NAME)) {
					DASSERT(i == n);
					TRANSLATOR.Translate_VarArgs();
				}
				else
					TRANSLATOR.Translate_IdentList(yy[i].str());
		}
		{
			TreeNode* funcNode = DPTR(node)->GetParent();
			DASSERT(funcNode);
			const util_ui32 n = DPTR(funcNode)->GetTotalChildren();
			DASSERT(n == 2 || n == 3);
			DeltaSymbol* func;
			if (n == 2)
				func = yytop.sym();
			else {
				yyrule(n);
				func = yy[2].sym();
				yypreserve;
			}
			TRANSLATOR.Translate_FunctionHeader(func);
		}
		DELTASYMBOLS.PushAndResetTempCounter();
	}
}

//*************************
// Compound: [Stmts] (1.stmt)
//
void AST::TranslationVisitor::Handle_Compound (AST_VISITOR_ARGS){ 
	if (entering) {
		if (DPTR(node)->GetTotalChildren())
			TRANSLATOR.Translate_CompoundBegin();
	}
	else {
		const util_ui32 n = DPTR(node)->GetTotalChildren();
		DASSERT(n == 1 || n == 0);

		yyrule(n);
		
		if (n) {
			Unparse_Compound(yv = yy[1].stmt()); 
			TRANSLATOR.Translate_CompoundEnd();
		}
		else {
			Unparse_Compound(yv = NEW_STMT);
			TRANSLATOR.Translate_EmptyCompound();
		}

		yyapply;
	}
} 

//*************************
//Stmts: Stmt* (i.stmt)
//
void AST::TranslationVisitor::Handle_Stmts (AST_VISITOR_ARGS){ 

	if (!entering) {

		Stmt*		stmt			= NIL_STMT;
		util_ui32	n				= DPTR(node)->GetTotalChildren();
		util_ui32	usingDirectives = 0;

		for (util_ui32 i = 0; i < n; ++i) {
			TreeNode* child = DPTR(node)->GetChild(i);
			DASSERT(child);
			child = DPTR(child)->GetChild(AST_CHILD_STMT);
			DASSERT(child);
			if(AST::ValidationVisitor::IsUsingDirective(DPTR(child)))
				++usingDirectives;
		}

		n -= usingDirectives;

		yyrule(n);	// All stmts on the stack.

		for (util_ui32 i = 1; i <= n; ++i)
			if (i == 1)
				stmt = yy[1].stmt();
			else {
				UNPARSABLE_GET(Unparse_Stmts(stmt, yy[i].stmt()));
				stmt = TRANSLATOR.Translate_Stmts(stmt, yy[i].stmt()); 
				UNPARSABLE_SET(stmt);
			}

		yv = stmt;
		yyapply;
	}
}

//*************************
// Stmt: Any stmt (1.stmt)
//
void AST::TranslationVisitor::Handle_Stmt (AST_VISITOR_ARGS){
	TreeNode* child = DPTR(node)->GetChild(AST_CHILD_STMT);
	DASSERT(child);
	if (!AST::ValidationVisitor::IsUsingDirective(DPTR(child)) && !entering) {
		yyrule(1);
		yv = TRANSLATOR.Translate_Stmts(yy[1].stmt());
		yyapply;
	}
} 

//*************************
// BasicStmt: M(1.quadNo) Stmt(2.stmt)
//

void AST::TranslationVisitor::Handle_BasicStmt (AST_VISITOR_ARGS){
	TreeNode* child = DPTR(node)->GetChild(AST_CHILD_STMT);
	DASSERT(child);
	bool isExprStmt = AST::ValidationVisitor::IsBasicExprStmt(DPTR(child));
	bool isNonExprStmt = AST::ValidationVisitor::IsBasicNonExprStmt(DPTR(child));
	DASSERT(isExprStmt || isNonExprStmt);
	if (entering)
		M();
	else {
		yyrule(2);
		SET_QUAD_LINE(1, isExprStmt);
		TRANSLATOR.Translate_BasicStmt(line);
		yv = yy[2].stmt();
		yyapply;
	}
} 

///////////////////////////////////////////////////////////
// ExpressionListStmt: Expr*(i.expr) Semi
//
void AST::TranslationVisitor::Handle_ExpressionListStmt (AST_VISITOR_ARGS){

	if (!entering) {

		DeltaExpr*	expr	= NIL_EXPR;
		const util_ui32	n	= DPTR(node)->GetTotalChildren();

		yyrule(n);	// All exprs on the stack.

		for (util_ui32 i = 1; i <= n; ++i)
			expr = TRANSLATOR.Translate_ExpressionList(expr, yy[i].expr());

		yv = TRANSLATOR.Translate_ExprListStmt(expr); 
		Unparse_ExprListStmt(yv.stmt(), expr);

		yyapply;
	}
}

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_EmptyStmt (AST_VISITOR_ARGS){
	if (!entering) {
		yyrule; 
		yv = NEW_STMT; 
		yv.stmt()->SetUnparsed(Unparse_EmptyStmt());
		DELTACOMP_WARNING_EMPTY_STMT();
		DELTACOMP_WARNING_EMPTY_STMT();
		yyapply;
	}
} 

///////////////////////////////////////////////////////////
// Assert: assert Expr(1.expr)
//
void AST::TranslationVisitor::Handle_AssertStmt (AST_VISITOR_ARGS){ 
	if (entering) 
		PARSEPARMS.InAssertStmt().enter();
	else {
		yyrule(1);
		TRANSLATOR.Translate_ASSERT(yy[1].expr()); 
		PARSEPARMS.InAssertStmt().exit(); 
		Unparse_ExprStmt(yv = NEW_STMT, ASSERT, yy[1].expr());
		yyapply;
	}
} 

///////////////////////////////////////////////////////////
// Return: return [ Expr(1.expr) ]
//
void AST::TranslationVisitor::Handle_Return (AST_VISITOR_ARGS){ 
	if (!entering) {
		if (node->GetChild(AST_CHILD_EXPR)) {
			yyrule(1);
			yv = TRANSLATOR.Translate_RETURN(yy[1].expr()); 
			Unparse_ExprStmt(yv.stmt(), RETURN, yy[1].expr());
			yyapply;
		}
		else {
			yyrule;
			Unparse_BuiltInStmt(yv = TRANSLATOR.Translate_RETURN(), RETURN);
			yyapply;
		}
	}
} 

///////////////////////////////////////////////////////////
// Using: using ( Namespace (i.str) | ident (1.str) )
//
void AST::TranslationVisitor::Handle_UsingNamespace (AST_VISITOR_ARGS){ 
	if (!entering) {
		const util_ui32 n = DPTR(node)->GetTotalChildren();
		DASSERT(n);
		yyrule(n);
		if (n == 1)
			TRANSLATOR.Translate_UsingNamespace(yy[1].str());
		else {
			std::list<std::string> namespacePath;
			for (util_ui32 i = 1; i <= n; ++i)
				namespacePath.push_back(yy[i].str());
			TRANSLATOR.Translate_UsingNamespace(namespacePath);
		}
	}
}	

///////////////////////////////////////////////////////////
// Using: using StringConst (value) : ident (1.str)
//
void AST::TranslationVisitor::Handle_UsingByteCodeLibrary (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(1);
		TRANSLATOR.Translate_UsingByteCodeLibrary(yy[1].str(), LINE(node));
	}
}	

///////////////////////////////////////////////////////////
// Const: const ident (1.str) Expr (2.expr)
//
void AST::TranslationVisitor::Handle_ConstDefExpression (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(2);
		yv = TRANSLATOR.Translate_ConstAssignExpression(yy[1].str(), yy[2].expr());
		yyapply;
	}
} 

///////////////////////////////////////////////////////////
// Assign: Lvalue(1.expr) Rvalue(2.expr)
//

#define	lvalue	yy[1].expr()

void AST::TranslationVisitor::Handle_AssignExpr (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(2);
		yv = TRANSLATOR.Translate_AssignExpr(lvalue, yy[2].expr()); 
		lvalue->SetInitialised(); 
		Unparse_BinaryOp(yv.expr(), lvalue, ASSIGN, yy[2].expr());
		yyapply;
	}
}

//*****************************

#define	RESULT(opcode,token)	\
		return std::pair<util_ui8, util_ui32>(DeltaIC_##opcode, token)

static std::pair<util_ui32,util_ui32> GetArithOpCode (const std::string& op) {
	if (op == AST_TAG_OP_ADD_ASSIGN)	RESULT(ADD, ADD_A);
	if (op == AST_TAG_OP_MUL_ASSIGN)	RESULT(MUL, MUL_A);
	if (op == AST_TAG_OP_SUB_ASSIGN)	RESULT(SUB, SUB_A);
	if (op == AST_TAG_OP_DIV_ASSIGN)	RESULT(DIV, DIV_A);
	if (op == AST_TAG_OP_MOD_ASSIGN)	RESULT(MOD, MOD_A);
	if (op == AST_TAG_OP_ADD)			RESULT(ADD, ADD);
	if (op == AST_TAG_OP_MUL)			RESULT(MUL, MUL);
	if (op == AST_TAG_OP_SUB)			RESULT(SUB, SUB);
	if (op == AST_TAG_OP_DIV)			RESULT(DIV, DIV);
	if (op == AST_TAG_OP_MOD)			RESULT(MOD, MOD);

	DASSERT(false);
	return std::pair<util_ui8, util_ui32>(0,0);
}

static std::pair<util_ui32,util_ui32> GetRelatOpCode (const std::string& op) {
	if (op == AST_TAG_OP_LT)			RESULT(JLT, LT);
	if (op == AST_TAG_OP_GT)			RESULT(JGT, GT);
	if (op == AST_TAG_OP_LE)			RESULT(JLE, LE);
	if (op == AST_TAG_OP_GE)			RESULT(JGE, GE);
	if (op == AST_TAG_OP_EQ)			RESULT(JEQ, EQ);
	if (op == AST_TAG_OP_NE)			RESULT(JNE, NE);

	DASSERT(false);
	return std::pair<util_ui8, util_ui32>(0,0);
}

#undef	RESULT

//*****************************

void AST::TranslationVisitor::Handle_AssignArithExpr (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(2);
		std::string	op							= node->GetTag();
		std::pair<util_ui8, util_ui32>	opcode	= GetArithOpCode(op);
		yv = TRANSLATOR.Translate_AssignArithExpr(lvalue, yy[2].expr(), opcode.first, Unparse_Token(opcode.second).c_str()); 
		Unparse_BinaryOp(yv.expr(), lvalue, opcode.second, yy[2].expr());
		yyapply;
	}
} 

#undef lvalue

//*****************************
// BinaryExpr: left(1.expr) op (from tag) right(2.expr)
//

#define	left	yy[1].expr()
#define	right	yy[2].expr()

void AST::TranslationVisitor::Handle_ArithmeticExpression (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(2);
		std::string	op							= node->GetTag();
		std::pair<util_ui8, util_ui32>	opcode	= GetArithOpCode(op);
		yv = TRANSLATOR.Translate_ArithmeticExpression(left, opcode.first, right, Unparse_Token(opcode.second).c_str()); 
		Unparse_BinaryOp(yv.expr(), left, opcode.second, right);
		yyapply;
	}
}
 
//*****************************

void AST::TranslationVisitor::Handle_RelationalExpr (AST_VISITOR_ARGS){ 

	if (!entering) {
		yyrule(2);
		std::string	op							= node->GetTag();
		std::pair<util_ui8, util_ui32>	opcode	= GetRelatOpCode(op);
		yv = TRANSLATOR.Translate_RelationalExpr(left, right, opcode.first, Unparse_Token(opcode.second).c_str()); 
		Unparse_BinaryOp(yv.expr(), left, opcode.second, right);
		yyapply;
	}
} 

//*****************************

void AST::TranslationVisitor::Handle_ExpressionANDOR( AST_VISITOR_ARGS){ 
	if (!entering) {
		{
		yyrule(1);
		DNPTR(left)->CheckUninitialised();
		DNPTR(left)->AdaptToBool();
		yv = left;
		yyapply;
		}		// Block necessary to emulate successive rules.
		M();	// Mark for backpatching.
	}
}

//*****************************

void AST::TranslationVisitor::Handle_BooleanExpression (
		AST_VISITOR_ARGS, 
		DeltaExpr* (Translator::*f)(DeltaExpr*, DeltaQuadAddress, DeltaExpr*),
		util_ui32 token
	){ 
	if (!entering) {
		yyrule(3);
		yv = (TRANSLATOR.*f)(yy[1].expr(), yy[2].quadNo(), yy[3].expr()); 
		Unparse_BinaryOp(yv.expr(), yy[1].expr(), token, yy[3].expr());
		yyapply;
	}
} 

void AST::TranslationVisitor::Handle_ExprANDExpr (AST_VISITOR_ARGS)
	{ Handle_BooleanExpression(AST_VISITOR_ACTUALS, &Translator::Translate_ExprANDExpr, AND); }

void AST::TranslationVisitor::Handle_ExprORExpr (AST_VISITOR_ARGS)
	{ Handle_BooleanExpression(AST_VISITOR_ACTUALS, &Translator::Translate_ExprORExpr, OR); }

//*****************************

void AST::TranslationVisitor::Handle_UnaryExpression (
		AST_VISITOR_ARGS,
		DeltaExpr* (Translator::*f)(DeltaExpr*),
		util_ui32 token,
		const std::string& infix
	){ 
	if (!entering) {
		yyrule(1);
		yv = (TRANSLATOR.*f)(left);
		Unparse_UnaryOp(yv.expr(), token, left, infix);
		yyapply;
	}
}

void AST::TranslationVisitor::Handle_NOTExpression (AST_VISITOR_ARGS)
	{ Handle_UnaryExpression(AST_VISITOR_ACTUALS, &Translator::Translate_NOTExpression, NOT, " "); }

void AST::TranslationVisitor::Handle_UMINUSExpression (AST_VISITOR_ARGS)
	{ Handle_UnaryExpression(AST_VISITOR_ACTUALS, &Translator::Translate_UMINUSExpression, SUB); }

//*****************************

void AST::TranslationVisitor::Handle_AdaptBoolExpression (AST_VISITOR_ARGS) {
	if (!entering) 
		{ yyrule(1); yv = yy[1].expr()->AdaptIfBool(); yyapply; }
}

//*****************************

#define	RESULT(opcode,token,after)	\
		return utriple<util_ui8, util_ui32,bool>(DeltaIC_##opcode, token, after)

static utriple<util_ui8, util_ui32,bool> GetIncDecOpCode (const std::string& op) {
	if (op == AST_LVALUE_PLUSPLUS)		RESULT(ADD, PLUSPLUS,	true);
	if (op == AST_LVALUE_MINUSMINUS)	RESULT(SUB, MINUSMINUS,	true);
	if (op == AST_PLUSPLUS_LVALUE)		RESULT(ADD, PLUSPLUS,	false);
	if (op == AST_MINUSMINUS_LVALUE)	RESULT(SUB, MINUSMINUS,	false);

	DASSERT(false);
	return utriple<util_ui8, util_ui32,bool>(0,0,false);
}

#undef	RESULT

void AST::TranslationVisitor::Handle_TermLvalueArith (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(1);
		utriple<util_ui8, util_ui32, bool> opcodes = GetIncDecOpCode(node->GetTag());
		yv = TRANSLATOR.Translate_TermLvalue(yy[1].expr(), opcodes.third, opcodes.first); 
		Unparse_UnaryIncDec(yv.expr(), opcodes.second, yy[1].expr(), opcodes.third);
		yyapply;
	}
}

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_PrimaryExpression (AST_VISITOR_ARGS) {
	if (!entering) {
		TreeNode* child = DPTR(node)->GetChild(AST_CHILD_EXPR);
		DASSERT(child);
		if (AST::ValidationVisitor::IsConstValue(DPTR(child)))
			Unparse_ConstValue(yytop.expr());
		else if (AST::ValidationVisitor::IsFunctionAndTableObject(DPTR(child)))
			Handle_AdaptTableContent(AST_VISITOR_ACTUALS);
	}
}

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_TernaryCondition (AST_VISITOR_ARGS) {
	if (!entering) {
		yyrule(1);
		yv = TRANSLATOR.Translate_Condition(left);
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_TernaryLeftExpr (AST_VISITOR_ARGS) {
	if (entering)
		M();
	else {
		yyrule(1);
		yv = DNPTR(left)->AdaptIfBool(); 
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_TernaryRightExpr (AST_VISITOR_ARGS) {
	if (entering) {
		N(); 
		M();
	}
	else {
		yyrule(1);
		yv = DNPTR(left)->AdaptIfBool(); 
		yyapply;
	}
}

//*****************************
// Ternary:	Condition(1.expr) ? M(2.quadNo) LeftExpr(3.expr) : N(4.quadNo) M(5.quadNo) RightExpr(6.expr)
//
void AST::TranslationVisitor::Handle_Ternary (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(6);
		yv =	TRANSLATOR.Translate_Ternary(
					yy[1].expr(),	yy[3].expr(),	yy[6].expr(), 
					yy[2].quadNo(), yy[5].quadNo(), yy[4].quadNo()
				); 
		Unparse_TernaryOp(yv.expr(), yy[1].expr(), yy[3].expr(), yy[6].expr());
		yyapply;
	}
} 

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_Expr (AST_VISITOR_ARGS){
	if (const util_ui32 line = LINE(node)) {
		if (entering)
			M();
		else {
			yyrule(2);
			QUADS.SetQuadLine(yy[1].quadNo(), line);
			yv = yy[2].expr();
			yyapply;
		}
	}
}

//*****************************

void AST::TranslationVisitor::Handle_ParenthesisedExpr (AST_VISITOR_ARGS){
	if (!entering) {
		yyrule(1);
		Unparse_ParenthesisedExpr(yv = DNPTR(yy[1].expr()));
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_ExprList (AST_VISITOR_ARGS){ 

	if (!entering) {

		DeltaExpr*	expr	= NIL_EXPR;
		const util_ui32	n	= DPTR(node)->GetTotalChildren();

		yyrule(n);	// All exprs on the stack.

		for (util_ui32 i = 1; i <= n; ++i)
			expr = TRANSLATOR.Translate_ExpressionList(expr, yy[i].expr());

		yv = expr;
		yyapply;	
	}	
}

//*****************************

void AST::TranslationVisitor::Handle_ExpressionListItem (AST_VISITOR_ARGS) {
	if (!entering) {
		yyrule(1);
		yv = TRANSLATOR.Translate_ExpressionListItem(yy[1].expr());
		yyapply;	
	}	
}

///////////////////////////////////////////////////////////

#define	HANDLE_CONSTVALUE(val)						\
	if (!entering) {								\
		yyrule;										\
		yv = TRANSLATOR.Translate_ConstValue(val);	\
		yyapply;									\
	}

void AST::TranslationVisitor::Handle_NumConst (AST_VISITOR_ARGS)
	{ HANDLE_CONSTVALUE(DPTR(node)->GetAttribute(AST_ATTRIBUTE_CONSTVALUE)->GetDouble()); }

//*****************************

void AST::TranslationVisitor::Handle_NilConst (AST_VISITOR_ARGS)
	{ HANDLE_CONSTVALUE(UEMPTY); }

//*****************************

void AST::TranslationVisitor::Handle_BoolConst (AST_VISITOR_ARGS) {
	bool val = DPTR(node)->GetAttribute(AST_ATTRIBUTE_CONSTVALUE)->GetBool();
	HANDLE_CONSTVALUE(val); 
}

//*****************************

void AST::TranslationVisitor::Handle_StringItems (AST_VISITOR_ARGS, DeltaExpr* (Translator::*f)(const std::string&, DeltaExpr*), void(*g)(DeltaExpr*)){ 
	if (!entering) {
		yyrule;
		std::list<std::string>&	l		= DPTR(node)->GetAttribute(AST_ATTRIBUTE_ITEMS)->GetStringList();
		DeltaExpr*				expr	= NIL_EXPR;

		for (std::list<std::string>::iterator i = l.begin(); i != l.end(); ++i)
			expr = (TRANSLATOR.*f)(i->c_str(), expr);

		(*g)(yv = expr);
		yyapply;
	}
} 

void AST::TranslationVisitor::Handle_StringConst (AST_VISITOR_ARGS)
	{ Handle_StringItems(AST_VISITOR_ACTUALS, &Translator::Translate_StringConst, &Unparse_ConstValue); }

//*****************************

void AST::TranslationVisitor::Handle_StringifyDottedIdents (AST_VISITOR_ARGS)
	{ Handle_StringItems(AST_VISITOR_ACTUALS, &Translator::Translate_StringifyDottedIdents, &Unparse_Stringify); }

//*****************************

void AST::TranslationVisitor::Handle_StringifyNamespaceIdent (AST_VISITOR_ARGS){ 
	if (!entering) {
		const util_ui32 n = DPTR(node)->GetTotalChildren();
		DASSERT(n > 1);
		yyrule(n);
		std::list<std::string> namespacePath;
		for (util_ui32 i = 1; i <= n; ++i)
			namespacePath.push_back(yy[i].str());
		Unparse_Stringify(yv = TRANSLATOR.Translate_StringifyNamespaceIdent(namespacePath));
		yyapply;
	}
} 

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_KwdExpr (AST_VISITOR_ARGS, DeltaExpr* (Translator::*f)(void), util_ui32 token){ 
	if (!entering) {
		yyrule;
		Unparse_BuiltIn(yv = (TRANSLATOR.*f)(), token);
		yyapply;
	}
}

void AST::TranslationVisitor::Handle_KwdExpr (
		AST_VISITOR_ARGS,
		DeltaExpr* (*f)(Translator& translator),
		util_ui32 token
	){ 
	if (!entering) {
		yyrule;
		Unparse_BuiltIn(yv = (*f)(TRANSLATOR), token);
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_SELF (AST_VISITOR_ARGS)
	{ Handle_KwdExpr(AST_VISITOR_ACTUALS, &Translator::Translate_SELF, SELF); }

void AST::TranslationVisitor::Handle_ARGUMENTS (AST_VISITOR_ARGS)
	{ Handle_KwdExpr(AST_VISITOR_ACTUALS, &Translator::Translate_ARGUMENTS, ARGUMENTS); }

void AST::TranslationVisitor::Handle_TRIPLE_DOT (AST_VISITOR_ARGS)
	{ Handle_KwdExpr(AST_VISITOR_ACTUALS, &Translator::Translate_TRIPLE_DOT, TRIPLE_DOT); }

static DeltaExpr* Translate_LAMBDA_REF_Adapter (Translator& translator)
	{ return translator.Translate_LAMBDA_REF(); }

void AST::TranslationVisitor::Handle_LAMBDA_REF (AST_VISITOR_ARGS)
	{ Handle_KwdExpr(AST_VISITOR_ACTUALS, &Translate_LAMBDA_REF_Adapter, LAMBDA_REF); }

void AST::TranslationVisitor::Handle_NEWSELF (AST_VISITOR_ARGS)
	{ Handle_KwdExpr(AST_VISITOR_ACTUALS, &Translator::Translate_NEWSELF, NEWSELF); }

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_BuiltInStmt (AST_VISITOR_ARGS, Stmt* (Translator::*f)(void), util_ui32 token){ 
	if (!entering) {
		yyrule;
		Unparse_BuiltInStmt(yv = (TRANSLATOR.*f)(), token);
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_BREAK (AST_VISITOR_ARGS)
	{ Handle_BuiltInStmt(AST_VISITOR_ACTUALS, &Translator::Translate_BREAK, BREAK); }

void AST::TranslationVisitor::Handle_CONTINUE (AST_VISITOR_ARGS)
	{ Handle_BuiltInStmt(AST_VISITOR_ACTUALS, &Translator::Translate_CONTINUE, CONTINUE); }

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_FunctionParenthesisForm (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(1);
		yv = TRANSLATOR.Translate_FunctionParenthesisForm(yy[1].sym()); 
		Unparse_FunctionParenthesisForm(yv.expr(), yy[1].sym());
		yyapply;
	}
}

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_LateBoundArgument (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(1);
		yv = left->AdaptAsArgumentVariable(); 
		DNPTR(yv.expr())->SetLateBound();
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_Argument (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(1);
		yv = left->AdaptAsArgumentVariable(); 
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_FunctionCall (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(2);
		UNPARSABLE_GET(Unparse_FunctionCall(yy[1].expr(), yy[2].expr()));
		yv = TRANSLATOR.Translate_FunctionCall(yy[1].expr()->CheckUninitialised(), yy[2].expr()); 
		UNPARSABLE_SET(yv.expr());	
		yyapply;
	}
} 

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_NamespaceLvalue (AST_VISITOR_ARGS){ 
	if (!entering) {
		const util_ui32 n = DPTR(node)->GetTotalChildren();
		DASSERT(n > 1);
		yyrule(n);
		std::list<std::string> namespacePath;
		for (util_ui32 i = 1; i < n; ++i)
			namespacePath.push_back(yy[i].str());
		std::string unparsed;
		yv = TRANSLATOR.Translate_NamespaceLvalue(namespacePath, yy[n].str(), &unparsed); 
		Unparse_Var(yv.expr(), unparsed);
		yyapply;
	}
}

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_LvalueByName (
		AST_VISITOR_ARGS,
		DeltaExpr* (Translator::*f)(const std::string& id),
		util_i32 token
	) {
	if (!entering) {
		yyrule(1);
		const char* name = yy[1].str();
		if (token != -1) {
			DASSERT(token >= 0);
			Unparse_Var(yv = (TRANSLATOR.*f)(name), name, (util_ui32) token);
		}
		else
			Unparse_Var(yv = (TRANSLATOR.*f)(name), name);
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_Lvalue (AST_VISITOR_ARGS)
	{ Handle_LvalueByName(AST_VISITOR_ACTUALS, &Translator::Translate_Lvalue); }

void AST::TranslationVisitor::Handle_AttrLvalue (AST_VISITOR_ARGS)
	{ Handle_LvalueByName(AST_VISITOR_ACTUALS, &Translator::Translate_AttrLvalue, ATTRIBUTE); }

void AST::TranslationVisitor::Handle_StaticLvalue (AST_VISITOR_ARGS)
	{ Handle_LvalueByName(AST_VISITOR_ACTUALS, &Translator::Translate_StaticLvalue); }

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_LocalLvalue (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(1);
		const char* name = yy[1].str();
		Unparse_Var(yv = TRANSLATOR.Translate_Lvalue(name, 0), name, LOCAL);
		yyapply;
	}
} 

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_StringValue (AST_VISITOR_ARGS) {
	if (!entering) {
		yyrule;
		yv = TRANSLATOR.Translate_StringConst(VALUE_STRING(node));
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_DotIndexIdent (AST_VISITOR_ARGS)
	{ Handle_StringValue(AST_VISITOR_ACTUALS); }

void AST::TranslationVisitor::Handle_DotIndexOpString (AST_VISITOR_ARGS)
	{ Handle_StringValue(AST_VISITOR_ACTUALS); }

void AST::TranslationVisitor::Handle_DotIndexStringConst (AST_VISITOR_ARGS)
	{ }	// $$ = $1

void AST::TranslationVisitor::Handle_BracketIndexOperatorMethod (AST_VISITOR_ARGS)
	{ Handle_StringValue(AST_VISITOR_ACTUALS); }

///////////////////////////////////////////////////////////

static bool IsDotIndex (TreeNode* node) {
	const std::string& tag = node->GetTag();
	if (tag ==AST_TAG_DOTINDEX_IDENT)		return true;
	if (tag ==AST_TAG_DOTINDEX_OPSTRING)	return true;
	if (tag ==AST_TAG_DOTINDEX_STRINGCONST)	return true;
	return false;
}

//*****************************
// TableContent: Table(1.expr) Index(2.expr)
//

void AST::TranslationVisitor::Handle_TableContent (
		AST_VISITOR_ARGS, 
		DeltaExpr*	(Translator::*f)(DeltaExpr*, DeltaExpr*), 
		bool bounded
	) {
	if (!entering) {

		yyrule(2);

		DeltaExpr* index = yy[2].expr();
		yv = (TRANSLATOR.*f)(yy[1].expr()->CheckUninitialised(), index); 

		if (IsDotIndex(node->GetChild(AST_CHILD_INDEX))) {
			DASSERT(index->GetType() == DeltaExprString);
			if (!bounded)
				Unparse_TableContentDot(yv.expr(), yy[1].expr(), DPTR(index)->strConst.c_str()); 
			else
				Unparse_TableContentDoubleDot(yv.expr(), yy[1].expr(), DPTR(index)->strConst.c_str()); 
		}
		else
			if (!bounded)
				Unparse_TableContentBracket(yv.expr(), yy[1].expr(), index); 
			else
				Unparse_TableContentDoubleBracket(yv.expr(), yy[1].expr(), index); 

		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_TableContentDot (AST_VISITOR_ARGS)
	{ Handle_TableContent(AST_VISITOR_ACTUALS, &Translator::Translate_TableContent, false); }

void AST::TranslationVisitor::Handle_BoundedTableContentDot (AST_VISITOR_ARGS)
	{ Handle_TableContent(AST_VISITOR_ACTUALS, &Translator::Translate_BoundedTableContent, true); }

void AST::TranslationVisitor::Handle_TableContentBracket (AST_VISITOR_ARGS)
	{ Handle_TableContent(AST_VISITOR_ACTUALS, &Translator::Translate_TableContent, false); }

void AST::TranslationVisitor::Handle_BoundedTableContentBracket (AST_VISITOR_ARGS)
	{ Handle_TableContent(AST_VISITOR_ACTUALS, &Translator::Translate_BoundedTableContent, true); }

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_AdaptTableContent (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(1);
		yv = DNPTR(yy[1].expr())->AdaptIfTableContent();
		yyapply;
	}
}

//*****************************
// TableConstructor: Table(1.elems) Elem*(i.elems)
//
void AST::TranslationVisitor::Handle_TableConstructor (AST_VISITOR_ARGS){ 

	if (entering) {
		yysetsourceinfo();
		PARSEPARMS.InTableExpr().enter(); 
		yypush(TRANSLATOR.Translate_TablePrefix());
	} 
	else {

		TableElements*	elems	= (TableElements*) 0;
		const util_ui32		n	= DPTR(node)->GetTotalChildren() + 1;	// The table being the first element.

		yyrule(n);	// All exprs on the stack.

		for (util_ui32 i = 2; i <= n; ++i)
			if (i == 2)
				elems = yy[i].elems();
			else {
				UNPARSABLE_GET(Unparse_TableElements(elems, yy[i].elems()));
				elems = TRANSLATOR.Translate_TableElements(elems, yy[i].elems()); 
				UNPARSABLE_SET(elems);
			}			

		DeltaExpr* table = yy[1].expr();
		PARSEPARMS.InTableExpr().exit();
		UNPARSABLE_GET(Unparse_TableConstructor(elems)); 
		table = TRANSLATOR.Translate_TableConstructor(table, elems); 
		UNPARSABLE_SET(table);

		yv = table;
		yyapply;	
	}	
} 

//*****************************

void AST::TranslationVisitor::Handle_UnindexedValue (AST_VISITOR_ARGS){ 
	if (entering)
		M();
	else {
		yyrule(2);
		yv = TRANSLATOR.Translate_UnindexedValue(yy[2].expr(), yy[1].quadNo(), LINE(node));
		yyapply;
	}
}	

//*****************************

void AST::TranslationVisitor::Handle_IdentIndexElement (AST_VISITOR_ARGS) {
	if (entering)
		M();
	else {
		yyrule(3);
		yv = TRANSLATOR.Translate_IdentIndexElement(yy[2].str(), yy[3].expr(), yy[1].quadNo(), LINE(node));
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_FunctionExpression (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(1);
		yv = TRANSLATOR.Translate_FunctionExpresssion(yy[1].sym());
		yyapply;
	}
}	

//*****************************

void AST::TranslationVisitor::Handle_DottedIdent (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule;
		const std::string name = NAME(node);
		Unparse_DottedIdent(yv = TRANSLATOR.Translate_ConstValue(name), name.c_str());
		yyapply;
	}
} 

//*****************************

void AST::TranslationVisitor::Handle_OperatorIndex (AST_VISITOR_ARGS)
	{ Handle_StringValue(AST_VISITOR_ACTUALS); }

//*****************************

void AST::TranslationVisitor::Handle_TableValues (AST_VISITOR_ARGS)
	{ Handle_ExprList(AST_VISITOR_ACTUALS); }

void AST::TranslationVisitor::Handle_TableIndices (AST_VISITOR_ARGS) {

	if (!entering) {

		DeltaExpr*	expr	= NIL_EXPR;
		const util_ui32	n	= DPTR(node)->GetTotalChildren();

		yyrule(n);	// All exprs on the stack.

		for (util_ui32 i = 1; i <= n; ++i)
			expr = TRANSLATOR.Translate_ExpressionList(expr, yy[i].expr()->AdaptAsArgumentVariable());

		yv = expr;
		yyapply;	
	}						
}

//*****************************
// IndexedValues: { IndexedList(1.expr) : ExpressionList(2.expr) }
//
void AST::TranslationVisitor::Handle_IndexedValues (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(2);
		yv = TRANSLATOR.Translate_IndexedValues(yy[1].expr(), yy[2].expr());
		Unparse_IndexedValues(yv.elems());
		yyapply;
	}
} 

//*****************************
// Attribute: ident (1.str) Set(2.expr) Get(3.expr)
//
void AST::TranslationVisitor::Handle_NewAttribute (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(3);
		DeltaExpr* attr = TRANSLATOR.Translate_NewAttribute(yy[1].str(), yy[2].expr(), yy[3].expr());
		Unparse_UnindexedValue(yv = TRANSLATOR.Translate_TableElement(attr), attr); 
		yyapply;
	}
} 

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_AttributeMethod (AST_VISITOR_ARGS) {
	if (entering)
		M();
	else {
		yyrule(2);
		yv = TRANSLATOR.Translate_AttributeMethod(yy[2].expr(), yy[1].quadNo(), LINE(node));
		yyapply;
	}
}

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_Condition (AST_VISITOR_ARGS){ 
	if (!entering) {
		{
		yyrule(1);
		yv = TRANSLATOR.Translate_Condition(yy[1].expr()); 
		TRANSLATOR.Translate_BasicStmt(LINE(node));
		yyapply;
		}
		M();
	}
} 

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_IfCondition (AST_VISITOR_ARGS){ 
	if (!entering) {
		Handle_Condition(AST_VISITOR_ACTUALS);
		TRANSLATOR.Translate_IfStmtPrefix();
	}
}

//*****************************
// If: Condition(1.expr) M(2.quadNo) Stmt(3.stmt)
//
void AST::TranslationVisitor::Handle_IfStmt (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(3);
		TRANSLATOR.Translate_IfStmt(yy[1].expr(), yy[2].quadNo());
		Unparse_If(yv = yy[3].stmt(), yy[1].expr(), yy[3].stmt());
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_ElsePrefix (AST_VISITOR_ARGS){ 
	if (entering) {
		TRANSLATOR.Translate_ElseStmtPrefix();
		N(); 
		M(); 
	} 
}

//*****************************
// IfElse: Condition(1.expr) M(2.quadNo) Stmt(3.stmt) N(4.quadNo) M(5.quadNo) Stmt(6.stmt)
//
void AST::TranslationVisitor::Handle_IfElseStmt (AST_VISITOR_ARGS){ 
	
	if (!entering) {
		yyrule(6);

		TRANSLATOR.Translate_IfElseStmt(yy[1].expr(), yy[2].quadNo(), yy[4].quadNo(), yy[5].quadNo());
		UNPARSABLE_GET(Unparse_IfElse(yy[1].expr(), yy[3].stmt(), yy[6].stmt()));
		DELTASYMBOLS.ResetTemp();
		yv = TRANSLATOR.Translate_Stmts(yy[3].stmt(), yy[6].stmt()); 
		UNPARSABLE_SET(yv.stmt());

		yyapply;
	}
} 

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_WhileCondition (AST_VISITOR_ARGS){ 
	if (!entering) {
		Handle_Condition(AST_VISITOR_ACTUALS);
		TRANSLATOR.Translate_WhilePrefix();
	}
}

//*****************************
// While: WHILE M(1.quadNo) Condition(2.expr) M(3.quadNo) Stmt(4.stmt)
//
void AST::TranslationVisitor::Handle_WhileStmt (AST_VISITOR_ARGS){ 
	if (entering)
		M();
	else {
		yyrule(4);

		const util_ui32 line = LINE(DPTR(node)->GetChild(AST_CHILD_STMT));
		UNPARSABLE_GET(Unparse_While(yy[2].expr(), yy[4].stmt()));
		yv = TRANSLATOR.Translate_WhileStmt(yy[2].expr(), yy[1].quadNo(), yy[3].quadNo(), yy[4].stmt(), line); 
		UNPARSABLE_SET(yv.stmt());

		yyapply;
	}
} 

///////////////////////////////////////////////////////////
// ForInit: M(1.quadNo) ExprList(2.expr) M(3.quadNo) => Stmt M
//
void AST::TranslationVisitor::Handle_ForInit (AST_VISITOR_ARGS){
	if (entering) {
		TRANSLATOR.Translate_ForOpening(); 
		M();
	}
	else {
		{
		yyrule(2);

		Unparse_ForInitList(yv = NEW_STMT, yy[2].expr());

		const util_ui32 line = LINE(node);
		DASSERT(line);

		TRANSLATOR.Translate_ForPrefix(yy[1].quadNo(), line);
		TRANSLATOR.Translate_BasicStmt(line);

		yyapply;
		}

		M();
	}
}

//*****************************
// ForSuffix: M(1.quadNo) ExprList(2.expr) N(3.quadNo) M(4.quadNo) => Stmt N M
//
void AST::TranslationVisitor::Handle_ForSuffix (AST_VISITOR_ARGS){
	
	if (entering) {
		if (DPTR(node)->GetTotalChildren()) {
			M();
		}
	}
	else {
		{
		const util_ui32 n = DPTR(node)->GetTotalChildren(); // Subexpressions.
		yyrule(n ? 2 : 1); // M and ExprList already translated

		yv = NEW_STMT; 
		if (n) {		// Has an expression list.
			UNPARSABLE_GET(Unparse_ExprList(yy[2].expr()));
			UNPARSABLE_SET(yv.stmt()); 		
			SET_QUAD_LINE(1, true);
			TRANSLATOR.Translate_BasicStmt(line);
		}
		else // Has a null expr for expression list.
			DASSERT(!yy[1].expr());
		yyapply;
		}

		N();
		M();
	}
}

//*****************************
// Condition: M Expr => Expr M
//
void AST::TranslationVisitor::Handle_ForCond (AST_VISITOR_ARGS){

	if (entering)
		M();
	else {
		{
		yyrule(2);
		yv = TRANSLATOR.Translate_Condition(yy[2].expr()); 
		
		SET_QUAD_LINE(1, true);
		TRANSLATOR.Translate_BasicStmt(line);

		yyapply;
		}

		M();
	}
}

//*****************************
//For: Init(1.stmt) M(2.quadNo) Cond(3.expr) M(4.quadNo) Suffix(5.stmt) N(6.quadNo) M(7.quadNo) Stmt(8.stmt)

void AST::TranslationVisitor::Handle_ForStmt (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(8);
		UNPARSABLE_GET(Unparse_For(yy[1].stmt(), yy[3].expr(), yy[5].stmt(), yy[8].stmt()));
		yv = TRANSLATOR.Translate_ForStmt(yy[3].expr(), yy[2].quadNo(), yy[4].quadNo(), yy[7].quadNo(), yy[6].quadNo(), yy[8].stmt()); 
		UNPARSABLE_SET(yv.stmt());
		yyapply;
	}
} 

///////////////////////////////////////////////////////////
// ForeachPrefix: Lvalue(1.expr) [Lvalue(2.expr)] M(3.quadNo) Container(4.expr)
//
void AST::TranslationVisitor::Handle_ForeachContainer (AST_VISITOR_ARGS){ 
	if (entering)
		M();
	else {
		
		{
#define	VALUE_CONTAINER_FORM		3
#define	INDEX_VALUE_CONTAINER_FORM	4
#define	CONTAINER_POS				n
#define	M_POS						(n-1)
#define	VALUE_POS					(n == VALUE_CONTAINER_FORM ? 1 : 2)
#define	INDEX_POS					1

		const util_ui32 n = DPTR(DPTR(node->GetParent()))->GetTotalChildren();
		DASSERT(n == VALUE_CONTAINER_FORM || n == INDEX_VALUE_CONTAINER_FORM);
		yyrule(n);
		
		SET_QUAD_LINE(M_POS, false);
		
		UNPARSABLE_GET(
			Unparse_ForeachPrefix(
				yy[VALUE_POS].expr(), 
				n == VALUE_CONTAINER_FORM ? NIL_EXPR : yy[INDEX_POS].expr(),
				yy[CONTAINER_POS].expr()
			)
		); 

		if (n == VALUE_CONTAINER_FORM)
			yv = TRANSLATOR.Translate_ForeachPrefix(yy[VALUE_POS].expr(), NIL_EXPR, yy[CONTAINER_POS].expr()); 
		else
			yv = TRANSLATOR.Translate_ForeachPrefix(yy[VALUE_POS].expr(), yy[INDEX_POS].expr(), yy[CONTAINER_POS].expr()); 
		UNPARSABLE_SET(yv.stmt());		

		yyapply;

#undef	VALUE_CONTAINER_FORM
#undef	INDEX_VALUE_CONTAINER_FORM
#undef	CONTAINER_POS
#undef	M_POS		
#undef	VALUE_POS	
#undef	INDEX_POS	

		}

		M();
	}
} 

//*****************************
// Foreach: Prefix(1.stmt) M(2.quadNo) Stmt(3.stmt)
//
void AST::TranslationVisitor::Handle_ForeachStmt (AST_VISITOR_ARGS){ 

	if (!entering) {
		yyrule(3);

		UNPARSABLE_GET(Unparse_ForeachStmt(yy[1].stmt(), yy[3].stmt()));
		yv = TRANSLATOR.Translate_ForeachStmt(yy[1].stmt(), yy[3].stmt(), yy[2].quadNo());
		SET_QUAD_LINE(2, false);
		UNPARSABLE_SET(yv.stmt());	

		yyapply;
	}
} 

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_THROW (AST_VISITOR_ARGS){ 

	if (!entering) {
		yyrule(1);
		yv = TRANSLATOR.Translate_THROW(yy[1].expr()); 
		Unparse_ExprStmt(yv.stmt(), THROW, yy[1].expr());
		yyapply;
	}
} 

//*****************************
// Try(1.quadNo) Stmt(2.stmt) Trap(3.quadNo) JumpOver(4.quadNo) VarName(5.str)
//
void AST::TranslationVisitor::Handle_ExceptionVar (AST_VISITOR_ARGS){ 
	if (entering) {
		yypush(TRANSLATOR.Translate_TRAP());			// Trap quad
		yypush(TRANSLATOR.Translate_TrapJumpOver());	// JumpOver quad
	}
	else {
		yyrule(5);
		if (yv = TRANSLATOR.Translate_Lvalue(yy[5].str()))
			DPTR(yv.expr())->SetInitialised();

		TRANSLATOR.Translate_TrapStart(yy[1].quadNo(), yy[3].quadNo(), yv.expr());
		yypreserve;
		yyapply;
	}
} 

//*****************************
// Try(1.quadNo) Stmt(2.stmt) Trap(3.quadNo) JumpOver(4.quadNo) VarName(5.str) Var(6.expr) Stmt(7.stmt)
//
void AST::TranslationVisitor::Handle_Exception (AST_VISITOR_ARGS){ 
	if (entering)
		yypush(TRANSLATOR.Translate_TRY());	// Try
	else {
		yyrule(7);

		TRANSLATOR.Translate_TrapEnd(yy[4].quadNo()); ; 
		UNPARSABLE_GET(Unparse_TryTrap(yy[2].stmt(), yy[7].stmt(), yy[6].expr()));
		DELTASYMBOLS.ResetTemp();
		yv = TRANSLATOR.Translate_Stmts(yy[2].stmt(), yy[7].stmt());
		UNPARSABLE_SET(yv.stmt());

		yyapply;
	}
} 

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_ScopedStmt (AST_VISITOR_ARGS) {
	if (node->GetTag() != AST_TAG_EMPTY_STMT && node->GetTag() != AST_TAG_COMPOUND)
		if (entering)
			TRANSLATOR.Translate_CompoundBegin();
		else
			TRANSLATOR.Translate_CompoundEnd();
}

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_QuasiQuotes (AST_VISITOR_ARGS){ 
	if (entering) {
		yyrule;
		AST::Node* child = (Node *) node->GetChild(AST_CHILD_EXPR);

		Translator& translator = TRANSLATOR;
		const std::string quoted = UnparseVisitor()(child);
		std::string ns;
		DeltaExpr* func = translator.Translate_NamespaceLvalue(NameList(1, DELTA_STDLIB_NAMESPACE), "vmparsequotedelements", &ns);
		DeltaExpr* code = translator.Translate_ConstValue(quoted);
		
		if (!VISITOR->quasiQuoteOnErrorFunc) {
			// On first quasi-quote encountered generate an empty OnError callback for vmparsequotedelements
			// Note that this function may be reused out-of-scope.
			DeltaQuadAddress quad = QUADS.NextQuadNo();
			DeltaSymbol* funcdef = translator.Translate_Function((const char *) 0, DELTA_FUNCCLASS_PROGRAMFUNCTION);
			translator.Translate_FunctionHeader(funcdef);
			DELTASYMBOLS.PushAndResetTempCounter();
			translator.Translate_CompoundBegin();
			translator.Translate_CompoundEnd();
			VISITOR->quasiQuoteOnErrorFunc = translator.Translate_Function(funcdef, (Stmt*) 0, quad, 0);
		}

		DeltaExpr* args = translator.Translate_FunctionExpresssion(VISITOR->quasiQuoteOnErrorFunc);
		DASSERT(args || COMPMESSENGER.ErrorsExist());
		if (args)
			args->next = code;

		yv = translator.Translate_FunctionCall(func, args);

		EscapeTranslationVisitor visitor(VISITOR->GET_COMPONENT_DIRECTORY());
		bool hasEscapes = visitor(child);
		EscapeTranslationVisitor::EvaluationStack::StackValues& values = visitor.GetEvalStack().GetValues();
		for (EscapeTranslationVisitor::EvaluationStack::StackValues::reverse_iterator i = values.rbegin(); i != values.rend(); ++i) {
			func = translator.Translate_NamespaceLvalue(NameList(1, DELTA_STDLIB_NAMESPACE), "ast_escape", &ns);
			SetSourceInfo(i->first.first, "", entering, closure);
			DeltaExpr* args = i->first.second.expr()->AdaptIfBool();
			DASSERT(args || COMPMESSENGER.ErrorsExist());
			if (args)
				args->next = yv.expr();
			yv = translator.Translate_FunctionCall(func, args, yv.expr()->sym);
		}
		if (hasEscapes) {
			func = translator.Translate_NamespaceLvalue(NameList(1, DELTA_STDLIB_NAMESPACE), "ast_decr_esc_cardinalities", &ns);
			translator.Translate_FunctionCall(func, yv.expr(), DELTASYMBOLS.NewTemp());
		}
		yyapply;

		((AST::TranslationVisitor *) closure)->Leave();
	}
}

void AST::TranslationVisitor::Handle_Escape (AST_VISITOR_ARGS)
	{ DASSERT(false); }	//handled before translation

void AST::TranslationVisitor::Handle_Inline (AST_VISITOR_ARGS)
	{ DASSERT(false); }	//never found during normal translation 

void AST::TranslationVisitor::Handle_Execute (AST_VISITOR_ARGS)
	{ DASSERT(false); }	//never found during normal translation

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::SetSourceInfo (AST_VISITOR_ARGS) {
	AST::Node* n = (AST::Node*) node;
	PARSEPARMS.SetLine(DPTR(n)->GetStartLine());
	if (const AST::Node::SourceInfoReferences* refs = DPTR(n)->GetSourceReferences())
		COMPMESSENGER.SetSourceReferences(*refs);
	else
		COMPMESSENGER.SetSourceReferences();
}

///////////////////////////////////////////////////////////
