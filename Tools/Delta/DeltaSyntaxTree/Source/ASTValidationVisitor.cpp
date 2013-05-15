// ASTValidationVisitor.cpp
// Tree visitor to validate that the AST is well-formed.
// ScriptFighter Project.
// Y. Lilis, July 2011.
//
#include "ASTValidationVisitor.h"
#include "ASTTags.h"
#include "ASTMetaTags.h"
#include "ustrings.h"

///////////////////////////////////////////////////////////
//Utility functions

//TODO: Copied from DeltaString::IsIdentifier, RF: put in lexutils
static bool IsIdentifier (const std::string& s) {

	if (s.empty())
		return false;

	std::string::const_iterator i = s.begin();

	if (!isalpha(*i))
		return false;

	while (++i != s.end())
		if (!isalnum(*i) && *i != '_')
			return false;
	return true;
}

///////////////////////////////////////////////////////////

#define VISITOR	DNULLCHECK((AST::ValidationVisitor*) closure)

#define	yysetline_ex(node)											\
	do {															\
		if (VISITOR->lineGetter && VISITOR->lineSetter)				\
			if (util_ui32 line = (*VISITOR->lineGetter)(node))		\
				VISITOR->lineSetter(line);							\
	}	while(false)

#define	yysetline()	yysetline_ex(node)

///////////////////////////////////////////////////////////

#define	_H(name, tag)		\
		SetHandler(tag, &Handle_##name, this)

#define	_C(name, tag, id)	\
		SetContextDependentHandler(tag, id, &Handle_##name, this)

///////////////////////////////////////////////////////////

#define VALIDATE(cond, msg)																					\
	do {																									\
		if (!(cond)) {																						\
			VISITOR->error = uconstructstr("At node '%s', %s", node->GetTag().c_str(), ucstringarg(msg));	\
			VISITOR->Stop();																					\
			return;																							\
		}																									\
	} while(false)

///////////////////////////////////////////////////////////

#define TAG_VALIDATOR_EX(index, tag, validator)	\
	IndexedChildValidator(index, validator, CHILD_INDEX_TAG_MISMATCH(index, tag))

#define TAG_VALIDATOR(index, tag)	TAG_VALIDATOR_EX(index, tag, TagValidator(tag))

#define FUNC_VALIDATOR_EX(index, func, validator)	\
	IndexedChildValidator(index, validator, CHILD_INDEX_FAIL(index, VISITOR->GetFuncValidatorMessage(func)))

#define FUNC_VALIDATOR(index, func)	FUNC_VALIDATOR_EX(index, func, PtrFuncValidator(func))

#define LIST_TAG_VALIDATOR(tag)	\
	IndexedChildValidator("", TagValidator(tag), uconstructstr("all children should be of type '%s'", ucstringarg(tag)))

#define LIST_NAME_VALIDATOR()	\
	IndexedChildValidator("", PtrFuncValidator(&IsName), uconstructstr("all children should be valid names"))

//*************************

#define INVALID_FUNCTION_NAME(name)				uconstructstr("'%s' is not a valid function name", ucstringarg(name))
#define INVALID_NAME(name)						uconstructstr("'%s' is not a valid identifier", ucstringarg(name))

#define CHILDREN_TOTAL_MISMATCH(size)			uconstructstr("%d children expected ", size)
#define NO_CHILDREN								"no children expected"
#define AT_LEAST_ONE_CHILD						"at least one child expected"

#define CHILD_INDEX_FAIL(index, failureCause)	uconstructstr("child with index '%s' %s", index, ucstringarg(failureCause))
#define CHILD_INDEX_TAG_MISMATCH(index, tag)	CHILD_INDEX_FAIL(index, uconstructstr("should be of type '%s'", ucstringarg(tag)))

#define MISSING_CHILD(index)					uconstructstr("missing child with index '%s'", ucstringarg(index))
#define MISSING_ATTRIBUTE(attribute)			uconstructstr("missing attribute '%s'", ucstringarg(attribute))
#define ATTRIBUTE_TYPE_MISMATCH(attr, type)		uconstructstr("attribute '%s' should be of type '%s'", ucstringarg(attr), ucstringarg(type))

#define META_GENERATED_INVALID_PARENT			"invalid parent"
#define INLINE_CHILDREN							uconstructstr("should have at most one child with index %s", AST_CHILD_EXPR)
#define ESCAPE_CARDINALITY						"cardinality should be one or greater"
#define OUTSIDE_QUOTES							"only valid within quotes"
#define RENAME_OUTSIDE_QUOTES					"rename attribute is only valid within quotes"

#define STMTS_PARENT							"statement lists should reside only in top level code (normal or quasi-quoted) or compounds"
#define STMTS_CHILDREN							"statement lists should contain only statements and using directives"
#define NON_TOP_LEVEL_USING_DIRECTIVES			"using directives are valid only at global scope"
#define USING_DIRECTIVE_AFTER_STMT				"no statement should precede a using directive"

#define RETURN_CHILD							"child with index 'expr' should be an expression or a function expression"

#define TABLE_ELEMENT_CHILDREN					"all children should be table elements"
#define QUOTED_ELEMENT_CHILDREN					"all children should be valid quoted elements"

#define ACTUAL_ARGUMENTS						"should contain only valid actual arguments"
#define TABLE_INDICES							"should contain only valid table indices"
#define TABLE_VALUES							"should contain only valid table values"
#define EXPRESSIONS								"should contain only valid expressions"

///////////////////////////////////////////////////////////

AST::ValidationVisitor::ValidationVisitor (bool allowEmptyInlines, LineGetter lineGetter, LineSetter lineSetter)
	: allowEmptyInlines(allowEmptyInlines), allowRenames(false), lineGetter(lineGetter), lineSetter(lineSetter)
{
	// Context independent handlers.
	_H(Program,							AST_TAG_PROGRAM);
	_H(Name,							AST_TAG_NAME);
	_H(Stmts,							AST_TAG_STMTS);
	_H(Stmt,							AST_TAG_STMT);
	_H(BasicStmt,						AST_TAG_BASIC_STMT);
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
	_H(AssignExpr,						AST_TAG_OP_ADD_ASSIGN);
	_H(AssignExpr,						AST_TAG_OP_SUB_ASSIGN);
	_H(AssignExpr,						AST_TAG_OP_MUL_ASSIGN);
	_H(AssignExpr,						AST_TAG_OP_DIV_ASSIGN);
	_H(AssignExpr,						AST_TAG_OP_MOD_ASSIGN);

	std::string binaryExpressions[] = {
		AST_TAG_OP_GT,
		AST_TAG_OP_LT,
		AST_TAG_OP_GE,
		AST_TAG_OP_LE,
		AST_TAG_OP_EQ,
		AST_TAG_OP_NE,
		AST_TAG_AND,
		AST_TAG_OR,
		AST_TAG_OP_ADD,
		AST_TAG_OP_SUB,
		AST_TAG_OP_MUL,
		AST_TAG_OP_DIV,
		AST_TAG_OP_MOD
	};
	for (unsigned i = 0; i < uarraysize(binaryExpressions); ++i)
		_H(BinaryExpr, binaryExpressions[i]);

	std::string unaryExpressions[] = {
		AST_TAG_NOT,
		AST_TAG_UMINUS,
		AST_TAG_EXPR,
		AST_TAG_ARGUMENT,
		AST_TAG_LATEBOUND_ARGUMENT,
		AST_TAG_PARENTHESISED_EXPR
	};
	for (unsigned i = 0; i < uarraysize(unaryExpressions); ++i)
		_H(UnaryExprExpression, unaryExpressions[i]);

	_H(TermLvalueArith,					AST_LVALUE_PLUSPLUS);
	_H(TermLvalueArith,					AST_LVALUE_MINUSMINUS);
	_H(TermLvalueArith,					AST_PLUSPLUS_LVALUE);
	_H(TermLvalueArith,					AST_MINUSMINUS_LVALUE);
	_H(Ternary,							AST_TAG_TERNARY);
	_H(PrimaryExpression,				AST_TAG_PRIMARY_EXPRESSION);
	_H(ExprList,						AST_TAG_EXPR_LIST);
	_H(NumConst, 						AST_TAG_NUM_CONST);
	_H(NilConst, 						AST_TAG_NIL);
	_H(BoolConst, 						AST_TAG_BOOL_CONST);
	_H(StringConst, 					AST_TAG_STRING_CONST);
	_H(StringifyDottedIdents, 			AST_TAG_STRINGIFY_DOTTED_IDENTS);
	_H(StringifyNamespaceIdent,			AST_TAG_STRINGIFY_NAMESPACE_IDENT);
	_H(NamespaceLvalue,					AST_TAG_LVALUE_NAMESPACE_IDENT);
	_H(FunctionParenthesisForm,			AST_TAG_FUNCTION_PARENTHESIS);

	std::string keywords[] = {
		AST_TAG_SELF,
		AST_TAG_ARGUMENTS,
		AST_TAG_TRIPLE_DOT,
		AST_TAG_LAMBDA_REF,
		AST_TAG_LVALUE_NEWSELF,
		AST_TAG_BREAK,
		AST_TAG_CONTINUE
	};
	for (unsigned i = 0; i < uarraysize(keywords); ++i)
		_H(KwdExpr, keywords[i]);

	_H(FunctionCall,					AST_TAG_FUNCTION_CALL);
	_H(Lvalues,							AST_TAG_LVALUE_IDENT);
	_H(Lvalues,							AST_TAG_LVALUE_STATIC_IDENT);
	_H(Lvalues,							AST_TAG_LVALUE_LOCAL_IDENT);
	_H(AttrLvalue,						AST_TAG_LVALUE_ATTRIBUTE);
	_H(DotIndexIdent,					AST_TAG_DOTINDEX_IDENT);
	_H(DotIndexOpString,				AST_TAG_DOTINDEX_OPSTRING);
	_H(DotIndexStringConst,				AST_TAG_DOTINDEX_STRINGCONST);
	_H(BracketIndexOperatorMethod,		AST_TAG_BRACKETINDEX_OPMETHOD);
	_H(TableContentDot,					AST_TAG_TABLE_CONTENT_DOT);
	_H(TableContentDot,					AST_TAG_TABLE_CONTENT_DOUBLE_DOT);
	_H(TableContentBracket,				AST_TAG_TABLE_CONTENT_BRACKET);
	_H(TableContentBracket,				AST_TAG_TABLE_CONTENT_DOUBLE_BRACKET);
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
	_H(Escape,							AST_TAG_ESCAPE);
	_H(QuotedElements,					AST_TAG_QUOTED_ELEMENTS);
	_H(QuasiQuotes,						AST_TAG_QUASI_QUOTES);
	_H(Execute,							AST_TAG_EXECUTE);
	_H(Inline,							AST_TAG_INLINE);

	// Context dependent handlers.
	_C(Identifier,					AST_TAG_USING_BYTECODE_LIBRARY,	AST_CHILD_NAME);
	_C(Identifier,					AST_TAG_CONSTDEF,				AST_CHILD_NAME);
	_C(Identifier,					AST_TAG_LVALUE_IDENT,			AST_CHILD_NAME);
	_C(Identifier,					AST_TAG_LVALUE_ATTRIBUTE,		AST_CHILD_NAME);
	_C(Identifier,					AST_TAG_LVALUE_STATIC_IDENT,	AST_CHILD_NAME);
	_C(Identifier,					AST_TAG_LVALUE_LOCAL_IDENT,		AST_CHILD_NAME);
	_C(Identifier,					AST_TAG_EXCEPTION_VAR,			AST_CHILD_NAME);
	_C(Identifier,					AST_TAG_TABLE_IDENTINDEX_ELEM,	AST_CHILD_NAME);
	_C(Identifier,					AST_TAG_TABLE_NEW_ATTRIBUTE,	AST_CHILD_NAME);
	



#define FUNC_VALIDATOR_MESSAGE(func, msg)	\
	funcValidatorMessages[func] = uconstructstr("should be %s", msg)

	FUNC_VALIDATOR_MESSAGE(IsName,					"a valid name");
	FUNC_VALIDATOR_MESSAGE(IsUsingDirective,		"a using directive");
	FUNC_VALIDATOR_MESSAGE(IsFunction,				"a function");
	FUNC_VALIDATOR_MESSAGE(IsStmt,					"a statement");
	FUNC_VALIDATOR_MESSAGE(IsBasicStmt,				"a basic statement");
	FUNC_VALIDATOR_MESSAGE(IsBasicExprStmt,			"a basic expression statement");
	FUNC_VALIDATOR_MESSAGE(IsBasicNonExprStmt,		"a basic non-expression statement");
	FUNC_VALIDATOR_MESSAGE(IsExpression,			"an expression");
	FUNC_VALIDATOR_MESSAGE(IsAssignExpression,		"an assignment expression");
	FUNC_VALIDATOR_MESSAGE(IsRelationalExpression,	"a relational expression");
	FUNC_VALIDATOR_MESSAGE(IsBooleanExpression,		"a boolean expression");
	FUNC_VALIDATOR_MESSAGE(IsArithmeticExpression,	"an arithmetic expression");
	FUNC_VALIDATOR_MESSAGE(IsTerm,					"a term expression");
	FUNC_VALIDATOR_MESSAGE(IsPrimary,				"a primary expression");
	FUNC_VALIDATOR_MESSAGE(IsConstValue,			"a const value");
	FUNC_VALIDATOR_MESSAGE(IsStringConstUsed,		"a used string const");
	FUNC_VALIDATOR_MESSAGE(IsLvalue,				"an lvalue");
	FUNC_VALIDATOR_MESSAGE(IsTableContent,			"a table content");
	FUNC_VALIDATOR_MESSAGE(IsActualArgument,		"an actual argument");
	FUNC_VALIDATOR_MESSAGE(IsFunctionCallObject,	"a function call object");
	FUNC_VALIDATOR_MESSAGE(IsTableElement,			"a table element");
	FUNC_VALIDATOR_MESSAGE(IsFunctionAndTableObject,"a function or table object");
	FUNC_VALIDATOR_MESSAGE(IsDotIndex,				"a dot index");
	FUNC_VALIDATOR_MESSAGE(IsBracketIndex,			"a bracket index");
	FUNC_VALIDATOR_MESSAGE(IsIndexExpression,		"an index expression");
	FUNC_VALIDATOR_MESSAGE(IsContentExpression,		"a content expression");
	FUNC_VALIDATOR_MESSAGE(IsFunctionExpression,	"a function expression");
	FUNC_VALIDATOR_MESSAGE(IsUnindexedValue,		"an unindexed value");
	FUNC_VALIDATOR_MESSAGE(IsMetaExpression,		"a meta expression");
	FUNC_VALIDATOR_MESSAGE(IsQuasiQuoted,			"valid quasi-quoted code");
	FUNC_VALIDATOR_MESSAGE(IsQuotedElement,			"a quoted element");
}

AST::ValidationVisitor::~ValidationVisitor () {}

///////////////////////////////////////////////////////////

bool AST::ValidationVisitor::operator()(TreeNode* root)
	{ if (root) DPTR(root)->AcceptPreOrder(this); return error.empty(); }

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_Program (AST_VISITOR_ARGS)
	{ Handle_Compound(AST_VISITOR_ACTUALS); }

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_Name (AST_VISITOR_ARGS) {
	if (!entering) {
		VALIDATE(DPTR(node)->HasAttribute(AST_ATTRIBUTE_NAME), MISSING_ATTRIBUTE(AST_ATTRIBUTE_NAME));
		TreeAttribute* name = DPTR(node)->GetAttribute(AST_ATTRIBUTE_NAME);
		VALIDATE(DPTR(name)->IsString(), ATTRIBUTE_TYPE_MISMATCH(AST_ATTRIBUTE_NAME, "string"));
		if (!VISITOR->allowRenames && DPTR(node)->HasAttribute(AST_ATTRIBUTE_RENAME))
			VALIDATE(VISITOR->quotes.inside(), RENAME_OUTSIDE_QUOTES);
	}
}

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_FunctionBasic (AST_VISITOR_ARGS, const IndexedChildValidator& bodyValidator) {

	PtrFuncValidator isName(&IsName);
	TagValidator formalsTag(AST_TAG_FORMAL_ARGS);

	IndexedChildValidatorList childValidators;
	if (TreeNode* name = DPTR(node)->GetChild(AST_CHILD_NAME))
		childValidators.push_back(FUNC_VALIDATOR_EX(AST_CHILD_NAME, IsName, isName));

	childValidators.push_back(TAG_VALIDATOR_EX(AST_CHILD_FORMALS, AST_TAG_FORMAL_ARGS, formalsTag));
	childValidators.push_back(bodyValidator);

	StringList attributes;
	attributes.push_back(AST_ATTRIBUTE_CLASS);
	if (DPTR(node)->HasAttribute(AST_ATTRIBUTE_LINKAGE))
		attributes.push_back(AST_ATTRIBUTE_LINKAGE);

	Handle_NaryNode(AST_VISITOR_ACTUALS, childValidators, attributes);
}

//*************************

void AST::ValidationVisitor::Handle_Function (AST_VISITOR_ARGS) {
	Handle_FunctionBasic(AST_VISITOR_ACTUALS, TAG_VALIDATOR(AST_CHILD_BODY, AST_TAG_COMPOUND));
	if (!VISITOR->ShouldLeave() && !entering) {
		TreeNode* n;
		if ((n = DPTR(node)->GetChild(AST_CHILD_NAME)) && DPTR(n)->GetTag() == AST_TAG_NAME) {
			const std::string name = NAME(n);
			const char* operators[] = {
				"+", "-", "*", "/", "%",
				">", "<", "!=", "==", ">=", "<=", "=",
				"+_", "-_", "*_", "/_", "%_",
				">_", "<_", "!=_", "==_", ">=_", "<=_",
				"()", "=()", ".", ".=", AST_VALUE_TOSTRING_SYMBOLIC_NAME
			};
			bool found = false;
			for (util_ui8 i = 0; i < uarraysize(operators); ++i)
				if (name == operators[i]) {
					found = true;
					break;
				}
			VALIDATE(found || IsIdentifier(name), INVALID_FUNCTION_NAME(name));
		}
	}
}

void AST::ValidationVisitor::Handle_LambdaFunction (AST_VISITOR_ARGS)
	{  Handle_FunctionBasic(AST_VISITOR_ACTUALS, FUNC_VALIDATOR(AST_CHILD_EXPR, IsExpression)); }

//*************************

void AST::ValidationVisitor::Handle_FunctionStmt (AST_VISITOR_ARGS)
	{ Handle_UnaryNode(AST_VISITOR_ACTUALS, FUNC_VALIDATOR(AST_CHILD_FUNCTION, &IsFunction)); }

//*************************

void AST::ValidationVisitor::Handle_FormalArgs (AST_VISITOR_ARGS){
	Handle_ListNode(AST_VISITOR_ACTUALS, LIST_NAME_VALIDATOR());
	if (!VISITOR->ShouldStop() && !entering) {
		const util_ui32 totalChildren = DPTR(node)->GetTotalChildren();
		for (util_ui32 i = 0; i < totalChildren; ++i) {
			TreeNode* child = DPTR(node)->GetChild(i);
			if (i != totalChildren - 1 || DPTR(child)->GetTag() == AST_TAG_NAME && NAME(child) != AST_VALUE_VARARGS_FORMAL_NAME)
				Handle_Identifier(child, "", entering, closure);
			if (VISITOR->ShouldStop())
				break;
		}
	}
}

//*************************

void AST::ValidationVisitor::Handle_Compound (AST_VISITOR_ARGS) {
	yysetline();
	TreeNode* child;
	if (entering && (child = DPTR(node)->GetChild(AST_CHILD_STMTS)))
		VALIDATE(DPTR(child)->GetTag() == AST_TAG_STMTS, CHILD_INDEX_TAG_MISMATCH(AST_CHILD_STMTS, AST_TAG_STMTS));
} 

//*************************

void AST::ValidationVisitor::Handle_Stmts (AST_VISITOR_ARGS) {
	Handle_ListNode(AST_VISITOR_ACTUALS, LIST_TAG_VALIDATOR(AST_TAG_STMT));
	const util_i32 n = DPTR(node)->GetTotalChildren();
	VALIDATE(n, AT_LEAST_ONE_CHILD);
	if (!VISITOR->ShouldStop() && !entering) {	//after individual stmt validation
		if (TreeNode* parent = DPTR(node)->GetParent()) {
			const std::string& tag = DPTR(parent)->GetTag();
			VALIDATE(tag == AST_TAG_PROGRAM || tag == AST_TAG_COMPOUND || tag == AST_TAG_QUASI_QUOTES, STMTS_PARENT);
			bool afterFirstStmt = false;
			for (util_i32 i = 0; i < n; ++i) {
				TreeNode* child = DPTR(node)->GetChild(i);
				DASSERT(child);
				child = DPTR(child)->GetChild(AST_CHILD_STMT);
				DASSERT(child);
				yysetline_ex(child);
				VALIDATE(IsUsingDirective(DPTR(child)) || IsStmt(DPTR(child)), STMTS_CHILDREN);
				if (IsUsingDirective(DPTR(child))) {
					VALIDATE(tag != AST_TAG_COMPOUND, NON_TOP_LEVEL_USING_DIRECTIVES);
					VALIDATE(!afterFirstStmt, USING_DIRECTIVE_AFTER_STMT);
				}
				else {
					DASSERT(IsStmt(DPTR(child)));
					afterFirstStmt = !HasTag(DPTR(child), AST_TAG_EXECUTE);	//Execute may come before using directives
				}
			}
		}
	}
}

//*************************

void AST::ValidationVisitor::Handle_Stmt (AST_VISITOR_ARGS)
	{ Handle_UnaryNode(AST_VISITOR_ACTUALS, FUNC_VALIDATOR(AST_CHILD_STMT, &IsStmt)); }

//*************************

void AST::ValidationVisitor::Handle_BasicStmt (AST_VISITOR_ARGS)
	{ Handle_UnaryNode(AST_VISITOR_ACTUALS, FUNC_VALIDATOR(AST_CHILD_STMT, IsBasicStmt)); }

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_ExpressionListStmt (AST_VISITOR_ARGS) {
	Handle_ListNode(AST_VISITOR_ACTUALS, LIST_TAG_VALIDATOR(AST_TAG_EXPR));
	VALIDATE(DPTR(node)->GetTotalChildren(), AT_LEAST_ONE_CHILD);
}

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_EmptyStmt (AST_VISITOR_ARGS)
	{ Handle_NoChildren(AST_VISITOR_ACTUALS); }

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_AssertStmt (AST_VISITOR_ARGS)
	{ Handle_UnaryExprExpression(AST_VISITOR_ACTUALS); }

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_Return (AST_VISITOR_ARGS) {
	if (TreeNode* expr = node->GetChild(AST_CHILD_EXPR))
		VALIDATE(IsExpression(DPTR(expr)) || IsFunctionExpression(DPTR(expr)), RETURN_CHILD);
	else
		Handle_KwdExpr(AST_VISITOR_ACTUALS);
} 

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_UsingNamespace (AST_VISITOR_ARGS)
	{ Handle_NamespacePath(AST_VISITOR_ACTUALS); }

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_UsingByteCodeLibrary (AST_VISITOR_ARGS)
	{ Handle_UnaryNode(AST_VISITOR_ACTUALS, FUNC_VALIDATOR(AST_CHILD_NAME, IsName)); }

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_ConstDefExpression (AST_VISITOR_ARGS) {
	Handle_BinaryNode(
		AST_VISITOR_ACTUALS,
		FUNC_VALIDATOR(AST_CHILD_NAME, IsName),
		FUNC_VALIDATOR(AST_CHILD_EXPR, IsExpression)
	);
}

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_AssignExpr (AST_VISITOR_ARGS) {
	Handle_BinaryNode(
		AST_VISITOR_ACTUALS,
		FUNC_VALIDATOR(AST_CHILD_LVALUE, IsLvalue),
		FUNC_VALIDATOR(AST_CHILD_RVALUE, IsExpression)
	);
}

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_BinaryExpr (AST_VISITOR_ARGS) {
	Handle_BinaryNode(
		AST_VISITOR_ACTUALS,
		FUNC_VALIDATOR(AST_CHILD_LEFT, IsExpression),
		FUNC_VALIDATOR(AST_CHILD_RIGHT, IsExpression)
	);
}

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_UnaryExprExpression (AST_VISITOR_ARGS)
	{  Handle_UnaryNode(AST_VISITOR_ACTUALS, FUNC_VALIDATOR(AST_CHILD_EXPR, IsExpression)); }

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_TermLvalueArith (AST_VISITOR_ARGS)
	{ Handle_UnaryNode(AST_VISITOR_ACTUALS, FUNC_VALIDATOR(AST_CHILD_LVALUE, IsLvalue)); }

//*****************************

void AST::ValidationVisitor::Handle_Ternary (AST_VISITOR_ARGS) {
	Handle_TriadicNode(
		AST_VISITOR_ACTUALS,
		FUNC_VALIDATOR(AST_CHILD_COND, IsExpression),
		FUNC_VALIDATOR(AST_CHILD_LEFT, IsExpression),
		FUNC_VALIDATOR(AST_CHILD_RIGHT, IsExpression)
	);
}

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_PrimaryExpression (AST_VISITOR_ARGS)
	{ Handle_UnaryNode(AST_VISITOR_ACTUALS, FUNC_VALIDATOR(AST_CHILD_EXPR, IsPrimary)); }

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_ExprList (AST_VISITOR_ARGS)
	{ Handle_ListNode(AST_VISITOR_ACTUALS, LIST_TAG_VALIDATOR(AST_TAG_EXPRLIST_ITEM)); }

//*****************************

void AST::ValidationVisitor::Handle_ExprListChild (AST_VISITOR_ARGS, const IndexedChildValidator& validator) {
	yysetline();
	if (!VISITOR->ShouldStop() && !entering) {
		TreeNode* exprList;
		if (!validator.first.empty()) {
			exprList = DPTR(node)->GetChild(validator.first);
			DASSERT(exprList && DPTR(exprList)->GetTag() == AST_TAG_EXPR_LIST);
		}
		else
			exprList = DPTR(node);
		for (util_ui32 i = 0; i < DPTR(exprList)->GetTotalChildren(); ++i) {
			TreeNode* listItem = DPTR(exprList)->GetChild(i);
			DASSERT(listItem && DPTR(listItem)->GetTag() == AST_TAG_EXPRLIST_ITEM);
			TreeNode* child = DPTR(listItem)->GetChild(AST_CHILD_EXPR);
			VALIDATE(child, MISSING_CHILD(AST_CHILD_EXPR));
			VALIDATE((validator.second)(DPTR(child)), validator.third);
		}
	}
}

///////////////////////////////////////////////////////////

#define SINGLE_ATTRIBUTE_FUNC_IMPL(name, attr, func, type, extra)										\
	void AST::ValidationVisitor::name (AST_VISITOR_ARGS) {												\
		yysetline();																					\
		if (entering) {																					\
			VALIDATE(DPTR(node)->GetTotalChildren() == 0, NO_CHILDREN);									\
			VALIDATE(DPTR(node)->HasAttribute(attr), MISSING_ATTRIBUTE(attr));							\
			VALIDATE(DPTR(node)->GetAttribute(attr)->func(), ATTRIBUTE_TYPE_MISMATCH(attr, type));		\
			extra																						\
		}																								\
	}

SINGLE_ATTRIBUTE_FUNC_IMPL(Handle_NumConst, AST_ATTRIBUTE_CONSTVALUE, IsDouble, "double", UEMPTY)
SINGLE_ATTRIBUTE_FUNC_IMPL(Handle_BoolConst, AST_ATTRIBUTE_CONSTVALUE, IsBool, "boolean", UEMPTY)
SINGLE_ATTRIBUTE_FUNC_IMPL(Handle_StringConst, AST_ATTRIBUTE_ITEMS, IsStringList, "string list", UEMPTY)

#define LIST_IDENTIFIER_CHECK																				\
	const std::list<std::string>& l = DPTR(node)->GetAttribute(AST_ATTRIBUTE_ITEMS)->GetStringList();		\
	for(std::list<std::string>::const_iterator i = l.begin(); i != l.end(); ++i)							\
		VALIDATE(IsIdentifier(*i), INVALID_NAME(*i));

SINGLE_ATTRIBUTE_FUNC_IMPL(Handle_StringifyDottedIdents, AST_ATTRIBUTE_ITEMS, IsStringList, "string list", LIST_IDENTIFIER_CHECK)

//*****************************

void AST::ValidationVisitor::Handle_NilConst (AST_VISITOR_ARGS)
	{ Handle_NoChildren(AST_VISITOR_ACTUALS); }

//*****************************

void AST::ValidationVisitor::Handle_StringifyNamespaceIdent (AST_VISITOR_ARGS)
	{ Handle_NamespacePath(AST_VISITOR_ACTUALS); }

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_KwdExpr (AST_VISITOR_ARGS)
	{ Handle_NoChildren(AST_VISITOR_ACTUALS); }

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_FunctionCall (AST_VISITOR_ARGS) {
	Handle_BinaryNode(
		AST_VISITOR_ACTUALS,
		FUNC_VALIDATOR(AST_CHILD_FUNCTION, IsFunctionCallObject),
		TAG_VALIDATOR(AST_CHILD_ACTUALS, AST_TAG_EXPR_LIST)
	);
	Handle_ExprListChild(
		AST_VISITOR_ACTUALS,
		IndexedChildValidator(
			AST_CHILD_ACTUALS,
			PtrFuncValidator(&IsActualArgument),
			CHILD_INDEX_FAIL(AST_CHILD_ACTUALS, ACTUAL_ARGUMENTS)
		)
	);
}

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_NamespaceLvalue (AST_VISITOR_ARGS)
	{ Handle_NamespacePath(AST_VISITOR_ACTUALS); }

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_FunctionParenthesisForm (AST_VISITOR_ARGS)
	{ Handle_UnaryNode(AST_VISITOR_ACTUALS, FUNC_VALIDATOR(AST_CHILD_FUNCTION, IsFunction)); }

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_Lvalues (AST_VISITOR_ARGS)
	{ Handle_UnaryNode(AST_VISITOR_ACTUALS, FUNC_VALIDATOR(AST_CHILD_NAME, IsName)); }

//*****************************

void AST::ValidationVisitor::Handle_AttrLvalue (AST_VISITOR_ARGS)
	{ Handle_UnaryNode(AST_VISITOR_ACTUALS, FUNC_VALIDATOR(AST_CHILD_NAME, IsName)); }

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_DotIndexIdent (AST_VISITOR_ARGS) {
	if (!entering) {
		VALIDATE(DPTR(node)->HasAttribute(AST_ATTRIBUTE_VALUE), MISSING_ATTRIBUTE(AST_ATTRIBUTE_VALUE));
		TreeAttribute* value = DPTR(node)->GetAttribute(AST_ATTRIBUTE_VALUE);
		VALIDATE(DPTR(value)->IsString(), ATTRIBUTE_TYPE_MISMATCH(AST_ATTRIBUTE_VALUE, "string"));
		const std::string name = DPTR(value)->GetString();
		VALIDATE(IsIdentifier(name), INVALID_NAME(name));
	}
}

//*****************************

//TODO: validate that AST_ATTRIBUTE_VALUE is a valid operator
void AST::ValidationVisitor::Handle_DotIndexOpString (AST_VISITOR_ARGS)
	{ Handle_SingleAttributeNode(AST_VISITOR_ACTUALS, AST_ATTRIBUTE_VALUE); }

//*****************************

void AST::ValidationVisitor::Handle_DotIndexStringConst (AST_VISITOR_ARGS)
	{ Handle_UnaryExprExpression(AST_VISITOR_ACTUALS); }

//*****************************

void AST::ValidationVisitor::Handle_BracketIndexOperatorMethod (AST_VISITOR_ARGS)
	{ Handle_SingleAttributeNode(AST_VISITOR_ACTUALS, AST_ATTRIBUTE_VALUE); }

//*****************************

void AST::ValidationVisitor::Handle_TableContentDot (AST_VISITOR_ARGS) {
	Handle_BinaryNode(
		AST_VISITOR_ACTUALS,
		FUNC_VALIDATOR(AST_CHILD_TABLE, IsFunctionAndTableObject),
		FUNC_VALIDATOR(AST_CHILD_INDEX, IsDotIndex)
	);
}

//*****************************

void AST::ValidationVisitor::Handle_TableContentBracket (AST_VISITOR_ARGS) {
	Handle_BinaryNode(
		AST_VISITOR_ACTUALS,
		FUNC_VALIDATOR(AST_CHILD_TABLE, IsFunctionAndTableObject),
		FUNC_VALIDATOR(AST_CHILD_INDEX, IsBracketIndex)
	);
}

//*****************************

void AST::ValidationVisitor::Handle_TableConstructor (AST_VISITOR_ARGS){
	Handle_ListNode(
		AST_VISITOR_ACTUALS,
		IndexedChildValidator(
			"",
			PtrFuncValidator(&IsTableElement),
			TABLE_ELEMENT_CHILDREN			
		)
	);
}

//*****************************

void AST::ValidationVisitor::Handle_UnindexedValue (AST_VISITOR_ARGS)
	{ Handle_UnaryNode(AST_VISITOR_ACTUALS, FUNC_VALIDATOR(AST_CHILD_EXPR, IsUnindexedValue)); }

//*****************************

void  AST::ValidationVisitor::Handle_IdentIndexElement (AST_VISITOR_ARGS) {
	Handle_BinaryNode(
		AST_VISITOR_ACTUALS,
		FUNC_VALIDATOR(AST_CHILD_NAME, IsName),
		FUNC_VALIDATOR(AST_CHILD_EXPR, IsContentExpression)
	);
}

//*****************************

void  AST::ValidationVisitor::Handle_FunctionExpression (AST_VISITOR_ARGS)
	{ Handle_UnaryNode(AST_VISITOR_ACTUALS, FUNC_VALIDATOR(AST_CHILD_EXPR, IsFunction)); }

//*****************************

void AST::ValidationVisitor::Handle_DottedIdent (AST_VISITOR_ARGS) {
	if (!entering) {
		VALIDATE(DPTR(node)->HasAttribute(AST_ATTRIBUTE_NAME), MISSING_ATTRIBUTE(AST_ATTRIBUTE_NAME));
		TreeAttribute* nameAttr = DPTR(node)->GetAttribute(AST_ATTRIBUTE_NAME);
		VALIDATE(DPTR(nameAttr)->IsString(), ATTRIBUTE_TYPE_MISMATCH(AST_ATTRIBUTE_NAME, "string"));
		const std::string name = DPTR(nameAttr)->GetString();
		VALIDATE(IsIdentifier(name), INVALID_NAME(name));
	}
}

//*****************************

void AST::ValidationVisitor::Handle_OperatorIndex (AST_VISITOR_ARGS)
	{ Handle_SingleAttributeNode(AST_VISITOR_ACTUALS, AST_ATTRIBUTE_VALUE); }

//*****************************

void AST::ValidationVisitor::Handle_TableValues (AST_VISITOR_ARGS){
	Handle_ListNode(AST_VISITOR_ACTUALS, LIST_TAG_VALIDATOR(AST_TAG_EXPRLIST_ITEM));
	Handle_ExprListChild(
		AST_VISITOR_ACTUALS,
		IndexedChildValidator(
			"",
			PtrFuncValidator(&IsContentExpression),
			CHILD_INDEX_FAIL(AST_CHILD_VALUES, TABLE_VALUES)
		)
	);
}

//*****************************

void AST::ValidationVisitor::Handle_TableIndices (AST_VISITOR_ARGS) {
	Handle_ListNode(AST_VISITOR_ACTUALS, LIST_TAG_VALIDATOR(AST_TAG_EXPRLIST_ITEM));
	Handle_ExprListChild(
		AST_VISITOR_ACTUALS,
		IndexedChildValidator(
			"",
			PtrFuncValidator(&IsIndexExpression),
			CHILD_INDEX_FAIL(AST_CHILD_INDICES, TABLE_INDICES)
		)
	);
}

//*****************************

void AST::ValidationVisitor::Handle_IndexedValues (AST_VISITOR_ARGS) {
	Handle_BinaryNode(
		AST_VISITOR_ACTUALS,
		TAG_VALIDATOR(AST_CHILD_INDICES, AST_TAG_TABLE_INDICES),
		TAG_VALIDATOR(AST_CHILD_VALUES, AST_TAG_TABLE_VALUES)
	);
}

//*****************************

void AST::ValidationVisitor::Handle_NewAttribute (AST_VISITOR_ARGS) {
	Handle_TriadicNode(
		AST_VISITOR_ACTUALS,
		FUNC_VALIDATOR(AST_CHILD_NAME, IsName),
		FUNC_VALIDATOR(AST_CHILD_SET, IsContentExpression),
		FUNC_VALIDATOR(AST_CHILD_GET, IsContentExpression)
	);
} 

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_IfStmt (AST_VISITOR_ARGS) {
	Handle_BinaryNode(
		AST_VISITOR_ACTUALS,
		FUNC_VALIDATOR(AST_CHILD_COND, IsExpression),
		FUNC_VALIDATOR(AST_CHILD_STMT, IsStmt)
	);
}

//*****************************

void AST::ValidationVisitor::Handle_IfElseStmt (AST_VISITOR_ARGS) {
	Handle_TriadicNode(
		AST_VISITOR_ACTUALS,
		FUNC_VALIDATOR(AST_CHILD_COND,	IsExpression),
		FUNC_VALIDATOR(AST_CHILD_IF,	IsStmt),
		FUNC_VALIDATOR(AST_CHILD_ELSE,	IsStmt)
	);
}

//*****************************

void AST::ValidationVisitor::Handle_WhileStmt (AST_VISITOR_ARGS) {
	Handle_BinaryNode(
		AST_VISITOR_ACTUALS,
		FUNC_VALIDATOR(AST_CHILD_COND, IsExpression),
		FUNC_VALIDATOR(AST_CHILD_STMT, IsStmt)
	);
}


//*****************************

void AST::ValidationVisitor::Handle_ForStmt (AST_VISITOR_ARGS) {
	IndexedChildValidatorList childValidators;

	TagValidator isExprList(AST_TAG_EXPR_LIST);
	PtrFuncValidator isExpr(&IsExpression);
	PtrFuncValidator isStmt(&IsStmt);

	childValidators.push_back(TAG_VALIDATOR_EX(AST_CHILD_INIT, AST_TAG_EXPR_LIST, isExprList));
	childValidators.push_back(FUNC_VALIDATOR_EX(AST_CHILD_COND, IsExpression, isExpr));
	childValidators.push_back(TAG_VALIDATOR_EX(AST_CHILD_SUFFIX, AST_TAG_EXPR_LIST, isExprList));
	childValidators.push_back(FUNC_VALIDATOR_EX(AST_CHILD_STMT, IsStmt, isStmt));
	
	Handle_NaryNode(AST_VISITOR_ACTUALS, childValidators);
	Handle_ExprListChild(
		AST_VISITOR_ACTUALS,
		IndexedChildValidator(
			AST_CHILD_INIT,
			isExpr,
			CHILD_INDEX_FAIL(AST_CHILD_INIT, EXPRESSIONS)
		)
	);
	Handle_ExprListChild(
		AST_VISITOR_ACTUALS,
		IndexedChildValidator(
			AST_CHILD_SUFFIX,
			isExpr,
			CHILD_INDEX_FAIL(AST_CHILD_SUFFIX, EXPRESSIONS)
		)
	);
}

//*****************************

void AST::ValidationVisitor::Handle_ForeachStmt (AST_VISITOR_ARGS) {
	IndexedChildValidatorList childValidators;

	PtrFuncValidator isLvalue(&IsLvalue);

	if (node->GetChild(AST_CHILD_INDEX))
		childValidators.push_back(FUNC_VALIDATOR_EX(AST_CHILD_INDEX, IsLvalue, isLvalue));
	childValidators.push_back(FUNC_VALIDATOR_EX(AST_CHILD_LVALUE, IsLvalue, isLvalue));
	
	PtrFuncValidator isExpr(&IsExpression);
	childValidators.push_back(FUNC_VALIDATOR_EX(AST_CHILD_CONTAINER, IsExpression, isExpr));
	
	PtrFuncValidator isStmt(&IsStmt);
	childValidators.push_back(FUNC_VALIDATOR_EX(AST_CHILD_STMT, IsStmt, isStmt));
	
	Handle_NaryNode(AST_VISITOR_ACTUALS, childValidators);
} 

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_THROW (AST_VISITOR_ARGS)
	{ Handle_UnaryExprExpression(AST_VISITOR_ACTUALS); }

//*****************************

void AST::ValidationVisitor::Handle_ExceptionVar (AST_VISITOR_ARGS)
	{ Handle_Lvalues(AST_VISITOR_ACTUALS); }

//*****************************

void AST::ValidationVisitor::Handle_Exception (AST_VISITOR_ARGS) {
	Handle_TriadicNode(
		AST_VISITOR_ACTUALS,
		FUNC_VALIDATOR(AST_CHILD_TRY, IsStmt),
		TAG_VALIDATOR(AST_CHILD_VAR, AST_TAG_EXCEPTION_VAR),
		FUNC_VALIDATOR(AST_CHILD_TRAP, IsStmt)
	);
}

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_Escape (AST_VISITOR_ARGS) {
	Handle_NaryNode(AST_VISITOR_ACTUALS, IndexedChildValidatorList(1, FUNC_VALIDATOR(AST_CHILD_EXPR, IsExpression)));
	if (!VISITOR->ShouldStop() && entering) {
		VALIDATE(VISITOR->quotes.inside(), OUTSIDE_QUOTES);
		VALIDATE(DPTR(node)->HasAttribute(AST_ATTRIBUTE_CARDINALITY), MISSING_ATTRIBUTE(AST_ATTRIBUTE_CARDINALITY));
		TreeAttribute* cardinality = DPTR(node)->GetAttribute(AST_ATTRIBUTE_CARDINALITY);
		VALIDATE(DPTR(cardinality)->IsUInt(), ATTRIBUTE_TYPE_MISMATCH(AST_ATTRIBUTE_CARDINALITY, "unsigned int"));
		VALIDATE(DPTR(cardinality)->GetUInt() >= 1, ESCAPE_CARDINALITY);
	}
	if (CARDINALITY(node) == 1) {
		DASSERT(VISITOR->quotes.inside() == entering);
		if (entering)
			VISITOR->quotes.save();
		else
			VISITOR->quotes.restore();
	}
	if (TreeNode* parent = DPTR(node)->GetParent())
		VALIDATE(DPTR(parent)->GetTag() != AST_TAG_PRIMARY_EXPRESSION, META_GENERATED_INVALID_PARENT);
}

//*****************************

void AST::ValidationVisitor::Handle_QuotedElements (AST_VISITOR_ARGS) {
	Handle_ListNode(
		AST_VISITOR_ACTUALS,
		IndexedChildValidator(
			"",
			PtrFuncValidator(&IsQuotedElement),
			QUOTED_ELEMENT_CHILDREN
		)
	);
	VALIDATE(DPTR(node)->GetTotalChildren(), AT_LEAST_ONE_CHILD);
}

//*****************************

void AST::ValidationVisitor::Handle_QuasiQuotes (AST_VISITOR_ARGS) {
	Handle_UnaryNode(AST_VISITOR_ACTUALS, FUNC_VALIDATOR(AST_CHILD_EXPR, IsQuasiQuoted));
	if (entering)
		VISITOR->quotes.enter();
	else
		VISITOR->quotes.exit();
}

//*****************************

void AST::ValidationVisitor::Handle_MetaExpr (AST_VISITOR_ARGS)
	{ Handle_UnaryNode(AST_VISITOR_ACTUALS, FUNC_VALIDATOR(AST_CHILD_EXPR, IsExpression)); }

//*****************************

void AST::ValidationVisitor::Handle_Execute (AST_VISITOR_ARGS)
	{ Handle_UnaryNode(AST_VISITOR_ACTUALS, FUNC_VALIDATOR(AST_CHILD_EXPR, IsStmt)); }

//*****************************

void AST::ValidationVisitor::Handle_Inline (AST_VISITOR_ARGS) {
	yysetline();
	if (VISITOR->allowEmptyInlines) {
		util_i32 n = DPTR(node)->GetTotalChildren();
		VALIDATE(n == 0 || n == 1, INLINE_CHILDREN);
		if (!n)
			return;
	}
	Handle_UnaryNode(AST_VISITOR_ACTUALS, FUNC_VALIDATOR(AST_CHILD_EXPR, IsExpression));
	if (TreeNode* parent = DPTR(node)->GetParent())
		VALIDATE(DPTR(parent)->GetTag() != AST_TAG_PRIMARY_EXPRESSION, META_GENERATED_INVALID_PARENT);
}

///////////////////////////////////////////////////////////

void AST::ValidationVisitor::Handle_Identifier (AST_VISITOR_ARGS) {	
	if (!entering && DPTR(node)->GetTag() == AST_TAG_NAME) {
		const std::string name = NAME(node);
		VALIDATE(IsIdentifier(name), INVALID_NAME(name));
	}
}

//*****************************

void AST::ValidationVisitor::Handle_NamespacePath (AST_VISITOR_ARGS) {
	Handle_ListNode(AST_VISITOR_ACTUALS, LIST_NAME_VALIDATOR());
	if (!VISITOR->ShouldStop() && !entering) {
		const util_ui32 totalChildren = DPTR(node)->GetTotalChildren();
		VALIDATE(totalChildren, AT_LEAST_ONE_CHILD);
		if (totalChildren == 1)
			Handle_Identifier(DPTR(node)->GetChild(0), "", entering, closure);
		else
			for (util_ui32 i = 0; i < totalChildren; ++i) {
				TreeNode* child = DPTR(node)->GetChild(i);
				if (i > 0 || DPTR(child)->GetTag() == AST_TAG_NAME && NAME(child) != DELTA_LIBRARYNAMESPACE_SEPARATOR)
					Handle_Identifier(child, "", entering, closure);
				if (VISITOR->ShouldStop())
					break;
			}
	}
}

//*****************************

void AST::ValidationVisitor::Handle_ListNode (AST_VISITOR_ARGS, const IndexedChildValidator& validator) {
	yysetline();
	if (!VISITOR->ShouldStop() && entering) {
		DASSERT(validator.first.empty());
		const util_i32 n = DPTR(node)->GetTotalChildren();
		for (util_i32 i = 0; i < n; ++i) {
			TreeNode* child = DPTR(node)->GetChild(i);
			VALIDATE(child, MISSING_CHILD(unum2string(i)));
			VALIDATE((validator.second)(DPTR(child)), validator.third);
		}
	}
}

//*****************************

void AST::ValidationVisitor::Handle_NaryNode (
	AST_VISITOR_ARGS,
	const IndexedChildValidatorList&	childValidators,
	const StringList&					stringAttributes
) {
	yysetline();
	if (!VISITOR->ShouldStop() && entering) {
		VALIDATE(DPTR(node)->GetTotalChildren() == childValidators.size(), CHILDREN_TOTAL_MISMATCH(childValidators.size()));
		for (IndexedChildValidatorList::const_iterator i = childValidators.begin(); i != childValidators.end(); ++i) {
			TreeNode* child = DPTR(node)->GetChild(i->first);
			VALIDATE(child, MISSING_CHILD(i->first));
			VALIDATE((i->second)(DPTR(child)), i->third);
		}
		for (StringList::const_iterator i = stringAttributes.begin(); i != stringAttributes.end(); ++i) {
			VALIDATE(DPTR(node)->HasAttribute(*i), MISSING_ATTRIBUTE(*i));
			VALIDATE(DPTR(node)->GetAttribute(*i)->IsString(), ATTRIBUTE_TYPE_MISMATCH(*i, "string"));

		}
	}
}

//*****************************

void AST::ValidationVisitor::Handle_TriadicNode (
	AST_VISITOR_ARGS,
	const IndexedChildValidator& first,
	const IndexedChildValidator& second,
	const IndexedChildValidator& third
) {
	IndexedChildValidatorList childValidators;
	childValidators.push_back(first);
	childValidators.push_back(second);
	childValidators.push_back(third);
	Handle_NaryNode(AST_VISITOR_ACTUALS, childValidators);	
}

//*****************************

void AST::ValidationVisitor::Handle_BinaryNode (AST_VISITOR_ARGS, const IndexedChildValidator& left, const IndexedChildValidator& right) {
	IndexedChildValidatorList childValidators;
	childValidators.push_back(left);
	childValidators.push_back(right);
	Handle_NaryNode(AST_VISITOR_ACTUALS, childValidators);
}

//*****************************

void AST::ValidationVisitor::Handle_UnaryNode (AST_VISITOR_ARGS, const IndexedChildValidator& validator)
	{ Handle_NaryNode(AST_VISITOR_ACTUALS, IndexedChildValidatorList(1, validator)); }

//*****************************

void AST::ValidationVisitor::Handle_NoChildren (AST_VISITOR_ARGS) {
	yysetline();
	if (!VISITOR->ShouldStop() && entering)
		VALIDATE(DPTR(node)->GetTotalChildren() == 0, NO_CHILDREN);
}

//*****************************

void AST::ValidationVisitor::Handle_SingleAttributeNode (AST_VISITOR_ARGS, const char *attribute)
	{ Handle_NaryNode(AST_VISITOR_ACTUALS, IndexedChildValidatorList(), StringList(1, attribute)); }

///////////////////////////////////////////////////////////

bool AST::ValidationVisitor::IsName (TreeNode* node)
	{ return HasTag(DPTR(node), AST_TAG_NAME) || IsMetaGeneratedCode(DPTR(node)); }

bool AST::ValidationVisitor::IsUsingDirective (TreeNode* node) {
	const char *tags[] = { AST_TAG_USING_NAMESPACE, AST_TAG_USING_BYTECODE_LIBRARY };
	return	IsValidTag(DPTR(node)->GetTag(), tags);
}

//*****************************

bool AST::ValidationVisitor::IsFunction (TreeNode* node)
	{ return DPTR(node)->GetTag() == AST_TAG_FUNCTION; }

//*****************************

bool AST::ValidationVisitor::IsStmt (TreeNode* node) {
	const char *tags[] = {
		AST_TAG_BASIC_STMT,
		AST_TAG_COMPOUND,
		AST_TAG_FUNCTION_STMT,
		AST_TAG_EMPTY_STMT,
		AST_TAG_EXECUTE
	};
	return	IsValidTag(DPTR(node)->GetTag(), tags)	||
			IsBasicStmt(DPTR(node))					||
			IsUsingDirective(DPTR(node))			;
}

//*****************************

bool AST::ValidationVisitor::IsBasicStmt (TreeNode* node)
	{ return IsBasicExprStmt(DPTR(node)) || IsBasicNonExprStmt(DPTR(node)); }

//*****************************

bool AST::ValidationVisitor::IsBasicExprStmt (TreeNode* node) {
	const char *tags[] = {
		AST_TAG_EXPRLIST_STMT,
		AST_TAG_ASSERT,
		AST_TAG_RETURN
	};
	return IsValidTag(DPTR(node)->GetTag(), tags);
}

//*****************************

bool AST::ValidationVisitor::IsBasicNonExprStmt (TreeNode* node) {
	const char *tags[] = {
		AST_TAG_WHILE,
		AST_TAG_FOR,
		AST_TAG_FOREACH,
		AST_TAG_IF,
		AST_TAG_IF_ELSE,
		AST_TAG_BREAK,
		AST_TAG_CONTINUE,
		AST_TAG_TRYTRAP,
		AST_TAG_THROW
	};
	return IsValidTag(DPTR(node)->GetTag(), tags);
}

//*****************************

bool AST::ValidationVisitor::IsExpression (TreeNode* node) {
	const char *tags[] = { AST_TAG_CONSTDEF, AST_TAG_TERNARY };
	return	IsValidTag(DPTR(node)->GetTag(), tags)	||
			IsAssignExpression(DPTR(node))			||
			IsRelationalExpression(DPTR(node))		||
			IsBooleanExpression(DPTR(node))			||
			IsArithmeticExpression(DPTR(node))		||
			IsTerm(DPTR(node))						;
}

//*****************************

bool AST::ValidationVisitor::IsAssignExpression (TreeNode* node) {
	const char *tags[] = {
		AST_TAG_ASSIGN,
		AST_TAG_OP_ADD_ASSIGN,
		AST_TAG_OP_MUL_ASSIGN,
		AST_TAG_OP_SUB_ASSIGN,
		AST_TAG_OP_DIV_ASSIGN,
		AST_TAG_OP_MOD_ASSIGN
	};
	return IsValidTag(DPTR(node)->GetTag(), tags);
}

//*****************************

bool AST::ValidationVisitor::IsRelationalExpression (TreeNode* node) {
	const char *tags[] = {
		AST_TAG_OP_GT,
		AST_TAG_OP_LT,
		AST_TAG_OP_GE,
		AST_TAG_OP_LE,
		AST_TAG_OP_EQ,
		AST_TAG_OP_NE
	};
	return IsValidTag(DPTR(node)->GetTag(), tags);
}

//*****************************

bool AST::ValidationVisitor::IsBooleanExpression (TreeNode* node) {
	const char *tags[] = { AST_TAG_AND, AST_TAG_OR };
	return IsValidTag(DPTR(node)->GetTag(), tags);
}

//*****************************

bool AST::ValidationVisitor::IsArithmeticExpression (TreeNode* node) {
	const char *tags[] = {
		AST_TAG_OP_ADD,
		AST_TAG_OP_SUB,
		AST_TAG_OP_MUL,
		AST_TAG_OP_SUB,
		AST_TAG_OP_DIV,
		AST_TAG_OP_MOD
	};
	return IsValidTag(DPTR(node)->GetTag(), tags);
}

//*****************************

bool AST::ValidationVisitor::IsTerm (TreeNode* node) {
	const char *tags[] = {
		AST_LVALUE_PLUSPLUS,
		AST_PLUSPLUS_LVALUE,
		AST_LVALUE_MINUSMINUS,
		AST_MINUSMINUS_LVALUE,
		AST_TAG_UMINUS,
		AST_TAG_NOT,
		AST_TAG_PRIMARY_EXPRESSION
	};
	return	IsValidTag(DPTR(node)->GetTag(), tags)	||
			IsMetaExpression(DPTR(node))			||
			IsMetaGeneratedCode(DPTR(node))			;
}

//*****************************

bool AST::ValidationVisitor::IsPrimary (TreeNode* node) {
	return	IsFunctionAndTableObject(DPTR(node))		||
			IsStringConstUsed(DPTR(node))				||
			IsConstValue(DPTR(node))					||
			IsMetaGeneratedCode(DPTR(node))				||
			HasTag(DPTR(node), AST_TAG_LAMBDA_FUNCTION)	;
}

//*****************************

bool AST::ValidationVisitor::IsConstValue (TreeNode* node) {
	const char *tags[] = {
		AST_TAG_NUM_CONST,
		AST_TAG_NIL,
		AST_TAG_BOOL_CONST
	};
	return IsValidTag(DPTR(node)->GetTag(), tags);
}

//*****************************

bool AST::ValidationVisitor::IsStringConstUsed (TreeNode* node) {
	const char *tags[] = { AST_TAG_STRING_CONST, AST_TAG_STRINGIFY_DOTTED_IDENTS, AST_TAG_STRINGIFY_NAMESPACE_IDENT };
	return IsValidTag(DPTR(node)->GetTag(), tags);
}

//*****************************

bool AST::ValidationVisitor::IsLvalue (TreeNode* node) {
	const char *tags[] = {
		AST_TAG_LVALUE_IDENT,
		AST_TAG_LVALUE_NAMESPACE_IDENT,
		AST_TAG_LVALUE_ATTRIBUTE,
		AST_TAG_LVALUE_STATIC_IDENT,
		AST_TAG_LVALUE_LOCAL_IDENT
	};
	return	IsValidTag(DPTR(node)->GetTag(), tags)	||
			IsTableContent(DPTR(node))				||
			IsMetaGeneratedCode(DPTR(node))			;
}

//*****************************

bool AST::ValidationVisitor::IsTableContent (TreeNode* node) {
	const char *tags[] = {
		AST_TAG_TABLE_CONTENT_DOT,
		AST_TAG_TABLE_CONTENT_DOUBLE_DOT,
		AST_TAG_TABLE_CONTENT_BRACKET,
		AST_TAG_TABLE_CONTENT_DOUBLE_BRACKET
	};
	return IsValidTag(DPTR(node)->GetTag(), tags);
}

//*****************************

bool AST::ValidationVisitor::IsActualArgument (TreeNode* node) {
	const char *tags[] = {
		AST_TAG_ARGUMENT,
		AST_TAG_LATEBOUND_ARGUMENT,
		AST_TAG_TRIPLE_DOT
	};
	return IsValidTag(DPTR(node)->GetTag(), tags) || IsFunctionExpression(DPTR(node));
}

//*****************************

bool AST::ValidationVisitor::IsFunctionCallObject (TreeNode* node)
	{ return IsFunctionAndTableObject(DPTR(node)) || IsStringConstUsed(DPTR(node)); }

//*****************************

bool AST::ValidationVisitor::IsTableElement (TreeNode* node) {
	const char *tags[] = {
		AST_TAG_TABLE_UNINDEXED_ELEM,
		AST_TAG_TABLE_INDEXED_VALUES,
		AST_TAG_TABLE_NEW_ATTRIBUTE,
		AST_TAG_TABLE_IDENTINDEX_ELEM
	};
	return	IsValidTag(DPTR(node)->GetTag(), tags)	||
			IsMetaGeneratedCode(DPTR(node))			;
}

//*****************************

bool AST::ValidationVisitor::IsFunctionAndTableObject (TreeNode* node) {
	const char *tags[] = {
		AST_TAG_SELF,
		AST_TAG_LAMBDA_REF,
		AST_TAG_LVALUE_NEWSELF,
		AST_TAG_ARGUMENTS,
		AST_TAG_TABLE_CONSTRUCTOR,
		AST_TAG_PARENTHESISED_EXPR,
		AST_TAG_FUNCTION_CALL,
		AST_TAG_FUNCTION_PARENTHESIS
	};
	return	IsValidTag(DPTR(node)->GetTag(), tags)	||
			IsLvalue(DPTR(node))					;
}

//*****************************

bool AST::ValidationVisitor::IsDotIndex (TreeNode* node) {
	const char *tags[] = {
		AST_TAG_DOTINDEX_IDENT,
		AST_TAG_DOTINDEX_OPSTRING,
		AST_TAG_DOTINDEX_STRINGCONST
	};
	return	IsValidTag(DPTR(node)->GetTag(), tags);
}

//*****************************

bool AST::ValidationVisitor::IsBracketIndex (TreeNode* node)
	{ return HasTag(DPTR(node), AST_TAG_BRACKETINDEX_OPMETHOD) || IsExpression(DPTR(node)); }

//*****************************

bool AST::ValidationVisitor::IsFunctionExpression (TreeNode* node)
	{ return HasTag(DPTR(node), AST_TAG_FUNCTION_EXPR); }

//*****************************

bool AST::ValidationVisitor::IsUnindexedValue (TreeNode* node)
	{ return IsExpression(DPTR(node)) || IsFunctionExpression(DPTR(node)); }

//*****************************

bool AST::ValidationVisitor::IsIndexExpression (TreeNode* node) {
	const char *tags[] = {
		AST_TAG_TABLE_DOTTED_IDENT,
		AST_TAG_TABLE_OPERATOR_INDEX
	};
	return IsValidTag(DPTR(node)->GetTag(), tags) || IsExpression(DPTR(node));
}

//*****************************

bool AST::ValidationVisitor::IsContentExpression (TreeNode* node)
	{ return IsExpression(DPTR(node)) || IsFunctionExpression(DPTR(node)); }

//*****************************

bool AST::ValidationVisitor::IsMetaGeneratedCode (TreeNode* node) {
	const char *tags[] = {
		AST_TAG_ESCAPE,
		AST_TAG_INLINE
	};
	return IsValidTag(DPTR(node)->GetTag(), tags);
}

//*****************************

bool AST::ValidationVisitor::IsMetaExpression (TreeNode* node)
	{ return HasTag(DPTR(node), AST_TAG_QUASI_QUOTES); }

//*****************************

bool AST::ValidationVisitor::IsQuasiQuoted (TreeNode* node) {
		const char *tags[] = {
		AST_TAG_STMTS,
		AST_TAG_QUOTED_ELEMENTS
	};
	return IsValidTag(DPTR(node)->GetTag(), tags);
}

//*****************************

bool AST::ValidationVisitor::IsQuotedElement (TreeNode* node) {
	const char *tags[] = {
		AST_TAG_TABLE_INDEXED_VALUES,
		AST_TAG_TABLE_NEW_ATTRIBUTE,
		AST_TAG_TABLE_IDENTINDEX_ELEM,
		AST_TAG_TABLE_DOTTED_IDENT,
		AST_TAG_TABLE_OPERATOR_INDEX
	};
	return	IsValidTag(DPTR(node)->GetTag(), tags)	||
			IsActualArgument(DPTR(node))			||
			IsExpression(DPTR(node))				||
			IsFunction(DPTR(node))					;

}

///////////////////////////////////////////////////////////
