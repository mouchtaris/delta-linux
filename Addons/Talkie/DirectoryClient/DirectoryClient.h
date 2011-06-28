/**
 *	DirectoryClient.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef TALKIE_DIRECTORYCLIENT_H
#define TALKIE_DIRECTORYCLIENT_H

#include "Common.h"
#include "Endpoint.h"
#include "FriendEntry.h"
#include "TalkieNetwork.h"

#include <boost/function.hpp>
#include <boost/thread/recursive_mutex.hpp>

namespace talkie {

////////////////////////////////////////////////////////////////////////

class DirectoryClient {
public:
	typedef net::Endpoint		Endpoint;
	typedef std::list<Endpoint>	EndPointList;

	typedef boost::function<void (bool)>										Func1;
	typedef boost::function<void (bool, const FriendEntry&)>					Func2;
	typedef boost::function<void (const PendingOp&)>							Func3;
	typedef boost::function<void (bool, const String&, bool, const Endpoint&)>	Func4;
	typedef boost::function<void (bool, const String&)>							Func5;
	typedef boost::function<void (bool, const Endpoint&)>						Func6;

	DirectoryClient (boost::asio::io_service& service);
	~DirectoryClient (void);

	const String&	GetUserName (void) const { return m_userName; }
	void			SetUserName (const String& name) { m_userName = name; }

	const String&	GetPassword (void) const { return m_passwd; }
	void			SetPassword (const String& pass) { m_passwd = pass; }

	//******************************************************************

	void AddDirectory (const Endpoint& endPoint);
	void RemoveDirectory (const Endpoint& endPoint);

	//******************************************************************
	// Directory functions
	//
	void Register (const FriendEntry& personalInfo, const String& passwd);
	void Login (const Endpoint& local);
	void Logout (void);
	void GetUserInfo (const String& handle);
	void GetPendingOperations (void);

	void RequestAddFriend (const String& handle);
	void RequestRemoveFriend (const String& handle);

	void AuthorizeFriend (const String& handle, bool authorize);
	void RequestMoreServers (void);

	void SendOfflineMessage (const String& handle, const String& message);

	//******************************************************************

	void ClearFunctions (void);

	Func1 funcRegisterReply;
	Func2 funcLoginReply;
	Func3 funcPendingOperationReply;
	Func2 funcGetUserInfoReply;
	Func4 funcRequestAddFriendReply;
	Func5 funcRequestRemoveFriendReply;
	Func4 funcAuthorizeFriendReply;
	Func6 funcRequestMoreServersReply;
	Func5 funcSendOfflineMsgReply;

private:
	typedef std::pair<
		boost::function<void (void)>, boost::function<void (void)>
	> CommandInvoker;

	typedef std::list<CommandInvoker>	CommandInvokerList;

	//******************************************************************

	void doDirectoryCommand0 (DirectoryCommands command);

	template<class TArg>
	void doDirectoryCommand1 (DirectoryCommands command, CREF(TArg) arg);

	template<class TArg1, class TArg2>
	void doDirectoryCommand2 (DirectoryCommands command, CREF(TArg1) arg1, CREF(TArg2) arg2);

	//******************************************************************

	bool cmdRegisterReply (int32 command, net::connection_ptr conn, comm::decoder& dec);
	bool cmdLoginReply (int32 command, net::connection_ptr conn, comm::decoder& dec);
	bool cmdLogoutReply (int32 command, net::connection_ptr conn, comm::decoder& dec);
	bool cmdGetUserInfoReply (int32 command, net::connection_ptr conn, comm::decoder& dec);
	bool cmdGetOperationsReply (int32 command, net::connection_ptr conn, comm::decoder& dec);
	bool cmdRequestAddFriendReply (int32 command, net::connection_ptr conn, comm::decoder& dec);
	bool cmdRequestRemoveFriendReply (int32 command, net::connection_ptr conn, comm::decoder& dec);
	bool cmdAuthorizeFriendReply (int32 command, net::connection_ptr conn, comm::decoder& dec);
	bool cmdRequestMoreServersReply (int32 command, net::connection_ptr conn, comm::decoder& dec);
	bool cmdSendOfflineMsgReply (int32 command, net::connection_ptr conn, comm::decoder& dec);

	void connectionError (const boost::asio::error& error, net::connection_ptr conn);

	//******************************************************************

	const Endpoint	getFirstDirectory (void);
	const Endpoint	getNextDirectory (void);
	void			promoteCurrentDirectory (void);

	//******************************************************************

	void connectionExpired (const NetError& error);
	void connectionFinished (const NetError& error);
	void asyncConnect (const net::Endpoint& peer);
	bool connect (void);
	void disconnect (void);

	//******************************************************************

	void replyExpired (const NetError& error);
	void invokeCommand (const CommandInvoker& invoker);
	void processNextCommand (bool popFrontCommand = true);
	void clearAllCommands (void);

	//******************************************************************

	boost::asio::io_service&	m_service;
	bool						m_connected;
	String						m_userName;
	String						m_passwd;
	GTCommandDispatcher			m_dispatcher;

	net::connection_ptr			m_connection;
	EndPointList				m_endPoints;
	EndPointList::iterator		m_endPointIter;

	boost::asio::deadline_timer m_timer;
	CommandInvokerList			m_pendingCommands;
	boost::recursive_mutex		m_mutex;
};

////////////////////////////////////////////////////////////////////////

} // namespace talkie

#endif // TALKIE_DIRECTORYCLIENT_H
