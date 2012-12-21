// SelectiveStepInPreparator.h
// Singleton to prepare selective step-in debug information.
// ScriptFighter Project.
// A. Savidis, September 2008.
//

#ifndef	SELECTIVESTEPINPREPARATOR_H
#define	SELECTIVESTEPINPREPARATOR_H

#include "CompilerComponentDirectory.h"
#include "DeltaUnparsedCallsInStmt.h"
#include "DeltaUnparsedCallsInStmtHolder.h"
#include <list>

///////////////////////////////////////////////////////

class SelectiveStepInPreparator {

	USE_COMPILER_COMPONENT_DIRECTORY();

	private:
	std::list<DeltaUnparsedCallsInStmt*>*	stmtStack;
	DeltaUnparsedCallsInStmt*				currStmt;
	DeltaUnparsedCallsInStmtHolder*			allStmts;

	///////////////////////////////////////////////////////

	public:
	void	OnFunctionEnter (void);
	void	OnFunctionExit (void);

	void	OnCallDone (
				const std::string&	func,
				const std::string&	args,
				util_ui32			label
			);

	void	OnStmtDone (util_ui32 line);
	void	OnTargetCodeProduced (void);

	DeltaUnparsedCallsInStmtHolder* GetAllStmts (void);

	///////////////////////////////////////////////////////

	void	Initialise (void);
	void	CleanUp (void);

	SelectiveStepInPreparator (void);
	~SelectiveStepInPreparator();
};

///////////////////////////////////////////////////////

#define SELECTIVESTEPIN_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), SelectiveStepInPreparator)))

#define SELECTIVESTEPIN	SELECTIVESTEPIN_EX(COMPONENT_DIRECTORY())

///////////////////////////////////////////////////////

#endif	// Do not ad stuff beyond this point.
