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
	{ contextHandlers[CONTEXT(parentTag, childId)].add(f,c); }

void TreeVisitor::SetDefaultHandler (Handler f, void* c) 
	{ defaultHandler.set(f,c); }

/////////////////////////////////////////////////////////

TreeVisitor::State TreeVisitor::Visit (TreeNode* node, const std::string& childId, bool entering) {

	HandlerCallbacks*	context = (HandlerCallbacks*) 0;
	HandlerCallback*	normal	= (HandlerCallback*) 0;

	Handlers::iterator i = handlers.find(DPTR(node)->GetTag());	// Handling current node.
	if (i != handlers.end())
		normal = &i->second;
	else
	if (defaultHandler.f())
		normal = &defaultHandler;

	if (TreeNode* parent = DPTR(node)->GetParent()) {
		MultiHandlers::iterator	j = contextHandlers.find(CONTEXT(DPTR(parent)->GetTag(), childId));
		if (j != contextHandlers.end())
			context = &j->second;
	}

	if (entering) {									// Firstly the specific rule and then the generic rule
		if (context)
			context->call(node, childId, true);
		if (normal)
			(*normal)(node, childId, true);
	}
	else {											// Firstly the generic and then the specific rule
		if (normal)
			(*normal)(node, childId, false);
		if (context)
			context->call(node, childId, false);
	}

	return state;			
}

/////////////////////////////////////////////////////////
