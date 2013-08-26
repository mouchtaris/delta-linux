// AOPLibrary.cpp
// AOP library for Delta.
// ScriptFighter Project.
// Y. Lilis, August 2013.
//

#include "AOPLibrary.h"

/////////////////////////////////////////////////////////

bool AOPLibrary::IsValidAdviceType (const std::string& str)
	{ return str == BEFORE_STR || str == AFTER_STR || str == AROUND_STR; }

AOPLibrary::AdviceType AOPLibrary::ToAdviceType (const std::string& str) {
	if (str == BEFORE_STR)
		return BEFORE;
	else if (str == AFTER_STR)
		return AFTER;
	else {
		DASSERT(str == AROUND_STR);
		return AROUND;
	}
}

/////////////////////////////////////////////////////////

void AOPLibrary::Aspect(TreeNode* target, const std::string& pointcut, AdviceType type, TreeNode* advice) {
	const ASTList asts = Match(target, pointcut);
	for (ASTList::const_iterator i = asts.begin(); i != asts.end(); ++i)
		Advise(*i, type, advice);
}

/////////////////////////////////////////////////////////

AOPLibrary::ASTList AOPLibrary::Match(TreeNode* target, const std::string& pointcut) {
	//Pointcut *p;
	//list = p->Evaluate(target);

	return ASTList();
}

/////////////////////////////////////////////////////////

void AOPLibrary::Advise(TreeNode* target, AdviceType type, TreeNode* advice) {
}