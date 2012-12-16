/**
 *	DelayedCaller.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "DelayedCaller.h"

namespace timer {

////////////////////////////////////////////////////////////////////////

template <class TCallable>
struct Caller : public std::unary_function<const TCallable&, void> {
	void operator() (const TCallable& callable) const { callable(); }
};

////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(DelayedCaller);

//**********************************************************************

//DECLARE_EVENT_TYPE(EVENT_CALL, -1);
DEFINE_EVENT_TYPE(EVENT_CALL);

//**********************************************************************

BEGIN_EVENT_TABLE(DelayedCaller, wxEvtHandler)
	EVT_COMMAND(wxID_ANY, EVENT_CALL, DelayedCaller::onCallExpired)
END_EVENT_TABLE();

//**********************************************************************

void DelayedCaller::PostDelayedCall (const Callback& callback)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);

	if (m_duringCall)
		m_nextRound.push_back(callback);
	else {
		m_callbackList.push_back(callback);
		wxCommandEvent event(EVENT_CALL);
		this->AddPendingEvent(event);
	}
}

//**********************************************************************

void DelayedCaller::CancelAllDelayedCalls (void)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);

	m_nextRound.clear();
	if (!m_duringCall)
		m_callbackList.clear();
}

//**********************************************************************

void DelayedCaller::onCallExpired(wxCommandEvent& PORT_UNUSED_PARAM(event))
{
	CallbackList copy;
	{
		boost::recursive_mutex::scoped_lock lock(m_mutex);
		if (!m_callbackList.empty())
			m_callbackList.swap(copy);
	}

	if (!copy.empty()) {
		m_duringCall = true;
		std::for_each(copy.begin(), copy.end(), Caller<Callback>());
		m_duringCall = false;
		copy.clear();
	}
	{
		boost::recursive_mutex::scoped_lock lock(m_mutex);
		if (!m_nextRound.empty()) {
			assert(m_callbackList.empty());
			m_callbackList.swap(m_nextRound);
			wxCommandEvent event(EVENT_CALL);
			this->AddPendingEvent(event);
		}
	}
}

////////////////////////////////////////////////////////////////////////

} // namespace timer
