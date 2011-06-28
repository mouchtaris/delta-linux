/**
 *	ConnectionManager.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef NET_CONNECTIONMANAGER_H
#define NET_CONNECTIONMANAGER_H

#include "Common.h"
#include "Connection.h"

#include <boost/noncopyable.hpp>

namespace net {

////////////////////////////////////////////////////////////////////////

template<class TConnPtr, class TConnData>
class ConnectionManager : private boost::noncopyable {
public:
	typedef TConnPtr	ConnectionPtrType;
	typedef TConnData	ConnectionDataType;

	ConnectionManager (void) {}
	~ConnectionManager (void) {}

	void Manage (ConnectionPtrType conn, PARAM(ConnectionDataType) data)
	{
	
	}

	void UnManage (ConnectionPtrType conn)
	{
	
	}
	
private:
	std::list<ConnectionPtrType> ConnPtrList;
	ConnPtrList m_connections;
};

////////////////////////////////////////////////////////////////////////

} // namespace net

#endif // NET_CONNECTIONMANAGER_H
