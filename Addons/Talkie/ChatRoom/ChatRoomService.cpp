/**
 *	ChatRoomService.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ChatRoomService.h"
#include "Connection.h"
#include "NetUtils.h"
#include "Streams.h"
#include "Algorithms.h"
#include "Adaptors.h"
#include "SimpleTimer.h"

using namespace timer;

////////////////////////////////////////////////////////////////////////

#define NET_TREE_DEGREE 2

////////////////////////////////////////////////////////////////////////

enum ChatRoomCommands {

	CRC_PEER_CONNECTION,

	CRC_JOIN_GET_REMOTE_ENTRY,
	CRC_JOIN_GET_TOPOLOGY,
};

//**********************************************************************

enum ChatRoomSession {
	CRS_FWD_MESSAGE,
	CRS_MESSAGE,

	CRS_FWD_PEER_DISCONNECTED,
	CRS_PEER_DISCONNECTED,

	CRS_FWD_PEER_CONNECTED,
	CRS_PEER_CONNECTED,

	CRS_FWD_PEER_REMOVED,
	CRS_PEER_REMOVED,
};

////////////////////////////////////////////////////////////////////////

namespace talkie {

////////////////////////////////////////////////////////////////////////
// ChatRoomService class
//
ChatRoomService::ChatRoomService (boost::asio::io_service& service, const FriendEntry& me) :
	m_commandServer(service), m_myself(me), m_connections(0),
	m_chatRoomOnline(false), m_timerId(0)
{
	////////////////////////////////////////////////////////////////////
	// Chat room command server initialization
	//
	m_commandServer.RegisterCommand(CRC_PEER_CONNECTION,
		boost::bind(&ChatRoomService::cmdNewConnection, this, _1, _2, _3));

	m_commandServer.RegisterCommand(CRC_JOIN_GET_REMOTE_ENTRY,
		boost::bind(&ChatRoomService::cmdReadRemoteEntry, this, _1, _2, _3));
	m_commandServer.RegisterCommand(CRC_JOIN_GET_TOPOLOGY,
		boost::bind(&ChatRoomService::cmdReadTopology, this, _1, _2, _3));

	m_commandServer.Accept();
	m_myself.GetEndpoint().SetPortNum(m_commandServer.GetPort());

	////////////////////////////////////////////////////////////////////
	// Chat room session command dispatcher initialization
	//
	m_chatRoomDispatcher.SetLinkErrorHandler(
		boost::bind(&ChatRoomService::sCmdConnectionError, this, _1, _2)
	);
	m_chatRoomDispatcher.RegisterCommand(CRS_FWD_MESSAGE,
		boost::bind(&ChatRoomService::sCmdFwdMessage, this, _1, _2, _3));
	m_chatRoomDispatcher.RegisterCommand(CRS_MESSAGE,
		boost::bind(&ChatRoomService::sCmdNewMessage, this, _1, _2, _3));

	m_chatRoomDispatcher.RegisterCommand(CRS_FWD_PEER_DISCONNECTED,
		boost::bind(&ChatRoomService::sCmdFwdPeerDisconnected, this, _1, _2, _3));
	m_chatRoomDispatcher.RegisterCommand(CRS_PEER_DISCONNECTED,
		boost::bind(&ChatRoomService::sCmdPeerDisconnected, this, _1, _2, _3));

	m_chatRoomDispatcher.RegisterCommand(CRS_FWD_PEER_CONNECTED,
		boost::bind(&ChatRoomService::sCmdFwdPeerConnected, this, _1, _2, _3));
	m_chatRoomDispatcher.RegisterCommand(CRS_PEER_CONNECTED,
		boost::bind(&ChatRoomService::sCmdPeerConnected, this, _1, _2, _3));

	m_chatRoomDispatcher.SetContinuousMonitor(true);
}

//**********************************************************************

ChatRoomService::~ChatRoomService (void)
{
	this->closeAllConnections();
}

//**********************************************************************

void ChatRoomService::AddParticipants (const FriendEntryVec& participants)
{
	FriendEntryVec uniqueParticipants;
	FriendEntry::CanConnectPred canConnect;

	FriendEntryVec::const_iterator iter = participants.begin();
	for (;iter != participants.end(); ++iter) {
		if (!m_participants.FindFriend(iter->GetHandle()) && canConnect(*iter))
			uniqueParticipants.push_back(*iter);
	}
	if (!uniqueParticipants.empty())
		this->InitializeChatRoom(uniqueParticipants);
}

//**********************************************************************

void ChatRoomService::RemoveParticipant (const String& handle)
{
}

//**********************************************************************

void ChatRoomService::SendText (const String& message)
	{ this->sendText(m_myself.GetHandle(), message, true); }

//**********************************************************************

void ChatRoomService::SendFile (const String& file)
{

}

//**********************************************************************

void ChatRoomService::InitializeChatRoom (const FriendEntryVec& participants)
{
	const std::string data = comm::encoder() << BC_JOIN_CHATROOM << m_myself.GetHandle();

	FriendEntryVec::const_iterator iter = participants.begin();
	while (iter != participants.end()) {
		const FriendEntry& entry = *iter++;

		net::connection_ptr conn(new net::connection(m_commandServer.GetIOService()));

		if (net::connect_to<ProtoTCP>(entry.GetEndpoint(), *conn)) {
			conn->write(data);
			++m_connections;
			m_pendingAdditions.push_back(ConnEntry(conn, entry.GetHandle()));

			m_commandServer.ReadCommandFromLink(conn);
		}
	}
	if (!m_chatRoomOnline)
		m_participants.AddFriend(m_myself); // Add me-self to da list

	SimpleTimer::Cancel(m_timerId);
	m_timerId = SimpleTimer::Set(5, boost::bind(&ChatRoomService::timerExpired, this, _1));
}

//**********************************************************************

void ChatRoomService::JoinChatRoom (net::connection_ptr conn)
{
	conn->write(comm::encoder() << CRC_JOIN_GET_REMOTE_ENTRY << m_myself);
	m_commandServer.ReadCommandFromLink(conn);
}

//**********************************************************************

bool ChatRoomService::cmdNewConnection (int32 command, net::connection_ptr conn, comm::decoder& dec)
{
	String handle;
	dec >> handle;
	m_children.push_back(ConnEntry(conn, handle));
	m_chatRoomDispatcher.ReadCommandFromLink(conn);
	return true;
}

//**********************************************************************

bool ChatRoomService::cmdReadRemoteEntry (int32 command, net::connection_ptr conn, comm::decoder& dec)
{
	FriendEntry entry;
	dec >> entry;

	if (m_chatRoomOnline) {
		this->sendNodeConnected(entry, true);
		this->connectNode(entry);
	}
	else
		m_participants.AddFriend(entry);

	if (--m_connections == 0) {
		SimpleTimer::Cancel(m_timerId);
		this->sendTopology();
	}
	return true;
}

//**********************************************************************

bool ChatRoomService::cmdReadTopology (int32 command, net::connection_ptr conn, comm::decoder& dec)
{
	FriendEntryVec topology;
	dec >> topology;

	conn->close();

	m_participants.AddFriends(topology);

	if (this->connectToParent())
		this->createChatRoom(false);
	else
		this->generalChatRoomError(_("Failed to join the chat. Is it cause I is black ?"));

	return true;
}

//**********************************************************************

void ChatRoomService::sCmdConnectionError (const NetError& error, net::connection_ptr conn)
{
	String handle;

	if (m_parent == conn) {

		handle = m_parent.GetHandle();
		this->disconnectNode(handle);

		if (!this->connectToParent()) {
			this->generalChatRoomError(_("Failed to recover. Is it cause I is black ?"));
			return;
		}
	}
	else {
		ConnectionList::iterator iter = std::find(m_children.begin(), m_children.end(), conn);

		if (iter != m_children.end()) {

			handle = iter->GetHandle();
			this->disconnectNode(handle);
			m_children.erase(iter);
		}
	}
	this->sendNodeDisconnected(handle, true);
}

//**********************************************************************

bool ChatRoomService::sCmdFwdMessage (int32 command, net::connection_ptr conn, comm::decoder& dec)
{
	String handle, message;
	dec >> handle >> message;
	this->sendText(handle, message, true);
	return true;
}

//**********************************************************************

bool ChatRoomService::sCmdNewMessage (int32 command, net::connection_ptr conn, comm::decoder& dec)
{
	String handle, message;
	dec >> handle >> message;
	this->sendText(handle, message, false);
	return true;
}

//**********************************************************************

bool ChatRoomService::sCmdFwdPeerDisconnected (
		int32 command, net::connection_ptr conn, comm::decoder& dec
	)
{
	String handle;
	dec >> handle;
	this->sendNodeDisconnected(handle, true);
	return true;
}

//**********************************************************************

bool ChatRoomService::sCmdPeerDisconnected (
		int32 command, net::connection_ptr conn, comm::decoder& dec
	)
{
	String handle;
	dec >> handle;
	this->sendNodeDisconnected(handle, false);
	return true;
}

//**********************************************************************

bool ChatRoomService::sCmdFwdPeerConnected (
		int32 command, net::connection_ptr conn, comm::decoder& dec
	)
{
	FriendEntry entry;
	dec >> entry;
	this->sendNodeConnected(entry, true);
	return true;
}

//**********************************************************************

bool ChatRoomService::sCmdPeerConnected (
		int32 command, net::connection_ptr conn, comm::decoder& dec
	)
{
	FriendEntry entry;
	dec >> entry;
	this->sendNodeConnected(entry, false);
	return true;
}

//**********************************************************************

void ChatRoomService::sendText (const String& handle, const String& message, bool fwd)
{
	if (m_parent && fwd)
		m_parent.Write(comm::encoder() << CRS_FWD_MESSAGE << handle << message);
	else {
		const std::string data = comm::encoder() << CRS_MESSAGE << handle << message;
		util::for_all(m_children, boost::bind(&ConnEntry::Write, _1, boost::ref(data)));
		const FriendEntry& entry = m_participants.FindFriend(handle);
		sigNewMessage(entry, message);
	}
}

//**********************************************************************

void ChatRoomService::sendNodeDisconnected (const String& handle, bool fwd)
{
	if (m_parent && fwd)
		m_parent.Write(comm::encoder() << CRS_FWD_PEER_DISCONNECTED << handle);
	else {
		const std::string data = comm::encoder() << CRS_PEER_DISCONNECTED << handle;
		util::for_all(m_children, boost::bind(&ConnEntry::Write, _1, boost::ref(data)));
		this->disconnectNode(handle);
	}
}

//**********************************************************************

void ChatRoomService::sendNodeConnected (const FriendEntry& entry, bool fwd)
{
	if (m_parent && fwd)
		m_parent.Write(comm::encoder() << CRS_FWD_PEER_CONNECTED << entry);
	else {
		const std::string data = comm::encoder() << CRS_PEER_CONNECTED << entry;
		util::for_all(m_children, boost::bind(&ConnEntry::Write, _1, boost::ref(data)));
		this->connectNode(entry);
	}
}

//**********************************************************************

void ChatRoomService::createChatRoom (bool initializerNode)
{
	assert(initializerNode || !m_chatRoomOnline);

	if (!m_chatRoomOnline) {
		m_chatRoomOnline = true;
		sigChatRoomCreated();
	}
}

//**********************************************************************

void ChatRoomService::generalChatRoomError (const String& reason)
{
	this->closeAllConnections();
	sigChatRoomError(reason);
}

//**********************************************************************

void ChatRoomService::disconnectNode (const String& handle)
{
	const FriendEntry& failedFriend = m_participants.FindFriend(handle);
	if (failedFriend) {
		sigUserDisconnected(failedFriend, _("Your main man was disconnected"));
		m_participants.RemoveFriend(handle);
	}
}

//**********************************************************************

void ChatRoomService::connectNode (const FriendEntry& entry)
{
	const FriendEntry& addedFriend = m_participants.FindFriend(entry.GetHandle());
	if (!addedFriend) {
		m_participants.AddFriend(entry);
		sigUserConnected(entry, _("Booyakasha"));
	}
}

//**********************************************************************

bool ChatRoomService::connectToParent (void)
{
	const FriendEntryVec& participants = m_participants.GetFriends();

	FriendEntryVec::const_iterator iter =
		std::find(participants.begin(), participants.end(), m_myself);

	if (iter == participants.end())
		return false;

	////////////////////////////////////////////////////////////////////
	// Calculate and test myOffset and parentIndex
	//
	uint myOffset = std::distance(participants.begin(), iter);

	if (myOffset == 0) { // I is the main man !
		m_parent.Reset();
		return true;
	}

	uint parentIndex = (myOffset - 1) / NET_TREE_DEGREE;
	assert(parentIndex < participants.size());

	////////////////////////////////////////////////////////////////////
	// Initialize the connection entry and try to connect
	//
	if (m_parent == participants[parentIndex]) // Already connected (?)
		return true;

	m_parent.SetConnection(
		net::connection_ptr(new net::connection(m_commandServer.GetIOService()))
	);

	while (true) {
		const FriendEntry& entry = participants[parentIndex];

		m_parent.SetHandle(entry.GetHandle());

		if (net::connect_to<ProtoTCP>(entry.GetEndpoint(), *m_parent.GetConnection())) {
			m_parent.Write(comm::encoder() << CRC_PEER_CONNECTION << m_myself.GetHandle());
			m_chatRoomDispatcher.ReadCommandFromLink(m_parent.GetConnection());
			return true;
		}

		if (++parentIndex == myOffset)
			++parentIndex;

		if (parentIndex >= participants.size())
			break;
	}
	DBGOUT << "connectTo: FAILED TO CONNECT to a PARENT NODE" << std::endl;
	return false;
}

//**********************************************************************

void ChatRoomService::sendTopology (void)
{
	const std::string data =
		comm::encoder() << CRC_JOIN_GET_TOPOLOGY << m_participants.GetFriends();

	ConnectionList::iterator iter = m_pendingAdditions.begin();
	for (; iter != m_pendingAdditions.end(); ++iter) {
		if (m_participants.FindFriend(iter->GetHandle()))
			iter->Write(data);
	}
	m_pendingAdditions.clear();
	this->createChatRoom(true);
}

//**********************************************************************

void ChatRoomService::closeAllConnections (void)
{
	if (m_parent)
		m_parent.Close();

	util::for_all(m_children, boost::mem_fn(&ConnEntry::Close));
	util::for_all(m_pendingAdditions, boost::mem_fn(&ConnEntry::Close));

	m_children.clear();
	m_pendingAdditions.clear();

	m_participants.ClearAllItems();
}

//**********************************************************************

void ChatRoomService::timerExpired (int32 timerId)
{
	if (m_connections != 0) {
		m_connections = 0;
		this->sendTopology();
	}
}

////////////////////////////////////////////////////////////////////////
// ConnEntry class
//
void ChatRoomService::ConnEntry::Reset (void)
{
	m_handle = String();

	if (m_conn) {
		m_conn->close();
		m_conn = net::connection_ptr();
	}
}

////////////////////////////////////////////////////////////////////////

} // namespace talkie
