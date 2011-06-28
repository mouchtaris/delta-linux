/**
 *	ChatRoomService.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef TALKIE_CHATROOMSERVICE_H
#define TALKIE_CHATROOMSERVICE_H

#include "Common.h"
#include "FriendListMVC.h"
#include "TalkieNetwork.h"

#include <boost/signal.hpp>

namespace net {
	class connection;
	class decoder;
}

namespace talkie {

////////////////////////////////////////////////////////////////////////

class ChatRoomService {
public:
	typedef boost::signal<void (const FriendEntry&, const String&)> Sig1;
	typedef boost::signal<void (void)>								Sig2;
	typedef boost::signal<void (const String&)>						Sig3;

	ChatRoomService (boost::asio::io_service& service, const FriendEntry& me);
	~ChatRoomService (void);

	//******************************************************************

	FriendListModel&		GetFriendListModel (void) { return m_participants; }
	const FriendListModel&	GetFriendListModel (void) const { return m_participants; }

	void AddParticipants (const FriendEntryVec& participants);
	void RemoveParticipant (const String& handle);

	void SendText (const String& message);
	void SendFile (const String& file);

	//******************************************************************

	void InitializeChatRoom (const FriendEntryVec& participants);
	void JoinChatRoom (net::connection_ptr conn);

	//******************************************************************
	// Signals
	//
	Sig1 sigNewMessage;
	Sig2 sigChatRoomCreated;
	Sig3 sigChatRoomError;

	Sig1 sigUserDisconnected;
	Sig1 sigUserConnected;

private:

	////////////////////////////////////////////////////////////////////

	class ConnEntry {
	public:
		ConnEntry (
			net::connection_ptr conn = net::connection_ptr(),
			const String&		handle = String()
		) : m_conn(conn), m_handle(handle) {}

		const String&		GetHandle (void) const { return m_handle; }
		net::connection_ptr	GetConnection (void) { return m_conn; }

		void SetHandle (const String& handle) { m_handle = handle; }
		void SetConnection (net::connection_ptr conn) { m_conn = conn; }

		void Write (const std::string& data) { m_conn->write(data); }
		void Close (void) { m_conn->close(); }
		void Reset (void);

		operator bool (void) { return m_conn; }

		bool operator== (const ConnEntry& entry) const { return m_conn == entry.m_conn; }

		bool operator== (const FriendEntry& entry) const { return entry == m_handle; }
		bool operator== (const String& handle) const { return m_handle == handle; }
		bool operator== (net::connection_ptr conn) const { return m_conn == conn; }

	private:
		net::connection_ptr	m_conn;
		String				m_handle;
	};

	////////////////////////////////////////////////////////////////////

	typedef std::list<ConnEntry> ConnectionList;

	////////////////////////////////////////////////////////////////////

	bool cmdNewConnection (int32 command, net::connection_ptr conn, comm::decoder& dec);
	bool cmdReadRemoteEntry (int32 command, net::connection_ptr conn, comm::decoder& dec);
	bool cmdReadTopology (int32 command, net::connection_ptr conn, comm::decoder& dec);

	//******************************************************************

	void sCmdConnectionError (const NetError& error, net::connection_ptr conn);

	bool sCmdFwdMessage (int32 command, net::connection_ptr conn, comm::decoder& dec);
	bool sCmdNewMessage (int32 command, net::connection_ptr conn, comm::decoder& dec);

	bool sCmdFwdPeerDisconnected (int32 command, net::connection_ptr conn, comm::decoder& dec);
	bool sCmdPeerDisconnected (int32 command, net::connection_ptr conn, comm::decoder& dec);

	bool sCmdFwdPeerConnected (int32 command, net::connection_ptr conn, comm::decoder& dec);
	bool sCmdPeerConnected (int32 command, net::connection_ptr conn, comm::decoder& dec);

	//******************************************************************

	void sendText (const String& handle, const String& message, bool fwd);
	void sendNodeDisconnected (const String& handle, bool fwd);
	void sendNodeConnected (const FriendEntry& entry, bool fwd);

	void createChatRoom (bool initializerNode);
	void generalChatRoomError (const String& reason);
	void disconnectNode (const String& handle);
	void connectNode (const FriendEntry& entry);

	//******************************************************************

	bool connectToParent (void);
	void sendTopology (void);
	void closeAllConnections (void);

	//******************************************************************

	void timerExpired (int32 timerId);

	//******************************************************************

	GTCommandServer		m_commandServer;
	GTCommandDispatcher	m_chatRoomDispatcher;

	FriendEntry			m_myself;
	FriendListModel		m_participants;

	uint				m_connections;
	bool				m_chatRoomOnline;
	ConnectionList		m_pendingAdditions;
	int32				m_timerId;

	ConnEntry			m_parent;
	ConnectionList		m_children;
};

////////////////////////////////////////////////////////////////////////

} // namespace talkie

#endif // TALKIE_CHATROOMSERVICE_H
