// SelectiveStepInPreparator.cpp
// Singleton to prepare selective step-in debug information.
// ScriptFighter Project.
// A. Savidis, September 2008.
//

#include "SelectiveStepInPreparator.h"
#include "InterCode.h"
#include "ufunctors.h"
#include <list>
#include <algorithm>

///////////////////////////////////////////////////////

SelectiveStepInPreparator::SelectiveStepInPreparator(ucomponentdirectory* directory) :
	ucomponentdirectoryclient(directory), currStmt((DeltaUnparsedCallsInStmt*) 0)
{
	stmtStack = DNEW(std::list<DeltaUnparsedCallsInStmt*>);
	allStmts = DNEW(DeltaUnparsedCallsInStmtHolder);
}

SelectiveStepInPreparator::~SelectiveStepInPreparator()
	{ udelete(stmtStack); udelete(allStmts); }

DeltaUnparsedCallsInStmtHolder* SelectiveStepInPreparator:: GetAllStmts (void)
	{ DASSERT(stmtStack && allStmts); return allStmts; }

///////////////////////////////////////////////////////

void SelectiveStepInPreparator::Initialise (void) {
	DASSERT(stmtStack && allStmts);
	DASSERT(!currStmt);
	stmtStack->push_front(currStmt = DNEW(DeltaUnparsedCallsInStmt));
	DASSERT(!DPTR(allStmts)->GetTotal()); 
}

void SelectiveStepInPreparator::CleanUp (void) {
	DASSERT(stmtStack && allStmts);
	unullify(currStmt);
	std::for_each(
		DPTR(stmtStack)->begin(),
		DPTR(stmtStack)->end(),
		uptrdestructorfunctor<DeltaUnparsedCallsInStmt*>()
	);
	DPTR(stmtStack)->clear();
	DPTR(allStmts)->Clear();
}

///////////////////////////////////////////////////////

void SelectiveStepInPreparator::OnCallDone (
		const std::string&	func,
		const std::string&	args,
		util_ui32			label
		) {
	DASSERT(currStmt && allStmts);
	DPTR(currStmt)->Add(
		DeltaUnparsedCall(
			func,
			args,
			label
		)
	);
}

///////////////////////////////////////////////////////

void SelectiveStepInPreparator::OnFunctionEnter (void) 
	{ DPTR(stmtStack)->push_front(currStmt = DNEW(DeltaUnparsedCallsInStmt)); }

void SelectiveStepInPreparator::OnFunctionExit (void) {
	DASSERT(!DPTR(stmtStack)->empty() && DPTR(stmtStack)->front() == currStmt);
	DDELETE(currStmt);
	DPTR(stmtStack)->pop_front();
	DASSERT(!DPTR(stmtStack)->empty());
	currStmt = DPTR(stmtStack)->front();
}

///////////////////////////////////////////////////////

void SelectiveStepInPreparator::OnStmtDone (util_ui32 line) {

	if (!DPTR(currStmt)->GetTotal())			// No calls, simply ingore.
		return;

	if (!DPTR(allStmts)->Has(line)) {
		if (DPTR(currStmt)->GetTotal() == 1)	// We end up with a single call stmt, so ingore it.
			{ DPTR(currStmt)->Clear(); return; }
		else
			DPTR(allStmts)->New(line);
	}

	DeltaUnparsedCallsInStmt*		stmt	= DPTR(allStmts)->Get(line);
	std::list<DeltaUnparsedCall>&	calls	= DPTR(currStmt)->GetAllCalls();

	for (std::list<DeltaUnparsedCall>::iterator i = calls.begin(); i != calls.end(); ++i)
		if (!QUADS.GetQuad(i->GetLabel()).IsDropped())
			DPTR(stmt)->Add(*i);
	
	DPTR(currStmt)->Clear();
}

///////////////////////////////////////////////////////
// Substitute in all call records the label that was set with an i-code address
// to its respective t-code address.
//
void SelectiveStepInPreparator::OnTargetCodeProduced (void) {
	
	DASSERT(currStmt && allStmts);
	
	std::list<util_ui32> lines;
	DPTR(allStmts)->GetLines(lines);

	for (std::list<util_ui32>::iterator i = lines.begin(); i != lines.end(); ++i) {

		DeltaUnparsedCallsInStmt* stmt = DPTR(allStmts)->Get(*i);
		DASSERT(stmt);
		std::list<DeltaUnparsedCall>& calls = DPTR(stmt)->GetAllCalls();

		for (std::list<DeltaUnparsedCall>::iterator j = calls.begin(); j != calls.end(); ++j) {

			DeltaCodeAddress tcodeAddress = QUADS.GetQuad(j->GetLabel()).targetLabel;
			DASSERT(tcodeAddress);
			j->SetLabel(tcodeAddress);
		}
	}
}

///////////////////////////////////////////////////////
