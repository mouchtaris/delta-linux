// ASTCreationActions.cpp
// Actions for creating the Delta AST.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#include "DDebug.h"
#include "ASTCreationActions.h"
#include "ASTCreationHelpers.h"
#include "Symbol.h"
#include "ASTTags.h"
#include "ASTMetaTags.h"
#include "ustrings.h"

/////////////////////////////////////////////////////////////

#define RETAIN_IF_METAGENERATED(n)	\
	if (IsMetaGeneratedCode(n)) return n;

static bool IsMetaGeneratedCode(AST::Node* node) {
	const std::string& tag = DPTR(node)->GetTag();
	return tag == AST_TAG_ESCAPE || tag == AST_TAG_INLINE;
}

/////////////////////////////////////////////////////////////

util_ui8 AST::GetFuncClass (const std::string& val) {
	if (val == AST_VALUE_FUNCCLASS_PROGRAMFUNCTION) return DELTA_FUNCCLASS_PROGRAMFUNCTION;
	if (val == AST_VALUE_FUNCCLASS_ONEVENT)			return DELTA_FUNCCLASS_ONEVENT;			
	if (val == AST_VALUE_FUNCCLASS_METHOD)			return DELTA_FUNCCLASS_METHOD;
	return DELTA_FUNCCLASS_NOTAFUNCTION;
}

util_ui8 AST::GetFuncClass (AST::Node* func)
	{ return GetFuncClass(DPTR(func)->GetAttribute(AST_ATTRIBUTE_CLASS)->GetString()); }

util_ui8 AST::GetFuncLinkage (const std::string& val) {
	if (val == AST_VALUE_FUNCLINKAGE_NO_EXPORT)			return DELTA_FUNCTION_NO_EXPORT;
	if (val == AST_VALUE_FUNCLINKAGE_DEFAULT_EXPORT)	return DELTA_FUNCTION_DEFAULT_EXPORT;			
	return DELTA_FUNCCLASS_NOTAFUNCTION;
}

util_ui8 AST::GetFuncLinkage (AST::Node* func) {
	if (TreeAttribute* linkage = DPTR(func)->GetAttribute(AST_ATTRIBUTE_LINKAGE))
		return GetFuncLinkage(linkage->GetString());
	else
		return DELTA_FUNCTION_DEFAULT_EXPORT;
}
/////////////////////////////////////////////////////////////

AST::Creator::Creator (ucomponentdirectory* directory) : 
	ucomponentdirectoryclient(directory), ast((AST::Node*) 0) {}

AST::Creator::~Creator () {}

void AST::Creator::SetSyntaxTree (AST::Node* _ast)
	{ ast = _ast; }

AST::Node* AST::Creator::GetSyntaxTree (void)
	{ return DNULLCHECK(ast); }

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_Program (AST::Node* stmts)
	{ MAKE_WITH_ONE_OPTIONAL_CHILD(AST_TAG_PROGRAM, AST_CHILD_STMTS, stmts); }

AST::Node* AST::Creator::MakeNode_BasicStmt (AST::Node* stmt)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_BASIC_STMT, AST_CHILD_STMT, stmt); }

AST::Node* AST::Creator::MakeNode_Stmts (AST::Node* stmts, AST::Node* stmt) {
	if (!stmts)
		stmts = ASTFACTORY.NewNode(AST_TAG_STMTS);
	DASSERT(stmt);
	AST::Node* stmtNode = ASTFACTORY.NewNode(AST_TAG_STMT);
	DPTR(stmtNode)->PushBack(stmt, AST_CHILD_STMT);
	DPTR(stmtNode)->SetLocation(stmt->GetLocation());

	DPTR(stmts)->PushBack(stmtNode);
	return stmts;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_Name (const char* name, bool rename) {
	AST::Node* node = ASTFACTORY.NewNode(AST_TAG_NAME);
	DASSERT(name);
	DPTR(node)->SetAttribute(AST_ATTRIBUTE_NAME, name);
	if (rename)
		DPTR(node)->SetAttribute(AST_ATTRIBUTE_RENAME, true);
	return node;
}

//***********************

AST::Node* AST::Creator::MakeNode_FunctionStmt (AST::Node* func) 
	{ MAKE_WITH_ONE_CHILD(AST_TAG_FUNCTION_STMT, AST_CHILD_FUNCTION, func); }

//***********************

AST::Node* AST::Creator::MakeNode_FormalArgs (AST::NodeList* formals)
	{ MAKE_FROM_NODELIST(AST_TAG_FORMAL_ARGS, formals); }

//***********************

AST::Node* AST::Creator::MakeNode_Function (const char* type, AST::Node* name, const char* linkage, AST::Node* formals, AST::Node* body) {
	
	AST::Node* func = ASTFACTORY.NewNode(AST_TAG_FUNCTION);
	DPTR(func)->SetAttribute(AST_ATTRIBUTE_CLASS, type);
	DPTR(func)->SetAttribute(AST_ATTRIBUTE_LINKAGE, linkage);

	DASSERT(formals && body);
	if (name)
		DPTR(func)->PushBack(name, AST_CHILD_NAME);
	DPTR(func)->PushBack(formals, AST_CHILD_FORMALS);
	DPTR(func)->PushBack(body, AST_CHILD_BODY);
	return func;
}

//***********************

AST::Node* AST::Creator::MakeNode_Compound (AST::Node* stmts)
	{ MAKE_WITH_ONE_OPTIONAL_CHILD(AST_TAG_COMPOUND, AST_CHILD_STMTS, stmts); }

//***********************

AST::Node* AST::Creator::MakeNode_LambdaFunction (AST::Node* formals, Node* expr) {

	AST::Node* lambda = ASTFACTORY.NewNode(AST_TAG_LAMBDA_FUNCTION);
	DPTR(lambda)->SetAttribute(AST_ATTRIBUTE_CLASS, AST_VALUE_FUNCCLASS_PROGRAMFUNCTION);

	DASSERT(formals && expr);
	DPTR(lambda)->PushBack(formals, AST_CHILD_FORMALS);
	DPTR(lambda)->PushBack(expr, AST_CHILD_EXPR);

	return lambda;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_ExpressionListStmt (AST::NodeList* exprs) 
	{ DASSERT(exprs); MAKE_FROM_NODELIST_EX(AST_TAG_EXPRLIST_STMT, AST_TAG_EXPR, AST_CHILD_EXPR, exprs); }

AST::Node* AST::Creator::MakeNode_EmptyStmt (void) 
	{ return ASTFACTORY.NewNode(AST_TAG_EMPTY_STMT); }

AST::Node* AST::Creator::MakeNode_AssertStmt (AST::Node* expr) 
	{ MAKE_WITH_ONE_CHILD(AST_TAG_ASSERT, AST_CHILD_EXPR, expr); }

AST::Node* AST::Creator::MakeNode_Return (AST::Node* expr) 
	{ MAKE_WITH_ONE_OPTIONAL_CHILD(AST_TAG_RETURN, AST_CHILD_EXPR, expr); }

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_UsingNamespace (AST::NodeList* path, AST::Node* id)
	{ path = MakeNode_List(path, id); MAKE_FROM_NODELIST(AST_TAG_USING_NAMESPACE, path); }

AST::Node* AST::Creator::MakeNode_UsingByteCodeLibrary (AST::Node* id)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_USING_BYTECODE_LIBRARY, AST_CHILD_NAME, id); }

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_ConstDefExpression (AST::Node* id, AST::Node* expr) {
	AST::Node* def = ASTFACTORY.NewNode(AST_TAG_CONSTDEF);
	DASSERT(id && expr);
	DPTR(def)->PushBack(id, AST_CHILD_NAME);
	DPTR(def)->PushBack(expr, AST_CHILD_EXPR);
	return def;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_AssignExpr (AST::Node* lvalue, AST::Node* rvalue) 
	{ MAKE_BINARY_OPERATION(AST_TAG_ASSIGN, AST_CHILD_LVALUE, lvalue, AST_CHILD_RVALUE, rvalue); }

AST::Node* AST::Creator::MakeNode_AssignArithExpr (AST::Node* lvalue, AST::Node* rvalue, const char* op) 
	{ MAKE_BINARY_OPERATION(op, AST_CHILD_LVALUE, lvalue, AST_CHILD_RVALUE, rvalue); }

AST::Node* AST::Creator::MakeNode_RelationalExpr (AST::Node* left, AST::Node* right, const char* op) 
	{ MAKE_BINARY_OPERATION(op, AST_CHILD_LEFT, left, AST_CHILD_RIGHT, right); }

AST::Node* AST::Creator::MakeNode_ExprANDExpr (AST::Node* left, AST::Node* right) 
	{ MAKE_BINARY_OPERATION(AST_TAG_AND, AST_CHILD_LEFT, left, AST_CHILD_RIGHT, right); }

AST::Node* AST::Creator::MakeNode_ExprORExpr (AST::Node* left, AST::Node* right) 
	{ MAKE_BINARY_OPERATION(AST_TAG_OR, AST_CHILD_LEFT, left, AST_CHILD_RIGHT, right); }

AST::Node* AST::Creator::MakeNode_ArithmeticExpression (AST::Node* left, AST::Node* right, const char* op) 
	{ MAKE_BINARY_OPERATION(op, AST_CHILD_LEFT, left, AST_CHILD_RIGHT, right); }

AST::Node* AST::Creator::MakeNode_NOTExpression	(AST::Node* expr)
	{ MAKE_UNARY_OPERATION(AST_TAG_NOT, AST_CHILD_EXPR, expr); }

AST::Node* AST::Creator::MakeNode_UMINUSExpression (AST::Node* expr)
	{ MAKE_UNARY_OPERATION(AST_TAG_UMINUS, AST_CHILD_EXPR, expr); }

AST::Node* AST::Creator::MakeNode_TermLvalueArith (AST::Node* lvalue, const char* op)
	{ MAKE_UNARY_OPERATION(op, AST_CHILD_LVALUE, lvalue); }

AST::Node* AST::Creator::MakeNode_PrimaryExpression (Node* expr) {
	RETAIN_IF_METAGENERATED(expr);
	MAKE_WITH_ONE_CHILD(AST_TAG_PRIMARY_EXPRESSION, AST_CHILD_EXPR, expr);
}

AST::Node* AST::Creator::MakeNode_ParenthesisedExpr (AST::Node* expr)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_PARENTHESISED_EXPR, AST_CHILD_EXPR, expr);  }

AST::Node* AST::Creator::MakeNode_Ternary (AST::Node* cond, AST::Node* left, AST::Node* right) {
	AST::Node* node = ASTFACTORY.NewNode(AST_TAG_TERNARY);
	DASSERT(cond && left && right);
	DPTR(node)->PushBack(cond,	AST_CHILD_COND);
	DPTR(node)->PushBack(left,	AST_CHILD_LEFT);
	DPTR(node)->PushBack(right,	AST_CHILD_RIGHT);
	return node;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_ConstValue (double num) {
	AST::Node* node = ASTFACTORY.NewNode(AST_TAG_NUM_CONST);
	node->SetAttribute(AST_ATTRIBUTE_CONSTVALUE, num);
	return node;
}

AST::Node* AST::Creator::MakeNode_ConstValue (void)
	{ return ASTFACTORY.NewNode(AST_TAG_NIL); }

AST::Node* AST::Creator::MakeNode_ConstValue (bool val){ 
	AST::Node* node = ASTFACTORY.NewNode(AST_TAG_BOOL_CONST);
	node->SetAttribute(AST_ATTRIBUTE_CONSTVALUE, val);
	return node;
}

/////////////////////////////////////////////////////////////

#define	MAKE_STRING_LIST(tag, attr, str)								\
	AST::Node* node = ASTFACTORY.NewNode(tag);							\
	StringList* list = DNEW(StringList);								\
	list->push_back(str);												\
	node->SetAttribute(attr, list);										\
	return node

#define	ADD_STRING_LIST(node, attr, str)								\
	AST::StringList& l = node->GetAttribute(attr)->GetStringList();		\
	l.push_back(str);													\
	return node

AST::Node* AST::Creator::MakeNode_StringConst (const char* str) 
	{ MAKE_STRING_LIST(AST_TAG_STRING_CONST, AST_ATTRIBUTE_ITEMS, str); }

AST::Node* AST::Creator::MakeNode_StringConst (AST::Node* left, char* right) 
	{ ADD_STRING_LIST(left, AST_ATTRIBUTE_ITEMS, right); }

AST::Node* AST::Creator::MakeNode_StringifyDottedIdents (const char* id)
	{ MAKE_STRING_LIST(AST_TAG_STRINGIFY_DOTTED_IDENTS, AST_ATTRIBUTE_ITEMS, id); }

AST::Node* AST::Creator::MakeNode_StringifyDottedIdents (AST::Node* prefix, const char* id)
	{ ADD_STRING_LIST(prefix, AST_ATTRIBUTE_ITEMS, id); }

AST::Node* AST::Creator::MakeNode_StringifyNamespaceIdent (AST::NodeList* path, AST::Node* id)
	{ path = MakeNode_List(path, id); MAKE_FROM_NODELIST(AST_TAG_STRINGIFY_NAMESPACE_IDENT, path); }

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_NamespaceLvalue (AST::NodeList* path, AST::Node* id)
	{ path = MakeNode_List(path, id); MAKE_FROM_NODELIST(AST_TAG_LVALUE_NAMESPACE_IDENT, path); }

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_SELF (void)		{ MAKE_LEAF(AST_TAG_SELF);				}
AST::Node* AST::Creator::MakeNode_ARGUMENTS (void)	{ MAKE_LEAF(AST_TAG_ARGUMENTS);			}
AST::Node* AST::Creator::MakeNode_TRIPLE_DOT (void)	{ MAKE_LEAF(AST_TAG_TRIPLE_DOT);		}
AST::Node* AST::Creator::MakeNode_LAMBDA_REF(void)	{ MAKE_LEAF(AST_TAG_LAMBDA_REF);		}
AST::Node* AST::Creator::MakeNode_NEWSELF (void)	{ MAKE_LEAF(AST_TAG_LVALUE_NEWSELF);	}
AST::Node* AST::Creator::MakeNode_BREAK(void)		{ MAKE_LEAF(AST_TAG_BREAK);				}
AST::Node* AST::Creator::MakeNode_CONTINUE (void)	{ MAKE_LEAF(AST_TAG_CONTINUE);			}

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_EntireExpressionList (AST::NodeList* exprs)
	{ MAKE_FROM_NODELIST_EX(AST_TAG_EXPR_LIST, AST_TAG_EXPRLIST_ITEM, AST_CHILD_EXPR, exprs); }

AST::Node* AST::Creator::MakeNode_FunctionParenthesisForm (AST::Node* func)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_FUNCTION_PARENTHESIS, AST_CHILD_FUNCTION, func);  }

AST::NodeList* AST::Creator::MakeNode_List (AST::NodeList* l, AST::Node* node)
	{ MAKE_NODELIST(l, DNULLCHECK(node)); }

AST::Node* AST::Creator::MakeNode_LateBoundArgument (AST::Node* expr)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_LATEBOUND_ARGUMENT, AST_CHILD_EXPR, expr); } 

AST::Node* AST::Creator::MakeNode_Argument (AST::Node* expr)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_ARGUMENT, AST_CHILD_EXPR, expr); } 

AST::Node* AST::Creator::MakeNode_ActualArguments (AST::NodeList* args)
	{ return MakeNode_EntireExpressionList(args); }

AST::Node* AST::Creator::MakeNode_FunctionCall (AST::Node* func, AST::Node* args) {
	AST::Node* call = ASTFACTORY.NewNode(AST_TAG_FUNCTION_CALL);
	call->PushBack(func, AST_CHILD_FUNCTION);
	call->PushBack(args, AST_CHILD_ACTUALS);
	return call;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_Lvalue (AST::Node* id) {
	RETAIN_IF_METAGENERATED(id);
	MAKE_WITH_ONE_CHILD(AST_TAG_LVALUE_IDENT, AST_CHILD_NAME, id);
}

AST::Node* AST::Creator::MakeNode_AttrLvalue (AST::Node* id)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_LVALUE_ATTRIBUTE, AST_CHILD_NAME, id); }

AST::Node* AST::Creator::MakeNode_StaticLvalue (AST::Node* id)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_LVALUE_STATIC_IDENT, AST_CHILD_NAME, id); }

AST::Node* AST::Creator::MakeNode_LocalLvalue (AST::Node* id)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_LVALUE_LOCAL_IDENT, AST_CHILD_NAME, id); } 

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_DotIndexIdent (const char* id)
	{ MAKE_WITH_ONE_ATTR(AST_TAG_DOTINDEX_IDENT, AST_ATTRIBUTE_VALUE, id); }

AST::Node* AST::Creator::MakeNode_DotIndexOpString (const char* op)
	{ MAKE_WITH_ONE_ATTR(AST_TAG_DOTINDEX_OPSTRING, AST_ATTRIBUTE_VALUE, op); }

AST::Node* AST::Creator::MakeNode_DotIndexStringConst (AST::Node* stringConst)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_DOTINDEX_STRINGCONST, AST_CHILD_EXPR, stringConst); }

AST::Node* AST::Creator::MakeNode_BracketIndexOperatorMethod (const char* op)
	{ MAKE_WITH_ONE_ATTR(AST_TAG_BRACKETINDEX_OPMETHOD, AST_ATTRIBUTE_VALUE, op); }

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_TableContentDot (AST::Node* table, AST::Node* index) 
	{ MAKE_WITH_TWO_CHILDREN(AST_TAG_TABLE_CONTENT_DOT, AST_CHILD_TABLE, table, AST_CHILD_INDEX, index); }

AST::Node* AST::Creator::MakeNode_BoundedTableContentDot (AST::Node* table, AST::Node* index)
	{ MAKE_WITH_TWO_CHILDREN(AST_TAG_TABLE_CONTENT_DOUBLE_DOT, AST_CHILD_TABLE, table, AST_CHILD_INDEX, index); }

AST::Node* AST::Creator::MakeNode_TableContentBracket (AST::Node* table, AST::Node* index)
	{ MAKE_WITH_TWO_CHILDREN(AST_TAG_TABLE_CONTENT_BRACKET, AST_CHILD_TABLE, table, AST_CHILD_INDEX, index); }

AST::Node* AST::Creator::MakeNode_BoundedTableContentBracket	(AST::Node* table, AST::Node* index)
	{ MAKE_WITH_TWO_CHILDREN(AST_TAG_TABLE_CONTENT_DOUBLE_BRACKET, AST_CHILD_TABLE, table, AST_CHILD_INDEX, index); }

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_TableConstructor (AST::NodeList* elems)
	{ MAKE_FROM_NODELIST(AST_TAG_TABLE_CONSTRUCTOR, elems); }

AST::Node* AST::Creator::MakeNode_FunctionExpression (AST::Node* func)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_FUNCTION_EXPR, AST_CHILD_EXPR, func); }

AST::NodeList* AST::Creator::MakeNode_TableElements (AST::NodeList* elems, AST::Node* elem)
	{ MAKE_NODELIST(elems, DNULLCHECK(elem)); }

AST::Node* AST::Creator::MakeNode_IdentIndexElement (AST::Node* id, AST::Node* expr) {
	AST::Node* node = ASTFACTORY.NewNode(AST_TAG_TABLE_IDENTINDEX_ELEM);
	DPTR(node)->PushBack(id, AST_CHILD_NAME);
	DPTR(node)->PushBack(expr, AST_CHILD_EXPR);
	return node;
}

AST::Node* AST::Creator::MakeNode_UnindexedValue (AST::Node* elem) {
	RETAIN_IF_METAGENERATED(elem);
	MAKE_WITH_ONE_CHILD(AST_TAG_TABLE_UNINDEXED_ELEM, AST_CHILD_EXPR, elem);
}

AST::Node* AST::Creator::MakeNode_DottedIdent (const char* id)
	{ MAKE_WITH_ONE_ATTR(AST_TAG_TABLE_DOTTED_IDENT, AST_ATTRIBUTE_NAME, id); }

AST::Node* AST::Creator::MakeNode_OperatorIndex (const char* op)
	{ MAKE_WITH_ONE_ATTR(AST_TAG_TABLE_OPERATOR_INDEX, AST_ATTRIBUTE_VALUE, op); }

AST::Node* AST::Creator::MakeNode_TableIndices (AST::NodeList* indices) {
	DASSERT(indices);
	MAKE_FROM_NODELIST_EX(AST_TAG_TABLE_INDICES, AST_TAG_EXPRLIST_ITEM, AST_CHILD_EXPR, indices);
}

AST::Node* AST::Creator::MakeNode_TableValues (AST::NodeList* values) {
	DASSERT(values);
	MAKE_FROM_NODELIST_EX(AST_TAG_TABLE_VALUES, AST_TAG_EXPRLIST_ITEM, AST_CHILD_EXPR, values);
}

AST::Node* AST::Creator::MakeNode_IndexedValues (AST::Node* indices, AST::Node* values) {
	MAKE_WITH_TWO_CHILDREN(
		AST_TAG_TABLE_INDEXED_VALUES, 
		AST_CHILD_INDICES,	indices,	// 1st child
		AST_CHILD_VALUES,	values		// 2nd child
	);
}

AST::Node* AST::Creator::MakeNode_NewAttribute (AST::Node* id, AST::Node* set, AST::Node* get) {
	AST::Node* attr = ASTFACTORY.NewNode(AST_TAG_TABLE_NEW_ATTRIBUTE);
	attr->PushBack(id, AST_CHILD_NAME);
	attr->PushBack(set, AST_CHILD_SET);
	attr->PushBack(get, AST_CHILD_GET);
	return attr;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_IfStmt (AST::Node* cond, AST::Node* stmt) {
	MAKE_WITH_TWO_CHILDREN(AST_TAG_IF, AST_CHILD_COND, cond, AST_CHILD_STMT, stmt);
}

AST::Node* AST::Creator::MakeNode_IfElseStmt (AST::Node* cond, AST::Node* ifStmt, AST::Node* elseStmt) {
	AST::Node* stmt = ASTFACTORY.NewNode(AST_TAG_IF_ELSE);
	stmt->PushBack(cond,		AST_CHILD_COND);
	stmt->PushBack(ifStmt,	AST_CHILD_IF);
	stmt->PushBack(elseStmt,	AST_CHILD_ELSE);
	return stmt;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_WhileStmt (AST::Node* cond, AST::Node* stmt) 
	{ MAKE_WITH_TWO_CHILDREN(AST_TAG_WHILE, AST_CHILD_COND, cond, AST_CHILD_STMT, stmt); }

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_ForSuffix (AST::NodeList* exprs) 
	{ return MakeNode_EntireExpressionList(exprs); }

AST::Node* AST::Creator::MakeNode_ForInit (AST::NodeList* exprs)
	{ return MakeNode_EntireExpressionList(exprs); }

AST::Node* AST::Creator::MakeNode_ForStmt (AST::Node* init, AST::Node* cond, AST::Node* suffix, AST::Node* stmt){

	AST::Node* forStmt = ASTFACTORY.NewNode(AST_TAG_FOR);
	forStmt->PushBack(init,		AST_CHILD_INIT);	
	forStmt->PushBack(cond,		AST_CHILD_COND);
	forStmt->PushBack(suffix,	AST_CHILD_SUFFIX);	
	forStmt->PushBack(stmt,		AST_CHILD_STMT);

	return forStmt;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_ForeachStmt (AST::Node* value, AST::Node* index, AST::Node* container, AST::Node* stmt) {
	AST::Node* foreachStmt = ASTFACTORY.NewNode(AST_TAG_FOREACH);
	if (index)
		foreachStmt->PushBack(index, AST_CHILD_INDEX);
	foreachStmt->PushBack(value,		AST_CHILD_LVALUE);		
	foreachStmt->PushBack(container,	AST_CHILD_CONTAINER);
	foreachStmt->PushBack(stmt,		AST_CHILD_STMT);
	return foreachStmt;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_THROW (AST::Node* expr)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_THROW, AST_CHILD_EXPR, expr); }

AST::Node* AST::Creator::MakeNode_ExceptionVar (AST::Node* id)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_EXCEPTION_VAR, AST_CHILD_NAME, id); }

AST::Node* AST::Creator::MakeNode_Exception (AST::Node* tryStmt, AST::Node* var, AST::Node* trapStmt) {
	AST::Node* handlerStmt = ASTFACTORY.NewNode(AST_TAG_TRYTRAP);
	handlerStmt->PushBack(tryStmt,	AST_CHILD_TRY);
	handlerStmt->PushBack(var,		AST_CHILD_VAR);
	handlerStmt->PushBack(trapStmt,	AST_CHILD_TRAP);
	return handlerStmt;
}

/////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_FromList (const char* tag, AST::NodeList* elems)
	{ MAKE_FROM_NODELIST(tag, elems); }

/////////////////////////////////////////////////////////

AST::Node* AST::Creator::MakeNode_ChildExpr	(const char* tag, AST::Node* value) 
	{ MAKE_WITH_ONE_CHILD(tag, AST_CHILD_EXPR, value); }

/////////////////////////////////////////////////////////