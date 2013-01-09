// ASTEscapeTranslationVisitor.cpp
// Tree visitor for translating escapes
// within quasi-quotes.
// ScriptFighter Project.
// Y. Lilis, June 2011.
//

#include "ASTEscapeTranslationVisitor.h"
#include "ASTTranslationVisitor.h"
#include "ASTTags.h"
#include "ASTMetaTags.h"

///////////////////////////////////////////////////////////

#define VISITOR		DNULLCHECK((AST::EscapeTranslationVisitor*) closure)

#define Rule		AST::EscapeTranslationVisitor::EvaluationStack::TopRule
#define	EvalStack()	VISITOR->GetEvalStack()
#define	yyrule		Rule yy(EvalStack().GetValues());		\
					yy = Rule::RuleSize()
#define	yypush		EvalStack().Set(__LINE__, node->GetTag().c_str()).Push

///////////////////////////////////////////////////////////

AST::EscapeTranslationVisitor::EscapeTranslationVisitor (ucomponentdirectory* directory) :
	ucomponentdirectoryclient(directory), hasEscapes(false)
{
	SetHandler(AST_TAG_ESCAPE, &Handle_Escape, this);
	GetEvalStack().Initialise();
}

AST::EscapeTranslationVisitor::~EscapeTranslationVisitor (void) { GetEvalStack().CleanUp(); }

///////////////////////////////////////////////////////////

bool AST::EscapeTranslationVisitor::operator()(AST::Node* root) {
	hasEscapes = false;
	if (root)
		DPTR(root)->AcceptPreOrder(this);
	return hasEscapes;
}

///////////////////////////////////////////////////////////

void AST::EscapeTranslationVisitor::Handle_Escape (AST_VISITOR_ARGS) {
	DASSERT(CARDINALITY(node) >= 1);
	VISITOR->hasEscapes = true;
	if (entering && CARDINALITY(node) == 1) {
		yyrule;
		TranslationVisitor visitor(VISITOR->GET_COMPONENT_DIRECTORY());
		visitor((Node *) node->GetChild(AST_CHILD_EXPR));
		yypush(visitor.GetEvalStack().Top());
		((AST::EscapeTranslationVisitor *) closure)->Leave();
	}
} 

///////////////////////////////////////////////////////////
