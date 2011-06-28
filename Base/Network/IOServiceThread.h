/**
 *	IOServiceThread.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef NET_IOSERVICETHREAD_H
#define NET_IOSERVICETHREAD_H

#include "Common.h"
#include "LibraryAPIDecls.h"
#include "Singleton.h"

#pragma warning(disable: 4267)

#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>

namespace net {

////////////////////////////////////////////////////////////////////////

class _BASE_API IOServiceThread {
public:
	IOServiceThread (void);
	~IOServiceThread (void);

	void Start (void);
	void Stop (void);		// Blocks like hell
	void Interrupt (void);	// Blocks for a little while

	bool IsRunning (void) const { return m_running; }

	boost::asio::io_service& GetIOService (void) { return m_service; }
	operator boost::asio::io_service& (void) { return m_service; }

private:
	DISABLE_COPY(IOServiceThread);

	void serviceLoop (void);

	boost::asio::io_service		m_service;
	boost::thread*				m_thread;	// Ptr because function only at constructor
	volatile bool				m_running;
};

////////////////////////////////////////////////////////////////////////

class _BASE_API IOServiceThreadSingleton : public util::Singleton, public IOServiceThread {
	DECLARE_SINGLETON(IOServiceThreadSingleton);
};

////////////////////////////////////////////////////////////////////////

} // namespace net

#endif // NET_IOSERVICETHREAD_H
