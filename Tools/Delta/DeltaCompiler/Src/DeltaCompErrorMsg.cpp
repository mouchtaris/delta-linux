// Basic error reporting.
// ScriptFighter Project.
// A. Savidis, 1999.
//

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "DDebug.h"
#include "utypes.h"
#include "DeltaCompErrorMsg.h"
#include "CompilerAPI.h"

/////////////////////////////////////////////////////////////

static char			srcFile[256];
static void			(*notifyError) (const char*) = 0;
static util_ui32	errors = 0;
static util_ui32	warnings = 0;

/////////////////////////////////////////////////////////////

DCOMPLIB_FUNC void DeltaCompSetErrorCallback (void (*callback)(const char*)) 
	{ notifyError = callback; }

DCOMPLIB_FUNC DeltaCompErrorCallback DeltaCompGetErrorCallback (void)
	{ return notifyError; }

DCOMPLIB_FUNC void DeltaCompResetErrors (void) 
	{ errors = 0; }

DCOMPLIB_FUNC void DeltaCompResetWarnings (void) 
	{ warnings = 0; }

DCOMPLIB_FUNC util_ui32 DeltaCompErrorsExist (void) 
	{ return errors; }

DCOMPLIB_FUNC util_ui32 DeltaCompWarningsExist (void) 
	{ return warnings; }

DCOMPLIB_FUNC void DeltaCompSetCurrentFile (const char* fname) 
	{ strcpy(srcFile, fname); }

/////////////////////////////////////////////////////////////

#define	MAKE_ERROR_REPORT()							\
			va_list args;							\
			va_start(args, format);					\
			std::string report;						\
			uvsprintf(report, format, args);		\
  			va_end(args);

#define	MAKE_ERROR_OR_WARNING(_prefix)				\
			(uconstructstr(							\
				_prefix ", file '%s', line %d: ",	\
				srcFile,							\
				DeltaCompiler::GetLine()			\
			) +	 report	+ ".\n")

/////////////////////////////////////////////////////////////

DCOMPLIB_FUNC void DeltaCompError (const char* format,...) {
	if (notifyError) {
		MAKE_ERROR_REPORT();
		(*notifyError)(MAKE_ERROR_OR_WARNING(DELTA_COMPILER_ERROR_PREFIX).c_str());
	}
	++errors;
}

/////////////////////////////////////////////////////////////

void DeltaCompWarning (const char* format,...) {
	if (notifyError) {
		MAKE_ERROR_REPORT();
		(*notifyError)(MAKE_ERROR_OR_WARNING(DELTA_COMPILER_WARNING_PREFIX).c_str());
	}
	++warnings;
}

/////////////////////////////////////////////////////////////

void DeltaCompMsg (const char* format,...) {
	if (notifyError) {
		MAKE_ERROR_REPORT();
		(*notifyError)(report.c_str());
	}
}

/////////////////////////////////////////////////////////////
