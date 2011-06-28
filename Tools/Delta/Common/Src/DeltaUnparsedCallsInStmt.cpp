// DeltaUnparsedCallsInStmt.cpp
// Part of program debug information.
// ScriptFighter Project.
// A. Savidis, September 2008.
//

#include "DeltaUnparsedCallsInStmt.h"
#include "ufiles.h"
#include "uerrorclass.h"
#include <algorithm> 
#include <functional>

////////////////////////////////////////////////////////

DeltaUnparsedCallsInStmt::DeltaUnparsedCallsInStmt (const DeltaUnparsedCallsInStmt& stmt) :
	calls(stmt.calls),
	line(stmt.line){}

////////////////////////////////////////////////////////

void DeltaUnparsedCallsInStmt::GetCallsAfterInstruction (
		DeltaCodeAddress				addr,
		std::list<DeltaUnparsedCall>&	output
	) const {

	for (std::list<DeltaUnparsedCall>::const_iterator i = calls.begin(); i != calls.end(); ++i)
		if (i->GetLabel() >= addr)
			output.push_back(*i);
}

////////////////////////////////////////////////////////

void DeltaUnparsedCallsInStmt::Write (GenericWriter& writer) const {
	writer.write(line);
	writer.write(GetTotal());
	for (std::list<DeltaUnparsedCall>::const_iterator i = calls.begin(); i != calls.end(); ++i)
		i->Write(writer);
}

////////////////////////////////////////////////////////

void DeltaUnparsedCallsInStmt::WriteText (FILE* fp) const {
	fprintf(fp,"%d CALLS FOR STMT AT LINE %d.\n",  GetTotal(), line);
	std::for_each(
		calls.begin(),
		calls.end(),
		std::bind2nd(std::mem_fun_ref(&DeltaUnparsedCall::WriteText), fp)
	);
}

////////////////////////////////////////////////////////

#define	ERROR_HANDLER(what, errclass) \
	uerror::GetSingleton().post##errclass( \
		"Loading (debug info) unparsed calls in stmt: error in reading '%s'!", what \
	); goto FAIL; \


bool DeltaUnparsedCallsInStmt::Read (GenericReader& reader) {

	UCHECK_PRIMARY_ERROR(reader.read(&line), "stmt line");
	util_ui16 total;
	UCHECK_PRIMARY_ERROR(reader.read(&total), "total calls");
	
	for (util_ui16 i = 0; i < total; ++i) {
		DeltaUnparsedCall call;
		UCHECK_DOMINO_ERROR(call.Read(reader), uconstructstr("call #%d", i) );
		Add(call);
	}

	return true;
	FAIL: return false;
}

////////////////////////////////////////////////////////
