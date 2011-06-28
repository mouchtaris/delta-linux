/**
 *	PeerResponse.cpp
 *
 *	-- Chord Routing Algorithm --
 *
 *	PeerResponse class represents a chord's peer
 *	response to another peer's request in the network.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	November 2006.
 */
#include "PeerResponse.h"

#include <boost/asio.hpp>

namespace chord
{
	//-------------------------------------------------------//
	//---- Class PeerResponse -------------------------------//

	PeerResponse::PeerResponse(const PeerResponse::MessageBuffer& buf)
		: buffer(buf)
	{
	}

	//-----------------------------------------------------------------------

	PeerResponse::PeerResponse(PeerResponse::CMD cmd, uint32 seq)
	{
		assert(false);

		buffer[0] = static_cast<byte>(cmd);
		std::copy((byte*)&seq, ((byte*)&seq) + SEQ_SIZE, buffer.rbegin() + PORT_SIZE + ADDR_SIZE);
			///< FIXME: mlk edw o kodikas exartate apo to gegonos an KEY_SIZE < PORT_SIZE + ADDR_SIZE
	}

	//-----------------------------------------------------------------------

	PeerResponse::PeerResponse(PeerResponse::CMD cmd, uint32 seq, unsigned long addr, unsigned short port)
	{
		assert(cmd == FIND_SUCCESSOR_RESPONSE || cmd == STABILIZE_RESPONSE || cmd == CHECK_PREDECESSOR_RESPONSE);

		buffer[0] = static_cast<byte>(cmd);
		std::copy((byte*)&seq, ((byte*)&seq) + SEQ_SIZE, buffer.rbegin() + PORT_SIZE + ADDR_SIZE);
			///< FIXME: mlk edw o kodikas exartate apo to gegonos an KEY_SIZE < PORT_SIZE + ADDR_SIZE
		std::copy((byte*)&addr, ((byte*)&addr) + ADDR_SIZE, buffer.rbegin() + PORT_SIZE);
		std::copy((byte*)&port, ((byte*)&port) + PORT_SIZE, buffer.rbegin());
	}

	//-----------------------------------------------------------------------

	PeerResponse::PeerResponse(PeerResponse::CMD cmd, uint32 seq, const Key& key)
	{
		assert(cmd == GET_ID_RESPONSE);

		buffer[0] = static_cast<byte>(cmd);
		std::copy((byte*)&seq, ((byte*)&seq) + SEQ_SIZE, buffer.rbegin() + PORT_SIZE + ADDR_SIZE);
			///< FIXME: mlk edw o kodikas exartate apo to gegonos an KEY_SIZE < PORT_SIZE + ADDR_SIZE
		std::copy(key.GetData().begin(), key.GetData().end(), buffer.begin() + CMD_SIZE + SEQ_SIZE);
	}

	//-----------------------------------------------------------------------

	PeerResponse::PeerResponse(const PeerResponse& src)
		: buffer(src.buffer)
	{
	}

	//-----------------------------------------------------------------------

	PeerResponse::CMD PeerResponse::GetCmd(void) const
	{
		assert(FIND_SUCCESSOR_RESPONSE <= static_cast<CMD>(buffer[0]) && static_cast<CMD>(buffer[0]) < CMD_TOTAL);

		return static_cast<CMD>(buffer[0]);
	}

	//-----------------------------------------------------------------------

	uint32 PeerResponse::GetSequence(void) const
	{
		assert(FIND_SUCCESSOR_RESPONSE <= static_cast<CMD>(buffer[0]) && static_cast<CMD>(buffer[0]) < CMD_TOTAL);

		return ntohl(*(unsigned long*)(&(buffer[CMD_SIZE])));
	}

	//-----------------------------------------------------------------------

	uint32 PeerResponse::GetAddress(void) const
	{
		assert(GetCmd() == FIND_SUCCESSOR_RESPONSE || GetCmd() == STABILIZE_RESPONSE || GetCmd() == CHECK_PREDECESSOR_RESPONSE);

		return ntohl(*(unsigned long*)(&(buffer[CMD_SIZE + SEQ_SIZE])));
	}

	//-----------------------------------------------------------------------

	uint16 PeerResponse::GetPort(void) const
	{
		assert(GetCmd() == FIND_SUCCESSOR_RESPONSE || GetCmd() == STABILIZE_RESPONSE || GetCmd() == CHECK_PREDECESSOR_RESPONSE);

		return ntohs(*(unsigned short*)(&(buffer[CMD_SIZE + SEQ_SIZE + ADDR_SIZE])));
	}

	//-----------------------------------------------------------------------

	const Key PeerResponse::GetKey(void) const
	{
		assert(GetCmd() == GET_ID_RESPONSE);

		return Key(buffer.begin() + CMD_SIZE + SEQ_SIZE, buffer.begin() + CMD_SIZE + SEQ_SIZE + KEY_SIZE);
	}

	//-----------------------------------------------------------------------
}
