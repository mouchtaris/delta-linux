// AOPLibrary.cpp
// AOP library for Delta.
// ScriptFighter Project.
// Y. Lilis, August 2013.
//

#include "AOPLibrary.h"
#include "PointcutScanner.h"
#include "Pointcut.h"

#include <sstream>

/////////////////////////////////////////////////////////

static Pointcut* ParsePointcut(const std::string& pointcut) {
	Pointcut* p = (Pointcut*) 0;
	std::istringstream input(pointcut, std::ios::in);
	if (!input.fail()) {
		extern int PointcutSyntax_yydebug;
		extern int PointcutSyntax_yyparse (Pointcut** pointcut, PointcutScannerFlexLexer& lexer);
		PointcutScannerFlexLexer lexer;
		lexer.switch_streams(&input, 0);
		lexer.set_position(0);
		PointcutSyntax_yydebug = 0;	
		PointcutSyntax_yyparse(&p, lexer);
	}
	return p;
}
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
	ASTList result;
	if (Pointcut *p = ParsePointcut(pointcut)) {
		const ASTSet matches = p->Evaluate(target);
		result.insert(result.end(), matches.begin(), matches.end());
	}
	return result;
}

/////////////////////////////////////////////////////////

void AOPLibrary::Advise(TreeNode* target, AdviceType type, TreeNode* advice) {

}