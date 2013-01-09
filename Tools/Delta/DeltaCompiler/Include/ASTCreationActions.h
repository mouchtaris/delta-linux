// ASTCreationActions.h
// Actions for creating the Delta AST.
// ScriptFighter Project.
// A. Savidis, September 2009.
//
 
#ifndef	ASTCREATIONACTIONS_H
#define	ASTCREATIONACTIONS_H

#include "ASTNode.h"
#include "AutoCollection.h"
#include "CompilerComponentDirectory.h"
#include <list>
#include <string>

namespace AST {

/////////////////////////////////////////////////////////

extern util_ui8	GetFuncClass	(const std::string& val);
extern util_ui8	GetFuncClass	(Node* func);
extern util_ui8	GetFuncLinkage	(const std::string& val);
extern util_ui8	GetFuncLinkage	(Node* func);

/////////////////////////////////////////////////////////

class Creator : public ucomponentdirectoryclient {
	
	private:
	Node* ast;

	Node*		MakeNode_EntireExpressionList			(AST::NodeList* exprs);

	public:
	Node*		MakeNode_Program						(Node* defs);
	Node*		MakeNode_BasicStmt						(Node* stmt);
	Node* 		MakeNode_Stmts							(Node* stmts, Node* stmt);
	Node*		MakeNode_Name							(const char* name, bool rename = false);
	Node*		MakeNode_FormalArgs						(NodeList* formals);
	Node* 		MakeNode_Function						(const char* type, Node* name, const char* linkage, Node* formals, Node* body); 
	Node*		MakeNode_FunctionStmt					(Node* func);
	Node*		MakeNode_LambdaFunction					(Node* formals, Node* expr);
	Node* 		MakeNode_Compound						(Node* stmts = NIL_NODE);
	Node* 		MakeNode_ExpressionListStmt				(NodeList* exprs); 
	Node* 		MakeNode_EmptyStmt						(void); 
	Node* 		MakeNode_AssertStmt						(Node* expr);
	Node* 		MakeNode_Return							(Node* expr = NIL_NODE);
	Node* 		MakeNode_UsingNamespace					(NodeList* path, Node* id); 
	Node* 		MakeNode_UsingByteCodeLibrary			(Node* id); 
	Node* 		MakeNode_ConstDefExpression				(Node* id, Node* expr);
	Node* 		MakeNode_AssignExpr						(Node* lvalue, Node* rvalue);
	Node* 		MakeNode_AssignArithExpr				(Node* lvalue, Node* rvalue, const char* op);
	Node* 		MakeNode_RelationalExpr					(Node* left, Node* right, const char* op);
	Node* 		MakeNode_ExprANDExpr					(Node* left, Node* right); 
	Node* 		MakeNode_ExprORExpr						(Node* left, Node* right); 
	Node* 		MakeNode_NOTExpression					(Node* expr); 
	Node* 		MakeNode_Ternary						(Node* cond, Node* left, Node* right);
	Node* 		MakeNode_ArithmeticExpression			(Node* left, Node* right, const char* op);
	Node* 		MakeNode_UMINUSExpression				(Node* expr); 
	Node* 		MakeNode_TermLvalueArith				(Node* lvalue, const char* op);
	Node*		MakeNode_PrimaryExpression				(Node* expr);
	Node*		MakeNode_ParenthesisedExpr				(Node* expr);
	Node* 		MakeNode_ConstValue						(double num);
	Node* 		MakeNode_ConstValue						(void);		// Nil
	Node* 		MakeNode_ConstValue						(bool val);
	Node* 		MakeNode_StringConst					(const char* str); 
	Node* 		MakeNode_StringConst					(Node* left, char* right); 
	Node* 		MakeNode_StringifyDottedIdents			(const char* id); 
	Node* 		MakeNode_StringifyDottedIdents			(Node* prefix, const char* id); 
	Node* 		MakeNode_StringifyNamespaceIdent		(NodeList* path, Node* id); 
	Node* 		MakeNode_SELF							(void);
	Node* 		MakeNode_ARGUMENTS						(void);  
	Node* 		MakeNode_FunctionParenthesisForm		(Node* func); 
	NodeList*	MakeNode_List							(NodeList* l, Node* node); 
	Node* 		MakeNode_Argument						(Node* expr);
	Node* 		MakeNode_LateBoundArgument				(Node* expr);
	Node* 		MakeNode_TRIPLE_DOT						(void);
	Node* 		MakeNode_ActualArguments				(NodeList* args);
	Node* 		MakeNode_FunctionCall					(Node* func, Node* args);
	Node* 		MakeNode_Lvalue							(Node* id);
	Node* 		MakeNode_NamespaceLvalue				(NodeList* path, Node* id); 
	Node* 		MakeNode_AttrLvalue						(Node* id); 
	Node* 		MakeNode_StaticLvalue					(Node* id); 
	Node* 		MakeNode_LocalLvalue					(Node* id); 
	Node* 		MakeNode_LAMBDA_REF						(void); 
	Node* 		MakeNode_NEWSELF						(void); 
	Node*		MakeNode_DotIndexIdent					(const char* id);
	Node*		MakeNode_DotIndexOpString				(const char* op);
	Node*		MakeNode_DotIndexStringConst			(Node* stringConst);
	Node*		MakeNode_BracketIndexOperatorMethod		(const char* op);
	Node* 		MakeNode_TableContentDot				(Node* table, Node* index);
	Node* 		MakeNode_BoundedTableContentDot			(Node* table, Node* index); 
	Node* 		MakeNode_TableContentBracket			(Node* table, Node* index); 
	Node* 		MakeNode_BoundedTableContentBracket		(Node* table, Node* index); 
	Node* 		MakeNode_FunctionExpression				(Node* func); 
	Node* 		MakeNode_TableConstructor				(NodeList* elems = (NodeList*) 0); 
	NodeList* 	MakeNode_TableElements					(NodeList* elems, Node* elem);
	Node*		MakeNode_IdentIndexElement				(Node* id, Node* expr);
	Node*		MakeNode_UnindexedValue					(Node* elem);
	Node* 		MakeNode_DottedIdent					(const char* id);
	Node* 		MakeNode_OperatorIndex					(const char* op);
	Node* 		MakeNode_TableIndices					(NodeList* indices);
	Node* 		MakeNode_TableValues					(NodeList* values);
	Node* 		MakeNode_IndexedValues					(Node* indices, Node* values);
	Node* 		MakeNode_NewAttribute					(Node* id, Node* set, Node* get);
	Node* 		MakeNode_BREAK							(void);
	Node* 		MakeNode_CONTINUE						(void); 
	Node* 		MakeNode_IfStmt							(Node* cond, Node* stmt); 
	Node* 		MakeNode_IfElseStmt						(Node* cond, Node* ifStmt, Node* elseStmt);
	Node* 		MakeNode_WhileStmt						(Node* cond, Node* stmt);
	Node* 		MakeNode_ForStmt						(Node* init, Node* cond, Node* suffix, Node* stmt);
	Node*		MakeNode_ForInit						(NodeList* exprs);
	Node*		MakeNode_ForSuffix						(NodeList* exprs);
	Node* 		MakeNode_ForeachStmt					(Node* value, Node* index, Node* container, Node* stmt);
	Node* 		MakeNode_THROW							(Node* expr);
	Node*		MakeNode_ExceptionVar					(Node* id);
	Node* 		MakeNode_Exception						(Node* tryStmt, Node* var, Node* trapStmt);

	Node*		MakeNode_FromList						(const char* tag, NodeList* value);
	Node*		MakeNode_ChildExpr						(const char* tag, Node* value);

	/////////////////////////////////////////////////////////

	void		SetSyntaxTree (Node* ast);
	Node*		GetSyntaxTree (void);

	Creator (ucomponentdirectory* directory);
	~Creator ();

};

}	// AST namespace

/////////////////////////////////////////////////////////

#define ASTCREATOR_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), AST::Creator)))

#define ASTCREATOR	ASTCREATOR_EX(GET_COMPONENT_DIRECTORY())

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

