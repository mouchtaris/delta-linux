/**
 *	ChatRoomServer.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef TALKIE_CHATROOMSERVICE_H
#define TALKIE_CHATROOMSERVICE_H

#include "Common.h"
#include "TalkieNetwork.h"

#include <boost/tuple/tuple.hpp>

namespace talkie {

////////////////////////////////////////////////////////////////////////

class ChatRoomServer : protected GTCommandServer {
public:
	ChatRoomServer (boost::asio::io_service& service);
	~ChatRoomServer (void);

	//******************************************************************


private:
	typedef boost::tuple<net::connection_ptr, String> ConnEntry;

	typedef std::list<ConnEntry> ConnPtrList;
	ConnPtrList m_connections;
};

////////////////////////////////////////////////////////////////////////

} // namespace talkie

#endif // TALKIE_CHATROOMSERVICE_H
