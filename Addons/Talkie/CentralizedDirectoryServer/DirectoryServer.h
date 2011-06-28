/**
 *	DirectoryServer.h
 *
 *	-- Talkie Centralized Directory Server --
 *
 *	Centralized directory server of the talkie
 *	application retrieving.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#ifndef TALKIE_CENTRALIZED_DIRECTORY_SERVER_H_
#define TALKIE_CENTRALIZED_DIRECTORY_SERVER_H_

#pragma warning(disable: 4267)

#include "Common.h"
#include "CommonFwdDecls.h"
#include "CommandServer.h"

#include "DirectoryEntry.h"
#include "StorageService.h"

namespace talkie
{
	using namespace net;

	//----------------------------
	//-- class DirectoryServer

	class DirectoryServer :
		public TCPCommandServer<connection, int32>
	{
	public:
		///--- type definitions

		///--- constructors / destructor
		DirectoryServer(boost::asio::io_service& io_service, uint16 port);
		~DirectoryServer(void);

		///--- public API

	private:
		///--- private API
		template <class Tuple>
		bool ResponseHandler(LinkPtrType client, CommandType command, DecoderType& dec,
			boost::function<void (DirectoryServer::LinkPtrType, DirectoryEntry&, const Tuple&)> handler)
		{
			Tuple args;
			decode_tuple(dec, args);

			//-- Lookup if there is already a registered entry
			StorageService::Instance().Lookup(
				boost::get<0>(args),		///< first argument is always username
				boost::bind(handler, client, _1, args)
			);

			return true;
		}
	};
}

#endif	//TALKIE_CENTRALIZED_DIRECTORY_SERVER_H_
