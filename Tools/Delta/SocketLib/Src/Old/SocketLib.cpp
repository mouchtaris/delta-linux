// SocketLib.cpp.
// Socket library for I-GET communication layer.
// Unix and Windows support.
// A. Savidis, January 2000.
// Imported for use to build the Delta Debug Server / Client.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h> 
 
#ifdef	_UNIX_
extern "C"{
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
}
#endif

#ifdef	_W32BIT_
#include <winsock2.h>
#include <process.h>
#include <winbase.h>
#endif

#include "SocketLib.h"

//	#define	SOCKET_DEBUG

static FILE* logFile;

//---------------------------------------------------------

char SocketLink::buffer[IGET_SOCKET_BUFFER_SIZE];

SocketLink::SocketLink (void) {
	sockId=INVALID_SOCKET;
	data=(void*) 0;
	recvData=(void*) 0;
	dataSize=0;
	myHost[0]='\0';
	myPort=0;
}


SocketLink::SocketLink (char* host, unsigned int port, SOCKET _sockId) {
	sockId=_sockId;
	data=(void*) 0;
	dataSize=0;
	strcpy(myHost, host);
	myPort=port;
}


SocketLink::~SocketLink() {
	if (!BAD_SOCKET(sockId))
		SOCKET_CLOSE(sockId);
}


//---------------------------------------------------------
// Creates the primary socket of the server, with which connections
// will be accepted. This is opened at an available port within the
// local host. Both the host and the port will be given back to
// clients via command line arguments.
//
void SocketLink::EstablishAsServer (unsigned int portNum) {

	// Make socket.
	//
	struct protoent* proto=getprotobyname(TCP);
	if (!proto) {
		sockId=INVALID_SOCKET;
#ifdef	SOCKET_DEBUG
		LOGMSG "SocketLink::EstablishAsServer(): getprotobyname(TCP) failed.\n");
		LOGADD;
#endif
	}

	sockId=socket(AF_INET, SOCK_STREAM, proto->p_proto);
	if (BAD_SOCKET(sockId)) {
#ifdef	SOCKET_DEBUG
		LOGMSG "SocketLink::EstablishAsServer(): socket() failed.\n");
		LOGADD;
#endif
		return;
	}

	// Set socket option so that it is reusable, when closed.
	//
	int opt=1;
	if (setsockopt(sockId, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(opt)) IS_CALL_ERROR) {
#ifdef	SOCKET_DEBUG
		LOGMSG "SocketLink::EstablishAsServer(): setsockopt() failed");
		LOGADD;
#endif
		SOCKET_CLOSE(sockId);
		sockId=INVALID_SOCKET;
		return;
	}
 
	// Set socket send size.
	//
	opt=IGET_SOCKET_SEND_SIZE;
	if (setsockopt(sockId, SOL_SOCKET, SO_SNDBUF, (char*) &opt, sizeof(opt)) IS_CALL_ERROR) {
#ifdef	SOCKET_DEBUG
		LOGMSG "SocketLink::EstablishAsServer(): setsockopt() send size failed");
		LOGADD;
#endif
		SOCKET_CLOSE(sockId);
		sockId=INVALID_SOCKET;
		return;
	}

	// Set socket send size.
	//
	opt=IGET_SOCKET_RECV_SIZE;
	if (setsockopt(sockId, SOL_SOCKET, SO_RCVBUF, (char*) &opt, sizeof(opt)) IS_CALL_ERROR) {
#ifdef	SOCKET_DEBUG
		LOGMSG "SocketLink::EstablishAsServer(): setsockopt() recv size failed");
		LOGADD;
#endif
		SOCKET_CLOSE(sockId);
		sockId=INVALID_SOCKET;
		return;
	}
		 


	// Bind service.
	//
	struct sockaddr_in sockAddr;
	memset((void*) &sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family=AF_INET;
	sockAddr.sin_port=htons(portNum);			// Auto allocation.
	sockAddr.sin_addr.s_addr=htonl(INADDR_ANY);	// Local host address.
	if (bind(sockId, (struct sockaddr*) &sockAddr, sizeof(sockAddr)) IS_CALL_ERROR) { 
#ifdef	SOCKET_DEBUG
		LOGMSG "SocketLink::EstablishAsServer(): bind() failed.\n");
		LOGADD;
#endif
		SOCKET_CLOSE(sockId);
		sockId=INVALID_SOCKET;
		return;
	}

	// Get host and port information.
	//
	gethostname(myHost, IGET_HOST_NAME_LEN);
	int len=sizeof(sockAddr);
	getsockname(sockId, (sockaddr*) &sockAddr, &len);
	myPort=ntohs(sockAddr.sin_port);

	// Now listen for clients.
	//
	listen(sockId, IGET_MAX_LISTEN_QUEUE);
}


//---------------------------------------------------------
// Makes a client socket.
//
void SocketLink::EstablishAsClient (void) {

	// Make socket.
	//
	sockId=socket(AF_INET, SOCK_STREAM, 0);
	if (BAD_SOCKET(sockId)) {
#ifdef	SOCKET_DEBUG
		LOGMSG "SocketLink::EstablishAsClient(): socket() failed");
		LOGADD;
#endif
		return;
	}

	// Set socket option so that it is reusable, when closed.
	//
	int opt=1;
	if (setsockopt(sockId, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(opt)) IS_CALL_ERROR) {
#ifdef	SOCKET_DEBUG
		LOGMSG "SocketLink::EstablishAsClient(): setsockopt() failed.\n");
		LOGADD;
#endif
		SOCKET_CLOSE(sockId);
		sockId=INVALID_SOCKET;
		return;
	}

	// At this point, we can only get local host information,
	// but not the port information, unless connection gets
	// established.
	//
	gethostname(myHost, IGET_HOST_NAME_LEN);
}


//---------------------------------------------------------
// Here, the client will try a number of times to connect.
// Each try, all the addresses of the server will be tried,
// while after each try, it will sleep for a while. If no
// connection, an error is reported.
//
void SocketLink::ConnectWithServer (char* serverHost, unsigned int serverPort) {

	struct hostent* hp;
	if (!serverHost) {
		static char hostName[IGET_HOST_NAME_LEN];
		gethostname(hostName, IGET_HOST_NAME_LEN);		// Try with local host.
		hp=gethostbyname(serverHost=hostName);
	}
	else {
		hp=gethostbyname(serverHost);	// Try it firstly as a host name.
		if (!hp) {						// If failed, retry as a byte order address.
			in_addr addr;
			addr.s_addr=inet_addr(serverHost);
			hp=gethostbyaddr((char*) &addr, sizeof(addr), AF_INET);
		}
	}

	if (!hp) {
#ifdef	SOCKET_DEBUG
		LOGMSG "SocketLink::ConnectWithServer: gethostbyname('%s') failed.\n", serverHost);
		LOGADD;
#endif
		SOCKET_CLOSE(sockId);
		sockId=INVALID_SOCKET;
		return;
	}

	struct sockaddr_in sockAddr;
	sockAddr.sin_family=AF_INET;
	sockAddr.sin_port=htons((unsigned short) serverPort);

	// Now start connection request rounds.
	//
	MakeNonBlocking(0);
	for (unsigned int connTry=0; connTry<IGET_CONNECT_TRIES; connTry++) {

		for (unsigned int addrNo=0; hp->h_addr_list[addrNo]; addrNo++) {
			memcpy((void*) &sockAddr.sin_addr, hp->h_addr_list[addrNo], hp->h_length);
			if (connect(sockId, (struct sockaddr*) &sockAddr, sizeof(sockAddr)) IS_CALL_ERROR) {
				SOCKET_CLOSE(sockId);	// Close this socket.
				EstablishAsClient();	// Re-open new socket to test.
				continue;
			}
			else {
				// Connection successfully established.
				//
				myPort=serverPort;
				MakeNonBlocking();

#ifdef	SOCKET_DEBUG
				struct sockaddr_in peerName;
				int nameLen=sizeof(peerName);
				if (getpeername(sockId, (struct sockaddr*) &peerName, &nameLen) IS_CALL_ERROR) {
					LOGMSG "SocketLink::ConnectWithServer: getpeername() failed.\n");
					LOGADD;
				}
				else {
					LOGMSG "SocketLink::ConnectWithServer: Connected with '%s'.\n", 
								inet_ntoa(peerName.sin_addr));
					LOGADD;
				}
#endif
				return;
			}
		}

		// Else, sleep for a while, and continue.
		//
		SLEEP(IGET_CONNECT_WAIT);
	}

	// Else, post an error message and disable socket.
	//
#ifdef	SOCKET_DEBUG
	LOGMSG "SocketLink::ConnectWithServer: connect() failed.\n");
	LOGADD;
#endif
	SOCKET_CLOSE(sockId);
	sockId=INVALID_SOCKET;
}


//---------------------------------------------------------
// After accepting client, a new socket link is created,
// for this connection. Boolean variable controls blocking /
// non-blocking operation.
//
SocketLink*	SocketLink::AcceptClient (int blocking) {

	if (blocking)					// Set as blocking if needed.
		MakeNonBlocking(0);
	SOCKET connSock=accept(sockId, NULL, NULL);
	if (blocking)					// restore if needed.
		MakeNonBlocking(1);

	if (BAD_SOCKET(connSock) && blocking) {

#ifdef	SOCKET_DEBUG
		LOGMSG "SocketLink::AcceptClient(): accept() failed.\n");
		LOGADD;
#endif
		return (SocketLink*) 0;
	}

#ifdef	SOCKET_DEBUG
	struct sockaddr_in peerName;
	int nameLen=sizeof(peerName);
	if (getpeername(connSock, (struct sockaddr*) &peerName, &nameLen) IS_CALL_ERROR) {
		LOGMSG "SocketLink::AcceptClient: getpeername() failed.\n");
		LOGADD;
	}
	else {
		LOGMSG "SocketLink::AcceptClient: Connected with '%s'.\n", 
					inet_ntoa(peerName.sin_addr));
		LOGADD;
	}
#endif

	SocketLink* newLink=new SocketLink(GetHost(), GetPort(), connSock);
	newLink->MakeNonBlocking();
	return newLink;
}

//---------------------------------------------------------
// Makes a socket non-blocking. Subsequent calls to recv
// should be checked wrt return value for existence or not
// of pending received data.
//
void SocketLink::MakeNonBlocking (int status) {

	if (BAD_SOCKET(sockId))
		return;

#ifdef	_W32BIT_
	unsigned long opt=status ? 1 : 0;
	if (ioctlsocket(sockId, FIONBIO, &opt)==SOCKET_ERROR) {
#ifdef	SOCKET_DEBUG
			LOGMSG "ioctlsocket failed.\n");
			LOGADD;
#endif
	}
#endif

#ifdef	_UNIX_
	int opt=status ? 1 : 0;
	if (ioctl(sockId, FIONBIO, &opt)==-1)
		if (fcntl(sockId, F_SETFL, O_NONBLOCK)==-1) {
#ifdef	SOCKET_DEBUG
			LOGMSG "Both ioctl and fcntl failed.\n");
			LOGADD;
#endif
		}
#endif
}

//---------------------------------------------------------
// If dataSize is 0, it means all locally stored msgs
// have been successfully consumed. Else, we have to concatenate
// the remaining with the newlly received data.
//
void SocketLink::ReceiveDataLocally (int checkRead) {

	if (checkRead) {
		fd_set set;
		FD_ZERO(&set);
		FD_SET(sockId, &set);
		struct timeval tv;
		tv.tv_sec=1;
		tv.tv_usec=0;

#ifdef	_W32BIT_
		if (select(1, &set, NULL, NULL, &tv)==SOCKET_ERROR) {
#endif
#ifdef	_UNIX_
		if (!select(1, &set, NULL, NULL, &tv)) {
#endif
#ifdef	SOCKET_DEBUG
			LOGMSG "SocketLink::ReceiveDataLocally: select() failed.\n");
			LOGADD;
#endif
		}
	}

	int recvSize;

#ifdef	_W32BIT_
	if ((recvSize=recv(sockId, buffer, IGET_SOCKET_BUFFER_SIZE, 0))==SOCKET_ERROR) {
		recvSize=0;
		return;
	}
#endif

#ifdef	_UNIX_
	if ((recvSize=recv(sockId, buffer, IGET_SOCKET_BUFFER_SIZE, 0))==-1) {
		recvSize=0;
		return;
	}
#endif

#ifdef	SOCKET_DEBUG
	LOGMSG "Data received, size %d.\n", recvSize);
	LOGADD;
#endif

	// Make a new buffer to hold both data not read yet, as well as new
	// data just received.
	//
	void* newData=(void*) new char[dataSize+recvSize];						// Buffer size to hold both.
	if (dataSize)
		memcpy(newData, recvData, dataSize);								// Copy old in front (if any).
	memcpy((void*) (((char*) newData)+dataSize), (void*) buffer, recvSize);	// Concatenate new.
	dataSize+=recvSize;														// Set unread dataSize properly.

	if (data)
		delete[] ((char*) data);											// Delete old data.
	data=recvData=newData;													// Set new buffer.
}

//---------------------------------------------------------

// Returns true if no message can be locally extracted, either
// because no data are locally stored, or because the msg
// received is incomplete.
//
int SocketLink::MessageUnAvailable (void) {
	if (!dataSize)
		return 1;
	unsigned long netSize;
	memcpy((void*) &netSize, recvData, sizeof(unsigned long));
	int msgSize=(int) ntohl(netSize);
#ifdef	SOCKET_DEBUG
	LOGMSG "Received size %d, size header %d.\n", dataSize-sizeof(unsigned long), msgSize);
	LOGADD;
#endif
	return (msgSize > (dataSize-sizeof(unsigned long)));
}

//---------------------------------------------------------
// If no message is available try to receive one.
// return new status of message availability. 
//
int SocketLink::DataPending (int checkRead) {

	if (BAD_SOCKET(sockId))
		return 0;

	if (MessageUnAvailable())
			ReceiveDataLocally(checkRead);

	return !MessageUnAvailable();
}

//---------------------------------------------------------
// Send will be considered failed if not all data have been
// sent successfully. We put in front of the buffer the total
// size of the current message.
//
void SocketLink::SendData (const void* data, unsigned int size, int checkWrite) {

	if (BAD_SOCKET(sockId) || size<=0)
		return;

	// If checkWrite, then check with a select the capability to write
	// data to this socket.
	//
	if (checkWrite) {
		fd_set set;
		FD_ZERO(&set);
		FD_SET(sockId, &set);
		struct timeval tv;
		tv.tv_sec=1;
		tv.tv_usec=0;

#ifdef	_W32BIT_
		if (select(1, NULL, &set, NULL, &tv)==SOCKET_ERROR) {
#endif
#ifdef	_UNIX_
		if (!select(1, NULL, &set, NULL, &tv)) {
#endif
#ifdef	SOCKET_DEBUG
			LOGMSG "SocketLink::SendData: select() failed.\n");
			LOGADD;
#endif
		}
	}

	// Data are |size|data| now, where size is network 
	// byte ordered converted.
	//
	char* sendData=buffer;
	unsigned long netSize=htonl((unsigned long) size);
	memcpy(sendData, &netSize, sizeof(unsigned long));
	memcpy((void*) (sendData+sizeof(unsigned long)), data, size);
	size+=sizeof(unsigned long);

	// Enter a send loop. In case of a failure, sleep for a second,
	// and then retry (three tries at most). If this still fails,
	// it means we have a communication break down.
	//
	int tries=1;
	while (1) {
		int dataSent=send(sockId, (char*) sendData, size,0);
		if (dataSent IS_CALL_ERROR) {
#ifdef	SOCKET_DEBUG
			LOGMSG "SocketLink::SendData: send() failed. Try %d.\n", tries);
			LOGADD;
#endif
			if (tries==IGET_MAX_SEND_TRIES)
				return;
			tries++;
			SLEEP(IGET_FAILSEND_WAIT);
			continue;
		}
		else
		if (dataSent<size) {
#ifdef	SOCKET_DEBUG
			LOGMSG"SocketLink::SendData: send(): less data sent.\n");
			LOGADD;
#endif
			// Continue by trying to sent remaining data.
			//
			size-=dataSent;
			sendData+=dataSent;
		}
		else	{		// Ok, all sent.
#ifdef	SOCKET_DEBUG
		LOGMSG "Data sent, size %d.\n", dataSent);
		LOGADD;
#endif
			return;
		}
	}
}

//---------------------------------------------------------
// Firstly check if we already have data locally.
// If not, initialise with null and 0 the arguments.
//
void SocketLink::RecvData (void** putData, unsigned int* size, int checkRead) {

	if (DataPending(checkRead)) {

		// Get message size. Then, copy data, and move recvData.
		//
		unsigned long netSize;
		memcpy((void*) &netSize, recvData, sizeof(unsigned long));			// Get size in net byte order.
		*size=(int) ntohl(netSize);											// Convert to host info.
		*putData=new char[*size];											// Allocate as needed.
		memcpy(*putData, (void*) (((char*) recvData)+sizeof(unsigned long)), *size);	// Copy igoring size info.

		// Now decrease from dataSize received data size, and
		// set recvData to new position.
		//
		dataSize-= *size + sizeof(int);										// Decrease by recved msg.
		if (!dataSize)
			recvData=(void*) 0;												// No more data locally.
		else
			recvData=((char*) recvData) + (*size + sizeof(int));			// Move to next msg.
	}
	else {
		*size=0;
		*putData=(void*) 0;
	}
}


void SocketLink::BlockRecvData (void** data, unsigned int* size, int checkRead) {
	while (!DataPending(checkRead))
		;
	RecvData(data, size, checkRead);
}


void SocketLink::ClearData (void* data) {
	if (data)
		delete[] ((char*) data);
}
//---------------------------------------------------------

void SocketLink::Initialise (void) {

#ifdef	_W32BIT_

	WORD	wVersionRequested=MAKEWORD( 2, 0 );;
	WSADATA	wsaData;

	int err=WSAStartup(wVersionRequested, &wsaData);
	if (err) {
		printf("WinSock DLL not found.\n");
		exit(0);
	}
 
	/* Confirm that the WinSock DLL supports 2.0.		 */
	/* Note that if the DLL supports versions greater    */
	/* than 2.0 in addition to 2.0, it will still return */
	/* 2.0 in wVersion since that is the version we      */
	/* requested.                                        */
 
	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0 ) {
		printf("WinShock 2.2 or higher not found.\n");
		WSACleanup( );
		exit(0); 
	}
#endif

}

void SocketLink::CleanUp (void) {
	#ifdef	_W32BIT_
		WSACleanup();
	#endif
}

//---------------------------------------------------------
