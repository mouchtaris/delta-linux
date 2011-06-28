/**
 *	Peer.cpp
 *
 *	-- Chord Routing Algorithm --
 *
 *	VirtualPeer class represents a chord's virtual peer
 *	in the system.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	November 2006.
 */
#include "Peer.h"

namespace chord
{
	//-------------------------------------------------------//
	//---- Free Functions -----------------------------------//

	std::ostream& operator <<(std::ostream& os, const Peer& key)
	{
		os << key.endpoint << " [" << key.id << "]";

		return os;
	}

	//-----------------------------------------------------------------------
}