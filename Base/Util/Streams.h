/**
 *	Streams.h
 *
 *	-- Utility Library --
 *
 *	December 2005
 */

#ifndef UTIL_STREAMS_H
#define UTIL_STREAMS_H

#include "Common.h"
#include "LibraryAPIDecls.h"

#include <iostream>
#include <fstream>
#include <strstream>
#include <iomanip>

namespace util {

////////////////////////////////////////////////////////////////////////
// Stream view class
//
struct _BASE_API streamview {
	virtual void display (const char* buffer, size_t len) = 0;
	virtual void close (void) = 0;
};

////////////////////////////////////////////////////////////////////////
// Null stream
//
class _BASE_API nullstream {
public:
	nullstream (void) {}
	~nullstream (void) {}

	template<class T>
	nullstream const& operator<< (const T&) const { return *this; }

	// This crashes MSC_VER == 1600
//	template <typename _Elem, typename _Traits>
//	nullstream const& operator<< (std::ostream<_Elem, _Traits>& (*) (std::ostream<_Elem, _Traits>& _Ostr)) const { return *this; }

//	nullstream const& operator<< (std::ios& (*pfn)(std::ios&)) const { return *this; }
//	nullstream const& operator<< (std::ostream& (*pfn)(std::ostream&)) const { return *this; }
};

////////////////////////////////////////////////////////////////////////
// Debug stream
//
class _BASE_API dbgstream : public std::ostream {
public:
	dbgstream (std::streambuf* buff) : std::ostream(buff) {}

	void add_view (streamview* view);
	void remove_view (streamview* view);
};

////////////////////////////////////////////////////////////////////////
// Schwarz initializer for streams
//
class _BASE_API streamsinitializer {
public:
	streamsinitializer (void);
	~streamsinitializer (void);

	static dbgstream& get_stream (void);

private:
	static dbgstream*	s_dbg;
	static uint			s_counter;
};

//**********************************************************************

static streamsinitializer streamsinitializerlocal;

////////////////////////////////////////////////////////////////////////

inline dbgstream& get_dbg_stream (void)
	{ return streamsinitializer::get_stream(); }

} // namespace util

////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG
#define DBGOUT util::get_dbg_stream()
#define DBGENDL std::endl
#else
#define DBGOUT util::nullstream()
#define DBGENDL "\n";
#endif

#define TRACEOUT util::get_dbg_stream()

////////////////////////////////////////////////////////////////////////

#endif // UTIL_STREAMS_H
