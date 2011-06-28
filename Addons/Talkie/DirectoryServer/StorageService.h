/**
 *	StorageService.h
 *
 *	-- Talkie Directory Server --
 *
 *	Storage service of the Directory entries to
 *	the Chord DHT. It handles insertion, lookup,
 *	replication and recovery of lost entries.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#ifndef TALKIE_STORAGE_SERVICE_H_
#define TALKIE_STORAGE_SERVICE_H_

#pragma warning(disable: 4267)

#include "Common.h"

#include "DHTClient.h"
#include "DirectoryEntry.h"
#include "DirectoryConnection.h"
#include "StorageServiceManager.h"

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace talkie
{
	using namespace net;

	//----------------------------
	//-- class StorageService

	class StorageService
	{
	public:
		///--- type definitions
		enum { ReplicaFactor =1, };

		///--- constructors / destructor
		StorageService(DirectoryConnectionPtr client)
			: client(client), responses(0)
		{ StorageServiceManager::Instance().InsertService(this); }

		///--- public API
		bool IsDone(void) const	{ return responses == ReplicaFactor; }

	protected:
		///--- private type definitions
		typedef std::vector<chord::Key> KeyVec;

		///--- private API
		inline KeyVec GenerateKeys(const String& token) const {
			KeyVec keys(ReplicaFactor);
			keys[0] = DirectoryEntry::GenerateKey(token);
			for (uint8 i=1; i < ReplicaFactor; ++i)
				keys[i] = DirectoryEntry::GenerateKey(keys[i-1].GetData().begin(), chord::Key::length);
			return keys;
		}

		///--- protected members
		DirectoryConnectionPtr client;
		uint8 responses;
	};


	//----------------------------
	//-- class InsertService

	class InsertService :
		public StorageService
	{
	public:
		///--- type definitions
		typedef boost::function<void (bool)> ResponseHandler;

		///--- constructors / destructor
		InsertService(DirectoryConnectionPtr client, 
			const DirectoryEntry& entry, ResponseHandler handler)
			: StorageService(client), handler(handler), success(false) { Insert(entry); }

	private:
		///--- private API
		void Insert(const DirectoryEntry& entry);
		void onInsertResponse(const chord::Key&, bool success);

		///--- private members
		ResponseHandler handler;
		bool success;
	};


	//----------------------------
	//-- class LookupService

	class LookupService :
		public StorageService
	{
	public:
		///--- type definitions
		typedef boost::function<void (DirectoryEntry&)> ResponseHandler;

		///--- constructors / destructor
		LookupService(DirectoryConnectionPtr client, 
			const String& user, ResponseHandler handler)
			: StorageService(client), handler(handler) { Lookup(user); }

	private:
		///--- private API
		void Lookup(const String& user);
		void onLookupResponse(const chord::Key& key, const chord::Value& value);

		///--- private members
		ResponseHandler handler;

		typedef std::list<chord::Key> KeyList;
		KeyList missingEntries;
		chord::Value value;
	};
}

#endif	//TALKIE_STORAGE_SERVICE_H_
