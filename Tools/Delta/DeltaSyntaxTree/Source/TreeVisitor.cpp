// TreeVisitor.cpp
// Generic visitor for tree nodes.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#include "TreeVisitor.h"

#define	CONTEXT(parentTag, childId)	(parentTag + "." + childId)

/////////////////////////////////////////////////////////

void TreeVisitor::SetHandler (const std::string& nodeTag, Handler f, void* c) 
	{ handlers[nodeTag].set(f,c); }

void TreeVisitor::SetContextDependentHandler (const std::string& parentTag, const std::string& childId, Handler f, void* c)
	{ contextHandlers[CONTEXT(parentTag, childId)].set(f,c); }

void TreeVisitor::SetDefaultHandler (Handler f, void* c) 
	{ defaultHandler.set(f,c); }

/////////////////////////////////////////////////////////

TreeVisitor::State TreeVisitor::Visit (TreeNode* node, const std::string& childId, bool entering) {

	Handlers::iterator i = handlers.find(DPTR(node)->GetTag());	// Handling current node.

	if (i != handlers.end()) {

		TreeNode*			parent	= DPTR(node)->GetParent();
		Handlers::iterator	j		= parent ?	contextHandlers.find(CONTEXT(DPTR(parent)->GetTag(), childId)) : 
												contextHandlers.end();

		if (j != contextHandlers.end())
			if (entering) {		// Firstly the specific rule and then the generic rule.
				j->second(node, childId, entering);
				i->second(node, childId, entering);
			}
			else {				// Firstly the generic rule and then the specific rule.
				i->second(node, childId, entering);
				j->second(node, childId, entering);
			}
		else
			i->second(node, childId, entering);
	}
	else
	if (defaultHandler.f())
		defaultHandler(node, childId, entering);

	return state;			
}

/////////////////////////////////////////////////////////
