#include "JsonLoaderErrorMsg.h"
#include "utypes.h"
#include <stdarg.h>
#include "DDebug.h"
#include "JsonLoaderAPI.h"
#include <sstream>
#include <list>

static char						errorText1[256];
static std::list<std::string>	errorsList;

static void (*notifyError) (const char*)=0;
static void (*notifyMsg) (const char*)=0;
static util_ui32 errors = 0;

void JsonParserLoaderErrorMsg::JsonLoaderSetErrorCallback (void (*callback)(const char*))
{
	notifyError = callback;
}

void JsonParserLoaderErrorMsg::JsonLoaderSetMsgCallback (void (*callback)(const char*))
{
	notifyMsg = callback;
}

void JsonParserLoaderErrorMsg::JsonLoaderResetErrors (void)
{
	errorsList.clear();
	errors = 0;
}

int JsonParserLoaderErrorMsg::JsonLoaderNumErrors (void)
{
	return errors;
}

const std::string JsonParserLoaderErrorMsg::JsonLoaderGetError (void)
{
	if (errors) {
		std::string ret_str;
		std::list<std::string>::iterator it;
		for (it = errorsList.begin(); it != errorsList.end(); ++it) {
			ret_str += *it;
		}
		return ret_str;
	} else {
		return "";
	}
}

void JsonParserLoaderErrorMsg::JsonLoaderError (const char* format, ...)
{
	std::stringstream sstream;
	va_list args;
	va_start(args, format);
  	vsprintf(errorText1, format, args);
	DASSERT(strlen(errorText1) < uarraysize(errorText1));
  	va_end(args);

	sstream << "Error: file '" << JsonLoaderAPI::GetFile()
		<< "' line " << JsonLoaderAPI::GetLine() << ": "
		<< errorText1 << ".\n";

	errorsList.push_back(sstream.str());

	if (notifyError)
		(*notifyError)(errorsList.back().c_str());
	errors++;
}

void JsonParserLoaderErrorMsg::JsonLoaderMsg (const char* format, ...) {

	if (notifyMsg) {
		va_list args;
		va_start(args, format);

		vsprintf(errorText1,format,args);
		DASSERT(strlen(errorText1) < uarraysize(errorText1));

		va_end(args);

		(*notifyMsg)(errorText1);
	}
}
