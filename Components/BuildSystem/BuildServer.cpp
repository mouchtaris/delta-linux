// BuildServer.cpp
// Build server and thread implementation.
// Y.Lilis, February 2012.
//

#include "BuildServer.h"
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

/*
FIXME: changes for genericity

1) pendingStageSources	->	pendingSources

2) HandleBuildSource accept only STAGE_SOURCE_CLASS_ID

3) BuildClient::DoAddSource	BuildClient::DoAttachSource

	SourceInfo				->	struct SourceLineOriginInfo { line, symbolicURI }
	SourceInfoReferences	->  ChainOfSourceLineOriginInfo typedef list<>
	decouple from AST::Node

	typedef util_ui32 NodeSerialNo;
	map<NodeSerialNo, ChainOfSourceLineOriginInfo> NodeToChainOfSourceLineOriginInfo and use util_ui32 in code

	#define	STAGE_SOURCE_CLASS_ID	"StageSource"
	#define	STAGE_RESULT_CLASS_ID	"StageResult"
	#define	ASPECT_RESULT_CLASS_ID	"AspectResult"

	static void			DoAttachSource (
							const std::string&		originalSource,	->	primarySymbolicURI
							const std::string&		stageSource,	->	attachedSourceText
							const LineMappings&		lineMappings,
							const SourceReferences&	sourceRefs,		-> NodeToChainOfSourceLineOriginInfo& info
							const std::string&		classId,	
							util_ui32				index,		MEANING OF THIS? AUTO APPEND?
							bool					isResultFinal	// applicable onyl for result sources
						);

4)	
	Build_AddSource				->	as below
	extra typed messages:
	BuildSystem_AddStageSource
	BuildSystem_AddStageResult
	BuildSystem_AddAspectResult

	Build_GetTransformations	->	BuildSystem_GetAspects
	Build_GetLineMappings		->	BuildSystem_GetSourceLineMappings
	Build_GetSourceReferences	->	BuildSystem_GetNodeToChainOfSourceLineOriginInfo
	Build_RequestInvalid		
};

enum ServerToClientResponse {
	Build_SourceAdded			= 0,	BuildSystem_SourceIsAttached
	Build_StageBinary			= 1,	BuildSystem_StageBinaryInfo
	Build_Transformations		= 2,
	Build_LineMappings			= 3,
	Build_SourceReferences		= 4,
	Build_ResponseInvalid		= 5

	HandleSource -> RegisterSource
*/

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

static const StringList LineMappingsToStringList(const BuildServer::LineMappings& lineMappings) {
	StringList result;
	for (std::map< util_ui32, std::set<util_ui32> >::const_iterator i = lineMappings.begin(); i != lineMappings.end(); ++i) {
		String str = boost::lexical_cast<String>(i->first);
		for (std::set<util_ui32>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
			str += (j == i->second.begin() ? _T(":") : _T(";")) + boost::lexical_cast<String>(*j);
		result.push_back(str);
	}
	return result;
}

static const BuildServer::LineMappings StringVecToLineMappings (const StringVec& strs) {
	BuildServer::LineMappings result;

	for (StringVec::const_iterator i = strs.begin(); i != strs.end(); ++i) {
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

static const StringList SourceReferencesToStringList(const BuildServer::SourceReferences& sourceRefs) {
	StringList result;
	for (BuildServer::SourceReferences::const_iterator i = sourceRefs.begin(); i != sourceRefs.end(); ++i) {
		String str = boost::lexical_cast<String>(i->first);
		for (std::list< std::pair<std::string, util_ui32> >::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
			str += (j == i->second.begin() ? _T(":") : _T(";")); 
			str += boost::lexical_cast<String>(j->first) + _T(",") + boost::lexical_cast<String>(j->second);
		}
		result.push_back(str);
	}
	return result;
}

static const BuildServer::SourceReferences StringVecToSourceReferences(const StringVec& sourceRefs) {
	BuildServer::SourceReferences result;
	for (StringVec::const_iterator i = sourceRefs.begin(); i != sourceRefs.end(); ++i) {
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

static	const std::string				s_classId				= "BuildServer";
static	MessageBuffer*					buffer 					= (MessageBuffer*) 0;
static	ServerSocketPacketLink*			server 					= (ServerSocketPacketLink*) 0;

typedef std::map<Client*, std::string>	ClientMap;
static	ClientMap						clients;

typedef std::set<Component*>			ComponentSet;
ComponentSet							pendingStageSources;

BuildServer::HandlerMap*				BuildServer::handlers	= (HandlerMap*) 0;
boost::thread*							serviceLoopThread		=  (boost::thread*) 0;
bool									shouldExitServiceLoop	= false;

////////////////////////////////////////////////////////////

void BuildServer::ServiceLoop (void) {

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

void BuildServer::TryAcceptNewClients(void) {

	while(SocketPacketNetLink* client = DPTR(server)->AcceptClient(false)) {
		if (DPTR(client)->IsConnectionBroken())
			udelete(client);
		else 
			clients[DNEWCLASS(Client, (client))];	//insert empty string
	}
}

////////////////////////////////////////////////////////////

bool BuildServer::HandleIncomingMessages(void) {
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

void BuildServer::DispatchRequest(Client* client) {
	(*(*DPTR(handlers))[GetRequestType(DPTR(client)->GetData())])(client);
}

////////////////////////////////////////////////////////////

ClientToServerRequest BuildServer::GetRequestType (void* data) {
	if (!data)
		return Build_RequestInvalid;
	else {
		util_ui32 command;
		ReadUnsignedInt((char*) data, &command);
		return (ClientToServerRequest) command;
	}
}

////////////////////////////////////////////////////////////

void BuildServer::AddSource(
	Client*				client,
	Component*			script,
	const std::string&	source,
	const StringList&	lineMappings,
	const StringList&	sourceRefs,
	const std::string&	type,
	util_ui32			index,
	bool				isFinal
) {
	if (clients.find(client) != clients.end()) {	//may be removed by a cancel build event
		Call<const Handle (const String&, const StringList&, const StringList&, const String&, uint, bool)>(s_classId, script, "AddSource")(
			util::std2str(source),
			lineMappings,
			sourceRefs,
			util::std2str(type),
			index,
			isFinal
		);
		BuildServer::DoStageSourceResponse(client);
	}
}

////////////////////////////////////////////////////////////

Client* BuildServer::GetClientFromStageSource(Component* script) {
	DASSERT(script->GetClassId() == "StageSource");
	Component* parent = script->GetParent();
	DASSERT(parent && (parent->GetClassId() == "Script" || parent->GetClassId() == "Aspect" || parent->GetClassId() == "StageSource"));
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

void BuildServer::HandleAddSource (Client* client) {
	char* origin;
	char* target;
	LineMappings lineMappings;
	SourceReferences sourceRefs;
	char* type;
	util_ui32 index;
	bool isFinal;
	bool success;
	{
		boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
		success = GetAddSource(DPTR(client)->GetData(), &origin, &target, &lineMappings, &sourceRefs, &type, &index, &isFinal);
	}
	if (success) {
		ClientMap::iterator i = clients.find(client);
		DASSERT(i != clients.end());
		i->second = origin;	//identify self
		const Handle handle = 
			Call<Handle (const String&)>(s_classId, "ProjectManager", "GetResourceBySymbolicURI")(util::std2str(origin));
		if (handle) {
			const StringList lines = LineMappingsToStringList(lineMappings);
			const StringList refs = SourceReferencesToStringList(sourceRefs);
			timer::DelayedCaller::Instance().PostDelayedCall(
				boost::bind(AddSource, client, handle.Resolve(), std::string(target), lines, refs, std::string(type), index, isFinal)
			);
		}
	}
}

////////////////////////////////////////////////////////////

void BuildServer::HandleBuildSource (Client* client) {
	char* source;
	bool success;
	{
		boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
		success = GetBuildSource(DPTR(client)->GetData(), &source);
	}
	if (success) {
		const Handle handle = 
			Call<Handle (const String&)>(s_classId, "ProjectManager", "GetResourceBySymbolicURI")(util::std2str(source));
		if (handle) {
			Component* comp = handle.Resolve();
			Component* parent = comp->GetParent();
			UIntList buildId = Call<const UIntList (void)>(s_classId, parent, "GetWorkId")();
			buildId.push_back(Call<uint (void)>(s_classId, parent, "NextWorkSerial")());
			pendingStageSources.insert(comp);
			Call<void (const UIntList&)>(s_classId, comp, "Build")(buildId);
		}
	}
}

////////////////////////////////////////////////////////////

void BuildServer::HandleGetTransformations (Client* client) {
	char* source;
	bool success;
	{
		boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
		success = GetTransformationsSource(DPTR(client)->GetData(), &source);
	}
	if (success) {
		const Handle handle = 
			Call<Handle (const String&)>(s_classId, "ProjectManager", "GetResourceBySymbolicURI")(util::std2str(source));
		StdStringList transformations;
		if (handle) {
			DASSERT(handle.GetClassId() == "StageSource");
			BOOST_FOREACH(Component* child, handle->GetChildren())
				transformations.push_back(util::str2std(Call<const String (void)>(s_classId, child, "GetSymbolicURI")()));
		}
		DoTransformationsResponse(client, transformations);
	}
}

////////////////////////////////////////////////////////////

void BuildServer::HandleGetLineMappings (Client* client) {
	char* source;
	bool success;
	{
		boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
		success = GetLineMappingsSource(DPTR(client)->GetData(), &source);
	}
	if (success) {
		const Handle handle = 
			Call<Handle (const String&)>(s_classId, "ProjectManager", "GetResourceBySymbolicURI")(util::std2str(source));
		LineMappings lineMappings;
		if (handle) {
			DASSERT(handle.GetClassId() == "StageSource");
			if (const conf::StringListProperty* p = static_cast<const conf::StringListProperty *>(handle->GetInstanceProperty("lineMappings")))
				lineMappings = StringVecToLineMappings(p->GetValues());
		}
		DoLineMappingsResponse(client, lineMappings);
	}
}

////////////////////////////////////////////////////////////

void BuildServer::HandleGetSourceReferences (Client* client) {
	char* source;
	bool success;
	{
		boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
		success = GetSourceReferencesSource(DPTR(client)->GetData(), &source);
	}
	if (success) {
		const Handle handle = 
			Call<Handle (const String&)>(s_classId, "ProjectManager", "GetResourceBySymbolicURI")(util::std2str(source));
		SourceReferences sourceRefs;
		if (handle)
			if (const conf::StringListProperty* p = static_cast<const conf::StringListProperty *>(handle->GetInstanceProperty("sourceRefs")))
				sourceRefs = StringVecToSourceReferences(p->GetValues());
		DoSourceReferencesResponse(client, sourceRefs);
	}
}

////////////////////////////////////////////////////////////

bool BuildServer::GetAddSource (
	void*				data,
	char**				origin,
	char**				target,
	LineMappings*		lineMappings,
	SourceReferences*	sourceRefs,
	char**				type,
	util_ui32*			index,
	bool*				isFinal
) {
	if (!data || GetRequestType(data) != Build_AddSource)
		return false;
	
	util_ui32 _isFinal;
	ReadUnsignedInt(
		ReadUnsignedInt(
			ReadString(
				ReadSourceReferences(
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
					sourceRefs
				),
				type
			),
			index
			),
		&_isFinal
	);

	DASSERT(_isFinal == 0 || _isFinal == 1);
	*isFinal = !!_isFinal;

	return true;
}

////////////////////////////////////////////////////////////

bool BuildServer::GetString (ClientToServerRequest request, void* data, char** str) {

	if (!data || GetRequestType(data) != request)
		return false;

	ReadString(
		ForwardHeader((char*) data), 
		str
	);

	return true;
}

////////////////////////////////////////////////////////////

bool BuildServer::GetBuildSource (void* data, char** source)
	{ return GetString(Build_BuildSource, data, source); }

////////////////////////////////////////////////////////////

bool BuildServer::GetTransformationsSource (void* data, char** source)
	{ return GetString(Build_GetTransformations, data, source); }

////////////////////////////////////////////////////////////

bool BuildServer::GetLineMappingsSource (void* data, char** source)
	{ return GetString(Build_GetLineMappings, data, source); }

////////////////////////////////////////////////////////////

bool BuildServer::GetSourceReferencesSource (void* data, char** source)
	{ return GetString(Build_GetSourceReferences, data, source); }

////////////////////////////////////////////////////////////

void BuildServer::DoStageSourceResponse (Client* client) {
	boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
	DPTR(DPTR(client)->GetSocket())->SendUnsignedInt(Build_SourceAdded);
}

////////////////////////////////////////////////////////////

void BuildServer::DoBuildSourceResponse (Client* client, const std::string& binary, const std::string& bytecodePath, const std::string& dllimportPath) {
	util_ui32 size = SizeUnsignedInt() + SizeString(binary) + SizeString(bytecodePath) + SizeString(dllimportPath);
	DPTR(buffer)->Prepare(size);
	WriteString(
		WriteString(
			WriteString(
				WriteUnsignedInt(
					DPTR(buffer)->Get(),
					Build_StageBinary
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

void BuildServer::DoTransformationsResponse (Client* client, const StdStringList& transformations) {

	util_ui32 size = SizeUnsignedInt() + SizeStringList(transformations);
	DPTR(buffer)->Prepare(size);
	WriteStringList(
		WriteUnsignedInt(
			DPTR(buffer)->Get(),
			Build_Transformations
		),
		transformations
	);

	{
		boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
		DPTR(DPTR(client)->GetSocket())->SendMsg(DPTR(buffer)->Get(), size);
	}
}

////////////////////////////////////////////////////////////

void BuildServer::DoLineMappingsResponse (Client* client, const LineMappings& lineMappings) {

	util_ui32 size = SizeUnsignedInt() + SizeLineMappings(lineMappings);
	DPTR(buffer)->Prepare(size);
	WriteLineMappings(
		WriteUnsignedInt(
			DPTR(buffer)->Get(),
			Build_LineMappings
		),
		lineMappings
	);

	{
		boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
		DPTR(DPTR(client)->GetSocket())->SendMsg(DPTR(buffer)->Get(), size);
	}
}

////////////////////////////////////////////////////////////

void BuildServer::DoSourceReferencesResponse (Client* client, const SourceReferences& sourceRefs) {

	util_ui32 size = SizeUnsignedInt() + SizeSourceReferences(sourceRefs);
	DPTR(buffer)->Prepare(size);
	WriteSourceReferences(
		WriteUnsignedInt(
			DPTR(buffer)->Get(),
			Build_SourceReferences
		),
		sourceRefs
	);

	{
		boost::mutex::scoped_lock lock(DPTR(client)->GetMutex());
		DPTR(DPTR(client)->GetSocket())->SendMsg(DPTR(buffer)->Get(), size);
	}
}

////////////////////////////////////////////////////////////

util_ui32 BuildServer::GetPort (void) {
	DASSERT(server);
	return DPTR(server)->GetPort();
}

////////////////////////////////////////////////////////////

bool BuildServer::Initialise (util_ui32 port) {

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

void BuildServer::CleanUpClients (void) {
	for (ClientMap::iterator i = clients.begin(); i != clients.end(); ++i)
		DDELETE(i->first);
	clients.clear();
}

////////////////////////////////////////////////////////////

void BuildServer::CleanUp (void) {
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

void BuildServer::OnCompileFinished (Component* script, const UIntList& buildId) {
	ComponentSet::iterator iter = pendingStageSources.find(script);
	if (iter != pendingStageSources.end()) {
		pendingStageSources.erase(iter);
		if (Client* client = GetClientFromStageSource(script)) {
			std::string binary, bytecodePath, dllimportPath;
			if (Call<bool (void)>(s_classId, script, "IsByteCodeUpToDate")()) {
				binary = Call<const std::string (void)>(s_classId, script, "GetProducedByteCodeFileFullPath")();
				bytecodePath = util::str2std(conf::get_path_prop_value<conf::DirectoryListProperty>(script->GetInstanceProperty("bytecode_path"), _T("")));
				dllimportPath = util::str2std(conf::get_path_prop_value<conf::DirectoryListProperty>(script->GetInstanceProperty("dllimport_path"), _T("")));
			}
			DoBuildSourceResponse(client, binary, bytecodePath, dllimportPath);
		}
	}
}

////////////////////////////////////////////////////////////

#define	ADD_HANDLER(cmmd) \
	(*DPTR(handlers))[Build_##cmmd] = &BuildServer::Handle##cmmd

void BuildServer::InstallRequestHandlers(void) {
	ADD_HANDLER(AddSource);
	ADD_HANDLER(BuildSource);
	ADD_HANDLER(GetTransformations);
	ADD_HANDLER(GetLineMappings);
	ADD_HANDLER(GetSourceReferences);
}

////////////////////////////////////////////////////////////

}