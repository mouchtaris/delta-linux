/**
 *	IOServiceThread.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "IOServiceThread.h"
#include "Streams.h"

#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>

namespace net {

////////////////////////////////////////////////////////////////////////
// IOServiceThread class
//
IOServiceThread::IOServiceThread (void) :
	m_thread(0), m_running(false)
{
}

//**********************************************************************

IOServiceThread::~IOServiceThread (void)
{
	this->Interrupt();
}

//**********************************************************************

void IOServiceThread::Start (void)
{
	if (m_running)
		return;

	assert(!m_thread);
	m_running = true;
	m_thread = new boost::thread(boost::bind(&IOServiceThread::serviceLoop, this));
}

//**********************************************************************

void IOServiceThread::Stop (void)
{
	if (!m_running)
		return;

	assert(m_thread);
	m_running = false;
	m_thread->join();
	delete m_thread;
	m_thread = 0;
}

//**********************************************************************

void IOServiceThread::Interrupt (void)
{
	if (!m_running)
		return;

	BOOST_ASIO_IO_SERVICE_INTERRUPT(m_service);
	this->Stop();
}

//**********************************************************************

void IOServiceThread::serviceLoop (void)
{
	DBGOUT << "IOServiceThread: Service loop started..." << DBGENDL;

	while (m_running) {
		// DBGOUT << "IOServiceThread: invoking io_service::run" << std::endl;
		m_service.run();
		m_service.reset();

		boost::xtime xt;
		boost::xtime_get(&xt, boost::TIME_UTC);
		xt.sec += 2;
		boost::thread::sleep(xt);

		// DBGOUT << "IOServiceThread: io_service::run returned" << std::endl;
	}

	DBGOUT << "IOServiceThread: Service loop stopped..." << DBGENDL;
}

////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(IOServiceThreadSingleton);

////////////////////////////////////////////////////////////////////////

} // namespace net
