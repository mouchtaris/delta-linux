// DeltaUnparsedCallsInStmtHolder.cpp
// ScriptFighter Project.
// A. Savidis, September 2008.
//

#include "DeltaUnparsedCallsInStmtHolder.h"
#include "ufiles.h"
#include "uerrorclass.h"
#include "ufunctors.h"
#include <algorithm> 
#include <functional>

DeltaUnparsedCallsInStmtHolder::DeltaUnparsedCallsInStmtHolder (void) {}

DeltaUnparsedCallsInStmtHolder::~DeltaUnparsedCallsInStmtHolder() 
	{ Clear(); }

////////////////////////////////////////////////////////

DeltaUnparsedCallsInStmtHolder::DeltaUnparsedCallsInStmtHolder (const DeltaUnparsedCallsInStmtHolder& holder) {
	for (StmtMap::const_iterator i = holder.stmts.begin(); i != holder.stmts.end(); ++i)
		stmts[i->first] = DNEWCLASS(DeltaUnparsedCallsInStmt, (*DPTR(i->second)));
}

////////////////////////////////////////////////////////

void DeltaUnparsedCallsInStmtHolder::Clear (void) {
	std::for_each(
		stmts.begin(), 
		stmts.end(),
		udestroy_second< std::pair<util_ui32, DeltaUnparsedCallsInStmt*> >()
	);
	stmts.clear();
}

////////////////////////////////////////////////////////

void DeltaUnparsedCallsInStmtHolder::Write (GenericWriter& writer) const {

	writer.write((util_ui16) stmts.size());

	for (StmtMap::const_iterator i = stmts.begin(); i != stmts.end(); ++i)
		DPTR(i->second)->Write(writer);
}

////////////////////////////////////////////////////////

void DeltaUnparsedCallsInStmtHolder::GetLines (std::list<util_ui32>& lines) const {
	for (StmtMap::const_iterator i = stmts.begin(); i != stmts.end(); ++i)
		lines.push_back(i->first);
}

////////////////////////////////////////////////////////

void DeltaUnparsedCallsInStmtHolder::WriteText (FILE* fp) const {
	for (StmtMap::const_iterator i = stmts.begin(); i != stmts.end(); ++i)
		DPTR(i->second)->WriteText(fp);
}

////////////////////////////////////////////////////////

#define	ERROR_HANDLER(what, errclass) \
	uerror::GetSingleton().post##errclass( \
		"Loading (debug info) holder of unparsed calls in stmt: error in reading '%s'!", what \
	); goto FAIL; \


bool DeltaUnparsedCallsInStmtHolder::Read (GenericReader& reader) {

	Clear();
	util_ui16 total;
	UCHECK_PRIMARY_ERROR(reader.read(&total), "total stmts");
	
	for (util_ui16 i = 0; i < total; ++i) {
		DeltaUnparsedCallsInStmt* stmt = DNEWCLASS(DeltaUnparsedCallsInStmt, (0));
		UCHECK_DOMINO_ERROR_EX(
			DPTR(stmt)->Read(reader), 
			uconstructstr("stmt #%d", i),
			DDELETE(stmt)
		);
		stmts[DPTR(stmt)->GetLine()] = stmt;
	}

	return true;
	FAIL: return false;
}

////////////////////////////////////////////////////////

void DeltaUnparsedCallsInStmtHolder::New (util_ui32 line) {
	DASSERT(!Has(line));
	stmts[line] = DNEWCLASS(DeltaUnparsedCallsInStmt, (line));
}

////////////////////////////////////////////////////////

void DeltaUnparsedCallsInStmtHolder::Delete (util_ui32 line) {
	StmtMap::iterator i = stmts.find(line);
	DASSERT(i != stmts.end());
	stmts.erase(i);
}

////////////////////////////////////////////////////////

DeltaUnparsedCallsInStmt* DeltaUnparsedCallsInStmtHolder::Get (util_ui32 line) {
	StmtMap::iterator i = stmts.find(line);
	DASSERT(i != stmts.end());
	return i->second;
}

const DeltaUnparsedCallsInStmt& DeltaUnparsedCallsInStmtHolder::Get (util_ui32 line) const {
	StmtMap::const_iterator i = stmts.find(line);
	DASSERT(i != stmts.end());
	return *DPTR(i->second);
}

////////////////////////////////////////////////////////
