// usystem.h
// System specific utilities.
// ScriptFighter Project.
// A. Savidis, July 2004.
//

#ifndef	UYSYTEM_H
#define	USYSTEM_H

#include "utypes.h"
#include <string>

//---------------------------------------------------------------

typedef unsigned long			(*ugettimefunc_t) (void);	// Returns time in micro secs

UTILLIB_FUNC  unsigned long		ugettime (void);
UTILLIB_FUNC  void				usettimefunc (ugettimefunc_t f);
UTILLIB_FUNC  void				uprocesssleep (util_ui32 msecs);
UTILLIB_FUNC  const std::string	ugetenvironmentvar (const std::string& id);	// Returns empty when not found

typedef void (*umodalmessagehandler)(const std::string&, const std::string&);
UTILLIB_FUNC  void				usetmodalmessagehandler (umodalmessagehandler f);
UTILLIB_FUNC  void				umodalmessage (const std::string& title, const std::string& msg);
UTILLIB_FUNC  void				uabort (void);

typedef void (*uprintfhandler)(const std::string& output);
UTILLIB_FUNC  void				usetprintfhandler (uprintfhandler f);
UTILLIB_FUNC  void				uprintf(const char* format,...);

UTILLIB_FUNC  util_ui32			ugetpid (void);
UTILLIB_FUNC  util_ui32			ugettid (void);
UTILLIB_FUNC  util_i32			uspawnv(const char *exec, const char** argv, bool wait);

UTILLIB_FUNC  util_i32			ugetch (void);			
UTILLIB_FUNC  util_i32			ugetche (void);
UTILLIB_FUNC  bool				ukbhit (void);

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
