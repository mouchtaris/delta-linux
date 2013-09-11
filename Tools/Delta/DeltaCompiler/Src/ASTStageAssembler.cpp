// ASTStageAssembler.cpp
// Tree visitor for assembling the stage AST.
// ScriptFighter Project.
// Y. Lilis, May 2011.
//
#include "ASTStageAssembler.h"
#include "ASTTags.h"
#include "ASTMetaTags.h"
#include "DeltaCompErrorDefs.h"
#include "ParseParms.h"
#include "ASTCreationActions.h"
#include "ASTSanitiseVisitor.h"
#include "DeltaStdClassNames.h"

///////////////////////////////////////////////////////////

#define	AST_ATTRIBUTE_SYMBOL	"symbol"

#define VISITOR					((StageAssembler*) closure)
#define SYMBOLS					(VISITOR->symbols)

#undef PARSEPARMS
#define PARSEPARMS				PARSEPARMS_EX(VISITOR->GET_COMPONENT_DIRECTORY())

#undef COMPMESSENGER
#define COMPMESSENGER			COMPMESSENGER_EX(VISITOR->GET_COMPONENT_DIRECTORY())

#undef ASTCREATOR
#define ASTCREATOR				ASTCREATOR_EX(VISITOR->GET_COMPONENT_DIRECTORY())

#define STARTLINE(node)			((AST::Node*)node)->GetStartLine()
#define	yysetsourceinfo()		SetSourceInfo(AST_VISITOR_ACTUALS)

///////////////////////////////////////////////////////////

#define	_H(name, tag)		\
		SetHandler(tag, &Handle_##name, this)

#define	_C(name, tag, id)	\
		SetContextDependentHandler(tag, id, &Handle_##name, this)

///////////////////////////////////////////////////////////

AST::StageAssembler::StageAssembler (ucomponentdirectory* directory) : ucomponentdirectoryclient(directory) {

	// Context independent handlers.
	_H(Function,						AST_TAG_FUNCTION);
	_H(Function,						AST_TAG_LAMBDA_FUNCTION);
	_H(FormalArgs,						AST_TAG_FORMAL_ARGS);
	_H(Compound,						AST_TAG_COMPOUND);
	_H(UsingNamespace,					AST_TAG_USING_NAMESPACE);
	_H(ConstDefExpression,				AST_TAG_CONSTDEF);
	_H(NamespaceLvalue,					AST_TAG_LVALUE_NAMESPACE_IDENT);
	_H(QuasiQuotes,						AST_TAG_QUASI_QUOTES);
	_H(Escape,							AST_TAG_ESCAPE);
	_H(Inline,							AST_TAG_INLINE);
	_H(Execute,							AST_TAG_EXECUTE);
	
	// Context dependent handlers.
	_C(Compound,						AST_TAG_LAMBDA_FUNCTION,		AST_CHILD_EXPR);
	_C(Lvalue,							AST_TAG_LVALUE_IDENT,			AST_CHILD_NAME);
	_C(StaticLvalue,					AST_TAG_LVALUE_STATIC_IDENT,	AST_CHILD_NAME);
	_C(LocalLvalue,						AST_TAG_LVALUE_LOCAL_IDENT,		AST_CHILD_NAME);
	_C(ExceptionVar,					AST_TAG_EXCEPTION_VAR,			AST_CHILD_NAME);
}

///////////////////////////////////////////////////////////

void AST::StageAssembler::OnImpureCrossStageAccess(Symbol* symbol, util_ui32 stage, void* closure)
	{ DELTACOMP_ERROR_STAGE_ACCESS_NON_PURE_FUNCTION(symbol, stage); }

void AST::StageAssembler::Initialise(unsigned depth) {
	symbols.SetImpureCrossStageAccessCallback(umakecallback(&OnImpureCrossStageAccess, this));
	quotes.reset();
	targetStage	= depth;
	stmts		= NIL_NODE;
}

void AST::StageAssembler::CleanUp(void) {
	quotes.reset();
	targetStage	= 0;
	stmts		= NIL_NODE;
}

///////////////////////////////////////////////////////////

AST::Node* AST::StageAssembler::operator()(AST::Node* root, unsigned depth) {
	AST::Node* ast = NIL_NODE;
	if (root) {
		Initialise(depth);
		DPTR(root)->AcceptPreOrder(this);
		//symbols.Print();
		DASSERT(stmts);
		AST::Node* first = static_cast<AST::Node*>(DPTR(stmts)->GetChild(0));
		AST::Node* last = static_cast<AST::Node*>(DPTR(stmts)->GetChild(DPTR(stmts)->GetTotalChildren() - 1));
		AST::Node::Location location(
			DPTR(first)->GetStartLine(),
			DPTR(last)->GetEndLine(),
			DPTR(first)->GetStartPos(),
			DPTR(last)->GetEndPos()
		);
		DPTR(stmts)->SetLocation(location);
		ast = ASTCREATOR_EX(GET_COMPONENT_DIRECTORY()).MakeNode_Program(stmts);
		DPTR(ast)->SetLocation(location);
		CleanUp();
	}
	return ast;
}

///////////////////////////////////////////////////////////

static void PruneExecuteTagsAndImpureInnerFuncs (AST::Node* node) {
	class Pruner : public TreeVisitor {
		unestedstate<DELTA_MAXNESTED_QUASI_QUOTES> quotes;

		static void Handle_QuasiQuotes (AST_VISITOR_ARGS) {
			Pruner* visitor = (Pruner*) closure;
			if (entering)
				visitor->quotes.enter();
			else
				visitor->quotes.exit();
		}

		static void Handle_Execute (AST_VISITOR_ARGS)
			{ if (!((Pruner*) closure)->quotes.inside()) ((Pruner*) closure)->Prune(); }

		static void Handle_Function (AST_VISITOR_ARGS){
			if (TreeAttribute* funcAttribute = node->GetAttribute(AST_ATTRIBUTE_SYMBOL)) {
				Symbol* func = (Symbol*) funcAttribute->GetPointer();
				DASSERT(DPTR(func)->IsCompleted() && !DPTR(func)->PurenessPending());
				if (DPTR(func)->IsImpure() && !DPTR(func)->DependsOnlyOnFunctionClosures(DPTR(func)->MyFunction()))
					((Pruner*) closure)->Prune();
			}
		}

	public:
		void operator()(AST::Node* root) { if (root) DPTR(root)->AcceptPreOrder(this);	}
		Pruner() {
			_H(QuasiQuotes,	AST_TAG_QUASI_QUOTES);
			_H(Execute,		AST_TAG_EXECUTE);
			_H(Function,	AST_TAG_FUNCTION);
			_H(Function,	AST_TAG_LAMBDA_FUNCTION);
		}
	};
	Pruner()(node);
	AST::SanitiseVisitor()(node);
}

//*************************

void AST::StageAssembler::Handle_Function (AST_VISITOR_ARGS) {
	if (!VISITOR->quotes.inside()) {
		TreeNode* funcName = DPTR(node)->GetChild(AST_CHILD_NAME);
		if (funcName && DPTR(funcName)->GetTag() != AST_TAG_NAME)
			return;	//generated name, so ignore function for this stage

		if (entering) {
			const std::string name = funcName ? NAME(funcName) : SYMBOLS.NewTempFuncName();
			Symbol* function = SYMBOLS.NewSymbol(name, SymbolType_Function, STARTLINE(node), (AST::Node*) node);
			SYMBOLS.Install(function);
			node->SetAttribute(AST_ATTRIBUTE_SYMBOL, function);

			const std::string& tag = DPTR(node)->GetTag();
			DASSERT(DPTR(node)->GetParent());
			const std::string& parentTag = DPTR(DPTR(node)->GetParent())->GetTag();
			if (tag			== AST_TAG_LAMBDA_FUNCTION		||
				parentTag	== AST_TAG_FUNCTION_PARENTHESIS ||
				parentTag	== AST_TAG_FUNCTION_EXPR
			)
				HandleSymbolUsage(AST_VISITOR_ACTUALS, function);

			SYMBOLS.PushFunction(function);
		}
		else {
			Symbol* function = SYMBOLS.CurrFunction();
			DASSERT(function->IsFunction());
			function->SetCompleted();
			function->DeterminePureness();

			//TODO: globalize propertly
			unsigned stage = function->GetStage();
			unsigned currStage = SYMBOLS.GetStage();
			if (function->IsPure()														&&
				(function->ShouldGlobalize() || stage == 0 && function->GetScope() == 0)	&&
				(stage == currStage || stage == currStage - 1 || stage == 0)
			)
				GlobalizeFunc(AST_VISITOR_ACTUALS, function);

			SYMBOLS.PopFunction();
		}
	}
}

//*************************

void AST::StageAssembler::Handle_FormalArgs (AST_VISITOR_ARGS) {
	if (!VISITOR->quotes.inside() && entering) {
		SYMBOLS.EnterScope();
		const util_ui32 n = DPTR(node)->GetTotalChildren();
		for (util_ui32 i = 0; i < n; ++i) {
			TreeNode* child = DPTR(node)->GetChild(i);
			DASSERT(child);
			if (DPTR(child)->GetTag() == AST_TAG_NAME) {		//ignore generated names
				const std::string name = NAME(child);
				if (name != AST_VALUE_VARARGS_FORMAL_NAME)
					SYMBOLS.Install(SYMBOLS.NewSymbol(name, SymbolType_Variable, STARTLINE(child)));
			}
		}
		SYMBOLS.ExitScope(false);
	}
} 

//*************************

void AST::StageAssembler::Handle_Compound (AST_VISITOR_ARGS) { 
	if (!VISITOR->quotes.inside())
		if (entering)
			SYMBOLS.EnterScope();
		else
			SYMBOLS.ExitScope();
}

///////////////////////////////////////////////////////////

void AST::StageAssembler::Handle_UsingNamespace (AST_VISITOR_ARGS) {
	//Uncomment the following to have all top level using namespace directives inherited by stages
	//if (!VISITOR->quotes.inside() && !entering && !SYMBOLS.GetStage())
	//	VISITOR->stmts = ASTCREATOR.MakeNode_Stmts(VISITOR->stmts, (AST::Node*) node->Clone());
}

///////////////////////////////////////////////////////////

void AST::StageAssembler::Handle_ConstDefExpression (AST_VISITOR_ARGS) {
	if (!VISITOR->quotes.inside() && !entering) {
		TreeNode* name = DPTR(node)->GetChild(AST_CHILD_NAME	);
		DASSERT(name);
		if (DPTR(name)->GetTag() == AST_TAG_NAME) {
			Symbol* constant = SYMBOLS.NewSymbol(NAME(name), SymbolType_Const, STARTLINE(name));
			SYMBOLS.Install(constant);
			if (!SYMBOLS.GetStage())
				GlobalizeConst(AST_VISITOR_ACTUALS, constant);
		}
	}
} 

///////////////////////////////////////////////////////////

static bool IsGlobalLvalue(TreeNode* node, std::string* name) {
	TreeNode* path, *id;
	if (DPTR(node)->GetTotalChildren() == 2				&&
		(path = DPTR(node)->GetChild(0))					&&
		DPTR(path)->GetTag() == AST_TAG_NAME				&&
		NAME(path) == DELTA_LIBRARYNAMESPACE_SEPARATOR	&&
		(id = DPTR(node)->GetChild(1))					&&
		DPTR(id)->GetTag() == AST_TAG_NAME
	) {
		if (name)
			*name = NAME(id);
		return true;
	}
	else
		return false;
}

void AST::StageAssembler::Handle_NamespaceLvalue (AST_VISITOR_ARGS) {
	std::string name;
	if (!VISITOR->quotes.inside() && !entering) {
		if (IsGlobalLvalue(node, &name)) {
			Symbol* symbol = SYMBOLS.Lookup(name);
			if (!symbol) {	//assume it refers to a generated global variable
				symbol = SYMBOLS.NewSymbol(name, SymbolType_Variable, STARTLINE(node));
				symbol->SetMyFunction((Symbol*) 0);
				SYMBOLS.Install(symbol);
			}
			HandleSymbolUsage(AST_VISITOR_ACTUALS, symbol);
		}
		else if (Symbol* function = SYMBOLS.CurrFunction()) {	//using some external namespace or bytecodepath, so impure
			function->AddDependency(Symbol::NonLocal, (Symbol*) 0);
			function->SetPure(false);
		}
	}
}

///////////////////////////////////////////////////////////

void AST::StageAssembler::Handle_Lvalue (AST_VISITOR_ARGS) {
	if (!VISITOR->quotes.inside() && !entering && DPTR(node)->GetTag() == AST_TAG_NAME) {
		Symbol* symbol = SYMBOLS.Lookup(NAME(node));
		if (!symbol) {
			symbol = SYMBOLS.NewSymbol(NAME(node), SymbolType_Variable, STARTLINE(node));
			DPTR(symbol)->SetNonLocal(true);
			SYMBOLS.Install(symbol);
		}
		HandleSymbolUsage(AST_VISITOR_ACTUALS, symbol);
	}
}

///////////////////////////////////////////////////////////

void AST::StageAssembler::Handle_StaticLvalue (AST_VISITOR_ARGS) {
	if (!VISITOR->quotes.inside() && !entering && DPTR(node)->GetTag() == AST_TAG_NAME)
		SYMBOLS.Install(SYMBOLS.NewSymbol(NAME(node), SymbolType_Variable, STARTLINE(node)));
}

///////////////////////////////////////////////////////////

void AST::StageAssembler::Handle_LocalLvalue (AST_VISITOR_ARGS) {
	if (!VISITOR->quotes.inside() && !entering && DPTR(node)->GetTag() == AST_TAG_NAME)
		SYMBOLS.Install(SYMBOLS.NewSymbol(NAME(node), SymbolType_Variable, STARTLINE(node)));
} 

///////////////////////////////////////////////////////////

void AST::StageAssembler::Handle_ExceptionVar (AST_VISITOR_ARGS) {
	if (!VISITOR->quotes.inside() && !entering && DPTR(node)->GetTag() == AST_TAG_NAME) {
		Symbol* symbol = SYMBOLS.Lookup(NAME(node));
		if (!symbol) {
			symbol = SYMBOLS.NewSymbol(NAME(node), SymbolType_Variable, STARTLINE(node));
			SYMBOLS.Install(symbol);
		}
		HandleSymbolUsage(AST_VISITOR_ACTUALS, symbol);
	}
}

///////////////////////////////////////////////////////////

void AST::StageAssembler::Handle_QuasiQuotes (AST_VISITOR_ARGS) {
	StageAssembler* visitor = (StageAssembler*) closure;
	if (entering)
		visitor->quotes.enter();
	else
		visitor->quotes.exit();
}

void AST::StageAssembler::Handle_Escape (AST_VISITOR_ARGS) {
	StageAssembler* visitor = (StageAssembler*) closure;
	if (CARDINALITY(node) == 1) {
		DASSERT(visitor->quotes.inside() == entering);
		if (entering)
			visitor->quotes.save();
		else
			visitor->quotes.restore();
	}
}

static AST::Node* AddSourceInfo (AST::Node* to, AST::Node* from) {
	DPTR(to)->SetLine(DPTR(from)->GetLine());
	DPTR(to)->SetLocation(DPTR(from)->GetLocation());
	//DPTR(to)->AddSourceReference(AST::Node::SourceInfo(DPTR(from)->GetSource(), DPTR(from)->GetStartLine()));
	return to;
}

AST::Node* AST::StageAssembler::GenerateInlineCode(AST::Node* target, AST::Node* original) {
	using namespace AST;
	AST::Creator& creator = ASTCREATOR_EX(GET_COMPONENT_DIRECTORY());
	NodeList* std = creator.MakeNode_List(
		NIL_NODELIST,
		AddSourceInfo(creator.MakeNode_Name(DELTA_STDLIB_NAMESPACE), original)
	);
	Node* func = creator.MakeNode_NamespaceLvalue(std, AddSourceInfo(creator.MakeNode_Name("inline"), original));
	Node* arg = creator.MakeNode_Argument(target);
	Node* args = creator.MakeNode_ActualArguments(creator.MakeNode_List(NIL_NODELIST, AddSourceInfo(arg, original)));
	Node* call = creator.MakeNode_FunctionCall(AddSourceInfo(func, original), AddSourceInfo(args, original));
	Node* expr = creator.MakeNode_PrimaryExpression(AddSourceInfo(call, original));
	Node* exprListStmt = creator.MakeNode_ExpressionListStmt(
		creator.MakeNode_List(NIL_NODELIST, AddSourceInfo(expr, original))
	);
	Node* stmt = creator.MakeNode_BasicStmt(AddSourceInfo(exprListStmt, original));

	struct ReferenceSetter : public TreeVisitor {
		AST::Node* original;
		AST::Node* target;
		static void Handle_All (AST_VISITOR_ARGS) {			
			if (entering) {
				ReferenceSetter* visitor = (ReferenceSetter*) closure;
				if (node == visitor->target)
					visitor->Leave();
				else {
					AST::Node* original = visitor->original;
					AST::Node* n = (AST::Node*) node;
					DPTR(n)->AddSourceReference(AST::Node::SourceInfo(DPTR(original)->GetSource(), DPTR(original)->GetStartLine()));
					if (const AST::Node::SourceInfoReferences* refs = DPTR(original)->GetSourceReferences())
						DPTR(n)->AddSourceReferences(*refs);
				}
			}
		}
		void operator()(AST::Node* root) { if (root) DPTR(root)->AcceptPreOrder(this); }
		ReferenceSetter (AST::Node* original, AST::Node* target) :
			original(original), target(target) { SetDefaultHandler(&Handle_All, this); }
	};
	(ReferenceSetter(original, target))(DPTR(stmt));

	AST::Node::SourceInfoReferences references;
	const AST::Node::SourceInfoReferences* refs = DPTR(stmt)->GetSourceReferences();
	DASSERT(refs);
	references = *refs;
	references.push_back(AST::Node::SourceInfo("", DPTR(original)->GetStartLine()));
	inlineReferences.push_back(references);

	return AddSourceInfo(stmt, original);
}

void AST::StageAssembler::Handle_Inline (AST_VISITOR_ARGS) {
	if (!VISITOR->quotes.inside())
		if (entering)
			SYMBOLS.EnterStage();
		else {
			if (Symbol* outerFunc = SYMBOLS.PreviousStageCurrFunction()) {
				outerFunc->AddDependency(Symbol::Inline, (Symbol*) 0);
				outerFunc->SetPure(false);
			}
			if (SYMBOLS.GetStage() == VISITOR->targetStage) {
				AST::Node* n = (AST::Node*) node->GetChild(AST_CHILD_EXPR);
				n->RemoveFromParent();
				VISITOR->stmts = ASTCREATOR.MakeNode_Stmts(
					VISITOR->stmts,
					VISITOR->GenerateInlineCode(n, (AST::Node*)node)
				);
				//Leave the empty inline node as a locatable placeholder for the inline result
			}
			SYMBOLS.LeaveStage();
		}
}

void AST::StageAssembler::Handle_Execute (AST_VISITOR_ARGS) {
	if (!VISITOR->quotes.inside())
		if (entering)
			SYMBOLS.EnterStage();
		else {
			if (SYMBOLS.GetStage() == VISITOR->targetStage) {
				AST::Node* n = (AST::Node*) node->GetChild(AST_CHILD_EXPR);
				DASSERT(n);
				n->RemoveFromParent();
				VISITOR->stmts = ASTCREATOR.MakeNode_Stmts(VISITOR->stmts, n);
				VISITOR->Prune();	//Remove the node
			}
			SYMBOLS.LeaveStage();
		}
}

///////////////////////////////////////////////////////////		

void AST::StageAssembler::HandleSymbolUsage(AST_VISITOR_ARGS, Symbol* symbol) {
	yysetsourceinfo();
	unsigned symbolStage = symbol->GetStage();
	unsigned currStage = SYMBOLS.GetStage();
	DASSERT(
		symbolStage == currStage		||
		symbolStage == currStage - 1		||
		symbolStage == 0
	);									//staging visibility sanity check
	if (symbolStage != currStage) {		//found in previous stage or main program
		if (symbol->IsVariable())
			DELTACOMP_ERROR_STAGE_ACCESS_VARIABLE(symbol, currStage, symbolStage);
		else 
		if (symbol->IsFunction()) {
			if (symbol->PurenessPending())
				symbol->AddCrossStageUsage(currStage, STARTLINE(node));
			else 
			if (symbol->IsImpure())
				DELTACOMP_ERROR_STAGE_ACCESS_NON_PURE_FUNCTION(symbol, currStage);

			if (symbol->GetScope()) {
				//TODO: globalize properly
				if (symbol->IsCompleted() && symbol->IsPure() && !symbol->IsGlobalized())
					GlobalizeFunc(AST_VISITOR_ACTUALS, symbol);
				else
					symbol->SetShouldGlobalize();
			}
		}
	}
	else 
	if (Symbol* function = SYMBOLS.CurrFunction()) {
		if (symbol->IsVariable()) {
			if (symbol->IsNonLocal()) {
				function->AddDependency(Symbol::NonLocal, symbol);
				function->SetPure(false);
			}
			else {
				Symbol* myFunction = symbol->MyFunction();
				if (!myFunction) {					//uses global variable
					function->AddDependency(Symbol::GlobalVariable, symbol);
					function->SetPure(false);
				}
				else if (myFunction != function) {	//closure var
					Symbol* closureVar =	DNEWCLASS(
												Symbol, 
												(
													symbol->GetName(),
													symbol->GetType(),
													symbol->GetStage(),
													symbol->GetScope(),
													STARTLINE(node)
												)
											);
					closureVar->SetMyFunction(function);
					SYMBOLS.Install(closureVar);	//install closure symbol to handle closures of closures
					if (myFunction == SYMBOLS.PrevFunction()) {	//valid closure var
						function->AddDependency(Symbol::Closure, symbol);
						function->SetPure(false);
					}
					else
						DELTACOMP_ERROR_INACCESSIBLE_SYMBOL(symbol->GetName(), function->GetName());
				}
			}
		}
		else if (symbol->IsFunction())
			function->ReferTo(symbol);
	}
}

///////////////////////////////////////////////////////////

void AST::StageAssembler::GlobalizeFunc(AST_VISITOR_ARGS, Symbol* func) {
	DASSERT(func->IsFunction());

	func->SetGlobalized();
	AST::Node* funcNode = (AST::Node*) DPTR(func->GetNode())->Clone();
	PruneExecuteTagsAndImpureInnerFuncs(funcNode);

	AST::Node* stmt = ASTCREATOR.MakeNode_FunctionStmt(funcNode);
	stmt->SetLocation(funcNode->GetLocation());
	VISITOR->stmts = ASTCREATOR.MakeNode_Stmts(VISITOR->stmts, stmt);
}

///////////////////////////////////////////////////////////

void AST::StageAssembler::GlobalizeConst(AST_VISITOR_ARGS, Symbol* constant) {
	DASSERT(constant->IsConst());

	constant->SetGlobalized();
	AST::Node* constNode = (AST::Node*) DPTR(node)->Clone();
	PruneExecuteTagsAndImpureInnerFuncs(constNode);

	AST::Node* stmt = ASTCREATOR.MakeNode_ExpressionListStmt(ASTCREATOR.MakeNode_List(NIL_NODELIST, constNode));
	stmt->SetLocation(constNode->GetLocation());
	stmt = ASTCREATOR.MakeNode_BasicStmt(stmt);
	stmt->SetLocation(constNode->GetLocation());
	VISITOR->stmts = ASTCREATOR.MakeNode_Stmts(VISITOR->stmts, stmt);
}

///////////////////////////////////////////////////////////

void AST::StageAssembler::SetSourceInfo (AST_VISITOR_ARGS) {
	AST::Node* n = (AST::Node*) node;
	PARSEPARMS.SetLine(DPTR(n)->GetStartLine());
	if (const AST::Node::SourceInfoReferences* refs = DPTR(n)->GetSourceReferences())
		COMPMESSENGER.SetSourceReferences(*refs);
	else
		COMPMESSENGER.SetSourceReferences();
}
