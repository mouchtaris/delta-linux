/**
 *	EventDispatcher.h
 *
 * The event dispatcher is a low-level construct that associates events
 * with Handler functions that are registered by the client beforehand.
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef THREAD_EVENTDISPATCHER_H
#define THREAD_EVENTDISPATCHER_H

#include "Common.h"

#include <map>
#include <boost/function.hpp>

namespace thread {

////////////////////////////////////////////////////////////////////////

template<class TEvent, class TArg>
class EventDispatcher {
public:
	typedef TEvent EventType;

	//******************************************************************

	typedef boost::function<bool (PARAM(EventType), PARAM(TArg))> Handler;
	typedef boost::function<void (PARAM(EventType), PARAM(TArg))> EventErrorHandler;

	//******************************************************************

	EventDispatcher (void) {}
	virtual ~EventDispatcher (void) {}

	//******************************************************************

	virtual bool RegisterEvent (PARAM(EventType) event, const Handler& handler)
		{ return m_eventMap.insert(EventMap::value_type(event, handler)).second; }

	virtual void ClearEvents (void) { m_eventMap.clear(); }

	//******************************************************************

	void SetEventFailedHandler (const EventErrorHandler& handler)
		{ m_eventFailedHandler = handler; }

	void SetUndefinedEventHandler (const EventErrorHandler& handler)
		{ m_undefinedEventHandler = handler; }

	void ClearHandlers (void)
	{
		m_eventFailedHandler.clear();
		m_undefinedEventHandler.clear();
	}

	//******************************************************************

	void PostEvent (PARAM(EventType) event, PARAM(TArg) arg = VALUE(TArg)())
		{ this->HandleDispatch(event, arg); }

	//******************************************************************

protected:
	virtual void HandleDispatch (PARAM(EventType) event, PARAM(TArg) arg)
	{
		EventMap::const_iterator iter = m_eventMap.find(event);
		if (iter != m_eventMap.end()) {
			if (!iter->second(event, arg) && m_eventFailedHandler)
				m_eventFailedHandler(event, arg);
		}
		else if (m_undefinedEventHandler)
			m_undefinedEventHandler(event, arg);
	}

private:
	typedef std::map<VALUE(EventType), Handler> EventMap;

	//******************************************************************

	EventMap			m_eventMap;
	EventErrorHandler	m_eventFailedHandler;
	EventErrorHandler	m_undefinedEventHandler;
};

////////////////////////////////////////////////////////////////////////

} // namespace thread

#endif // THREAD_EVENTDISPATCHER_H
