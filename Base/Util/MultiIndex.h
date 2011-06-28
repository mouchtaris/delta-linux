/**
 *	MultiIndex.h
 *
 * Property visitor that resolves the correct type of a
 * property and calls the appropriate "Visit" function
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef UTIL_MULTIINDEX_H
#define UTIL_MULTIINDEX_H

#ifndef NDEBUG
#define BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING
#define BOOST_MULTI_INDEX_ENABLE_SAFE_MODE
#endif

#include "Serialization.h" // Not happy to include it here ... but ...

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

using boost::multi_index_container;
using namespace boost::multi_index;

namespace util {

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

} // namespace util

#endif // UTIL_MULTIINDEX_H
