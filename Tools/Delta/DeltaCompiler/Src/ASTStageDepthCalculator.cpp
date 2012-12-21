// ASTStageDepthCalculator.cpp
// Tree visitor for calculating the stage depth.
// ScriptFighter Project.
// Y. Lilis, May 2011.
//
#include "ASTStageDepthCalculator.h"
#include "ASTMetaTags.h"

///////////////////////////////////////////////////////////

#define	_H(name, tag)		\
		SetHandler(tag, &Handle_##name, this)

AST::StageDepthCalculator::StageDepthCalculator (void) {
	// Context independent handlers.
	_H(QuasiQuotes,	AST_TAG_QUASI_QUOTES);
	_H(Escape,		AST_TAG_ESCAPE);
	_H(Staging,		AST_TAG_INLINE);
	_H(Staging,		AST_TAG_EXECUTE);
}

///////////////////////////////////////////////////////////

void AST::StageDepthCalculator::Initialise(void){
	currentStageDepth	= 0;
	maxStageDepth		= 0;
}

void AST::StageDepthCalculator::CleanUp(void){
	currentStageDepth	= 0;
	maxStageDepth		= 0;
}

///////////////////////////////////////////////////////////

unsigned AST::StageDepthCalculator::operator()(AST::Node* root){
	unsigned depth = 0;
	if (root) {
		Initialise();
		DPTR(root)->AcceptPreOrder(this);
		depth = maxStageDepth;
		CleanUp();
	}
	return depth;
}

///////////////////////////////////////////////////////////

void AST::StageDepthCalculator::Handle_QuasiQuotes (AST_VISITOR_ARGS){
	StageDepthCalculator* visitor = (StageDepthCalculator*) closure;
	if (entering)
		visitor->quotes.enter();
	else
		visitor->quotes.exit();
} 

void AST::StageDepthCalculator::Handle_Escape (AST_VISITOR_ARGS){
	StageDepthCalculator* visitor = (StageDepthCalculator*) closure;
	if (CARDINALITY(node) == 1) {
		DASSERT(visitor->quotes.inside() == entering);
		if (entering)
			visitor->quotes.save();
		else
			visitor->quotes.restore();
	}
} 

///////////////////////////////////////////////////////////		

void AST::StageDepthCalculator::Handle_Staging (AST_VISITOR_ARGS){
	StageDepthCalculator* visitor = (StageDepthCalculator*) closure;
	if (!visitor->quotes.inside())
		if (entering)
			++visitor->currentStageDepth;
		else {
			if (visitor->currentStageDepth > visitor->maxStageDepth)
				visitor->maxStageDepth = visitor->currentStageDepth;
			--visitor->currentStageDepth;
		}
}
