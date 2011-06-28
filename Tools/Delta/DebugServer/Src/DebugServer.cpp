// DebugServer.cpp
// Debug server and thread implementation.
// ScriptFighter Project.
// A. Savidis, February 2002.
// Re-factored May 2006, detecting client link failure
// and forced exit.
// Updated July 2007, using source file to organize bpts instead of
// the VM id.
//

#include "DebugServer.h"
#include "SocketPacketNetLink.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DebugBreakPointHolder.h"
#include "DebugExprParserAPI.h"
#include "uthreadlib.h"
#include "usystem.h"
#include "DeltaDebugExtensions.h"
#include "DeltaDebugMessageEncoding.h"
#include "DeltaDebugServerBreakPoints.h"
#include "ufiles.h"
#include "ugeometry.h"
#include "DeltaDebugDynamicActivator.h"

using namespace DebugMessages;

////////////////////////////////////////////////////////////

#define	CHECK_SENDRECEIVE				0
#define	LAST_SOURCE						"*"
#define	LAST_BPTLINE					0
#define	SERVICELOOP_MIN_SLEEPTIME		10
#define	SERVICELOOP_MAX_SLEEPTIME		2000
#define	SERVICELOOP_ADD_SLEEPTIME		50
#define	NEGOTIATIONCONNECT_TIMEOUT		5000
#define	TRYCLIENT_CONNECTION_SLEEPTIME	300

#define	CLIENT_LINK_LOCK() \
	LOCK_BLOCK((*clientLinkMutex))

#define	DATA_CREATION_LOCK() \
	LOCK_BLOCK((*dataCreationMutex))

////////////////////////////////////////////////////////////

static	Buffer*						buffer 					= (Buffer*) 0;
static	ServerSocketPacketLink*		server 					= (ServerSocketPacketLink*) 0;
static	SocketPacketNetLink*		client 					= (SocketPacketNetLink*) 0;
static	void*						receivedData			= (void*) 0;
static	util_ui32					receivedSize			= 0;
static	uthread*					debugThread				= (uthread*) 0;			// The debug server thread.
static	uthread*					connectThread			= (uthread*) 0;			// The client connection thread.
static	umutex*						clientLinkMutex			= (umutex*) 0;			// umutex to ensure proper socket use by independent threads.
static	umutex*						dataCreationMutex		= (umutex*) 0;			// umutex to ensure proper socket use by independent threads.
static	bool						debugStoppedByClient	= false;
static	void						(*exitFunc)(void)		= (void(*)(void)) 0;
static	bool						clientWasAttached		= false;

////////////////////////////////////////////////////////////

bool DeltaDebugServer::IsClientDisconnectedAndStartedRetryingReconnection (void) {

	DASSERT(debugThread);

	if (!client || IsConnectionBroken()) {	// Client link may have been destroyed by the product thread.

		udeleteunlessnull(client);

		// We reset the debug extensions of the messenger so that
		// no debug client notifications are posted.
		//
		DeltaDebugExtensions::GetMessenger().ResetOnInvalidBreakPoint();
		DeltaDebugExtensions::GetMessenger().ResetOnValidBreakPoint();
		DeltaDebugExtensions::GetMessenger().ResetOnInvalidCondition();
		DeltaDebugExtensions::GetMessenger().ResetOnMostRecentFuncResults();
		DeltaDebugExtensions::GetMessenger().ResetOnStopPoint();
		DeltaDebugExtensions::GetMessenger().ResetOnSendCurrFunction();
		DeltaDebugExtensions::GetMessenger().ResetOnSendDynamicCode();
		DeltaDebugExtensions::GetMessenger().ResetIsConnected();

		// In case we were stopped in a breakpoint (tracing) we must artificially post 
		// a Go() request to the tracer so that the product continues its execution.
		HandleGo();

		// We respawn the debug client connection thread.
		DPTR(connectThread)->Start();

		// And ask to unload the backend if it was dynamically loaded.
		if (DeltaDebugDynamicActivator::GetSingleton().HasLoaded())
			DeltaDebugDynamicActivator::GetSingleton().SetPendingUnload();

		return true;
	}
	else
		return false;
}

////////////////////////////////////////////////////////////

struct AdaptiveSleep {

	util_ui32	delay;
	util_ui32	t_min;
	util_ui32	t_max;
	util_ui32	t_add;

	util_ui32	bedtime (void) const 
					{ return delay;  }

	void		action (void) 
					{ delay = t_min; }

	void		wait (void) {
					if (delay < (t_max - t_add))
						delay += t_add;
					else
						delay = t_max;
				}

	AdaptiveSleep (util_ui32 _min, util_ui32 _max, util_ui32 _add) :
		delay(_min), 
		t_min(_min), 
		t_max(_max), 
		t_add(_add){}
};

////////////////////////////////////////////////////////////

void DeltaDebugServer::DebugServiceLoopThread (void* unused) {

	uprocesssleep(TRYCLIENT_CONNECTION_SLEEPTIME);

	AdaptiveSleep	sleeper(
						SERVICELOOP_MIN_SLEEPTIME,
						SERVICELOOP_MAX_SLEEPTIME,
						SERVICELOOP_ADD_SLEEPTIME
					);

	while (!DPTR(debugThread)->IsExitRequested()) {

		uprocesssleep(sleeper.bedtime());

		{ 
			CLIENT_LINK_LOCK();

			if (IsClientDisconnectedAndStartedRetryingReconnection())
				return;
			else
			if (DeltaDebugServer::IsRequestPending()) {
				DeltaDebugServer::ReceiveRequest();
				sleeper.action();
			}
			else
				{ sleeper.wait(); continue; }
		}

		DeltaDebugServer::DispatchRequest(DeltaDebugServer::GetRequestType());
		DeltaDebugServer::RequestProcessed();
	}
}

////////////////////////////////////////////////////////////
// Receive all break points synchronously from the client.
// Messages should be of Debug_AddBreakPoint type.
// Finishing with source=='*' and line==0.
//
bool DeltaDebugServer::ReceiveAllBreakPoints (void) {

	bool waitingAddRequest = true;

	while (true) 
		if (IsConnectionBroken())	// Failed while receiving breakpoints.
			return false;
		else
		if (IsRequestPending()) {

			ReceiveRequest();

			char*		source;
			util_ui32	line;

			if (waitingAddRequest) {

				DASSERT(GetRequestType() == Debug_AddBreakPoint);

				char* cond;
				GetAddBreakPoint(&source, &line, &cond);

				// Check for the last bpt message.
				//
				if (!strcmp(source, LAST_SOURCE) && line == LAST_BPTLINE)	
					{ RequestProcessed(); return true; }

				DeltaDebugServerBreakPoints::Get().Add(source, line, cond);
			}
			else {
				if (GetRequestType() == Debug_EnableBreakPoint) {
					GetEnableBreakPoint(&source, &line);
					DeltaDebugServerBreakPoints::Get().Enable(source, line);
				}
				else
				if (GetRequestType() == Debug_DisableBreakPoint) {
					GetDisableBreakPoint(&source, &line);
					DeltaDebugServerBreakPoints::Get().Disable(source, line);
				}
				else
					DASSERT(false);	
			}

			waitingAddRequest = !waitingAddRequest;
			RequestProcessed();
		}
}

////////////////////////////////////////////////////////////

bool DeltaDebugServer::IsNegotiationWithClientSuccessful (int argc, char** argv) {

	util_ui32 negotiationPort = 0;
	while (argc-- > 1)
		if (!strcmp(argv[argc], DEBUG_CLIENT_NEGOTIATIONPORT_CMMDARG))
			{ negotiationPort = atoi(argv[argc + 1]); break; }

	if (negotiationPort) {

		ClientSocketPacketLink*	client = (ClientSocketPacketLink*) 0;
		unew(client);

		DPTR(client)->ConnectWithServer(
			SocketNetLink::GetHostName(), 
			negotiationPort, 
			NEGOTIATIONCONNECT_TIMEOUT
		);

		if (!DPTR(client)->IsConnectionBroken()) {	

			util_ui32	size = SizeUnsignedInt();
			void*		data = DNEWARR(char, size);	

			WriteUnsignedInt((char*) data, DPTR(server)->GetPort());
			DPTR(client)->SendMsg(data, size);

			DDELARR((char*) data);
			udelete(client);

			return clientWasAttached = true;	// Client informed. Automatic attach.
		}
	}
	
	return false;	// Client was not informed. Manual attach.
}

////////////////////////////////////////////////////////////

void DeltaDebugServer::UserMessageAboutServerPort (void) {

	umodalmessage(
		"Debug server information", 
		uconstructstr(
			"Running at host '%s', with process 'id' %d\n"
			"while waiting at port '%d' for debug client connection.",
			SocketNetLink::GetHostName(),
			ugetpid(),
			DPTR(server)->GetPort()
		)
	);

	FILE* fp = fopen(DEBUG_SERVER_PORT_INFO_FILE, "wt");
	fprintf(fp, "Debug server pid %d, port %d.\n", ugetpid(), DPTR(server)->GetPort());
	fclose(fp);
}

////////////////////////////////////////////////////////////
// Can be done externally to cause closing down the service loop
// and restarting a connection retry round.
//
void DeltaDebugServer::CloseClientConnection (void) {
	if (client) {
		CLIENT_LINK_LOCK();
		udelete(client);
		clientWasAttached = false;
	}
}

////////////////////////////////////////////////////////////

bool DeltaDebugServer::TryAcceptClient (void) {

	while (!DPTR(connectThread)->IsExitRequested()) {
		uprocesssleep(TRYCLIENT_CONNECTION_SLEEPTIME);
		if ((client = DPTR(server)->AcceptClient(false)) != 0) {
			if (DPTR(client)->IsConnectionBroken())
				udelete(client);
			else
				break;
		}
	}
	if (DPTR(connectThread)->IsExitRequested()) 
		{ udeleteunlessnull(client); return false; }
	else
		return true;
}

////////////////////////////////////////////////////////////

bool DeltaDebugServer::InitialiseAfterClientConnection (void) {

	DATA_CREATION_LOCK();

	if (!ReceiveAllBreakPoints())
		{ udeleteunlessnull(client); return false; }
	else {
		if (!debugThread)
			debugThread = DNEWCLASS(uthread, (DebugServiceLoopThread));

		DeltaDebugExtensions::GetMessenger().SetOnInvalidBreakPoint(&DoInvalidBreakPointAdapter);
		DeltaDebugExtensions::GetMessenger().SetOnValidBreakPoint(&DoInfoValidBreakPoint);
		DeltaDebugExtensions::GetMessenger().SetOnInvalidCondition(&DoInvalidConditionAdapter);
		DeltaDebugExtensions::GetMessenger().SetOnMostRecentFuncResults(&DoInfoMostRecentFuncResults);
		DeltaDebugExtensions::GetMessenger().SetOnStopPoint(&DoInfoStopPointAdapter);
		DeltaDebugExtensions::GetMessenger().SetOnSendCurrFunction(&DoInfoCurrFunction);
		DeltaDebugExtensions::GetMessenger().SetOnSendDynamicCode(&DoInfoDynamicCode);
		DeltaDebugExtensions::GetMessenger().SetIsConnected();

		DPTR(debugThread)->Start();

		return true;
	}
}

////////////////////////////////////////////////////////////

void DeltaDebugServer::CleanUpForClientConnection (void) {

	DATA_CREATION_LOCK();

	if (debugThread)	// May be running or not.		
		uthread::RequestExitWaitAndDestroy(debugThread);

	if (client) {		// Was already connected.

		// If it was not the client that requested stop us we 
		// should inform the client that we are closing down.
		//
		if (!debugStoppedByClient && !DPTR(client)->IsConnectionBroken())	
			DeltaDebugServer::DoInfoStop();	
		udelete(client);
	}

	if (connectThread)	// Running or not the following call is safe.
		uthread::RequestExitWaitAndDestroy(connectThread);
}

////////////////////////////////////////////////////////////
// Blocking unless an error occurs while trying to accept.
//
void DeltaDebugServer::TryEstablishClientConnection (void* unused) {

	CLIENT_LINK_LOCK();
	while (TryAcceptClient() && !InitialiseAfterClientConnection())
		;
}	

////////////////////////////////////////////////////////////

util_ui32 DeltaDebugServer::GetPort (void) {
	DASSERT(server);
	return DPTR(server)->GetPort();
}

////////////////////////////////////////////////////////////

bool DeltaDebugServer::Initialise (util_ui32 port, int argc, char** argv) {

	SocketNetLink::Initialise();
	DebugExprEvaluator::SingletonCreate();
	unew(handlers);
	InstallRequestHandlers();

	unew(buffer);
	unew(clientLinkMutex);
	unew(dataCreationMutex);

	if (!(server = DNEWCLASS(ServerSocketPacketLink, (port))))
		return false;
	else {

		// We create the thread object for coherence of the code
		// handling the connection trials.
		//
		connectThread = DNEWCLASS(uthread, (TryEstablishClientConnection));
		if (IsNegotiationWithClientSuccessful(argc, argv)) {
			TryEstablishClientConnection();		// Blocking waiting for connection.
			return !!client;
		}
		else {
			DPTR(connectThread)->Start();		// Attaching debug client at any time.
			UserMessageAboutServerPort();		// Message
			return true;
		}
	}
}

////////////////////////////////////////////////////////////

bool DeltaDebugServer::IsClientConnected (void) {
	if (client) {
		CLIENT_LINK_LOCK();
		return !DPTR(client)->IsConnectionBroken();
	}
	else
		return false;
}

////////////////////////////////////////////////////////////

bool DeltaDebugServer::ClientWasAttached (void) {
	DASSERT(client);
	return clientWasAttached;
}

////////////////////////////////////////////////////////////
// Should never be called from within the debugger thread function.
// It has to be called externally from the main script program
// upon finishing-up. We need not lock the client link as
// it is only used by the debug thread which is explicitly
// stopped when cleaning-up client connection.
//
void DeltaDebugServer::CleanUp (void) {

	CleanUpForClientConnection();

	udelete(dataCreationMutex);
	udelete(clientLinkMutex);
	udelete(buffer);
	udelete(handlers);
	udeleteunlessnull(server);

	DebugExprEvaluator::SingletonDestroy();
	SocketNetLink::CleanUp();
}

////////////////////////////////////////////////////////////

 void DeltaDebugServer::DoInfoStopPointAdapter (
		const char*		source, 
		util_ui32		line, 
		bool			isGlobal,
		const char*		cond
	) {
	insideBreakPoint = true;
	DoInfoStopPoint(source, line, isGlobal, cond);
}

void DeltaDebugServer::DoInfoStopPoint (
		const char*	source, 
		util_ui32	line, 
		bool		isGlobal,
		const char*	cond
	) {
	CLIENT_LINK_LOCK();

	DASSERT(cond);
	util_ui32 size =	3 * SizeUnsignedInt()	+ 
						SizeString(source)		+ 
						SizeString(cond);
	DPTR(buffer)->Prepare(size);
	WriteString(
		WriteSimple(
			WriteUnsignedInt(
				WriteString(
					WriteUnsignedInt(
						DPTR(buffer)->Get(), 
						Debug_InfoStopPoint
					),
					source
				),
				line
			),
			isGlobal
		),
		cond
	);

	DPTR(client)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

void DeltaDebugServer::DoInfoInvalidBreakPoint (
		DebugServerToClientResponse cmmd,
		const char*					source, 
		util_ui32					line, 
		util_ui32					newLine,
		const char*					cond
	) {

	CLIENT_LINK_LOCK();

	DASSERT(cond);

	util_ui32 size =	3 * SizeUnsignedInt()	+ 
						SizeString(source)		+ 
						SizeString(cond);

	DPTR(buffer)->Prepare(size);

	WriteString(
		WriteUnsignedInt(
			WriteUnsignedInt(
				WriteString(
					WriteUnsignedInt(
						DPTR(buffer)->Get(), 
						cmmd
					),
					source
				),
				line
			),
			newLine
		),
		cond
	);

	DPTR(client)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

void DeltaDebugServer::DoInfoValidBreakPoint (
		const char*					source, 
		util_ui32					line, 
		const char*					cond
	) {

	CLIENT_LINK_LOCK();

	DASSERT(cond);

	util_ui32 size =	2 * SizeUnsignedInt()	+ 
						SizeString(source)		+ 
						SizeString(cond);

	DPTR(buffer)->Prepare(size);

	WriteString(
		WriteUnsignedInt(
			WriteString(
				WriteUnsignedInt(
					DPTR(buffer)->Get(), 
					Debug_InfoValidBreakPoint
				),
				source
			),
			line
		),
		cond
	);

	DPTR(client)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

void DeltaDebugServer::DoInfoGlobalContext (void) {
	CLIENT_LINK_LOCK();
	DPTR(client)->SendUnsignedInt(Debug_InfoGlobalContext);
}

////////////////////////////////////////////////////////////

void DeltaDebugServer::DoInfoTopContext (void) {
	CLIENT_LINK_LOCK();
	DPTR(client)->SendUnsignedInt(Debug_InfoTopContext);
}

////////////////////////////////////////////////////////////

void DeltaDebugServer::DoInfoCurrFunction (
		const char*	func,	
		util_ui32	defLine,
		util_ui32	callLine,
		util_ui32	scope,
		const char*	call
	) {

	CLIENT_LINK_LOCK();

	util_ui32 size =	4 * SizeUnsignedInt()	+
						SizeString(func)		+
						SizeString(call);

	DPTR(buffer)->Prepare(size);

	WriteString(
		WriteUnsignedInt(
			WriteUnsignedInt(
				WriteUnsignedInt(
					WriteString(
						WriteUnsignedInt(
							DPTR(buffer)->Get(), 
							Debug_InfoCurrFunction
						),
						func
					),
					defLine
				),
				callLine
			),
			scope
		),
		call
	);

	DPTR(client)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

void DeltaDebugServer::DoResponse (
		DebugServerToClientResponse cmmd,
		const char*					content
	) {

	util_ui32 size = SizeUnsignedInt() + SizeString(content);
	
	DPTR(buffer)->Prepare(size);

	WriteString(
		WriteUnsignedInt(
			DPTR(buffer)->Get(), 
			cmmd
		),
		content
	);

	DPTR(client)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

void DeltaDebugServer::DoInfoValue (const char* content) 
	{ CLIENT_LINK_LOCK(); DoResponse(Debug_InfoValue, content); }

void DeltaDebugServer::DoInfoErrorValue (const char* error) 
	{ CLIENT_LINK_LOCK(); DoResponse(Debug_InfoErrorValue, error); }

void DeltaDebugServer::DoInfoError (const char* error)
	{  CLIENT_LINK_LOCK(); DoResponse(Debug_InfoError, error); }

void DeltaDebugServer::DoInfoBreakPointError (const char* error)
	{  CLIENT_LINK_LOCK(); DoResponse(Debug_InfoBreakPointError, error); }

void DeltaDebugServer::DoInfoDynamicCode (const char* src)
	{  CLIENT_LINK_LOCK(); DoResponse(Debug_InfoDynamicCode, src); }

void DeltaDebugServer::DoInfoWarning (const char* warning)
	{  CLIENT_LINK_LOCK(); DoResponse(Debug_InfoWarning, warning); }

void DeltaDebugServer::DoInfoStop (void) 
	{ CLIENT_LINK_LOCK(); DPTR(client)->SendUnsignedInt(Debug_InfoStop); }

////////////////////////////////////////////////////////////

void DeltaDebugServer::DoInfoBinaryData (void* data, util_ui32 n) {

	 CLIENT_LINK_LOCK(); 

	 util_ui32 size = SizeUnsignedInt() + SizeSimple(n) + n;
	
	DPTR(buffer)->Prepare(size);

	WriteBinaryData(
		WriteUnsignedInt(
			DPTR(buffer)->Get(), 
			Debug_InfoBinaryData
		),
		data, n
	);

	DPTR(client)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

void DeltaDebugServer::DoInvalidBreakPointAdapter(
			const char*	source, 
			util_ui32	line, 
			util_ui32	newLine,
			const char*	cond
		) { 
	DoInfoInvalidBreakPoint(
		Debug_InfoInvalidBreakPoint,
		source,
		line,
		newLine,
		cond
	);
}

void DeltaDebugServer::DoInvalidConditionAdapter(
			const char*	source, 
			util_ui32	line, 
			util_ui32	newLine,
			const char*	cond
		) { 
	DoInfoInvalidBreakPoint(
		Debug_InfoBreakPointConditionError,
		source,
		line,
		newLine,
		cond
	);
}

////////////////////////////////////////////////////////////

void DeltaDebugServer::DoResponse (
		DebugServerToClientResponse								cmmd,
		const std::list< std::pair<std::string, std::string> >& content
	) {

	CLIENT_LINK_LOCK();

	util_ui32 n		= (util_ui32) content.size();
	util_ui32 size	= 2 * SizeUnsignedInt();

	for (	std::list< std::pair<std::string, std::string> >::const_iterator i = content.begin();
			i != content.end();
			size += SizeString(i->first.c_str()) + SizeString(i->second.c_str()), ++i	)
			;

	DPTR(buffer)->Prepare(size);
	const char* p = WriteUnsignedInt(WriteUnsignedInt(DPTR(buffer)->Get(), cmmd), n);

	for (	std::list< std::pair<std::string, std::string> >::const_iterator i = content.begin();
			i != content.end();
			++i	)

			p = WriteString(
					WriteString(
						p, 
						i->first.c_str()
					),
					i->second.c_str()
				);

	DPTR(client)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

void DeltaDebugServer::DoResponse (
		DebugServerToClientResponse							cmmd,
		const std::list< std::pair<std::string, bool> >&	contents
	) {
	
	util_ui32 n		= (util_ui32) contents.size();
	util_ui32 size	= 2 * SizeUnsignedInt();

	for (	std::list< std::pair<std::string, bool> >::const_iterator i = contents.begin();
			i != contents.end();
			++i	)
			size += SizeString(i->first) + SizeSimple(i->second);

	DPTR(buffer)->Prepare(size);
	const char* p = WriteUnsignedInt(WriteUnsignedInt(DPTR(buffer)->Get(), cmmd), n);

	for (	std::list< std::pair<std::string, bool> >::const_iterator i = contents.begin();
			i != contents.end();
			++i		)

			p = WriteSimple(
					WriteString(
						p, 
						i->first
					),
					i->second
				);

	DPTR(client)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

void DeltaDebugServer::DoInfoValueMany (const std::list< std::pair<std::string, bool> >& contents) 
	{ CLIENT_LINK_LOCK(); DoResponse(Debug_InfoValueMany, contents); }

void DeltaDebugServer::DoInfoExprTypeData (const char* content)
	{ CLIENT_LINK_LOCK(); DoResponse(Debug_InfoExprTypeData, content); }

void DeltaDebugServer::DoInfoExprTypeDataMany (const std::list< std::pair<std::string, bool> >& contents)
	{ CLIENT_LINK_LOCK(); DoResponse(Debug_InfoExprTypeDataMany, contents); }

////////////////////////////////////////////////////////////

void DeltaDebugServer::DoInfoObjectGraph (const ObjectGraph& graph) {

	util_ui32 n		=	(util_ui32) graph.size();
	util_ui32 size	=	2 * SizeUnsignedInt()	+	// Size of cmmd and size of graph.
						n * SizeUnsignedInt();		// Size information per node.

	for (ObjectGraph::const_iterator i = graph.begin(); i != graph.end(); ++i) {
		size += SizeString(i->first);	// The object id.
		for (ObjectNode::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
			size +=	SizeString(j->first)	+ 
					SizeSimple(j->second)	+
					SizeString(j->third)	+ 
					SizeSimple(j->fourth)	+
					SizeString(j->fifth);

	}

	DPTR(buffer)->Prepare(size);
	const char* p = WriteUnsignedInt(WriteUnsignedInt(DPTR(buffer)->Get(), Debug_InfoObjectGraph), n);

	for (ObjectGraph::const_iterator i = graph.begin(); i != graph.end(); ++i) {
		
		p = WriteUnsignedInt(
				WriteString(
					p,
					i->first
				),
				(util_ui32) i->second.size()
			);

		for (ObjectNode::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
			p=	WriteString(
					WriteSimple(
						WriteString(
							WriteSimple(
								WriteString(
									p, 
									j->first
								),
								j->second
							),
							j->third
						),
						j->fourth
					),
					j->fifth
				);
	}	
	
	DPTR(client)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

void DeltaDebugServer::DoInfoVariables (
		const std::list< std::pair<std::string, std::string> >& vars
	) { DoResponse(Debug_InfoVariables, vars); }

void DeltaDebugServer::DoInfoAllPossibleCalls (
		const std::list< std::pair<std::string, std::string> >& calls
	) { DoResponse(Debug_InfoAllPossibleCalls, calls); }

void DeltaDebugServer::DoInfoMostRecentFuncResults (
		const std::list< std::pair<std::string, std::string> >& results
	) { DoResponse(Debug_InfoMostRecentFuncResults, results); }

////////////////////////////////////////////////////////////

bool DeltaDebugServer::IsConnectionBroken (void) 
	{ return DPTR(client)->IsConnectionBroken(); }

bool DeltaDebugServer::IsRequestPending (void) 
	{ return (receivedData && receivedSize) || DPTR(client)->IsMsgPending(); }

////////////////////////////////////////////////////////////

bool DeltaDebugServer::ReceiveRequest (void) {

	if (receivedData && receivedSize)
		return true;
	else
	if (!client->IsMsgPending())
		return false;
	else {
		DPTR(client)->ReceiveMsg();
		DPTR(client)->ReadMsg(&receivedData, &receivedSize);

		return true;
	}
}

////////////////////////////////////////////////////////////

DC2SR DeltaDebugServer::GetRequestType (void) {

	if (!receivedData)
		return Debug_RequestInvalid;
	else {
		util_ui32 command;
		ReadUnsignedInt((char*) receivedData, &command);

		return (DC2SR) command;
	}
}

////////////////////////////////////////////////////////////

void DeltaDebugServer::RequestProcessed (void) {

	if (receivedData) {
		DPTR(client)->ClearMsg();
		unullify(receivedData);
		receivedSize = 0;
	}
}

////////////////////////////////////////////////////////////

bool DeltaDebugServer::GetUnsignedInt (DebugClientToServerRequest cmmd, util_ui32* val) {

	if (!receivedData ||  DeltaDebugServer::GetRequestType()!= cmmd)
		return false;

	ReadUnsignedInt(
		ForwardHeader((char*) receivedData), 
		val
	);

	return true;
}

////////////////////////////////////////////////////////////

bool DeltaDebugServer::GetStringUnsignedInt (DebugClientToServerRequest cmmd, char** str, util_ui32* val) {

	if (!receivedData ||  DeltaDebugServer::GetRequestType()!= cmmd)
		return false;

	ReadUnsignedInt(
		ReadString(
			ForwardHeader((char*) receivedData), 
			str
		),
		val
	);

	return true;
}

////////////////////////////////////////////////////////////

bool DeltaDebugServer::GetStringString (DebugClientToServerRequest cmmd, char** str1, char** str2) {

	if (!receivedData ||  DeltaDebugServer::GetRequestType()!= cmmd)
		return false;

	ReadString(
		ReadString(
			ForwardHeader((char*) receivedData), 
			str1
		),
		str2
	);

	return true;
}

////////////////////////////////////////////////////////////

bool DeltaDebugServer::GetStringUnsignedIntString (DebugClientToServerRequest cmmd, char** str1, util_ui32* val, char** str2) {

	if (!receivedData ||  DeltaDebugServer::GetRequestType()!= cmmd)
		return false;

	ReadString(
		ReadUnsignedInt(
			ReadString(
				ForwardHeader((char*) receivedData), 
				str1
			),
			val
		),
		str2
	);

	return true;
}

////////////////////////////////////////////////////////////

bool DeltaDebugServer::GetString (DebugClientToServerRequest cmmd, char** str) {

	if (!receivedData ||  DeltaDebugServer::GetRequestType()!= cmmd)
		return false;

	ReadString(
		ForwardHeader((char*) receivedData), 
		str
	);

	return true;
}

////////////////////////////////////////////////////////////

bool DeltaDebugServer::GetGetGlobal (char** name) 
	{ return GetString(Debug_GetGlobal, name); }

////////////////////////////////////////////////////////////

bool DeltaDebugServer::GetGetExpr (char** expr) 
	{ return GetString(Debug_GetExpr, expr); }

bool DeltaDebugServer::GetGetBinaryData (char** expr)
	{ return GetString(Debug_GetBinaryData, expr); }

bool DeltaDebugServer::GetGetExprMany (std::list<std::string>& exprs) {
	if (!receivedData ||  DeltaDebugServer::GetRequestType()!= Debug_GetExprMany)
		return false;

	ReadStringList(
		ForwardHeader((char*) receivedData), 
		exprs
	);
	return true;
}

////////////////////////////////////////////////////////////

bool DeltaDebugServer::GetGetExprTypeData (char** formatId, char** expr) {

	if (!receivedData || DeltaDebugServer::GetRequestType()!= Debug_GetExprTypeData)
		return false;

	ReadString(
		ReadString(
			ForwardHeader((char*) receivedData), 
			formatId
		),
		expr
	);

	return true;
}

//****************************

bool DeltaDebugServer::GetGetExprTypeDataMany (char** formatId, std::list<std::string>& exprs) {

	if (!receivedData || DeltaDebugServer::GetRequestType()!= Debug_GetExprTypeDataMany)
		return false;

	ReadStringList(
		ReadString(
			ForwardHeader((char*) receivedData), 
			formatId
		),
		exprs
	);

	return true;
}

////////////////////////////////////////////////////////////

bool DeltaDebugServer::GetSetObjectGraphConfiguration (
		bool*	expandLibraryFuncs,
		bool*	expandMethodFuncs,
		bool*	expandProgramFuncs,
		bool*	expandFuncVMs
	) {

	if (!receivedData || DeltaDebugServer::GetRequestType()!= Debug_SetObjectGraphConfiguration)
		return false;

	ReadSimple(
		ReadSimple(
			ReadSimple(
				ReadSimple(
					ForwardHeader((char*) receivedData), 
					expandLibraryFuncs
				),
				expandMethodFuncs
			),
			expandProgramFuncs
		),
		expandFuncVMs
	);
	return true;
}

////////////////////////////////////////////////////////////

bool DeltaDebugServer::GetGetObjectGraph (char** expr, util_ui32* depth, bool* directed) {

	if (!receivedData || DeltaDebugServer::GetRequestType()!= Debug_GetObjectGraph)
		return false;

	ReadSimple(
		ReadUnsignedInt(
			ReadString(
				ForwardHeader((char*) receivedData), 
				expr
			),
			depth
		),
		directed
	);
	return true;
}

////////////////////////////////////////////////////////////

bool DeltaDebugServer::GetGetLocal (char** name) 
	{ return GetString(Debug_GetLocal, name); }

bool DeltaDebugServer::GetGetActual (char** name) 
	{ return GetString(Debug_GetActual, name); }

////////////////////////////////////////////////////////////

bool DeltaDebugServer::GetRunTo (util_ui32* line) 
	{ return GetUnsignedInt(Debug_RunTo, line); }

bool DeltaDebugServer::GetSelectiveStepIn (util_ui32* callOrder) 
	{ return GetUnsignedInt(Debug_SelectiveStepIn, callOrder); }

bool DeltaDebugServer::GetSetToStringMaxLength (util_ui32* maxLen)
	{ return GetUnsignedInt(Debug_SetToStringMaxLength, maxLen); }

bool DeltaDebugServer::GetSetBreakOnThrownException (bool* val) {
	util_ui32 b; 
	if (!GetUnsignedInt(Debug_SetBreakOnThrownException, &b))
		return false;
	else {
		DASSERT(b == 1 || b == 0);
		*val = !!b;
		return true;
	}
}

bool DeltaDebugServer::GetAddBreakPoint (char** source, util_ui32* line, char** cond) 
	{ return GetStringUnsignedIntString(Debug_AddBreakPoint, source, line, cond); }

bool DeltaDebugServer::GetChangeBreakPointCondition (char** source, util_ui32* line, char** cond) 
	{ return GetStringUnsignedIntString(Debug_ChangeBreakPointCondition, source, line, cond); }

bool DeltaDebugServer::GetRemoveBreakPoint (char** source, util_ui32* line) 
	{ return GetStringUnsignedInt(Debug_RemoveBreakPoint, source, line); }

bool DeltaDebugServer::GetEnableBreakPoint (char** source, util_ui32* line) 
	{ return GetStringUnsignedInt(Debug_EnableBreakPoint, source, line); }

bool DeltaDebugServer::GetDisableBreakPoint (char** source, util_ui32* line) 
	{ return GetStringUnsignedInt(Debug_DisableBreakPoint, source, line); }

bool DeltaDebugServer::GetRemoveAllBreakPoints (char** source) 
	{ return GetString(Debug_RemoveAllBreakPoints, source); }

bool DeltaDebugServer::GetEnableAllBreakPoints (char** source) 
	{ return GetString(Debug_EnableAllBreakPoints, source); }

bool DeltaDebugServer::GetDisableAllBreakPoints (char** source) 
	{ return GetString(Debug_DisableAllBreakPoints, source); }

////////////////////////////////////////////////////////////

void DeltaDebugServer::SetExitServeFunction (void (*_exitFunc) (void)) 
	{ exitFunc = _exitFunc; }

bool DeltaDebugServer::IsDebugStopped (void) 
	{ return debugStoppedByClient; }

////////////////////////////////////////////////////////////
// Called only by the debugger command dispatcher function.
// Hence, the CleanUp should never be called indirectly by the notify 
// function as it will lead to a deadlock.
//
void DeltaDebugServer::NotifyExit (void) {
	if (exitFunc)
		(*exitFunc)();
	debugStoppedByClient = true;
	if (DeltaDebugDynamicActivator::GetSingleton().HasLoaded())
		DeltaDebugDynamicActivator::GetSingleton().SetPendingUnload();
}

////////////////////////////////////////////////////////////
