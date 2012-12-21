// ASTSanitiseVisitor.h
// Tree visitor for sanitising the AST (Remove empty stmts).
// ScriptFighter Project.
// Y. Lilis, July 2011.
//

#ifndef	ASTSANITISEVISITOR_H
#define	ASTSANITISEVISITOR_H

#include "TreeVisitor.h"
#include "ASTTags.h"
#include "ASTMetaTags.h"

/////////////////////////////////////////////////////////

namespace AST {

	class SanitiseVisitor : public TreeVisitor {

	private:

	/////////////////////////////////////////////////////////
	// Visitor handlers.
	static void Handle_EmptyChildren (AST_VISITOR_ARGS)
		{ if (!node->GetTotalChildren()) ((SanitiseVisitor*) closure)->Prune();	}

	/////////////////////////////////////////////////////////

	public:
	void operator()(TreeNode* root) { if (root) DPTR(root)->AcceptPreOrder(this);	}

	SanitiseVisitor(void) {
		SetHandler(AST_TAG_BASIC_STMT,		&Handle_EmptyChildren, this);
		SetHandler(AST_TAG_EXPRLIST_STMT,	&Handle_EmptyChildren, this);
		SetHandler(AST_TAG_STMT,			&Handle_EmptyChildren, this);
		SetHandler(AST_TAG_STMTS,			&Handle_EmptyChildren, this);
		SetHandler(AST_TAG_FUNCTION_STMT,	&Handle_EmptyChildren, this);
		SetHandler(AST_TAG_EXECUTE,			&Handle_EmptyChildren, this);
	}
	~SanitiseVisitor(){}
};

}

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
