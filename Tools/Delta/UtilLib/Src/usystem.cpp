// usystem.cpp
// System specific utilities.
// ScriptFighter Project.
// A. Savidis, July 2004.
//

#ifdef	_WIN32_
#include <windows.h>
#include <conio.h>
#include <process.h>
#endif

#ifdef	_UNIX_
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#endif

#include <stdarg.h>
#include <stdlib.h>
#include "usystem.h"
#include "ufiles.h"

///////////////////////////////////////////////////////////////

#ifdef	_UNIX_

UTILLIB_FUNC unsigned long ugettime (void) 
	{ return time((time_t*) 0) * 1000; }

static void defaultmodalmessage (const std::string& title, const std::string& msg) {
	fprintf(stderr, "%s: %s\n", title.c_str(), msg.c_str());
	while (true) {
		util_ui32 i;
		fprintf(stderr, "Type 0 and enter.");
		if (scanf("%u", &i) != 1) continue;
		if (!i)	break;
	}
}

UTILLIB_FUNC void uabort (void) 
	{ abort(); }

UTILLIB_FUNC  util_ui32	 ugetpid (void)
	{ return (util_ui32) getpid(); }


UTILLIB_FUNC void uprocesssleep (util_ui32 msecs) 
	{ usleep(1000 * msecs); }

UTILLIB_FUNC const std::string ugetenvironmentvar (const std::string& id) {
	if (const char* var = getenv(id.c_str()))
		return var;
	else
		return "";
}

UTILLIB_FUNC util_i32 ugetch (void) {
	struct termios _old, _new;
	tcgetattr(STDIN_FILENO, &_old);				// Get terminal settings
	_new = _old;
	_new.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &_new );	// Set new.
	int c = getchar();							// Read terminal input.
	tcsetattr(STDIN_FILENO, TCSANOW, &_old);	// Restore old.
	return c;
}

UTILLIB_FUNC util_i32 ugetche (void) // Echo mode.
	{ util_i32 c = ugetch(); putchar(c); return c; }

UTILLIB_FUNC bool ukbhit (void) {
	struct termios _old, _new;
	tcgetattr(STDIN_FILENO, &_old);
	_new = _old;
	_new.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &_new);
	util_i32 oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	util_i32 c = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &_old);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if(c != EOF) 
		{ ungetc(c, stdin); return true; }
	return false;
}

UTILLIB_FUNC util_i32 uspawnv (const char *exec, const char** argv, bool wait)	{

	DASSERT (!wait || !"Not implemented yet");

	if (!ufileexists(exec))	// Can't find executable.
		return -1;

	pid_t pid = fork();
	if (pid < 0)
		return -1;
	else
	if (pid == 0 && execv(exec, const_cast<char**>(argv)) < 0)
		_exit(0);

	return 0;
}

static void defaultprintf (const std::string& output)
	{ fwrite(output.c_str(), output.length(), sizeof(output.c_str()[0]), stdout); }

#endif	// _UNIX_

///////////////////////////////////////////////////////////////

#ifdef	_WIN32_

static void defaultprintf (const std::string& output)
	{ printf(output.c_str()); }

UTILLIB_FUNC unsigned long ugettime (void) 
	{ return timeGetTime(); }

UTILLIB_FUNC void uprocesssleep (util_ui32 msecs) 
	{ Sleep(msecs); }

UTILLIB_FUNC const std::string	ugetenvironmentvar (const std::string& id) {
	static TCHAR var[0xFFFF];
	DWORD result = GetEnvironmentVariable(id.c_str(), var, 0xFFFF);
	return !result || result > 0xFFFF ? "" : var;
}

static void defaultmodalmessage (const std::string& title, const std::string& msg) {
	MessageBox(
			GetForegroundWindow(),
			msg.c_str(),
			title.c_str(),
			MB_OK | MB_APPLMODAL
	);
}

UTILLIB_FUNC  util_ui32	ugetpid (void) 
	{ return (util_ui32) GetProcessId(GetCurrentProcess()); }

UTILLIB_FUNC  util_ui32	 ugettid (void)
	{ return (util_ui32) GetCurrentThreadId(); }

UTILLIB_FUNC util_i32	ugetch  (void)	{ return getch();	}
UTILLIB_FUNC util_i32	ugetche (void)	{ return getche();	}
UTILLIB_FUNC bool		ukbhit	(void)	{ return kbhit();	}

UTILLIB_FUNC  util_i32	uspawnv(const char *exec, const char** argv, bool wait)
	{ return (util_i32) spawnv(wait ? P_WAIT : P_NOWAIT, exec, argv); }

UTILLIB_FUNC  void uabort (void) {
	TerminateProcess(GetCurrentProcess(), 1);
	ExitProcess(1);
}

#endif	//_WIN32_

///////////////////////////////////////////////////////////////

static umodalmessagehandler modalMessageHandler = &defaultmodalmessage;
static uprintfhandler		printfHandler		= &defaultprintf;

UTILLIB_FUNC  void usetmodalmessagehandler (umodalmessagehandler f)
	{ modalMessageHandler  = f; }

UTILLIB_FUNC  void usetprintfhandler (uprintfhandler f)
	{ printfHandler = f; }

UTILLIB_FUNC void umodalmessage (const std::string& title, const std::string& msg) 
	{ (*modalMessageHandler)(title, msg); }

UTILLIB_FUNC void uprintf (const char* format,...) {

	va_list args;
	va_start(args, format);

	std::string output;
	uvsprintf(output, format, args);
	va_end(args);

	(*printfHandler)(output);
}

///////////////////////////////////////////////////////////////
