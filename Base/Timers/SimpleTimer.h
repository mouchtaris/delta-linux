/**
 *	SimpleTimer.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#ifndef TIMER_SIMPLETIMER_H
#define TIMER_SIMPLETIMER_H

#include "Common.h"
#include "LibraryAPIDecls.h"

#include <boost/function.hpp>

namespace timer {

////////////////////////////////////////////////////////////////////////

#define SIMPLE_TIMER_RESOLUTION 1000
class TimerImpl;

////////////////////////////////////////////////////////////////////////

class _BASE_API SimpleTimer {
public:
	// void callback (int32 id);
	//
	typedef boost::function<void (int32)> TimerCallback;

	static void Initialize (void);
	static void CleanUp (void);	

	// Offset parameter is the number of SIMPLE_TIMER_RESOLUTION units
	// Set returns the timer's id.
	//
	static int32 Set (int32 offset, const TimerCallback& cb, int32 id = -1);
	static void	Cancel (int32 id);

protected:
	friend class TimerImpl;
	static void Notify (void);

private:
	DISABLE_CONSTRUCTION(SimpleTimer);

	////////////////////////////////////////////////////////////////////

	struct _BASE_API TimerEntry {
		TimerEntry (int32 _offset, const TimerCallback& _cb, int32 _id = -1) :
			offset(_offset), cb(_cb), id(_id) { if (id == -1) id = ++s_id; }
		TimerEntry (const TimerEntry& t) : offset(t.offset), cb(t.cb), id(t.id) {}
		TimerEntry& operator= (const TimerEntry& t)
			{ offset = t.offset; cb = t.cb; id = t.id; return *this; }

		int32			offset;
		TimerCallback	cb;
		int32			id;

	private:
		static int32 s_id;
	};

	////////////////////////////////////////////////////////////////////

	typedef std::list<TimerEntry>	TimerEntryList;
	typedef std::list<int32>		TimerRemList;

	////////////////////////////////////////////////////////////////////

	struct GreaterThanPred : public std::unary_function<const TimerEntry&, bool> {
		GreaterThanPred (int32 val) : m_val(val) {}
		bool operator() (const TimerEntry& entry) const { return m_val > entry.offset; }
	private:
		int32 m_val;
	};

	////////////////////////////////////////////////////////////////////

	struct EqualByIdPred : public std::unary_function<const TimerEntry&, bool> {
		EqualByIdPred (int32 id) : m_id(id) {}
		bool operator() (const TimerEntry& entry) const { return m_id == entry.id; }
	private:
		int32 m_id;
	};

	////////////////////////////////////////////////////////////////////

	static void setInTimerList (const TimerEntry& entry);

	static TimerEntryList	s_timers;
	static TimerEntryList	s_delayedTimers;
	static uint				s_delayedRemovals;
	static bool				s_inNotify;
};

////////////////////////////////////////////////////////////////////////

} // namespace timer

#endif // TIMER_SIMPLETIMER_H
