// ASTEscapeTranslationVisitor.h
// Tree visitor for translating escapes
// within quasi-quotes.
// ScriptFighter Project.
// Y. Lilis, June 2011.
//

#ifndef	ASTESCAPETRANSLATIONVISITOR_H
#define	ASTESCAPETRANSLATIONVISITOR_H

#include "TreeVisitor.h"
#include "ASTNode.h"
#include "ASTEvaluationHelpers.h"
#include "ASTStackValue.h"

/////////////////////////////////////////////////////////

namespace AST {

class EscapeTranslationVisitor :	public ucomponentdirectoryclient,
									public TreeVisitor {
	public:

	/////////////////////////////////////////////////////////
	// Translation value stack.

	typedef std::pair<TreeNode*, StackValue> EscapeStackValue;

	struct IsUndefFunctor {
		bool operator()(const EscapeStackValue& v) const
			{ return v.second.GetType() == StackValue::undef_t; }
	};

	typedef AST::EvaluationStack<EscapeStackValue, IsUndefFunctor> EvaluationStack;

	private:

	/////////////////////////////////////////////////////////
	// Translation handlers.

	static void Handle_Escape (AST_VISITOR_ARGS);
	
	/////////////////////////////////////////////////////////
	// Members.

	EvaluationStack	evalStack;
	bool hasEscapes;

	/////////////////////////////////////////////////////////

	public:
	bool operator()(AST::Node* root);

	EvaluationStack& GetEvalStack (void) { return evalStack; }

	EscapeTranslationVisitor (ucomponentdirectory* directory);
	~EscapeTranslationVisitor();
};

}

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
