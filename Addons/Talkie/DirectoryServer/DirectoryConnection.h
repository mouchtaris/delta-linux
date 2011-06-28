/**
 *	DirectoryConnection.h
 *
 *	-- Talkie Directory Server --
 *
 *	DirectoryConnection is a specialization of the
 *	net::connection class that can .
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#ifndef TALKIE_DIRECTORY_CONNECTION_H_
#define TALKIE_DIRECTORY_CONNECTION_H_

#pragma warning(disable: 4267)

#include "Common.h"
#include "CommonFwdDecls.h"
#include "Connection.h"

#include "DHTClient.h"

#include <boost/shared_ptr.hpp>

namespace talkie
{
	//----------------------------
	//-- class DirectoryConnection

	class DirectoryConnection :
		public net::connection
	{
	public:
		///--- constructors / destructor
		DirectoryConnection(boost::asio::io_service& io_service)
			: net::connection(io_service), dhtClient(io_service) {}

		chord::DHTClient dhtClient;
	};

	typedef boost::shared_ptr<DirectoryConnection> DirectoryConnectionPtr;
}


#endif	//TALKIE_DIRECTORY_CONNECTION_H_