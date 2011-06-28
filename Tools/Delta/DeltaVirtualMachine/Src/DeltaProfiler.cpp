// DeltaProfiler.cpp
// Simple profiling facilities for the Delta language.
// Script Fighter Project.
// A. Savidis, January 2011.
//

#include <stdio.h>
#include "DDebug.h"
#include "DeltaProfiler.h"
#include "ustrings.h"
#include <vector>

DeltaProfiler* DeltaProfiler::singletonPtr = (DeltaProfiler*) 0;

USINGLETON_APISTYLE_IMPL_PUBLICSTDMETHODS(DeltaProfiler)
USINGLETON_APISTYLE_IMPL_GETTER(DeltaProfiler)

/////////////////////////////////////////////////////////////////

DeltaProfiler::DeltaProfiler (void) {}
DeltaProfiler::~DeltaProfiler() {}

/////////////////////////////////////////////////////////////////

void DeltaProfiler::EnterFunc (const std::string& func) 
	{ calls.push_front(CallEntry(func, ugettime())); }

/////////////////////////////////////////////////////////////////

void DeltaProfiler::ExitFunc (void) {
	DASSERT(!calls.empty());
	const std::string&		f	= calls.front().first;
	FunctionMap::iterator	i	= functions.find(f);
	util_ui32				t	= ugettime() - calls.front().second;

	if (i == functions.end())
		functions[f] = ProfileEntry(t, 1);
	else
		i->second.first += t, ++i->second.second;
	calls.pop_front();
}

/////////////////////////////////////////////////////////////////

bool DeltaProfiler::EntryGreaterThan (const ReportEntry& a, const ReportEntry& b) 
	{ return a.second.first > b.second.first; }

/////////////////////////////////////////////////////////////////

const std::string DeltaProfiler::MakeReport (void) const {
	
	std::vector<ReportEntry> v;
	v.reserve(functions.size());
	for (FunctionMap::const_iterator i = functions.begin(); i != functions.end(); ++i)
		v.push_back(*i);

	std::sort(v.begin(), v.end(), EntryGreaterThan);

	std::string report;
	for (std::vector<ReportEntry>::iterator i = v.begin(); i != v.end(); ++i)
		report +=	uconstructstr(
						"Function %s (%u calls) total %u msecs\n", 
						i->first.c_str(), 
						i->second.second, 
						i->second.first
					);
	return report;
}

/////////////////////////////////////////////////////////////////

bool DeltaProfiler::WriteReport (const std::string& path) const {
	if (functions.empty())
		return true;	// No report is actually written when not profiled.
	else
	if (FILE* fp = fopen(path.c_str(), "wt")) {
		fputs(MakeReport().c_str(), fp);
		fclose(fp);
		return true;
	}
	else
		return false;
}

/////////////////////////////////////////////////////////////////

