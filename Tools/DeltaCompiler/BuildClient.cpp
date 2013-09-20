// BuildClient.cpp
// Client for remote communication with the Build server.
// Y.Lilis, February 2012.
//

#include "BuildClient.h"
#include "SocketPacketNetLink.h"
#include "DDebug.h"
#include "usystem.h"

////////////////////////////////////////////////////////////

const char* WriteString (const char* buffer, const std::string& str) {
	util_ui32 len = (util_ui32) str.length() + 1;
	memcpy((void*) buffer, (void*) str.c_str(), len);
	return buffer + len;
}

char* ReadString (char* buffer, char** str) {	// String read is part of buffer.
	*str = buffer;
	return buffer + strlen(*str) + 1;
}

util_ui32 SizeString (const std::string& str) 
	{ return (util_ui32) str.length() + 1; }

//*****************************

char* ForwardHeader (char* buffer)
	{ return buffer + ubinaryio::SizeInteger<util_ui32>(); }

//*****************************

const char*	WriteUnsignedInt (const char* buffer, util_ui32 val)
	{ return (char*) ubinaryio::WriteInteger((void*) const_cast<char*>(buffer), val); }

char* ReadUnsignedInt (char* buffer, util_ui32* val) {
	ubinaryio::ReadInteger(buffer, val);
	return (char*) POFFS(buffer, ubinaryio::SizeInteger<util_ui32>());
}

util_ui32 SizeUnsignedInt (void) 
	{ return ubinaryio::SizeInteger<util_ui32>(); }

//*****************************

util_ui32 SizeStringList (const std::list<std::string>& strs) {
	util_ui32 size = ubinaryio::SizeInteger<util_ui32>();
	for (std::list<std::string>::const_iterator i = strs.begin(); i != strs.end(); ++i)
		size += SizeString(*i);
	return size;
}

const char* WriteStringList (const char* buffer, const std::list<std::string>& strs) {
	const char* buf = WriteUnsignedInt(buffer, (util_ui32) strs.size());	//list size
	for (std::list<std::string>::const_iterator i = strs.begin(); i != strs.end(); ++i)
		buf = WriteString(buf, *i);	//strs
	return buf;
}

char* ReadStringList (char* buffer, std::list<std::string>* strs) {
	util_ui32 size;
	char* buf = ReadUnsignedInt(buffer, &size);
	for (util_ui32 i = 0; i < size; ++i) {
		char* str;
		buf = ReadString(buf, &str);
		strs->push_back(std::string(str));
	}
	return buf;
}

//*****************************

util_ui32 SizeLineMappings (const std::map< util_ui32, std::set<util_ui32> >& lineMappings) {
	const util_ui32 uintSize = ubinaryio::SizeInteger<util_ui32>();
	util_ui32 size = uintSize;
	for (std::map< util_ui32, std::set<util_ui32> >::const_iterator i = lineMappings.begin(); i != lineMappings.end(); ++i) {
		size += 2 * uintSize;
		for (std::set<util_ui32>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
			size += uintSize;
	}
	return size;
}

const char* WriteLineMappings (const char* buffer, const std::map< util_ui32, std::set<util_ui32> >& lineMappings) {
	const char* buf = WriteUnsignedInt(buffer, (util_ui32) lineMappings.size());	//map size
	for (std::map< util_ui32, std::set<util_ui32> >::const_iterator i = lineMappings.begin(); i != lineMappings.end(); ++i) {
		buf = WriteUnsignedInt(buf, (util_ui32) i->first);	//initial line
		buf = WriteUnsignedInt(buf, (util_ui32) i->second.size());	//set size
		for (std::set<util_ui32>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
			buf = WriteUnsignedInt(buf, (util_ui32) *j);	//mapped lines
	}
	return buf;
}

char* ReadLineMappings(char* buffer, std::map< util_ui32, std::set<util_ui32> >* lineMappings) {
	util_ui32 size;
	char* buf = ReadUnsignedInt(buffer, &size);
	for (util_ui32 i = 0; i < size; ++i) {
		util_ui32 line;
		buf = ReadUnsignedInt(buf, &line);
		(*lineMappings)[line];	//insert line in the map

		util_ui32 setSize;
		buf = ReadUnsignedInt(buf, &setSize);
		for (util_ui32 i = 0; i < setSize; ++i) {
			util_ui32 mappedLine;
			buf = ReadUnsignedInt(buf, &mappedLine);
			(*lineMappings)[line].insert(mappedLine);
		}
	}
	return buf;
}

//*****************************

util_ui32 SizeNodeToChainOfSourceLineOriginInfo (const AST::NodeToChainOfSourceLineOriginInfo& info) {
	const util_ui32 uintSize = ubinaryio::SizeInteger<util_ui32>();
	util_ui32 size = uintSize;
	for (AST::NodeToChainOfSourceLineOriginInfo::const_iterator i = info.begin(); i != info.end(); ++i) {
		size += 2 * uintSize;
		for (AST::ChainOfSourceLineOriginInfo::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
			size += SizeString(j->symbolicURI) + uintSize;
	}
	return size;
}

const char* WriteNodeToChainOfSourceLineOriginInfo (const char* buffer, const AST::NodeToChainOfSourceLineOriginInfo& info) {
	const char* buf = WriteUnsignedInt(buffer, (util_ui32) info.size());	//map size
	for (AST::NodeToChainOfSourceLineOriginInfo::const_iterator i = info.begin(); i != info.end(); ++i) {
		buf = WriteUnsignedInt(buf, (util_ui32) i->first);			//serial
		buf = WriteUnsignedInt(buf, (util_ui32) i->second.size());	//list size
		for (AST::ChainOfSourceLineOriginInfo::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
			buf = WriteString(buf, j->symbolicURI);					//source
			buf = WriteUnsignedInt(buf, j->line);					//line
		}
	}
	return buf;
}

char* ReadNodeToChainOfSourceLineOriginInfo (char* buffer, AST::NodeToChainOfSourceLineOriginInfo* info) {
	util_ui32 size;
	char* buf = ReadUnsignedInt(buffer, &size);
	for (util_ui32 i = 0; i < size; ++i) {
		util_ui32 serial;
		buf = ReadUnsignedInt(buf, &serial);
		(*info)[serial];	//insert serial in the map

		util_ui32 listSize;
		buf = ReadUnsignedInt(buf, &listSize);
		for (util_ui32 i = 0; i < listSize; ++i) {
			char* source;
			util_ui32 line;
			buf = ReadUnsignedInt(ReadString(buf, &source), &line);
			(*info)[serial].push_back(AST::SourceLineOriginInfo(std::string(source), line));
		}
	}
	return buf;
}

////////////////////////////////////////////////////////////

class Buffer {
	private:
	char*		buffer;
	util_ui32	size;

	public:
	char* Get (void) { return buffer; }
	
	void Prepare (util_ui32 requestedSize) {
		if (size < requestedSize) {
			if (buffer)
				DDELARR(buffer);
			size = requestedSize;
			buffer = DNEWARR(char, size);
		}
		else
			DASSERT(buffer);
	}

	void Clear (void) {
		if (buffer) 
			DDELARR(buffer); 
		unullify(buffer); 
	}

	Buffer (void) : buffer((char*) 0), size(0){}
	~Buffer() {  Clear(); }
};

////////////////////////////////////////////////////////////

static Buffer*						buffer			= (Buffer*) 0;
static ClientSocketPacketLink*		clientLink		= (ClientSocketPacketLink*) 0;						
static void*						receivedData	= (void*) 0;
static util_ui32					receivedSize	= 0;

////////////////////////////////////////////////////////////

void BuildClient::Initialise (void) {
	SocketNetLink::Initialise();
	unew(buffer);
}

////////////////////////////////////////////////////////////

void BuildClient::CleanUp (void) {

	udeleteunlessnull(clientLink);
	udelete(buffer);
	unullify(receivedData);
	receivedSize = 0;
	SocketNetLink::CleanUp();
}

////////////////////////////////////////////////////////////

#define	PRODUCT_CONNECT_TIMEOUT	5000

bool BuildClient::Connect (const std::string& host, util_ui32 port) {
	udeleteunlessnull(clientLink);
	unew(clientLink);
	DPTR(clientLink)->ConnectWithServer(host.c_str(), port, PRODUCT_CONNECT_TIMEOUT);
	return !DPTR(clientLink)->IsConnectionBroken();
}

////////////////////////////////////////////////////////////

bool BuildClient::IsResponsePending (void) {
	DASSERT(!IsConnectionBroken());
	return	(receivedData && receivedSize) ||
			DPTR(clientLink)->IsMsgPending();
}

////////////////////////////////////////////////////////////
// Clears previous response data.
//
void BuildClient::ResponseProcessed (void) {

	DASSERT(receivedData);
	DPTR(clientLink)->ClearMsg();
	receivedData = (void*) 0;
	receivedSize = 0;
}

////////////////////////////////////////////////////////////

bool BuildClient::ReceiveResponse (void) {
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

ServerToClientResponse BuildClient::GetResponseType (void) {
	if (!receivedData)
		return MetaBuildSystem_ResponseInvalid;
	else {
		util_ui32 command;
		ReadUnsignedInt((char*) receivedData, &command);

		return (ServerToClientResponse) command;
	}
}

////////////////////////////////////////////////////////////

void BuildClient::WaitAnyMessage (void) {

	while (!IsConnectionBroken() && !IsResponsePending())
		uprocesssleep(100);
	if (!IsConnectionBroken())
		ReceiveResponse();
}

////////////////////////////////////////////////////////////

bool BuildClient::IsConnectionBroken (void) {
	return !clientLink || DPTR(clientLink)->IsConnectionBroken();
}

////////////////////////////////////////////////////////////

void BuildClient::SendString (ClientToServerRequest request, const std::string& str) {
	util_ui32 size = SizeUnsignedInt() + SizeString(str);
	DPTR(buffer)->Prepare(size);

	WriteString(
		WriteUnsignedInt(
			DPTR(buffer)->Get(), 
			request
		),
		str
	);

	DPTR(clientLink)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

void BuildClient::DoAttachSource (
	const std::string&								primarySymbolicURI,
	const std::string&								symbolicURI,							
	const LineMappings&								lineMappings,
	const AST::NodeToChainOfSourceLineOriginInfo&	info,
	const std::string&								classId,
	bool											isResultFinal	// applicable only for result sources
) {
	util_ui32 size =	2 * SizeUnsignedInt()						+
						SizeString(primarySymbolicURI)				+
						SizeString(symbolicURI)						+
						SizeLineMappings(lineMappings)				+
						SizeNodeToChainOfSourceLineOriginInfo(info) +
						SizeString(classId);
	DPTR(buffer)->Prepare(size);

	WriteUnsignedInt(
		WriteString(
			WriteNodeToChainOfSourceLineOriginInfo(
				WriteLineMappings(
					WriteString(
						WriteString(
							WriteUnsignedInt(
								DPTR(buffer)->Get(),
								MetaBuildSystem_AttachStageSource
							),
							primarySymbolicURI
						),
						symbolicURI
					),
					lineMappings
				),
				info
			),
			classId
		),
		isResultFinal ? 1 : 0
	);

	DPTR(clientLink)->SendMsg(DPTR(buffer)->Get(), size);
}

////////////////////////////////////////////////////////////

void BuildClient::DoAttachStageSource (
	const std::string&								primarySymbolicURI,
	const std::string&								sourceText,							
	const LineMappings&								lineMappings,
	const AST::NodeToChainOfSourceLineOriginInfo&	info
) { DoAttachSource(primarySymbolicURI, sourceText, lineMappings, info, STAGE_SOURCE_CLASS_ID); }

////////////////////////////////////////////////////////////

void BuildClient::DoAttachStageResult (
	const std::string&								primarySymbolicURI,
	const std::string&								sourceText,							
	const LineMappings&								lineMappings,
	const AST::NodeToChainOfSourceLineOriginInfo&	info,
	bool											isFinal
) { DoAttachSource(primarySymbolicURI, sourceText, lineMappings, info, STAGE_RESULT_CLASS_ID, isFinal); }

////////////////////////////////////////////////////////////

void BuildClient::DoAttachAspectResult (
	const std::string&								primarySymbolicURI,
	const std::string&								sourceText,							
	const LineMappings&								lineMappings,
	const AST::NodeToChainOfSourceLineOriginInfo&	info,
	bool											isFinal
) { DoAttachSource(primarySymbolicURI, sourceText, lineMappings, info, ASPECT_RESULT_CLASS_ID, isFinal); }

////////////////////////////////////////////////////////////

void BuildClient::DoBuildStageSource (const std::string& symbolicURI)
	{ SendString(MetaBuildSystem_BuildStageSource, symbolicURI); }

////////////////////////////////////////////////////////////

void BuildClient::DoGetAspectResults (const std::string& symbolicURI)
	{ SendString(MetaBuildSystem_GetAspectResults, symbolicURI); }

////////////////////////////////////////////////////////////

void BuildClient::DoGetSourceLineMappings (const std::string& symbolicURI)
	{ SendString(MetaBuildSystem_GetSourceLineMappings, symbolicURI); }

////////////////////////////////////////////////////////////

void BuildClient::DoGetNodeToChainOfSourceLineOriginInfo (const std::string& symbolicURI)
	{ SendString(MetaBuildSystem_GetNodeToChainOfSourceLineOriginInfo, symbolicURI); }

////////////////////////////////////////////////////////////

bool BuildClient::GetStageBinaryInfo (std::string* stageBinary, std::string* bytecodePath, std::string* dllimportPath) {
	if (!receivedData || GetResponseType() != MetaBuildSystem_StageBinaryInfo)
		return false;

	char* binary;
	char* bytecode;
	char* dllimport;

	ReadString(
		ReadString(
			ReadString(
				ForwardHeader((char*) receivedData), 
				&binary
			),
			&bytecode
		),
		&dllimport
	);
	*stageBinary = binary;
	*bytecodePath = bytecode;
	*dllimportPath = dllimport;
	return true;
}

////////////////////////////////////////////////////////////

bool BuildClient::GetAspectResults (StringList& results) {

	if (!receivedData || GetResponseType()!= MetaBuildSystem_AspectResults)
		return false;

	results.clear();
	ReadStringList(ForwardHeader((char*) receivedData), &results);
	return true;
}

////////////////////////////////////////////////////////////

bool BuildClient::GetSourceLineMappings (LineMappings& lineMappings) {

	if (!receivedData || GetResponseType()!= MetaBuildSystem_SourceLineMappings)
		return false;

	lineMappings.clear();
	ReadLineMappings(ForwardHeader((char*) receivedData), &lineMappings);
	return true;
}

////////////////////////////////////////////////////////////

bool BuildClient::GetNodeToChainOfSourceLineOriginInfo (AST::NodeToChainOfSourceLineOriginInfo& info) {

	if (!receivedData || GetResponseType()!= MetaBuildSystem_NodeToChainOfSourceLineOriginInfo)
		return false;

	info.clear();
	ReadNodeToChainOfSourceLineOriginInfo(ForwardHeader((char*) receivedData), &info);
	return true;
}
