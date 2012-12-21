// ASTAlphaRenamer.cpp
// Tree visitor for alpha renaming variables.
// ScriptFighter Project.
// Y. Lilis, September 2011.
//
#include "ASTAlphaRenamer.h"
#include "ASTTags.h"
#include "ASTMetaTags.h"
#include "ustrings.h"

///////////////////////////////////////////////////////////

#define VISITOR					((AlphaRenamer*) closure)
#define SYMBOLS					(VISITOR->symbols)
#define STARTLINE(node)			0	//no need for line information

///////////////////////////////////////////////////////////

#define	_H(name, tag)		\
		SetHandler(tag, &Handle_##name, this)

#define	_C(name, tag, id)	\
		SetContextDependentHandler(tag, id, &Handle_##name, this)

///////////////////////////////////////////////////////////

static inline bool ShouldRename (TreeNode* node) {
	TreeAttribute* attr = DPTR(node)->GetAttribute(AST_ATTRIBUTE_RENAME);
	return attr && DPTR(attr)->GetBool();
}

const std::string AST::AlphaRenamer::AlphaRename (const std::string& name) {
	DASSERT(!name.empty() && name[0] == '$');
	static int renameCounter = 0;
	return uconstructstr("%s_%d", name.substr(1).c_str(), renameCounter++);
}

const std::string AST::AlphaRenamer::GetNodeName (TreeNode* node)
	{ return (ShouldRename(node) ? "$" : "") + NAME(node); }

void AST::AlphaRenamer::RenameNode (TreeNode* node, const std::string& newName) {
	DASSERT(node && DPTR(node)->GetTag() == AST_TAG_NAME);
	DPTR(node)->RemoveAttribute(AST_ATTRIBUTE_RENAME);
	DPTR(node)->SetAttribute(AST_ATTRIBUTE_NAME, newName);
}

void AST::AlphaRenamer::OnHide (Symbol* symbol, void* closure) {
	SymbolRenameMap::iterator i = VISITOR->symbolsToBeRenamed.find(symbol);
	if (i != VISITOR->symbolsToBeRenamed.end()) {
		VISITOR->RenamePendingSymbol(symbol);
		VISITOR->symbolsToBeRenamed.erase(i);
	}
}

///////////////////////////////////////////////////////////

AST::AlphaRenamer::AlphaRenamer (void) {

	// Context independent handlers.
	_H(FormalArgs,						AST_TAG_FORMAL_ARGS);
	_H(Compound,						AST_TAG_COMPOUND);
	_H(QuasiQuotes,						AST_TAG_QUASI_QUOTES);
	_H(Escape,							AST_TAG_ESCAPE);
	_H(Staged,							AST_TAG_INLINE);
	_H(Staged,							AST_TAG_EXECUTE);

	// Context dependent handlers.
	_C(Compound,						AST_TAG_LAMBDA_FUNCTION,		AST_CHILD_EXPR);
	_C(Function,						AST_TAG_FUNCTION,				AST_CHILD_NAME);
	_C(ConstDefExpression,				AST_TAG_CONSTDEF,				AST_CHILD_NAME);
	_C(Lvalue,							AST_TAG_LVALUE_IDENT,			AST_CHILD_NAME);
	_C(StaticLvalue,					AST_TAG_LVALUE_STATIC_IDENT,	AST_CHILD_NAME);
	_C(LocalLvalue,						AST_TAG_LVALUE_LOCAL_IDENT,		AST_CHILD_NAME);
	_C(Lvalue,							AST_TAG_EXCEPTION_VAR,			AST_CHILD_NAME);

	symbols.SetSymbolHideCallback(umakecallback(&OnHide, this));
}

///////////////////////////////////////////////////////////

void AST::AlphaRenamer::operator()(TreeNode* root) {
	if (root) {
		quotes.reset();
		DPTR(root)->AcceptPreOrder(this);
		//Global symbols are not hidden so manually rename them
		SymbolRenameMap copy(symbolsToBeRenamed.begin(), symbolsToBeRenamed.end());
		for (SymbolRenameMap::const_iterator i = copy.begin(); i != copy.end(); ++i)
			OnHide(i->first, this);
		DASSERT(symbolsToBeRenamed.empty());
	}
}

///////////////////////////////////////////////////////////

void AST::AlphaRenamer::Handle_Function (AST_VISITOR_ARGS) {
	if (!VISITOR->quotes.inside() && !entering && DPTR(node)->GetTag() == AST_TAG_NAME)
		VISITOR->NewSymbol(node, SymbolType_Function);
}

//*************************

void AST::AlphaRenamer::Handle_FormalArgs (AST_VISITOR_ARGS) {
	if (!VISITOR->quotes.inside() && entering) {
		SYMBOLS.EnterScope();
		const util_ui32 n = DPTR(node)->GetTotalChildren();
		for (util_ui32 i = 0; i < n; ++i) {
			TreeNode* child = DPTR(node)->GetChild(i);
			DASSERT(child);
			if (DPTR(child)->GetTag() == AST_TAG_NAME && GetNodeName(child) != AST_VALUE_VARARGS_FORMAL_NAME)
				VISITOR->NewSymbol(child, SymbolType_Variable);
		}
		SYMBOLS.ExitScope(false);
	}
} 

//*************************

void AST::AlphaRenamer::Handle_Compound (AST_VISITOR_ARGS) { 
	if (!VISITOR->quotes.inside())
		if (entering)
			SYMBOLS.EnterScope();
		else
			SYMBOLS.ExitScope();
}

///////////////////////////////////////////////////////////

void AST::AlphaRenamer::Handle_ConstDefExpression (AST_VISITOR_ARGS) {
	if (!VISITOR->quotes.inside() && !entering && DPTR(node)->GetTag() == AST_TAG_NAME)
		VISITOR->NewSymbol(node, SymbolType_Const);
} 

///////////////////////////////////////////////////////////

void AST::AlphaRenamer::Handle_Lvalue (AST_VISITOR_ARGS) {
	if (!VISITOR->quotes.inside() && !entering && DPTR(node)->GetTag() == AST_TAG_NAME) {
		if (Symbol* symbol = SYMBOLS.Lookup(GetNodeName(node))) {
			if (ShouldRename(node))
				VISITOR->symbolsToBeRenamed[symbol].second.insert(node);
		}
		else
			VISITOR->NewSymbol(node, SymbolType_Variable);
	}
}

void AST::AlphaRenamer::Handle_StaticLvalue (AST_VISITOR_ARGS) {
	if (!VISITOR->quotes.inside() && !entering && DPTR(node)->GetTag() == AST_TAG_NAME)
		VISITOR->NewSymbol(node, SymbolType_Variable);
}

///////////////////////////////////////////////////////////

void AST::AlphaRenamer::Handle_LocalLvalue (AST_VISITOR_ARGS) {
	if (!VISITOR->quotes.inside() && !entering && DPTR(node)->GetTag() == AST_TAG_NAME) {
		if (Symbol* symbol = SYMBOLS.Lookup(GetNodeName(node), SYMBOLS.GetStage(), SYMBOLS.GetScope())) {
			if (ShouldRename(node))
				VISITOR->symbolsToBeRenamed[symbol].second.insert(node);
		}
		else
			VISITOR->NewSymbol(node, SymbolType_Variable);
	}
} 

///////////////////////////////////////////////////////////

void AST::AlphaRenamer::Handle_QuasiQuotes (AST_VISITOR_ARGS) {
	AlphaRenamer* visitor = (AlphaRenamer*) closure;
	if (entering)
		visitor->quotes.enter();
	else
		visitor->quotes.exit();
} 

void AST::AlphaRenamer::Handle_Escape (AST_VISITOR_ARGS) {
	AlphaRenamer* visitor = (AlphaRenamer*) closure;
	if (CARDINALITY(node) == 1) {
		DASSERT(visitor->quotes.inside() == entering);
		if (entering)
			visitor->quotes.save();
		else
			visitor->quotes.restore();
	}
}

void AST::AlphaRenamer::Handle_Staged (AST_VISITOR_ARGS) {
	if (!VISITOR->quotes.inside())
		if (entering)
			SYMBOLS.EnterStage();
		else
			SYMBOLS.LeaveStage();
}

///////////////////////////////////////////////////////////

void AST::AlphaRenamer::RenamePendingSymbol (Symbol* symbol) {
	SymbolRenameMap::const_iterator i = symbolsToBeRenamed.find(symbol);
	DASSERT(i != symbolsToBeRenamed.end());

	const std::string name = symbol->GetName();
	std::string newName;
	do { newName = AlphaRename(name); }
	while (symbols.Lookup(newName) || i->second.first.find(newName) != i->second.first.end());
	
	symbol->SetNewName(newName);
	for (std::set<TreeNode*>::const_iterator j = i->second.second.begin(); j != i->second.second.end(); ++j)
		RenameNode(*j, newName);
}

void AST::AlphaRenamer::CheckPossibleVariableCapture (const std::string& name) {
	for (SymbolRenameMap::iterator i = symbolsToBeRenamed.begin(); i != symbolsToBeRenamed.end(); ++i) {
		std::string n = i->first->GetName();
		DASSERT(!n.empty() && n[0] == '$');
		n = n.substr(1);
		if (name.compare(0, n.length(), n) == 0)
			i->second.first.insert(name);
	}
}

void AST::AlphaRenamer::NewSymbol (TreeNode* node, SymbolType type) {
	DASSERT(node && DPTR(node)->GetTag() == AST_TAG_NAME);
	const std::string name = GetNodeName(node);
	Symbol* symbol = symbols.NewSymbol(name, type, STARTLINE(node));
	if (ShouldRename(node))
		symbolsToBeRenamed[symbol].second.insert(node);
	else 
		CheckPossibleVariableCapture(name);
	symbols.Install(symbol);
}
