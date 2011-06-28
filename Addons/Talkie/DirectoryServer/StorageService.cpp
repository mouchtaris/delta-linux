/**
 *	StorageService.cpp
 *
 *	-- Talkie Directory Server --
 *
 *	Storage system of the Directory entries to
 *	the Chord DHT. It handles insertion, lookup,
 *	replication and recovery of entries.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#include "StorageService.h"

namespace talkie
{
	//----------------------------
	//-- struct ignore_response

	struct ignore_response :
		public std::unary_function<bool, void>
	{
		result_type operator ()(argument_type) {}
	};

	//-----------------------------------------------------------------------


	//-------------------------------------------------------//
	//---- class InsertService ------------------------------//

	void InsertService::Insert(const DirectoryEntry& entry)
	{
		KeyVec keys = GenerateKeys(entry.userInfo.GetHandle());
		for (uint8 i=0; i < keys.size(); ++i)
		{
			client->dhtClient.Insert(
				keys[i], comm::encoder() << entry,
				boost::bind(
					&InsertService::onInsertResponse, this, keys[i], _1
				)
			);
		}
	}

	//-----------------------------------------------------------------------

	void InsertService::onInsertResponse(const chord::Key&, bool success)
	{
		++responses;

		if (!this->success && success)
		{
			this->success = true;
			handler(true);			///< notify requestor
		}

		if (responses == ReplicaFactor)
		{
			if (!this->success)
				handler(false);		///< notify requestor
		}
	}

	//-----------------------------------------------------------------------


	//-------------------------------------------------------//
	//---- class LookupService ------------------------------//

	void LookupService::Lookup(const String& user)
	{
		KeyVec keys = GenerateKeys(user);
		for (uint8 i=0; i < keys.size(); ++i)
		{
			client->dhtClient.Lookup(
				keys[i],
				boost::bind(
					&LookupService::onLookupResponse, this, keys[i], _1
				)
			);
		}
	}

	//-----------------------------------------------------------------------

	void LookupService::onLookupResponse(const chord::Key& key, const chord::Value& value)
	{
		++responses;

		if (value.empty())					///< no entry found for key
			missingEntries.push_back(key);
		else if (this->value.empty())		///< it is the first actual response
		{
			this->value = value;
			DirectoryEntry entry;
			comm::decoder(this->value) >> entry;
			handler(entry);			///< notify requestor
		}
		else
			;		///< TODO: cache consistency checking?

		if (responses == ReplicaFactor)
			///< restore missing entries and finish
		{
			if (this->value.empty())
				handler(DirectoryEntry());
			else
			{
				//KeyList::const_iterator it = missingEntries.begin();
				//for (; it != missingEntries.end(); ++it)
				//	client->dhtClient.Insert(
				//		*it, this->value, ignore_response()
				//	);
			}
		}
	}

	//-----------------------------------------------------------------------
}
