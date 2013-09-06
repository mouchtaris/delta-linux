// AdviceHandler.h
// Classes for inserting advice functionality.
// ScriptFighter Project.
// Y. Lilis, August 2013.
//

#ifndef ADVICE_HANDLER_H
#define ADVICE_HANDLER_H

#include "TreeNode.h"
#include "ASTInjector.h"
#include "DeltaStdDefs.h"
#include "unestedstate.h"

/////////////////////////////////////////////////////////

class AdviceHandler {
protected:
	ASTInjector injector;
	TreeNode* (*escapeExtender)(TreeNode* escape);
	bool extendAroundEscapes;

	static TreeNode* Identity	(TreeNode* ast) { return ast; }
	static TreeNode* ToStmt		(TreeNode* escape);
	static TreeNode* Extend		(TreeNode* child, const std::string& tag, const std::string& childId = "");

	void HandleEscape	(TreeNode* advice, TreeNode* original, const std::string& id) const;
	void HandleProceed	(TreeNode* advice, TreeNode* original) const;
	void HandleRetval	(TreeNode* advice, TreeNode* original) const;

private:
	static TreeNode* CreateASTNode (const std::string& tag, void*) { return TreeNode::New(tag); }
public:
	virtual void before	(TreeNode* ast, TreeNode* advice) const;
	virtual void after	(TreeNode* ast, TreeNode* advice) const;
	virtual void around	(TreeNode* ast, TreeNode* advice) const;

	AdviceHandler(void) : injector(umakecallback(CreateASTNode, (void*) 0)), escapeExtender(ToStmt), extendAroundEscapes(false) {}
};

/////////////////////////////////////////////////////////

class ProgramAdviceHandler : public AdviceHandler {
public:
	void around (TreeNode* ast, TreeNode* advice) const;
};

/////////////////////////////////////////////////////////

class StmtsAdviceHandler : public AdviceHandler {};

/////////////////////////////////////////////////////////

class StmtAdviceHandler : public AdviceHandler {
private:
	void insert	(TreeNode* ast, TreeNode* advice, void (TreeNode::*inserter)(TreeNode*, TreeNode*, const std::string&)) const;
public:
	void before	(TreeNode* ast, TreeNode* advice) const;
	void after	(TreeNode* ast, TreeNode* advice) const;
};

/////////////////////////////////////////////////////////

class FunctionStmtAdviceHandler : public StmtAdviceHandler {
public:
	void before	(TreeNode* ast, TreeNode* advice) const;
	void after	(TreeNode* ast, TreeNode* advice) const;
	void around	(TreeNode* ast, TreeNode* advice) const;
	FunctionStmtAdviceHandler (void) { extendAroundEscapes = true; }
};

/////////////////////////////////////////////////////////

class FunctionAdviceHandler : public AdviceHandler {
private:
	static TreeNode* MakeEscape					(const std::string& escapeName);
	static TreeNode* ToCompoundStmts			(TreeNode* stmts);
	static TreeNode* GetAndCreateCompoundStmts	(TreeNode* ast);

public:
	void before	(TreeNode* ast, TreeNode* advice) const;
	void after	(TreeNode* ast, TreeNode* advice) const;
	void around (TreeNode* ast, TreeNode* advice) const;
};

/////////////////////////////////////////////////////////

class ObjectConstructorHandler : public AdviceHandler {
public:
	void around (TreeNode* ast, TreeNode* advice) const;
	ObjectConstructorHandler (void) { escapeExtender = Identity; }
};

/////////////////////////////////////////////////////////

#endif