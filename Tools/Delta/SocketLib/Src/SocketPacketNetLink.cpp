// SocketPacketNetLink.cpp
// 2WEAR Project.
// A. Savidis, March 2003
//

#include "SocketPacketNetLink.h"
#include "usystem.h"

#define	RESERVED_PACKET_SIZE	sizeof(util_ui32)

////////////////////////////////////////////////////////

util_ui32 SocketPacketNetLink::Encode (util_ui32 val) {
	util_ui32 encoded; 
	ubinaryio::WriteInteger(&encoded, val); 
	return encoded; 
}

util_ui32 SocketPacketNetLink::Decode (util_ui32 val) {
	util_ui32 decoded;
	ubinaryio::ReadInteger(&val, &decoded);
	return decoded;
}

////////////////////////////////////////////////////////

bool SocketPacketNetLink::MessageLocallyAvailable (void) {

	if (!currSize || currSize < RESERVED_PACKET_SIZE)
		return false;
	else {
		util_ui32 netSize;
		memcpy(&netSize, currData, RESERVED_PACKET_SIZE);
		return Decode(netSize) <= (currSize - RESERVED_PACKET_SIZE);
	}
}

////////////////////////////////////////////////////////

void SocketPacketNetLink::ReceiveMessageLocally (void) {

	// If there are data from the socket.
	//
	if (link->IsMsgPending()) {
		
		link->ReceiveMsg();
		void*		recvData;
		util_ui32	recvSize;

		link->ReadMsg(&recvData, &recvSize);

		// Make a new buffer to hold both data not read yet, as well as new
		// data just received.
		//
		void* newData = DNEWARR(char, currSize + recvSize);		// Buffer of size to hold both.
		if (currSize)											
			memcpy(newData, currData, currSize);				// Copy not yet read packet in front (if any).

		memcpy(POFFS(newData, currSize), recvData, recvSize);	// Concatenate with new packet.
		currSize += recvSize;									// Set unread size properly.
									
		if (allData)
			DDELARR((char*) allData);							// Delete old packet data.
																
		allData = currData = newData;							// Set new buffer.
		link->ClearMsg();
	}
}

////////////////////////////////////////////////////////

bool SocketPacketNetLink::IsMsgPending (void) {

	if (IsConnectionBroken())
		return false;
	else 
	if ((buffer && size) || !data.empty())
		return true;
	else {

		if (!MessageLocallyAvailable())
			ReceiveMessageLocally();

		return MessageLocallyAvailable();
	}
}
				
////////////////////////////////////////////////////////

void SocketPacketNetLink::BlockReceiveMsg (void) {
	while (!IsMsgPending())
		{}
	ReceiveMsg();
}

////////////////////////////////////////////////////////

void SocketPacketNetLink::ReceiveMsg (void) {

	DASSERT(IsMsgPending());

	if (buffer && size)			// Already received ?
		return;
	else
	if (!data.empty()) {		// If we do have data received locally we just extract.

		

		std::list<Data>::iterator first = data.begin();

		buffer	= first->buffer;
		size	= first->size;

		data.erase(first);
	}
	else {						// Receive data from the socket packet buffer.
		
		DASSERT(currSize >= RESERVED_PACKET_SIZE);

		util_ui32 netSize;
		memcpy(&netSize, currData, RESERVED_PACKET_SIZE);	// Get size in encoded form.
		size	= Decode(netSize);							// Decode size info.
		buffer	= DNEWARR(char, size);						// Allocate as needed.
															
		memcpy(												// Copy ignoring size info.
			buffer,												
			POFFS(currData, RESERVED_PACKET_SIZE),				
			size												
		);	
																	
		// Now decrease from currSize received data size, and		
		// set currData to new position.							
		//
		DASSERT(currSize >=size + RESERVED_PACKET_SIZE);
		currSize -= (size + RESERVED_PACKET_SIZE);			// Decrease by received size.
																	
		if (!currSize) {											
			DDELARR((char*) allData);											
			currData = allData = (void*) 0;					// No more data locally.
		}
		else
			PINC(currData, (size + RESERVED_PACKET_SIZE));	// Move to the next msg.
	}	
}
								
////////////////////////////////////////////////////////

void SocketPacketNetLink::SendMsg (void* data, util_ui32 size) {

	// Data are |SIZE|DATA| now, where size is network 
	// byte ordered converted.
	//
	void*			sendData	= DNEWARR(util_ui8, size + RESERVED_PACKET_SIZE);
	util_ui32		netSize		= Encode(size);

	memcpy(sendData, &netSize, RESERVED_PACKET_SIZE);
	memcpy(POFFS(sendData, RESERVED_PACKET_SIZE), data, size);

	size += RESERVED_PACKET_SIZE;
	link->SendMsg(sendData, size);

	DDELARR((char*) sendData);
}
				
////////////////////////////////////////////////////////

void ClientSocketPacketLink::ConnectWithServer (
		const char*	serverHost, 
		util_ui32	serverPort, 
		util_ui32	timeout
	) {
	util_ui32 t = ugettime() + timeout;
	do
		DPTR(client)->ConnectWithServer(serverHost, serverPort);
	while (DPTR(client)->IsConnectionBroken() && ugettime() < t);	
}

////////////////////////////////////////////////////////

SocketPacketNetLink* ServerSocketPacketLink::AcceptClient (bool blockingWait, util_ui32 timeout) {
	if (blockingWait) {
		DASSERT(!timeout);
		SocketNetLink* client = DPTR(server)->AcceptClient(blockingWait);
		return client ? DNEWCLASS(SocketPacketNetLink, (client)) : (SocketPacketNetLink*) 0;
	}
	else {
		SocketNetLink*	client	= (SocketNetLink*) 0;
		util_ui32		t		= ugettime() + timeout;	

		do
			if ((client = DPTR(server)->AcceptClient(false))) {
				if (DPTR(client)->IsConnectionBroken())
					udelete(client);
				else
					return DNEWCLASS(SocketPacketNetLink, (client));
			}
		while (ugettime() < t);

		return (SocketPacketNetLink*) 0;
	}
}

////////////////////////////////////////////////////////
