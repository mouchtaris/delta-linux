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

util_ui32 SizeSourceReferences (const std::map< util_ui32, std::list< std::pair<std::string, util_ui32> > >& sourceRefs) {
	const util_ui32 uintSize = ubinaryio::SizeInteger<util_ui32>();
	util_ui32 size = uintSize;
	for (std::map< util_ui32, std::list< std::pair<std::string, util_ui32> > >::const_iterator i = sourceRefs.begin(); i != sourceRefs.end(); ++i) {
		size += 2 * uintSize;
		for (std::list< std::pair<std::string, util_ui32> >::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
			size += SizeString(j->first) + uintSize;
	}
	return size;
}

const char* WriteSourceReferences (const char* buffer, const std::map< util_ui32, std::list< std::pair<std::string, util_ui32> > >& sourceRefs) {
	const char* buf = WriteUnsignedInt(buffer, (util_ui32) sourceRefs.size());	//map size
	for (std::map< util_ui32, std::list< std::pair<std::string, util_ui32> > >::const_iterator i = sourceRefs.begin(); i != sourceRefs.end(); ++i) {
		buf = WriteUnsignedInt(buf, (util_ui32) i->first);			//serial
		buf = WriteUnsignedInt(buf, (util_ui32) i->second.size());	//list size
		for (std::list< std::pair<std::string, util_ui32> >::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
			buf = WriteString(buf, j->first);						//source
			buf = WriteUnsignedInt(buf, j->second);					//line
		}
	}
	return buf;
}

char* ReadSourceReferences(char* buffer, std::map< util_ui32, std::list< std::pair<std::string, util_ui32> > >* sourceRefs) {
	util_ui32 size;
	char* buf = ReadUnsignedInt(buffer, &size);
	for (util_ui32 i = 0; i < size; ++i) {
		util_ui32 serial;
		buf = ReadUnsignedInt(buf, &serial);
		(*sourceRefs)[serial];	//insert serial in the map

		util_ui32 listSize;
		buf = ReadUnsignedInt(buf, &listSize);
		for (util_ui32 i = 0; i < listSize; ++i) {
			char* source;
			util_ui32 line;
			buf = ReadUnsignedInt(ReadString(buf, &source), &line);
			(*sourceRefs)[serial].push_back(std::make_pair(std::string(source), line));
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
		return Build_ResponseInvalid;
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

void BuildClient::DoAddSource (
	const std::string&		originalSource,
	const std::string&		stageSource,
	const LineMappings&		lineMappings,
	const SourceReferences& sourceRefs,
	const std::string&		type,
	util_ui32				index,
	bool					isFinal
) {
	util_ui32 size =	3 * SizeUnsignedInt() +
						SizeString(originalSource) +
						SizeString(stageSource) + 						
						SizeLineMappings(lineMappings) +
						SizeSourceReferences(sourceRefs) +
						SizeString(type);
	DPTR(buffer)->Prepare(size);

	WriteUnsignedInt(
		WriteUnsignedInt(
			WriteString(
				WriteSourceReferences(
					WriteLineMappings(
						WriteString(
							WriteString(
								WriteUnsignedInt(
									DPTR(buffer)->Get(), 
									Build_AddSource
								),
								originalSource
							),
							stageSource
						),
						lineMappings
					),
					sourceRefs
				),
				type
			),
			index
		),
		isFinal ? 1 : 0
	);

	DPTR(clientLink)->SendMsg(DPTR(buffer)->Get(), size);
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

void BuildClient::DoBuildSource (const std::string& source)
	{ SendString(Build_BuildSource, source); }

////////////////////////////////////////////////////////////

void BuildClient::DoGetTransformations (const std::string& source)
	{ SendString(Build_GetTransformations, source); }

////////////////////////////////////////////////////////////

void BuildClient::DoGetLineMappings (const std::string& source)
	{ SendString(Build_GetLineMappings, source); }

////////////////////////////////////////////////////////////

void BuildClient::DoGetSourceReferences (const std::string& source)
	{ SendString(Build_GetSourceReferences, source); }

////////////////////////////////////////////////////////////

bool BuildClient::GetStageBinaryData (std::string* stageBinary, std::string* bytecodePath, std::string* dllimportPath) {
	if (!receivedData || GetResponseType() != Build_StageBinary)
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

bool BuildClient::GetTransformations (StringList& transformations) {

	if (!receivedData || GetResponseType()!= Build_Transformations)
		return false;

	transformations.clear();
	ReadStringList(ForwardHeader((char*) receivedData), &transformations);
	return true;
}

////////////////////////////////////////////////////////////

bool BuildClient::GetLineMappings (LineMappings& lineMappings) {

	if (!receivedData || GetResponseType()!= Build_LineMappings)
		return false;

	lineMappings.clear();
	ReadLineMappings(ForwardHeader((char*) receivedData), &lineMappings);
	return true;
}

////////////////////////////////////////////////////////////

bool BuildClient::GetSourceReferences (SourceReferences& sourceRefs) {

	if (!receivedData || GetResponseType()!= Build_SourceReferences)
		return false;

	sourceRefs.clear();
	ReadSourceReferences(ForwardHeader((char*) receivedData), &sourceRefs);
	return true;
}
