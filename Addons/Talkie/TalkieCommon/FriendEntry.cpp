/**
 *	FriendEntry.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "FriendEntry.h"

namespace talkie {

////////////////////////////////////////////////////////////////////////
// FriendEntry class
//

////////////////////////////////////////////////////////////////////////

} // namespace talkie

////////////////////////////////////////////////////////////////////////
// Pretty printing
//
std::ostream& operator<< (std::ostream& stream, const talkie::FriendEntry& entry)
{
	return stream << entry.GetName() << "(" << entry.GetHandle() << ")";
}
