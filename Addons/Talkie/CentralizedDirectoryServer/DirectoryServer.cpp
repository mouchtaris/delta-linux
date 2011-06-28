/**
 *	DirectoryServer.cpp
 *
 *	-- Talkie Centralized Directory Server --
 *
 *	Centralized directory server of the talkie
 *	application retrieving.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#include "NetUtils.h"

#include "TalkieNetwork.h"
#include "FriendEntry.h"

#include "DirectoryServer.h"
#include "DirectoryEntry.h"
#include "StorageService.h"

#include <boost/serialization/tuple.hpp>

namespace talkie
{
	//-------------------------------------------------------//
	//---- class DirectoryServer ----------------------------//

	void ignore_reply(DirectoryServer::LinkPtrType client, bool success)
	{
	}

	//-----------------------------------------------------------------------

	template <class Tuple>
	void onPropagateResponse(DirectoryServer::LinkPtrType client, DirectoryCommands cmd,
		bool success, const Tuple& tuple)
	{
		if (success)
			client->write(encode_tuple(comm::encoder() << cmd << true, tuple));
		else
			client->write(comm::encoder() << cmd << false);
	}

	//-----------------------------------------------------------------------

	template <class Tuple>
	void onPropagateResponseAlt(DirectoryServer::LinkPtrType client, DirectoryCommands cmd,
		bool success, const Tuple& tuple)
	{
		if (success)
			client->write(encode_tuple(comm::encoder() << cmd << true, tuple));
		else
			client->write(comm::encoder() << cmd << false << boost::get<0>(tuple));
	}

	//-----------------------------------------------------------------------

	void onYouWillBeMyFriend(DirectoryServer::LinkPtrType client,
		DirectoryEntry& developer, const DirectoryEntry& user)
	{
		if (!developer)
			return;

		//-- Why can't we be friends???
		developer.friends.push_back(user.userInfo.GetHandle());

		//-- Update developer's entry in the DHT
		StorageService::Instance().Insert(
			developer,
			boost::bind(&ignore_reply, client, _1)
		);
	}

	//-----------------------------------------------------------------------

	void onRegister(
		DirectoryServer::LinkPtrType client, DirectoryEntry& invalid_user,
		const boost::tuple<String, String, FriendEntry>& args)	///< { username, password, userInfo }
	{
		//-- Check user's existence
		if (invalid_user)
		{
			client->write(comm::encoder() << DC_REGISTER_RESPONSE << false);
			return;
		}

		//-- Update user's record
		DirectoryEntry user(boost::get<2>(args), boost::get<1>(args));
		user.userInfo.SetStatus(FriendEntry::OFFLINE);

		//-- NOTE: we are friendly developers
		if (boost::get<0>(args) != _T("bourdenas"))
			user.friends.push_back(_T("bourdenas"));
		if (boost::get<0>(args) != _T("jgeorgal"))
			user.friends.push_back(_T("jgeorgal"));

		//-- Oh my! We have a new friend
		if (boost::get<0>(args) != _T("bourdenas") && boost::get<0>(args) != _T("jgeorgal"))
		{
			StorageService::Instance().Lookup(
				_T("bourdenas"),
				boost::bind(&onYouWillBeMyFriend, client, _1, user)
			);
			StorageService::Instance().Lookup(
				_T("jgeorgal"),
				boost::bind(&onYouWillBeMyFriend, client, _1, user)
			);
		}

		//-- Insert user's entry in the DHT
		StorageService::Instance().Insert(
			user,
			boost::bind(
				&onPropagateResponse< boost::tuple<boost::tuples::null_type> >,
				client, DC_REGISTER_RESPONSE, _1, boost::make_tuple()
			)
		);
	}

	//-----------------------------------------------------------------------

	void onLogin(
		DirectoryServer::LinkPtrType client, DirectoryEntry& user,
		const boost::tuple<String, String, uint16>& args)		///< { username, password, port }
	{
		//-- Check user's existence & password
		if (!user || user.passwd != boost::get<1>(args) ||
			user.userInfo.GetHandle() != boost::get<0>(args))
		{
			client->write(comm::encoder() << DC_LOGIN_RESPONSE << false);
			return;
		}

		//-- Update user's record
		user.userInfo.SetStatus(FriendEntry::UNDEFINED);
		user.userInfo.SetEndpoint(
			convert<ProtoTCP>(
				tcp_endpoint(
					client->socket().remote_endpoint().address(),
					boost::get<2>(args)
				)				
			)
		);

		//-- Update user's entry in the DHT
		StorageService::Instance().Insert(
			user,
			boost::bind(
				&onPropagateResponse< boost::tuple<FriendEntry, DirectoryEntry::FriendList> >,
				client, DC_LOGIN_RESPONSE, _1, boost::make_tuple(user.userInfo, user.friends)
			)
		);
	}

	//-----------------------------------------------------------------------

	void onLogout(
		DirectoryServer::LinkPtrType client, DirectoryEntry& user,
		const boost::tuple<String, String>& args)				///< { username, password }
	{
		//-- Check user's existence & password
		if (!user || user.passwd != boost::get<1>(args) ||
			user.userInfo.GetHandle() != boost::get<0>(args))
		{
			client->write(comm::encoder() << DC_LOGOUT_RESPONSE << false);
			return;
		}

		//-- Update user's record
		user.userInfo.SetStatus(FriendEntry::OFFLINE);

		//-- Update user's entry in the DHT
		StorageService::Instance().Insert(
			user,
			boost::bind(
				&onPropagateResponse< boost::tuple<boost::tuples::null_type> >,
				client, DC_LOGOUT_RESPONSE, _1, boost::make_tuple()
			)
		);
	}

	//-----------------------------------------------------------------------

	void onGetUserInfoStep(DirectoryServer::LinkPtrType client, const String& user_handle,
		const DirectoryEntry& userB)
	{
		//-- Send reply to the user
		if (!userB)
			client->write(comm::encoder() << DC_GET_USER_INFO_RESPONSE << false << user_handle);
		else
			client->write(comm::encoder() << DC_GET_USER_INFO_RESPONSE << true << userB.userInfo);
	}

	//-----------------------------------------------------------------------

	void onGetUserInfo(
		DirectoryServer::LinkPtrType client, DirectoryEntry& user,
		const boost::tuple<String, String, String>& args)		///< { username, password, userB }
	{
		//-- Check user's existence & password
		if (!user || user.passwd != boost::get<1>(args) ||
			user.userInfo.GetHandle() != boost::get<0>(args))
		{
			client->write(comm::encoder() << DC_GET_USER_INFO_RESPONSE << false << boost::get<2>(args));
			return;
		}

		//-- Check if requested user is a friend of the requester
		if (std::find(user.friends.begin(), user.friends.end(), boost::get<2>(args))
				== user.friends.end())
		{
			client->write(comm::encoder() << DC_GET_USER_INFO_RESPONSE << false << boost::get<2>(args));
			return;
		}

		//-- Retrieve requested user's entry from the DHT
		StorageService::Instance().Lookup(
			boost::get<2>(args),
			boost::bind(&onGetUserInfoStep, client, boost::get<2>(args), _1)
		);
	}
	
	//-----------------------------------------------------------------------

	void onGetPending(
		DirectoryServer::LinkPtrType client, DirectoryEntry& user,
		const boost::tuple<String, String>& args)				///< { username, password }
	{
		//-- Check user's existence & password
		if (!user || user.passwd != boost::get<1>(args) ||
			user.userInfo.GetHandle() != boost::get<0>(args))
		{
			client->write(comm::encoder() << DC_GET_PENDING_RESPONSE << false);
			return;
		}

		//-- Reply to the client
		client->write(comm::encoder() << DC_GET_PENDING_RESPONSE << true << user.pending);
	}

	//-----------------------------------------------------------------------

	void onAddFriendStep(DirectoryServer::LinkPtrType client, DirectoryEntry& userA,
		const String& userB_handle, DirectoryEntry& userB)
	{
		if (!userB) {
			client->write(comm::encoder() << DC_ADD_FRIEND_RESPONSE << false << userB_handle);
			return;
		}

		//-- Send response to user A
		bool immediate_addition =
			std::find(userB.friends.begin(), userB.friends.end(), userA.userInfo.GetHandle())
				!= userB.friends.end();
		Endpoint endpoint = (userB.userInfo.GetStatus() == FriendEntry::UNDEFINED)
			? userB.userInfo.GetEndpoint()
			: Endpoint();

		//-- Update userA & userB directory entries
		if (!immediate_addition)
		{
			userA.pending.push_back(boost::make_tuple(PO_INVITE, comm::encoder() << userB.userInfo.GetHandle()));
			userB.pending.push_back(boost::make_tuple(PO_AUTHORIZE, comm::encoder() << userA.userInfo.GetHandle()));
		}
		else
			userA.friends.push_back(userB.userInfo.GetHandle());

		//-- Update userA & userB entries in the DHT
		StorageService::Instance().Insert(
			userA,
			boost::bind(&ignore_reply, client, _1)
		);
		StorageService::Instance().Insert(
			userB,
			boost::bind(
				&onPropagateResponseAlt< boost::tuple<String, bool, Endpoint> >,
				client, DC_ADD_FRIEND_RESPONSE, _1,
				boost::make_tuple(userB_handle, immediate_addition, endpoint)
			)
		);
	}

	//-----------------------------------------------------------------------

	void onAddFriend(
		DirectoryServer::LinkPtrType client, DirectoryEntry& userA,
		const boost::tuple<String, String, String>& args)		///< { username, password, userB }
	{
		//-- Check user's existence & password
		if (!userA || userA.passwd != boost::get<1>(args) ||
			userA.userInfo.GetHandle() != boost::get<0>(args))
		{
			client->write(comm::encoder() << DC_ADD_FRIEND_RESPONSE << false << boost::get<2>(args));
			return;
		}

		//-- Check that userB is not already in friend list
		PendingOp pending_invitation = boost::make_tuple(PO_INVITE, comm::encoder() << boost::get<2>(args));
		if (boost::get<2>(args) == boost::get<0>(args)		||
			std::find(userA.friends.begin(), userA.friends.end(), boost::get<2>(args))
				!= userA.friends.end()						||
			std::find(userA.pending.begin(), userA.pending.end(), pending_invitation)
				!= userA.pending.end())
		{
			client->write(comm::encoder() << DC_ADD_FRIEND_RESPONSE << false
				<< boost::get<2>(args));
			return;
		}

		//-- Retrive invited userB's entry from the DHT
		StorageService::Instance().Lookup(
			boost::get<2>(args),
			boost::bind(&onAddFriendStep, client, userA, boost::get<2>(args), _1)
		);
	}

	//-----------------------------------------------------------------------

	void onRemoveFriend(
		DirectoryServer::LinkPtrType client, DirectoryEntry& user,
		const boost::tuple<String, String, String>& args)		///< { username, password, userB }
	{
		//-- Check user's existence & password & if requested user is a friend of the requester
		if (!user || user.passwd != boost::get<1>(args)			||
			user.userInfo.GetHandle() != boost::get<0>(args)	||
			std::find(user.friends.begin(), user.friends.end(), boost::get<2>(args))
				== user.friends.end())
		{
			client->write(comm::encoder() << DC_REMOVE_FRIEND_RESPONSE << false << boost::get<2>(args));
			return;
		}

		//-- Update user's record
		user.friends.remove(boost::get<2>(args));

		//-- Update user's record in the DHT
		StorageService::Instance().Insert(
			user,
			boost::bind(
				&onPropagateResponseAlt< boost::tuple<String> >,
				client, DC_REMOVE_FRIEND_RESPONSE, _1,
				boost::make_tuple(boost::get<2>(args))
			)
		);
	}

	//-----------------------------------------------------------------------

	void onAuthorizeFriendStep(DirectoryServer::LinkPtrType client, bool authorized,
		DirectoryEntry& userB, const String& userA_handle, DirectoryEntry& userA)
	{
		PendingOp invitation = boost::make_tuple(PO_INVITE, comm::encoder() << userB.userInfo.GetHandle());
		if (!userA	||
			std::find(userA.pending.begin(), userA.pending.end(), invitation)
			== userA.pending.end())
		{
			client->write(comm::encoder() << DC_AUTHORIZE_FRIEND_RESPONSE << false
				<< userA_handle << authorized);
			return;
		}

		//-- Remove invite request from user's pending list
		userA.pending.remove(invitation);

		//-- Update userA & userB friend lists
		if (authorized) {
			userA.friends.push_back(userB.userInfo.GetHandle());
			userB.friends.push_back(userA.userInfo.GetHandle());
		}

		//-- Update userA & userB entries in the DHT
		StorageService::Instance().Insert(
			userA,
			boost::bind(&ignore_reply, client, _1)
		);
		StorageService::Instance().Insert(
			userB,
			boost::bind(&ignore_reply, client, _1)
		);

		//-- Send reply to client
		client->write(comm::encoder() << DC_AUTHORIZE_FRIEND_RESPONSE << true
			<< userA_handle << authorized << userA.userInfo.GetEndpoint());
	}

	//-----------------------------------------------------------------------

	void onAuthorizeFriend(
		DirectoryServer::LinkPtrType client, DirectoryEntry& user,
		const boost::tuple<String, String, String, bool>& args)	///< { username, password, userA, authorize }
	{
		PendingOp authorization = boost::make_tuple(PO_AUTHORIZE, comm::encoder() << boost::get<2>(args));

		//-- Check user's existence & password & if requested user has asked for user's authorization
		if (!user || user.passwd != boost::get<1>(args)			||
			user.userInfo.GetHandle() != boost::get<0>(args)	||
			std::find(user.pending.begin(), user.pending.end(), authorization)
				== user.pending.end())
		{
			client->write(comm::encoder() << DC_AUTHORIZE_FRIEND_RESPONSE << false << boost::get<2>(args) << boost::get<3>(args));
			return;
		}

		//-- Remove authorization request from user's pending list
		user.pending.remove(authorization);

		//-- Retrive userA's entry from the DHT
		StorageService::Instance().Lookup(
			boost::get<2>(args),
			boost::bind(&onAuthorizeFriendStep, client, boost::get<3>(args), user,
				boost::get<2>(args), _1)
		);
	}

	//-----------------------------------------------------------------------

	void onIntroduceServers(
		DirectoryServer::LinkPtrType client, DirectoryEntry& user,
		const boost::tuple<String, String>& args)				///< { username, password }
	{
		//-- Check user's existence & password
		if (!user || user.passwd != boost::get<1>(args) ||
			user.userInfo.GetHandle() != boost::get<0>(args))
		{
			client->write(comm::encoder() << DC_INTRODUCE_SERVERS_RESPONSE << false);
			return;
		}

		//-- TODO: implement this
	}

	//-----------------------------------------------------------------------

	void onSendOfflineMsgStep(DirectoryServer::LinkPtrType client, 
		DirectoryEntry& userA, const String& msg, const String& userB_handle,
		DirectoryEntry& userB)
	{
		if (!userB	||
			std::find(userB.friends.begin(), userB.friends.end(), userA.userInfo.GetHandle())
			== userB.friends.end())
		{
			client->write(comm::encoder() << DC_SEND_OFFLINE_MSG << false << userB_handle);
			return;
		}

		//-- Update userB directory entry
		userB.pending.push_back(
			boost::make_tuple(PO_OFFLINE_MSG, comm::encoder() << userA.userInfo.GetHandle() << msg)
		);

		//-- Update userB entry in the DHT
		StorageService::Instance().Insert(
			userB,
			boost::bind(
				&onPropagateResponseAlt< boost::tuple<String> >,
				client, DC_SEND_OFFLINE_MSG, _1,
				boost::make_tuple(userB_handle)
			)
		);
	}

	//-----------------------------------------------------------------------

	void onSendOfflineMsg(
		DirectoryServer::LinkPtrType client, DirectoryEntry& user,
		const boost::tuple<String, String, String, String>& args)///< { username, password, userB, msg }
	{
		//-- Check user's existence & password
		if (!user || user.passwd != boost::get<1>(args) ||
			user.userInfo.GetHandle() != boost::get<0>(args))
		{
			client->write(comm::encoder() << DC_SEND_OFFLINE_MSG << false << boost::get<2>(args));
			return;
		}

		//-- Retrive userA's entry from the DHT
		StorageService::Instance().Lookup(
			boost::get<2>(args),
			boost::bind(&onSendOfflineMsgStep, client, user, boost::get<3>(args),
				boost::get<2>(args), _1)
		);
	}

	//-----------------------------------------------------------------------


	//-------------------------------------------------------//
	//---- class DirectoryServer ----------------------------//

	DirectoryServer::DirectoryServer(boost::asio::io_service& io_service, uint16 port)
		: TCPCommandServer<connection, int32>(io_service, port)
	{
		SetContinuousMonitor(true);		///< Set continuous mode true

		RegisterCommand(
			DC_REGISTER,
			boost::bind(
				&DirectoryServer::ResponseHandler< boost::tuple<String, String, FriendEntry> >,
				this, _2, _1, _3, onRegister
			)
		);
		RegisterCommand(
			DC_LOGIN,
			boost::bind(
				&DirectoryServer::ResponseHandler< boost::tuple<String, String, uint16> >,
				this, _2, _1, _3, onLogin
			)
		);
		RegisterCommand(
			DC_LOGOUT,
			boost::bind(
				&DirectoryServer::ResponseHandler< boost::tuple<String, String> >,
				this, _2, _1, _3, onLogout
			)
		);
		RegisterCommand(
			DC_GET_USER_INFO,
			boost::bind(
				&DirectoryServer::ResponseHandler< boost::tuple<String, String, String> >,
				this, _2, _1, _3, onGetUserInfo
			)
		);
		RegisterCommand(
			DC_GET_PENDING,
			boost::bind(
				&DirectoryServer::ResponseHandler< boost::tuple<String, String> >,
				this, _2, _1, _3, onGetPending
			)
		);
		RegisterCommand(
			DC_ADD_FRIEND,
			boost::bind(
				&DirectoryServer::ResponseHandler< boost::tuple<String, String, String> >,
				this, _2, _1, _3, onAddFriend
			)
		);
		RegisterCommand(
			DC_REMOVE_FRIEND,
			boost::bind(
				&DirectoryServer::ResponseHandler< boost::tuple<String, String, String> >,
				this, _2, _1, _3, onRemoveFriend
			)
		);
		RegisterCommand(
			DC_AUTHORIZE_FRIEND,
			boost::bind(
				&DirectoryServer::ResponseHandler< boost::tuple<String, String, String, bool> >,
				this, _2, _1, _3, onAuthorizeFriend
			)
		);
		RegisterCommand(
			DC_INTRODUCE_SERVERS,
			boost::bind(
				&DirectoryServer::ResponseHandler< boost::tuple<String, String> >,
				this, _2, _1, _3, onIntroduceServers
			)
		);
		RegisterCommand(
			DC_SEND_OFFLINE_MSG,
			boost::bind(
				&DirectoryServer::ResponseHandler< boost::tuple<String, String, String, String> >,
				this, _2, _1, _3, onSendOfflineMsg
			)
		);

		Accept();

		//-- Creates StroageService singleton and loads directory from disk
		StorageService::Instance();
	}

	//-----------------------------------------------------------------------

	DirectoryServer::~DirectoryServer(void)
	{
	}

	//-----------------------------------------------------------------------
}
