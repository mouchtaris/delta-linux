/**
 *	FriendListService.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "FriendListService.h"
#include "NetUtils.h"

namespace talkie {

////////////////////////////////////////////////////////////////////////
// FriendListService class
//
FriendListService::FriendListService (boost::asio::io_service& service, uint16 port) :
	m_commandServer(service, port), m_started(false)
{
	////////////////////////////////////////////////////////////////////
	// Initialize command server
	//
	m_commandServer.RegisterCommand(BC_LOGIN,
		boost::bind(&FriendListService::cmdlogin, this, _1, _2, _3));
	m_commandServer.RegisterCommand(BC_LOGIN_RESPONSE,
		boost::bind(&FriendListService::cmdloginReply, this, _1, _2, _3));

	m_commandServer.RegisterCommand(BC_CHECK_PENDING,
		boost::bind(&FriendListService::cmdCheckPending, this, _1, _2, _3));

	m_commandServer.RegisterCommand(BC_AUTHORIZED,
		boost::bind(&FriendListService::cmdAuthorized, this, _1, _2, _3));

	m_commandServer.RegisterCommand(BC_JOIN_CHATROOM,
		boost::bind(&FriendListService::cmdJoinChatRoom, this, _1, _2, _3));

	m_commandServer.RegisterCommand(BC_CHANGED_STATUS,
		boost::bind(&FriendListService::cmdChangedStatus, this, _1, _2, _3));
}

//**********************************************************************

FriendListService::~FriendListService (void)
{
}

//**********************************************************************

void FriendListService::Start (void)
{
	if (!m_started) {
		m_started = true;
		m_commandServer.Accept();
	}
}

//**********************************************************************

void FriendListService::Stop (void)
{
	if (m_started) {
		m_started = false;
		m_commandServer.Close();
	}
}

//**********************************************************************

void FriendListService::Clear (void)
{
	m_myEntry = FriendEntry();
	m_friendList.ClearAllItems();
}

//**********************************************************************

void FriendListService::SetUserInfo (const FriendEntry& entry)
{
	m_myEntry = entry;
	m_myEntry.SetStatus(FriendEntry::ONLINE);
}

//**********************************************************************

void FriendListService::SetStatus (FriendEntry::Status status)
{
	m_myEntry.SetStatus(status);

	const std::string data = comm::encoder() << BC_CHANGED_STATUS <<
		m_myEntry.GetHandle() << status;

	util::for_all_if(
		m_friendList.GetFriends(),
		boost::bind(&FriendListService::sendData, this, _1, boost::ref(data)),
		std::not1(FriendEntry::HasStatusPred(FriendEntry::OFFLINE))
	);
}

//**********************************************************************

bool FriendListService::ContainsFriend (const FriendEntry& entry) const
{
	return m_friendList.FindFriend(entry.GetHandle());
}

//**********************************************************************

void FriendListService::AddFriend (const FriendEntry& entry)
{
	m_friendList.AddFriend(entry);

	if (entry.GetStatus() != FriendEntry::OFFLINE) {
		const std::string data = comm::encoder() << BC_LOGIN << m_myEntry.GetHandle() <<
			m_myEntry.GetStatus() << m_commandServer.GetPort();
		this->sendData(entry, data);
	}
}

//**********************************************************************

void FriendListService::RemoveFriend (const String& handle)
{
	m_friendList.RemoveFriend(handle);
}

//**********************************************************************

void FriendListService::NotifyOfPendingInvitation (const net::Endpoint& peer)
{
	this->sendDataToEndpoint(peer,
		comm::encoder() << BC_CHECK_PENDING << m_myEntry.GetHandle());
}

//**********************************************************************

void FriendListService::NotifyOfAuthorization (const net::Endpoint& peer)
{
	this->sendDataToEndpoint(peer,
		comm::encoder() << BC_AUTHORIZED << m_myEntry.GetHandle());
}

//**********************************************************************

void FriendListService::AddPendingAuthorization (const String& handle)
	{ m_pendingAuthorizations.push_back(handle); }

void FriendListService::RemovePendingAuthorization (const String& handle)
	{ m_pendingAuthorizations.remove(handle); }

//**********************************************************************

bool FriendListService::cmdlogin (int32 command, net::connection_hybrid_ptr conn, comm::decoder& dec)
{
	String user;
	FriendEntry::Status status;
	uint16 port;
	dec >> user >> status >> port;

	const FriendEntry& loggedUser = m_friendList.FindFriend(user);

	if (!loggedUser)
		sigUnknownUser(user);
	else {

		net::Endpoint peer(util::std2str(conn->remote_address_str()), port);
		m_friendList.ChangeEndpoint(user, peer);
		sigUserLoggedIn(loggedUser);
	}
	conn->write(
		comm::encoder() << BC_LOGIN_RESPONSE << m_myEntry.GetHandle() << m_myEntry.GetStatus()
	);
	m_friendList.ChangeStatus(user, status);
	return true;
}

//**********************************************************************

bool FriendListService::cmdloginReply (int32 command, net::connection_hybrid_ptr conn, comm::decoder& dec)
{
	String user;
	FriendEntry::Status status;
	dec >> user >> status;
	m_friendList.ChangeStatus(user, status);
	return true;
}

//**********************************************************************

bool FriendListService::cmdCheckPending (int32 command, net::connection_hybrid_ptr conn, comm::decoder& dec)
{
	String user;
	dec >> user;
	sigPendingInvitations();
	return true;
}

//**********************************************************************

bool FriendListService::cmdAuthorized (int32 command, net::connection_hybrid_ptr conn, comm::decoder& dec)
{
	String user;
	dec >> user;

	StringList::iterator iter = std::find(
			m_pendingAuthorizations.begin(), m_pendingAuthorizations.end(), user
		);

	if (iter != m_pendingAuthorizations.end()) {

		m_pendingAuthorizations.erase(iter);
		sigAuthorized(user);
	}
	return true;
}

//**********************************************************************

bool FriendListService::cmdJoinChatRoom (int32 command, net::connection_hybrid_ptr conn, comm::decoder& dec)
{
	String user;
	dec >> user;

	sigRequestChat(conn->get_connection_ptr());
	return true;
}

//**********************************************************************

bool FriendListService::cmdChangedStatus (int32 command, net::connection_hybrid_ptr conn, comm::decoder& dec)
{
	String user;
	FriendEntry::Status status;
	dec >> user >> status;
	m_friendList.ChangeStatus(user, status);
	return true;
}

//**********************************************************************

void FriendListService::sendData (const FriendEntry& entry, const std::string& data)
	{ this->sendDataToEndpoint(entry.GetEndpoint(), data); }

//**********************************************************************

void FriendListService::sendDataToEndpoint (const net::Endpoint& peer, const std::string& data)
{
	typedef boost::asio::ip::udp Proto;

	net::connection_udp_ptr conn = m_commandServer.GetUDPConnection();
	Proto::endpoint endpoint = net::resolve<Proto>(peer, conn->socket().io_service());
	conn->write(data, endpoint);
}

////////////////////////////////////////////////////////////////////////

} // namespace talkie
