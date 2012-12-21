// ASTAlphaRenamer.h
// Tree visitor for alpha renaming variables.
// ScriptFighter Project.
// Y. Lilis, September 2011.
//

#ifndef	ASTALPHARENAMER_H
#define	ASTALPHARENAMER_H

#include "TreeVisitor.h"
#include "DeltaStdDefs.h"
#include "unestedstate.h"
#include "StagedSymbolTable.h"

/////////////////////////////////////////////////////////

namespace AST {

class SYNTAXTREELIB_CLASS AlphaRenamer : public TreeVisitor {

	private:

	/////////////////////////////////////////////////////////
	// Visitor handlers.

	static void Handle_Function (AST_VISITOR_ARGS);
	static void Handle_FormalArgs (AST_VISITOR_ARGS);
	static void Handle_Compound (AST_VISITOR_ARGS);
	static void Handle_ConstDefExpression (AST_VISITOR_ARGS);
	static void Handle_Lvalue (AST_VISITOR_ARGS);
	static void Handle_StaticLvalue (AST_VISITOR_ARGS);
	static void Handle_LocalLvalue (AST_VISITOR_ARGS);
	static void Handle_QuasiQuotes (AST_VISITOR_ARGS);
	static void Handle_Escape (AST_VISITOR_ARGS);
	static void Handle_Staged (AST_VISITOR_ARGS);

	/////////////////////////////////////////////////////////

	static const std::string	AlphaRename (const std::string& name);
	static const std::string	GetNodeName (TreeNode* node);
	static void					RenameNode	(TreeNode* node, const std::string& newName);
	static void					OnHide (Symbol* symbol, void* closure);
	
	void						RenamePendingSymbol			(Symbol* symbol);
	void						CheckPossibleVariableCapture(const std::string& name);
	void						NewSymbol					(TreeNode* node, SymbolType type);

	/////////////////////////////////////////////////////////

	unestedstate<DELTA_MAXNESTED_QUASI_QUOTES>			quotes;
	StagedSymbolTable									symbols;

	typedef std::pair< std::set<std::string>, std::set<TreeNode*> > SymbolInfo;
	typedef std::map<Symbol*, SymbolInfo>							SymbolRenameMap;
	SymbolRenameMap													symbolsToBeRenamed;


	/////////////////////////////////////////////////////////

	public:
	void operator()(TreeNode* root);

	AlphaRenamer (void);
	~AlphaRenamer(){}
};

}

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
