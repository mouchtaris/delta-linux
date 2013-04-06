// ASTValidationVisitor.h
// Tree visitor to validate that the AST is well-formed.
// ScriptFighter Project.
// Y. Lilis, July 2011.
//

#ifndef	ASTVALIDATIONVISITOR_H
#define	ASTVALIDATIONVISITOR_H

#include "TreeVisitor.h"
#include "DeltaStdDefs.h"
#include "unestedstate.h"

#include <functional>
#include <map>

/////////////////////////////////////////////////////////

namespace AST {

class SYNTAXTREELIB_CLASS ValidationVisitor : public TreeVisitor {
	private:

	/////////////////////////////////////////////////////////
	// Validation helpers.

	typedef std::list<std::string> StringList;
	
	struct Validator : public std::unary_function<TreeNode*, bool>
		{ virtual result_type operator() (argument_type node) const = 0; };

	struct PtrFuncValidator : public Validator {
		bool (*func)(TreeNode*);
		result_type operator() (argument_type node) const { return (*func)(node); }
		PtrFuncValidator(bool (*func)(TreeNode*)) : func(func) {}
	};
	struct TagValidator : public Validator {
		const std::string tag;
		result_type operator() (argument_type node) const { return DPTR(node)->GetTag() == tag; }
		TagValidator(const std::string& tag) : tag(tag) {}
	};
	struct TagListValidator : public Validator {
		const StringList tags;
		result_type operator() (argument_type node) const
			{ return std::find(tags.begin(), tags.end(), DPTR(node)->GetTag()) != tags.end(); }
		TagListValidator(const StringList& tagList) : tags(tags) {}
	};

	typedef utriple<std::string, const Validator&, std::string> IndexedChildValidator;
	typedef std::list<IndexedChildValidator>					IndexedChildValidatorList;

	/////////////////////////////////////////////////////////
	// Validation handlers.

	static void	Handle_Program (AST_VISITOR_ARGS);
	static void	Handle_Name (AST_VISITOR_ARGS);
	static void Handle_Stmts (AST_VISITOR_ARGS);
	static void Handle_Stmt (AST_VISITOR_ARGS);
	static void Handle_BasicStmt (AST_VISITOR_ARGS);
	static void Handle_FunctionBasic (AST_VISITOR_ARGS, const IndexedChildValidator& bodyValidator);
	static void Handle_Function (AST_VISITOR_ARGS);
	static void Handle_LambdaFunction (AST_VISITOR_ARGS);
	static void Handle_FunctionStmt (AST_VISITOR_ARGS);
	static void Handle_FormalArgs (AST_VISITOR_ARGS);
	static void Handle_Compound (AST_VISITOR_ARGS);
	static void Handle_ExpressionListStmt (AST_VISITOR_ARGS);
	static void Handle_EmptyStmt (AST_VISITOR_ARGS);
	static void Handle_AssertStmt (AST_VISITOR_ARGS);
	static void Handle_Return (AST_VISITOR_ARGS);
	static void Handle_UsingNamespace (AST_VISITOR_ARGS);
	static void Handle_UsingByteCodeLibrary (AST_VISITOR_ARGS);
	static void Handle_ConstDefExpression (AST_VISITOR_ARGS);
	static void Handle_AssignExpr (AST_VISITOR_ARGS);
	static void Handle_BinaryExpr (AST_VISITOR_ARGS);
	static void Handle_UnaryExprExpression (AST_VISITOR_ARGS);
	static void Handle_TermLvalueArith (AST_VISITOR_ARGS);
	static void Handle_Ternary (AST_VISITOR_ARGS);
	static void Handle_PrimaryExpression (AST_VISITOR_ARGS);
	static void Handle_ExprList (AST_VISITOR_ARGS);
	static void Handle_ExprListChild (AST_VISITOR_ARGS, const IndexedChildValidator& validator);
	static void Handle_NumConst (AST_VISITOR_ARGS);
	static void Handle_NilConst (AST_VISITOR_ARGS);
	static void Handle_BoolConst (AST_VISITOR_ARGS);
	static void Handle_StringConst (AST_VISITOR_ARGS);
	static void Handle_StringifyDottedIdents (AST_VISITOR_ARGS);
	static void Handle_StringifyNamespaceIdent (AST_VISITOR_ARGS);
	static void Handle_NamespaceLvalue (AST_VISITOR_ARGS);
	static void Handle_FunctionParenthesisForm (AST_VISITOR_ARGS);
	static void Handle_KwdExpr (AST_VISITOR_ARGS);
	static void Handle_FunctionCall (AST_VISITOR_ARGS);
	static void Handle_Lvalues (AST_VISITOR_ARGS);
	static void Handle_AttrLvalue (AST_VISITOR_ARGS);
	static void Handle_DotIndexIdent (AST_VISITOR_ARGS);
	static void Handle_DotIndexOpString (AST_VISITOR_ARGS);
	static void Handle_DotIndexStringConst (AST_VISITOR_ARGS);
	static void Handle_BracketIndexOperatorMethod (AST_VISITOR_ARGS);
	static void Handle_TableContentDot (AST_VISITOR_ARGS);
	static void Handle_TableContentBracket (AST_VISITOR_ARGS);
	static void Handle_TableConstructor (AST_VISITOR_ARGS);
	static void Handle_UnindexedValue (AST_VISITOR_ARGS);
	static void Handle_IdentIndexElement (AST_VISITOR_ARGS);
	static void Handle_FunctionExpression (AST_VISITOR_ARGS);
	static void Handle_DottedIdent (AST_VISITOR_ARGS);
	static void Handle_OperatorIndex (AST_VISITOR_ARGS);
	static void Handle_TableValues (AST_VISITOR_ARGS);
	static void Handle_TableIndices (AST_VISITOR_ARGS);
	static void Handle_IndexedValues (AST_VISITOR_ARGS);
	static void Handle_NewAttribute (AST_VISITOR_ARGS);
	static void Handle_IfStmt (AST_VISITOR_ARGS);
	static void Handle_IfElseStmt (AST_VISITOR_ARGS);
	static void Handle_WhileStmt (AST_VISITOR_ARGS);
	static void Handle_WhileCondition (AST_VISITOR_ARGS);
	static void Handle_ForStmt (AST_VISITOR_ARGS);
	static void Handle_ForeachStmt (AST_VISITOR_ARGS);
	static void Handle_THROW (AST_VISITOR_ARGS);
	static void Handle_ExceptionVar (AST_VISITOR_ARGS);
	static void Handle_Exception (AST_VISITOR_ARGS);
	static void Handle_Escape (AST_VISITOR_ARGS);
	static void Handle_QuotedElements (AST_VISITOR_ARGS);
	static void Handle_QuasiQuotes (AST_VISITOR_ARGS);
	static void Handle_MetaExpr (AST_VISITOR_ARGS);
	static void Handle_Execute (AST_VISITOR_ARGS);
	static void Handle_Inline (AST_VISITOR_ARGS);

	/////////////////////////////////////////////////////////
	// Translation helpers.

	static void Handle_ListNode (AST_VISITOR_ARGS, const IndexedChildValidator& validator);
	static void Handle_NaryNode (
					AST_VISITOR_ARGS,
					const IndexedChildValidatorList& childValidators,
					const StringList& stringAttributes = StringList()
				);
	static void Handle_TriadicNode (
					AST_VISITOR_ARGS,
					const IndexedChildValidator& first,
					const IndexedChildValidator& second,
					const IndexedChildValidator& third
				);

	static void Handle_BinaryNode (AST_VISITOR_ARGS, const IndexedChildValidator& left, const IndexedChildValidator& right);
	static void Handle_UnaryNode (AST_VISITOR_ARGS, const IndexedChildValidator& validator);

	static void Handle_NoChildren (AST_VISITOR_ARGS);
	static void Handle_SingleAttributeNode (AST_VISITOR_ARGS, const char *attribute);
	
	static bool HasTag (TreeNode* node, const std::string& tag)
		{ return DPTR(node)->GetTag() == tag; }

	template <class T, const util_ui32 N> static bool IsValidTag (const std::string& tag, T (&tags)[N])
		{ return std::find(tags, tags + N, tag) != tags + N; }


	/////////////////////////////////////////////////////////
	// Private data
	unestedstate<DELTA_MAXNESTED_QUASI_QUOTES>	quotes;
	bool										allowEmptyInlines;
	bool										allowRenames;
	std::string									error;

	typedef util_ui32 (*LineGetter)(TreeNode*);
	LineGetter lineGetter;

	typedef ucallbackwithclosure<void (*)(util_ui32, void*)> LineSetter;
	LineSetter lineSetter;

	typedef bool (*FuncValidator)(TreeNode*);
	typedef std::map<FuncValidator, std::string> FuncValidatorMessages;
	FuncValidatorMessages funcValidatorMessages;

	const std::string& GetFuncValidatorMessage(FuncValidator func) const {
		FuncValidatorMessages::const_iterator i = funcValidatorMessages.find(func);
		DASSERT(i != funcValidatorMessages.end());
		return i->second;
	}

	/////////////////////////////////////////////////////////

	public:

	static bool IsName (TreeNode* node);
	static bool IsUsingDirective (TreeNode* node);
	static bool IsFunction (TreeNode* node);
	static bool IsStmt (TreeNode* node);
	static bool IsBasicStmt (TreeNode* node);
	static bool IsBasicExprStmt (TreeNode* node);
	static bool IsBasicNonExprStmt (TreeNode* node);
	static bool IsExpression (TreeNode* node);
	static bool IsAssignExpression (TreeNode* node);
	static bool IsRelationalExpression (TreeNode* node);
	static bool IsBooleanExpression (TreeNode* node);
	static bool IsArithmeticExpression (TreeNode* node);
	static bool IsTerm (TreeNode* node);
	static bool IsPrimary (TreeNode* node);
	static bool IsConstValue (TreeNode* node);
	static bool IsStringConstUsed (TreeNode* node);
	static bool IsLvalue (TreeNode* node);
	static bool IsTableContent (TreeNode* node);
	static bool IsActualArgument (TreeNode* node);
	static bool IsFunctionCallObject (TreeNode* node);
	static bool IsTableElement (TreeNode* node);
	static bool IsFunctionAndTableObject (TreeNode* node);
	static bool IsDotIndex (TreeNode* node);
	static bool IsBracketIndex (TreeNode* node);
	static bool IsFunctionExpression (TreeNode* node);
	static bool IsUnindexedValue (TreeNode* node);
	static bool IsIndexExpression (TreeNode* node);
	static bool IsContentExpression (TreeNode* node);
	static bool IsMetaGeneratedCode (TreeNode* node);
	static bool IsMetaExpression (TreeNode* node);
	static bool IsQuasiQuoted (TreeNode* node);
	static bool IsQuotedElement (TreeNode* node);

	bool operator()(TreeNode* root);

	void EnterQuotes (void) { quotes.enter(); }
	void SetAllowEmptyInlines (bool val = true) { allowEmptyInlines = val; }
	void SetAllowRenames (bool val = true) { allowRenames = val; }

	const std::string&	GetValidationError (void) const { return error; }

	ValidationVisitor (
		bool allowEmptyInlines = false,
		LineGetter lineGetter = (LineGetter) 0,
		LineSetter lineSetter = LineSetter()
	);
	~ValidationVisitor();
};

}

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
