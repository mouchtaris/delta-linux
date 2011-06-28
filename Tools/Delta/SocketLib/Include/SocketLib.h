// SocketLib.h
// Socket library for building the I-GET basic communication
// layer. Addresses both UNIX and Windows system, original version January 1996.
// A. Savidis, deployed as it is from I-GET UIMS, January 2000.
//

#ifndef	SOCKETLIB_H
#define	SOCKETLIB_H

#include "utypes.h"
#include "SocketLibDefs.h"

//---------------------------------------------------------

#define	SOCKETLIB_MAX_LISTEN_QUEUE		3
#define	SOCKETLIB_MAX_SEND_TRIES		3
#define	KBYTES							1024
#define	SOCKETLIB_SOCKET_SEND_SIZE		4*KBYTES
#define	SOCKETLIB_SOCKET_RECV_SIZE		4*KBYTES

#ifdef	_WIN32_
#define	SOCKETLIB_CONNECT_WAIT			300		// msecs.
#define	SOCKETLIB_FAILSEND_WAIT			1000	// msecs
#endif

#ifdef	_UNIX_
#define	SOCKETLIB_CONNECT_WAIT			1		// secs.
#define	SOCKETLIB_FAILSEND_WAIT			1		// secs.
#endif

#define	SOCKETLIB_CONNECT_TRIES			3
#define	SOCKETLIB_SOCKET_LOGFILE		"socketlog.txt"
#define	SOCKETLIB_HOST_NAME_LEN			32
#define	SOCKETLIB_MAX_CONNECTIONS		10		// To ckeck max TSs allowed.
#define	TCP								"tcp"

#include <stdio.h>
#include <list>
#include "uthreadlib.h"

//---------------------------------------------------------

#ifdef	_UNIX_
#define	SOCKET				int
#define	BAD_SOCKET(s)		((s)<0)
#define	INVALID_SOCKET		-1
#define	SOCKET_CLOSE		close
#define	IS_CALL_ERROR		< 0
#define	SOCKLEN_T			socklen_t
#define	GETPID				getpid
#define	SLEEP				sleep
#endif

#ifdef	_WIN32_
#include <winsock2.h>
#include <process.h>
#define	BAD_SOCKET(s)		((s)==INVALID_SOCKET)
#define	SOCKET_CLOSE		closesocket
#define	SOCKLEN_T			int
#define	IS_CALL_ERROR		==SOCKET_ERROR
#define	GETPID				_getpid
#define	SLEEP				Sleep
#endif

//---------------------------------------------------------
// In some systems, int uses only 2 bytes. So, to avoid
// problems, the buffer size of min(65535, INT_MAX). This
// is necessary since the I-GET  communication layer uses
// 'int' to indicate communication buffer sizes.
//
#include <limits.h>

#if (INT_MAX > 65535)
#define	SOCKETLIB_SOCKET_BUFFER_SIZE		65535	
#else
#define	SOCKETLIB_SOCKET_BUFFER_SIZE		INT_MAX
#endif

//---------------------------------------------------------
// Macros for logging facilities.
//
#define	LOGMSG									\
	logFile = fopen(LOGNAME,"at");				\
    fprintf(logFile,

#define	LOGADD	fclose(logFile);
#define	LOGNAME	"socketlog.txt"

#ifdef IF_WINDOZE_SUPPORT_THIS
#define LOGMSG(...) do {						\
		logFile = fopen(LOGNAME, "at");			\
		assert (logFile);						\
		fprintf(logFile, __VA_ARGS__);			\
		fclose(logFile);						\
	}											\
	while (0)
#endif /* IF_WINDOZE_SUPPORT_THIS */

//---------------------------------------------------------
// A higher-level abstraction on sockets, supporting
// data transfer, recv interrogation and local data
// storage.
//
class SOCKETLIB_CLASS SocketLink {

	private:
	static FILE*		logFile;
	static umutex		bufferumutex;
	static umutex		connectumutex;
	static umutex		acceptumutex;

	///////////////////////////////////////////////////////////

	typedef	std::pair<void*, util_ui32>	Packet;
	typedef	std::list<Packet>			PacketList;

	PacketList		incoming;
	bool			isBlocking;
	static char		buffer[SOCKETLIB_SOCKET_BUFFER_SIZE];

	SOCKET			sockId;
	char			myHost[SOCKETLIB_HOST_NAME_LEN];
	util_ui32		myPort;	

	void			SetIsBlocking (bool isBlocking);
	void			ReceiveDataLocally (void);
	bool			SetOptions (const char* func);
	bool			SetRecvSendBufferSize (const char* func);

	SocketLink (const char* host, util_ui32 port, SOCKET _sockId);

	///////////////////////////////////////////////////////////

	public:
	static void				Initialise (void);
	static bool				IsInitialised (void);
	static void				CleanUp (void);
	
	static void				GetDottedAddress (unsigned long addr, char* putDottedAddr);
	static unsigned long	GetAddress (const char* dottedAddr);

	void					EstablishAsServer (util_ui32 portNum = 0);
	void					EstablishAsClient (void);

	SocketLink*				AcceptClient (bool blocking = true);
	void					ConnectWithServer (const char* serverHost, util_ui32 serverPort);
	bool					SocketValid (void) const
								{ return !BAD_SOCKET(sockId); }
	bool					ConnectionOk (void);
	const char*				GetHost (void) { return myHost; }
	static const char*	    GetHostName (void);
	util_ui32				GetPort (void) const 
									{ return myPort; }

	bool					DataPending (void);
	void					SendData (void* data, util_ui32 size);
	void					RecvData (void** data, util_ui32* size);
	void					BlockRecvData (void** data, util_ui32* size);
	static void				ClearData (void* data);

	SocketLink (void);
	virtual ~SocketLink();
};

//---------------------------------------------------------

#endif	// Do not add stuff beyond this point.
