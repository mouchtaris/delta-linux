/**
 *	GUIThreadCommandDispatcher.h
 *
 * The GUI Thread command server is the same as the command server but
 * executes the provided command functions in the context of the main (GUI)
 * thread.
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef NET_GUITHREADCOMMANDDISPATCHER_H
#define NET_GUITHREADCOMMANDDISPATCHER_H

#include "Common.h"
#include "CommandDispatcher.h"
#include "UndefineAllWin32Shit.h"

#include <wx/event.h>
#include <boost/thread/mutex.hpp>

#pragma warning(disable:4273)

namespace comm {
	class encoder;
	class decoder;
}

namespace net {

////////////////////////////////////////////////////////////////////////

DECLARE_EVENT_TYPE(EVENT_NET_COMMAND_RECEIVED, -1);
DECLARE_EVENT_TYPE(EVENT_NET_LINK_ERROR, -1);

////////////////////////////////////////////////////////////////////////
// Event class
//
template<class TLinkPtr>
class NetEvent : public wxEvent {
public:
	typedef TLinkPtr LinkPtrType;

	typedef void (wxEvtHandler::*HandlerType)(NetEvent&);

	NetEvent (const std::string& data, LinkPtrType conn) :
		wxEvent(0, EVENT_NET_COMMAND_RECEIVED), m_data(data), m_conn(conn) {}
	~NetEvent (void) {}

	wxEvent* Clone (void) const { return new NetEvent(m_data, m_conn); }

	const std::string&	GetData (void) const { return m_data; }
	LinkPtrType			GetLink (void) const { return m_conn; }

private:
	const std::string	m_data;
	LinkPtrType			m_conn;
};

////////////////////////////////////////////////////////////////////////
// NetLink error class
//
template<class TLinkPtr>
class NetErrorEvent : public wxEvent {
public:
	typedef TLinkPtr			LinkPtrType;
	typedef boost_asio_error_t	ErrorType;

	typedef void (wxEvtHandler::*HandlerType)(NetErrorEvent&);

	NetErrorEvent (const ErrorType& error, LinkPtrType conn) :
		wxEvent(0, EVENT_NET_LINK_ERROR), m_error(error), m_conn(conn) {}
	~NetErrorEvent (void) {}

	wxEvent* Clone (void) const { return new NetErrorEvent(m_error, m_conn); }

	const ErrorType&	GetError (void) const { return m_error; }
	LinkPtrType			GetLink (void) const { return m_conn; }

private:
	const ErrorType		m_error;
	LinkPtrType			m_conn;
};

////////////////////////////////////////////////////////////////////////

template<class TLink, class TCommand, class TEncoder = comm::encoder, class TDecoder = comm::decoder>
class GUIThreadCommandDispatcher :
	public wxEvtHandler, public CommandDispatcher<TLink, TCommand, TEncoder, TDecoder> {
public:
	typedef CommandDispatcher<TLink, TCommand, TEncoder, TDecoder>	BaseType;
	typedef NetEvent<LinkPtrType>									NetEventType;
	typedef NetErrorEvent<LinkPtrType>								NetErrorEventType;

	GUIThreadCommandDispatcher (void) { this->connectSignals(); }
	~GUIThreadCommandDispatcher (void) {}

	//******************************************************************

	virtual bool RegisterCommand (PARAM(CommandType) command, const Handler& handler)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		return this->BaseType::RegisterCommand(command, handler);
	}

	virtual void ClearCommands (void)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		this->BaseType::ClearCommands();
	}

protected:
	virtual void HandleDispatch (const std::string& data, LinkPtrType conn)
	{
		NetEventType event(data, conn);
		this->AddPendingEvent(event);
	}

	virtual void HandleLinkError (const ErrorType& error, LinkPtrType conn)
	{
		NetErrorEventType event(error, conn);
		this->AddPendingEvent(event);
	}

private:
	void onCommandReceived (NetEventType& event)
		{ this->dispatchCommand(event.GetData(), event.GetLink()); }

	void onLinkError (NetErrorEventType& event)
		{ this->reportLinkError(event.GetError(), event.GetLink()); }

	//******************************************************************

	inline void dispatchCommand (const std::string& data, LinkPtrType conn)
	{
		boost::mutex::scoped_lock lock(m_mutex);

		CommandType command;
		DecoderType decoder(data);
		decoder >> command;
		this->DispatchCommand(command, conn, decoder);
	}

	inline void reportLinkError (const ErrorType& error, LinkPtrType conn)
	{
		this->ReportLinkError(error, conn);
	}

	//******************************************************************

	void connectSignals (void)
	{
		this->Connect(
			EVENT_NET_COMMAND_RECEIVED, (wxObjectEventFunction)(wxEventFunction)
			wxStaticCastEvent(NetEventType::HandlerType,
				&GUIThreadCommandDispatcher::onCommandReceived)
		);

		this->Connect(
			EVENT_NET_LINK_ERROR, (wxObjectEventFunction)(wxEventFunction)
			wxStaticCastEvent(NetErrorEventType::HandlerType,
				&GUIThreadCommandDispatcher::onLinkError)
		);
	}

	boost::mutex m_mutex;
};

////////////////////////////////////////////////////////////////////////

} // namespace net

#endif // NET_GUITHREADCOMMANDDISPATCHER_H
