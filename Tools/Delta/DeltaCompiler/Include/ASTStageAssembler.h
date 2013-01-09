// ASTStageAssembler.h
// Tree visitor for assembling the stage AST.
// ScriptFighter Project.
// Y. Lilis, May 2011.
//

#ifndef	ASTSTAGASSEMBLER_H
#define	ASTSTAGASSEMBLER_H

#include "TreeVisitor.h"
#include "CompilerComponentDirectory.h"
#include "ASTNode.h"
#include "DeltaStdDefs.h"
#include "unestedstate.h"
#include "StagedSymbolTable.h"

/////////////////////////////////////////////////////////

namespace AST {

class StageAssembler :	public ucomponentdirectoryclient,
						public TreeVisitor {
	private:

	static void OnImpureCrossStageAccess(Symbol* symbol, util_ui32 stage, void* closure);

	/////////////////////////////////////////////////////////
	// Visitor handlers.

	static void Handle_Function (AST_VISITOR_ARGS);
	static void Handle_FormalArgs (AST_VISITOR_ARGS);
	static void Handle_Compound (AST_VISITOR_ARGS);
	static void Handle_UsingNamespace (AST_VISITOR_ARGS);
	static void Handle_NamespaceLvalue (AST_VISITOR_ARGS);
	static void Handle_ConstDefExpression (AST_VISITOR_ARGS);
	static void Handle_Lvalue (AST_VISITOR_ARGS);
	static void Handle_StaticLvalue (AST_VISITOR_ARGS);
	static void Handle_LocalLvalue (AST_VISITOR_ARGS);
	static void Handle_ExceptionVar (AST_VISITOR_ARGS);
	static void Handle_QuasiQuotes (AST_VISITOR_ARGS);
	static void Handle_Escape (AST_VISITOR_ARGS);
	static void Handle_Inline (AST_VISITOR_ARGS);
	static void Handle_Execute (AST_VISITOR_ARGS);
	static void HandleSymbolUsage(AST_VISITOR_ARGS, Symbol* symbol);
	
	static void GlobalizeFunc (AST_VISITOR_ARGS, Symbol* func);
	static void GlobalizeConst(AST_VISITOR_ARGS, Symbol* constant);
	static void SetSourceInfo (AST_VISITOR_ARGS);

	/////////////////////////////////////////////////////////

	unestedstate<DELTA_MAXNESTED_QUASI_QUOTES>	quotes;
	StagedSymbolTable							symbols;
	unsigned									targetStage;
	AST::Node*									stmts;

	typedef std::list<AST::Node::SourceInfoReferences> InlineReferences;
	InlineReferences inlineReferences;
	
	void		Initialise	(unsigned depth);
	void		CleanUp		(void);

	AST::Node*	GenerateInlineCode (AST::Node* target, AST::Node* original);

	/////////////////////////////////////////////////////////

	public:
	AST::Node* operator()(AST::Node* root, unsigned depth);

	const InlineReferences& GetInlineReferences (void) const
		{ return inlineReferences; }

	StageAssembler (ucomponentdirectory* directory);
	~StageAssembler(){}
};

}

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
