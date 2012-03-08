///////////////////////////////////////////////////// 
// Basic error reporting.
// For the Delta Virtual Machine.
// A. Savidis, original version 1999.
//

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "DDebug.h"
#include "DeltaRuntimeError.h"
#include "utypes.h"
#include "DeltaVirtualMachine.h"
#include "DeltaExceptionHandling.h"
#include "ustrings.h"
#include "uerrorclass.h"

#define	MAX_DISPLAYED_CALLS_ON_ERROR	32

#define	MAKE_ERROR_REPORT()				\
	va_list args;						\
	va_start(args, format);				\
	std::string report;					\
	uvsprintf(report, format, args);	\
  	va_end(args);

////////////////////////////////////////////

void (*DeltaVirtualMachine::notifyError) (const char*)		= (void (*) (const char*)) 0;
void (*DeltaVirtualMachine::notifyWarning) (const char*)	= (void (*) (const char*)) 0;

////////////////////////////////////////////

void DeltaVirtualMachine::SetErrorCallback (ErrorWarningCallback f) 
	{ notifyError = f; }

void DeltaVirtualMachine::SetWarningCallback (ErrorWarningCallback f) 
	{ notifyWarning = f; }

DeltaVirtualMachine::ErrorWarningCallback DeltaVirtualMachine::GetErrorCallback (void)
	{ return notifyError; }

DeltaVirtualMachine::ErrorWarningCallback DeltaVirtualMachine::GetWarningCallback (void)
	{ return notifyWarning; }

//////////////////////////////////

void DeltaVirtualMachine::ResetError (void) {
	DASSERT(HasProducedError());
	DASSERT(PCAtNormalProgramEnd());
	DASSERT(!IsFoundInAnyCallTrace());
	DASSERT(!GetPrimaryFailing());
	ResetExecutionError();
}

////////////////////////////////////////////

void DeltaVirtualMachine::RunTimeError (const char* format,...) {

	MAKE_ERROR_REPORT();

	// First time an error is posted we collect any error report
	// from uerror and post it to the notifier. The original errors
	// stored in uerror are not affected.

	std::string fullReport;
	if (UERROR_ISRAISED() && !somewhereWeHadAnError)
		fullReport = UERROR_GETREPORT() + "\n" + report;
	else
		fullReport = report;

	if (UERROR_ISRAISED())
		UDOMINOERROR("%s", report.c_str());
	else
		UPRIMARYERROR("%s", report.c_str());

	somewhereWeHadAnError = true;

	if (notifyError) 
		{ report += "\n"; (*notifyError)(fullReport.c_str()); }
}

////////////////////////////////////////////

void DeltaVirtualMachine::PossiblePrimaryError (const char* format,...) {
	
	MAKE_ERROR_REPORT();

	if (GetPrimaryFailing() || HasProducedError())
		Error("%s", report.c_str());
	else
		PrimaryError("%s", report.c_str());
}

////////////////////////////////////////////

void DeltaVirtualMachine::Error (const char* format,...) {

	MAKE_ERROR_REPORT();
	
	RunTimeError(
		"Runtime error: VM '%s', source '%s', line %d: %s", 
		Id(),
		*Source() ? Source() : "none",
		Line(),
		report.c_str()
	);

	if (!HasProducedError())
		InvalidateExecution(CallerFailed);
}

////////////////////////////////////////////

void DeltaVirtualMachine::PrimaryError (const char* format,...) {

	MAKE_ERROR_REPORT();


	if (!EXCEPTION_HANDLERS->IsPostingUnhandledExceptionError()) {

		if (errorCode.empty())
			errorCode = DELTA_GENERAL_RUNTIME_ERROR;

		DeltaValue exception(report);
		exception.SetTypeTag(errorCode);
		EXCEPTION_HANDLERS->Throw(this, exception);

		errorCode.clear();
	}
	else {
	
		DASSERT(!HasProducedError() && !GetPrimaryFailing() && IsErrorCauseReset());
		SetAsPrimaryFailing();

		CreateStackTrace(MAX_DISPLAYED_CALLS_ON_ERROR); 
		std::string stackTrace = std::string("\nStack trace:\n") + GetStackTrace(); 
	
		RunTimeError(
			"Runtime error: VM '%s', source '%s', line %d: %s%s", 
			Id(),
			*Source() ? Source() : "none",
			Line(),
			report.c_str(),
			stackTrace.c_str()
		);

		InvalidateExecution();
	}
}

////////////////////////////////////////////

void DeltaVirtualMachine::RunTimeWarning (const char* format,...) {

	MAKE_ERROR_REPORT();

	if (notifyWarning) 
		{ report += "\n"; (*notifyWarning)(report.c_str()); }
}

////////////////////////////////////////////

void DeltaVirtualMachine::Warning (const char* format,...) {
	
	MAKE_ERROR_REPORT();

	RunTimeWarning(
		"Runtime warning: VM '%s', source '%s', line %d: %s", 
		Id(),
		Source(),
		Line(),
		report.c_str()
	);
}

////////////////////////////////////////////
