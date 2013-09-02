// AOPLibrary.cpp
// AOP library for Delta.
// ScriptFighter Project.
// Y. Lilis, August 2013.
//

#include "AOPLibrary.h"
#include "AdviceHandler.h"
#include "ASTTags.h"
#include "PointcutScanner.h"
#include "Pointcut.h"
#include "UtilVisitors.h"
#include <sstream>

/////////////////////////////////////////////////////////

AOPLibrary::HandlerMap AOPLibrary::handlers;

/////////////////////////////////////////////////////////

static Pointcut* ParsePointcut (const std::string& pointcut) {
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

void AOPLibrary::Aspect (TreeNode* target, const std::string& pointcut, AdviceType type, TreeNode* advice) {
	const ASTList asts = Match(target, pointcut);
	for (ASTList::const_iterator i = asts.begin(); i != asts.end(); ++i)
		Advise(*i, type, advice);
}

/////////////////////////////////////////////////////////

AOPLibrary::ASTList AOPLibrary::Match (TreeNode* target, const std::string& pointcut) {
	ASTList result;
	if (Pointcut *p = ParsePointcut(pointcut)) {
		const ASTSet matches = p->Evaluate(target);
		DDELETE(p);
		result.insert(result.end(), matches.begin(), matches.end());
	}
	return result;
}

/////////////////////////////////////////////////////////

void AOPLibrary::Advise (TreeNode* target, AdviceType type, TreeNode* advice) {
	DASSERT(target);
	if (AdviceHandler* handler = handlers[DPTR(target)->GetTag()]) {
		void (AdviceHandler::*func)(TreeNode* ast, TreeNode* advice) const;
		switch (type) {
			case BEFORE:	func = &AdviceHandler::before;	break;
			case AFTER:		func = &AdviceHandler::after;	break;
			case AROUND:	func = &AdviceHandler::around;	break;
			default:		DASSERT(false);
		}
		(DPTR(handler)->*func)(target, advice);
	}
}

/////////////////////////////////////////////////////////

void AOPLibrary::Initialise (void) {
	handlers[AST_TAG_PROGRAM]			= DNEW(ProgramAdviceHandler);
	handlers[AST_TAG_STMTS]				= DNEW(AdviceHandler);
	handlers[AST_TAG_FUNCTION]			= DNEW(FunctionAdviceHandler);
	handlers[AST_TAG_TABLE_CONSTRUCTOR]	= DNEW(ObjectConstructorHandler);
}

/////////////////////////////////////////////////////////

void AOPLibrary::CleanUp (void) {
	for (HandlerMap::const_iterator i = handlers.begin(); i != handlers.end(); ++i)
		DDELETE(i->second);
	handlers.clear();
}
