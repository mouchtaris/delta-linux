// DebugBreakPointHolder.cpp
// Break point holder implementation.
// ScriptFighter Project.
// A. Savidis, February 2002, addition for new dbugger.
// Refactored to be a separate project, September 2005.
// Refactored to support conditions for breakpoints, November 2006.
//

#include <stdio.h>

#include "DebugBreakPointHolder.h"
#include "DeltaStdDefs.h"
#include "DDebug.h"
#include "ustrings.h"
#include "ulexutil.h"
#include "ufunctors.h"
#include "uerrorclass.h"

#define	ERROR_HANDLER(what, errclass) \
	uerror::GetSingleton().post##errclass( \
		"%s: '%s' error: '%s'!", CURR_UNIT, CURR_CALL, what \
	); \
	goto FAIL;

#define	NO_THREAD_SAFE_CALL()

#define	THREAD_SAFE_CALL() \
	LOCK_BLOCK(breakPointMutex)

////////////////////////////////////////////////////////////////////

#define	CURR_UNIT	"Break point"
#define	CURR_CALL	"load"

bool DeltaBreakPoint::Load (FILE* fp) {

	NO_THREAD_SAFE_CALL();

	std::string s;
	UCHECK_PRIMARY_ERROR(fscanf(fp,"%hu", &line) == 1, "reading source line");
	UCHECK_PRIMARY_ERROR(ureadquotedstring(fp, s), "reading state");

	ustrupper(s);
	if (s == ubool2conststr(true))
		enabled = true;
	else
	if (s == ubool2conststr(false))
		enabled = false;
	else
		UCHECK_PRIMARY_ERROR(false, uconstructstr("read invalid state %s", s.c_str()));

	UCHECK_PRIMARY_ERROR(ureadquotedstring(fp, cond), "reading condition");

	return true;
	FAIL: return false;
}

void DeltaBreakPoint::WriteText (FILE* fp) const {
	fprintf(fp, "%d \"%s\" \"%s\"\n", 
		GetLine(), 
		ubool2conststr(IsEnabled()),
		HasCondition()? uunparseescapesequences(GetCondition()).c_str() : ""
	);
}

////////////////////////////////////////////////////////////////////

DebugBreakPointHolder::DebugBreakPointHolder (void) { unullify(breakPointsDirty); isDirty = false; }
DebugBreakPointHolder::~DebugBreakPointHolder (void) { CleanUpPriv(); }

////////////////////////////////////////////////////////////////////

void DebugBreakPointHolder::CleanUpPriv (void) {

	NO_THREAD_SAFE_CALL();

	std::for_each(
		breakPoints.begin(),
		breakPoints.end(),
		udestroy_second< std::pair<std::string, BreakPointsList*> >()
	);
	breakPoints.clear();
	ResetSourceDirty();
}

////////////////////////////////////////////////////////////////////

DebugBreakPointHolder::BreakPointsList::iterator DebugBreakPointHolder::Find (
		BreakPointsList&	bpts, 
		util_ui16			line
	) {

	return	std::find_if(
				bpts.begin(), 
				bpts.end(), 
				std::bind2nd(DeltaBreakPoint::MatchPred(), line)
			);
}

DebugBreakPointHolder::BreakPointsList::const_iterator DebugBreakPointHolder::Find (
		BreakPointsList&	bpts, 
		util_ui16			line
	) const {

	return	std::find_if(
				bpts.begin(), 
				bpts.end(), 
				std::bind2nd(DeltaBreakPoint::MatchPred(), line)
			);
}

////////////////////////////////////////////////////////////////////

DebugBreakPointHolder::BreakPointsMap::iterator DebugBreakPointHolder::Find (const std::string& src) {
	return std::find_if(
		breakPoints.begin(),
		breakPoints.end(),
		std::bind2nd(MatchSource(), src)
	);
}

DebugBreakPointHolder::BreakPointsMap::const_iterator DebugBreakPointHolder::Find (const std::string& src) const {
	return std::find_if(
		breakPoints.begin(),
		breakPoints.end(),
		std::bind2nd(MatchSource(), src)
	);
}

///////////////////////////////////////////////////////
// Private, no thread safety.
//
void DebugBreakPointHolder::AddPriv (const std::string& source, const DeltaBreakPoint& pt) {

	NO_THREAD_SAFE_CALL();

	BreakPointsList* bpts;
	BreakPointsMap::iterator i = Find(source);

	if (i == breakPoints.end()) {
		bpts = DNEW(BreakPointsList);
		breakPoints[source] = bpts;
	}
	else
		bpts = i->second;

	BreakPointsList::iterator j = Find(*DPTR(bpts), pt.GetLine());
	if (j == DPTR(bpts)->end())
		DPTR(bpts)->push_back(pt);
	else
		*j = pt;
}

///////////////////////////////////////////////////////

bool DebugBreakPointHolder::HasBreakPointsWithStatePriv (bool isEnabled) const{

	NO_THREAD_SAFE_CALL();

	for (BreakPointsMap::const_iterator i = breakPoints.begin(); i != breakPoints.end(); ++i)

			for (	std::list<DeltaBreakPoint>::const_iterator j = DPTR(i->second)->begin();
					j != DPTR(i->second)->end();
					++j		)

					if (j->IsEnabled() == isEnabled)
						return true;
	return false;
}

///////////////////////////////////////////////////////

void DebugBreakPointHolder::Add (const std::string& source, util_ui16 line, const std::string& cond) {

	THREAD_SAFE_CALL();

	BreakPointsList* bpts;
	BreakPointsMap::iterator i = Find(source);
	if (i == breakPoints.end()) {
		bpts = DNEW(BreakPointsList);
		breakPoints[source] = bpts;
	}
	else
		bpts = i->second;
	
	BreakPointsList::iterator j = Find(*DPTR(bpts), line);
	if (j == DPTR(bpts)->end())
		DPTR(bpts)->push_back(DeltaBreakPoint(line, cond));
	else
	if ((cond.empty() && j->HasCondition()) || (cond.size() && !j->HasCondition()))
		j->SetCondition(cond);
}

///////////////////////////////////////////////////////

util_ui16 DebugBreakPointHolder::Total (void) const {
	THREAD_SAFE_CALL();
	return (util_ui16) breakPoints.size();
}

///////////////////////////////////////////////////////

const std::string& DebugBreakPointHolder::GetSource (util_ui16 order) const {
	
	THREAD_SAFE_CALL();

	DASSERT(order < breakPoints.size());

	BreakPointsMap::const_iterator i = breakPoints.begin();
	while (order && i != breakPoints.end())
			++i, --order;

	DASSERT(i != breakPoints.end());
	return i->first;
}	

///////////////////////////////////////////////////////

void DebugBreakPointHolder::Change (const std::string& source, util_ui16 line, util_ui16 newLine) {

	THREAD_SAFE_CALL();

	BreakPointsMap::iterator i = Find(source);
	DASSERT(i != breakPoints.end());
		
	BreakPointsList* bpts = i->second;	
	BreakPointsList::iterator j = Find(*DPTR(bpts), line);
	DASSERT(j != DPTR(bpts)->end());

	j->SetLine(newLine);
}

///////////////////////////////////////////////////////

void DebugBreakPointHolder::Change (const std::string& source, util_ui16 line, const std::string& newCond) {

	THREAD_SAFE_CALL();

	BreakPointsMap::iterator i = Find(source);
	DASSERT(i != breakPoints.end());
		
	BreakPointsList* bpts = i->second;	
	BreakPointsList::iterator j = Find(*DPTR(bpts), line);
	DASSERT(j != DPTR(bpts)->end());

	j->SetCondition(newCond);
}

///////////////////////////////////////////////////////

const DeltaBreakPoint& DebugBreakPointHolder::Get (const std::string& source, util_ui16 line) const {

	THREAD_SAFE_CALL();

	if (!IsSourceDirty(source)) {
		BreakPointsMap::const_iterator i = Find(source);
		DASSERT(i != breakPoints.end());
		SetSourceDirty(source, i->second); 
	}

	BreakPointsList::const_iterator j = Find(*DPTR(breakPointsDirty), line);
	DASSERT(j != DPTR(breakPointsDirty)->end());

	return *j;
}

///////////////////////////////////////////////////////

bool DebugBreakPointHolder::Remove (const std::string& source, util_ui16 line) {

	THREAD_SAFE_CALL();

	BreakPointsMap::iterator i = Find(source);
	if (i == breakPoints.end())
		return false;

	BreakPointsList* bpts = i->second;	
	BreakPointsList::iterator j = Find(*DPTR(bpts), line);
	if (j == DPTR(bpts)->end())			// Bpt not found.
		return false;

	DPTR(bpts)->erase(j);

	if (DPTR(bpts)->empty()) {			// bpts get empty.
	
		breakPoints.erase(i);
		udelete(bpts);

		if (IsSourceDirty(source)) 
			ResetSourceDirty();
	}

	return true;
}

///////////////////////////////////////////////////////
							
bool DebugBreakPointHolder::RemoveAll (const std::string& source) {

	THREAD_SAFE_CALL();

	if (IsSourceDirty(source)) 
		ResetSourceDirty();

	BreakPointsMap::iterator i = Find(source);
	if (i == breakPoints.end())	// VM not found.
		return false;

	BreakPointsList* bpts = i->second;	
	breakPoints.erase(i);
	DDELETE(bpts);

	return true;
}

///////////////////////////////////////////////////////
				
bool DebugBreakPointHolder::SetBreakPointStatePriv (const std::string& source, util_ui16 line, bool state) {

	NO_THREAD_SAFE_CALL();

	BreakPointsMap::iterator i = Find(source);
	if (i == breakPoints.end())
		return false;
	
	BreakPointsList* bpts = i->second;	
	BreakPointsList::iterator j = Find(*DPTR(bpts), line);
	if (j == DPTR(bpts)->end())			// Bpt not found.
		return false;
	
	DeltaBreakPoint::SetStateFunctor()(*j, state);

	return true;
}

///////////////////////////////////////////////////////

bool DebugBreakPointHolder::SetBreakPointStatePriv (const std::string& source, bool state) {

	NO_THREAD_SAFE_CALL();

	BreakPointsMap::iterator i = Find(source);
	if (i == breakPoints.end())
		return false;
	
	BreakPointsList* bpts = i->second;	
	std::for_each(
		DPTR(bpts)->begin(), 
		DPTR(bpts)->end(),
		std::bind2nd(DeltaBreakPoint::SetStateFunctor(), state)
	);

	return true;
}

///////////////////////////////////////////////////////

bool DebugBreakPointHolder::Enable (const std::string& source, util_ui16 line) {
	THREAD_SAFE_CALL();
	return SetBreakPointStatePriv(source, line, true);
}

///////////////////////////////////////////////////////
							
bool DebugBreakPointHolder::EnableAll (const std::string& source) {
	THREAD_SAFE_CALL();
	return SetBreakPointStatePriv(source, true);
}

///////////////////////////////////////////////////////

bool DebugBreakPointHolder::HasEnabledBreakPoints (void) const {
	THREAD_SAFE_CALL();
	return HasBreakPointsWithStatePriv(true);
}

///////////////////////////////////////////////////////
							
bool DebugBreakPointHolder::Disable (const std::string& source, util_ui16 line) {
	THREAD_SAFE_CALL();
	return SetBreakPointStatePriv(source, line, false);
}

///////////////////////////////////////////////////////
							
bool DebugBreakPointHolder::DisableAll (const std::string& source) {
	THREAD_SAFE_CALL();
	return SetBreakPointStatePriv(source, false);
}

///////////////////////////////////////////////////////

bool DebugBreakPointHolder::HasDisabledBreakPoints (void) const {
	THREAD_SAFE_CALL();
	return HasBreakPointsWithStatePriv(false);
}

///////////////////////////////////////////////////////

bool DebugBreakPointHolder::HasBreakPoint (const std::string& source, util_ui16 line) const {

	THREAD_SAFE_CALL();
	
	if (!IsSourceDirty(source)) {
		BreakPointsMap::const_iterator i = Find(source);
		if (i == breakPoints.end())
			return false;
		else
			SetSourceDirty(source, i->second);	
	}

	return Find(*DPTR(breakPointsDirty), line) != DPTR(breakPointsDirty)->end();
}

///////////////////////////////////////////////////////

bool DebugBreakPointHolder::HasAnyBreakPoints (const std::string& source) const {

	THREAD_SAFE_CALL();

	BreakPointsMap::const_iterator i = Find(source);
	return i != breakPoints.end();
}

///////////////////////////////////////////////////////

const DebugBreakPointHolder::BreakPointsList& DebugBreakPointHolder::Get (const std::string& source) const {
	
	THREAD_SAFE_CALL();

	if (!IsSourceDirty(source)) {
		BreakPointsMap::const_iterator i = Find(source);
		DASSERT(i != breakPoints.end());	
		SetSourceDirty(source, i->second);
	}

	return *DPTR(breakPointsDirty);
}

///////////////////////////////////////////////////////
// The textual storage syntax is the following.
// AllBreakPoints :	Total BreakPoints
// BreakPoints:		Source(quoted string) Total BreakPoint+
// BreakPoint:		Line State(0 or 1)
//
bool DebugBreakPointHolder::StoreText (const char* path) const {

	THREAD_SAFE_CALL();

	FILE* fp = fopen(path, "wt");
	if (!fp)
		return false;

	fprintf(fp,"%d\n", breakPoints.size());

	std::for_each(
		breakPoints.begin(),
		breakPoints.end(),
		std::bind2nd(DebugBreakPointHolder::WriteTextFunctor(), fp)
	);

	fclose(fp);
	return true;
}

///////////////////////////////////////////////////////

#undef	CURR_UNIT
#undef	CURR_CALL
#define	CURR_UNIT	"Break point holder"
#define	CURR_CALL	"load"

bool DebugBreakPointHolder::LoadText (const char* path) {

	THREAD_SAFE_CALL();

	FILE* fp = fopen(path, "rt");
	UCHECK_PRIMARY_ERROR(fp, uconstructstr("could not open '%s'", path));

	CleanUp();

	util_ui32 total;
	UCHECK_PRIMARY_ERROR(fscanf(fp, "%u", &total) == 1, "reading 'total sources'");

	while (total--) {

		std::string source;		
		UCHECK_PRIMARY_ERROR(ureadquotedstring(fp, source), "reading 'source name'");

		util_ui32 n;
		UCHECK_PRIMARY_ERROR(fscanf(fp, "%u", &n) == 1, "reading 'total source bpts'");

		for (util_ui32 i = 0; i < n; ++i) {
			DeltaBreakPoint bpt;
			UCHECK_DOMINO_ERROR(
				bpt.Load(fp), 
				uconstructstr("reading bpt #%d for source '%s'", i, source.c_str())
			);
			AddPriv(source, bpt);
		}
	}

	fclose(fp);
	return true;

	FAIL: 
	if (fp) 
		fclose(fp); 
	CleanUpPriv();
	return false;
}

///////////////////////////////////////////////////////

void DebugBreakPointHolder::CleanUp (void) {

	THREAD_SAFE_CALL();

	CleanUpPriv();
}

///////////////////////////////////////////////////////
