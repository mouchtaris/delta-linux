/**
 *	DHTCommands.h
 *
 *	-- Chord Distributed Directory --
 *
 *	DHTCommands are the commands exchanged between
 *	a DHT client and server.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007.
 */
#ifndef CHORD_DHT_COMMANDS_H_
#define CHORD_DHT_COMMANDS_H_

#include "Common.h"
#include "ChordCommon.h"

#include <vector>

namespace chord
{
	//----------------------------
	//-- enum DHTCommands

	enum DHTCommands {
		DHT_NO_REQUEST,
		DHT_NO_RESPONSE,

		DHT_INSERT,						///< payload = { Key key, Value value }
		DHT_INSERT_RESPONSE,			///< payload = { void }

		DHT_REMOVE,						///< payload = { Key key }
		DHT_REMOVE_RESPONSE,			///< payload = { void }

		DHT_LOOKUP,						///< payload = { Key key }
		DHT_LOOKUP_RESPONSE,			///< payload = { Value value }
		DHT_LOOKUP_FAILED,				///< payload = { void }

		DHT_GET_KEYS,					///< payload = { Key node_id }
		DHT_GET_KEYS_RESPONSE,			///< payload = { vector<Key> keys }

		DHT_INTRODUCE_SERVERS,			///< payload = { void }
		DHT_INTRODUCE_SERVERS_RESPONSE,	///< payload = { list< tuple<uint32, uint16> > dhtServers }

		DHT_REPORT,						///< payload = { void }
		DHT_REPORT_RESPONSE,			///< payload = { vector< tuple<Peer vpeer_host, Peer predecessor, FingerTable fingers> >, Directory }
	};

	typedef std::vector<Key> KeySet;
}

#endif	//CHORD_DHT_COMMANDS_H_
