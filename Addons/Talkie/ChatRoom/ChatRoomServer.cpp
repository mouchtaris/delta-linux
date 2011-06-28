/**
 *	ChatRoomServer.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ChatRoomServer.h"
#include "Connection.h"
#include "NetUtils.h"
#include "Streams.h"
#include "Algorithms.h"
#include "Adaptors.h"

namespace talkie {

////////////////////////////////////////////////////////////////////////
// ChatRoomServer class
//
ChatRoomServer::ChatRoomServer (boost::asio::io_service& service) :
	GTCommandServer(service)
{
}

//**********************************************************************

ChatRoomServer::~ChatRoomServer (void)
{
}

////////////////////////////////////////////////////////////////////////

} // namespace talkie
