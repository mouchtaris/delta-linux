// SelectiveStepInPreparator.h
// Singleton to prepare selective step-in debug information.
// ScriptFighter Project.
// A. Savidis, September 2008.
//

#ifndef	SELECTIVESTEPINPREPARATOR_H
#define	SELECTIVESTEPINPREPARATOR_H

#include "DeltaUnparsedCallsInStmt.h"
#include "DeltaUnparsedCallsInStmtHolder.h"
#include "usingleton.h"
#include <list>

///////////////////////////////////////////////////////

class SelectiveStepInPreparator {

	private:
	static std::list<DeltaUnparsedCallsInStmt*>*	stmtStack;
	static DeltaUnparsedCallsInStmt*				currStmt;
	static DeltaUnparsedCallsInStmtHolder*			allStmts;

	///////////////////////////////////////////////////////

	public:
	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS
	static void			Initialise (void);
	static void			CleanUp (void);

	static void			OnFunctionEnter (void);
	static void			OnFunctionExit (void);

	static void			OnCallDone (
							const std::string&	func,
							const std::string&	args,
							util_ui32			label
						);

	static void			OnStmtDone (util_ui32 line);
	static void			OnTargetCodeProduced (void);

	static DeltaUnparsedCallsInStmtHolder*
						GetAllStmts (void);
};

///////////////////////////////////////////////////////

#endif	// Do not ad stuff beyond this point.
