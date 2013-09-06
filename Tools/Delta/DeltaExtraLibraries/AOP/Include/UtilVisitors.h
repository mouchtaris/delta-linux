// UtilVisitors.h
// ScriptFighter Project.
// Y. Lilis, August 2013.
//

#ifndef	UTIL_VISITORS_H
#define	UTIL_VISITORS_H

#include "TreeVisitor.h"
#include "ASTTags.h"
#include "ASTMetaTags.h"
#include "Pointcut.h"

/////////////////////////////////////////////////////////

struct Linearizer : public TreeVisitor {
	ASTSet nodes;
	static void Handle_All (AST_VISITOR_ARGS) { if (entering) ((Linearizer*) closure)->nodes.insert(node); }
	ASTSet operator()(TreeNode* root) { DASSERT(root); DPTR(root)->AcceptPreOrder(this); return nodes; }
	Linearizer (void) { SetDefaultHandler(&Handle_All, this); }
};

/////////////////////////////////////////////////////////

struct NodeCollector : public TreeVisitor {
	ASTSet nodes;
	static void Handle (AST_VISITOR_ARGS) { if (entering) ((NodeCollector*) closure)->nodes.insert(node); }
	ASTSet operator()(TreeNode* root) { DASSERT(root); DPTR(root)->AcceptPreOrder(this); return nodes; }
	NodeCollector (const std::string& tag) { this->SetHandler(tag, &Handle, this); }
};

/////////////////////////////////////////////////////////

struct EscapeLocator : public TreeVisitor {
	const std::string	id;
	TreeNode*			node;

	static bool MatchesId (TreeNode* node, const std::string& id) {
		if (TreeNode* primary = DPTR(node)->GetChild(AST_CHILD_EXPR))
			if (DPTR(primary)->GetTag() == AST_TAG_PRIMARY_EXPRESSION)
				if (TreeNode* ident = DPTR(primary)->GetChild(AST_CHILD_EXPR))
					if (DPTR(ident)->GetTag() == AST_TAG_LVALUE_IDENT)
						if (TreeNode* name = DPTR(ident)->GetChild(AST_CHILD_NAME))
							if (DPTR(name)->GetTag() == AST_TAG_NAME)
								return NAME(name) == id;
		return false;
	}
	static void Handle_Escape (AST_VISITOR_ARGS) {
		if (entering) {
			EscapeLocator* visitor = (EscapeLocator*) closure;
			if (CARDINALITY(node) == 1 && MatchesId(node, visitor->id)) {
				visitor->node = node;
				visitor->Stop();
			}
		}
	}
	TreeNode* operator()(TreeNode* root) { if (root) DPTR(root)->AcceptPreOrder(this); return node; }
	EscapeLocator(const std::string& id) : id(id), node((TreeNode*) 0) { SetHandler(AST_TAG_ESCAPE, &Handle_Escape, this); }
};

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.