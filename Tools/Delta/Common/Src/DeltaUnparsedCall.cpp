// DeltaUnparsedCall.cpp
// Part of program debug information.
// ScriptFighter Project.
// A. Savidis, September 2008.
//

#include "DeltaUnparsedCall.h"
#include "ufiles.h"
#include "uerrorclass.h"

////////////////////////////////////////////////////////

void DeltaUnparsedCall::Write (GenericWriter& writer) const {
	writer.write(func);
	writer.write(args);
	writer.write(label);
}

////////////////////////////////////////////////////////

void DeltaUnparsedCall::WriteText (FILE* fp) const {
	fprintf(
		fp,
		"\t%s(%s), ADDR %d.\n", 
		func.c_str(), 
		args.c_str(), 
		label
	);
}

////////////////////////////////////////////////////////

#define	ERROR_HANDLER(what, errclass) \
	uerror::GetSingleton().post##errclass( \
		"Loading (debug info) unparsed call: error in reading '%s'!", what \
	); goto FAIL; \

bool DeltaUnparsedCall::Read (GenericReader& reader) {

	UCHECK_PRIMARY_ERROR(reader.read(func, false), "function");
	UCHECK_PRIMARY_ERROR(reader.read(args, false), "arguments");
	UCHECK_PRIMARY_ERROR(reader.read(&label), "label");

	return true;
	FAIL: return false;
}

////////////////////////////////////////////////////////
