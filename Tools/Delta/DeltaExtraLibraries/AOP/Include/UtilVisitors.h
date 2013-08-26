// UtilVisitors.h
// ScriptFighter Project.
// Y. Lilis, August 2013.
//

#ifndef	UTIL_VISITORS_H
#define	UTIL_VISITORS_H

#include "TreeVisitor.h"

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


#endif	// Do not add stuff beyond this point.