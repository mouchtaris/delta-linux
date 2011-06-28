// udynamiclibloader.cpp
// DLL loader for UNIX variants and Windows.
// Y. Lilis, July 2008, initial version for windows.
// A. Savidis, June 2009, ported for Unix, and generic version put here.
//
#include "DDebug.h"
#include "udynamiclibloader.h"
#include "uerrorclass.h"

#ifdef	_UNIX_
#include <dlfcn.h>
#define	DLOPEN_FLAGS	RTLD_LAZY | RTLD_GLOBAL
#endif

#ifdef _WIN32_
#include <windows.h>
#endif

/////////////////////////////////////////////////////////////////

#define	ERROR_HANDLER(what, errclass)								\
	uerror::GetSingleton().post##errclass(							\
		"Dynamic library '%s': error '%s'!", path.c_str(), what		\
	); goto FAIL;

/////////////////////////////////////////////////////////////////

static const char* errorMessages[] = {
	"function not found",
	(char*) 0,
	"function not trusted",
	(char*) 0,
	"function caused an error" 
};

/////////////////////////////////////////////////////////////////

udynamiclibloader::ResultType* udynamiclibloader::result = (ResultType*) 0;

udynamiclibloader::ResultType* udynamiclibloader::New (const std::string& val, bool isCleanUp) {
	if (result)
		return new (result)	ResultType(
								isCleanUp ? UTIL_DLLFUNC_TRUSTED_SETCLEANUP : UTIL_DLLIMPORT_TRUSTVALUE, 
								val
							);
	else
		return result = DNEWCLASS(ResultType, (UTIL_DLLIMPORT_TRUSTVALUE, val));
}

/////////////////////////////////////////////////////////////////

udynamiclibloader::udynamiclibloader(const std::string& dll) : path(dll) {
#ifdef	_UNIX_
	hDll = dlopen(dll.c_str(), DLOPEN_FLAGS);
#endif
#ifdef _WIN32_
	hDll = (void*) LoadLibrary(dll.c_str());
#endif
	if (!hDll)
		StoreLastError("DLL loading failed");
}

udynamiclibloader::~udynamiclibloader() {

	if (!cleanUp.empty()) {
		ResultType r = Call(cleanUp);
		UCHECK_WHATEVER_ERROR(
			r.first != UTIL_DLLIMPORT_TRUSTVALUE,
			uconstructstr("failed to invoke clean up function '%s'", cleanUp.c_str())
		);
	}

	FAIL:
	if (hDll) {
#ifdef	_UNIX_
		dlclose(hDll);
#endif
#ifdef _WIN32_
		FreeLibrary((HINSTANCE) hDll);
#endif
	}
}

/////////////////////////////////////////////////////////////////

const char*	udynamiclibloader::GetErrorString (util_ui32 errCode) {
	DASSERT(errCode < uarraysize(errorMessages));
	return DNULLCHECK(errorMessages[errCode]);
}

/////////////////////////////////////////////////////////////////

void udynamiclibloader::SetError (const std::string& s) 
	{ error = s; }

void udynamiclibloader::SetCleanUp (const std::string& _cleanUp)
	{ cleanUp = _cleanUp; }

void udynamiclibloader::StoreLastError (const char* alt) {
#ifdef	_UNIX_
	if (const char* dlsym_error = dlerror())
		error = dlsym_error;
	else
		error = alt;
#endif

#ifdef	_WIN32_
	static TCHAR message[256];
	message[0] = 0;
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		0,
		message,
		uarraysize(message),
		NULL
	);
	error = message[0] ? message : alt;
#endif
}

/////////////////////////////////////////////////////////////////

bool udynamiclibloader::Has (const std::string& function) {
#ifdef	_UNIX_
		return !!dlsym(hDll, function.c_str());
#endif
#ifdef _WIN32_
		return !!GetProcAddress((HINSTANCE) hDll, function.c_str());
#endif
	}

/////////////////////////////////////////////////////////////////

const udynamiclibloader::ResultType udynamiclibloader::Call (const std::string& function) {
		
		typedef ResultType* (*Func)(void);
		Func F;

#ifdef	_UNIX_
		* (void**) (&F) = dlsym(hDll, function.c_str());
#endif		
#ifdef _WIN32_
		F = (Func) GetProcAddress((HINSTANCE) hDll, function.c_str());
#endif
		if (!F) {
			StoreLastError("DLL function invocation failed");
			return ResultType(UTIL_DLLFUNC_NOTFOUND, "");
		}

		ResultType* p = (ResultType*) 0;
		try			{ p = F(); }
		catch(...)	{ return ResultType(UTIL_DLLFUNC_ERROR, ""); }

		if (result && p == result) {
			ResultType	returnedResult(
							DPTR(p)->first == UTIL_DLLIMPORT_TRUSTVALUE ? UTIL_DLLFUNC_TRUSTED : UTIL_DLLFUNC_UNTRUSTED,
							DPTR(p)->second
						);
			udelete(result);
			return returnedResult;
		}
		else {
			udeleteunlessnull(result);
			SetError("invalid return value");
			return ResultType(UTIL_DLLFUNC_UNTRUSTED, error);
		}
	}

/////////////////////////////////////////////////////////////////

void udynamiclibloader::SingletonCreate (void) 
	{ DASSERT(!result); }

void udynamiclibloader::SingletonDestroy (void) 
	{ udeleteunlessnull(result); }

/////////////////////////////////////////////////////////////////
