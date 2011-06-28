// SocketPacketNetLink.h
// A wrapper of the SocketNetLink class, which introduces a very
// simple protocol to ensure that each packet sent is also
// received as an independent packet. However, since this is a protocol,
// it has to be used both sides of communication.
// 2WEAR Project.
// A. Savidis, March 2003
//

#ifndef	SOCKETPACKETNETLINK_H
#define	SOCKETPACKETNETLINK_H

#include "NetLink.h"
#include "SocketNetLink.h"
#include "DDebug.h"
#include "utypes.h"
#include "ubinaryio.h"
#include "SocketLibDefs.h"

////////////////////////////////////////////////////////////

class SOCKETLIB_CLASS SocketPacketNetLink : public NetLink {

	protected:
	SocketNetLink*			link;

	private:
	void*					allData;		// Receive local buffer with many msgs pending.
	void*					currData;		// Next msg to be read.
	util_ui32				currSize;		// Remaining size.

	bool					MessageLocallyAvailable (void);
	void					ReceiveMessageLocally (void);

	static util_ui32		Encode (util_ui32 val);
	static util_ui32		Decode (util_ui32 val);

	//*************************
		
	public:
	static void				Initialise (void) { SocketNetLink::Initialise(); }
	static void				CleanUp (void) { SocketNetLink::CleanUp(); }

	void					SendUnsignedInt (util_ui32 n) 
								{ util_ui32 val = Encode(n); SendMsg(&val, sizeof(val)); }

	SocketNetLink*			GetPureLink (void) 
								{ return link; }

	//*************************

	bool					IsMsgPending (void);					
	void					BlockReceiveMsg (void);
	void					ReceiveMsg (void);		
	void					SendMsg (void* data, unsigned int size);
							
	bool					IsConnectionBroken (void)
								{ return link->IsConnectionBroken(); }

	SocketPacketNetLink (SocketNetLink* _link) :
		allData		((void*) 0),
		currData	((void*) 0),
		currSize	(0) { DASSERT(_link); link = _link; }

	virtual ~SocketPacketNetLink() 
		{ DDELETE(link); }
};

////////////////////////////////////////////////////////////

class SOCKETLIB_CLASS ServerSocketPacketLink : public SocketPacketNetLink {
	private:
	ServerSocketLink* server;

	public:
	util_ui32				GetPort (void) 
								{ return DPTR(server)->GetPort(); }
	SocketPacketNetLink*	AcceptClient (bool blockingWait = true, util_ui32 timeout = 0);

	ServerSocketPacketLink (util_ui32 portNo = 0) :
		SocketPacketNetLink(DNEWCLASS(ServerSocketLink, (portNo))) 
		{ server = (ServerSocketLink*) link; }
	virtual ~ServerSocketPacketLink()
		{}

};

////////////////////////////////////////////////////////////

class SOCKETLIB_CLASS ClientSocketPacketLink : public SocketPacketNetLink {
	private:
	ClientSocketLink* client;

	public:
	void ConnectWithServer (const char* serverHost, util_ui32 serverPort, util_ui32 timout = 0);

	ClientSocketPacketLink (void) :
		SocketPacketNetLink(DNEW(ClientSocketLink)) 
		{ client = (ClientSocketLink*) link; }
	virtual ~ClientSocketPacketLink(){}
};

////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
