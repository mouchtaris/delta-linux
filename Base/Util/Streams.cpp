/**
 *	Streams.cpp
 *
 *	-- Utility Library --
 *
 *	December 2005
 */

#include "Streams.h"
#include "Algorithms.h"

#include <boost/thread/mutex.hpp>
#include <ctime>
#include <cstdio>

////////////////////////////////////////////////////////////////////////

#define NEW_SESSION_STR \
	"\n\n -------------------------- SESSION ------------------------------\n\n"

////////////////////////////////////////////////////////////////////////

namespace util {

////////////////////////////////////////////////////////////////////////
// Debug stream buffer
//
class dbgstreambuff : public std::streambuf
{
public:
	dbgstreambuff (void);
	~dbgstreambuff (void);

	void add_view (streamview* view) {
		boost::mutex::scoped_lock lock(m_mutex);
		m_views.push_back(view);
	}
	void remove_view (streamview* view) {
		boost::mutex::scoped_lock lock(m_mutex);
		m_views.remove(view);
	}

protected:
	static const size_t BUFFSIZE = 2048;
	char buffer[BUFFSIZE];

	virtual int overflow (int c);
	virtual int sync (void);

private:
	typedef std::list<streamview*> views_type;

	bool			m_triedToOpenFile;
	FILE*			m_outputFile;

	views_type		m_views;
	boost::mutex	m_mutex;
};

////////////////////////////////////////////////////////////////////////

dbgstreambuff::dbgstreambuff (void) : m_triedToOpenFile(false), m_outputFile(0)
{
	this->setbuf(buffer, BUFFSIZE);
	this->setp(buffer, buffer + BUFFSIZE);
}

//**********************************************************************

dbgstreambuff::~dbgstreambuff (void)
{
	// boost::mutex::scoped_lock lock(m_mutex); // No, no

	util::for_all(m_views, std::mem_fun(&streamview::close));

	if (m_outputFile)
		std::fclose(m_outputFile);
}

//**********************************************************************

int dbgstreambuff::overflow (int c)
{
	this->sync();
	assert(this->pbase() != epptr()); // Zero size buffer? WTF ?
	return this->sputc(c);
}

//**********************************************************************

int dbgstreambuff::sync (void)
{
	boost::mutex::scoped_lock lock(m_mutex);

	char* p = this->pbase();
	int pSize = (int)(this->pptr() - p);

	assert(pSize > 0);
	util::for_all(m_views, boost::bind(&streamview::display, _1, p, pSize));

	if (!m_triedToOpenFile) {
		m_triedToOpenFile = true;
		if ((m_outputFile = std::fopen("trace.log", "a")) != NULL)
			std::fwrite(NEW_SESSION_STR, 1, sizeof (NEW_SESSION_STR) - 1, m_outputFile);
	}
	if (m_outputFile) {
		std::fwrite(p, 1, pSize, m_outputFile);
		std::fflush(m_outputFile);
	}
	this->pbump(-pSize);
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Debug stream implementation
//
void dbgstream::add_view (streamview* view)
{
	dbgstreambuff* strbuff = static_cast<dbgstreambuff*>(this->rdbuf());
	strbuff->add_view(view);
}

//**********************************************************************

void dbgstream::remove_view (streamview* view)
{
	dbgstreambuff* strbuff = static_cast<dbgstreambuff*>(this->rdbuf());
	strbuff->remove_view(view);
}

////////////////////////////////////////////////////////////////////////
// Stream initializer
//
streamsinitializer::streamsinitializer (void)
{
	if (s_counter++ == 0) { // Initialize
		s_dbg = new dbgstream(new dbgstreambuff);
	}
}

//**********************************************************************

streamsinitializer::~streamsinitializer (void)
{
	if (--s_counter == 0) { // CleanUp

		assert(s_dbg);

		std::streambuf* streamBuff = s_dbg->rdbuf();
		s_dbg->rdbuf(0);

		delete streamBuff;
		delete s_dbg;
		s_dbg = 0;
	}
}

//**********************************************************************

dbgstream& streamsinitializer::get_stream (void) { return *s_dbg; }

//**********************************************************************

dbgstream*	streamsinitializer::s_dbg = 0;
uint		streamsinitializer::s_counter = 0;

////////////////////////////////////////////////////////////////////////

/*
#pragma warning(disable:4073)
#pragma init_seg(lib)

static dbgstreambuff dbgbuff;
static dbgstreambuff logbuff;

dbgstream dbg(&dbgbuff);
logstream log(&logbuff);
*/

////////////////////////////////////////////////////////////////////////

} // namespace util
