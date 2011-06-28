// ASTCreationActions.h
// Actions for creating the Delta AST.
// ScriptFighter Project.
// A. Savidis, September 2009.
//
 
#ifndef	ASTCREATIONACTIONS_H
#define	ASTCREATIONACTIONS_H

#include "ASTNode.h"
#include "AutoCollection.h"
#include <list>
#include <string>

namespace AST {

/////////////////////////////////////////////////////////

extern util_ui8		GetFuncClass							(const std::string& val);
extern util_ui8		GetFuncClass							(Node* func);
extern util_ui8		GetFuncLinkage							(const std::string& val);
extern util_ui8		GetFuncLinkage							(Node* func);
extern Node*		MakeNode_Program						(NodeList* defs = NIL_NODELIST);
extern NodeList* 	MakeNode_CodeDefs						(NodeList* defs, Node* def);
extern Node* 		MakeNode_Stmts							(Node* stmts, Node* stmt);
extern Node* 		MakeNode_BasicExprStmt					(Node* stmt);
extern Node* 		MakeNode_BasicNonExprStmt				(Node* stmt);
extern Node*		MakeNode_FunctionName					(const char* id);
extern Node* 		MakeNode_Function						(const char* type, Node* name, const char* linkage, IdList* formals, Node* body); 
extern Node*		MakeNode_FunctionStmt					(Node* func);
extern Node*		MakeNode_LambdaFunction					(IdList* formals, Node* expr);
extern IdList* 		MakeNode_IdentList						(IdList* ids, const char* id); 
extern Node* 		MakeNode_Compound						(Node* stmts = NIL_NODE); 
extern Node* 		MakeNode_ExpressionListStmt				(NodeList* exprs); 
extern Node* 		MakeNode_EmptyStmt						(void); 
extern Node* 		MakeNode_AssertStmt						(Node* expr);
extern Node* 		MakeNode_Return							(Node* expr = NIL_NODE);
extern Node* 		MakeNode_UsingNamespace					(IdList* path, const char* id); 
extern Node* 		MakeNode_UsingNamespace					(const char* id); 
extern Node* 		MakeNode_UsingByteCodeLibrary			(const char* id); 
extern char*		MakeNode_StringWithLateDestruction		(char* s);
extern Node* 		MakeNode_ConstDefExpression				(const char* id, Node* expr);
extern Node* 		MakeNode_AssignExpr						(Node* lvalue, Node* rvalue);
extern Node* 		MakeNode_AssignArithExpr				(Node* lvalue, Node* rvalue, const char* op);
extern Node* 		MakeNode_RelationalExpr					(Node* left, Node* right, const char* op);
extern Node* 		MakeNode_ExprANDExpr					(Node* left, Node* right); 
extern Node* 		MakeNode_ExprORExpr						(Node* left, Node* right); 
extern Node* 		MakeNode_NOTExpression					(Node* expr); 
extern Node* 		MakeNode_Ternary						(Node* cond, Node* left, Node* right);
extern Node* 		MakeNode_ArithmeticExpression			(Node* left, Node* right, const char* op);
extern Node* 		MakeNode_UMINUSExpression				(Node* expr); 
extern Node* 		MakeNode_TermLvalueArith				(Node* lvalue, const char* op); 
extern Node*		MakeNode_PrimaryFunctionAndTableObject	(Node* expr);
extern Node*		MakeNode_PrimaryLambdaFunction			(Node* expr);
extern Node*		MakeNode_ParenthesisedExpr				(Node* expr);
extern Node*		MakeNode_PrimaryConstValue				(Node* expr);
extern Node* 		MakeNode_ConstValue						(double num);
extern Node* 		MakeNode_ConstValue						(void);		// Nil
extern Node* 		MakeNode_ConstValue						(bool val);
extern Node* 		MakeNode_StringConst					(char* str); 
extern Node* 		MakeNode_StringConst					(Node* left, char* right); 
extern Node* 		MakeNode_StringifyDottedIdents			(const char* id); 
extern Node* 		MakeNode_StringifyDottedIdents			(Node* prefix, const char* id); 
extern Node* 		MakeNode_StringifyNamespaceIdent		(IdList* ns, const char* id); 
extern Node* 		MakeNode_SELF							(void);
extern Node* 		MakeNode_ARGUMENTS						(void);  
extern Node* 		MakeNode_FunctionParenthesisForm		(Node* func); 
extern NodeList*	MakeNode_ExpressionList					(NodeList* left, Node* expr); 
extern Node* 		MakeNode_Argument						(Node* expr);
extern Node* 		MakeNode_LateBoundArgument				(Node* expr);
extern Node* 		MakeNode_TRIPLE_DOT						(void);
extern Node* 		MakeNode_FunctionCall					(Node* func, NodeList* args);
extern IdList* 		MakeNode_NamespacePath					(IdList* ns, const char* id);
extern Node* 		MakeNode_Lvalue							(const char* id);
extern Node* 		MakeNode_NamespaceLvalue				(IdList* ns, const char* id); 
extern Node* 		MakeNode_AttrLvalue						(const char* id); 
extern Node* 		MakeNode_StaticLvalue					(const char* id); 
extern Node* 		MakeNode_LocalLvalue					(const char* id); 
extern Node* 		MakeNode_LAMBDA_REF						(void); 
extern Node* 		MakeNode_NEWSELF						(void); 
extern Node*		MakeNode_DotIndexIdent					(const char* id);
extern Node*		MakeNode_DotIndexOpString				(const char* op);
extern Node*		MakeNode_DotIndexStringConst			(Node* stringConst);
extern Node*		MakeNode_BracketIndexOperatorMethod		(const char* op);
extern Node* 		MakeNode_TableContentDot				(Node* table, Node* index);
extern Node* 		MakeNode_BoundedTableContentDot			(Node* table, Node* index); 
extern Node* 		MakeNode_TableContentBracket			(Node* table, Node* index); 
extern Node* 		MakeNode_BoundedTableContentBracket		(Node* table, Node* index); 
extern Node* 		MakeNode_TableObject					(Node* table); 
extern Node* 		MakeNode_FunctionExpression				(Node* func); 
extern Node* 		MakeNode_TableConstructor				(NodeList* elems = (NodeList*) 0); 
extern NodeList* 	MakeNode_TableElements					(NodeList* elems, Node* elem);
extern Node*		MakeNode_IdentIndexElement				(const char* id, Node* expr);
extern Node*		MakeNode_UnindexedValue					(Node* elem);
extern Node* 		MakeNode_DottedIdent					(const char* id);
extern Node* 		MakeNode_OperatorIndex					(const char* op); 
extern Node* 		MakeNode_IndexedValues					(NodeList* indices, NodeList* values);
extern Node* 		MakeNode_NewAttribute					(char* id, Node* set, Node* get);
extern Node* 		MakeNode_BREAK							(void);
extern Node* 		MakeNode_CONTINUE						(void); 
extern Node* 		MakeNode_IfStmt							(Node* cond, Node* stmt); 
extern Node* 		MakeNode_IfElseStmt						(Node* cond, Node* ifStmt, Node* elseStmt);
extern Node* 		MakeNode_WhileStmt						(Node* cond, Node* stmt);
extern Node* 		MakeNode_ForStmt						(Node* init, Node* cond, Node* suffix, Node* stmt);
extern Node*		MakeNode_ForInit						(NodeList* exprs);
extern Node*		MakeNode_ForSuffix						(NodeList* exprs);
extern Node* 		MakeNode_ForeachStmt					(Node* value, Node* index, Node* container, Node* stmt);
extern Node* 		MakeNode_THROW							(Node* expr);
extern Node*		MakeNode_ExceptionVar					(const char* id);
extern Node* 		MakeNode_Exception						(Node* tryStmt, Node* var, Node* trapStmt);

/////////////////////////////////////////////////////////

extern void			Initialise (void);
extern void			CleanUp (void);
extern void			SetSyntaxTree (Node* ast);
extern Node*		GetSyntaxTree (void);
extern void			SingletonCreate (void);
extern void			SingletonDestroy (void);

}	// AST namespace

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

