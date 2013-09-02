// ASTPointcuts.cpp
// ScriptFighter Project.
// Y. Lilis, August 2013.
//

#include "ASTPointcuts.h"
#include "UtilVisitors.h"

/////////////////////////////////////////////////////////

const ASTSet ASTPointcut::Evaluate(TreeNode* ast, bool includeChildren) const {
	ASTSet result;
	if (includeChildren)
		result = NodeCollector(type)(ast);
	else if (DPTR(ast)->GetTag() == type)
		result.insert(ast);
	return result;
}

/////////////////////////////////////////////////////////

const ASTSet AttributePointcut::Evaluate(TreeNode* ast, bool includeChildren) const {
	return ASTSet();
}

/////////////////////////////////////////////////////////

const ASTSet ChildPointcut::Evaluate(TreeNode* ast, bool includeChildren) const {
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

/////////////////////////////////////////////////////////

const ASTSet ParentPointcut::Evaluate(TreeNode* ast, bool includeChildren) const {
	const ASTSet nodes = DPTR(pointcut)->Evaluate(ast, includeChildren);
	ASTSet result;
	for (ASTSet::const_iterator i = nodes.begin(); i != nodes.end(); ++i)
		if (TreeNode* parent = DPTR(*i)->GetParent()) {
			if (index.empty() || DPTR(parent)->GetChild(index) == *i)
				result.insert(parent);
		}
	return result;
}

/////////////////////////////////////////////////////////

const ASTSet DescendantPointcut::Evaluate(TreeNode* ast, bool includeChildren) const {
	const ASTSet nodes = DPTR(pointcut)->Evaluate(ast, includeChildren);
	ASTSet result;
	for (ASTSet::const_iterator i = nodes.begin(); i != nodes.end(); ++i) {
		const ASTSet subtree = Linearizer()(*i);
		result.insert(subtree.begin(), subtree.end());
	}
	return result;
}

/////////////////////////////////////////////////////////

const ASTSet AscendantPointcut::Evaluate(TreeNode* ast, bool includeChildren) const {
	const ASTSet nodes = DPTR(pointcut)->Evaluate(ast, includeChildren);
	ASTSet result;
	for (ASTSet::const_iterator i = nodes.begin(); i != nodes.end(); ++i) {
		TreeNode* parent = DPTR(*i)->GetParent();
		while (parent != ast) {	//do not go beyond the original root
			result.insert(parent);
			parent = DPTR(*i)->GetParent();
			DASSERT(parent);
		}
	}
	return result;
}

/////////////////////////////////////////////////////////
