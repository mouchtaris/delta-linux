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
	const IdList		formalsPattern;
public:
	const ASTSet Evaluate(TreeNode* ast) const;
	ExecutionPointcut(const std::string& funcClass, const std::string& name, const IdList& formals) :
		funcClassPattern(funcClass), namePattern(name), formalsPattern(formals) {}
};

/////////////////////////////////////////////////////////

class CallPointcut : public Pointcut {
protected:
	const IdList argsPattern;
public:
	static const std::string GetChildName (TreeNode* node);
	bool MatchArgs (TreeNode* call) const;
	CallPointcut(const IdList& args) : argsPattern(args) {}
};

/////////////////////////////////////////////////////////

class NormalCallPointcut : public CallPointcut {
private:
	const IdList namePattern;
	static bool MatchQualifiedName(TreeNode* node, const IdList& namePattern);
public:
	const ASTSet Evaluate(TreeNode* ast) const;
	NormalCallPointcut(const IdList& qualifiedName, const IdList& args) :
		CallPointcut(args), namePattern(qualifiedName) {}
};

/////////////////////////////////////////////////////////

class MethodCallPointcut : public CallPointcut {
private:
	const std::string	objectPattern;
	const std::string	namePattern;
	bool				localLookup;

	static const std::string	GetDotIndexIdent		(TreeNode* node);
	static const std::string	GetBracketConstString	(TreeNode* node);
	static bool IsMethodCall (TreeNode* call, std::string* object, std::string* method, bool *localLookup);
public:
	const ASTSet Evaluate(TreeNode* ast) const;
	MethodCallPointcut(const std::string& object, const std::string& name, const IdList& args, bool localLookup) :
		CallPointcut(args), objectPattern(object), namePattern(name), localLookup(localLookup) {}
};

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.