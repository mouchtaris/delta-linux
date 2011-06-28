// uerrorclass.cpp
// A class that can be used for error reporting.
// A. Savidis, March 2007.
//

#include <stdarg.h>
#include <algorithm>
#include "uerrorclass.h"
#include "ustrings.h"
#include "DDebug.h"

USINGLETON_INSTANCE(usingleton<uerrorclass>)

//////////////////////////////////////////////////////////////////

const std::string uerrorclass::assemblereport (bool orderErrors) const {

	DASSERT(raised);

	if (errors.size() == 1)
		return *errors.begin();
	else {
		util_ui16 order = 0;
		std::string report;

		for (	std::list<std::string>::const_iterator i = errors.begin();
				i != errors.end();
				++i	) {

				if (order)
					report.append("\n");
				if (!orderErrors)
					report += *i;
				else
					report.append(uconstructstr("%d:%s", ++order, i->c_str()));
		}

		return report;
	}
}

//////////////////////////////////////////////////////////////////

const std::string uerrorclass::getreport (bool orderErrors) const 
	{ return raised? assemblereport(orderErrors) : ""; }

//////////////////////////////////////////////////////////////////

void uerrorclass::postprimary (const char* format,...) {

	raised = true;

	va_list args;
	va_start(args, format);

	std::string error;
	uvsprintf(error, format, args);

	va_end(args);

	return errors.push_back(error);
}

//////////////////////////////////////////////////////////////////

void uerrorclass::postdomino (const char* format,...) {

	DASSERT(raised);
	va_list args;
	va_start(args, format);

	std::string error;
	uvsprintf(error, format, args);

	return errors.push_back(error);
}
	
//////////////////////////////////////////////////////////////////
