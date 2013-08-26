// ASTPointcuts.h
// ScriptFighter Project.
// Y. Lilis, August 2013.
//

#ifndef	AST_POINTCUTS_H
#define	AST_POINTCUTS_H

#include "Pointcut.h"
#include "UtilVisitors.h"

/////////////////////////////////////////////////////////

class ASTPointcut : public Pointcut {
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

	ASTPointcut(const std::string& type) : type(type) {}
};

/////////////////////////////////////////////////////////

class AttributePointcut : public Pointcut {
private:
	//TODO
public:
	const ASTSet Evaluate(TreeNode* ast, bool includeChildren = true) const {
		return ASTSet();
	}

	AttributePointcut() {}
};

/////////////////////////////////////////////////////////

class ChildPointcut : public Pointcut {
private:
	Pointcut* pointcut;
	const std::string index;
public:
	const ASTSet Evaluate(TreeNode* ast, bool includeChildren = true) const {
		const ASTSet nodes = DPTR(pointcut)->Evaluate(ast, includeChildren);
		ASTSet result;
		for (ASTSet::const_iterator i = nodes.begin(); i != nodes.end(); ++i)
			if (index.empty()) {
				for (unsigned j = 0; j < DPTR(*i)->GetTotalChildren(); ++j)
					result.insert(DPTR(*i)->GetChild(j));
			}
			else if (TreeNode* child = DPTR(*i)->GetChild(index))
				result.insert(child);
		return result;
	}

	ChildPointcut(Pointcut* pointcut, const std::string childIndex = "") : pointcut(pointcut), index(childIndex) {}
};

/////////////////////////////////////////////////////////

class ParentPointcut : public Pointcut {
private:
	Pointcut* pointcut;
	const std::string index;
public:
	const ASTSet Evaluate(TreeNode* ast, bool includeChildren = true) const {
		const ASTSet nodes = DPTR(pointcut)->Evaluate(ast, includeChildren);
		ASTSet result;
		for (ASTSet::const_iterator i = nodes.begin(); i != nodes.end(); ++i)
			if (TreeNode* parent = DPTR(*i)->GetParent()) {
				if (index.empty() || DPTR(parent)->GetChild(index) == *i)
					result.insert(parent);
			}
		return result;
	}

	ParentPointcut(Pointcut* pointcut, const std::string childIndex = "") : pointcut(pointcut), index(childIndex) {}
};

/////////////////////////////////////////////////////////

class DescendantPointcut : public Pointcut {
private:
	Pointcut* pointcut;
public:
	const ASTSet Evaluate(TreeNode* ast, bool includeChildren = true) const {
		const ASTSet nodes = DPTR(pointcut)->Evaluate(ast, includeChildren);
		ASTSet result;
		for (ASTSet::const_iterator i = nodes.begin(); i != nodes.end(); ++i) {
			const ASTSet subtree = Linearizer()(*i);
			result.insert(subtree.begin(), subtree.end());
		}
		return result;
	}

	DescendantPointcut(Pointcut* pointcut) : pointcut(pointcut) {}
};

/////////////////////////////////////////////////////////

class AscendantPointcut : public Pointcut {
private:
	Pointcut* pointcut;
public:
	const ASTSet Evaluate(TreeNode* ast, bool includeChildren = true) const {
		const ASTSet nodes = DPTR(pointcut)->Evaluate(ast, includeChildren);
		ASTSet result;
		for (ASTSet::const_iterator i = nodes.begin(); i != nodes.end(); ++i) {
			TreeNode* parent = DPTR(*i)->GetParent();
			while (parent) {
				result.insert(parent);
				parent = DPTR(*i)->GetParent();
			}
		}
		return result;
	}

	AscendantPointcut(Pointcut* pointcut) : pointcut(pointcut) {}
};

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.