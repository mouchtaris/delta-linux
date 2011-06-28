/**
 *	TalkieNetwork.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef TALKIE_TALKIENETWORK_H
#define TALKIE_TALKIENETWORK_H

#include "Common.h"
#include "GUIThreadCommandServer.h"
#include "Connection.h"
#include "UndefineAllWin32Shit.h"

namespace talkie {

////////////////////////////////////////////////////////////////////////

enum BaseCommands {
	BC_PING,			// { String handle, FriendEntry::Status status }
	BC_PING_RESPONSE,	// { String handle, FriendEntry::Status status }

	BC_LOGIN,			// { String handle, FriendEntry::Status status, uint16 port }
	BC_LOGIN_RESPONSE,	// { String handle, FriendEntry::Status status }

	BC_CHANGED_STATUS,	// { String handle, FriendEntry::Status status }
	BC_CHECK_PENDING,	// { String handle }
	BC_AUTHORIZED,		// { String handle }
	BC_JOIN_CHATROOM,	// { String handle }
};

////////////////////////////////////////////////////////////////////////

enum DirectoryCommands {
	DC_REGISTER,					// { String username, String password, FriendEntry userInfo }
	DC_REGISTER_RESPONSE,			// { bool success }

	DC_LOGIN,						// { String username, String password, uint16 port }
	DC_LOGIN_RESPONSE,				// { bool success, FriendEntry userInfo, list<String> friends }

	DC_LOGOUT,						// { String username, String password }
	DC_LOGOUT_RESPONSE,				// { bool success }

	DC_GET_USER_INFO,				// { String username, String password, String userB }
	DC_GET_USER_INFO_RESPONSE,		// { bool success, FriendEntry userB_entry } or on failure { bool success, String userB }

	DC_GET_PENDING,					// { String username, String password }
	DC_GET_PENDING_RESPONSE,		// { bool success, list< tuple<operation, strbuf> > pending_requests }

	DC_ADD_FRIEND,					// { String username, String password, String userB }
	DC_ADD_FRIEND_RESPONSE,			// { bool success, String userB, bool immediate_addition, Endpoint userB }

	DC_REMOVE_FRIEND,				// { String username, String password, String userB }
	DC_REMOVE_FRIEND_RESPONSE,		// { bool success, String userB }

	DC_AUTHORIZE_FRIEND,			// { String username, String password, String userA, bool authorize }
	DC_AUTHORIZE_FRIEND_RESPONSE,	// { bool success, String userA, bool authorize, Endpoint userA_endpoint }

	DC_INTRODUCE_SERVERS,			// { String username, String password }
	DC_INTRODUCE_SERVERS_RESPONSE,	// { bool success, list<Endpoint> directoryServers }

	DC_SEND_OFFLINE_MSG,			// { String username, String password, String userB, String message }
	DC_SEND_OFFLINE_MSG_RESPONSE,	// { bool success, String userB }
};

////////////////////////////////////////////////////////////////////////

enum PendingOperations {
	PO_INVITE,						// { String userB }
	PO_AUTHORIZE,					// { String userA }
	PO_OFFLINE_MSG,					// { String userA, String msg }
};

//***********************************************************************

typedef boost::tuple<PendingOperations, std::string>	PendingOp;
typedef std::list<PendingOp>							PendingOperationList;

////////////////////////////////////////////////////////////////////////

typedef boost::asio::ip::tcp									ProtoTCP;
typedef boost::asio::error										NetError;

typedef net::GUIThreadTCPCommandServer<net::connection, int32>	GTCommandServer;
typedef net::GUIThreadCommandDispatcher<net::connection, int32> GTCommandDispatcher;

////////////////////////////////////////////////////////////////////////

} // namespace talkie

#endif // TALKIE_TALKIENETWORK_H
