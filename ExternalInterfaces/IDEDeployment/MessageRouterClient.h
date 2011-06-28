/**
 *	MessageRouterClient.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef EXT_MESSAGEROUTERCLIENT_H
#define EXT_MESSAGEROUTERCLIENT_H

#include "Common.h"
#include "Singleton.h"
#include "Message.h"
#include "Connection.h"

#include <boost/signal.hpp>

namespace ext {

////////////////////////////////////////////////////////////////////////

class ComponentFuncEntry;

////////////////////////////////////////////////////////////////////////

typedef ide::Message	Message;
typedef ide::MessageVec MessageVec;

////////////////////////////////////////////////////////////////////////

class MessageRouterClient : public util::Singleton {
	DECLARE_SINGLETON(MessageRouterClient);

public:
	bool Connect (const String& host, ushort port);
	void Disconnect (void);
	bool IsConnected (void) const { return m_isConnected; }

	bool DispatchMessage (const std::string& callerId, Message& result, const Message& msg);
	bool DispatchMessageToAll (const std::string& callerId, MessageVec& results, const Message& msg);

	void DispatchSignal (const Message& msg);

private:
	MessageRouterClient (void) : m_isConnected(false) {}

	bool				m_isConnected;
	net::connection_ptr m_connection;
};

////////////////////////////////////////////////////////////////////////

} // namespace ext

#endif // EXT_MESSAGEROUTERCLIENT_H
