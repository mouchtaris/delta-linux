// ASTStageDepthCalculator.h
// Tree visitor for calculating the stage depth.
// ScriptFighter Project.
// Y. Lilis, May 2011.
//

#ifndef	ASTSTAGEDEPTHCALCULATOR_H
#define	ASTSTAGEDEPTHCALCULATOR_H

#include "TreeVisitor.h"
#include "ASTNode.h"
#include "DeltaStdDefs.h"
#include "unestedstate.h"

#include <stack>

/////////////////////////////////////////////////////////

namespace AST {

class StageDepthCalculator : public TreeVisitor {

	private:

	/////////////////////////////////////////////////////////
	// Visitor handlers.

	static void Handle_QuasiQuotes (AST_VISITOR_ARGS);
	static void Handle_Escape (AST_VISITOR_ARGS);
	static void Handle_Staging (AST_VISITOR_ARGS);

	/////////////////////////////////////////////////////////
	// Depth calculation data
	unestedstate<DELTA_MAXNESTED_QUASI_QUOTES>	quotes;
	unsigned									currentStageDepth;
	unsigned									maxStageDepth;
		
	void	Initialise	(void);
	void	CleanUp		(void);

	/////////////////////////////////////////////////////////

	public:
	unsigned operator()(AST::Node* root);

	StageDepthCalculator (void);
	~StageDepthCalculator(){}
};

}

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
