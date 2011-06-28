/**
 *	SimpleTimer.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "SimpleTimer.h"
#include <wx/timer.h>

namespace timer {

////////////////////////////////////////////////////////////////////////

#define INTERVAL (SIMPLE_TIMER_RESOLUTION >> 1)

////////////////////////////////////////////////////////////////////////
// TimerImpl
//
class TimerImpl : public wxTimer {
public:
	void StartDefault (void) { this->Start(INTERVAL, wxTIMER_CONTINUOUS); }
protected:
	void Notify (void) { SimpleTimer::Notify(); }
};

////////////////////////////////////////////////////////////////////////

static TimerImpl* s_timerImpl = 0;

////////////////////////////////////////////////////////////////////////
// Simple Timer
//
void SimpleTimer::Initialize (void)
{
	s_timerImpl = new TimerImpl;
}

//**********************************************************************

void SimpleTimer::CleanUp (void)
{
	s_timerImpl->Stop();

	s_timers.clear();
	s_delayedTimers.clear();
	s_delayedRemovals = 0;
	s_inNotify = false;

	delete s_timerImpl;
	s_timerImpl = 0;
}

//**********************************************************************


int32 SimpleTimer::Set (int32 offset, const TimerCallback& cb, int32 id)
{
	const TimerEntry entry(offset * 2, cb, id);

	if (!s_inNotify)
		setInTimerList(entry);
	else
		s_delayedTimers.push_back(entry);

	return entry.id;
}

//**********************************************************************

void SimpleTimer::Cancel (int32 id)
{
	TimerEntryList::iterator iter =
		std::find_if(s_timers.begin(), s_timers.end(), EqualByIdPred(id));

	if (!s_inNotify) {
		if (iter != s_timers.end())
			s_timers.erase(iter);

		if (s_timers.empty())
			s_timerImpl->Stop();
	}
	else {
		iter->id = -1;
		++s_delayedRemovals;
	}
}

//**********************************************************************

void SimpleTimer::Notify (void)
{
	////////////////////////////////////////////////////////////////////
	// Notify clients
	//
	s_inNotify = true;	// Begin notification

	TimerEntryList::iterator iter = s_timers.begin();
	while (iter != s_timers.end()) {
		if (iter->id == -1) {			// Delayed removal
			assert(s_delayedRemovals);
			--s_delayedRemovals;
			iter = s_timers.erase(iter);
		}
		else if (--iter->offset <= 0) {
			assert(!iter->cb.empty());
			iter->cb(iter->id);
			iter = s_timers.erase(iter);
		}
		else
			++iter;
	}

	s_inNotify = false;	// End notification

	////////////////////////////////////////////////////////////////////
	// Perform delayed operations
	//
	if (!s_delayedTimers.empty()) {

		std::for_each(s_delayedTimers.begin(), s_delayedTimers.end(),
			&SimpleTimer::setInTimerList);
		s_delayedTimers.clear();
	}

	if (s_delayedRemovals) {
		iter = s_timers.begin();
		while (iter != s_timers.end()) {
			if (iter->id == -1) {
				assert(s_delayedRemovals);
				--s_delayedRemovals;
				iter = s_timers.erase(iter);
			}
			else
				++iter;
		}
		assert(!s_delayedRemovals);
	}

	////////////////////////////////////////////////////////////////////
	// Check if we don't need to tick
	//
	if (s_timers.empty())
		s_timerImpl->Stop();
}

//**********************************************************************

void SimpleTimer::setInTimerList (const TimerEntry& entry)
{
	Cancel(entry.id);

	if (s_timers.empty())
		s_timerImpl->StartDefault();

	TimerEntryList::iterator iter =
		std::find_if(s_timers.begin(), s_timers.end(), GreaterThanPred(entry.offset));
	s_timers.insert(iter, entry);
}

//**********************************************************************

int32 SimpleTimer::TimerEntry::s_id = 4096;

//**********************************************************************

SimpleTimer::TimerEntryList SimpleTimer::s_timers;
SimpleTimer::TimerEntryList SimpleTimer::s_delayedTimers;
uint						SimpleTimer::s_delayedRemovals = 0;
bool						SimpleTimer::s_inNotify = false;

////////////////////////////////////////////////////////////////////////

} // namespace timer
