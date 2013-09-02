// AOPLibrary.cpp
// AOP library for Delta.
// ScriptFighter Project.
// Y. Lilis, August 2013.
//

#include "AOPLibrary.h"
#include "PointcutScanner.h"
#include "Pointcut.h"
#include "UtilVisitors.h"
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
		DDELETE(p);
		result.insert(result.end(), matches.begin(), matches.end());
	}
	return result;
}

/////////////////////////////////////////////////////////

#include "ASTInjector.h"
#include "DeltaMetaCompiler.h"

#define AST_TAG_DUMMY	"_"

class AdviceHandler {
protected:
	ASTInjector injector;

	void HandleProceed(TreeNode* advice, TreeNode* original) const {
		DASSERT(advice && original && !DPTR(original)->GetParent());
		if (TreeNode* escape = EscapeLocator()(advice))
			injector(escape, original);
		else
			TreeNode::Delete(original);
	}

	void HandleRetval(TreeNode* advice, TreeNode* original) const {
		HandleProceed(advice, original);
		//TODO: fix this if the injection is different for ~~proceed or ~~retval
	}
private:
	static TreeNode* CreateASTNode (const std::string& tag, void*) { return TreeNode::New(tag); }
public:
	virtual void before	(TreeNode* ast, TreeNode* advice) const {
		DASSERT(ast && advice);
		TreeNode* dummy = TreeNode::New(AST_TAG_DUMMY);
		DPTR(ast)->PushFront(dummy);	
		injector(dummy, DPTR(advice)->Clone());
	}
	virtual void after	(TreeNode* ast, TreeNode* advice) const {
		DASSERT(ast && advice);
		TreeNode* dummy = TreeNode::New(AST_TAG_DUMMY);
		DPTR(ast)->PushBack(dummy);
		injector(dummy, DPTR(advice)->Clone());
	}

	virtual void around	(TreeNode* ast, TreeNode* advice) const {
		DASSERT(ast);
		TreeNode* parent = DPTR(ast)->GetParent();
		DASSERT(parent);
		if (advice) {
			advice = DPTR(advice)->Clone();
			DPTR(parent)->Replace(ast, advice);
			HandleProceed(advice, ast);
		}
		else {
			DPTR(ast)->RemoveFromParent();
			TreeNode::Delete(ast);
		}
	}

	AdviceHandler(void) : injector(umakecallback(CreateASTNode, (void*) 0)) {}
};

class ProgramAdviceHandler : public AdviceHandler {
public:
	void around (TreeNode* ast, TreeNode* advice) const {
		DASSERT(ast);
		TreeNode* stmts = DPTR(ast)->GetChild(AST_CHILD_STMTS);
		if (stmts)
			DPTR(stmts)->RemoveFromParent();

		advice = advice ? DPTR(advice)->Clone() : (TreeNode*) 0;
		HandleProceed(advice, stmts);
		TreeNode* dummy = TreeNode::New(AST_TAG_DUMMY);
		DPTR(ast)->PushFront(dummy);	
		injector(dummy, advice);
	}
};

#include "DeltaStdDefs.h"
#include "unestedstate.h"

struct ReturnCollector : public TreeVisitor {
	ASTSet nodes;
	unestedstate<DELTA_MAXNESTED_QUASI_QUOTES> quotes;
	util_ui32 funcNesting;

	static void Handle_QuasiQuotes (AST_VISITOR_ARGS) {
		if (entering) ((ReturnCollector*) closure)->quotes.enter();
		else ((ReturnCollector*) closure)->quotes.exit();
	}

	static void Handle_Escape (AST_VISITOR_ARGS) {
		ReturnCollector* visitor = (ReturnCollector*) closure;
		if (CARDINALITY(node) == 1) {
			DASSERT(visitor->quotes.inside() == entering);
			if (entering)
				visitor->quotes.save();
			else
				visitor->quotes.restore();
		}
	}

	static void Handle_Function (AST_VISITOR_ARGS) {
		ReturnCollector* visitor = (ReturnCollector*) closure;
		if (!visitor->quotes.inside())
			if (entering) ++visitor->funcNesting;
			else { DASSERT(visitor->funcNesting > 0); --visitor->funcNesting; }
	}

	static void Handle_Return (AST_VISITOR_ARGS) {
		if (entering && !((ReturnCollector*) closure)->quotes.inside() && !((ReturnCollector*) closure)->funcNesting)
			((ReturnCollector*) closure)->nodes.insert(node);
	}

	ASTSet operator()(TreeNode* root) { if (root) DPTR(root)->AcceptPreOrder(this); return nodes; }
	ReturnCollector(void) : funcNesting(0) {
		SetHandler(AST_TAG_QUASI_QUOTES,	&Handle_QuasiQuotes,	this);
		SetHandler(AST_TAG_ESCAPE,			&Handle_Escape,			this);
		SetHandler(AST_TAG_FUNCTION,		&Handle_Function,		this);
		SetHandler(AST_TAG_LAMBDA_FUNCTION, &Handle_Function,		this);
		SetHandler(AST_TAG_RETURN,			&Handle_Return,			this);
	}
};

static TreeNode* Extend(TreeNode* child, const std::string& tag, const std::string& childId = "") {
	TreeNode* node = TreeNode::New(tag);
	DPTR(node)->PushBack(child, childId);
	return node;
}

#include "ASTValidationVisitor.h"

class FunctionAdviceHandler : public AdviceHandler {
private:

	static TreeNode* MakeEscape (const std::string& escapeName) {
		TreeNode* name = TreeNode::New(AST_TAG_NAME);
		DPTR(name)->SetAttribute(AST_ATTRIBUTE_NAME, escapeName);
		TreeNode* ident = Extend(DPTR(name), AST_TAG_LVALUE_IDENT, AST_CHILD_NAME);
		TreeNode* primary = Extend(ident, AST_TAG_PRIMARY_EXPRESSION, AST_CHILD_EXPR);
		TreeNode* escape = Extend(primary, AST_TAG_ESCAPE, AST_CHILD_EXPR);
		DPTR(escape)->SetAttribute(AST_ATTRIBUTE_CARDINALITY, 1u);
		return escape;
	}

	static TreeNode* ToStmt(TreeNode* escape) {
		TreeNode* expr = Extend(escape, AST_TAG_EXPR, AST_CHILD_EXPR);
		TreeNode* exprlist = Extend(expr, AST_TAG_EXPRLIST_STMT, AST_CHILD_EXPR);
		TreeNode* basicStmt = Extend(exprlist, AST_TAG_BASIC_STMT, AST_CHILD_STMT);
		return Extend(basicStmt, AST_TAG_STMT, AST_CHILD_STMT);
	}

	static TreeNode* ToCompoundStmts(TreeNode* stmts) {
		TreeNode* compound = Extend(stmts, AST_TAG_COMPOUND, AST_CHILD_STMTS);
		TreeNode* compoundStmt = Extend(compound, AST_TAG_STMT, AST_CHILD_STMT);
		return Extend(compoundStmt, AST_TAG_STMTS);
	}

public:
	void after (TreeNode* ast, TreeNode* advice) const {
		DASSERT(ast && DPTR(ast)->GetTag() == AST_TAG_FUNCTION && advice);
		const ASTSet returns = ReturnCollector()(DPTR(ast)->GetChild(AST_CHILD_BODY));
		for (ASTSet::const_iterator i = returns.begin(); i != returns.end(); ++i) {
			TreeNode* parent = DPTR(*i)->GetParent();
			DASSERT(parent);
			TreeNode* grandparent = DPTR(parent)->GetParent();
			DASSERT(grandparent);
			TreeNode* dummyEscape = TreeNode::New(AST_TAG_DUMMY);
			TreeNode* dummy = Extend(dummyEscape, AST_TAG_EXPR, AST_CHILD_EXPR);
			dummy = Extend(dummy, AST_TAG_EXPRLIST_STMT, AST_CHILD_EXPR);
			DPTR(parent)->Replace(*i, dummy);

			TreeNode* adviceCopy = DPTR(advice)->Clone();
			if ((*i)->GetTotalChildren()) { //return expr; --> <<{ local $retval = ~expr; ~exit; return $retval; }>>
				//assignment
				TreeNode* name = TreeNode::New(AST_TAG_NAME);
				DPTR(name)->SetAttribute(AST_ATTRIBUTE_NAME, "retval");
				//DPTR(name)->SetAttribute(AST_ATTRIBUTE_RENAME, true);
				TreeNode* localId = Extend(name, AST_TAG_LVALUE_LOCAL_IDENT, AST_CHILD_NAME);
				TreeNode* retvalEscape = MakeEscape("retval");
				TreeNode* assignment = TreeNode::New(AST_TAG_ASSIGN);
				DPTR(assignment)->PushBack(localId, AST_CHILD_LVALUE);
				DPTR(assignment)->PushBack(retvalEscape, AST_CHILD_RVALUE);
				//exit
				TreeNode* exitEscape = MakeEscape("exit");
				TreeNode* exit = ToStmt(exitEscape);
				//return
				TreeNode* ident = Extend(DPTR(name)->Clone(), AST_TAG_LVALUE_IDENT, AST_CHILD_NAME);
				TreeNode* expr = Extend(ident, AST_TAG_PRIMARY_EXPRESSION, AST_CHILD_EXPR);
				TreeNode* ret = Extend(expr, AST_TAG_RETURN, AST_CHILD_EXPR);

				TreeNode* stmts = TreeNode::New(AST_TAG_STMTS);
				DPTR(stmts)->PushBack(ToStmt(assignment));
				DPTR(stmts)->PushBack(exit);
				DPTR(stmts)->PushBack(Extend(ret, AST_TAG_STMT, AST_CHILD_STMT));
				
				injector(dummyEscape, ToCompoundStmts(stmts));
				TreeNode* returnExpr = DPTR(*i)->GetChild(AST_CHILD_EXPR);
				DPTR(returnExpr)->RemoveFromParent();
				TreeNode::Delete((TreeNode*&)*i);
				injector(retvalEscape, returnExpr);
				injector(exitEscape, adviceCopy);
			}
			else {	//return; -> <<{ ~exit; return; }>>
				TreeNode* escape = MakeEscape("exit");
				TreeNode* ret = Extend(TreeNode::New(AST_TAG_RETURN), AST_TAG_BASIC_STMT, AST_CHILD_STMT);
				TreeNode* stmts = TreeNode::New(AST_TAG_STMTS);
				DPTR(stmts)->PushBack(ToStmt(escape));
				DPTR(stmts)->PushBack(Extend(ret, AST_TAG_STMT, AST_CHILD_STMT));
				injector(dummyEscape, ToCompoundStmts(stmts));
				injector(escape, adviceCopy);
			}
		}
	}
	void around (TreeNode* ast, TreeNode* advice) const {
		DASSERT(ast);
		TreeNode* stmts = DPTR(ast)->GetChild(AST_CHILD_STMTS);
		if (stmts)
			DPTR(stmts)->RemoveFromParent();

		advice = advice ? DPTR(advice)->Clone() : (TreeNode*) 0;
		HandleProceed(advice, stmts);
		TreeNode* dummy = TreeNode::New(AST_TAG_DUMMY);
		DPTR(ast)->PushFront(dummy);	
		injector(dummy, advice);
	}
};

#include "ASTTags.h"

void AOPLibrary::Advise(TreeNode* target, AdviceType type, TreeNode* advice) {
	typedef std::map<std::string, AdviceHandler*> HandlerMap;
	HandlerMap handlers;
	handlers[AST_TAG_PROGRAM] = DNEW(ProgramAdviceHandler);
	handlers[AST_TAG_FUNCTION] = DNEW(FunctionAdviceHandler);

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

	for (HandlerMap::const_iterator i = handlers.begin(); i != handlers.end(); ++i)
		DDELETE(i->second);
	handlers.clear();
}
