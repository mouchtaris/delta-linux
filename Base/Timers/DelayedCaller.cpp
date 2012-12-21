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
	boost::mutex::scoped_lock lock(m_mutex);
	m_callbackList.push_back(callback);
	if (!m_duringCall) {
		wxCommandEvent event(EVENT_CALL);
		this->AddPendingEvent(event);
	}
}

//**********************************************************************

void DelayedCaller::CancelAllDelayedCalls (void)
{
	boost::mutex::scoped_lock lock(m_mutex);
	m_callbackList.clear();
}

//**********************************************************************

void DelayedCaller::onCallExpired(wxCommandEvent& PORT_UNUSED_PARAM(event))
{
	{
		boost::mutex::scoped_lock lock(m_mutex);
		m_duringCall = true;
	}


	CallbackList current;

	{
		boost::mutex::scoped_lock lock(m_mutex);
		m_callbackList.swap(current);
	}
	
	if (!current.empty()) {
		try { std::for_each(current.begin(), current.end(), Caller<Callback>()); }
		catch(...) {}
		current.clear();
	}

	{
		boost::mutex::scoped_lock lock(m_mutex);
		if (!m_callbackList.empty()) {
			wxCommandEvent event(EVENT_CALL);
			this->AddPendingEvent(event);
		}
	}

	{
		boost::mutex::scoped_lock lock(m_mutex);
		m_duringCall = false;
	}
}

////////////////////////////////////////////////////////////////////////

} // namespace timer
