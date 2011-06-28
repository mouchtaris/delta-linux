/**
 *	DirectoryClient.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "DirectoryClient.h"
#include "Algorithms.h"
#include "NetUtils.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace talkie {

////////////////////////////////////////////////////////////////////////
// DirectoryClient class
//
DirectoryClient::DirectoryClient (boost::asio::io_service& service) :
	m_service(service), m_connected(false), m_timer(service)
{
	////////////////////////////////////////////////////////////////////
	// Configure dispatcher
	//
	m_dispatcher.SetContinuousMonitor();

	m_dispatcher.SetLinkErrorHandler(
		boost::bind(&DirectoryClient::connectionError, this, _1, _2)
	);

	m_dispatcher.RegisterCommand(DC_REGISTER_RESPONSE,
		boost::bind(&DirectoryClient::cmdRegisterReply, this, _1, _2, _3));

	m_dispatcher.RegisterCommand(DC_LOGIN_RESPONSE,
		boost::bind(&DirectoryClient::cmdLoginReply, this, _1, _2, _3));

	m_dispatcher.RegisterCommand(DC_LOGOUT_RESPONSE,
		boost::bind(&DirectoryClient::cmdLogoutReply, this, _1, _2, _3));

	m_dispatcher.RegisterCommand(DC_GET_USER_INFO_RESPONSE,
		boost::bind(&DirectoryClient::cmdGetUserInfoReply, this, _1, _2, _3));

	m_dispatcher.RegisterCommand(DC_GET_PENDING_RESPONSE,
		boost::bind(&DirectoryClient::cmdGetOperationsReply, this, _1, _2, _3));

	m_dispatcher.RegisterCommand(DC_ADD_FRIEND_RESPONSE,
		boost::bind(&DirectoryClient::cmdRequestAddFriendReply, this, _1, _2, _3));

	m_dispatcher.RegisterCommand(DC_REMOVE_FRIEND_RESPONSE,
		boost::bind(&DirectoryClient::cmdRequestRemoveFriendReply, this, _1, _2, _3));

	m_dispatcher.RegisterCommand(DC_AUTHORIZE_FRIEND_RESPONSE,
		boost::bind(&DirectoryClient::cmdAuthorizeFriendReply, this, _1, _2, _3));

	m_dispatcher.RegisterCommand(DC_INTRODUCE_SERVERS_RESPONSE,
		boost::bind(&DirectoryClient::cmdRequestMoreServersReply, this, _1, _2, _3));

	m_dispatcher.RegisterCommand(DC_SEND_OFFLINE_MSG_RESPONSE,
		boost::bind(&DirectoryClient::cmdSendOfflineMsgReply, this, _1, _2, _3));

	////////////////////////////////////////////////////////////////////
	// Add harcoded directory servers
	//
	this->AddDirectory(Endpoint(_T("139.91.186.47"), 2123));
	this->AddDirectory(Endpoint(_T("139.91.186.39"), 2123));
	this->AddDirectory(Endpoint(_T("139.91.186.34"), 2123));
}

//**********************************************************************

DirectoryClient::~DirectoryClient (void)
{
}

//**********************************************************************

void DirectoryClient::AddDirectory (const Endpoint& endPoint)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);
	m_endPoints.push_front(endPoint);
	m_endPointIter = m_endPoints.begin();
}

//**********************************************************************

void DirectoryClient::RemoveDirectory (const Endpoint& endPoint)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);
	m_endPoints.remove(endPoint);
	m_endPointIter = m_endPoints.begin();
}

//**********************************************************************

void DirectoryClient::Register (const FriendEntry& personalInfo, const String& passwd)
{
	m_userName = personalInfo.GetHandle();
	m_passwd = passwd;

	CommandInvoker invoker(
		boost::bind(&DirectoryClient::doDirectoryCommand1<FriendEntry>,
			this, DC_REGISTER, personalInfo),
		boost::bind(boost::ref(funcRegisterReply), false)
	);
	this->invokeCommand(invoker);
}

//**********************************************************************

void DirectoryClient::Login (const Endpoint& local)
{
	CommandInvoker invoker(
		boost::bind(&DirectoryClient::doDirectoryCommand1<uint16>,
			this, DC_LOGIN, local.GetPortNum()),
		boost::bind(boost::ref(funcLoginReply), false, FriendEntry())
	);
	this->invokeCommand(invoker);
}

//**********************************************************************

void DirectoryClient::Logout (void)
{
	this->clearAllCommands();

	CommandInvoker invoker(
		boost::bind(&DirectoryClient::doDirectoryCommand0, this, DC_LOGOUT),
		boost::function<void (void)>()
	);
	this->invokeCommand(invoker);
}

//**********************************************************************

void DirectoryClient::GetUserInfo (const String& handle)
{
	CommandInvoker invoker(
		boost::bind(&DirectoryClient::doDirectoryCommand1<String>,
			this, DC_GET_USER_INFO, handle),
		boost::bind(boost::ref(funcGetUserInfoReply), false, FriendEntry())
	);
	this->invokeCommand(invoker);
}

//**********************************************************************

void DirectoryClient::GetPendingOperations (void)
{
	CommandInvoker invoker(
		boost::bind(&DirectoryClient::doDirectoryCommand0, this, DC_GET_PENDING),
		boost::function<void (void)>()
	);
	this->invokeCommand(invoker);
}

//**********************************************************************

void DirectoryClient::RequestAddFriend (const String& handle)
{
	CommandInvoker invoker(
		boost::bind(&DirectoryClient::doDirectoryCommand1<String>,
			this, DC_ADD_FRIEND, handle),
		boost::bind(boost::ref(funcRequestAddFriendReply), false, String(), false, Endpoint())
	);
	this->invokeCommand(invoker);
}

//**********************************************************************

void DirectoryClient::RequestRemoveFriend (const String& handle)
{
	CommandInvoker invoker(
		boost::bind(&DirectoryClient::doDirectoryCommand1<String>,
			this, DC_REMOVE_FRIEND, handle),
		boost::bind(boost::ref(funcRequestRemoveFriendReply), false, String())
	);
	this->invokeCommand(invoker);
}

//**********************************************************************

void DirectoryClient::AuthorizeFriend (const String& handle, bool authorize)
{
	CommandInvoker invoker(
		boost::bind(&DirectoryClient::doDirectoryCommand2<String, bool>,
			this, DC_AUTHORIZE_FRIEND, handle, authorize),
		boost::bind(boost::ref(funcAuthorizeFriendReply), false, String(), false, Endpoint())
	);
	this->invokeCommand(invoker);
}

//**********************************************************************

void DirectoryClient::RequestMoreServers (void)
{
	CommandInvoker invoker(
		boost::bind(&DirectoryClient::doDirectoryCommand0, this, DC_INTRODUCE_SERVERS),
		boost::bind(boost::ref(funcRequestMoreServersReply), false, Endpoint())
	);
	this->invokeCommand(invoker);
}

//**********************************************************************

void DirectoryClient::SendOfflineMessage (const String& handle, const String& message)
{
	CommandInvoker invoker(
		boost::bind(&DirectoryClient::doDirectoryCommand2<String, String>,
			this, DC_SEND_OFFLINE_MSG, handle, message),
		boost::bind(boost::ref(funcSendOfflineMsgReply), false, String())
	);
	this->invokeCommand(invoker);
}

//**********************************************************************

void DirectoryClient::ClearFunctions (void)
{
	this->clearAllCommands();

	funcRegisterReply.clear();
	funcLoginReply.clear();
	funcPendingOperationReply.clear();
	funcGetUserInfoReply.clear();
	funcRequestAddFriendReply.clear();
	funcRequestRemoveFriendReply.clear();
	funcAuthorizeFriendReply.clear();
	funcRequestMoreServersReply.clear();
	funcSendOfflineMsgReply.clear();
}

//**********************************************************************
// Private members
//
void DirectoryClient::doDirectoryCommand0 (DirectoryCommands command)
	{ m_connection->write(comm::encoder() << command << m_userName << m_passwd); }

template<class TArg>
void DirectoryClient::doDirectoryCommand1 (DirectoryCommands command, CREF(TArg) arg)
	{ m_connection->write(comm::encoder() << command << m_userName << m_passwd << arg); }

template<class TArg1, class TArg2>
void DirectoryClient::doDirectoryCommand2 (
		DirectoryCommands command, CREF(TArg1) arg1, CREF(TArg2) arg2
	) { m_connection->write(comm::encoder() << command << m_userName << m_passwd << arg1 << arg2); }

//**********************************************************************

bool DirectoryClient::cmdRegisterReply (
		int32 command, net::connection_ptr conn, comm::decoder& dec
	)
{
	this->processNextCommand();

	bool result;
	dec >> result;

	if (funcRegisterReply)
		funcRegisterReply(result);
	return true;
}

//**********************************************************************

bool DirectoryClient::cmdLoginReply (
		int32 command, net::connection_ptr conn, comm::decoder& dec
	)
{
	bool result;
	FriendEntry userInfo;
	StringList friends;

	dec >> result;

	if (result)
		dec >> userInfo >> friends;

	util::for_all(friends, boost::bind(&DirectoryClient::GetUserInfo, this, _1));
	this->processNextCommand();

	if (funcLoginReply)
		funcLoginReply(result, userInfo);
	return true;
}

//**********************************************************************

bool DirectoryClient::cmdLogoutReply (
		int32 command, net::connection_ptr conn, comm::decoder& dec
	)
{
	bool result;
	dec >> result;
	this->processNextCommand();
	return true;
}

//**********************************************************************

bool DirectoryClient::cmdGetUserInfoReply (
		int32 command, net::connection_ptr conn, comm::decoder& dec
	)
{
	this->processNextCommand();

	bool result;
	FriendEntry userInfo;
	dec >> result;

	if (result)
		dec >> userInfo;

	if (funcGetUserInfoReply)
		funcGetUserInfoReply(result, userInfo);
	return true;
}

//**********************************************************************

bool DirectoryClient::cmdGetOperationsReply (
		int32 command, net::connection_ptr conn, comm::decoder& dec
	)
{
	this->processNextCommand();

	bool result;
	PendingOperationList operations;
	dec >> result;

	if (result)
		dec >> operations;

	if (funcPendingOperationReply)
		util::for_all(operations, boost::bind(boost::ref(funcPendingOperationReply), _1));
	return true;
}

//**********************************************************************

bool DirectoryClient::cmdRequestAddFriendReply (
		int32 command, net::connection_ptr conn, comm::decoder& dec
	)
{
	this->processNextCommand();

	bool result, imediateAddition = false;
	Endpoint p;
	String handle;
	dec >> result >> handle;

	if (result)
		dec >> imediateAddition >> p;

	if (funcRequestAddFriendReply)
		funcRequestAddFriendReply(result, handle, imediateAddition, p);
	return true;
}

//**********************************************************************

bool DirectoryClient::cmdRequestRemoveFriendReply (
		int32 command, net::connection_ptr conn, comm::decoder& dec
	)
{
	this->processNextCommand();

	bool result;
	String handle;
	dec >> result;

	if (result)
		dec >> handle;

	if (funcRequestRemoveFriendReply)
		funcRequestRemoveFriendReply(result, handle);
	return true;
}

//**********************************************************************

bool DirectoryClient::cmdAuthorizeFriendReply (
		int32 command, net::connection_ptr conn, comm::decoder& dec
	)
{
	this->processNextCommand();

	bool result, authorize = false;
	String handle;
	Endpoint peer;
	dec >> result >> handle;

	if (result)
		dec >> authorize >> peer;

	if (funcAuthorizeFriendReply)
		funcAuthorizeFriendReply(result, handle, authorize, peer);
	return true;
}

//**********************************************************************

bool DirectoryClient::cmdRequestMoreServersReply (
		int32 command, net::connection_ptr conn, comm::decoder& dec
	)
{
	this->processNextCommand();

	bool result;
	Endpoint p;
	dec >> result;

	if (result) {
		dec >> p;
		this->AddDirectory(p);
	}

	if (funcRequestMoreServersReply)
		funcRequestMoreServersReply(result, p);
	return true;
}

//**********************************************************************

bool DirectoryClient::cmdSendOfflineMsgReply (
		int32 command, net::connection_ptr conn, comm::decoder& dec
	)
{
	this->processNextCommand();

	bool result;
	String handle;
	dec >> result;

	if (result)
		dec >> handle;

	if (funcSendOfflineMsgReply)
		funcSendOfflineMsgReply(result, handle);
	return true;
}

//**********************************************************************

void DirectoryClient::connectionError (const boost::asio::error& error, net::connection_ptr conn)
{
	DBGOUT << "connectionError: " << error << std::endl;
	if (m_connected) {
		m_connected = false;
		this->clearAllCommands();
	}
}

//**********************************************************************

const DirectoryClient::Endpoint DirectoryClient::getFirstDirectory (void)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);
	assert(!m_endPoints.empty());
	m_endPointIter = m_endPoints.begin();
	return *m_endPointIter;
}

const DirectoryClient::Endpoint DirectoryClient::getNextDirectory (void)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);
	return (++m_endPointIter != m_endPoints.end()) ? *m_endPointIter : Endpoint();
}

void DirectoryClient::promoteCurrentDirectory (void)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);
	assert(m_endPointIter != m_endPoints.end());
	Endpoint p = *m_endPointIter;
	m_endPoints.erase(m_endPointIter);
	m_endPoints.push_front(p);
	m_endPointIter = m_endPoints.begin();
}

//**********************************************************************

void DirectoryClient::connectionExpired (const NetError& error)
{
	if (!error)
		m_connection->close();
}

//**********************************************************************

void DirectoryClient::connectionFinished (const NetError& error)
{
	m_timer.cancel();

	if (!error) {

		m_connected = true;
		this->promoteCurrentDirectory();
		m_dispatcher.ReadCommandFromLink(m_connection);
		this->processNextCommand(false);
	}
	else {
		const Endpoint peer = this->getNextDirectory();

		if (peer)
			this->asyncConnect(peer);
		else
			this->clearAllCommands();
	}
}

//**********************************************************************

void DirectoryClient::asyncConnect (const Endpoint& peer)
{
	assert(peer);
	assert(!m_connected);

	if (!m_connection)
		m_connection = net::connection_ptr(new net::connection(m_service));

	m_timer.expires_from_now(boost::posix_time::seconds(7));
	m_timer.async_wait(boost::bind(&DirectoryClient::connectionExpired, this, _1));

	net::async_connect_to<ProtoTCP>(peer, *m_connection,
		boost::bind(&DirectoryClient::connectionFinished, this, _1));
}

//**********************************************************************

bool DirectoryClient::connect (void)
{
	if (m_connected)
		return true;

	if (!m_connection)
		m_connection = net::connection_ptr(new net::connection(m_service));

	boost::asio::error error = boost::asio::error::host_not_found;
	Endpoint peer = this->getFirstDirectory();
	do {
		if (net::connect_to<ProtoTCP>(peer, *m_connection)) {
			m_connected = true;
			m_dispatcher.ReadCommandFromLink(m_connection);
			this->promoteCurrentDirectory();
			return true;
		}
	} while (peer = this->getNextDirectory());

	return false;
}

//**********************************************************************

void DirectoryClient::disconnect (void)
{
	if (m_connected) {
		m_connected = false;
		m_connection->close();
	}
}

//**********************************************************************

void DirectoryClient::replyExpired (const NetError& error)
{
	if (!error) {
		this->clearAllCommands();
		this->disconnect();
	}
}

//**********************************************************************

void DirectoryClient::invokeCommand (const CommandInvoker& invoker)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);

	bool hasPendingCommands = !m_pendingCommands.empty();
	m_pendingCommands.push_back(invoker);

	if (!m_connected) {

		if (!hasPendingCommands)
			this->asyncConnect(this->getFirstDirectory());
	}
	else if (!hasPendingCommands)
		this->processNextCommand(false);
}

//**********************************************************************

void DirectoryClient::processNextCommand (bool popFrontCommand)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);

	m_timer.cancel();

	if (popFrontCommand && !m_pendingCommands.empty())
		m_pendingCommands.pop_front();

	if (!m_connected) {
		// Some serious error occured or the io_service stoped running
		this->clearAllCommands();
	}
	else if (!m_pendingCommands.empty()) {

		m_pendingCommands.front().first();

		m_timer.expires_from_now(boost::posix_time::seconds(10));
		m_timer.async_wait(boost::bind(&DirectoryClient::replyExpired, this, _1));
	}
	else
		this->disconnect();
}

//**********************************************************************

void DirectoryClient::clearAllCommands (void)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);

	CommandInvokerList::iterator iter = m_pendingCommands.begin();
	for (; iter != m_pendingCommands.end(); ++iter)
		if (iter->second)
			iter->second();

	m_pendingCommands.clear();
}

////////////////////////////////////////////////////////////////////////

} // namespace talkie
