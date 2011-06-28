// uerrorclass.h
// A class that can be used for error reporting.
// A. Savidis, March 2007.
// Updates December 2008, to handle platforms where ecxeptions
// are not supported.
//

#ifndef	UERRORCLASS_H
#define	UERRORCLASS_H

#include "usingleton.h"
#include "ustrings.h"
#include "utypes.h"
#include <string>
#include <list>
#include <functional>

#ifdef	UHAS_EXCEPTIONS	
#define	UERRORCLASS_EXCEPTIONIMPL  throw udisengageexception()
#define	UERRORCLASS_POSTEXCEPTION  uerror::GetSingleton().postexception()
#else
#define	UERRORCLASS_EXCEPTIONIMPL  postprimary("error disengage exception!")
#define	UERRORCLASS_POSTEXCEPTION  uerror::GetSingleton().postexception(); return
#endif

//////////////////////////////////////////////////////////////////

class UTILLIB_CLASS uerrorclass {

	private:
	bool					raised;	
	std::list<std::string>	errors;

	const std::string	assemblereport (bool orderErrors) const;

	//////////////////////////////////////////////////////////////////

	public:
	struct udisengageexception {};

	void				postexception (void) { UERRORCLASS_EXCEPTIONIMPL; }

	bool				israised (void) const 
							{ return raised; }

	void				postprimary (const char* format,...);
	void				postdomino (const char* format,...);

	void				postprimary (const std::string& s) { postprimary(s.c_str()); }
	void				postdomino (const std::string& s) { postdomino(s.c_str()); }

	const std::string	getreport (bool orderErrors = true) const;
	void				clear (void) { raised = false; errors.clear(); }
	
	uerrorclass (void): raised(false){}
	~uerrorclass(){}
};

class UTILLIB_CLASS UERRORCLASS_SINGLETON : public usingleton<uerrorclass>{}; 
class UTILLIB_CLASS uerror : public UERRORCLASS_SINGLETON {};

//////////////////////////////////////////////////////////////////

#define UERROR_ISRAISED		uerror::GetSingleton().israised
#define	UPRIMARYERROR		uerror::GetSingleton().postprimary
#define	UDOMINOERROR		uerror::GetSingleton().postdomino
#define UERROR_GETREPORT	uerror::GetSingleton().getreport
#define UERROR_CLEAR		uerror::GetSingleton().clear

#define	UCHECK_PRIMARY_ERROR(call, what)				\
	if (!(call)) { ERROR_HANDLER(ucstringarg(what), primary); } else

#define	UCHECK_DOMINO_ERROR(call, what)					\
	if (!(call)) { ERROR_HANDLER(ucstringarg(what), domino); } else

#define	UCHECK_WHATEVER_ERROR(call, what)				\
	if (!(call))										\
		if (uerror::GetSingleton().israised())			\
		{ ERROR_HANDLER(ucstringarg(what), domino);	}	\
		else											\
		{ ERROR_HANDLER(ucstringarg(what), primary); }	\
	else

#define	UCHECK_PRIMARY_ERROR_EX(call, what, onerror)	\
	if (!(call)) { onerror; ERROR_HANDLER(ucstringarg(what), primary); } else

#define	UCHECK_DOMINO_ERROR_EX(call, what, onerror)		\
	if (!(call)) { onerror; ERROR_HANDLER(ucstringarg(what), domino); } else

#ifdef	UHAS_EXCEPTIONS	
#define	UXCHECK_DOMINO_ERROR(call, what)				\
	if (true)											\
	try { (call); }										\
	catch (uerrorclass::udisengageexception)			\
	{ ERROR_HANDLER(ucstringarg(what), domino); }		\
	else
#else
#define	UXCHECK_DOMINO_ERROR(call, what)				\
	if (true) {											\
		{ call;	}										\
		UCHECK_WHATEVER_ERROR(UERROR_ISRAISED(), what);	\
	} else
#endif

//////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

