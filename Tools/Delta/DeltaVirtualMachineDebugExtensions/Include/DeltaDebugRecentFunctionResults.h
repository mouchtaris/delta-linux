// DeltaDebugRecentFunctionResults.h
// Handling of recent function results during tracing.
// Debug support extensions.
// Script Fighter Project.
// A. Savidis, June 2007 (original).
// RF to separate class (July 2008).
//

#ifndef	DELTADEBUGRECENTFUNCTIONRESULTS_H
#define	DELTADEBUGRECENTFUNCTIONRESULTS_H

#include "DeltaVirtualMachine.h"
#include "utypes.h"
#include <list>
#include <stack>

/////////////////////////////////////////////////////////////////

class DeltaDebugRecentFunctionResults {

	private:
	typedef std::list< std::pair<std::string, std::string> > ResultValueList;

	ResultValueList			recentFuncResults;
	std::stack<std::string>	funcNameStack;
	std::string				previousStopContext;
	util_ui16				funcsAlreadyAtStopPoint;

	/////////////////////////////////////////////////////////////////

	public:

	void					EnteringNewStopPoint (void);
	void					EnteringRecentlyCalledFunction (const std::string& name, DeltaVirtualMachine* vm);
	void					ExitingRecentlyCalledFunction (const std::string& name, DeltaVirtualMachine* vm);
	void					ExitingRecentlyCalledFunction (void)
								{ ExitingRecentlyCalledFunction("", (DeltaVirtualMachine*) 0); }

	void					Clear (void);

	const ResultValueList&	GetResults (void) const
								{ return recentFuncResults; }

	DeltaDebugRecentFunctionResults (void);
	~DeltaDebugRecentFunctionResults();
};

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
