// PointcutCombinators.h
// ScriptFighter Project.
// Y. Lilis, August 2013.
//

#ifndef	POINTCUT_COMBINATORS_H
#define	POINTCUT_COMBINATORS_H

#include "Pointcut.h"
#include "UtilVisitors.h"

/////////////////////////////////////////////////////////

class AndPointcut : public Pointcut {
private:
	Pointcut* left;
	Pointcut* right;
public:
	const ASTSet Evaluate(TreeNode* ast) const {
		ASTSet leftResult = DPTR(left)->Evaluate(ast);
		const ASTSet rightResult = DPTR(right)->Evaluate(ast);
		
		for (ASTSet::iterator i = leftResult.begin(); i != leftResult.end(); /*empty*/)
			if (rightResult.find(*i) == rightResult.end())
				i = leftResult.erase(i);
			else
				++i;
		return leftResult;
	}

	AndPointcut(Pointcut* left, Pointcut* right) : left(left), right(right) {}
};

/////////////////////////////////////////////////////////

class OrPointcut : public Pointcut {
private:
	Pointcut* left;
	Pointcut* right;
public:
	const ASTSet Evaluate(TreeNode* ast) const {
		ASTSet leftResult = DPTR(left)->Evaluate(ast);
		const ASTSet rightResult = DPTR(right)->Evaluate(ast);
		leftResult.insert(rightResult.begin(), rightResult.end());
		return leftResult;
	}

	OrPointcut(Pointcut* left, Pointcut* right) : left(left), right(right) {}
};

/////////////////////////////////////////////////////////

class NotPointcut : public Pointcut {
private:
	Pointcut* pointcut;
public:
	const ASTSet Evaluate(TreeNode* ast) const {
		ASTSet complement = DPTR(pointcut)->Evaluate(ast);
		ASTSet result = Linearizer()(ast);
		for (ASTSet::const_iterator i = complement.begin(); i != complement.end(); ++i) {
			DASSERT(result.find(*i) != result.end());
			result.erase(*i);
		}
		return result;
	}

	NotPointcut(Pointcut* pointcut) : pointcut(pointcut) {}
};

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.