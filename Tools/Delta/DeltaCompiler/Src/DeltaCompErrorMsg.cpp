// DeltaCompErrorMsg.cpp
// Basic error reporting.
// Script Fighter Project.
// A. Savidis, October 1999.
//

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "DDebug.h"
#include "utypes.h"
#include "DeltaCompErrorMsg.h"
#include "ParseParms.h"
#include "CompileOptions.h"

/////////////////////////////////////////////////////////////

void DeltaCompilerMessenger::SetErrorCallback (ErrorCallback callback)
	{ notifyError = callback; }

DeltaCompilerMessenger::ErrorCallback DeltaCompilerMessenger::GetErrorCallback (void) const
	{ return notifyError; }

void DeltaCompilerMessenger::ResetErrors (void)
	{ errors = 0; }

void DeltaCompilerMessenger::ResetWarnings (void)
	{ warnings = 0; }

util_ui32 DeltaCompilerMessenger::ErrorsExist (void) const
	{ return errors; }

util_ui32 DeltaCompilerMessenger::WarningsExist (void) const
	{ return warnings; }

void DeltaCompilerMessenger::SetCurrentFile (const std::string& file)
	{ srcFile = file; }

void DeltaCompilerMessenger::SetSourceReferences (const SourceReferenceList& references)
	{ srcReferences = references; }

/////////////////////////////////////////////////////////////

const std::string DeltaCompilerMessenger::ReferencesStr (const SourceReferenceList& srcReferences) {
	std::string result;
	for (SourceReferenceList::const_iterator i = srcReferences.begin(); i != srcReferences.end(); ++i)
		result += uconstructstr(
			DELTA_COMPILER_REFERENCE_PREFIX ", file '%s', line %d.\n",
			i->first.c_str(),
			i->second
		);
	return result;
}

/////////////////////////////////////////////////////////////

#define	MAKE_REPORT()								\
			va_list args;							\
			va_start(args, format);					\
			std::string report;						\
			uvsprintf(report, format, args);		\
  			va_end(args);

#define	MAKE_ERROR_OR_WARNING(_prefix)				\
			(uconstructstr(							\
				_prefix ", file '%s', line %d: ",	\
				srcFile.c_str(),					\
				PARSEPARMS.GetLine()				\
			) +	 report	+ ".\n" + ReferencesStr(srcReferences))

/////////////////////////////////////////////////////////////

void DeltaCompilerMessenger::Error (const char* format,...) {
	if (notifyError) {
		MAKE_REPORT();
		notifyError(MAKE_ERROR_OR_WARNING(DELTA_COMPILER_ERROR_PREFIX).c_str());
	}
	++errors;
}

/////////////////////////////////////////////////////////////

void DeltaCompilerMessenger::Warning (const char* format,...) {
	if (notifyError) {
		MAKE_REPORT();
		notifyError(MAKE_ERROR_OR_WARNING(DELTA_COMPILER_WARNING_PREFIX).c_str());
	}
	++warnings;
}

/////////////////////////////////////////////////////////////

void DeltaCompilerMessenger::Msg (const char* format,...) {
	if (notifyError) {
		MAKE_REPORT();
		notifyError(report.c_str());
	}
}

/////////////////////////////////////////////////////////////
