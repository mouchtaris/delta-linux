/**
 *	MessageRouterServer.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_MESSAGEROUTERSERVER_H
#define IDE_MESSAGEROUTERSERVER_H

#include "Common.h"
#include "Singleton.h"
#include "IDECommon.h"
#include "Message.h"

#include "Connection.h"
#include "GUIThreadCommandServer.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

class _IDE_API MessageRouterServer : public util::Singleton {
	DECLARE_SINGLETON_WITH_ARGS(MessageRouterServer, (int32));

public:

protected:
	MessageRouterServer(int32 port);

	void OnInitialize (void);
	void OnCleanUp (void);

private:
	typedef net::GUIThreadTCPCommandServer<net::connection, int32> CommandServer;

	bool cmdDispatchMessage (int32 command, net::connection_ptr conn, comm::decoder& dec);
	bool cmdDispatchMessageToAll (int32 command, net::connection_ptr conn, comm::decoder& dec);
	bool cmdDispatchSignal (int32 command, net::connection_ptr conn, comm::decoder& dec);

	CommandServer* m_server;
	int32 port;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_MESSAGEROUTERSERVER_H
