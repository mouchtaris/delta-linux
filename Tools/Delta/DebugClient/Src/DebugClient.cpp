// DebugClient.cpp
// Implementation of the debug client class.
// ScriptFighter Project.
// A. Savidis, February 2002.
//

#include "DebugClient.h"
#include "SocketPacketNetLink.h"
#include "DDebug.h"
#include "DebugBreakPointHolder.h"
#include "DeltaDebugMessageEncoding.h"
#include "DeltaDebugClientBreakPoints.h"
#include "usystem.h"

using namespace DebugMessages;

#define	ENDING_BREAKPOINTS_MESSAGE	"*"

////////////////////////////////////////////////////////////

static Buffer*						buffer			= (Buffer*) 0;
static ClientSocketPacketLink*		clientLink		= (ClientSocketPacketLink*) 0;						
static void*						receivedData	= (void*) 0;
static util_ui32					receivedSize	= 0;

////////////////////////////////////////////////////////////

#define	CHECK_LINK() \
	if (DPTR(clientLink)->IsConnectionBroken()) \
		throw BrokenConnection();

static bool IsValidBufferPosition (void* p) 
	{ return receivedData <= p && p <= POFFS(p, receivedSize); }

static void SendUnsignedInt (
				DebugClientToServerRequest	cmmd, 
				util_ui32					val
			);

static void SendStringUnsignedInt (
				DebugClientToServerRequest	cmmd, 
				const std::string&			str, 
				util_ui32					val
			);

static void SendStringString (
				DebugClientToServerRequest cmmd, 
				const std::string&			str1, 
				const std::string&			str2
			);

static void SendStringUnsignedIntString (
				DebugClientToServerRequest	cmmd, 
				const std::string&			str1, 
				util_ui32					val, 
				const std::string&			str2
			);

static void SendString (
				DebugClientToServerRequest	cmmd, 
				const std::string&			str
			);

////////////////////////////////////////////////////////////

static ServerSocketPacketLink* negotiationLink = (ServerSocketPacketLink*) 0;						

util_ui32 DeltaDebugClient::ServerPortNegotiation::Start (void) {
	SocketNetLink::Initialise();
	unew(negotiationLink);
	return DPTR(negotiationLink)->GetPort();
}

//**************************

#define	NEGOTIATION_TIMEOUT	3000

util_ui32 DeltaDebugClient::ServerPortNegotiation::Finish (void) {
	
	DASSERT(negotiationLink);
	util_ui32				port	= 0;
	SocketPacketNetLink*	client	= DPTR(negotiationLink)->AcceptClient(false, NEGOTIATION_TIMEOUT);

	if (client) {
		if (!DPTR(client)->IsConnectionBroken()	&& DPTR(client)->WaitAnyMessage(NEGOTIATION_TIMEOUT)) {

			void*		data;
			util_ui32	size;

			DPTR(client)->ReadMsg(&data, &size);

			ReadUnsignedInt((char*) data, &port);
			DPTR(client)->ClearMsg();
		}

		udelete(client);
	}

	udelete(negotiationLink);
	SocketNetLink::CleanUp();
	return port;
}

//**************************

const std::string DeltaDebugClient::ServerPortNegotiation::ToArg (util_ui32 port) {
	return	std::string(" ")						+
			DEBUG_CLIENT_NEGOTIATIONPORT_CMMDARG	+ 
			" "										+ 
			unum2string(port);
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::Initialise (void) {
	SocketNetLink::Initialise();
	unew(buffer);
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::CleanUp (void) {

	udelete(clientLink);
	udelete(buffer);
	unullify(receivedData);
	receivedSize = 0;
	SocketNetLink::CleanUp();
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::PostAllBreakoints (void) { // Sync.
	
	for (util_ui16 i = 0; i < DeltaDebugClientBreakPoints::Get().Total(); ++i) {

		const std::string& source = DeltaDebugClientBreakPoints::Get().GetSource(i);
		std::list<DeltaBreakPoint> bpts = DeltaDebugClientBreakPoints::Get().Get(source);

		for (	std::list<DeltaBreakPoint>::iterator i = bpts.begin();
				i != bpts.end();
				++i		) {

				DoAddBreakPoint(
					source.c_str(), 
					i->GetLine(), 
					i->HasCondition() ? i->GetCondition() : ""
				);

				if (i->IsEnabled())
					DoEnableBreakPoint(source.c_str(), i->GetLine());
				else
					DoDisableBreakPoint(source.c_str(), i->GetLine());
		}
	}

	DoAddBreakPoint(ENDING_BREAKPOINTS_MESSAGE, 0); // List termination message.
}

////////////////////////////////////////////////////////////

#define	PRODUCT_CONNECT_TIMEOUT	5000

bool DeltaDebugClient::Connect (const std::string& host, util_ui32 port) {

	unew(clientLink);
	DPTR(clientLink)->ConnectWithServer(host.c_str(), port, PRODUCT_CONNECT_TIMEOUT);
	if (DPTR(clientLink)->IsConnectionBroken())
		return false;
	else {
		PostAllBreakoints();
		return true;
	}
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::DoGo (void) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	DPTR(clientLink)->SendUnsignedInt(Debug_Go);
}

//****************************

void DeltaDebugClient::DoStepOver (void) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	DPTR(clientLink)->SendUnsignedInt(Debug_StepOver);
}

//****************************

void DeltaDebugClient::DoStepIn (void) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	DPTR(clientLink)->SendUnsignedInt(Debug_StepIn);
}

void DeltaDebugClient::DoSelectiveStepIn (util_ui32 callOrder) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	SendUnsignedInt(Debug_SelectiveStepIn, callOrder);
}

//****************************

void DeltaDebugClient::DoStepOut (void) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	DPTR(clientLink)->SendUnsignedInt(Debug_StepOut);
}

//****************************

void DeltaDebugClient::DoStart (void) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	DPTR(clientLink)->SendUnsignedInt(Debug_Start);
}

void DeltaDebugClient::DoStop (void) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	DPTR(clientLink)->SendUnsignedInt(Debug_Stop);
}

//****************************

void DeltaDebugClient::DoBreakExecution (void) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	DPTR(clientLink)->SendUnsignedInt(Debug_BreakExecution);
}

//****************************

void DeltaDebugClient::DoStackUp (void) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	DPTR(clientLink)->SendUnsignedInt(Debug_StackUp);
}

void DeltaDebugClient::DoMultipleStackUp (util_ui32 n) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	SendUnsignedInt(Debug_MultipleStackUp, n);
}

//****************************

void DeltaDebugClient::DoStackDown (void) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	DPTR(clientLink)->SendUnsignedInt(Debug_StackDown);
}

void DeltaDebugClient::DoMultipleStackDown (util_ui32 n) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	SendUnsignedInt(Debug_MultipleStackDown, n);
}

//****************************

void DeltaDebugClient::DoGetVariables (void) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	DPTR(clientLink)->SendUnsignedInt(Debug_GetVariables);
}

//****************************

void DeltaDebugClient::DoGetDynamicCode (void) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	DPTR(clientLink)->SendUnsignedInt(Debug_GetDynamicCode);
}

//****************************

void DeltaDebugClient::DoGetAllPossibleCalls (void) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	DPTR(clientLink)->SendUnsignedInt(Debug_GetAllPossibleCalls);
}

////////////////////////////////////////////////////////////

static void SendUnsignedInt (DebugClientToServerRequest cmmd, util_ui32 val) {

	util_ui32 size = 2 * SizeUnsignedInt();
	DPTR(buffer)->Prepare(size);

	WriteUnsignedInt(
		WriteUnsignedInt(DPTR(buffer)->Get(), cmmd),
		val
	);

	DPTR(clientLink)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

static void SendStringUnsignedInt (DebugClientToServerRequest cmmd, const std::string& str, util_ui32 val) {

	util_ui32 size = 2 * SizeUnsignedInt() + SizeString(str);
	DPTR(buffer)->Prepare(size);

	WriteUnsignedInt(
		WriteString(
			WriteUnsignedInt(
				DPTR(buffer)->Get(), 
				cmmd
			),
			str
		),
		val
	);

	DPTR(clientLink)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

static void SendStringString (DebugClientToServerRequest cmmd, const std::string& str1, const std::string& str2) {

	util_ui32 size = SizeUnsignedInt() + SizeString(str1) + SizeString(str2);
	DPTR(buffer)->Prepare(size);

	WriteString(
		WriteString(
			WriteUnsignedInt(
				DPTR(buffer)->Get(), 
				cmmd
			),
			str1
		),
		str2
	);

	DPTR(clientLink)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

static void SendStringUnsignedIntString (
		DebugClientToServerRequest	cmmd, 
		const std::string&			str1, 
		util_ui32					val, 
		const std::string&			str2
	) {

	util_ui32 size =	2 * SizeUnsignedInt() +
						SizeString(str1) +
						SizeString(str2);
	DPTR(buffer)->Prepare(size);

	WriteString(
		WriteUnsignedInt(
			WriteString(
				WriteUnsignedInt(
					DPTR(buffer)->Get(), 
					cmmd
				),
				str1
			),
			val
		),
		str2
	);

	DPTR(clientLink)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

static void SendString (DebugClientToServerRequest cmmd, const std::string& str) {

	util_ui32 size = SizeUnsignedInt() + SizeString(str);
	DPTR(buffer)->Prepare(size);

	WriteString(
		WriteUnsignedInt(
			DPTR(buffer)->Get(), 
			cmmd
		),
		str
	);

	DPTR(clientLink)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::DoGetGlobal (const std::string& name) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	SendString(Debug_GetGlobal, name);
}

////////////////////////////////////////////////////////////
// Notice that with 0xffff scope we require information without scope information.
//
void DeltaDebugClient::DoGetLocal (const std::string& name) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	SendString(Debug_GetLocal, name);
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::DoGetExpr (const std::string& expr) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	SendString(Debug_GetExpr, expr);
}

//****************************

void DeltaDebugClient::DoGetBinaryData (const std::string& expr) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	SendString(Debug_GetBinaryData, expr);
}

//****************************

void DeltaDebugClient::DoGetExprMany (const std::list<std::string>& exprs) {

	util_ui32 size = SizeUnsignedInt() + SizeStringList(exprs);
	DPTR(buffer)->Prepare(size);

	WriteStringList(
		WriteUnsignedInt(
			DPTR(buffer)->Get(), 
			Debug_GetExprMany
		),
		exprs
	);

	DPTR(clientLink)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::DoGetExprTypeData (const std::string& formatId, const std::string& expr) {

	util_ui32 size =	SizeUnsignedInt() + 
						SizeString(formatId) + 
						SizeString(expr);

	DPTR(buffer)->Prepare(size);

	WriteString(
		WriteString(
			WriteUnsignedInt(
				DPTR(buffer)->Get(), 
				Debug_GetExprTypeData
			),
			formatId
		),
		expr
	);

	DPTR(clientLink)->SendMsg(DPTR(buffer)->Get(), size);
}

//****************************

void DeltaDebugClient::DoGetExprTypeDataMany (const std::string& formatId, const std::list<std::string>& exprs) {

	util_ui32 size =	SizeUnsignedInt() + 
						SizeString(formatId) + 
						SizeStringList(exprs);

	DPTR(buffer)->Prepare(size);

	WriteStringList(
		WriteString(
			WriteUnsignedInt(
				DPTR(buffer)->Get(), 
				Debug_GetExprTypeDataMany
			),
			formatId
		),
		exprs
	);

	DPTR(clientLink)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::DoSetObjectGraphConfiguration (
		bool		expandLibraryFuncs,
		bool		expandMethodFuncs,
		bool		expandProgramFuncs,
		bool		expandFuncVMs
	) {
	util_ui32 size =	SizeUnsignedInt()	+  
						SizeSimple(expandLibraryFuncs)	+ 
						SizeSimple(expandMethodFuncs)	+
						SizeSimple(expandProgramFuncs)	+
						SizeSimple(expandFuncVMs);

	DPTR(buffer)->Prepare(size);

	WriteSimple(
		WriteSimple(
			WriteSimple(
				WriteSimple(
					WriteUnsignedInt(
						DPTR(buffer)->Get(), 
						Debug_SetObjectGraphConfiguration
					),
					expandLibraryFuncs
				),
				expandMethodFuncs
			),
			expandProgramFuncs
		),
		expandFuncVMs
	);

	DPTR(clientLink)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::DoGetObjectGraph (const std::string& expr, util_ui32 depth, bool directed) {

	util_ui32 size =	SizeUnsignedInt()	+ 
						SizeString(expr)	+ 
						SizeUnsignedInt()	+
						SizeSimple(directed);

	DPTR(buffer)->Prepare(size);

	WriteSimple(
		WriteUnsignedInt(
			WriteString(
				WriteUnsignedInt(
					DPTR(buffer)->Get(), 
					Debug_GetObjectGraph
				),
				expr
			),
			depth
		),
		directed
	);

	DPTR(clientLink)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::DoGetActual (const std::string& name) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	SendString(Debug_GetActual, name);
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::DoRunTo (util_ui32 line) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	SendUnsignedInt(Debug_RunTo, line);
}

void DeltaDebugClient::DoSetToStringMaxLength (util_ui32 maxLen) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	SendUnsignedInt(Debug_SetToStringMaxLength, maxLen);
}

void DeltaDebugClient::DoSetBreakOnThrownException (bool val) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	SendUnsignedInt(Debug_SetBreakOnThrownException, val ? 1 : 0);
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::DoAddBreakPoint (const std::string& source, util_ui32 line, const std::string& cond) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	SendStringUnsignedIntString(Debug_AddBreakPoint, source, line, cond);
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::DoChangeBreakPointCondition (const std::string& source, util_ui32 line, const std::string& cond) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	SendStringUnsignedIntString(Debug_ChangeBreakPointCondition, source, line, cond);
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::DoRemoveBreakPoint (const std::string& source, util_ui32 line) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	SendStringUnsignedInt(Debug_RemoveBreakPoint, source, line);
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::DoEnableBreakPoint (const std::string& source, util_ui32 line) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	SendStringUnsignedInt(Debug_EnableBreakPoint, source, line);
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::DoDisableBreakPoint (const std::string& source, util_ui32 line) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	SendStringUnsignedInt(Debug_DisableBreakPoint, source, line);
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::DoEnableAllBreakPoints (const std::string& source) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	SendString(Debug_EnableAllBreakPoints, source);
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::DoDisableAllBreakPoints (const std::string& source) {
	CHECK_LINK();
	SendString(Debug_DisableAllBreakPoints, source);
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::DoRemoveAllBreakPoints (const std::string& source) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	SendString(Debug_RemoveAllBreakPoints, source);
}

////////////////////////////////////////////////////////////

bool DeltaDebugClient::IsResponsePending (void) {
	CHECK_LINK();
	DASSERT(!IsConnectionBroken());
	return	(receivedData && receivedSize) ||
			DPTR(clientLink)->IsMsgPending();
}

////////////////////////////////////////////////////////////
// Clears previous response data.
//
void DeltaDebugClient::ResponseProcessed (void) {

	DASSERT(receivedData);
	DPTR(clientLink)->ClearMsg();
	receivedData = (void*) 0;
	receivedSize = 0;
}

////////////////////////////////////////////////////////////

bool DeltaDebugClient::ReceiveResponse (void) {
	CHECK_LINK();
	if (receivedData && receivedSize)
		return true;
	else
	if (!DPTR(clientLink)->IsMsgPending())
		return false;
	else {
		
		DPTR(clientLink)->ReceiveMsg();
		DPTR(clientLink)->ReadMsg(&receivedData, &receivedSize);
		DASSERT(receivedSize);
		return true;
	}
}

////////////////////////////////////////////////////////////

DS2CR DeltaDebugClient::GetResponseType (void) {

	CHECK_LINK();

	if (!receivedData)
		return Debug_ResponseInvalid;
	else {
		util_ui32 command;
		ReadUnsignedInt((char*) receivedData, &command);

		return (DS2CR) command;
	}
}

////////////////////////////////////////////////////////////

bool DeltaDebugClient::GetInfoStopPoint (
		std::string*	source,		
		util_ui32*		line, 
		bool*			isGlobal,
		std::string*	cond
	) {

	CHECK_LINK();

	if (!receivedData || GetResponseType()!= Debug_InfoStopPoint)
		return false;

	char* _source;
	char* _cond;

	ReadString(
		ReadSimple(
			ReadUnsignedInt(
				ReadString(
					ForwardHeader((char*) receivedData), 
					&_source
				),
				line
			),
			isGlobal
		),
		&_cond
	);

	*source	= _source;
	*cond	= _cond;
	return true;
}

////////////////////////////////////////////////////////////

bool DeltaDebugClient::GetInfoInvalidBreakPoint (
		std::string*	source,	
		util_ui32*		line,	
		util_ui32*		newLine,
		std::string*	cond
	) {

	CHECK_LINK();

	if (!receivedData || 
		(GetResponseType()!= Debug_InfoInvalidBreakPoint &&
		 GetResponseType()!= Debug_InfoBreakPointConditionError))
		return false;

	char* _source;
	char* _cond;

	ReadString(
		ReadUnsignedInt(
			ReadUnsignedInt(
				ReadString(
					ForwardHeader((char*) receivedData), 
					&_source
				),
				line
			),
			newLine
		),
		&_cond
	);

	*source	= _source;
	*cond	= _cond;
	return true;
}
////////////////////////////////////////////////////////////
// newLine == 0						=> The line was invalid but failed to reposition to a new line.
// newLine != 0 && newLine != line	=> The line was invalid, but succeeded to reposition to a new line.
// newLine == line					=> The error concerned the evaluation of the condition.

DeltaDebugClient::InvalidBreakPointReason DeltaDebugClient::GetInvalidBreakPointReason (util_ui32 line, util_ui32 newLine) {
	DASSERT(line);
	return	!newLine ?			LineWasInvalidButFailedToReposition			:
			newLine != line ?	LineWasInvalidButSuccceededToReposition		:
			ConditionWasInvalid;	// newLine == line
}

////////////////////////////////////////////////////////////

bool DeltaDebugClient::GetInfoValidBreakPoint (
		std::string*	source,	
		util_ui32*		line,
		std::string*	cond
	) {

	CHECK_LINK();

	if (!receivedData || GetResponseType() != Debug_InfoValidBreakPoint)
		return false;

	char* _source;
	char* _cond;

	ReadString(
		ReadUnsignedInt(
			ReadString(
				ForwardHeader((char*) receivedData), 
				&_source
			),
			line
		),
		&_cond
	);

	*source	= _source;
	*cond	= _cond;
	return true;
}

////////////////////////////////////////////////////////////

bool DeltaDebugClient::GetInfoCurrFunction (
		std::string*	func,	
		util_ui32*		defLine,	
		util_ui32*		callLine,
		util_ui32*		scope,
		std::string*	call
	) {

	CHECK_LINK();

	if (!receivedData || GetResponseType()!= Debug_InfoCurrFunction)
		return false;
	
	char* _func;
	char* _call;

	ReadString(
		ReadUnsignedInt(
			ReadUnsignedInt(
				ReadUnsignedInt(
					ReadString(
						ForwardHeader((char*) receivedData), 
						&_func
					),
					defLine
				),
				callLine
			),
			scope
		),
		&_call
	);

	*func = _func;
	*call = _call;
	return true;
}
					
////////////////////////////////////////////////////////////

bool DeltaDebugClient::GetResponse (
		DebugServerToClientResponse cmmd,
		std::string*				content
	) {

	CHECK_LINK();

	if (!receivedData || GetResponseType() != cmmd)
		return false;
	
	char* _content;
	ReadString(
		ForwardHeader((char*) receivedData), 
		&_content
	);

	*content = _content;
	return true;
}

////////////////////////////////////////////////////////////

bool DeltaDebugClient::GetResponse (
		DebugServerToClientResponse cmmd,
		std::list< std::pair<std::string, bool> >& contents
	) {
	
	CHECK_LINK();

	if (!receivedData || GetResponseType()!= cmmd)
		return false;

	util_ui32 n;
	void* p = ReadUnsignedInt(ForwardHeader((char*) receivedData), &n);
	DASSERT(IsValidBufferPosition(p));

	while (n--) {

		char* s;
		bool  b;

		p = ReadString((char*) p, &s);
		DASSERT(IsValidBufferPosition(p));

		p = ReadSimple((char*) p, &b);
		DASSERT(IsValidBufferPosition(p));

		contents.push_back(
			std::pair<std::string, bool>(s, b)
		);
	}

	return true;
}

////////////////////////////////////////////////////////////

bool DeltaDebugClient::GetResponse (
		DebugServerToClientResponse							cmmd,
		std::list< std::pair<std::string, std::string> >&	contents
	) {

	CHECK_LINK();

	if (!receivedData || GetResponseType()!= cmmd)
		return false;

	util_ui32 n;
	char* p = ReadUnsignedInt(ForwardHeader((char*) receivedData), &n);
	DASSERT(IsValidBufferPosition(p));

	while (n--) {

		char* s1;
		char* s2;

		p = ReadString((char*) p, &s1);
		DASSERT(IsValidBufferPosition(p));

		p = ReadString((char*) p, &s2);
		DASSERT(IsValidBufferPosition(p));

		contents.push_back(
			std::pair<std::string, std::string>(s1, s2)
		);
	}

	return true;
}

////////////////////////////////////////////////////////////

bool DeltaDebugClient::GetInfoValue (std::string* content) 
	{ return GetResponse(Debug_InfoValue, content); }

////////////////////////////////////////////////////////////

bool DeltaDebugClient::GetInfoBinaryData (void** data, util_ui32* n) {

	CHECK_LINK();
	if (!receivedData || GetResponseType()!= Debug_InfoBinaryData)
		return false;

	char* p = ForwardHeader((char*) receivedData);
	DASSERT(IsValidBufferPosition(p));

	p = ReadSimple(p,n);
	DASSERT(IsValidBufferPosition(p));

	if (*n)
		memcpy(*data = malloc(*n), p, *n);
	else
		unullify(*data);
	return true;
}

//******************************

void DeltaDebugClient::ClearBinaryData (void** data, util_ui32* n) {
	DASSERT(*data);
	free(*data);
	unullify(*data);
	*n = 0;
}

////////////////////////////////////////////////////////////

bool DeltaDebugClient::GetInfoDynamicCode (std::string* source)
	{ return GetResponse(Debug_InfoDynamicCode, source); }

const std::string DeltaDebugClient::GetDynamicCodeVirtualPath (const std::string& vmId)
	{ return std::string(DBG_DYNAMIC_SOURCE) + vmId; }

bool DeltaDebugClient::GetInfoValueMany (std::list< std::pair<std::string, bool> >& contents) 
	{ return GetResponse(Debug_InfoValueMany, contents); }
	
bool DeltaDebugClient::GetInfoExprTypeData (std::string* content)
	{ return GetResponse(Debug_InfoExprTypeData, content); }

bool DeltaDebugClient::GetInfoExprTypeDataMany (std::list< std::pair<std::string, bool> >& contents) 
	{  return GetResponse(Debug_InfoExprTypeDataMany, contents); }

//******************************

bool DeltaDebugClient::GetInfoObjectGraph (ObjectGraph& graph) {

	CHECK_LINK();

	if (!receivedData || GetResponseType()!= Debug_InfoObjectGraph)
		return false;

	graph.clear();

	util_ui32 n;
	char* p = ReadUnsignedInt(ForwardHeader((char*) receivedData), &n);	// Read number of nodes.
	DASSERT(IsValidBufferPosition(p));
	
	char* s;
	bool  b;

	while (n--) {

		p = ReadString(p, &s);				// Read object id.
		DASSERT(IsValidBufferPosition(p));
		ObjectNode& node = graph[s];		// Make a new node.

		util_ui32 k;
		p = ReadUnsignedInt(p, &k);			// Read number of fields.
		DASSERT(IsValidBufferPosition(p));
	
		while (k--) {

			p = ReadString(p, &s);
			DASSERT(IsValidBufferPosition(p));

			p = ReadSimple(p, &b);
			DASSERT(IsValidBufferPosition(p));
			
			char* s2;
			bool  b2;

			p = ReadString(p, &s2);
			DASSERT(IsValidBufferPosition(p));

			p = ReadSimple(p, &b2);
			DASSERT(IsValidBufferPosition(p));

			char* kt;
			p = ReadString(p, &kt);
			DASSERT(IsValidBufferPosition(p));

			node.push_back(ObjectSlot(s, b, s2, b2, kt));
		}
	}
	
	return true;
}

//******************************

bool DeltaDebugClient::GetInfoErrorValue (std::string* error) 
	{ return GetResponse(Debug_InfoErrorValue, error); }

bool DeltaDebugClient::GetInfoError (std::string* error) 
	{ return GetResponse(Debug_InfoError, error); }

bool DeltaDebugClient::GetInfoBreakPointError (std::string* error) 
	{ return GetResponse(Debug_InfoBreakPointError, error); }

bool DeltaDebugClient::GetInfoWarning (std::string* warning) 
	{ return GetResponse(Debug_InfoWarning, warning); }

bool DeltaDebugClient::GetInfoVariables (std::list< std::pair<std::string, std::string> >& vars)
	{ return GetResponse(Debug_InfoVariables,vars); }

bool DeltaDebugClient::GetInfoAllPossibleCalls (std::list< std::pair<std::string, std::string> >& calls)
	{ return GetResponse(Debug_InfoAllPossibleCalls, calls); }

bool DeltaDebugClient::GetMostRecentFuncResults (std::list< std::pair<std::string, std::string> >& results)
	{ return GetResponse(Debug_InfoMostRecentFuncResults, results); }

////////////////////////////////////////////////////////////

void DeltaDebugClient::ConsumeUntilReceive (DebugServerToClientResponse m) {

	while (1)
		if (IsResponsePending()) {
			ReceiveResponse();
			if (m == GetResponseType()) {
				ResponseProcessed();
				return;
			}
			ResponseProcessed();
		}
}

////////////////////////////////////////////////////////////

void DeltaDebugClient::WaitAnyMessage (void) {

	while (!IsResponsePending())
		;
	ReceiveResponse();
}

////////////////////////////////////////////////////////////

bool DeltaDebugClient::IsConnectionBroken (void) {
	return !clientLink || DPTR(clientLink)->IsConnectionBroken();
}

////////////////////////////////////////////////////////////
