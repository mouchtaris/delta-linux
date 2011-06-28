// ASTCreationActions.cpp
// Actions for creating the Delta AST.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#include "DDebug.h"
#include "ASTCreationActions.h"
#include "Symbol.h"
#include "ASTTags.h"
#include "ustrings.h"

extern char* Translate_StringWithLateDestruction (char* s);

/////////////////////////////////////////////////////////////

#define	MAKE_WITH_STRING_ATTR(tag, attr, id)				\
	AST::Node* node = DNEWCLASS(AST::Node, (tag));			\
	DASSERT(id);											\
	DPTR(node)->SetAttribute(								\
		attr,												\
		Translate_StringWithLateDestruction(ucopystr(id))	\
	);														\
	return node

#define	MAKE_FROM_NODELIST(tag, nodes)						\
	AST::Node* node = DNEWCLASS(AST::Node, (tag));			\
	if (nodes)												\
		DPTR(node)->AddChildren(nodes);						\
	return node

#define	MAKE_FROM_NODELIST_EX(tag, chTag, chId, nodes)		\
	AST::Node* node = DNEWCLASS(AST::Node, (tag));			\
	if (nodes)												\
		DPTR(node)->AddChildrenEx(nodes, chTag, chId);		\
	return node

#define	MAKE_WITH_ONE_CHILD(tag, name, child)				\
	AST::Node* node = DNEWCLASS(AST::Node, (tag));			\
	DASSERT(child);											\
	DPTR(node)->PushBack(child, name);						\
	return node

#define	MAKE_WITH_TWO_CHILDREN(tag, n1, c1, n2, c2)			\
	AST::Node* node = DNEWCLASS(AST::Node, (tag));			\
	DASSERT(c1 && c2);										\
	DPTR(node)->PushBack(c1, n1);							\
	DPTR(node)->PushBack(c2, n2);							\
	return node

#define	MAKE_WITH_ONE_OPTIONAL_CHILD(tag, name, child)		\
	AST::Node* node = DNEWCLASS(AST::Node, (tag));			\
	if (child)												\
		DPTR(node)->PushBack(child, name);					\
	return node

#define	MAKE_WITH_ONE_ATTR(tag, attr, val)					\
	AST::Node* node = DNEWCLASS(AST::Node, (tag));			\
	DASSERT(val);											\
	DPTR(node)->SetAttribute(attr, val);					\
	return node

#define	EXTEND_ARG(arg, tag, childId)						\
	arg = AST::Node::Extend(arg, tag, childId);

#define	MAKE_BINARY_OPERATION(op,lname,left,rname,right)	\
	AST::Node* node = DNEWCLASS(AST::Node, (op));			\
	DASSERT(left && right);									\
	DPTR(node)->PushBack(left, lname);						\
	DPTR(node)->PushBack(right, rname);						\
	return node

#define	MAKE_UNARY_OPERATION(op, name, expr)				\
	AST::Node* node = DNEWCLASS(AST::Node, (op));			\
	DASSERT(expr);											\
	DPTR(node)->PushBack(expr, name);						\
	return node

#define	MAKE_ITEMLIST(t, l, i)								\
	DASSERT(i);												\
	if (!l)													\
		l = t::New(i);										\
	else													\
	if (i)													\
		l->Add(i);											\
	return l

#define	MAKE_LEAF(tag)										\
	return DNEWCLASS(AST::Node, (tag))

/////////////////////////////////////////////////////////////

static AST::Node* ast = (AST::Node*) 0;

void AST::Initialise (void)			
	{ unullify(ast); }
void AST::CleanUp (void)			
	{}
void AST::SingletonCreate (void)	
	{}
void AST::SingletonDestroy (void)	
	{}
void AST::SetSyntaxTree (AST::Node* _ast)
	{ DASSERT(!ast); ast = _ast; }

AST::Node* AST::GetSyntaxTree (void)
	{ return DNULLCHECK(ast); }

/////////////////////////////////////////////////////////////

util_ui8 AST::GetFuncClass (const std::string& val) {
	if (val == AST_VALUE_FUNCCLASS_PROGRAMFUNCTION) return DELTA_FUNCCLASS_PROGRAMFUNCTION;
	if (val == AST_VALUE_FUNCCLASS_ONEVENT)			return DELTA_FUNCCLASS_ONEVENT;			
	if (val == AST_VALUE_FUNCCLASS_METHOD)			return DELTA_FUNCCLASS_METHOD;
	return DELTA_FUNCCLASS_NOTAFUNCTION;
}

util_ui8 AST::GetFuncClass (AST::Node* func)
	{ return AST::GetFuncClass((const char*) DPTR(func)->GetAttribute(AST_ATTRIBUTE_CLASS)); }

util_ui8 AST::GetFuncLinkage (const std::string& val) {
	if (val == AST_VALUE_FUNCLINKAGE_NO_EXPORT)			return DELTA_FUNCTION_NO_EXPORT;
	if (val == AST_VALUE_FUNCLINKAGE_DEFAULT_EXPORT)	return DELTA_FUNCTION_DEFAULT_EXPORT;			
	return DELTA_FUNCCLASS_NOTAFUNCTION;
}

util_ui8 AST::GetFuncLinkage (AST::Node* func) {
	if (const char* linkage =  (const char*) DPTR(func)->GetAttribute(AST_ATTRIBUTE_LINKAGE))
		return AST::GetFuncLinkage(linkage);
	else
		return DELTA_FUNCTION_DEFAULT_EXPORT;
}

/////////////////////////////////////////////////////////////

char* AST::MakeNode_StringWithLateDestruction (char* s) 
	{ return Translate_StringWithLateDestruction(s); }

AST::Node* AST::MakeNode_Program (AST::NodeList* defs) 
	{ MAKE_FROM_NODELIST(AST_TAG_PROGRAM, defs); }

AST::NodeList* AST::MakeNode_CodeDefs (AST::NodeList* defs, AST::Node* def) 
	{ MAKE_ITEMLIST(AST::NodeList, defs, def); }

AST::Node* AST::MakeNode_BasicExprStmt (AST::Node* stmt)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_BASIC_EXPR_STMT, AST_CHILD_STMT, stmt); }

AST::Node* AST::MakeNode_BasicNonExprStmt (AST::Node* stmt)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_BASIC_NON_EXPR_STMT, AST_CHILD_STMT, stmt); }

AST::Node* AST::MakeNode_Stmts (AST::Node* stmts, AST::Node* stmt) {
	if (!stmts)
		stmts = DNEWCLASS(AST::Node, (AST_TAG_STMTS));
	DASSERT(stmt);
	AST::Node* stmtNode = DNEWCLASS(AST::Node, (AST_TAG_STMT));
	DPTR(stmtNode)->PushBack(stmt, AST_CHILD_STMT);

	DPTR(stmts)->PushBack(stmtNode);
	return stmts;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::MakeNode_FunctionName (const char* name) 
	{ MAKE_WITH_STRING_ATTR(AST_TAG_FUNCTION, AST_ATTRIBUTE_NAME, name); }

//***********************

AST::Node* AST::MakeNode_FunctionStmt (AST::Node* func) 
	{ MAKE_WITH_ONE_CHILD(AST_TAG_FUNCTION_STMT, AST_CHILD_FUNCTION, func); }

//***********************

AST::Node* AST::MakeNode_Function (const char* type, AST::Node* func, const char* linkage, AST::IdList* formals, AST::Node* body) {
	
	if (!func)	// Anonymous function.
		func = DNEWCLASS(AST::Node, (AST_TAG_FUNCTION));
	DPTR(func)->SetAttribute(AST_ATTRIBUTE_CLASS, type);
	DPTR(func)->SetAttribute(AST_ATTRIBUTE_LINKAGE, linkage);

	AST::Node* node = DNEWCLASS(AST::Node, (AST_TAG_FORMAL_ARGS));
	if (formals)
		DPTR(node)->SetAttribute(AST_ATTRIBUTE_ITEMS, formals);
	DPTR(func)->PushBack(node, AST_CHILD_FORMALS);

	DASSERT(body);
	DPTR(func)->PushBack(body, AST_CHILD_BODY);
	return func;
}

//***********************

AST::Node* AST::MakeNode_Compound (AST::Node* stmts)
	{ MAKE_WITH_ONE_OPTIONAL_CHILD(AST_TAG_COMPOUND, AST_CHILD_STMTS, stmts); }

//***********************

AST::IdList* AST::MakeNode_IdentList (AST::IdList* ids, const char* id) 
	{ if (!id) return ids; else { MAKE_ITEMLIST(AST::IdList, ids, DNULLCHECK(id)); } }

//***********************

AST::Node* AST::MakeNode_LambdaFunction (IdList* formals, Node* expr) {

	AST::Node* lambda	= DNEWCLASS(AST::Node, (AST_TAG_LAMBDA_FUNCTION));
	DPTR(lambda)->SetAttribute(AST_ATTRIBUTE_CLASS, AST_VALUE_FUNCCLASS_PROGRAMFUNCTION);

	AST::Node* node		= DNEWCLASS(AST::Node, (AST_TAG_FORMAL_ARGS));
	if (formals)
		DPTR(node)->SetAttribute(AST_ATTRIBUTE_ITEMS, formals);
	DPTR(lambda)->PushBack(node, AST_CHILD_FORMALS);

	DASSERT(expr);
	DPTR(lambda)->PushBack(expr, AST_CHILD_EXPR);

	return lambda;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::MakeNode_ExpressionListStmt (AST::NodeList* exprs) 
	{ DASSERT(exprs); MAKE_FROM_NODELIST_EX(AST_TAG_EXPRLIST_STMT, AST_TAG_EXPR, AST_CHILD_EXPR, exprs); }

AST::Node* AST::MakeNode_EmptyStmt (void) 
	{ return DNEWCLASS(AST::Node, (AST_TAG_EMPTY_STMT)); }

AST::Node* AST::MakeNode_AssertStmt (AST::Node* expr) 
	{ MAKE_WITH_ONE_CHILD(AST_TAG_ASSERT, AST_CHILD_EXPR, expr); }

AST::Node* AST::MakeNode_Return (AST::Node* expr) 
	{ MAKE_WITH_ONE_OPTIONAL_CHILD(AST_TAG_RETURN, AST_CHILD_EXPR, expr); }

/////////////////////////////////////////////////////////////

AST::Node* AST::MakeNode_UsingNamespace	(AST::IdList* path, const char* id)
	{ path->Add(id); MAKE_WITH_ONE_ATTR(AST_TAG_USING_NAMESPACE, AST_ATTRIBUTE_NAMESPACE, path); }

AST::Node* AST::MakeNode_UsingNamespace	(const char* id)
	{ MAKE_WITH_STRING_ATTR(AST_TAG_USING_NAMESPACE, AST_ATTRIBUTE_NAME, id); }

AST::Node* AST::MakeNode_UsingByteCodeLibrary (const char* id) {
	DASSERT(id);
	AST::Node* def = DNEWCLASS(AST::Node, (AST_TAG_USING_BYTECODE_LIBRARY));
	DPTR(def)->SetAttribute(AST_ATTRIBUTE_NAME, Translate_StringWithLateDestruction(ucopystr(id)));
	return def;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::MakeNode_ConstDefExpression (const char* id, AST::Node* expr) {
	AST::Node* def = DNEWCLASS(AST::Node, (AST_TAG_CONSTDEF));
	DASSERT(id && expr);
	DPTR(def)->SetAttribute(AST_ATTRIBUTE_NAME, id);
	DPTR(def)->PushBack(expr, AST_CHILD_EXPR);
	return def;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::MakeNode_AssignExpr (AST::Node* lvalue, AST::Node* rvalue) 
	{ MAKE_BINARY_OPERATION(AST_TAG_ASSIGN, AST_CHILD_LVALUE, lvalue, AST_CHILD_RVALUE, rvalue); }

AST::Node* AST::MakeNode_AssignArithExpr (AST::Node* lvalue, AST::Node* rvalue, const char* op) 
	{ MAKE_BINARY_OPERATION(op, AST_CHILD_LVALUE, lvalue, AST_CHILD_RVALUE, rvalue); }

AST::Node* AST::MakeNode_RelationalExpr (AST::Node* left, AST::Node* right, const char* op) 
	{ MAKE_BINARY_OPERATION(op, AST_CHILD_LEFT, left, AST_CHILD_RIGHT, right); }

AST::Node* AST::MakeNode_ExprANDExpr (AST::Node* left, AST::Node* right) 
	{ MAKE_BINARY_OPERATION(AST_TAG_AND, AST_CHILD_LEFT, left, AST_CHILD_RIGHT, right); }

AST::Node* AST::MakeNode_ExprORExpr (AST::Node* left, AST::Node* right) 
	{ MAKE_BINARY_OPERATION(AST_TAG_OR, AST_CHILD_LEFT, left, AST_CHILD_RIGHT, right); }

AST::Node* AST::MakeNode_ArithmeticExpression (AST::Node* left, AST::Node* right, const char* op) 
	{ MAKE_BINARY_OPERATION(op, AST_CHILD_LEFT, left, AST_CHILD_RIGHT, right); }

AST::Node* AST::MakeNode_NOTExpression	(AST::Node* expr)
	{ MAKE_UNARY_OPERATION(AST_TAG_NOT, AST_CHILD_EXPR, expr); }

AST::Node* AST::MakeNode_UMINUSExpression (AST::Node* expr)
	{ MAKE_UNARY_OPERATION(AST_TAG_UMINUS, AST_CHILD_EXPR, expr); }

AST::Node* AST::MakeNode_TermLvalueArith (AST::Node* lvalue, const char* op)
	{ MAKE_UNARY_OPERATION(op, AST_CHILD_LVALUE, lvalue); }

AST::Node* AST::MakeNode_PrimaryFunctionAndTableObject (AST::Node* expr)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_PRIMARY_FUNCTION_TABLE_OBJECT, AST_CHILD_EXPR, expr);  }

AST::Node* AST::MakeNode_PrimaryLambdaFunction (Node* expr)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_PRIMARY_LAMBDA_DEFINITION, AST_CHILD_EXPR, expr);  }

AST::Node* AST::MakeNode_ParenthesisedExpr (AST::Node* expr)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_PARENTHESISED_EXPR, AST_CHILD_EXPR, expr);  }

AST::Node* AST::MakeNode_PrimaryConstValue (AST::Node* expr)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_PRIMARY_CONST_VALUE, AST_CHILD_EXPR, expr);  }

AST::Node* AST::MakeNode_Ternary (AST::Node* cond, AST::Node* left, AST::Node* right) {
	AST::Node* node = DNEWCLASS(AST::Node, (AST_TAG_TERNARY));
	DASSERT(cond && left && right);
	DPTR(node)->PushBack(cond,	AST_CHILD_COND);
	DPTR(node)->PushBack(left,	AST_CHILD_LEFT);
	DPTR(node)->PushBack(right,	AST_CHILD_RIGHT);
	return node;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::MakeNode_ConstValue (double num) {
	AST::Node* node = DNEWCLASS(AST::Node, (AST_TAG_NUM_CONST));
	node->SetAttribute(AST_ATTRIBUTE_CONSTVALUE, AST::NumItem::New(num));
	return node;
}

AST::Node* AST::MakeNode_ConstValue (void)
	{ return DNEWCLASS(AST::Node, (AST_TAG_NIL)); }

AST::Node* AST::MakeNode_ConstValue (bool val){ 
	AST::Node* node = DNEWCLASS(AST::Node, (AST_TAG_BOOL_CONST));
	node->SetAttribute(AST_ATTRIBUTE_CONSTVALUE, (val ? 1 : 0));
	return node;
}

/////////////////////////////////////////////////////////////

#define	MAKE_STRING_LIST(tag, attr, str)								\
	AST::Node* node = DNEWCLASS(AST::Node, (tag));						\
	node->SetAttribute(attr, AST::StringList::New(str));				\
	return node

#define	ADD_STRING_LIST(node, attr, str)								\
	AST::StringList* l = (AST::StringList*) node->GetAttribute(attr);	\
	DPTR(l)->Add(str);													\
	return node

AST::Node* AST::MakeNode_StringConst (char* str) 
	{ MAKE_STRING_LIST(AST_TAG_STRING_CONST, AST_ATTRIBUTE_ITEMS, str); }

AST::Node* AST::MakeNode_StringConst (AST::Node* left, char* right) 
	{ ADD_STRING_LIST(left, AST_ATTRIBUTE_ITEMS, right); }

AST::Node* AST::MakeNode_StringifyDottedIdents (const char* id)
	{ MAKE_STRING_LIST(AST_TAG_STRINGIFY_DOTTED_IDENTS, AST_ATTRIBUTE_ITEMS, id); }

AST::Node* AST::MakeNode_StringifyDottedIdents (AST::Node* prefix, const char* id)
	{ ADD_STRING_LIST(prefix, AST_ATTRIBUTE_ITEMS, id); }

AST::Node* AST::MakeNode_StringifyNamespaceIdent (AST::IdList* ns, const char* id) {
	DASSERT(ns);
	ns->Add(id);
	MAKE_WITH_ONE_ATTR(AST_TAG_STRINGIFY_NAMESPACE_IDENT, AST_ATTRIBUTE_NAMESPACE, ns);
}

/////////////////////////////////////////////////////////////

AST::IdList* AST::MakeNode_NamespacePath (AST::IdList* ns, const char* id)
	{ return MakeNode_IdentList(ns, id); }

AST::Node* AST::MakeNode_NamespaceLvalue (AST::IdList* ns, const char* id) {
	DASSERT(ns && !DPTR(ns)->l.empty());

	AST::Node* node = DNEWCLASS(AST::Node, (AST_TAG_LVALUE_NAMESPACE_IDENT));
	DPTR(node)->SetAttribute(AST_ATTRIBUTE_NAMESPACE, ns);
	DPTR(node)->SetAttribute(AST_ATTRIBUTE_NAME, Translate_StringWithLateDestruction(ucopystr(id)));

	return node;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::MakeNode_SELF (void)		{ MAKE_LEAF(AST_TAG_SELF);				}
AST::Node* AST::MakeNode_ARGUMENTS (void)	{ MAKE_LEAF(AST_TAG_ARGUMENTS);			}
AST::Node* AST::MakeNode_TRIPLE_DOT (void)	{ MAKE_LEAF(AST_TAG_TRIPLE_DOT);		}
AST::Node* AST::MakeNode_LAMBDA_REF	(void)	{ MAKE_LEAF(AST_TAG_LAMBDA_REF);	}
AST::Node* AST::MakeNode_NEWSELF (void)		{ MAKE_LEAF(AST_TAG_LVALUE_NEWSELF);	}
AST::Node* AST::MakeNode_BREAK (void)		{ MAKE_LEAF(AST_TAG_BREAK);				}
AST::Node* AST::MakeNode_CONTINUE (void)	{ MAKE_LEAF(AST_TAG_CONTINUE);			}

/////////////////////////////////////////////////////////////

static AST::Node* MakeNode_EntireExpressionList (AST::NodeList* exprs)
	{ MAKE_FROM_NODELIST_EX(AST_TAG_EXPR_LIST, AST_TAG_EXPRLIST_ITEM, AST_CHILD_EXPR, exprs); }

AST::Node* AST::MakeNode_FunctionParenthesisForm (AST::Node* func)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_FUNCTION_PARENTHESIS, AST_CHILD_FUNCTION, func);  }

AST::NodeList* AST::MakeNode_ExpressionList (AST::NodeList* left, AST::Node* expr)
	{ MAKE_ITEMLIST(AST::NodeList, left, DNULLCHECK(expr)); }

AST::Node* AST::MakeNode_LateBoundArgument (AST::Node* expr)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_LATEBOUND_ARGUMENT, AST_CHILD_EXPR, expr); } 

AST::Node* AST::MakeNode_Argument (AST::Node* expr)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_ARGUMENT, AST_CHILD_EXPR, expr); } 

AST::Node* AST::MakeNode_FunctionCall (AST::Node* func, AST::NodeList* args){
	AST::Node* call = DNEWCLASS(AST::Node, (AST_TAG_FUNCTION_CALL));
	call->PushBack(func, AST_CHILD_FUNCTION);
	call->PushBack(MakeNode_EntireExpressionList(args), AST_CHILD_ACTUALS);
	return call;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::MakeNode_Lvalue	(const char* id)
	{ MAKE_WITH_STRING_ATTR(AST_TAG_LVALUE_IDENT, AST_ATTRIBUTE_NAME, id); }

AST::Node* AST::MakeNode_AttrLvalue (const char* id)
	{ MAKE_WITH_STRING_ATTR(AST_TAG_LVALUE_ATTRIBUTE, AST_ATTRIBUTE_NAME, id); }

AST::Node* AST::MakeNode_StaticLvalue (const char* id)
	{ MAKE_WITH_STRING_ATTR(AST_TAG_LVALUE_STATIC_IDENT, AST_ATTRIBUTE_NAME, id); }

AST::Node* AST::MakeNode_LocalLvalue (const char* id)
	{ MAKE_WITH_STRING_ATTR(AST_TAG_LVALUE_LOCAL_IDENT, AST_ATTRIBUTE_NAME, id); } 

/////////////////////////////////////////////////////////////

AST::Node* AST::MakeNode_DotIndexIdent (const char* id)
	{ MAKE_WITH_STRING_ATTR(AST_TAG_DOTINDEX_IDENT, AST_ATTRIBUTE_VALUE, id); }

AST::Node* AST::MakeNode_DotIndexOpString (const char* op)
	{ MAKE_WITH_STRING_ATTR(AST_TAG_DOTINDEX_OPSTRING, AST_ATTRIBUTE_VALUE, op); }

AST::Node* AST::MakeNode_DotIndexStringConst (AST::Node* stringConst)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_DOTINDEX_STRINGCONST, AST_CHILD_EXPR, stringConst); }

AST::Node* AST::MakeNode_BracketIndexOperatorMethod (const char* op)
	{ MAKE_WITH_STRING_ATTR(AST_TAG_BRACKETINDEX_OPMETHOD, AST_ATTRIBUTE_VALUE, op); }

/////////////////////////////////////////////////////////////

AST::Node* AST::MakeNode_TableContentDot (AST::Node* table, AST::Node* index) 
	{ MAKE_WITH_TWO_CHILDREN(AST_TAG_TABLE_CONTENT_DOT, AST_CHILD_TABLE, table, AST_CHILD_INDEX, index); }

AST::Node* AST::MakeNode_BoundedTableContentDot (AST::Node* table, AST::Node* index)
	{ MAKE_WITH_TWO_CHILDREN(AST_TAG_TABLE_CONTENT_DOUBLE_DOT, AST_CHILD_TABLE, table, AST_CHILD_INDEX, index); }

AST::Node* AST::MakeNode_TableContentBracket (AST::Node* table, AST::Node* index)
	{ MAKE_WITH_TWO_CHILDREN(AST_TAG_TABLE_CONTENT_BRACKET, AST_CHILD_TABLE, table, AST_CHILD_INDEX, index); }

AST::Node* AST::MakeNode_BoundedTableContentBracket	(AST::Node* table, AST::Node* index)
	{ MAKE_WITH_TWO_CHILDREN(AST_TAG_TABLE_CONTENT_DOUBLE_BRACKET, AST_CHILD_TABLE, table, AST_CHILD_INDEX, index); }

/////////////////////////////////////////////////////////////

AST::Node* AST::MakeNode_TableObject (AST::Node* table)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_TABLE_OBJECT, AST_CHILD_EXPR, table); }

AST::Node* AST::MakeNode_TableConstructor (AST::NodeList* elems)
	{ MAKE_FROM_NODELIST(AST_TAG_TABLE_CONSTRUCTOR, elems); }

AST::Node* AST::MakeNode_FunctionExpression (AST::Node* func)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_FUNCTION_EXPR, AST_CHILD_EXPR, func); }

AST::NodeList* AST::MakeNode_TableElements (AST::NodeList* elems, AST::Node* elem)
	{ MAKE_ITEMLIST(AST::NodeList, elems, DNULLCHECK(elem)); }

AST::Node* AST::MakeNode_IdentIndexElement (const char* id, AST::Node* expr) {
	AST::Node* node = DNEWCLASS(AST::Node, (AST_TAG_TABLE_INDENTINDEX_ELEM));
	DPTR(node)->SetAttribute(AST_ATTRIBUTE_NAME, id);
	DPTR(node)->PushBack(expr, AST_CHILD_EXPR);
	return node;
}

AST::Node* AST::MakeNode_UnindexedValue	(AST::Node* elem)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_TABLE_UNINDEXED_ELEM, AST_CHILD_EXPR, elem); }

AST::Node* AST::MakeNode_DottedIdent (const char* id)
	{ MAKE_WITH_STRING_ATTR(AST_TAG_TABLE_DOTTED_IDENT, AST_ATTRIBUTE_NAME, id); }

AST::Node* AST::MakeNode_OperatorIndex (const char* op)
	{ MAKE_WITH_STRING_ATTR(AST_TAG_TABLE_OPERATOR_INDEX, AST_ATTRIBUTE_VALUE, op); }

AST::Node* AST::MakeNode_IndexedValues (AST::NodeList* indices, AST::NodeList* values){
	DASSERT(indices && values);
	AST::Node* indicesNode = DNEWCLASS(AST::Node, (AST_TAG_TABLE_INDICES));
	AST::Node* valuesNode = DNEWCLASS(AST::Node, (AST_TAG_TABLE_VALUES));

	indicesNode->AddChildrenEx(indices, AST_TAG_EXPRLIST_ITEM, AST_CHILD_EXPR);
	valuesNode->AddChildrenEx(values, AST_TAG_EXPRLIST_ITEM, AST_CHILD_EXPR);

	MAKE_WITH_TWO_CHILDREN(
		AST_TAG_TABLE_INDEXED_VALUES, 
		AST_CHILD_INDICES,	indicesNode,	// 1st child
		AST_CHILD_VALUES,	valuesNode		// 2nd child
	);
}

AST::Node* AST::MakeNode_NewAttribute (char* id, AST::Node* set, AST::Node* get) {
	AST::Node* attr = DNEWCLASS(AST::Node, (AST_TAG_TABLE_NEW_ATTRIBUTE));
	attr->SetAttribute(AST_ATTRIBUTE_NAME, Translate_StringWithLateDestruction(ucopystr(id)));
	attr->PushBack(set, AST_CHILD_SET);
	attr->PushBack(get, AST_CHILD_GET);
	return attr;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::MakeNode_IfStmt (AST::Node* cond, AST::Node* stmt) {
	MAKE_WITH_TWO_CHILDREN(AST_TAG_IF, AST_CHILD_COND, cond, AST_CHILD_STMT, stmt);
}

AST::Node* AST::MakeNode_IfElseStmt (AST::Node* cond, AST::Node* ifStmt, AST::Node* elseStmt) {
	AST::Node* stmt = DNEWCLASS(AST::Node, (AST_TAG_IF_ELSE));
	stmt->PushBack(cond,		AST_CHILD_COND);
	stmt->PushBack(ifStmt,		AST_CHILD_IF);
	stmt->PushBack(elseStmt,	AST_CHILD_ELSE);
	return stmt;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::MakeNode_WhileStmt (AST::Node* cond, AST::Node* stmt) 
	{ MAKE_WITH_TWO_CHILDREN(AST_TAG_WHILE, AST_CHILD_COND, cond, AST_CHILD_STMT, stmt); }

/////////////////////////////////////////////////////////////

AST::Node* AST::MakeNode_ForSuffix (AST::NodeList* exprs) 
	{ return MakeNode_EntireExpressionList(exprs); }

AST::Node* AST::MakeNode_ForInit (AST::NodeList* exprs)
	{ return MakeNode_EntireExpressionList(exprs); }

AST::Node* AST::MakeNode_ForStmt (AST::Node* init, AST::Node* cond, AST::Node* suffix, AST::Node* stmt){

	AST::Node* forStmt = DNEWCLASS(AST::Node, (AST_TAG_FOR));
	forStmt->PushBack(init,		AST_CHILD_INIT);	
	forStmt->PushBack(cond,		AST_CHILD_COND);
	forStmt->PushBack(suffix,	AST_CHILD_SUFFIX);	
	forStmt->PushBack(stmt,		AST_CHILD_STMT);

	return forStmt;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::MakeNode_ForeachStmt (AST::Node* value, AST::Node* index, AST::Node* container, AST::Node* stmt) {
	AST::Node* foreachStmt = DNEWCLASS(AST::Node, (AST_TAG_FOREACH));
	if (index)
		foreachStmt->PushBack(index, AST_CHILD_INDEX);
	foreachStmt->PushBack(value,		AST_CHILD_LVALUE);		
	foreachStmt->PushBack(container,	AST_CHILD_CONTAINER);
	foreachStmt->PushBack(stmt,			AST_CHILD_STMT);
	return foreachStmt;
}

/////////////////////////////////////////////////////////////

AST::Node* AST::MakeNode_THROW (AST::Node* expr)
	{ MAKE_WITH_ONE_CHILD(AST_TAG_THROW, AST_CHILD_EXPR, expr); }

AST::Node* AST::MakeNode_ExceptionVar (const char* id)
	{ MAKE_WITH_STRING_ATTR(AST_TAG_EXCEPTION_VAR, AST_ATTRIBUTE_NAME, id); }

AST::Node* AST::MakeNode_Exception (AST::Node* tryStmt, AST::Node* var, AST::Node* trapStmt) {
	AST::Node* handlerStmt = DNEWCLASS(AST::Node, (AST_TAG_TRYTRAP));
	handlerStmt->PushBack(tryStmt,	AST_CHILD_TRY);
	handlerStmt->PushBack(var,		AST_CHILD_VAR);
	handlerStmt->PushBack(trapStmt,	AST_CHILD_TRAP);
	return handlerStmt;
}

/////////////////////////////////////////////////////////

