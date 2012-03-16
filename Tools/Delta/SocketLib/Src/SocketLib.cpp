// SocketLib.cpp.
// Socket library for I-GET communication layer.
// Unix and Windows support. Original version january 1996.
// A. Savidis, January 2000, 2WEAR update.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h> 
#include <errno.h>

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
#include <signal.h>
}

#define SHUTDOWN_SIGNAL(SIGNUM) \
	signal(SIGNUM, SIG_IGN)

#define WOULD_BLOCK	(errno == EAGAIN || errno == EWOULDBLOCK)

#endif /* _UNIX_ */

#ifdef	_WIN32_
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <winbase.h>
#define SHUTDOWN_SIGNAL(SIGNUM)
#endif /* _WIN32_ */

#include "SocketLib.h"
#include "DDebug.h"
#include "ustrings.h"

//---------------------------------------------------------

#define	SOCKET_DEBUG
//	#define	VERBOSE

#define	LOGNAME	"socketlog.txt"

static void LOGMSG (const char* format, ...) {

#ifdef	 SOCKET_DEBUG
	va_list args;
	va_start(args, format);

	std::string result;
	uvsprintf(result, format, args);
	va_end(args);

	FILE* fp = fopen(LOGNAME,"at");
	fputs(result.c_str(), fp);
	fclose(fp);
#endif
}

//---------------------------------------------------------

extern char**		_argv;
extern int			_argc;
FILE*				SocketLink::logFile;
umutex				SocketLink::bufferumutex;
umutex				SocketLink::connectumutex;
umutex				SocketLink::acceptumutex;
util_ui8			SocketLink::buffer[SOCKETLIB_SOCKET_BUFFER_SIZE];

//---------------------------------------------------------

SocketLink::SocketLink (void) {

	sockId		= INVALID_SOCKET;
	myHost[0]	= '\0';
	myPort		= 0;
	isBlocking	= true;	// Default socket behaviour.
}

///////////////////////////////////////////////////////////

SocketLink::SocketLink (const char* host, util_ui32 port, SOCKET _sockId) {
	sockId		= _sockId;
	assert(strlen(host) < SOCKETLIB_HOST_NAME_LEN);
	strcpy(myHost, host);
	myPort		= port;
	isBlocking	= false;
}

///////////////////////////////////////////////////////////

SocketLink::~SocketLink() {

	if (!BAD_SOCKET(sockId)) {

		for (	PacketList::iterator packetIter = incoming.begin();
				packetIter != incoming.end();
				++packetIter )

				ClearData(packetIter->first);

		incoming.clear();

#ifdef	_UNIX_
		shutdown(sockId, SHUT_RDWR);	// Both send and receive.
#endif
#ifdef	_WIN32_
		shutdown(sockId, SD_BOTH);
#endif
		SOCKET_CLOSE(sockId);
	}
}

///////////////////////////////////////////////////////////

unsigned long SocketLink::GetAddress (const char* dottedAddr) {
	struct hostent* hp;
	hp = gethostbyname(dottedAddr);	
	if (!hp)
		return inet_addr(dottedAddr);
	else {

		hp = gethostbyname(hp->h_name);	// Ensure we get the fully qualified name.

		unsigned long result = 0;
		assert((util_ui32)hp->h_length <= sizeof(result));
		assert(hp->h_addrtype == AF_INET);

		memcpy(&result, hp->h_addr, hp->h_length);
		return result;
	}
}

void SocketLink::GetDottedAddress (unsigned long _addr, char* putDottedAddr) {

	in_addr addr;
	addr.s_addr = _addr;
	strcpy(putDottedAddr, inet_ntoa(addr));
	assert(strlen(putDottedAddr) < 16); // xxx.yyy.zzz.www at most. Post check.
}

//---------------------------------------------------------

bool SocketLink::SetOptions (const char* func) {

	assert(!BAD_SOCKET(sockId));

	// Set socket option so that it is reusable, when closed.
	//
	int opt = 1;
	if (setsockopt(sockId, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(opt)) IS_CALL_ERROR) {

		LOGMSG("SocketLink::%s(): setsockopt() failed", func);

		SOCKET_CLOSE(sockId);
		sockId = INVALID_SOCKET;

		return false;
	}
	else
		return true;
}

//---------------------------------------------------------

bool SocketLink::SetRecvSendBufferSize (const char* func) {

	assert(!BAD_SOCKET(sockId));

	// Set socket send size.
	//
	int opt = SOCKETLIB_SOCKET_SEND_SIZE;

	if (setsockopt(sockId, SOL_SOCKET, SO_SNDBUF, (char*) &opt, sizeof(opt)) IS_CALL_ERROR) {

		LOGMSG("SocketLink::%s(): setsockopt() send size failed", func);
		SOCKET_CLOSE(sockId);
		sockId = INVALID_SOCKET;
		return false;
	}

	// Set socket recv size.
	//
	opt = SOCKETLIB_SOCKET_RECV_SIZE;

	if (setsockopt(sockId, SOL_SOCKET, SO_RCVBUF, (char*) &opt, sizeof(opt)) IS_CALL_ERROR) {

		LOGMSG("SocketLink::%s(): setsockopt() recv size failed", func);

		SOCKET_CLOSE(sockId);
		sockId = INVALID_SOCKET;
		return false;
	}

	return true;
}

//---------------------------------------------------------
// Creates the primary socket of the server, with which connections
// will be accepted. This is opened at an available port within the
// local host. Both the host and the port will be given back to
// clients via command line arguments.
//
void SocketLink::EstablishAsServer (util_ui32 portNum) {

	SHUTDOWN_SIGNAL(SIGPIPE);

	struct protoent* proto = getprotobyname(TCP);

	if (!proto) {
		sockId = INVALID_SOCKET;
		LOGMSG("SocketLink::EstablishAsServer(): getprotobyname(TCP) failed.\n");
		return;
	}

	// Make socket.
	//
	sockId = socket(AF_INET, SOCK_STREAM, proto->p_proto);

	if (BAD_SOCKET(sockId)) {
		LOGMSG("SocketLink::EstablishAsServer(): socket() failed.\n");
		return;
	}

	if (!SetOptions("EstablishAsServer"))
		return;

	if (!SetRecvSendBufferSize("EstablishAsServer"))
		return; 
		 
	// Bind service.
	//
	struct sockaddr_in sockAddr;
	memset((void*) &sockAddr, 0, sizeof(sockAddr));

	sockAddr.sin_family			= AF_INET;
	sockAddr.sin_port			= htons(portNum);		// Auto allocation.
	sockAddr.sin_addr.s_addr	= htonl(INADDR_ANY);	// Local host address.

	if (bind(sockId, (struct sockaddr*) &sockAddr, sizeof(sockAddr)) IS_CALL_ERROR) { 

		LOGMSG("SocketLink::EstablishAsServer(): bind() failed.\n");

		SOCKET_CLOSE(sockId);
		sockId = INVALID_SOCKET;
		return;
	}

	// Get host and port information.
	//
	gethostname(myHost, SOCKETLIB_HOST_NAME_LEN);
	SOCKLEN_T len = sizeof(sockAddr);
	getsockname(sockId, (sockaddr*) &sockAddr, &len);
	myPort = ntohs(sockAddr.sin_port);

	// Now listen for clients.
	//
	listen(sockId, SOCKETLIB_MAX_LISTEN_QUEUE);
}

//---------------------------------------------------------
// Makes a client socket.
//
void SocketLink::EstablishAsClient (void) {

	SHUTDOWN_SIGNAL(SIGPIPE); // prevent broken pipe to exit application.

	// Make new socket.
	sockId = socket(AF_INET, SOCK_STREAM, 0);

	if (BAD_SOCKET(sockId)) {
		LOGMSG("SocketLink::EstablishAsClient(): socket() failed");
		return;
	}

	SetIsBlocking(false);

	if (!SetOptions("EstablishAsClient"))
		return;

	if (!SetRecvSendBufferSize("EstablishAsClient"))
		return; 

	// At this point, we can only get local host information,
	// but not the port information, unless connection gets
	// established.
	//
	gethostname(myHost, SOCKETLIB_HOST_NAME_LEN);
}

//---------------------------------------------------------
// Here, the client will try a number of times to connect.
// Each try, all the addresses of the server will be tried,
// while after each try, it will sleep for a while. If no
// connection, an error is reported.
//
void SocketLink::ConnectWithServer (const char* serverHost, util_ui32 serverPort) {

#ifdef	VERBOSE
	fprintf(
		stderr,
		"TCPIP connect(client %s, server %s, port %d).\n",
		myHost,
		serverHost,
		serverPort
	);
	fflush(stderr);
#endif

	LOCK_BLOCK(connectumutex);

	if (BAD_SOCKET(sockId))	// If we had a bad socket, just establish as a client.
		EstablishAsClient();

	struct hostent* hp;

	if (!serverHost) {
		static char hostName[SOCKETLIB_HOST_NAME_LEN];
		gethostname(hostName, SOCKETLIB_HOST_NAME_LEN);	// Try with local host.
		hp = gethostbyname(serverHost = hostName);
	}
	else {
		hp = gethostbyname(serverHost);	// Try it firstly as a host name.
		if (!hp) {			// If failed, retry as a byte order address.
			in_addr addr;
			addr.s_addr = inet_addr(serverHost);
			hp = gethostbyaddr((char*) &addr, sizeof(addr), AF_INET);
		}
	}

	if (!hp) {

		LOGMSG("SocketLink::ConnectWithServer: gethostbyname('%s') failed.\n", serverHost);

		SOCKET_CLOSE(sockId);
		sockId = INVALID_SOCKET;
		return;
	}

	struct sockaddr_in sockAddr;
	sockAddr.sin_family	= AF_INET;
	sockAddr.sin_port	= htons((unsigned short) serverPort);

	// Now start connection request rounds.
	// This should be done in temporary blocking mode.

	assert(!isBlocking);
	bool oldIsBlocking = isBlocking;
	SetIsBlocking(true);

	for (util_ui32 connTry = 0; connTry < SOCKETLIB_CONNECT_TRIES; ++connTry) {

		for (util_ui32 addrNo = 0; hp->h_addr_list[addrNo]; ++addrNo) {

			memcpy((void*) &sockAddr.sin_addr, hp->h_addr_list[addrNo], hp->h_length);

			if (connect(sockId, (struct sockaddr*) &sockAddr, sizeof(sockAddr)) IS_CALL_ERROR || !ConnectionOk() ) {

				SOCKET_CLOSE(sockId);	// Close this socket.

				EstablishAsClient();	// Re-open new socket to test.
				assert(!isBlocking);	// Should be set as non-blocking here

				SetIsBlocking(true);
				continue;
			}
			else {

				// Connection successfully established. Restore
				// previous blocking mode.

				myPort = serverPort;
				SetIsBlocking(oldIsBlocking);

#ifdef	SOCKET_DEBUG
				struct sockaddr_in peerName;
				SOCKLEN_T nameLen = sizeof(peerName);

				if (getpeername(sockId, (struct sockaddr*) &peerName, &nameLen) IS_CALL_ERROR)
					LOGMSG("SocketLink::ConnectWithServer: getpeername() failed.\n");
				else 
					LOGMSG("SocketLink::ConnectWithServer: Connected with '%s'.\n", inet_ntoa(peerName.sin_addr));
#endif
				assert(SocketValid() && ConnectionOk() );
				return;
			}
		}

		// Else, sleep for a while, and continue.
		//
		SLEEP(SOCKETLIB_CONNECT_WAIT);
	}

	// Else, post an error message and disable socket.
	//
	LOGMSG("SocketLink::ConnectWithServer: connect() failed.\n");

	SOCKET_CLOSE(sockId);
	sockId = INVALID_SOCKET;
	isBlocking = false;
}

//---------------------------------------------------------
// After accepting client, a new socket link is created,
// for this connection. Boolean variable controls blocking /
// non-blocking operation.
//
SocketLink*	SocketLink::AcceptClient (bool blockingWait) {

	LOCK_BLOCK(acceptumutex);

	assert(!isBlocking);
	bool oldIsBlocking = isBlocking;
	SetIsBlocking(blockingWait);

	SOCKET connSock = accept(sockId, NULL, NULL);
	SetIsBlocking(oldIsBlocking);

	if (BAD_SOCKET(connSock)) {
		if (blockingWait)
			LOGMSG("SocketLink::AcceptClient(): blocking accept() failed.\n");
		return (SocketLink*) 0;
	}

#ifdef	SOCKET_DEBUG
	struct sockaddr_in peerName;
	SOCKLEN_T nameLen = sizeof(peerName);

	if (getpeername(connSock, (struct sockaddr*) &peerName, &nameLen) IS_CALL_ERROR)
		LOGMSG("SocketLink::AcceptClient: getpeername() failed.\n");
	else
		LOGMSG("SocketLink::AcceptClient: Connected with '%s'.\n", inet_ntoa(peerName.sin_addr));
#endif

	// The client socket created is non-blocking.

	SocketLink* newLink = DNEWCLASS(SocketLink, (GetHost(), GetPort(), connSock));
	newLink->SetIsBlocking(false);

	return newLink;
}

//---------------------------------------------------------
// Makes a socket non-blocking. Subsequent calls to recv
// should be checked wrt return value for existence or not
// of pending received data.
//
void SocketLink::SetIsBlocking (bool _isBlocking) {

	if (BAD_SOCKET(sockId))
		return;

	if (isBlocking == _isBlocking)
		return;

	isBlocking = _isBlocking;

#ifdef	_WIN32_
	unsigned long opt = isBlocking ? 0 : 1;
	if (ioctlsocket(sockId, FIONBIO, &opt)==SOCKET_ERROR)
		LOGMSG("ioctlsocket failed.\n");
#endif

#ifdef	_UNIX_
	int opt = isBlocking ? 0 : 1;
	if (ioctl(sockId, FIONBIO, &opt)==-1)
		if (!isBlocking) {
			if (fcntl(sockId, F_SETFL, O_NONBLOCK)==-1)
			LOGMSG("Both ioctl and fcntl failed to make non-blocking.\n");
		}
		else
			LOGMSG("ioctl failed to make blocking.\n");
#endif
}

///////////////////////////////////////////////////////////

bool SocketLink::ConnectionOk (void) {

	if (BAD_SOCKET(sockId))
		return false;

	fd_set set;
	FD_ZERO(&set);
	FD_SET(sockId, &set);
	struct timeval tv;
	tv.tv_sec	= 0;
	tv.tv_usec	= 0;

	if (select(sockId + 1, NULL, NULL, &set, &tv) IS_CALL_ERROR || FD_ISSET(sockId, &set)) {

		SOCKET_CLOSE(sockId);
		sockId = INVALID_SOCKET;

		return false;
	}
	else
		return true;
}

//---------------------------------------------------------
// If dataSize is 0, it means all locally stored msgs
// have been successfully consumed. Else, we have to concatenate
// the remaining with the newlly received data.
//
void SocketLink::ReceiveDataLocally (void) {

	// We need to protect the buffer with a mutex,
	// to avoid overwritting by multiple processes.
	// Also the socket to avoid changing blocking
	// mode with concurrent send / recv

	LOCK_BLOCK(sendrecvmutex); {

	LOCK_BLOCK(bufferumutex);

	fd_set set;
	FD_ZERO(&set);
	FD_SET(sockId, &set);

	struct timeval tv;
	tv.tv_sec	= 0;
	tv.tv_usec	= 0;

	if (select(sockId + 1, &set, NULL, NULL, &tv) IS_CALL_ERROR) {

		LOGMSG("SocketLink::ReceiveDataLocally: select() failed.\n");

		SOCKET_CLOSE(sockId);	// Socket is closed now.
		sockId = INVALID_SOCKET;
		return;
	}

	int size;
	if ((size = recv(sockId, (char*) buffer, SOCKETLIB_SOCKET_BUFFER_SIZE, 0)) IS_CALL_ERROR) {

#ifdef	_WIN32_
		int error = WSAGetLastError();
		if (error==WSAECONNRESET || error==WSAENOTCONN || error==WSAEHOSTDOWN)
			size = 0;
		else
			return;
#endif

#ifdef	_UNIX_
			return;
#endif
		
	}

	if (!size) {

		SOCKET_CLOSE(sockId);
		sockId = INVALID_SOCKET;
		return;
	}

	LOGMSG("Data received, size %d.\n", size);

	void* data = DNEWARR(util_ui8, size);
	memcpy(data, buffer, size);
	incoming.push_back(Packet(data, size));
	}
}

//---------------------------------------------------------
// Returns the hostname of the current host. See gethostname(2).
//
static char name[256];
const char*	SocketLink::GetHostName (void) {
	gethostname(name , sizeof(name)-1);
	return name;
}

//---------------------------------------------------------
// If no message is available try to receive one.
//
bool SocketLink::DataPending (void) {

	if (BAD_SOCKET(sockId))
		return false;
	else {
		if (incoming.empty())
			ReceiveDataLocally();
		return !incoming.empty();
	}
}

//---------------------------------------------------------
// Send will be considered failed if not all data have been
// sent successfully.
//
void SocketLink::SendData (void* data, util_ui32 size) {

	if (BAD_SOCKET(sockId))
		return;

	LOCK_BLOCK(sendrecvmutex);

#define	RETURN \
	if (true) { SetIsBlocking(oldIsBlocking); return; } else

	assert(!isBlocking);
	bool oldIsBlocking = isBlocking;
	SetIsBlocking(true);

	fd_set set;
	FD_ZERO(&set);
	FD_SET(sockId, &set);

	struct timeval tv;
	tv.tv_sec	= 0;
	tv.tv_usec	= 0;

	if (select(sockId + 1, NULL, &set, NULL, &tv) IS_CALL_ERROR) {

		LOGMSG("SocketLink::SendData: select() failed.\n");

		SOCKET_CLOSE(sockId);	// Socket is closed now.
		sockId = INVALID_SOCKET;
		return;
	}

	// Enter a send loop. In case of a failure, sleep for a second,
	// and then retry (three tries at most). If this still fails,
	// it means we have a communication break down.
	//
	util_ui32 tries = 1;

	while (true) {

		util_ui32 dataSent = send(sockId, (char*) data, size, 0);

		if (dataSent IS_CALL_ERROR) {

			LOGMSG("SocketLink::data: send() failed. Try %d.\n", tries);

			if (tries++ == SOCKETLIB_MAX_SEND_TRIES)
				RETURN;

			SLEEP(SOCKETLIB_FAILSEND_WAIT);
			continue;
		}
		else
		if (dataSent < size) {

			LOGMSG("SocketLink::data: send(): less data sent.\n");

			// Continue by trying to sent remaining data.
			size -= dataSent;
			PINC(data, dataSent);
		}
		else	{		// Ok, all sent.
			LOGMSG("Data sent, size %d.\n", dataSent);
			RETURN;
		}
	}

#undef	RETURN
}

//---------------------------------------------------------
// Firstly check if we already have data locally.
// If not, initialise with null and 0 the arguments.
//
void SocketLink::RecvData (void** putData, util_ui32* size) {

	if (DataPending()) {

		PacketList::iterator packetIter = incoming.begin();

		*putData	= packetIter->first;
		*size		= packetIter->second;

		incoming.pop_front();
	}
	else {
		*size		= 0;
		*putData	= (void*) 0;
	}
}

///////////////////////////////////////////////////////////

void SocketLink::BlockRecvData (void** data, util_ui32* size) {

	while (!DataPending())
		if (!SocketValid())
			return;

	RecvData(data, size);
}

///////////////////////////////////////////////////////////

void SocketLink::ClearData (void* data) {
	if (data)
		DDELARR((char*) data);
}
//---------------------------------------------------------

static bool initialized = false;

bool SocketLink::IsInitialised (void)
	{ return initialized; }

void SocketLink::Initialise (void) {

	assert(!IsInitialised());
	initialized = true;

#ifdef	_WIN32_

	WORD	wVersionRequested=MAKEWORD( 2, 0 );;
	WSADATA	wsaData;

	int err=WSAStartup(wVersionRequested, &wsaData);
	if (err) {
		printf("WinSock DLL not found.\n");
		exit(0);
	}
 
	/* Confirm that the WinSock DLL supports 2.0.	     */
	/* Note that if the DLL supports versions greater    */
	/* than 2.0 in addition to 2.0, it will still return */
	/* 2.0 in wVersion since that is the version we      */
	/* requested.                                        */
 
	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0 ) {
		printf("WinSock 2.2 or higher not found.\n");
		WSACleanup( );
		exit(0); 
	}
#endif

}

///////////////////////////////////////////////////////////

void SocketLink::CleanUp (void) {
	#ifdef	_WIN32_
		WSACleanup();
	#endif
	initialized = false;
}

//---------------------------------------------------------