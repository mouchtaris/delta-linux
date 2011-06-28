/**
 *	GUIThreadEventDispatcher.h
 *
 * Dispatches events to the main thread (GUI thread).
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef THREAD_GUITHREADEVENTDISPATCHER_H
#define THREAD_GUITHREADEVENTDISPATCHER_H

#include "Common.h"
#include "EventDispatcher.h"
#include "UndefineAllWin32Shit.h"

#include <wx/event.h>
#include <boost/thread/mutex.hpp>

namespace thread {

////////////////////////////////////////////////////////////////////////

class encoder;
class decoder;

////////////////////////////////////////////////////////////////////////

DECLARE_LOCAL_EVENT_TYPE(EVENT_GUIEVENT_POSTED, -1);

////////////////////////////////////////////////////////////////////////
// GUIEvent class
//
template<class TEvent, class TArg>
class GUIEvent : public wxEvent {
public:
	typedef TEvent	EventType;
	typedef TArg	ArgType;

	typedef void (wxEvtHandler::*HandlerType)(GUIEvent&);

	GUIEvent (PARAM(EventType) event, PARAM(ArgType) arg) :
		wxEvent(0, EVENT_GUIEVENT_POSTED), m_event(event), m_arg(arg) {}
	~GUIEvent (void) {}

	wxEvent* Clone (void) const { return new GUIEvent(m_event, m_arg); }

	PARAM(EventType)	GetEvent (void) const { return m_event; }
	PARAM(ArgType)		GetArg (void) const { return m_arg; }

private:
	VALUE(EventType)	m_event;
	VALUE(ArgType)		m_arg;
};

////////////////////////////////////////////////////////////////////////

template<class TEvent, class TArg>
class GUIThreadEventDispatcher : public wxEvtHandler, public EventDispatcher<TEvent, TArg> {
public:
	typedef EventDispatcher<TEvent, TArg>	BaseType;
	typedef TArg							ArgType;
	typedef GUIEvent<TEvent, TArg>			GUIEventType;

	GUIThreadEventDispatcher (void) { this->connectSignals(); }
	~GUIThreadEventDispatcher (void) {}

	//******************************************************************

	virtual bool RegisterEvent (PARAM(EventType) event, const Handler& handler)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		return this->BaseType::RegisterEvent(event, handler);
	}

	virtual void ClearEvents (void)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		this->BaseType::ClearEvents();
	}

protected:
	virtual void HandleDispatch (PARAM(EventType) event, PARAM(ArgType) arg)
	{
		GUIEventType guiEvent(event, arg);
		this->AddPendingEvent(guiEvent);
	}

private:
	void onGUIEventReceived (GUIEventType& event)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		this->BaseType::HandleDispatch(event.GetEvent(), event.GetArg());
	}

	//******************************************************************

	void connectSignals (void)
	{
		this->Connect(
			EVENT_GUIEVENT_POSTED, (wxObjectEventFunction)(wxEventFunction)
			wxStaticCastEvent(GUIEventType::HandlerType,
				&GUIThreadEventDispatcher::onGUIEventReceived)
		);
	}

	boost::mutex m_mutex;
};

////////////////////////////////////////////////////////////////////////

} // namespace thread

#endif // THREAD_GUITHREADEVENTDISPATCHER_H
