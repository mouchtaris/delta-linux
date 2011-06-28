// DeltaProfiler.h
// Simple profiling facilities for the Delta language.
// Script Fighter Project.
// A. Savidis, January 2011.
//

#ifndef	DELTAPROFILER_H
#define	DELTAPROFILER_H

#include "utypes.h"
#include "usystem.h"
#include "usingleton.h"
#include "DeltaStdDefs.h"

#include <functional>
#include <string>
#include <list>
#include <map>

/////////////////////////////////////////////////////////////////

class DVM_CLASS DeltaProfiler {
	
	private:
	USINGLETON_APISTYLE_DECLARE_PRIVATESTDMETHODS
	
	typedef std::pair<std::string, util_ui32>		CallEntry;
	typedef std::list<CallEntry>					CallStack;
	typedef std::pair<util_ui32, util_ui32>			ProfileEntry;
	typedef std::pair<std::string, ProfileEntry>	ReportEntry;
	typedef std::map<std::string, ProfileEntry>		FunctionMap;

	FunctionMap				functions;
	CallStack				calls;
	static DeltaProfiler*	singletonPtr;

	static bool				EntryGreaterThan (const ReportEntry& a, const ReportEntry& b);

	/////////////////////////////////////////////////////////////////

	public:
	USINGLETON_APISTYLE_DECLARE_GETTER(DeltaProfiler)
	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS

	void					EnterFunc (const std::string& func);
	void					ExitFunc (void);

	const std::string		MakeReport (void) const;
	bool					WriteReport (const std::string& path) const;

	DeltaProfiler (void);
	~DeltaProfiler();
};

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

