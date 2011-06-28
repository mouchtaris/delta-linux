// SocketLib.hh
// Socket library for building the I-GET basic communication
// layer. Addresses both UNIX and Windows system.
// A. Savidis, January 2000.
//

#ifndef	SOCKETLIB_HH
#define	SOCKETLIB_HH

#define	_W32BIT_

//---------------------------------------------------------

#define	IGET_MAX_LISTEN_QUEUE		3
#define	IGET_MAX_SEND_TRIES			3
#define	KBYTES						1024
#define	IGET_SOCKET_SEND_SIZE		256*KBYTES
#define	IGET_SOCKET_RECV_SIZE		256*KBYTES

#ifdef	_W32BIT_
#define	IGET_CONNECT_WAIT			300		// msecs.
#define	IGET_FAILSEND_WAIT			1000	// msecs
#endif
#ifdef	_UNIX_
#define	IGET_CONNECT_WAIT			1		// secs.
#define	IGET_FAILSEND_WAIT			1		// secs.
#endif

#define	IGET_CONNECT_TRIES			3
#define	IGET_SOCKET_LOGFILE			"socketlog.txt"
#define	IGET_HOST_NAME_LEN			32
#define	IGET_MAX_CONNECTIONS		10		// To ckeck max TSs allowed.
#define	TCP							"tcp"
//---------------------------------------------------------

#ifdef	_UNIX_
#define	SOCKET			int
#define	BAD_SOCKET(s)	((s)<0)
#define	INVALID_SOCKET	-1
#define	SOCKET_CLOSE	close
#define	IS_CALL_ERROR	< 0
#define	GETPID			getpid
#define	SLEEP			sleep
#endif

#ifdef	_W32BIT_
#ifdef _MSC_VER
#include <winsock2.h>
#include <process.h>
#endif
#define	BAD_SOCKET(s)	((s)==INVALID_SOCKET)
#define	SOCKET_CLOSE	closesocket
#define	IS_CALL_ERROR	== SOCKET_ERROR
#define	GETPID			_getpid
#define	SLEEP			Sleep
#endif

//---------------------------------------------------------
// In some systems, int uses only 2 bytes. So, to avoid
// problems, the buffer size of min(65535, INT_MAX). This
// is necessary since the I-GET  communication layer uses
// 'int' to indicate communication buffer sizes.
//
#include <limits.h>

#if (INT_MAX > 65535)
#define	IGET_SOCKET_BUFFER_SIZE		65535	
#else
#define	IGET_SOCKET_BUFFER_SIZE		INT_MAX
#endif

//---------------------------------------------------------
// Macros for logging facilities.
//
#define	LOGMSG \
	logFile=fopen(LOGNAME,"at"); \
    fprintf(logFile,
#define	LOGADD	fclose(logFile);
#define	LOGNAME	"socketlog.txt"

//---------------------------------------------------------
// A higher-level abstraction on sockets, supporting
// data transfer, recv interrogation and local data
// storage.
//
class SocketLink {

	private:
	static char		buffer[IGET_SOCKET_BUFFER_SIZE];
	void*			data;			// Receive local buffer with many msgs pending.
	void*			recvData;		// Next msg to be received.
	int				dataSize;		// Available bytes not yet read from local buffer.
	SOCKET			sockId;
	char			myHost[IGET_HOST_NAME_LEN];
	unsigned int	myPort;	
	void			MakeNonBlocking (int status=1);
	void			ReceiveDataLocally (int checkRead=0);
	int				MessageUnAvailable (void);

	public:

	static void		Initialise (void);
	static void		CleanUp (void);

	void			EstablishAsServer (unsigned int portNum=0);
	void			EstablishAsClient (void);

	SocketLink*		AcceptClient (int blocking=1);
	void			ConnectWithServer (char* serverHost, unsigned int serverPort);
	int				SocketValid (void) { return !BAD_SOCKET(sockId); }
	char*			GetHost (void) { return myHost; }
	unsigned int	GetPort (void) { return myPort; }

	int				DataPending (int checkRead=0);
	static void		ClearData (void* data);
	void			SendData (const void* data, unsigned int size, int checkWrite=0);
	void			RecvData (void** data, unsigned int* size, int checkRead=0);
	void			BlockRecvData (void** data, unsigned int* size, int checkRead=0);

	SocketLink (void);
	SocketLink (char* host, unsigned int port, SOCKET _sockId);
	~SocketLink();
};

//---------------------------------------------------------

#endif	// Do not add stuff beyond this point.