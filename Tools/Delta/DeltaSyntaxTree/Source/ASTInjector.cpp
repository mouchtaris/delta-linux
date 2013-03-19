// ASTInjector.cpp
// AST injection logic for escapes and inlines.
// ScriptFighter Project.
// Y. Lilis, August 2011.
//

#include "ASTInjector.h"
#include "ASTValidationVisitor.h"
#include "ASTTags.h"
#include "ASTMetaTags.h"

///////////////////////////////////////////////////////////

TreeNode* ASTInjector::Conversion::Extend (TreeNode* child, const std::string& tag, const std::string& childId) const {
	TreeNode* node = Create(tag);
	DPTR(node)->PushBack(child, childId);
	return node;
}

//*****************************

TreeNode* ASTInjector::Conversion::GetSingleNode(TreeNode* node) const {
	TreeNode* child = DPTR(node)->GetChild(0);
	DASSERT(child);
	DPTR(child)->RemoveFromParent();
	Delete(node);
	return child;
}

//*****************************

void ASTInjector::Conversion::DefaultReplace (TreeNode* target, TreeNode* node) const {
	TreeNode* parent = DPTR(target)->GetParent();
	DASSERT(parent);
	DPTR(parent)->Replace(target, node);
	Delete(target);
}

///////////////////////////////////////////////////////////

#define CONVERSION_CLASS_EX(name, code)																\
	class name : public ASTInjector::Conversion {													\
		public:																						\
		bool IsValid	(TreeNode* target, TreeNode* node) const;									\
		void operator()	(TreeNode* target, TreeNode* node, TreeNode** newAst) const;				\
		code																						\
		name (NodeCreatorFunc creator, NodeDeleterFunc deleter) : Conversion(creator, deleter) {}	\
	}

#define CONVERSION_CLASS(name)	CONVERSION_CLASS_EX(name, UEMPTY)

/////////////////////////////////////////////////////////
// Utility functions
//

static bool IsList (TreeNode* node, bool (*func)(TreeNode*)) {
	const util_ui32 n = DPTR(node)->GetTotalChildren();
	for (util_ui32 i = 0; i < n; ++i)
		if (!(*func)(DPTR(node)->GetChild(i)))
			return false;
	return true;
}

//*****************************

static bool IsSingleNode(TreeNode* node, bool (*func)(TreeNode*)) {
	TreeNode* child;
	return	node								&&
			DPTR(node)->GetTotalChildren() == 1	&&
			(child = DPTR(node)->GetChild(0))	&&
			(*func)(child)						;
}

/////////////////////////////////////////////////////////
// ExpressionConversion
//
CONVERSION_CLASS_EX(
	ExpressionConversion,
	static bool IsConvertibleToExpression(TreeNode* node);
	TreeNode* ToExpression(TreeNode* node) const;
);

//*****************************

bool ExpressionConversion::IsValid (TreeNode* target, TreeNode* node) const
	{ return target && DPTR(target)->GetParent() && node && IsConvertibleToExpression(node); }

//*****************************

void ExpressionConversion::operator() (TreeNode* target, TreeNode* node, TreeNode**) const {
	DASSERT(IsValid(target, node) && target);
	DefaultReplace(target, ToExpression(node));
}

//*****************************
 
bool ExpressionConversion::IsConvertibleToExpression(TreeNode* node) {
	return	AST::ValidationVisitor::IsExpression(node)					||
			AST::ValidationVisitor::IsPrimary(node)						||
			IsSingleNode(node, &AST::ValidationVisitor::IsExpression)	||
			IsSingleNode(node, &AST::ValidationVisitor::IsPrimary)		;
}

//*****************************

TreeNode* ExpressionConversion::ToExpression(TreeNode* node) const {

	if (!AST::ValidationVisitor::IsExpression(node) && !AST::ValidationVisitor::IsPrimary(node))
		node = GetSingleNode(node);

	//Metagenerated code (i.e. inlines & escapes) should never have primary parent
	bool primary = !AST::ValidationVisitor::IsMetaGeneratedCode(node);

	if (primary && AST::ValidationVisitor::IsPrimary(node))
		node = Extend(node, AST_TAG_PRIMARY_EXPRESSION, AST_CHILD_EXPR);
	else if (DPTR(node)->GetTotalChildren() > 1) {	//use extra () to maintain operator precedence
		node = Extend(node, AST_TAG_PARENTHESISED_EXPR, AST_CHILD_EXPR);
		if (primary)
			node = Extend(node, AST_TAG_PRIMARY_EXPRESSION, AST_CHILD_EXPR);
	}
	return node;
}

/////////////////////////////////////////////////////////
// LvalueConversion
//
CONVERSION_CLASS_EX(
	LvalueConversion,
	static bool IsLvalueExpression(TreeNode* node);
);

//*****************************

bool LvalueConversion::IsLvalueExpression(TreeNode* node) {
	TreeNode* child;
	return	node												&&
			DPTR(node)->GetTag() == AST_TAG_PRIMARY_EXPRESSION	&&
			(child = DPTR(node)->GetChild(0))					&&
			AST::ValidationVisitor::IsLvalue(child)				;
}

//*****************************

bool LvalueConversion::IsValid (TreeNode* target, TreeNode* node) const {
	return	node &&
			(
				AST::ValidationVisitor::IsLvalue(node)		||
				IsLvalueExpression(node)					||
				IsSingleNode(node, &IsLvalueExpression)
			);
}

//*****************************

void LvalueConversion::operator() (TreeNode* target, TreeNode* node, TreeNode**) const {
	DASSERT(IsValid(target, node) && target);
	util_ui32 depth = 0;
	if (!AST::ValidationVisitor::IsLvalue(node))
		depth = IsLvalueExpression(node) ? 1 : 2;
	for (util_ui32 i = 0; i < depth; ++i)
		node = GetSingleNode(node);
	DefaultReplace(target, node);
}

/////////////////////////////////////////////////////////
// NameConversion
//
CONVERSION_CLASS_EX(
	NameConversion,
	static bool IsNameExpression(TreeNode* node);
	static bool IsConvertibleToName(TreeNode* node);
	TreeNode* ToName(TreeNode* node) const;
);

//*****************************

bool NameConversion::IsNameExpression(TreeNode* node) {
	TreeNode* child;
	return	node												&&
			DPTR(node)->GetTag() == AST_TAG_PRIMARY_EXPRESSION	&&
			(child = DPTR(node)->GetChild(0))					&&
			DPTR(child)->GetTag() == AST_TAG_LVALUE_IDENT		&&
			(child = DPTR(child)->GetChild(0))					&&
			DPTR(child)->GetTag() == AST_TAG_NAME				;
}

//*****************************

bool NameConversion::IsConvertibleToName(TreeNode* node) {
	return	AST::ValidationVisitor::IsName(node)	||
			IsNameExpression(node)					||
			IsSingleNode(node, &IsNameExpression)	;
}

//*****************************

TreeNode* NameConversion::ToName(TreeNode* node) const {
	util_ui32 depth = 0;
	if (!AST::ValidationVisitor::IsName(node))
		depth = IsNameExpression(node) ? 2 : 3;
	for (util_ui32 i = 0; i < depth; ++i)
		node = GetSingleNode(node);
	return node;
}

//*****************************

bool NameConversion::IsValid (TreeNode* target, TreeNode* node) const
	{ return node && IsConvertibleToName(node); }

//*****************************

void NameConversion::operator() (TreeNode* target, TreeNode* node, TreeNode**) const {
	DASSERT(IsValid(target, node) && target);
	DefaultReplace(target, ToName(node));
}

/////////////////////////////////////////////////////////
// NamespacePathConversion
//
CONVERSION_CLASS_EX(
	NamespacePathConversion,
	static bool IsNamespaceExpression (TreeNode* node);
	static void HandleSingleItem (TreeNode* target, TreeNode* node);
);

//*****************************

bool NamespacePathConversion::IsNamespaceExpression(TreeNode* node) {
	TreeNode* child;
	return	node													&&
			DPTR(node)->GetTag() == AST_TAG_PRIMARY_EXPRESSION		&&
			(child = DPTR(node)->GetChild(0))						&&
			DPTR(child)->GetTag() == AST_TAG_LVALUE_NAMESPACE_IDENT	;
}

//*****************************

bool NamespacePathConversion::IsValid (TreeNode* target, TreeNode* node) const {
	return	node &&
			(
				NameConversion::IsConvertibleToName(node)				||
				DPTR(node)->GetTag() == AST_TAG_LVALUE_NAMESPACE_IDENT	||
				IsNamespaceExpression(node)								||
				IsSingleNode(node, &IsNamespaceExpression)
			);
}

//*****************************

void NamespacePathConversion::HandleSingleItem (TreeNode* target, TreeNode* node) {
	TreeNode* parent = DPTR(target)->GetParent();
	DASSERT(parent && 
		(
			DPTR(parent)->GetTag() == AST_TAG_USING_NAMESPACE			||
			DPTR(parent)->GetTag() == AST_TAG_LVALUE_NAMESPACE_IDENT	||
			DPTR(parent)->GetTag() == AST_TAG_STRINGIFY_NAMESPACE_IDENT
		)
	);
	DPTR(parent)->InsertBefore(target, node);
}

void NamespacePathConversion::operator() (TreeNode* target, TreeNode* node, TreeNode**) const {
	DASSERT(IsValid(target, node) && target);
	if (NameConversion::IsConvertibleToName(node))
		HandleSingleItem(target, NameConversion(creator, deleter).ToName(node));
	else {
		util_ui32 depth = 0;	//AST_TAG_LVALUE_NAMESPACE_IDENT
		if (IsNamespaceExpression(node))
			depth = 1;
		else if (IsSingleNode(node, &IsNamespaceExpression))
			depth = 2;
		for (util_ui32 i = 0; i < depth; ++i)
			node = GetSingleNode(node);
		while (DPTR(node)->GetTotalChildren()) {
			TreeNode* child = DPTR(node)->GetChild(0);
			DASSERT(child);
			child->RemoveFromParent();
			HandleSingleItem(target, child);
		}
		Delete(node);
	}
	Delete(target);
}

/////////////////////////////////////////////////////////
// ContentExpressionConversion
//
CONVERSION_CLASS_EX(
	ContentExpressionConversion,
	static bool IsContentExpression (TreeNode* node);
	static bool IsConvertibleToContent(TreeNode* node);
	TreeNode* ToContent(TreeNode* node) const;
);

//*****************************

bool ContentExpressionConversion::IsContentExpression (TreeNode* node) {
	return	AST::ValidationVisitor::IsExpression(node)	||
			AST::ValidationVisitor::IsFunction(node)	;
}

//*****************************

bool ContentExpressionConversion::IsConvertibleToContent (TreeNode* node)
	{ return IsContentExpression(node) || IsSingleNode(node, &IsContentExpression); }

//*****************************

TreeNode* ContentExpressionConversion::ToContent(TreeNode* node) const {
	if (!IsContentExpression(node))
		node = GetSingleNode(node);
	if (!AST::ValidationVisitor::IsExpression(node))
		node = Extend(node, AST_TAG_FUNCTION_EXPR, AST_CHILD_EXPR);
	return node;
}

//*****************************

bool ContentExpressionConversion::IsValid (TreeNode* target, TreeNode* node) const
	{ return node && IsConvertibleToContent(node); }

//*****************************

void ContentExpressionConversion::operator() (TreeNode* target, TreeNode* node, TreeNode**) const {
	DASSERT(IsValid(target, node) && target);
	DefaultReplace(target, ToContent(node));
}

/////////////////////////////////////////////////////////
// FunctionAndTableConversion
//
CONVERSION_CLASS(FunctionAndTableConversion);

//*****************************

bool FunctionAndTableConversion::IsValid (TreeNode* target, TreeNode* node) const {
	return	node &&
			(
				AST::ValidationVisitor::IsName(node)						||
				AST::ValidationVisitor::IsLvalue(node)						||
				AST::ValidationVisitor::IsExpression(node)					||
				IsSingleNode(node, &AST::ValidationVisitor::IsExpression)
			);
}

//*****************************

void FunctionAndTableConversion::operator() (TreeNode* target, TreeNode* node, TreeNode**) const {
	DASSERT(IsValid(target, node) && target);
	if (AST::ValidationVisitor::IsName(node))
		node = Extend(node, AST_TAG_LVALUE_IDENT, AST_CHILD_NAME);
	if (AST::ValidationVisitor::IsLvalue(node))
		node = Extend(node, AST_TAG_PRIMARY_EXPRESSION, AST_CHILD_EXPR);
	if (!AST::ValidationVisitor::IsExpression(node))
		node = GetSingleNode(node);
	DefaultReplace(target, Extend(node, AST_TAG_PARENTHESISED_EXPR, AST_CHILD_EXPR));
}

/////////////////////////////////////////////////////////
// ItemListConversion
//
class ItemListConversion : public ASTInjector::Conversion {
	protected:
	bool (*isItem)(TreeNode*);
	const bool allowNullNode;
	
	public:
	bool IsValid	(TreeNode* target, TreeNode* node) const;
	void operator()	(TreeNode* target, TreeNode* node, TreeNode**) const;

	virtual TreeNode*	GetFinalTarget	(TreeNode* target) const = 0;
	virtual void		HandleItem		(TreeNode* target, TreeNode* node) const = 0;

	ItemListConversion (NodeCreatorFunc creator, NodeDeleterFunc deleter, bool (*isItem)(TreeNode*), bool allowNullNode) :
		Conversion(creator, deleter), isItem(isItem), allowNullNode(allowNullNode) {}
};

//*****************************

bool ItemListConversion::IsValid (TreeNode* target, TreeNode* node) const {
	if (!allowNullNode && !node)
		return false;
	return	!node || (*isItem)(node) || IsList(node, isItem);
}

//*****************************

void ItemListConversion::operator() (TreeNode* target, TreeNode* node, TreeNode**) const {
	DASSERT(IsValid(target, node) && target);
	target = GetFinalTarget(target);
	DASSERT(target);
	if (node && (*isItem)(node))
		HandleItem(target, node);
	else {
		while (node && DPTR(node)->GetTotalChildren()) {
			TreeNode* child = DPTR(node)->GetChild(0);
			DASSERT(child && (*isItem)(child));
			child->RemoveFromParent();
			HandleItem(target, child);
		}
		if (node)
			Delete(node);
	}
	Delete(target);
}

/////////////////////////////////////////////////////////
// ArgumentListConversion
//

class ArgumentListConversion : public ItemListConversion {
	private:
	static bool IsArgument(TreeNode* node);

	public:
	TreeNode*	GetFinalTarget	(TreeNode* target) const;
	void		HandleItem		(TreeNode* target, TreeNode* node) const;

	ArgumentListConversion (NodeCreatorFunc creator, NodeDeleterFunc deleter) :
		ItemListConversion(creator, deleter, &IsArgument, true) {}
};

//*****************************

bool ArgumentListConversion::IsArgument(TreeNode* node) {
	DASSERT(node);
	const std::string& tag = DPTR(node)->GetTag();	
	return	tag == AST_TAG_TRIPLE_DOT					||
			tag == AST_TAG_LATEBOUND_ARGUMENT			||
			AST::ValidationVisitor::IsExpression(node)	||
			AST::ValidationVisitor::IsFunction(node)	;
}

//*****************************

TreeNode* ArgumentListConversion::GetFinalTarget (TreeNode* target) const {
	DASSERT(target);
	TreeNode* parent = DPTR(target)->GetParent();
	DASSERT(parent && DPTR(parent)->GetTag() == AST_TAG_ARGUMENT);
	parent = DPTR(parent)->GetParent();
	DASSERT(parent && DPTR(parent)->GetTag() == AST_TAG_EXPRLIST_ITEM);
	return parent;
}

//*****************************

void ArgumentListConversion::HandleItem(TreeNode* target, TreeNode* node) const {
	DASSERT(target && node);
	TreeNode* child = DPTR(node);
	if (AST::ValidationVisitor::IsExpression(child))
		child = Extend(child, AST_TAG_ARGUMENT, AST_CHILD_EXPR);
	else if (AST::ValidationVisitor::IsFunction(child))
		child = Extend(child, AST_TAG_FUNCTION_EXPR, AST_CHILD_EXPR);
	child = Extend(child, AST_TAG_EXPRLIST_ITEM, AST_CHILD_EXPR);

	TreeNode* parent = DPTR(target)->GetParent();
	DASSERT(parent && DPTR(parent)->GetTag() == AST_TAG_EXPR_LIST);
	DPTR(parent)->InsertBefore(target, child);
}

/////////////////////////////////////////////////////////
// ExpressionListConversion
//
class ExpressionListConversion : public ItemListConversion {
	public:
	TreeNode*	GetFinalTarget	(TreeNode* target) const;
	void		HandleItem		(TreeNode* target, TreeNode* node) const;

	ExpressionListConversion (NodeCreatorFunc creator, NodeDeleterFunc deleter) :
		ItemListConversion(creator, deleter, &AST::ValidationVisitor::IsExpression, true) {}
};

//*****************************

TreeNode* ExpressionListConversion::GetFinalTarget (TreeNode* target) const {
	DASSERT(target);
	TreeNode* parent = DPTR(target)->GetParent();
	DASSERT(parent);
	DASSERT(DPTR(parent)->GetTag() == AST_TAG_EXPRLIST_ITEM || DPTR(parent)->GetTag() == AST_TAG_EXPR);
	return parent;
}

//*****************************

void ExpressionListConversion::HandleItem(TreeNode* target, TreeNode* node) const {
	DASSERT(target && node);
	TreeNode* parent = DPTR(target)->GetParent();
	DASSERT(parent);
	const std::string tag = DPTR(parent)->GetTag();
	DASSERT(tag == AST_TAG_EXPR_LIST || tag == AST_TAG_EXPRLIST_STMT);
	DPTR(parent)->InsertBefore(
		target,
		Extend(node, tag == AST_TAG_EXPR_LIST ? AST_TAG_EXPRLIST_ITEM : AST_TAG_EXPR, AST_CHILD_EXPR)
	);
}

/////////////////////////////////////////////////////////
// FormalsConversion
//
class FormalsConversion : public ItemListConversion {
	public:
	TreeNode*	GetFinalTarget	(TreeNode* target) const { return target; }
	void		HandleItem		(TreeNode* target, TreeNode* node) const;

	FormalsConversion (NodeCreatorFunc creator, NodeDeleterFunc deleter) :
		ItemListConversion(creator, deleter, &NameConversion::IsConvertibleToName, true) {}
};

//*****************************

void FormalsConversion::HandleItem(TreeNode* target, TreeNode* node) const {
	DASSERT(target && node);
	TreeNode* parent = DPTR(target)->GetParent();
	DASSERT(parent && DPTR(parent)->GetTag() == AST_TAG_FORMAL_ARGS);
	DPTR(parent)->InsertBefore(target, NameConversion(creator, deleter).ToName(node));
}

/////////////////////////////////////////////////////////
// TableElementListConversion
//
class TableElementListConversion : public ItemListConversion {
	private:
	static bool IsTableElement (TreeNode* node);

	public:
	TreeNode*	GetFinalTarget	(TreeNode* target) const { return target; }
	void		HandleItem		(TreeNode* target, TreeNode* node) const;

	TableElementListConversion (NodeCreatorFunc creator, NodeDeleterFunc deleter) :
		ItemListConversion(creator, deleter, &IsTableElement, false) {}
};

//*****************************

bool TableElementListConversion::IsTableElement(TreeNode* node) {
	DASSERT(node);
	return	AST::ValidationVisitor::IsTableElement(node)	||
			AST::ValidationVisitor::IsExpression(node)		||
			AST::ValidationVisitor::IsFunction(node)		;
}

//*****************************

void TableElementListConversion::HandleItem(TreeNode* target, TreeNode* node) const {
	DASSERT(target && node);
	TreeNode* child = DPTR(node);
	if (AST::ValidationVisitor::IsExpression(child))
		child = Extend(child, AST_TAG_TABLE_UNINDEXED_ELEM, AST_CHILD_EXPR);
	else if (AST::ValidationVisitor::IsFunction(child))
		child = Extend(
			Extend(child, AST_TAG_FUNCTION_EXPR, AST_CHILD_EXPR),
			AST_TAG_TABLE_UNINDEXED_ELEM,
			AST_CHILD_EXPR
		);
	TreeNode* parent = DPTR(target)->GetParent();
	DASSERT(parent && DPTR(parent)->GetTag() == AST_TAG_TABLE_CONSTRUCTOR);
	DPTR(parent)->InsertBefore(target, child);
}

/////////////////////////////////////////////////////////
// TableIndexListConversion
//
class TableIndexListConversion : public ItemListConversion {
	public:
	TreeNode*	GetFinalTarget	(TreeNode* target) const;
	void		HandleItem		(TreeNode* target, TreeNode* node) const;

	TableIndexListConversion (NodeCreatorFunc creator, NodeDeleterFunc deleter) :
		ItemListConversion(creator, deleter, &AST::ValidationVisitor::IsIndexExpression, true) {}
};

//*****************************

TreeNode* TableIndexListConversion::GetFinalTarget (TreeNode* target) const {
	DASSERT(target);
	TreeNode* parent = DPTR(target)->GetParent();
	DASSERT(parent);
	DASSERT(DPTR(parent)->GetTag() == AST_TAG_EXPRLIST_ITEM);
	return parent;
}

//*****************************

void TableIndexListConversion::HandleItem(TreeNode* target, TreeNode* node) const {
	DASSERT(target && node);
	TreeNode* parent = DPTR(target)->GetParent();
	DASSERT(parent && DPTR(parent)->GetTag() == AST_TAG_TABLE_INDICES);
	DPTR(parent)->InsertBefore(target, Extend(node, AST_TAG_EXPRLIST_ITEM, AST_CHILD_EXPR));
}

/////////////////////////////////////////////////////////
// TableValueListConversion
//
class TableValueListConversion : public ItemListConversion {
	public:
	TreeNode*	GetFinalTarget	(TreeNode* target) const;
	void		HandleItem		(TreeNode* target, TreeNode* node) const;

	TableValueListConversion (NodeCreatorFunc creator, NodeDeleterFunc deleter) :
		ItemListConversion(creator, deleter, &ContentExpressionConversion::IsConvertibleToContent, true) {}
};

//*****************************

TreeNode* TableValueListConversion::GetFinalTarget (TreeNode* target) const {
	DASSERT(target);
	TreeNode* parent = DPTR(target)->GetParent();
	DASSERT(parent);
	DASSERT(DPTR(parent)->GetTag() == AST_TAG_EXPRLIST_ITEM);
	return parent;
}

//*****************************

void TableValueListConversion::HandleItem(TreeNode* target, TreeNode* node) const {
	DASSERT(target && node);
	TreeNode* parent = DPTR(target)->GetParent();
	DASSERT(parent && DPTR(parent)->GetTag() == AST_TAG_TABLE_VALUES);
	node = ContentExpressionConversion(creator, deleter).ToContent(node);
	DPTR(parent)->InsertBefore(target, Extend(node, AST_TAG_EXPRLIST_ITEM, AST_CHILD_EXPR));
}

/////////////////////////////////////////////////////////
// StatementListConversion
//
CONVERSION_CLASS_EX(
	StatementListConversion,
	static bool IsConvertibleToStatement(TreeNode* node);
	TreeNode* ToStatement(TreeNode* node) const;
);

//*****************************

bool StatementListConversion::IsValid (TreeNode* target, TreeNode* node) const {
	DASSERT(target);
	TreeNode* parent = DPTR(target)->GetParent();
	DASSERT(parent && DPTR(parent)->GetTag() == AST_TAG_EXPR);
	parent = DPTR(parent)->GetParent();
	DASSERT(parent && DPTR(parent)->GetTag() == AST_TAG_EXPRLIST_STMT);
	return	DPTR(parent)->GetTotalChildren() == 1	&&	//ExprListStmt as a single stmt
			(
				!node									||
				DPTR(node)->GetTag() == AST_TAG_STMTS	||
				IsConvertibleToStatement(node)			||
				IsList(node, &IsConvertibleToStatement)
			);
}

//*****************************

void ASTInjector::Conversion::RemoveLastChildWithinQuasiQuotes(TreeNode *node, TreeNode **newAst) const {
	DASSERT(node && DPTR(node)->GetTotalChildren() == 1);
	if (TreeNode* parent = DPTR(node)->GetParent()) {
		if ((DPTR(parent)->GetTag() == AST_TAG_QUASI_QUOTES)) {
			TreeNode* expr = DPTR(parent)->GetParent();
			DASSERT(expr);
			expr->Replace(parent, ExpressionConversion(creator, deleter).ToExpression(creator(AST_TAG_NIL)));
			Delete(parent);
		}
		else
			Delete(node);
	}
	else {
		Delete(node);
		DASSERT(newAst);
		*newAst = (TreeNode*) 0;
	}
}

//*****************************

void StatementListConversion::operator() (TreeNode* target, TreeNode* node, TreeNode** newAst) const {
	DASSERT(target);
	TreeNode* parent = DPTR(target)->GetParent();
	DASSERT(parent && DPTR(parent)->GetTag() == AST_TAG_EXPR);
	parent = DPTR(parent)->GetParent();
	DASSERT(parent && DPTR(parent)->GetTag() == AST_TAG_EXPRLIST_STMT);
	parent = DPTR(parent)->GetParent();
	DASSERT(parent && DPTR(parent)->GetTag() == AST_TAG_BASIC_STMT);
	TreeNode* grandParent = parent->GetParent();
	DASSERT(grandParent);
	
	if (!node) {
		if (DPTR(grandParent)->GetTag() == AST_TAG_STMT) {
			TreeNode* stmts = DPTR(grandParent)->GetParent();
			DASSERT(stmts && DPTR(stmts)->GetTag() == AST_TAG_STMTS);
			if (DPTR(stmts)->GetTotalChildren() == 1)	//removing last child
				Conversion(creator, deleter).RemoveLastChildWithinQuasiQuotes(stmts, newAst);
			else
				Delete(grandParent);
		}
		else
			Delete(parent);
	}
	else if (IsConvertibleToStatement(node)) {
		DPTR(grandParent)->Replace(parent, ToStatement(node));
		Delete(parent);
	}
	else {	//STMTS or list of convertible statements
		TreeNode* stmt = DPTR(node)->GetChild(0);
		DASSERT(stmt);
		DPTR(stmt)->RemoveFromParent();
		stmt = ToStatement(stmt);
		DPTR(grandParent)->Replace(parent, stmt);
		Delete(parent);

		if (DPTR(node)->GetTotalChildren()) {
			TreeNode* current = stmt;
			parent = grandParent;
			while (DPTR(parent)->GetTag() != AST_TAG_STMTS) {
				const std::string tag = DPTR(parent)->GetTag();
				const TreeNode::Index index = DPTR(parent)->GetChildIndex(current);
				bool ifElseIfStmt = tag == AST_TAG_IF_ELSE && index.second == AST_CHILD_IF;
				bool tryStmt = tag == AST_TAG_TRYTRAP && index.second == AST_CHILD_TRY;
				if (ifElseIfStmt || tryStmt) {
					TreeNode* child = DPTR(parent)->GetChild(index.second);
					DPTR(parent)->Replace(child, node);
					TreeNode* stmt = Extend(child, AST_TAG_STMT, AST_CHILD_STMT);
					DPTR(node)->PushFront(stmt);
					return;
				}

				current = parent;
				parent = DPTR(parent)->GetParent();
				DASSERT(parent);
			}

			for(util_ui32 i = DPTR(node)->GetTotalChildren(); i; --i) {
				TreeNode* child = DPTR(node)->GetChild(i - 1);
				DASSERT(child);
				DPTR(child)->RemoveFromParent();
				child = Extend(ToStatement(child), AST_TAG_STMT, AST_CHILD_STMT);
				DPTR(parent)->InsertAfter(current, child);
			}
			DASSERT(!DPTR(node)->GetTotalChildren());
		}
		Delete(node);
	}
}

//*****************************

TreeNode* StatementListConversion::ToStatement(TreeNode* node) const {
	DASSERT(node->GetTag() == AST_TAG_STMT || IsConvertibleToStatement(node));
	if (node->GetTag() == AST_TAG_STMT) {	//part of STMTS
		TreeNode* stmtNode = node;
		node = DPTR(node)->GetChild(AST_CHILD_STMT);
		DPTR(node)->RemoveFromParent();
		Delete(stmtNode);
	}
	//Nothing to be done for IsStmt
	else if (AST::ValidationVisitor::IsBasicStmt(node))
		node = Extend(node, AST_TAG_BASIC_STMT, AST_CHILD_STMT);
	else if (AST::ValidationVisitor::IsFunction(node))
		node = Extend(node, AST_TAG_FUNCTION_STMT, AST_CHILD_FUNCTION);
	else if (ExpressionConversion::IsConvertibleToExpression(node)) {
		node = ExpressionConversion(creator, deleter).ToExpression(node);
		node = Extend(node, AST_TAG_EXPR, AST_CHILD_EXPR);
		node = Extend(node, AST_TAG_EXPRLIST_STMT);
		node = Extend(node, AST_TAG_BASIC_STMT, AST_CHILD_STMT);
	}
	return node;
}

//*****************************

bool StatementListConversion::IsConvertibleToStatement(TreeNode* node) {
	return	AST::ValidationVisitor::IsStmt(node)					||
			AST::ValidationVisitor::IsBasicStmt(node)				||
			AST::ValidationVisitor::IsFunction(node)				||
			ExpressionConversion::IsConvertibleToExpression(node);
}

///////////////////////////////////////////////////////////
// ASTInjector
//

#define	CONTEXT(parentTag, childId)		(parentTag + "." + childId)

//*************************

bool ASTInjector::MatchesHierarchy (TreeNode* node, const Hierarchy& hierarchy) const {
	TreeNode* current = node;
	for (std::list<std::string>::const_reverse_iterator i = hierarchy.rbegin(); i != hierarchy.rend(); ++i) {
		if (!current || DPTR(current)->GetTag() != *i)
			return false;
		current = DPTR(current)->GetParent();
	}
	return true;
}

//*************************

void ASTInjector::RegisterContextConversion (const std::string& parentTag, const std::string& childId, Conversion* conversion)
	{ contextConversions[CONTEXT(parentTag, childId)] = conversion; }

void ASTInjector::RegisterListConversion (const std::string& parentTag, Conversion* conversion)
	{ listConversions[parentTag] = conversion; }

void ASTInjector::RegisterHierarchyConversion (const Hierarchy& hierarchy, Conversion* conversion)
	{ hierarchyConversions.push_back(HierarchyConversion(hierarchy, conversion)); }

//*************************

void ASTInjector::operator()(TreeNode* target, TreeNode* ast, TreeNode** newAst) const {
	DASSERT(target);
	TreeNode* parent = DPTR(target)->GetParent();
	DASSERT(parent);

	for (HierarchyConversionList::const_iterator i = hierarchyConversions.begin(); i != hierarchyConversions.end(); ++i)
		if (MatchesHierarchy(DPTR(parent), i->first) && i->second->IsValid(DPTR(target), DNPTR(ast))) {
			(*i->second)(DPTR(target), DNPTR(ast), newAst);
			return;
		}

	const std::string parentTag = DPTR(parent)->GetTag();
	const TreeNode::Index index = DPTR(parent)->GetChildIndex(target);

	const Conversion* conversion = (Conversion*) 0;

	if (!index.second.empty()) {
		ConversionMap::const_iterator i = contextConversions.find(CONTEXT(parentTag, index.second));
		if (i != contextConversions.end())
			conversion = i->second;
	}
	else {
		ConversionMap::const_iterator i = listConversions.find(parentTag);
		if (i != listConversions.end())
			conversion = i->second;
	}

	if (conversion && conversion->IsValid(DPTR(target), DNPTR(ast)))
		(*conversion)(DPTR(target), DNPTR(ast), newAst);
	else {
		if (!ast) {
			if (parentTag == AST_TAG_QUOTED_ELEMENTS &&	DPTR(parent)->GetTotalChildren() == 1) {		//removing last child
				Conversion(creator, deleter).RemoveLastChildWithinQuasiQuotes(parent, newAst);
				return;
			}
			else
				DPTR(target)->RemoveFromParent();			
		}
		else if (parentTag == AST_TAG_QUOTED_ELEMENTS) {
			const std::string astTag = DPTR(ast)->GetTag();
			if (astTag == AST_TAG_QUOTED_ELEMENTS) {
				while (DPTR(ast)->GetTotalChildren()) {
					TreeNode* child = DPTR(ast)->GetChild(0);
					DASSERT(child);
					child->RemoveFromParent();
					DPTR(parent)->InsertBefore(target, child);
				}			
				Delete(ast);
			}
			else if (astTag == AST_TAG_STMTS && DPTR(parent)->GetTotalChildren() == 1) { //single child being injected
				if (TreeNode* quotes = DPTR(parent)->GetParent()) {	//within nested quasi-quote so transform them
					DASSERT(DPTR(quotes)->GetTag() == AST_TAG_QUASI_QUOTES);
					DPTR(quotes)->Replace(parent, ast);
					target = parent;	//to be deleted
				}
				else {
					DASSERT(newAst);
					*newAst = ast;
				}
			}
			else
				DPTR(parent)->Replace(target, ast);
		}
		else
			DPTR(parent)->Replace(target, ast);
		Delete(target);
	}
}

//*************************

ASTInjector::ASTInjector (NodeCreatorFunc creator, NodeDeleterFunc deleter) : creator(creator), deleter(deleter) {
#define NEW_CONVERTOR(class) convertors[#class] = DNEWCLASS(class, (creator, deleter))
	NEW_CONVERTOR(ExpressionConversion);
	NEW_CONVERTOR(LvalueConversion);
	NEW_CONVERTOR(FormalsConversion);
	NEW_CONVERTOR(NameConversion);
	NEW_CONVERTOR(NamespacePathConversion);
	NEW_CONVERTOR(ContentExpressionConversion);
	NEW_CONVERTOR(FunctionAndTableConversion);
	NEW_CONVERTOR(ArgumentListConversion);
	NEW_CONVERTOR(ExpressionListConversion);
	NEW_CONVERTOR(TableIndexListConversion);
	NEW_CONVERTOR(TableValueListConversion);
	NEW_CONVERTOR(TableElementListConversion);
	NEW_CONVERTOR(StatementListConversion);

#define EXPRESSION_INJECTION(parentTag, childId)	\
	RegisterContextConversion(parentTag, childId, convertors["ExpressionConversion"])

	EXPRESSION_INJECTION(AST_TAG_LAMBDA_FUNCTION,				AST_CHILD_EXPR);
	EXPRESSION_INJECTION(AST_TAG_ASSERT,						AST_CHILD_EXPR);
	EXPRESSION_INJECTION(AST_TAG_RETURN,						AST_CHILD_EXPR);
	EXPRESSION_INJECTION(AST_TAG_CONSTDEF,						AST_CHILD_EXPR);
	EXPRESSION_INJECTION(AST_TAG_NOT,							AST_CHILD_EXPR);
	EXPRESSION_INJECTION(AST_TAG_UMINUS,						AST_CHILD_EXPR);
	EXPRESSION_INJECTION(AST_TAG_PRIMARY_EXPRESSION,			AST_CHILD_EXPR);
	EXPRESSION_INJECTION(AST_TAG_PARENTHESISED_EXPR,			AST_CHILD_EXPR);
	EXPRESSION_INJECTION(AST_TAG_LATEBOUND_ARGUMENT,			AST_CHILD_EXPR);
	EXPRESSION_INJECTION(AST_TAG_THROW,							AST_CHILD_EXPR);
	EXPRESSION_INJECTION(AST_TAG_ESCAPE,						AST_CHILD_EXPR);
	EXPRESSION_INJECTION(AST_TAG_INLINE,						AST_CHILD_EXPR);

	EXPRESSION_INJECTION(AST_TAG_ASSIGN,						AST_CHILD_RVALUE);
	EXPRESSION_INJECTION(AST_TAG_OP_ADD_ASSIGN,					AST_CHILD_RVALUE);
	EXPRESSION_INJECTION(AST_TAG_OP_MUL_ASSIGN,					AST_CHILD_RVALUE);
	EXPRESSION_INJECTION(AST_TAG_OP_SUB_ASSIGN,					AST_CHILD_RVALUE);
	EXPRESSION_INJECTION(AST_TAG_OP_DIV_ASSIGN,					AST_CHILD_RVALUE);
	EXPRESSION_INJECTION(AST_TAG_OP_MOD_ASSIGN,					AST_CHILD_RVALUE);

	EXPRESSION_INJECTION(AST_TAG_OP_GT,							AST_CHILD_LEFT);
	EXPRESSION_INJECTION(AST_TAG_OP_LT,							AST_CHILD_LEFT);
	EXPRESSION_INJECTION(AST_TAG_OP_GE,							AST_CHILD_LEFT);
	EXPRESSION_INJECTION(AST_TAG_OP_LE,							AST_CHILD_LEFT);
	EXPRESSION_INJECTION(AST_TAG_OP_EQ,							AST_CHILD_LEFT);
	EXPRESSION_INJECTION(AST_TAG_OP_NE,							AST_CHILD_LEFT);
	EXPRESSION_INJECTION(AST_TAG_AND,							AST_CHILD_LEFT);
	EXPRESSION_INJECTION(AST_TAG_OR,							AST_CHILD_LEFT);
	EXPRESSION_INJECTION(AST_TAG_OP_ADD,						AST_CHILD_LEFT);
	EXPRESSION_INJECTION(AST_TAG_OP_SUB,						AST_CHILD_LEFT);
	EXPRESSION_INJECTION(AST_TAG_OP_MUL,						AST_CHILD_LEFT);
	EXPRESSION_INJECTION(AST_TAG_OP_DIV,						AST_CHILD_LEFT);
	EXPRESSION_INJECTION(AST_TAG_OP_MOD,						AST_CHILD_LEFT);

	EXPRESSION_INJECTION(AST_TAG_OP_GT,							AST_CHILD_RIGHT);
	EXPRESSION_INJECTION(AST_TAG_OP_LT,							AST_CHILD_RIGHT);
	EXPRESSION_INJECTION(AST_TAG_OP_GE,							AST_CHILD_RIGHT);
	EXPRESSION_INJECTION(AST_TAG_OP_LE,							AST_CHILD_RIGHT);
	EXPRESSION_INJECTION(AST_TAG_OP_EQ,							AST_CHILD_RIGHT);
	EXPRESSION_INJECTION(AST_TAG_OP_NE,							AST_CHILD_RIGHT);
	EXPRESSION_INJECTION(AST_TAG_AND,							AST_CHILD_RIGHT);
	EXPRESSION_INJECTION(AST_TAG_OR,							AST_CHILD_RIGHT);
	EXPRESSION_INJECTION(AST_TAG_OP_ADD,						AST_CHILD_RIGHT);
	EXPRESSION_INJECTION(AST_TAG_OP_SUB,						AST_CHILD_RIGHT);
	EXPRESSION_INJECTION(AST_TAG_OP_MUL,						AST_CHILD_RIGHT);
	EXPRESSION_INJECTION(AST_TAG_OP_DIV,						AST_CHILD_RIGHT);
	EXPRESSION_INJECTION(AST_TAG_OP_MOD,						AST_CHILD_RIGHT);

	EXPRESSION_INJECTION(AST_TAG_TERNARY,						AST_CHILD_LEFT);
	EXPRESSION_INJECTION(AST_TAG_TERNARY,						AST_CHILD_RIGHT);
	EXPRESSION_INJECTION(AST_TAG_TERNARY,						AST_CHILD_COND);

	EXPRESSION_INJECTION(AST_TAG_TABLE_CONTENT_BRACKET,			AST_CHILD_INDEX);
	EXPRESSION_INJECTION(AST_TAG_TABLE_CONTENT_DOUBLE_BRACKET,	AST_CHILD_INDEX);

	EXPRESSION_INJECTION(AST_TAG_IF,							AST_CHILD_COND);
	EXPRESSION_INJECTION(AST_TAG_IF_ELSE,						AST_CHILD_COND);
	EXPRESSION_INJECTION(AST_TAG_WHILE,							AST_CHILD_COND);
	EXPRESSION_INJECTION(AST_TAG_FOR,							AST_CHILD_COND);

	EXPRESSION_INJECTION(AST_TAG_FOREACH,						AST_CHILD_CONTAINER);

#define LVALUE_INJECTION(parentTag, childId)	\
	RegisterContextConversion(parentTag, childId, convertors["LvalueConversion"])

	LVALUE_INJECTION(AST_TAG_ASSIGN,		AST_CHILD_LVALUE);
	LVALUE_INJECTION(AST_TAG_OP_ADD_ASSIGN,	AST_CHILD_LVALUE);
	LVALUE_INJECTION(AST_TAG_OP_MUL_ASSIGN,	AST_CHILD_LVALUE);
	LVALUE_INJECTION(AST_TAG_OP_SUB_ASSIGN,	AST_CHILD_LVALUE);
	LVALUE_INJECTION(AST_TAG_OP_DIV_ASSIGN,	AST_CHILD_LVALUE);
	LVALUE_INJECTION(AST_TAG_OP_MOD_ASSIGN,	AST_CHILD_LVALUE);

	LVALUE_INJECTION(AST_LVALUE_PLUSPLUS,	AST_CHILD_LVALUE);
	LVALUE_INJECTION(AST_PLUSPLUS_LVALUE,	AST_CHILD_LVALUE);
	LVALUE_INJECTION(AST_LVALUE_MINUSMINUS,	AST_CHILD_LVALUE);
	LVALUE_INJECTION(AST_MINUSMINUS_LVALUE,	AST_CHILD_LVALUE);

	LVALUE_INJECTION(AST_TAG_FOREACH,		AST_CHILD_INDEX);
	LVALUE_INJECTION(AST_TAG_FOREACH,		AST_CHILD_LVALUE);

#define NAME_INJECTION(parentTag, childId)	\
	RegisterContextConversion(parentTag, childId, convertors["NameConversion"])

	NAME_INJECTION(AST_TAG_FUNCTION,				AST_CHILD_NAME);
	NAME_INJECTION(AST_TAG_CONSTDEF,				AST_CHILD_NAME);
	NAME_INJECTION(AST_TAG_LVALUE_STATIC_IDENT,		AST_CHILD_NAME);
	NAME_INJECTION(AST_TAG_LVALUE_LOCAL_IDENT,		AST_CHILD_NAME);
	NAME_INJECTION(AST_TAG_TABLE_IDENTINDEX_ELEM,	AST_CHILD_NAME);
	NAME_INJECTION(AST_TAG_TABLE_NEW_ATTRIBUTE,		AST_CHILD_NAME);
	NAME_INJECTION(AST_TAG_EXCEPTION_VAR,			AST_CHILD_NAME);

#define CONTENT_EXPRESSION_INJECTION(parentTag, childId)	\
	RegisterContextConversion(parentTag, childId, convertors["ContentExpressionConversion"])

	CONTENT_EXPRESSION_INJECTION(AST_TAG_TABLE_IDENTINDEX_ELEM,	AST_CHILD_EXPR);
	CONTENT_EXPRESSION_INJECTION(AST_TAG_TABLE_NEW_ATTRIBUTE,		AST_CHILD_SET);
	CONTENT_EXPRESSION_INJECTION(AST_TAG_TABLE_NEW_ATTRIBUTE,		AST_CHILD_GET);

#define FUNC_AND_TABLE_INJECTION(parentTag, childId)	\
	RegisterContextConversion(parentTag, childId, convertors["FunctionAndTableConversion"])

	FUNC_AND_TABLE_INJECTION(AST_TAG_FUNCTION_CALL,					AST_CHILD_FUNCTION);
	FUNC_AND_TABLE_INJECTION(AST_TAG_TABLE_CONTENT_DOT,				AST_CHILD_TABLE);
	FUNC_AND_TABLE_INJECTION(AST_TAG_TABLE_CONTENT_DOUBLE_DOT,		AST_CHILD_TABLE);
	FUNC_AND_TABLE_INJECTION(AST_TAG_TABLE_CONTENT_BRACKET,			AST_CHILD_TABLE);
	FUNC_AND_TABLE_INJECTION(AST_TAG_TABLE_CONTENT_DOUBLE_BRACKET,	AST_CHILD_TABLE);

	Hierarchy arguments;
	arguments.push_back(AST_TAG_FUNCTION_CALL);
	arguments.push_back(AST_TAG_EXPR_LIST);
	arguments.push_back(AST_TAG_EXPRLIST_ITEM);
	arguments.push_back(AST_TAG_ARGUMENT);
	RegisterHierarchyConversion(arguments, convertors["ArgumentListConversion"]);

	Hierarchy forList;
	forList.push_back(AST_TAG_FOR);
	forList.push_back(AST_TAG_EXPR_LIST);
	forList.push_back(AST_TAG_EXPRLIST_ITEM);
	RegisterHierarchyConversion(forList, convertors["ExpressionListConversion"]);

	Hierarchy tableIndices;
	tableIndices.push_back(AST_TAG_TABLE_INDICES);
	tableIndices.push_back(AST_TAG_EXPRLIST_ITEM);
	RegisterHierarchyConversion(tableIndices, convertors["TableIndexListConversion"]);

	Hierarchy tableValues;
	tableValues.push_back(AST_TAG_TABLE_VALUES);
	tableValues.push_back(AST_TAG_EXPRLIST_ITEM);
	RegisterHierarchyConversion(tableValues, convertors["TableValueListConversion"]);

	Hierarchy exprList;
	exprList.push_back(AST_TAG_EXPRLIST_STMT);
	exprList.push_back(AST_TAG_EXPR);
	RegisterHierarchyConversion(exprList, convertors["StatementListConversion"]);	//first check for StmtList
	RegisterHierarchyConversion(exprList, convertors["ExpressionListConversion"]);	//and then for ExprList

	RegisterListConversion(AST_TAG_FORMAL_ARGS,					convertors["FormalsConversion"]);
	RegisterListConversion(AST_TAG_TABLE_CONSTRUCTOR,			convertors["TableElementListConversion"]);
	RegisterListConversion(AST_TAG_USING_NAMESPACE,				convertors["NamespacePathConversion"]);
	RegisterListConversion(AST_TAG_LVALUE_NAMESPACE_IDENT,		convertors["NamespacePathConversion"]);
	RegisterListConversion(AST_TAG_STRINGIFY_NAMESPACE_IDENT,	convertors["NamespacePathConversion"]);
}

ASTInjector::~ASTInjector() {
	std::for_each(
		convertors.begin(),
		convertors.end(),
		udestroy_second<ConversionMap::value_type>()
	);
	contextConversions.clear();
	listConversions.clear();
	hierarchyConversions.clear();
}
