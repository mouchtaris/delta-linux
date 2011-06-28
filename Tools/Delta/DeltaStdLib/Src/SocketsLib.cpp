// SocketsLib.cpp
// Sockets library for Delta using input / output buffers.
// A. Savidis, May 2009.
// Extended March 2011, supported optional flag (arg)
// to choose if user packets are wrapped with extra
// size information or not.
//

#include "DeltaLibraryCreators.h"
#include "ubinaryio.h"
#include "BufferLib.h"
#include "SocketsLibP.h"
#include "SocketNetLink.h"
#include "SocketsLib.h"
#include "DeltaStdLibFuncNames.h"

//------------------------------------------------------------------
// FIELD GETTER
//
static const char* categoryStrings[] = {
	"basic", "client", "server"
};

static DeltaExternIdFieldGetter* getter = (DeltaExternIdFieldGetter*) 0;

static bool GetKeys (void* val, DeltaValue* at) 
	{ return SOCKET_VAL(val)? DPTR(getter)->GetAllKeys(val, at) : false ; }

static bool GetCategory (void* val, DeltaValue* at) 
	{ at->FromString(categoryStrings[SOCKET_VAL(val)->GetCategory()]); return true; }

static bool GetPort (void* val, DeltaValue* at) {
	SocketWrapper* wrapper = SOCKET_VAL(val);
	DASSERT(DPTR(wrapper)->GetCategory() == SocketWrapper::Server);
	at->FromNumber(DPTR(wrapper)->GetPort()); 
	return true;
}

static bool GetAddress (void* val, DeltaValue* at) {
	SocketWrapper* wrapper  = SOCKET_VAL(val);
	DASSERT(DPTR(wrapper)->GetCategory() == SocketWrapper::Client);
	at->FromString(DPTR(wrapper)->GetAddress()); 
	return true;
}

static void ToSocket (DeltaValue* at, SocketWrapper* server);

static bool GetCreator (void* val, DeltaValue* at) {
	SocketWrapper* wrapper  = SOCKET_VAL(val);
	DASSERT(DPTR(wrapper)->GetCategory() == SocketWrapper::Basic);
	ToSocket(at, DPTR(wrapper)->GetCreator());
	return true;
}

/////////////////////////////////////////////////////////////////////

static bool IsClient (void* val) {
	SocketWrapper* wrapper  = SOCKET_VAL(val); 
	return wrapper && DPTR(wrapper)->GetCategory() ==  SocketWrapper::Client; 
}

static bool IsServer (void* val) {
	SocketWrapper* wrapper  = SOCKET_VAL(val); 
	return wrapper && DPTR(wrapper)->GetCategory() ==  SocketWrapper::Server;
}

static bool IsBasic (void* val) {
	SocketWrapper* wrapper  = SOCKET_VAL(val); 
	return wrapper && DPTR(wrapper)->GetCategory() ==  SocketWrapper::Basic;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",			&GetKeys,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "category",		&GetCategory,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "port",			&GetPort,		&IsServer						},
	{ "address",		&GetAddress,	&IsClient						},
	{ "creator",		&GetCreator,	&IsBasic						}
};

//------------------------------------------------------------------
// API, CREATION, BINDERS.
//
static void socket_createforclient_LibFunc (DeltaVirtualMachine*);
static void socket_createforserver_LibFunc (DeltaVirtualMachine*);
static void socket_destroy_LibFunc (DeltaVirtualMachine*);
static void socket_connecttoserver_LibFunc (DeltaVirtualMachine*);
static void socket_acceptclient_LibFunc (DeltaVirtualMachine*);
static void socket_isconnectionbroken_LibFunc (DeltaVirtualMachine*);

static void socket_ismessagepending_LibFunc (DeltaVirtualMachine*);
static void socket_waitanymessage_LibFunc (DeltaVirtualMachine*);
static void socket_receive_LibFunc (DeltaVirtualMachine*);
static void socket_blockreceive_LibFunc (DeltaVirtualMachine*);
static void socket_send_LibFunc (DeltaVirtualMachine*);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "createforclient",	socket_createforclient_LibFunc },
	{ "createforserver",	socket_createforserver_LibFunc },
	{ "destroy",			socket_destroy_LibFunc },
	{ "connecttoserver",	socket_connecttoserver_LibFunc },
	{ "acceptclient",		socket_acceptclient_LibFunc },
	{ "isconnectionbroken", socket_isconnectionbroken_LibFunc },
	{ "ismessagepending",	socket_ismessagepending_LibFunc },
	{ "waitanymessage",		socket_waitanymessage_LibFunc },
	{ "receive",			socket_receive_LibFunc },
	{ "blockreceive",		socket_blockreceive_LibFunc },
	{ "send",				socket_send_LibFunc }
};

/////////////////////////////////////////////////////////////////////

inline bool VAL_IS_SOCKET (const DeltaValue& val)
	{ return val.Type() == DeltaValue_ExternId && val.GetExternIdTypeString() == SOCKET_TYPE_STR; }

DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(VAL_IS_SOCKET(val), SOCKET_TYPE_STR)

DELTALIBFUNC_DECLARECONSTS(2, uarraysize(funcs) - 2, "destroy", "send")

DLIB_INITIALIZE_CLEANUP_FUNCS_EX(
	Sockets, 
	"socket", 
	DELTA_STDLIB_NAMESPACE_PREFIX "socket_",
	SocketNetLink::Initialise(); DELTALIBFUNC_CREATE_METHODS_TABLE();,
	SocketNetLink::CleanUp(); DELTALIBFUNC_DESTROY_METHODS_TABLE();
)

DLIB_TOSTRING_FUNC(SocketWrapper, SOCKET_TYPE_STR, SOCKET_VAL)

DLIB_TOEXTERNID_FUNC_WITH_LIBRARY_METHODS(SocketWrapper, SOCKET_TYPE_STR, ToSocket)

//------------------------------------------------------------------

DLIB_FUNC_START(socket_createforclient, 0, Nil)
ClientSocketPacketLink* socket	= DNEW(ClientSocketPacketLink);
SocketWrapper*			wrapper = DNEWCLASS(SocketWrapper, (socket, DELTA_SOCKETSLIB_CLIENT_NOTCONNECTED_ADDRESS));
ToSocket(DLIB_RETVAL_PTR, wrapper);
DLIB_FUNC_END

DLIB_FUNC_START(socket_createforserver, 1, Nil)
DLIB_ARG(util_ui32, port)
ServerSocketPacketLink* socket	= DNEWCLASS(ServerSocketPacketLink, (DLIB_ARGVAL(port)));
SocketWrapper*			wrapper = DNEWCLASS(SocketWrapper, (socket,  DLIB_ARGVAL(port)));
ToSocket(DLIB_RETVAL_PTR, wrapper);
DLIB_FUNC_END

DLIB_FUNC_START(socket_destroy, 1, Empty)
DLIB_GET_SOCKET
SocketPacketNetLink* socket = DPTR(socketWrapper)->GetSocket();
DDELETE(socket);
DDELETE(socketWrapper);
DLIB_FUNC_END

/////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(socket_connecttoserver, 4, False)
DLIB_GET_SOCKET
DLIB_ARG(const char*,	address)
DLIB_ARG(util_ui32,		port)
DLIB_ARG(util_ui32,		timeout)
DLIB_ERROR_CHECK(
	DPTR(socketWrapper)->GetCategory() != SocketWrapper::Client,
	"client socket expected"
)
ClientSocketPacketLink* client = DPTR(socketWrapper)->GetClientSocket();
DPTR(client)->ConnectWithServer(
	DLIB_ARGVAL(address),
	DLIB_ARGVAL(port),
	DLIB_ARGVAL(timeout)
);
if (!DPTR(client)->IsConnectionBroken()) {
	socketWrapper->SetAddress(
		uconstructstr("host:%s, port:%u", DLIB_ARGVAL(address), DLIB_ARGVAL(port))
	);
	DLIB_RETVAL_REF.FromBool(true);
}
else
	DLIB_RETVAL_REF.FromBool(false);
DLIB_FUNC_END

/////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(socket_acceptclient, 3, Nil)
DLIB_GET_SOCKET
DLIB_ARG(bool,			blocking)
DLIB_ARG(util_ui32,		timeout)
DLIB_ERROR_CHECK(
	DPTR(socketWrapper)->GetCategory() != SocketWrapper::Server,
	"server socket expected"
)
ServerSocketPacketLink* server =	DPTR(socketWrapper)->GetServerSocket();
SocketPacketNetLink*	client =	DPTR(server)->AcceptClient(
										DLIB_ARGVAL(blocking),
										DLIB_ARGVAL(blocking) ? 0 : DLIB_ARGVAL(timeout)
									);
if (client) { // Accepted client successfully.
	SocketWrapper*	wrapper = DNEWCLASS(SocketWrapper, (client, socketWrapper));
	ToSocket(DLIB_RETVAL_PTR, wrapper);
}
else
	DLIB_RETVAL_REF.FromNil();
DLIB_FUNC_END

/////////////////////////////////////////////////////////////////////

#define	IF_SERVERSOCKET_ERROR_CHECK()									\
DLIB_ERROR_CHECK(														\
	DPTR(socketWrapper)->GetCategory() == SocketWrapper::Server,		\
	"can't use server socket here (it is only for accepting clients)"	\
)

#define	GET_OPT_PACKET_WRAPPING											\
DLIB_OPT_ARG(const char*, wrapping, DELTA_SOCKET_PACKET_ADD_SIZE);		\
if (strcmp(DLIB_ARGVAL(wrapping), DELTA_SOCKET_PACKET_ADD_SIZE) &&		\
	strcmp(DLIB_ARGVAL(wrapping), DELTA_SOCKET_PACKET_ORIGINAL))		\
	DLIB_ERROR_REPORT(													\
		uconstructstr(													\
			"invalid packet wrapping '%s'",								\
			DLIB_ARGVAL(wrapping)										\
		)																\
	)

#define	IS_WRAPPING_ADD_SIZE()	\
	(!strcmp(DLIB_ARGVAL(wrapping), DELTA_SOCKET_PACKET_ADD_SIZE))

#define	EXTRACT_SOCKET_LINK												\
NetLink* socket = (NetLink*) 0;											\
if (IS_WRAPPING_ADD_SIZE())												\
	socket = DPTR(socketWrapper)->GetSocket();							\
else																	\
	socket = DPTR(socketWrapper)->GetSocket()->GetPureLink();

DLIB_FUNC_START(socket_isconnectionbroken, 1, True)
DLIB_GET_SOCKET
IF_SERVERSOCKET_ERROR_CHECK()
DLIB_RETVAL_REF.FromBool(DPTR(DPTR(socketWrapper)->GetSocket())->IsConnectionBroken());
DLIB_FUNC_END

DLIB_FUNC_START_OPT_ARGS(socket_ismessagepending, 1, 1, False)
DLIB_GET_SOCKET
IF_SERVERSOCKET_ERROR_CHECK()
GET_OPT_PACKET_WRAPPING
EXTRACT_SOCKET_LINK
DLIB_RETVAL_REF.FromBool(DPTR(socket)->IsMsgPending());
DLIB_FUNC_END

DLIB_FUNC_START_OPT_ARGS(socket_waitanymessage, 2, 1, Empty)
DLIB_GET_SOCKET
IF_SERVERSOCKET_ERROR_CHECK()
DLIB_ARG(util_ui32, timeout)
GET_OPT_PACKET_WRAPPING
EXTRACT_SOCKET_LINK
DPTR(socket)->WaitAnyMessage(DLIB_ARGVAL(timeout));
DLIB_FUNC_END

/////////////////////////////////////////////////////////////////////

#define	RECEIVE_MESSAGE(call)														\
	if (true) {																		\
	DPTR(socket)->call();															\
	void*		data;																\
	util_ui32	size;																\
	DPTR(socket)->ReadMsg(&data, &size);											\
	ubinaryio::InputBuffer* ib = DNEWCLASS(ubinaryio::InputBuffer, (data, size));	\
	DeltaInputBuffer_Make(DLIB_RETVAL_PTR, ib);										\
	DPTR(socket)->ClearMsg();														\
	} else

//********************************

DLIB_FUNC_START_OPT_ARGS(socket_blockreceive, 1, 1, Nil)
DLIB_GET_SOCKET
IF_SERVERSOCKET_ERROR_CHECK()
GET_OPT_PACKET_WRAPPING
EXTRACT_SOCKET_LINK
if (DPTR(socket)->IsConnectionBroken())
	DLIB_RETVAL_REF.FromNil();
else 
	RECEIVE_MESSAGE(BlockReceiveMsg);
DLIB_FUNC_END

//********************************

DLIB_FUNC_START_OPT_ARGS(socket_receive, 1, 1, Nil)
DLIB_GET_SOCKET
IF_SERVERSOCKET_ERROR_CHECK()
GET_OPT_PACKET_WRAPPING
EXTRACT_SOCKET_LINK
if (DPTR(socket)->IsConnectionBroken())
	DLIB_RETVAL_REF.FromNil();
else
if (!DPTR(socket)->IsMsgPending())
	DLIB_RETVAL_REF.FromNil();
else 
	RECEIVE_MESSAGE(ReceiveMsg);
DLIB_FUNC_END

//********************************

DLIB_FUNC_START_OPT_ARGS(socket_send, 2, 1, False)
DLIB_GET_SOCKET
IF_SERVERSOCKET_ERROR_CHECK()
DLIB_GET_OUTPUTBUFFER
GET_OPT_PACKET_WRAPPING
EXTRACT_SOCKET_LINK
if (DPTR(socket)->IsConnectionBroken())
	DLIB_RETVAL_REF.FromBool(false);
else {
	util_ui32	size;
	void*		data = DPTR(ob)->MakeBuffer(&size);
	DPTR(socket)->SendMsg(data, size);
	ubinaryio::OutputBuffer::ClearBuffer(data);
	DLIB_RETVAL_REF.FromBool(true);
}
DLIB_FUNC_END

//------------------------------------------------------------------
