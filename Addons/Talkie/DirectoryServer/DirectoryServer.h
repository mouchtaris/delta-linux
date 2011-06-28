/**
 *	DirectoryServer.h
 *
 *	-- Talkie Directory Server --
 *
 *	Directory server of the talkie application retrieving
 *	user information from the Chord DHT.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2007
 */
#ifndef TALKIE_DIRECTORY_SERVER_H_
#define TALKIE_DIRECTORY_SERVER_H_

#pragma warning(disable: 4267)

#include "Common.h"
#include "CommonFwdDecls.h"
#include "ChordCommon.h"

#include "CommandServer.h"

#include "DHTClient.h"
#include "DirectoryConnection.h"
#include "StorageService.h"

namespace talkie
{
	using namespace net;

	//----------------------------
	//-- class DirectoryServer

	class DirectoryServer :
		public TCPCommandServer<DirectoryConnection, int32>
	{
	public:
		///--- type definitions

		///--- constructors / destructor
		DirectoryServer(boost::asio::io_service& io_service, uint16 port,
			const chord::PeerList& servers);
		~DirectoryServer(void);

		///--- public API

	private:
		///--- private API
		template <class Tuple>
		bool ResponseHandler(LinkPtrType client, CommandType command, DecoderType& dec,
			boost::function<void (DirectoryServer::LinkPtrType, DirectoryEntry&, const Tuple&)> handler)
		{
			Tuple args;
			comm::decode_tuple(dec, args);

			//-- Connect to DHTServer
			if (!client->dhtClient.IsConnected()) {
				if (!client->dhtClient.Connect(servers)) {
					std::cout << "Connection to DHTServer failed!" << std::endl;
					return false;
				}
			}

			//-- Lookup if there is already a registered entry
			try {
				new LookupService(
					client, boost::get<0>(args),		///< first argument is always username
					boost::bind(handler, client, _1, args)
				);
			} catch (const std::exception&) {
				client->write(comm::encoder() << (CommandType)((uint32)command +1));
			}

			return true;
		}

		///--- private members
		const chord::PeerList& servers;
	};
}

#endif	//TALKIE_DIRECTORY_SERVER_H_
