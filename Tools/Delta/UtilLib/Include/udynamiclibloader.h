// udynamiclibloader.h
// DLL loader for UNIX variants and Windows.
// Y. Lilis, July 2008, initial version for windows.
// A. Savidis, June 2009, ported for Unix, and generic version put here.
//

#ifndef	UDYNAMICLIBLOADER_H
#define	UDYNAMICLIBLOADER_H

#include "utypes.h"
#include "usingleton.h"
#include <functional>
#include <string>

//---------------------------------------------------------

#define	UTIL_DLLIMPORT_TRUSTVALUE			99512117
#define	UTIL_DLLFUNC_NOTFOUND				0
#define	UTIL_DLLFUNC_TRUSTED				1
#define	UTIL_DLLFUNC_UNTRUSTED				2
#define	UTIL_DLLFUNC_TRUSTED_SETCLEANUP		3
#define	UTIL_DLLFUNC_ERROR					4

class UTILLIB_CLASS udynamiclibloader {

	public:
	typedef std::pair<util_ui32, std::string> ResultType;

	private:
	std::string	error;
	std::string	path;
	std::string cleanUp;
	void*		hDll;

	void				StoreLastError (const char* alt = (char*) 0);
	void				SetError (const std::string& s);
	static ResultType*	result;

	/////////////////////////////////////////////////////////

	public:
	static ResultType*	New (const std::string& val, bool isCleanUp = false); 
	bool				Has (const std::string& function);
	const ResultType	Call (const std::string& function);
	const std::string&	GetError (void) const 
							{ return error; }
	static const char*	GetErrorString (util_ui32 errCode);
	const std::string&	GetPath (void) const 
							{ return path; }
	void				SetCleanUp (const std::string& _cleanUp); 
	operator bool (void) const	
						{ return !!hDll; }

	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS

	udynamiclibloader (const std::string& dll);
	~udynamiclibloader();
};

//---------------------------------------------------------

#endif	// Do not add stuff beyond this point.
