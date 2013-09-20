// MetaBuildServer.cpp
// Meta build server and thread implementation.
// Y.Lilis, February 2012.
//

#include "MetaBuildServer.h"
#include "SocketPacketNetLink.h"
#include "DDebug.h"
#include "usystem.h"
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "Component.h"
#include "ComponentHandle.h"
#include "Call.h"
#include "DelayedCaller.h"
#include "StringUtils.h"

#include "ExtProperties.h"
#include "PropertyUtils.h"

#include <windows.h>

namespace ide {

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

util_ui32 SizeNodeToChainOfSourceLineOriginInfo (const std::map< util_ui32, std::list< std::pair<std::string, util_ui32> > >& info) {
	const util_ui32 uintSize = ubinaryio::SizeInteger<util_ui32>();
	util_ui32 size = uintSize;
	for (std::map< util_ui32, std::list< std::pair<std::string, util_ui32> > >::const_iterator i = info.begin(); i != info.end(); ++i) {
		size += 2 * uintSize;
		for (std::list< std::pair<std::string, util_ui32> >::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
			size += SizeString(j->first) + uintSize;
	}
	return size;
}

const char* WriteNodeToChainOfSourceLineOriginInfo (const char* buffer, const std::map< util_ui32, std::list< std::pair<std::string, util_ui32> > >& info) {
	const char* buf = WriteUnsignedInt(buffer, (util_ui32) info.size());	//map size
	for (std::map< util_ui32, std::list< std::pair<std::string, util_ui32> > >::const_iterator i = info.begin(); i != info.end(); ++i) {
		buf = WriteUnsignedInt(buf, (util_ui32) i->first);			//serial
		buf = WriteUnsignedInt(buf, (util_ui32) i->second.size());	//list size
		for (std::list< std::pair<std::string, util_ui32> >::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
			buf = WriteString(buf, j->first);						//source
			buf = WriteUnsignedInt(buf, j->second);					//line
		}
	}
	return buf;
}

char* ReadNodeToChainOfSourceLineOriginInfo (char* buffer, std::map< util_ui32, std::list< std::pair<std::string, util_ui32> > >* info) {
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
			(*info)[serial].push_back(std::make_pair(std::string(source), line));
		}
	}
	return buf;
}

////////////////////////////////////////////////////////////

static const StringList LineMappingsToStringList(const MetaBuildServer::LineMappings& lineMappings) {
	StringList result;
	for (std::map< util_ui32, std::set<util_ui32> >::const_iterator i = lineMappings.begin(); i != lineMappings.end(); ++i) {
		String str = boost::lexical_cast<String>(i->first);
		for (std::set<util_ui32>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
			str += (j == i->second.begin() ? _T(":") : _T(";")) + boost::lexical_cast<String>(*j);
		result.push_back(str);
	}
	return result;
}

static const MetaBuildServer::LineMappings StringVecToLineMappings (const StringVec& encodedLineMappings) {
	MetaBuildServer::LineMappings result;

	for (StringVec::const_iterator i = encodedLineMappings.begin(); i != encodedLineMappings.end(); ++i) {
		StringVec split;
		util::stringtokenizer(split, *i, _T(":"));
		DASSERT(split.size() == 2);
		std::set<util_ui32>& s = result[atoi(util::str2std(split[0]).c_str())];

		StringVec setItems;
		util::stringtokenizer(setItems, split[1], _T(";"));
		for (StringVec::const_iterator j = setItems.begin(); j != setItems.end(); ++j)
			s.insert(atoi(util::str2std(*j).c_str()));
	}

	return result;
}

////////////////////////////////////////////////////////////

static const StringList NodeToChainOfSourceLineOriginInfoToStringList(const MetaBuildServer::NodeToChainOfSourceLineOriginInfo& info) {
	StringList result;
	for (MetaBuildServer::NodeToChainOfSourceLineOriginInfo::const_iterator i = info.begin(); i != info.end(); ++i) {
		String str = boost::lexical_cast<String>(i->first);
		for (std::list< std::pair<std::string, util_ui32> >::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
			str += (j == i->second.begin() ? _T(":") : _T(";")); 
			str += boost::lexical_cast<String>(j->first) + _T(",") + boost::lexical_cast<String>(j->second);
		}
		result.push_back(str);
	}
	return result;
}

static const MetaBuildServer::NodeToChainOfSourceLineOriginInfo StringVecToNodeToChainOfSourceLineOriginInfo(const StringVec& encodedInfo) {
	MetaBuildServer::NodeToChainOfSourceLineOriginInfo result;
	for (StringVec::const_iterator i = encodedInfo.begin(); i != encodedInfo.end(); ++i) {
		StringVec split;
		util::stringtokenizer(split, *i, _T(":"));
		DASSERT(split.size() == 2);
		std::list< std::pair<std::string, util_ui32> >& l = result[atoi(util::str2std(split[0]).c_str())];

		StringVec listItems;
		util::stringtokenizer(listItems, split[1], _T(";"));
		for (StringVec::const_iterator j = listItems.begin(); j != listItems.end(); ++j) {
			StringVec split;
			util::stringtokenizer(split, *j, _T(","));
			DASSERT(split.size() == 2);
			l.push_back(std::make_pair(util::str2std(split[0]), atoi(util::str2std(split[1]).c_str())));
		}
	}
	return result;
}

////////////////////////////////////////////////////////////

class MessageBuffer {
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

	MessageBuffer (void) : buffer((char*) 0), size(0){}
	~MessageBuffer() {  Clear(); }
};

////////////////////////////////////////////////////////////

class Client {
private:
	SocketPacketNetLink*	socket;
	void*					receivedData;
	util_ui32				receivedSize;
	boost::mutex			mutex;

public:
	bool IsConnectionBroken (void) { return DPTR(socket)->IsConnectionBroken(); }
	bool IsRequestPending (void) { return (receivedData && receivedSize) || DPTR(socket)->IsMsgPending(); }

	bool ReceiveRequest (void) {
		if (receivedData && receivedSize)
			return true;
		else if (!DPTR(socket)->IsMsgPending())
			return false;
		else {
			DPTR(socket)->ReceiveMsg();
			DPTR(socket)->ReadMsg(&receivedData, &receivedSize);
			return true;
		}
	}

	void RequestProcessed (void) {
		if (receivedData) {
			DPTR(socket)->ClearMsg();
			unullify(receivedData);
			receivedSize = 0;
		}
	}

	SocketPacketNetLink* GetSocket(void) { return socket; }
	void* GetData(void) { return receivedData; }
	util_ui32 GetSize(void) const { return receivedSize; }
	boost::mutex& GetMutex(void) { return mutex; }

	Client(SocketPacketNetLink* socket) : socket(socket), receivedData((void *) 0), receivedSize(0) {}
	~Client() { udelete(socket); }
};

////////////////////////////////////////////////////////////

#define	CHECK_SENDRECEIVE				0
#define	SERVICELOOP_MIN_SLEEPTIME		10
#define	SERVICELOOP_MAX_SLEEPTIME		2000
#define	SERVICELOOP_ADD_SLEEPTIME		50

////////////////////////////////////////////////////////////

static	const std::string				s_classId				= "MetaBuildServer";
static	MessageBuffer*					buffer 					= (MessageBuffer*) 0;
static	ServerSocketPacketLink*			server 					= (ServerSocketPacketLink*) 0;

typedef std::map<Client*, std::string>	ClientMap;
static	ClientMap						clients;

typedef std::set<Component*>			ComponentSet;
ComponentSet							pendingSources;

MetaBuildServer::HandlerMap*			MetaBuildServer::handlers	= (HandlerMap*) 0;
boost::thread*							serviceLoopThread			=  (boost::thread*) 0;
bool									shouldExitServiceLoop		= false;

////////////////////////////////////////////////////////////

void MetaBuildServer::ServiceLoop (void) {

	uadaptivesleep	sleeper(
						SERVICELOOP_MIN_SLEEPTIME,
						SERVICELOOP_MAX_SLEEPTIME,
						SERVICELOOP_ADD_SLEEPTIME
					);

	while (!shouldExitServiceLoop) {
		Sleep(sleeper.bedtime());
		TryAcceptNewClients();
		if (HandleIncomingMessages())
			sleeper.action();
		else
			sleeper.wait();
	}
}

////////////////////////////////////////////////////////////

void MetaBuildServer::TryAcceptNewClients(void) {

	while(SocketPacketNetLink* client = DPTR(server)->AcceptClient(false)) {
		if (DPTR(client)->IsConnectionBroken())
			udelete(client);
		else 
			clients[DNEWCLASS(Client, (client))];	//insert empty string
	}
}

////////////////////////////////////////////////////////////

bool MetaBuildServer::HandleIncomingMessages(void) {
	bool retval = false;
	for (ClientMap::iterator i = clients.begin(); i != clients.end(); /*empty*/) {
		Client* client = i->first;
		if (DPTR(client)->IsConnectionBroken()) {
			udelete(client);
			i = clients.erase(i);
		}
		else {
			if (DPTR(client)->IsRequestPending() && DPTR(client)->ReceiveRequest()) {
				DispatchRequest(client);
				DPTR(client)->RequestProcessed();
				retval = true;
			}
			++i;
		}
	}
	return retval;
}

////////////////////////////////////////////////////////////

void MetaBuildServer::DispatchRequest(Client* client) {
	(*(*DPTR(handlers))[GetRequestType(DPTR(client)->GetData())])(client);
}

////////////////////////////////////////////////////////////

ClientToServerRequest MetaBuildServer::GetRequestType (void* data) {
	if (!data)
		return MetaBuildSystem_RequestInvalid;
	else {
		util_ui32 command;
		ReadUnsignedInt((char*) data, &command);
		return (ClientToServerRequest) command;
	}
}

////////////////////////////////////////////////////////////

void MetaBuildServer::AttachSource(
	Client*				client,
	Component*			script,
	const std::string&	source,
	const StringList&	lineMappings,
	const StringList&	sourceRefs,
	const std::string&	classId,
	bool				isFinal
) {
	if (clients.find(client) != clients.end()) {	//may be removed by a cancel build event
		Call<const Handle (const String&, const StringList&, const StringList&, const String&, bool)>(s_classId, script, "AttachSource")(
			util::std2str(source),
			lineMappings,
			sourceRefs,
			util::std2str(classId),
			isFinal
		);
		MetaBuildServer::DoSourceIsAttachedResponse(client);
	}
}

////////////////////////////////////////////////////////////

Client* MetaBuildServer::GetClientFromStageSource(Component* script) {
	Component* parent = script->GetParent();
	DASSERT(parent);
	const std::string symbolic = util::str2std(Call<const String (void)>(s_classId, parent, "GetSymbolicURI")());
		
	Client* client = (Client*) 0;
	for (ClientMap::iterator i = clients.begin(); i != clients.end(); ++i)
		if (i->second == symbolic) {
			client = i->first;
			break;
		}
	return client;
}

////////////////////////////////////////////////////////////

void MetaBuildServer::HandleAttachSource (ClientToServerRequest request, Client* client) {
	char*								origin;
	char*								target;
	LineMappings						lineMappings;
	NodeToChainOfSourceLineOriginInfo	info;
	char*								classId;
	bool								isFinal;
	bool								success;
	{
		boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
		success = GetAttachSourceData(request, DPTR(client)->GetData(), &origin, &target, &lineMappings, &info, &classId, &isFinal);
	}
	if (success) {
		ClientMap::iterator i = clients.find(client);
		DASSERT(i != clients.end());
		i->second = origin;	//identify self
		const Handle handle = 
			Call<Handle (const String&)>(s_classId, "ProjectManager", "GetResourceBySymbolicURI")(util::std2str(origin));
		if (handle) {
			const StringList lines = LineMappingsToStringList(lineMappings);
			const StringList refs = NodeToChainOfSourceLineOriginInfoToStringList(info);
			timer::DelayedCaller::Instance().PostDelayedCall(
				boost::bind(AttachSource, client, handle.Resolve(), std::string(target), lines, refs, std::string(classId), isFinal)
			);
		}
	}
}

////////////////////////////////////////////////////////////

void MetaBuildServer::HandleAttachStageSource (Client* client)
	{ HandleAttachSource(MetaBuildSystem_AttachStageSource, client); }

////////////////////////////////////////////////////////////

void MetaBuildServer::HandleAttachStageResult (Client* client)
	{ HandleAttachSource(MetaBuildSystem_AttachStageResult, client); }

////////////////////////////////////////////////////////////

void MetaBuildServer::HandleAttachAspectResult (Client* client)
	{ HandleAttachSource(MetaBuildSystem_AttachAspectResult, client); }

////////////////////////////////////////////////////////////

void MetaBuildServer::HandleBuildStageSource (Client* client) {
	char* source;
	bool success;
	{
		boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
		success = GetSourceFromBuildStageSourceRequest(DPTR(client)->GetData(), &source);
	}
	if (success) {
		const Handle handle = 
			Call<Handle (const String&)>(s_classId, "ProjectManager", "GetResourceBySymbolicURI")(util::std2str(source));
		if (handle) {
			Component* comp = handle.Resolve();
			DASSERT(comp->GetClassId() == "StageSource" || comp->GetClassId() == "StageResult" || comp->GetClassId() == "AspectResult");		//stage or final result
			Component* parent = comp->GetParent();
			UIntList buildId = Call<const UIntList (void)>(s_classId, parent, "GetWorkId")();
			buildId.push_back(Call<uint (void)>(s_classId, parent, "NextWorkSerial")());
			pendingSources.insert(comp);
			Call<void (const UIntList&)>(s_classId, comp, "Build")(buildId);
		}
	}
}

////////////////////////////////////////////////////////////

void MetaBuildServer::HandleGetAspectResults (Client* client) {
	char* source;
	bool success;
	{
		boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
		success = GetSourceFromGetAspectResultsRequest(DPTR(client)->GetData(), &source);
	}
	if (success) {
		const Handle handle = 
			Call<Handle (const String&)>(s_classId, "ProjectManager", "GetResourceBySymbolicURI")(util::std2str(source));
		StdStringList aspectResults;
		if (handle) {
			DASSERT(handle.GetClassId() == "StageSource");	//invoked only to retrieve aspect results of a stage source
			BOOST_FOREACH(Component* child, handle->GetChildren())
				aspectResults.push_back(util::str2std(Call<const String (void)>(s_classId, child, "GetSymbolicURI")()));
		}
		DoAspectResultsResponse(client, aspectResults);
	}
}

////////////////////////////////////////////////////////////

void MetaBuildServer::HandleGetSourceLineMappings (Client* client) {
	char* source;
	bool success;
	{
		boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
		success = GetSourceFromGetSourceLineMappingsRequest(DPTR(client)->GetData(), &source);
	}
	if (success) {
		const Handle handle = 
			Call<Handle (const String&)>(s_classId, "ProjectManager", "GetResourceBySymbolicURI")(util::std2str(source));
		LineMappings lineMappings;
		if (handle) {
			DASSERT(handle.GetClassId() == "AspectResult");	//invoked only for stage source aspect results
			if (const conf::StringListProperty* p = static_cast<const conf::StringListProperty *>(handle->GetInstanceProperty("lineMappings")))
				lineMappings = StringVecToLineMappings(p->GetValues());
		}
		DoSourceLineMappingsResponse(client, lineMappings);
	}
}

////////////////////////////////////////////////////////////

void MetaBuildServer::HandleGetNodeToChainOfSourceLineOriginInfo (Client* client) {
	char* source;
	bool success;
	{
		boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
		success = GetSourceFromNodeToChainOfSourceLineOriginInfoRequest(DPTR(client)->GetData(), &source);
	}
	if (success) {
		const Handle handle = 
			Call<Handle (const String&)>(s_classId, "ProjectManager", "GetResourceBySymbolicURI")(util::std2str(source));
		NodeToChainOfSourceLineOriginInfo info;
		if (handle)
			if (const conf::StringListProperty* p = static_cast<const conf::StringListProperty *>(handle->GetInstanceProperty("sourceRefs")))
				info = StringVecToNodeToChainOfSourceLineOriginInfo(p->GetValues());
		DoNodeToChainOfSourceLineOriginInfoResponse(client, info);
	}
}

////////////////////////////////////////////////////////////

bool MetaBuildServer::GetAttachSourceData (
	ClientToServerRequest				request, 
	void*								data,
	char**								origin,
	char**								target,
	LineMappings*						lineMappings,
	NodeToChainOfSourceLineOriginInfo*	info,
	char**								type,
	bool*								isFinal
) {
	if (!data || GetRequestType(data) != request) 
		return false;

	util_ui32 _isFinal;
	ReadUnsignedInt(
		ReadString(
			ReadNodeToChainOfSourceLineOriginInfo(
				ReadLineMappings(
					ReadString(
						ReadString(
							ForwardHeader((char*) data), 
							origin
						),
						target
					),
					lineMappings
				),
				info
			),
			type
		),
		&_isFinal
	);

	DASSERT(_isFinal == 0 || _isFinal == 1);
	*isFinal = !!_isFinal;

	return true;
}

////////////////////////////////////////////////////////////

bool MetaBuildServer::GetString (ClientToServerRequest request, void* data, char** str) {

	if (!data || GetRequestType(data) != request)
		return false;

	ReadString(
		ForwardHeader((char*) data), 
		str
	);

	return true;
}

////////////////////////////////////////////////////////////

bool MetaBuildServer::GetSourceFromBuildStageSourceRequest (void* data, char** source)
	{ return GetString(MetaBuildSystem_BuildStageSource, data, source); }

////////////////////////////////////////////////////////////

bool MetaBuildServer::GetSourceFromGetAspectResultsRequest (void* data, char** source)
	{ return GetString(MetaBuildSystem_GetAspectResults, data, source); }

////////////////////////////////////////////////////////////

bool MetaBuildServer::GetSourceFromGetSourceLineMappingsRequest (void* data, char** source)
	{ return GetString(MetaBuildSystem_GetSourceLineMappings, data, source); }

////////////////////////////////////////////////////////////

bool MetaBuildServer::GetSourceFromNodeToChainOfSourceLineOriginInfoRequest (void* data, char** source)
	{ return GetString(MetaBuildSystem_GetNodeToChainOfSourceLineOriginInfo, data, source); }

////////////////////////////////////////////////////////////

void MetaBuildServer::DoSourceIsAttachedResponse (Client* client) {
	boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
	DPTR(DPTR(client)->GetSocket())->SendUnsignedInt(MetaBuildSystem_SourceIsAttached);
}

////////////////////////////////////////////////////////////

void MetaBuildServer::DoBuildStageSourceResponse (Client* client, const std::string& binary, const std::string& bytecodePath, const std::string& dllimportPath) {
	util_ui32 size = SizeUnsignedInt() + SizeString(binary) + SizeString(bytecodePath) + SizeString(dllimportPath);
	DPTR(buffer)->Prepare(size);
	WriteString(
		WriteString(
			WriteString(
				WriteUnsignedInt(
					DPTR(buffer)->Get(),
					MetaBuildSystem_StageBinaryInfo
				),
				binary
			),
			bytecodePath
		),
		dllimportPath
	);

	{
		boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
		DPTR(DPTR(client)->GetSocket())->SendMsg(DPTR(buffer)->Get(), size);
	}
}

////////////////////////////////////////////////////////////

void MetaBuildServer::DoAspectResultsResponse (Client* client, const StdStringList& results) {

	util_ui32 size = SizeUnsignedInt() + SizeStringList(results);
	DPTR(buffer)->Prepare(size);
	WriteStringList(
		WriteUnsignedInt(
			DPTR(buffer)->Get(),
			MetaBuildSystem_AspectResults
		),
		results
	);

	{
		boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
		DPTR(DPTR(client)->GetSocket())->SendMsg(DPTR(buffer)->Get(), size);
	}
}

////////////////////////////////////////////////////////////

void MetaBuildServer::DoSourceLineMappingsResponse (Client* client, const LineMappings& lineMappings) {

	util_ui32 size = SizeUnsignedInt() + SizeLineMappings(lineMappings);
	DPTR(buffer)->Prepare(size);
	WriteLineMappings(
		WriteUnsignedInt(
			DPTR(buffer)->Get(),
			MetaBuildSystem_SourceLineMappings
		),
		lineMappings
	);

	{
		boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
		DPTR(DPTR(client)->GetSocket())->SendMsg(DPTR(buffer)->Get(), size);
	}
}

////////////////////////////////////////////////////////////

void MetaBuildServer::DoNodeToChainOfSourceLineOriginInfoResponse (Client* client, const NodeToChainOfSourceLineOriginInfo& info) {

	util_ui32 size = SizeUnsignedInt() + SizeNodeToChainOfSourceLineOriginInfo(info);
	DPTR(buffer)->Prepare(size);
	WriteNodeToChainOfSourceLineOriginInfo(
		WriteUnsignedInt(
			DPTR(buffer)->Get(),
			MetaBuildSystem_NodeToChainOfSourceLineOriginInfo
		),
		info
	);

	{
		boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
		DPTR(DPTR(client)->GetSocket())->SendMsg(DPTR(buffer)->Get(), size);
	}
}

////////////////////////////////////////////////////////////

util_ui32 MetaBuildServer::GetPort (void) {
	DASSERT(server);
	return DPTR(server)->GetPort();
}

////////////////////////////////////////////////////////////

bool MetaBuildServer::Initialise (util_ui32 port) {

	SocketNetLink::Initialise();
	unew(handlers);
	InstallRequestHandlers();

	unew(buffer);
	shouldExitServiceLoop = false;

	if (!(server = DNEWCLASS(ServerSocketPacketLink, (port))))
		return false;
	else {
		serviceLoopThread = DNEWCLASS(boost::thread, (ServiceLoop));
		return true;
	}
}

////////////////////////////////////////////////////////////

void MetaBuildServer::CleanUpClients (void) {
	for (ClientMap::iterator i = clients.begin(); i != clients.end(); ++i)
		DDELETE(i->first);
	clients.clear();
}

////////////////////////////////////////////////////////////

void MetaBuildServer::CleanUp (void) {
	shouldExitServiceLoop = true;
	if (serviceLoopThread) {
		DPTR(serviceLoopThread)->join();
		udelete(serviceLoopThread);
	}
	CleanUpClients();

	udelete(buffer);
	udelete(handlers);
	udeleteunlessnull(server);

	SocketNetLink::CleanUp();
}

////////////////////////////////////////////////////////////

void MetaBuildServer::OnCompileFinished (Component* script, const UIntList& buildId) {
	DASSERT(script->GetClassId() == "StageSource" || script->GetClassId() == "StageResult" || script->GetClassId() == "AspectResult");
	ComponentSet::iterator iter = pendingSources.find(script);
	if (iter != pendingSources.end()) {
		pendingSources.erase(iter);
		if (Client* client = GetClientFromStageSource(script)) {
			std::string binary, bytecodePath, dllimportPath;
			if (Call<bool (void)>(s_classId, script, "IsByteCodeUpToDate")()) {
				binary = Call<const std::string (void)>(s_classId, script, "GetProducedByteCodeFileFullPath")();
				bytecodePath = util::str2std(conf::get_path_prop_value<conf::DirectoryListProperty>(script->GetInstanceProperty("bytecode_path"), _T("")));
				dllimportPath = util::str2std(conf::get_path_prop_value<conf::DirectoryListProperty>(script->GetInstanceProperty("dllimport_path"), _T("")));
			}
			DoBuildStageSourceResponse(client, binary, bytecodePath, dllimportPath);
		}
	}
}

////////////////////////////////////////////////////////////

#define	ADD_HANDLER(cmmd) \
	(*DPTR(handlers))[MetaBuildSystem_##cmmd] = &MetaBuildServer::Handle##cmmd

void MetaBuildServer::InstallRequestHandlers(void) {
	ADD_HANDLER(AttachStageSource);
	ADD_HANDLER(AttachStageResult);
	ADD_HANDLER(AttachAspectResult);
	ADD_HANDLER(BuildStageSource);
	ADD_HANDLER(GetAspectResults);
	ADD_HANDLER(GetSourceLineMappings);
	ADD_HANDLER(GetNodeToChainOfSourceLineOriginInfo);
}

////////////////////////////////////////////////////////////

}