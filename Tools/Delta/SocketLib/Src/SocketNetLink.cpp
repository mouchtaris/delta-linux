// SocketNetLink.cpp
// The net link style socket library.
// 2WEAR Project.
// A. Savidis, January 2003.
//

#include "uthreadlib.h"
#include "ustrings.h"
#include "DDebug.h"

#include <assert.h>
#include "SocketLib.h"
#include "SocketNetLink.h"

#define	MAX_BUFFER_SIZE	256

//---------------------------------------------------------------------
// SOCKET NET LINK.
//

SocketNetLink::SocketNetLink (SocketLink* _link) { 
	assert(_link);
	link = _link;
}

SocketNetLink::~SocketNetLink() {
	DDELETE(link);
}

///////////////////////////////////////////////////////////////////////

static char dottedAddr[32];
static umutex addrumutex;

//*****************************

char* SocketNetLink::ToDottedAddr (util_ui32 addr) {

	LOCK_BLOCK(addrumutex);

	SocketLink::GetDottedAddress((unsigned long) addr, dottedAddr);
	return ucopystr(dottedAddr);
}

//*****************************

util_ui32 SocketNetLink::GetAddr (const char* dottedAddr) {
	return (util_ui32) SocketLink::GetAddress(dottedAddr); 
}


///////////////////////////////////////////////////////////////////////

const char* SocketNetLink::GetHost (void) {

	Validate();
	assert(link);
	assert(link->SocketValid());

	return link->GetHost();
}

const char* SocketNetLink::GetHostName(void) { 
	return SocketLink::GetHostName(); 
}

///////////////////////////////////////////////////////////////////////
				
bool SocketNetLink::IsMsgPending (void) { 

	Validate();
	assert(link);
	assert(link->SocketValid());

	return	(buffer && size)	||
			!data.empty()		||
			link->DataPending();
}
	
///////////////////////////////////////////////////////////////////////

void SocketNetLink::BlockReceiveMsg (void) { 

	Validate();
	while (!IsMsgPending()) 
		if (IsConnectionBroken())
			return; 

	ReceiveMsg();
}

void SocketNetLink::ReceiveMsg (void) { 

	Validate();
	assert(link);
	assert(link->SocketValid());
	assert(IsMsgPending());		// To avoid blocking.

	if (buffer && size)			// Already received ?
		return;
	else
	if (!data.empty()) {

		// If we do have dfata received locally, we just extract from those
		// data.
		//
		std::list<Data>::iterator first = data.begin();

		buffer = first->buffer;
		size = first->size;

		data.erase(first);
	}
	else {

		// Else, we have to receive data from the
		// socket.
		//
		void* recvData;
		link->RecvData(&recvData, &size);

		buffer = CreateDataBuffer(size);
		memcpy(buffer, recvData, size);

		SocketLink::ClearData(recvData);
	}
}
							
///////////////////////////////////////////////////////////////////////

void SocketNetLink::SendMsg (void* data, unsigned int size) { 

	Validate();
	assert(link);
	assert(link->SocketValid());

	if (!MAX_BUFFER_SIZE)
		link->SendData(data, size);
	else
		while (size) {

			if (size <= MAX_BUFFER_SIZE) {
				link->SendData(data, size);
				size = 0;	// Ok, all sent.
			}
			else {
				link->SendData(data, MAX_BUFFER_SIZE);
				size -= MAX_BUFFER_SIZE;
				PINC(data, MAX_BUFFER_SIZE);
			}
		}
}

///////////////////////////////////////////////////////////////////////

bool SocketNetLink::IsConnectionBroken (void) {
	Validate();
	assert(link);
	return !link->SocketValid() || !link->ConnectionOk();
}

static util_ui32 refCounter = 0;
bool SocketNetLink::IsInitialised (void) {
	DASSERT(!refCounter || SocketLink::IsInitialised());
	return refCounter;
}

void SocketNetLink::Initialise (void) {
	if (!refCounter++) {
		SocketLink::Initialise();
		NetLink::Initialise();
	}
}

void SocketNetLink::CleanUp (void) {
	DASSERT(refCounter);
	if (!--refCounter) {
		SocketLink::CleanUp();
		NetLink::CleanUp();
	}
}

//---------------------------------------------------------------------
// SERVER NET LINK.
//
util_ui32 ServerSocketLink::GetPort (void) {
	Validate();
	return link->GetPort();
}

///////////////////////////////////////////////////////////////////////

SocketNetLink* ServerSocketLink::AcceptClient (bool blockingWait) {

	Validate();
	SocketLink* clientLink = link->AcceptClient(blockingWait ? 1 : 0);
	return clientLink ? DNEWCLASS(SocketNetLink, (clientLink)) : (SocketNetLink*) 0;
}

///////////////////////////////////////////////////////////////////////


ServerSocketLink::ServerSocketLink (util_ui32 portNo) : SocketNetLink(DNEW(SocketLink)) {
	link->EstablishAsServer(portNo);
}

///////////////////////////////////////////////////////////////////////

ServerSocketLink::~ServerSocketLink() {}

//---------------------------------------------------------------------
// CLIENT NET LINK.

void ClientSocketLink::ConnectWithServer (const char* serverHost, util_ui32 serverPort) {
	Validate();
	link->ConnectWithServer(serverHost, serverPort);
}

///////////////////////////////////////////////////////////////////////

ClientSocketLink::ClientSocketLink (void): SocketNetLink(DNEW(SocketLink))  {
	Validate();
	link->EstablishAsClient();
}

///////////////////////////////////////////////////////////////////////

ClientSocketLink::~ClientSocketLink() {}

//---------------------------------------------------------------------
