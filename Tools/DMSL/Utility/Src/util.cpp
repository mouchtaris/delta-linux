/**
 *	util.cpp
 *
 *	-- DMSL Project --
 *
 *	Implementation fothe util namespace.
 *
 *	Giannis Lilis <lilis@ics.forth.gr>
 *	August 2006 (Version 1) - July 2007 (Version 2)
 */

#include "util.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32_WCE
//Include this for the time structures
#include <windows.h>
#endif

//****************************
namespace dmsl {
	namespace util {
		const char* MakeString (const std::string format, ...) {
			va_list args;
			va_start(args, format);
			static char constructStr[1024];
			vsprintf(constructStr, format.c_str(), args);
			va_end(args);
			return constructStr;
		}

		//****************************

		char* strdup (const char *s) {
			char* cpy = new char[strlen(s) + 1];
			return strcpy(cpy, s);
		}

		//****************************

		char* strdup (const char *s, unsigned int len) {
			char* cpy = new char[len + 1];
			memcpy(cpy, s, len);
			cpy[len] = '\0';
			return cpy;
		}

		//****************************

		const char *concat (unsigned int numOfArgs, ...) {
			va_list args;
			va_start(args, numOfArgs);
			static char constructStr[1024];
			static char format[128];
			assert(numOfArgs * 3 + 1 < 128);	// 42 arguments should be enough, but still check
			unsigned int len = 0;
			for(unsigned int i = 0; i < numOfArgs; ++i)
				len += sprintf(format + len, "%%s");
			vsprintf(constructStr, format, args);
			va_end(args);
			return strdup(constructStr);
		}

		//****************************

		const std::string toString (int num) { return MakeString("%d", num); }

		const std::string toString (double num) {
			char *tmp;
			if(num == (int) num)	//actually integer?
				tmp = const_cast<char *>(MakeString("%d", (int) num));
			else
				tmp = const_cast<char *>(MakeString("%lf", num));
			
			//Remove trailing zeros in case of a double
			if(strchr(tmp, '.'))
				for(size_t i = strlen(tmp) - 1; tmp[i] != '\0'; --i)
					if(tmp[i] == '0')
						tmp[i] = '\0';
					else
						break;
			return tmp;
		}

		const std::string toString(bool b) { return b ? "true" : "false"; }

		const std::string toString(const std::string& s) { return "\"" + s + "\""; }

#ifdef _WIN32_WCE
		//WIN32_WCE does not have the "time" function so implement it here using existing functions
		time_t time( time_t *t ) {
			SYSTEMTIME sysTimeStruct;
			FILETIME fTime;
			ULARGE_INTEGER int64time;
			time_t locTT = 0;

			if ( t == NULL )
				t = &locTT;

			GetSystemTime( &sysTimeStruct );
			if ( SystemTimeToFileTime( &sysTimeStruct, &fTime ) ) {
				memcpy( &int64time, &fTime, sizeof( FILETIME ) );
				/* Subtract the value for 1970-01-01 00:00 (UTC) */
				int64time.QuadPart -= 0x19db1ded53e8000;
				/* Convert to seconds. */
				int64time.QuadPart /= 10000000;
				*t = (time_t)int64time.QuadPart;
			}
			return *t;
		}
#else
		//Else just use the default time implementation
		time_t time(time_t*t) { return ::time(t); }
#endif
	}
}