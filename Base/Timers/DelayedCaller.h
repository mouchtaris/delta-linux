/**
 *	DelayedCaller.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#ifndef TIMER_DELAYEDCALLER_H
#define TIMER_DELAYEDCALLER_H

#include "Common.h"
#include "LibraryAPIDecls.h"
#include "Singleton.h"

#include <boost/thread/mutex.hpp>
#include <boost/function.hpp>
#include <wx/event.h>

namespace timer {

////////////////////////////////////////////////////////////////////////

class _BASE_API DelayedCaller : public wxEvtHandler, public util::Singleton {
	DECLARE_SINGLETON(DelayedCaller);

public:
	typedef boost::function<void (void)>	Callback;
	typedef std::list<Callback>				CallbackList;

	void PostDelayedCall		(const Callback& callback);
	void CancelAllDelayedCalls	(void);

private:
	DECLARE_EVENT_TABLE();

	void onCallExpired(wxCommandEvent& event);

	DelayedCaller (void) : m_duringCall(false) {}
	~DelayedCaller (void) {}

	volatile bool	m_duringCall;
	CallbackList	m_callbackList;
	boost::mutex	m_mutex;
};

////////////////////////////////////////////////////////////////////////

} // namespace timer

#endif // TIMER_DELAYEDCALLER_H
