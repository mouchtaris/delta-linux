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
	const ASTSet Evaluate(TreeNode* ast, bool includeChildren = true) const {
		ASTSet leftResult = DPTR(left)->Evaluate(ast, includeChildren);
		const ASTSet rightResult = DPTR(right)->Evaluate(ast, includeChildren);
		
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
	const ASTSet Evaluate(TreeNode* ast, bool includeChildren = true) const {
		ASTSet leftResult = DPTR(left)->Evaluate(ast, includeChildren);
		const ASTSet rightResult = DPTR(right)->Evaluate(ast, includeChildren);
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
	const ASTSet Evaluate(TreeNode* ast, bool includeChildren = true) const {
		ASTSet result = DPTR(pointcut)->Evaluate(ast, includeChildren);

		if (includeChildren) {
			ASTSet all = Linearizer()(ast);
			for (ASTSet::const_iterator i = result.begin(); i != result.end(); ++i) {
				DASSERT(all.find(*i) != all.end());
				all.erase(*i);
			}
		}
		else {
			if (result.empty())
				result.insert(ast);
			else {
				DASSERT(result.size() == 1 && *result.begin() == ast);
				result.clear();
			}
		}

		return result;
	}

	NotPointcut(Pointcut* pointcut) : pointcut(pointcut) {}
};

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.