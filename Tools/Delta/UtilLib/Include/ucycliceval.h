// ucycliceval.h
// Some comon stuff for processes engaging cycles and their
// detection.
// ScriptFighter Project.
// A. Savidis, May 2003.
//

#ifndef	UCYCLICEVAL_H
#define	UCYCLICEVAL_H

#include "utypes.h"

//---------------------------------------------------------------

class uworkstate {

	protected:
	enum state_t { not_started = 0, in_progress = 1, completed = 2 };
	state_t state;

	public:
	virtual bool IsInProgress (void) const
					{ return state == in_progress; }
	virtual bool IsCompleted (void) const
					{ return state == completed; }
	virtual void SetInProgress (bool val)
					{ state = val ? in_progress : not_started; }
	virtual void SetCompleted (void)
					{ state = completed; }
	virtual bool NotStarted (void) const
					{ return state == not_started; }
	virtual bool HasStarted (void) const
					{ return state != not_started; }

	uworkstate (void) : state(not_started) {}
	virtual ~uworkstate(){}
};

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
