// Basic error reporting for resource loader.
// A. Savidis, January 2000.
//
#include <stdio.h>
#include <stdarg.h>

#include "RcLoaderErrorMsg.h"
#include "RcLoaderAPI.h"
#include "utypes.h"
#include "DDebug.h"
#include <cstring>

static char			errorText1[256];
static char			errorText2[256];

static void (*notifyError) (const char*)=0;
static void (*notifyMsg) (const char*)=0;
static util_ui32 errors = 0;

void ResourceLoaderSetErrorCallback (void (*callback)(const char*)) 
	{ notifyError = callback; }

void ResourceLoaderSetMsgCallback (void (*callback)(const char*)) 
	{ notifyMsg = callback; }

void ResourceLoaderResetErrors (void) 
	{ errors = 0; }

bool ResourceLoaderErrorsExist (void) 
	{ return !!errors; }

///////////////////////////////////////////////////////////

const std::string ResourceLoaderGetError (void) 
	{ return errors ? errorText2 : ""; }

void ResourceLoaderError (const char* format, ...) {

	va_list args;
	va_start(args, format);
  	vsprintf(errorText1, format, args);
	DASSERT(strlen(errorText1) < uarraysize(errorText1));
  	va_end(args);

	sprintf(errorText2, "Error: file '%s' line %d: %s.\n", 
		ResourceLoader::GetFile().c_str(), 
		ResourceLoader::GetLine(), 
		errorText1
	);
	DASSERT(strlen(errorText2) < uarraysize(errorText2));

	if (notifyError)
		(*notifyError)(errorText2);
	errors = 1;
}

///////////////////////////////////////////////////////////

void ResourceLoaderMsg (const char* format, ...) {

	if (notifyMsg) {
		va_list args;
		va_start(args, format);
  		
		vsprintf(errorText1,format,args);
		DASSERT(strlen(errorText1) < uarraysize(errorText1));

  		va_end(args);

		(*notifyMsg)(errorText1);
	}
}

///////////////////////////////////////////////////////////
