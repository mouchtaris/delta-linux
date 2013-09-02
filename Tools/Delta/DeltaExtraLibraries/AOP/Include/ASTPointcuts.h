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
	const ASTSet Evaluate(TreeNode* ast) const;
	ASTPointcut(const std::string& type) : type(type) {}
};

/////////////////////////////////////////////////////////

class AttributePointcut : public Pointcut {
private:
	//TODO
public:
	const ASTSet Evaluate(TreeNode* ast) const;
	AttributePointcut() {}
};

/////////////////////////////////////////////////////////

class ChildPointcut : public Pointcut {
private:
	Pointcut* pointcut;
	const std::string index;
public:
	const ASTSet Evaluate(TreeNode* ast) const;
	ChildPointcut(Pointcut* pointcut, const std::string childIndex = "") : pointcut(pointcut), index(childIndex) {}
};

/////////////////////////////////////////////////////////

class ParentPointcut : public Pointcut {
private:
	Pointcut* pointcut;
	const std::string index;
public:
	const ASTSet Evaluate(TreeNode* ast) const;
	ParentPointcut(Pointcut* pointcut, const std::string childIndex = "") : pointcut(pointcut), index(childIndex) {}
};

/////////////////////////////////////////////////////////

class DescendantPointcut : public Pointcut {
private:
	Pointcut* pointcut;
public:
	const ASTSet Evaluate(TreeNode* ast) const;
	DescendantPointcut(Pointcut* pointcut) : pointcut(pointcut) {}
};

/////////////////////////////////////////////////////////

class AscendantPointcut : public Pointcut {
private:
	Pointcut* pointcut;
public:
	const ASTSet Evaluate(TreeNode* ast) const;
	AscendantPointcut(Pointcut* pointcut) : pointcut(pointcut) {}
};

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.