/**
 *	GUIThreadCommandServer.h
 *
 * The GUI Thread command server is the same as the command server but
 * executes the provided command functions in the context of the main (GUI)
 * thread.
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef NET_GUITHREADCOMMANDSERVER_H
#define NET_GUITHREADCOMMANDSERVER_H

#include "Common.h"
#include "CommandServer.h"
#include "GUIThreadCommandDispatcher.h"

namespace comm {
	class encoder;
	class decoder;
}

namespace net {

////////////////////////////////////////////////////////////////////////

template<class TLink, class TCommand, class TEncoder = comm::encoder, class TDecoder = comm::decoder>
class GUIThreadTCPCommandServer :
	public TCPCommandServer<
		TLink,
		TCommand,
		TEncoder,
		TDecoder,
		GUIThreadCommandDispatcher<TLink, TCommand, TEncoder, TDecoder>
	> {
public:
	typedef TCPCommandServer<
		TLink,
		TCommand,
		TEncoder,
		TDecoder,
		GUIThreadCommandDispatcher<TLink, TCommand, TEncoder, TDecoder>
	> BaseType;

	GUIThreadTCPCommandServer (IOService& service, uint16 port = 0) : BaseType(service, port) {}
	~GUIThreadTCPCommandServer (void) {}
};

////////////////////////////////////////////////////////////////////////

template<
	class TLinkUDP,
	class TLinkTCP,
	class TLinkHybrid,
	class TCommand,
	class TEncoder = comm::encoder,
	class TDecoder = comm::decoder
>
class GUIThreadHybridCommandServer :
	public HybridCommandServer<
		TLinkUDP,
		TLinkTCP,
		TLinkHybrid,
		TCommand,
		TEncoder,
		TDecoder,
		GUIThreadCommandDispatcher<TLinkHybrid, TCommand, TEncoder, TDecoder>
	> {
public:
	typedef HybridCommandServer<
		TLinkUDP,
		TLinkTCP,
		TLinkHybrid,
		TCommand,
		TEncoder,
		TDecoder,
		GUIThreadCommandDispatcher<TLinkHybrid, TCommand, TEncoder, TDecoder>
	> BaseType;

	GUIThreadHybridCommandServer (IOService& service, uint16 port = 0) : BaseType(service, port) {}
	~GUIThreadHybridCommandServer (void) {}
};

////////////////////////////////////////////////////////////////////////

} // namespace net

#endif // NET_GUITHREADCOMMANDSERVER_H
