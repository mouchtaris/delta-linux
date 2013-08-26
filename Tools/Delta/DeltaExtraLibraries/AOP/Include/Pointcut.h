// Pointcut.h
// ScriptFighter Project.
// Y. Lilis, August 2013.
//

#ifndef	POINTCUT_H
#define	POINTCUT_H

#include "TreeNode.h"
#include <set>

typedef std::set<TreeNode*> ASTSet;

/////////////////////////////////////////////////////////

class Pointcut {
public:
	virtual const ASTSet Evaluate(TreeNode* ast, bool includeChildren = true) const = 0;
};

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.