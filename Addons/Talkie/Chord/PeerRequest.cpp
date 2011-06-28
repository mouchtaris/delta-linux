/**
 *	PeerRequest.cpp
 *
 *	-- Chord Routing Algorithm --
 *
 *	PeerRequest class represents a chord's peer
 *	request to another peer in the network.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	November 2006.
 */
#include "PeerRequest.h"

#include <boost/asio.hpp>

namespace chord
{
	//-------------------------------------------------------//
	//---- Class PeerRequest --------------------------------//

	PeerRequest::PeerRequest(const PeerRequest::MessageBuffer& buf)
		: buffer(buf)
	{
	}

	//-----------------------------------------------------------------------

	PeerRequest::PeerRequest(PeerRequest::CMD cmd, uint32 seq)
	{
		assert(cmd == GET_ID);

		buffer[0] = static_cast<byte>(cmd);
		std::copy((byte*)&seq, ((byte*)&seq) + SEQ_SIZE, buffer.rbegin() + PORT_SIZE + ADDR_SIZE);
			///< FIXME: mlk edw o kodikas exartate apo to gegonos an KEY_SIZE < PORT_SIZE + ADDR_SIZE
	}

	//-----------------------------------------------------------------------

	PeerRequest::PeerRequest(PeerRequest::CMD cmd, uint32 seq, unsigned long addr, unsigned short port)
	{
		assert(cmd == REPORT_DEAD);

		buffer[0] = static_cast<byte>(cmd);
		std::copy((byte*)&seq, ((byte*)&seq) + SEQ_SIZE, buffer.rbegin() + PORT_SIZE + ADDR_SIZE);
			///< FIXME: mlk edw o kodikas exartate apo to gegonos an KEY_SIZE < PORT_SIZE + ADDR_SIZE
		std::copy((byte*)&addr, ((byte*)&addr) + ADDR_SIZE, buffer.rbegin() + PORT_SIZE);
		std::copy((byte*)&port, ((byte*)&port) + PORT_SIZE, buffer.rbegin());
	}

	//-----------------------------------------------------------------------

	PeerRequest::PeerRequest(PeerRequest::CMD cmd, uint32 seq, const Key& key)
	{
		assert(cmd == FIND_SUCCESSOR || cmd == FIND_SUCCESSOR_SAFE || cmd == NOTIFY || cmd == STABILIZE || cmd == CHECK_PREDECESSOR);

		buffer[0] = static_cast<byte>(cmd);
		std::copy((byte*)&seq, ((byte*)&seq) + SEQ_SIZE, buffer.rbegin() + PORT_SIZE + ADDR_SIZE);
			///< FIXME: mlk edw o kodikas exartate apo to gegonos an KEY_SIZE < PORT_SIZE + ADDR_SIZE
		std::copy(key.GetData().begin(), key.GetData().end(), buffer.begin() + CMD_SIZE + SEQ_SIZE);
	}

	//-----------------------------------------------------------------------

	PeerRequest::PeerRequest(const PeerRequest& src)
		: buffer(src.buffer)
	{
	}

	//-----------------------------------------------------------------------

	PeerRequest::CMD PeerRequest::GetCmd(void) const
	{
		assert(FIND_SUCCESSOR <= static_cast<CMD>(buffer[0]) && static_cast<CMD>(buffer[0]) < CMD_TOTAL);

		return static_cast<CMD>(buffer[0]);
	}

	//-----------------------------------------------------------------------

	uint32 PeerRequest::GetSequence(void) const
	{
		assert(FIND_SUCCESSOR <= static_cast<CMD>(buffer[0]) && static_cast<CMD>(buffer[0]) < CMD_TOTAL);

		return ntohl(*(unsigned long*)(&(buffer[CMD_SIZE])));
	}

	//-----------------------------------------------------------------------

	uint32 PeerRequest::GetAddress(void) const
	{
		assert(GetCmd() == REPORT_DEAD);

		return ntohl(*(unsigned long*)(&(buffer[CMD_SIZE + SEQ_SIZE])));
	}

	//-----------------------------------------------------------------------

	uint16 PeerRequest::GetPort(void) const
	{
		assert(GetCmd() == REPORT_DEAD);

		return ntohs(*(unsigned short*)(&(buffer[CMD_SIZE + SEQ_SIZE + ADDR_SIZE])));
	}

	//-----------------------------------------------------------------------

	const Key PeerRequest::GetKey(void) const
	{
		assert(GetCmd() == FIND_SUCCESSOR || GetCmd() == FIND_SUCCESSOR_SAFE || GetCmd() == NOTIFY || GetCmd() == STABILIZE || GetCmd() == CHECK_PREDECESSOR);

		return Key(buffer.begin() + CMD_SIZE + SEQ_SIZE, buffer.begin() + CMD_SIZE + SEQ_SIZE + KEY_SIZE);
	}

	//-----------------------------------------------------------------------
}
