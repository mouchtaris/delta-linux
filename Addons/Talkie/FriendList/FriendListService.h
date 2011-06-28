/**
 *	FriendListService.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef TALKIE_FRIENDLISTSERVICE_H
#define TALKIE_FRIENDLISTSERVICE_H

#include "Common.h"
#include "TalkieNetwork.h"
#include "FriendListMVC.h"
#include "ConnectionHybrid.h"

#include <boost/signal.hpp>

namespace talkie {

////////////////////////////////////////////////////////////////////////

class FriendListService {
public:
	typedef boost::signal<void (void)>					Sig1;
	typedef boost::signal<void (net::connection_ptr)>	Sig2;
	typedef boost::signal<void (const String&)>			Sig3;
	typedef boost::signal<void (const FriendEntry&)>	Sig4;

	FriendListService (boost::asio::io_service& service, uint16 port = 0);
	~FriendListService (void);

	void Start (void);
	void Stop (void);
	void Clear (void);

	const net::Endpoint GetEndpoint (void) const
		{ return net::Endpoint(m_commandServer.GetPort()); }

	//******************************************************************

	const FriendEntry&	GetUserInfo (void) const { return m_myEntry; }
	void				SetUserInfo (const FriendEntry& entry);

	void				SetStatus (FriendEntry::Status status);

	//******************************************************************

	FriendListModel&		GetFriendList (void) { return m_friendList; }
	const FriendListModel&	GetFriendList (void) const { return m_friendList; }

	bool ContainsFriend (const FriendEntry& entry) const;
	void AddFriend (const FriendEntry& entry);
	void RemoveFriend (const String& handle);

	//******************************************************************

	void NotifyOfPendingInvitation (const net::Endpoint& peer);
	void NotifyOfAuthorization (const net::Endpoint& peer);

	void AddPendingAuthorization (const String& handle);
	void RemovePendingAuthorization (const String& handle);

	//******************************************************************

	Sig1 sigPendingInvitations;
	Sig2 sigRequestChat;
	Sig3 sigAuthorized;
	Sig3 sigUnknownUser;
	Sig4 sigUserLoggedIn;

private:
	typedef net::GUIThreadHybridCommandServer<
		net::connection_udp, net::connection, net::connection_hybrid, int32
	> GTHybridCommandServer;

	bool cmdlogin (int32 command, net::connection_hybrid_ptr conn, comm::decoder& dec);
	bool cmdloginReply (int32 command, net::connection_hybrid_ptr conn, comm::decoder& dec);
	bool cmdCheckPending (int32 command, net::connection_hybrid_ptr conn, comm::decoder& dec);
	bool cmdAuthorized (int32 command, net::connection_hybrid_ptr conn, comm::decoder& dec);
	bool cmdJoinChatRoom (int32 command, net::connection_hybrid_ptr conn, comm::decoder& dec);
	bool cmdChangedStatus (int32 command, net::connection_hybrid_ptr conn, comm::decoder& dec);

	void sendData (const FriendEntry& entry, const std::string& data);
	void sendDataToEndpoint (const net::Endpoint& peer, const std::string& data);

	GTHybridCommandServer	m_commandServer;
	bool					m_started;

	FriendEntry				m_myEntry;
	FriendListModel			m_friendList;
	StringList				m_pendingAuthorizations;
};

////////////////////////////////////////////////////////////////////////

} // namespace talkie

#endif // TALKIE_FRIENDLISTSERVICE_H
