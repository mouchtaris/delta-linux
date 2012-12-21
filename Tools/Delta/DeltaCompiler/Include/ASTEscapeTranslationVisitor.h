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

class EscapeTranslationVisitor : public TreeVisitor {
	USE_COMPILER_COMPONENT_DIRECTORY();
	private:

	/////////////////////////////////////////////////////////
	// Translation handlers.

	static void Handle_Escape (AST_VISITOR_ARGS);

	/////////////////////////////////////////////////////////
	// Translation value stack.

	struct IsUndefFunctor {
		bool operator()(const StackValue& v) const
			{ return v.GetType() == StackValue::undef_t; }
	};
	
	typedef AST::EvaluationStack<StackValue, IsUndefFunctor> EvaluationStack;
	EvaluationStack	evalStack;
	bool hasEscapes;

	/////////////////////////////////////////////////////////

	public:
	bool operator()(AST::Node* root);

	EvaluationStack& GetEvalStack (void) { return evalStack; }

	EscapeTranslationVisitor (void);
	~EscapeTranslationVisitor();
};

}

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
