// TreeVisitor.h
// Generic visitor for tree nodes.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#ifndef	TREEVISITOR_H
#define	TREEVISITOR_H

#include "DeltaSyntaxTreeDefs.h"
#include "DDebug.h"
#include "TreeNode.h"
#include "ucallbacks.h"
#include <map>
#include <string>

/////////////////////////////////////////////////////////

#define	AST_VISITOR_ARGS				\
		TreeNode*			node,		\
		const std::string&	childId,	\
		bool				entering,	\
		void*				closure

#define	AST_VISITOR_ACTUALS				\
		node, childId, entering, closure

class SYNTAXTREELIB_CLASS TreeVisitor {

	public:
	typedef void (*Handler)(TreeNode*, const std::string& childId, bool entering, void* closure);
	enum State { VisitStopped = 1, VisitLeave = 2, VisitPrune = 3, VisitContinue = 4 };

	protected:
	typedef ucallbackwithclosure<Handler>			HandlerCallback;
	typedef ucallbacklist<Handler>					HandlerCallbacks;
	typedef std::map<std::string, HandlerCallbacks>	MultiHandlers;
	typedef std::map<std::string, HandlerCallback>	Handlers;

	HandlerCallback	defaultHandler;
	Handlers		handlers;
	MultiHandlers	contextHandlers;
	State			state;
	bool			pruneAndDelete;

	virtual State	Visit (TreeNode* node, const std::string& childId, bool entering);

	State			InvokeVisit (TreeNode* node, const std::string& childId, bool entering) {
						State result = Visit(node, childId, entering); 
						DASSERT(result == state); 
						return result; 
					}

	public:
	State			GetState (void) const
						{ return state; }

	void			Stop (void) 
						{ state = VisitStopped; }
	bool			ShouldStop (void) const
						{ return state == VisitStopped; }

	void			Leave (void) // On enter skips children; on leave skips siblings
						{ state = VisitLeave; }
	bool			ShouldLeave (void) const
						{ return state == VisitLeave; }

	void			Prune (bool deleteToo = true) //Can prune everything but the root
						{ state = VisitPrune, pruneAndDelete = deleteToo; }
	bool			ShouldPrune (bool *shouldDelete) const {
						if (state == VisitPrune) {
							if (shouldDelete)
								*shouldDelete = pruneAndDelete;
							return true;
						}
						else
							return false;
					}

	void			Continue (void) 
						{ state = VisitContinue; }

	void			SetHandler (					// Absolute by child tag.
						const std::string&	nodeTag, 
						Handler				f, 
						void*				c = (void*) 0
					);

					// When adding multiple handlers for the same parent tag 
					// and child id, those are called with the order of registration.
	void			SetContextDependentHandler (	// Relative to parent tag and child id in parent node.
						const std::string&	parentTag, 
						const std::string&	childId, 
						Handler				f, 
						void*				c = (void*) 0
					);

	void			SetDefaultHandler (
						Handler				f, 
						void*				c = (void*) 0
					);

	// Visiting node by supplying also the id it has in its parent, if it has a parent.

	virtual State	OnVisitEntering (TreeNode* node, const std::string& childId) 
						{ DASSERT(state == VisitContinue); return InvokeVisit(node, childId, true); }

	virtual State	OnVisitLeaving (TreeNode* node, const std::string& childId) 
						{ return InvokeVisit(node, childId, false); }

	TreeVisitor (void) : state(VisitContinue), pruneAndDelete(false) {}
	virtual ~TreeVisitor(){}
};

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
