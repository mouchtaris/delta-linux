// AOPPointcuts.h
// ScriptFighter Project.
// Y. Lilis, August 2013.
//

#ifndef	AOP_POINTCUTS_H
#define	AOP_POINTCUTS_H

#include "Pointcut.h"

/////////////////////////////////////////////////////////

class ExecutionPointcut : public Pointcut {
private:
	const std::string	funcClassPattern;
	const std::string	namePattern;
	IdList				formalsPattern;

public:
	const ASTSet Evaluate(TreeNode* ast, bool includeChildren = true) const;

	ExecutionPointcut(const std::string& funcClass, const std::string& name, const IdList& formals) :
		funcClassPattern(funcClass), namePattern(name), formalsPattern(formals) {}
};

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.