// AOPPointcuts.h
// ScriptFighter Project.
// Y. Lilis, August 2013.
//

#ifndef	AOP_POINTCUTS_H
#define	AOP_POINTCUTS_H

#include "Pointcut.h"
#include "UtilVisitors.h"

/////////////////////////////////////////////////////////

class AST2Pointcut : public Pointcut {
private:
	const std::string type;
public:
	const ASTSet Evaluate(TreeNode* ast, bool includeChildren = true) const {
		ASTSet result;
		if (includeChildren)
			result = NodeCollector(type)(ast);
		else if (DPTR(ast)->GetTag() == type)
			result.insert(ast);
		return result;
	}

	AST2Pointcut(const std::string& type) : type(type) {}
};

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.