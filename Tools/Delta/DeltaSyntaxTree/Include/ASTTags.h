// ASTTags.h
// Tags for nodes of Delta ASTs.
// ScriptFighter Project.
// A. Savidis, September 2009.
//
 
#ifndef	ASTTAGS_H
#define	ASTTAGS_H

/////////////////////////////////////////////////////////
// NODE TAGS (TYPES).
//
#define	AST_TAG_PROGRAM							"Program"
#define	AST_TAG_STMTS							"Stmts"
#define	AST_TAG_STMT							"Stmt"
#define	AST_TAG_BASIC_STMT						"BasicStmt"
#define	AST_TAG_NAME							"Name"
#define	AST_TAG_FUNCTION						"Function"
#define	AST_TAG_LAMBDA_FUNCTION					"LambdaFunction"
#define	AST_TAG_FUNCTION_STMT					"FunctionStmt"
#define	AST_TAG_FUNCTION_PARENTHESIS			"(Function)"
#define	AST_TAG_FORMAL_ARGS						"FormalArgs"
#define	AST_TAG_COMPOUND						"Compound"
#define	AST_TAG_EXPRLIST_STMT					"ExprListStmt"
#define	AST_TAG_EMPTY_STMT						"EmptyStmt"
#define	AST_TAG_USING_NAMESPACE					"UsingNamespace"
#define	AST_TAG_USING_BYTECODE_LIBRARY			"UsingByteCodeLibrary"
#define	AST_TAG_FUNCTION_EXPR					"FunctionExpr"
#define	AST_TAG_PARENTHESISED_EXPR				"ParenthesisedExpr"					
#define	AST_TAG_CONSTDEF						"ConstDef"

#define	AST_TAG_ASSIGN							"AssignOp"
#define	AST_TAG_OP_ADD							"AddOp"
#define	AST_TAG_OP_SUB							"SubOp"
#define	AST_TAG_OP_MUL							"MulOp"
#define	AST_TAG_OP_DIV							"DivOp"
#define	AST_TAG_OP_MOD							"ModOp"
#define	AST_TAG_OP_ADD_ASSIGN					"AddAssignOp"
#define	AST_TAG_OP_SUB_ASSIGN					"SubAssignOp"
#define	AST_TAG_OP_MUL_ASSIGN					"MulAssignOp"
#define	AST_TAG_OP_DIV_ASSIGN					"DivAssignOp"
#define	AST_TAG_OP_MOD_ASSIGN					"ModAssignOp"
#define	AST_TAG_OP_GT							"GreaterThanOp"
#define	AST_TAG_OP_LT							"LessThanOp"
#define	AST_TAG_OP_GE							"GreaterEqualOp"
#define	AST_TAG_OP_LE							"LessQualOp"
#define	AST_TAG_OP_EQ							"EqualOp"
#define	AST_TAG_OP_NE							"NotEqualOp"
#define	AST_TAG_AND								"and"
#define	AST_TAG_OR								"or"
#define	AST_TAG_NOT								"not"

#define	AST_TAG_UMINUS							"UnaryMinus"
#define	AST_TAG_TERNARY							"Ternary"
#define	AST_LVALUE_PLUSPLUS						"Lvalue++"
#define	AST_LVALUE_MINUSMINUS					"Lvalue--"
#define	AST_PLUSPLUS_LVALUE						"++Lvalue"
#define	AST_MINUSMINUS_LVALUE					"--Lvalue"
#define AST_TAG_PRIMARY_EXPRESSION				"PrimaryExpression"

#define	AST_TAG_NUM_CONST						"NumConst"
#define	AST_TAG_BOOL_CONST						"BoolConst"
#define	AST_TAG_STRING_CONST					"StringConst"
#define	AST_TAG_STRINGIFY_DOTTED_IDENTS			"StringifyDottedIdents"
#define	AST_TAG_STRINGIFY_NAMESPACE_IDENT		"StringifyNamespaceIdent"

#define	AST_TAG_NIL								"nil"
#define	AST_TAG_SELF							"self"
#define	AST_TAG_ARGUMENTS						"arguments"
#define	AST_TAG_TRIPLE_DOT						"..."
#define	AST_TAG_BREAK							"break"
#define	AST_TAG_CONTINUE						"continue"
#define	AST_TAG_ASSERT							"assert"
#define	AST_TAG_RETURN							"return"
#define	AST_TAG_LAMBDA_REF						"@lambda"
#define	AST_TAG_LVALUE_NEWSELF					"@self"

#define	AST_TAG_LATEBOUND_ARGUMENT				"LateBoundArgument"
#define	AST_TAG_ARGUMENT						"Argument"
#define	AST_TAG_FUNCTION_CALL					"FunctionCall"
#define	AST_TAG_EXPR_LIST						"ExprList"
#define	AST_TAG_EXPR							"Expr"
#define	AST_TAG_EXPRLIST_ITEM					"ExprListItem"

#define	AST_TAG_LVALUE_IDENT					"LvalueIdent"
#define	AST_TAG_LVALUE_LOCAL_IDENT				"LvalueLocalIdent"
#define	AST_TAG_LVALUE_STATIC_IDENT				"LvalueStaticIdent"
#define	AST_TAG_LVALUE_NAMESPACE_IDENT			"LvalueNamespaceIdent"
#define	AST_TAG_LVALUE_ATTRIBUTE				"LvalueAttribute"

#define	AST_TAG_DOTINDEX_IDENT					"DotIndexIdent"
#define	AST_TAG_DOTINDEX_OPSTRING				"DotIndexOpString"
#define	AST_TAG_DOTINDEX_STRINGCONST			"DotIndexStringConst"
#define	AST_TAG_BRACKETINDEX_OPMETHOD			"BracketIndexOpMethod"
#define	AST_TAG_TABLE_DOTTED_IDENT				"DottedIdent"
#define	AST_TAG_TABLE_OPERATOR_INDEX			"OperatorIndex"
#define	AST_TAG_TABLE_CONTENT_DOT				"TableContentDot"
#define	AST_TAG_TABLE_CONTENT_DOUBLE_DOT		"TableContentDoubleDot"
#define	AST_TAG_TABLE_CONTENT_BRACKET			"TableContentBracket"
#define	AST_TAG_TABLE_CONTENT_DOUBLE_BRACKET	"TableContentDoubleBracket"

#define	AST_TAG_TABLE_CONSTRUCTOR				"TableConstructor"
#define	AST_TAG_TABLE_UNINDEXED_ELEM			"UnindexedElem"
#define	AST_TAG_TABLE_ATTRIBUTE_IDENT			"AttributeIdent"
#define	AST_TAG_TABLE_NEW_ATTRIBUTE				"NewAttribute"
#define	AST_TAG_TABLE_IDENTINDEX_ELEM			"IdentIndexElem"
#define	AST_TAG_TABLE_INDEXED_VALUES			"IndexedValues"
#define	AST_TAG_TABLE_INDICES					"TableIndices"
#define	AST_TAG_TABLE_VALUES					"TableValues"

#define	AST_TAG_IF								"IfStmt"
#define	AST_TAG_IF_ELSE							"IfElseStmt"
#define	AST_TAG_WHILE							"WhileStmt"
#define	AST_TAG_FOR								"ForStmt"
#define	AST_TAG_FOREACH							"ForeachStmt"
#define	AST_TAG_THROW							"ThrowStmt"
#define	AST_TAG_EXCEPTION_VAR					"ExceptionVar"
#define	AST_TAG_TRYTRAP							"TryTrap"

/////////////////////////////////////////////////////////
// NODE ATTRIBUTES AND CHILD NAMES (FIELDS).
//
#define	AST_ATTRIBUTE_NAME						"name"
#define	AST_ATTRIBUTE_RENAME					"rename"
#define	AST_ATTRIBUTE_CLASS						"class"
#define	AST_ATTRIBUTE_LINKAGE					"linkage"
#define	AST_ATTRIBUTE_CONSTVALUE				"constvalue"
#define	AST_ATTRIBUTE_ITEMS						"items"
#define	AST_ATTRIBUTE_VALUE						"value"

#define	NAME(node)			\
		(DPTR(DPTR(node)->GetAttribute(AST_ATTRIBUTE_NAME))->GetString())

#define	VALUE_STRING(node)	\
		(DPTR(DPTR(node)->GetAttribute(AST_ATTRIBUTE_VALUE))->GetString())

#define	AST_CHILD_BODY							"body"
#define	AST_CHILD_FUNCTION						"function"
#define	AST_CHILD_FORMALS						"formals"
#define	AST_CHILD_ACTUALS						"actuals"
#define	AST_CHILD_NAME							"name"
#define	AST_CHILD_NAMESPACE						"namespace"
#define	AST_CHILD_EXPR							"expr"
#define	AST_CHILD_STMT							"stmt"
#define	AST_CHILD_STMTS							"stmts"
#define	AST_CHILD_IF							"if"
#define	AST_CHILD_INIT							"init"
#define	AST_CHILD_SUFFIX						"suffix"
#define	AST_CHILD_ELSE							"else"
#define	AST_CHILD_PREFIX						"prefix"
#define	AST_CHILD_LVALUE						"lvalue"
#define	AST_CHILD_RVALUE						"rvalue"
#define	AST_CHILD_LEFT							"left"
#define	AST_CHILD_RIGHT							"right"
#define	AST_CHILD_COND							"cond"
#define	AST_CHILD_TABLE							"table"
#define	AST_CHILD_INDEX							"index"
#define	AST_CHILD_INDICES						"indices"
#define	AST_CHILD_VALUES						"values"
#define	AST_CHILD_CONTAINER						"container"
#define	AST_CHILD_VAR							"var"
#define	AST_CHILD_TRY							"try"
#define	AST_CHILD_TRAP							"trap"
#define	AST_CHILD_SET							"set"
#define	AST_CHILD_GET							"get"

/////////////////////////////////////////////////////////
// NODE ATTRIBUTES VALUES (ENUMERATED).
//
#define	AST_VALUE_FUNCCLASS_PROGRAMFUNCTION		"Function"
#define	AST_VALUE_FUNCCLASS_ONEVENT				"OnEvent"
#define	AST_VALUE_FUNCCLASS_METHOD				"Method"
#define	AST_VALUE_FUNCLINKAGE_NO_EXPORT			"NoExport"
#define	AST_VALUE_FUNCLINKAGE_DEFAULT_EXPORT	"DefaultExport"

#define	AST_VALUE_TOSTRING_SYMBOLIC_NAME		"@"
#define	AST_VALUE_VARARGS_FORMAL_NAME			"..."

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

