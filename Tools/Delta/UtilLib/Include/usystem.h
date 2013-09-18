// usystem.h
// System specific utilities.
// ScriptFighter Project.
// A. Savidis, July 2004.
//

#ifndef	USYSTEM_H
#define	USYSTEM_H

#include "utypes.h"
#include <string>

//---------------------------------------------------------------

typedef unsigned long			(*ugettimefunc_t) (void);	// Returns time in micro secs

struct UTILLIB_CLASS uadaptivesleep {

	util_ui32	delay;
	util_ui32	t_min;
	util_ui32	t_max;
	util_ui32	t_add;

	util_ui32	bedtime (void) const 
					{ return delay;  }
	void		action (void) 
					{ delay = t_min; }
	void		wait (void) {
					if (delay < (t_max - t_add))
						delay += t_add;
					else
						delay = t_max;
				}

	uadaptivesleep (util_ui32 _min, util_ui32 _max, util_ui32 _add) :
		delay(_min), 
		t_min(_min), 
		t_max(_max), 
		t_add(_add)
		{}
};

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
