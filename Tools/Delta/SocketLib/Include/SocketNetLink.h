// SocketNetLink.h
// Socket communication of the 2WEAR NetLink class.
// This implementation will not certify that the message
// sent is received as an independent packet (as it is
// normally the case with TCPIP). To have such functionality
// available, use the SocketPacketNetLink instead.
// Using the I-GET SocketLib library for implementation.
// A. Savidis, January 2003.
//

#ifndef	SOCKETNETLINK_H
#define	SOCKETNETLINK_H

#include "NetLink.h"
#include "utypes.h"
#include "SocketLibDefs.h"

class SocketLink;

//---------------------------------------------------------
// A net link implemented for TCP/IP (sockets).
//

class SOCKETLIB_CLASS SocketNetLink : public NetLink {
	protected:
	SocketLink*		link;
	
	public:
	static char*			ToDottedAddr (util_ui32 addr);	// Returns a static string.
	static util_ui32		GetAddr (const char* dottedAddr);
	static bool				IsInitialised (void);
	static void				Initialise (void);							
	static void				CleanUp (void);
							
	const char*				GetHost (void);							
	static const char*		GetHostName (void);

	bool					IsMsgPending (void);							
	void					BlockReceiveMsg (void);							
	void					ReceiveMsg (void);											
	void					SendMsg (void* data, unsigned int size);							
	bool					IsConnectionBroken (void);
														
	SocketNetLink (SocketLink* _link);
	virtual ~SocketNetLink();
};

////////////////////////////////////////////////////////////
// The server is the module providing one or more standard services with
// which clients connect and disconnect dynamically. When a socket instance
// is destroyed, it means the socket is closed (hence, destroying an instance
// is the same as if disconnecting). With server sockets, we use the
// returned client sockects for sending or receiving data, not the server
// socket itself.
//
class SOCKETLIB_CLASS ServerSocketLink : public SocketNetLink {
	public:
	util_ui32	GetPort (void);
	SocketNetLink*	AcceptClient (bool blockingWait = true);

	ServerSocketLink (util_ui32 portNo = 0);	// Default is autoallocation of free port.
	virtual ~ServerSocketLink();
};

////////////////////////////////////////////////////////////
// The client links dynamically to a server via a blocking
// connect function. It should know the port and host. A client
// may use the same socket to connect to different servers, but it is
// better to have one socket per server (once a connection is done, destroy
// the socket link and make a new one).
//
class SOCKETLIB_CLASS ClientSocketLink : public SocketNetLink {
	public:
	void ConnectWithServer (const char* serverHost, util_ui32 serverPort);
	
	ClientSocketLink (void);
	virtual ~ClientSocketLink();
};

////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
