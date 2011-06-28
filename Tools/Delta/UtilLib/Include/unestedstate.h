// unestedstate.h
// A simple utility class for all cases that a counter
// recording entering / leaving a state, supporting re-entrance (i.e.
// while in the state we may re-enter the state again).
// ScriptFighter Project.
// A. Savidis, June 2007.
//

#ifndef	UNESTEDSTATE_H
#define	UNESTEDSTATE_H

#include "utypes.h"
#include "DDebug.h"

//---------------------------------------------------------------

template <const util_ui32 N> class  unestedstate {
	private:
	util_ui32 n;
	util_ui32 stack[N];
	util_ui16 top;

	public:
	void		enter (void)			{ ++n; }
	void		exit (void)				{ DASSERT(n); --n; }
	void		reset (util_ui32 i = 0)	{ n = i; }

	void		save (void) { DASSERT(top < N); stack[top++] = n; n = 0; }
	void		restore (void) { DASSERT(top); n = stack[top-1]; --top; }

	bool		inside (void) const		{ return n > 0; }
	util_ui32	value (void) const		{ return n; }

	void operator=(const unestedstate& s) 
		{ if (this != &s) new (this) unestedstate(s); }

	unestedstate (void) : n(0), top(0){}
	unestedstate (util_ui32 _n) : n(_n), top(0){}
	unestedstate (const unestedstate& s) : n(s.n), top(s.top)
		{ memcpy(&stack[0], &s.stack[0], sizeof(stack)); }
	~unestedstate(){}
};

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
