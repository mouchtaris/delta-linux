// AdviceHandler.cpp
// Classes for inserting advice functionality.
// ScriptFighter Project.
// Y. Lilis, August 2013.
//

#include "AdviceHandler.h"
#include "ASTAlphaRenamer.h"
#include "ASTTags.h"
#include "UtilVisitors.h"
#include "ASTValidationVisitor.h"

#define AST_TAG_DUMMY	"_"

/////////////////////////////////////////////////////////

TreeNode* AdviceHandler::ToStmt (TreeNode* escape) {
	TreeNode* expr = Extend(escape, AST_TAG_EXPR, AST_CHILD_EXPR);
	TreeNode* exprlist = Extend(expr, AST_TAG_EXPRLIST_STMT, AST_CHILD_EXPR);
	TreeNode* basicStmt = Extend(exprlist, AST_TAG_BASIC_STMT, AST_CHILD_STMT);
	return Extend(basicStmt, AST_TAG_STMT, AST_CHILD_STMT);
}

//***********************

TreeNode* AdviceHandler::Extend (TreeNode* child, const std::string& tag, const std::string& childId) {
	TreeNode* node = TreeNode::New(tag);
	DPTR(node)->PushBack(child, childId);
	return node;
}

//***********************

void AdviceHandler::HandleEscape (TreeNode* advice, TreeNode* original, const std::string& id) const {
	DASSERT(advice && (!original || !DPTR(original)->GetParent()));
	if (TreeNode* escape = EscapeLocator(id)(advice))	//TODO: assumes a single ~proceed
		injector(escape, original);
	else if (original)
		TreeNode::Delete(original);
}

//***********************

void AdviceHandler::HandleProceed (TreeNode* advice, TreeNode* original) const
	{ HandleEscape(advice, original, "proceed"); }

void AdviceHandler::HandleRetval (TreeNode* ast, TreeNode* original) const
	{ HandleEscape(ast, original, "retval"); }

/////////////////////////////////////////////////////////

void AdviceHandler::before (TreeNode* ast, TreeNode* advice) const {
	DASSERT(ast && advice);
	TreeNode* dummy = TreeNode::New(AST_TAG_DUMMY);
	DPTR(ast)->PushFront((*escapeExtender)(dummy));
	injector(dummy, DPTR(advice)->Clone());
}

//***********************

void AdviceHandler::after (TreeNode* ast, TreeNode* advice) const {
	DASSERT(ast && advice);
	TreeNode* dummy = TreeNode::New(AST_TAG_DUMMY);
	DPTR(ast)->PushBack((*escapeExtender)(dummy));
	injector(dummy, DPTR(advice)->Clone());
}

//***********************

void AdviceHandler::around (TreeNode* ast, TreeNode* advice) const {
	DASSERT(ast);
	TreeNode* parent = DPTR(ast)->GetParent();
	DASSERT(parent);
	if (advice) {
		advice = DPTR(advice)->Clone();
		TreeNode* dummy = TreeNode::New(AST_TAG_DUMMY);
		DPTR(parent)->Replace(ast, extendAroundEscapes ? (*escapeExtender)(dummy) : dummy);
		HandleProceed(advice, ast);
		injector(dummy, advice);

		//TODO: ugly fix, do this better somehow
		if (DPTR(parent)->GetTag() == AST_TAG_PRIMARY_EXPRESSION &&
			AST::ValidationVisitor::IsMetaGeneratedCode(DPTR(parent)->GetChild(AST_CHILD_EXPR))
		) {
			
			TreeNode* meta = DPTR(parent)->GetChild(AST_CHILD_EXPR);
			DPTR(meta)->RemoveFromParent();
			DPTR(DPTR(parent)->GetParent())->Replace(parent, meta);
			TreeNode::Delete(parent);
		}
	}
	else {
		DPTR(ast)->RemoveFromParent();
		TreeNode::Delete(ast);
	}
}

/////////////////////////////////////////////////////////

void ProgramAdviceHandler::around (TreeNode* ast, TreeNode* advice) const {
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

/////////////////////////////////////////////////////////

void StmtAdviceHandler::insert (
	TreeNode*		ast,
	TreeNode*		advice,
	void (TreeNode::*inserter)(TreeNode*, TreeNode*, const std::string&)
) const {
	DASSERT(ast && advice);
	if (TreeNode* parent = DPTR(ast)->GetParent()) {
		TreeNode* dummy = TreeNode::New(AST_TAG_DUMMY);
		(DPTR(parent)->*inserter)(ast, (*escapeExtender)(dummy), "");
		injector(dummy, DPTR(advice)->Clone());
	}
}

//***********************

void StmtAdviceHandler::before (TreeNode* ast, TreeNode* advice) const
	{ insert(ast, advice, &TreeNode::InsertBefore); }

//***********************

void StmtAdviceHandler::after (TreeNode* ast, TreeNode* advice) const
	{ insert(ast, advice, &TreeNode::InsertAfter); }

/////////////////////////////////////////////////////////

void FunctionStmtAdviceHandler::before (TreeNode* ast, TreeNode* advice) const {
	TreeNode* stmt = DPTR(ast)->GetParent();
	if (stmt && DPTR(stmt)->GetTag() == AST_TAG_STMT)
		StmtAdviceHandler::before(stmt, advice);
}

//***********************

void FunctionStmtAdviceHandler::after (TreeNode* ast, TreeNode* advice) const {
	TreeNode* stmt = DPTR(ast)->GetParent();
	if (stmt && DPTR(stmt)->GetTag() == AST_TAG_STMT)
		StmtAdviceHandler::after(stmt, advice);
}

//***********************

void FunctionStmtAdviceHandler::around (TreeNode* ast, TreeNode* advice) const {
	TreeNode* stmt = DPTR(ast)->GetParent();
	if (stmt && DPTR(stmt)->GetTag() == AST_TAG_STMT)
		StmtAdviceHandler::around(stmt, advice);
}

/////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////

TreeNode* FunctionAdviceHandler::MakeEscape (const std::string& escapeName) {
	TreeNode* name = TreeNode::New(AST_TAG_NAME);
	DPTR(name)->SetAttribute(AST_ATTRIBUTE_NAME, escapeName);
	TreeNode* ident = Extend(DPTR(name), AST_TAG_LVALUE_IDENT, AST_CHILD_NAME);
	TreeNode* primary = Extend(ident, AST_TAG_PRIMARY_EXPRESSION, AST_CHILD_EXPR);
	TreeNode* escape = Extend(primary, AST_TAG_ESCAPE, AST_CHILD_EXPR);
	DPTR(escape)->SetAttribute(AST_ATTRIBUTE_CARDINALITY, 1u);
	return escape;
}

//***********************

TreeNode* FunctionAdviceHandler::ToCompoundStmts (TreeNode* stmts) {
	TreeNode* compound = Extend(stmts, AST_TAG_COMPOUND, AST_CHILD_STMTS);
	TreeNode* compoundStmt = Extend(compound, AST_TAG_STMT, AST_CHILD_STMT);
	return Extend(compoundStmt, AST_TAG_STMTS);
}

/////////////////////////////////////////////////////////

TreeNode* FunctionAdviceHandler::GetAndCreateCompoundStmts (TreeNode* ast) {
	TreeNode* body = DPTR(ast)->GetChild(AST_CHILD_BODY);
	DASSERT(body);
	TreeNode* stmts = DPTR(body)->GetChild(AST_CHILD_STMTS);
	if (!stmts) {
		stmts = TreeNode::New(AST_TAG_STMTS);
		DPTR(body)->PushBack(stmts, AST_CHILD_STMTS);
	}
	return stmts;
}

//***********************

void FunctionAdviceHandler::before (TreeNode* ast, TreeNode* advice) const {
	DASSERT(ast && DPTR(ast)->GetTag() == AST_TAG_FUNCTION && advice);
	AdviceHandler::before(GetAndCreateCompoundStmts(ast), advice);
}

//***********************

void FunctionAdviceHandler::after (TreeNode* ast, TreeNode* advice) const {
	DASSERT(ast && DPTR(ast)->GetTag() == AST_TAG_FUNCTION && advice);
	TreeNode* body = DPTR(ast)->GetChild(AST_CHILD_BODY);
	DASSERT(body);
	const ASTSet returns = ReturnCollector()(body);
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
			DPTR(name)->SetAttribute(AST_ATTRIBUTE_RENAME, true);
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
			HandleRetval(adviceCopy, DPTR(expr)->Clone());
			injector(exitEscape, adviceCopy);
			AST::AlphaRenamer()(ast);
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
	if (!EscapeLocator("retval")(advice))	//only add the ending advice for code not containing ~retval
		AdviceHandler::after(GetAndCreateCompoundStmts(ast), advice);
}

//***********************

void FunctionAdviceHandler::around (TreeNode* ast, TreeNode* advice) const {
	DASSERT(ast && DPTR(ast)->GetTag() == AST_TAG_FUNCTION);
	AdviceHandler::around(GetAndCreateCompoundStmts(ast), advice);
}

/////////////////////////////////////////////////////////

void ObjectConstructorHandler::around (TreeNode* ast, TreeNode* advice) const {
	DASSERT(ast && DPTR(ast)->GetTag() == AST_TAG_TABLE_CONSTRUCTOR);
	AdviceHandler::around(ast, advice);
}

/////////////////////////////////////////////////////////