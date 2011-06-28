// DeltaLocal.cpp
// ScriptFighter Project.
// A. Savidis, Feburary 2005.
// Refactored July 2007, adding error propagation.
//

#include "DeltaLocal.h"
#include "uerrorclass.h"
#include "ufiles.h"

////////////////////////////////////////////////////////

void DeltaDebugLocal::Write (GenericWriter& writer) const {
	writer.write(name);
	writer.write(offset);
	writer.write(line);
}

void DeltaDebugLocal::WriteText (FILE* fp) const {
	fprintf(fp,"\tLOCAL %s, OFFSET %d, LINE %d.\n", name.c_str(), offset, line);
}

////////////////////////////////////////////////////////

#define	ERROR_HANDLER(what, errclass) \
	uerror::GetSingleton().post##errclass( \
		"Loading (debug info) local var: error in reading '%s'!", what \
	); return false; \

bool DeltaDebugLocal::Read (GenericReader& reader) {
	UCHECK_PRIMARY_ERROR(reader.read(name, false), "name");
	UCHECK_PRIMARY_ERROR(reader.read(&offset), "offset");
	UCHECK_PRIMARY_ERROR(reader.read(&line), "line");
	return true;
}

////////////////////////////////////////////////////////
