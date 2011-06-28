// SocketsLibP.h
// Sockets library for Delta using input / output buffers.
// Some private definitions.
// A. Savidis, May 2009.
//

#ifndef	SOCKETSLIBP_H
#define	SOCKETSLIBP_H

#include "utypes.h"
#include "uvalidatable.h"
#include "SocketPacketNetLink.h"
#include "DeltaStdClassNames.h"
#include <string>

#define	DELTA_SOCKETSLIB_CLIENT_NOTCONNECTED_ADDRESS	"<NOT CONNECTED>"

////////////////////////////////////////////////////////////

class SocketWrapper : public Validatable {

	public:
	enum Category {  Basic = 0, Client = 1, Server = 2 };

	private:
	Category 				category;
	SocketPacketNetLink*	socket;
	util_ui32				port;
	std::string				address;
	SocketWrapper*			creator;
	
	bool					Invariant (void) const {
								return	(category == Basic || category == Client || category == Server)		&&
										(category != Server		|| (port && !creator && address.empty()))	&&
										(category != Client		|| (address.size() && !port && !creator))	&&
										(category != Basic		|| (creator && !port && address.empty()));
							}
	
	//**********************

	public:
	const char*				GetExtClassString (void) const
								{ return SOCKET_TYPE_STR; }

	Category				GetCategory (void) const
								{ DASSERT(Invariant()); return category; }

	const std::string&		GetAddress (void) const
								{ DASSERT(Invariant() && category == Client); return address; }

	void					SetAddress (const std::string& _address)
								{ DASSERT(Invariant() && category == Client); address = _address; }

	util_ui32				GetPort (void) const
								{ DASSERT(Invariant() && category == Server); return port; }

	SocketWrapper*			GetCreator (void) const
								{ DASSERT(Invariant() && category == Basic); return creator; }

	SocketPacketNetLink*	GetSocket (void) 
								{ DASSERT(Invariant()); return socket; }

	ClientSocketPacketLink*	GetClientSocket (void) {
								DASSERT(Invariant() && category == Client); 
								return (ClientSocketPacketLink*) socket; 
							}

	ServerSocketPacketLink*	GetServerSocket (void) {
								DASSERT(Invariant() && category == Server); 
								return (ServerSocketPacketLink*) socket; 
							}

	DYNAMIC_CAST_NOBASE_INLINE_IMPL(SOCKET_TYPE_STR)

	//**********************

	SocketWrapper (SocketPacketNetLink* _socket, SocketWrapper* _creator) :
		category(Basic), 
		socket(_socket),
		port(0),
		creator(_creator){}

	SocketWrapper (SocketPacketNetLink* _socket, const std::string& _address) :
		category(Client), 
		socket(_socket),
		port(0),
		address(_address),
		creator((SocketWrapper*)0){}

	SocketWrapper (SocketPacketNetLink* _socket, util_ui32 _port) :
		category(Server), 
		socket(_socket),
		port(_port),
		creator((SocketWrapper*)0){}

	virtual ~SocketWrapper(){}
};

#define	SOCKET_VAL(val)	\
		VGET_INST(SocketWrapper, val, SOCKET_TYPE_STR)

#define	DLIB_GET_SOCKET \
	DLIB_GET_VALIDATABLE_ARG(socketVal, socketWrapper, SocketWrapper, SOCKET_TYPE_STR, SOCKET_VAL)

////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
