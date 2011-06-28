/**
 *	StorageService.h
 *
 *	-- Talkie Centralized Directory Server --
 *
 *	Centralized directory server of the talkie
 *	application retrieving.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#ifndef TALKIE_CENTRALIZED_DIRECTORY_STORAGE_SERVICE_H_
#define TALKIE_CENTRALIZED_DIRECTORY_STORAGE_SERVICE_H_

#pragma warning(disable: 4267)

#include "Common.h"
#include "CommonFwdDecls.h"

#include "DirectoryEntry.h"

#include <boost/function.hpp>

namespace talkie
{
	//----------------------------
	//-- class StorageService

	class StorageService
	{
	public:
		///--- Singleton Pattern
		static StorageService& Instance(void) {
			static StorageService singleton;		///< Mayer's Singleton
			return singleton;
		}

		///--- type definitions
		typedef std::map<Key, DirectoryEntry> Directory;

		typedef boost::function<void (bool)> InsertResponseHandler;
		typedef boost::function<void (DirectoryEntry&)> LookupResponseHandler;

		///--- public API
		void Insert(const DirectoryEntry& entry, InsertResponseHandler handler);
		void Lookup(const String& user, LookupResponseHandler handler);

		Directory& GetDirectory(void) { return directory; }

	private:
		///--- constructors / destructor
		StorageService(void) { LoadRecords(); }
		~StorageService(void) {}
		DISABLE_COPY(StorageService);

		///--- private API
		void LoadRecords(void);
		void StoreRecord(const DirectoryEntry& entry);

		///--- private members
		Directory directory;		///< directory
	};
}

#endif	//TALKIE_CENTRALIZED_DIRECTORY_STORAGE_SERVICE_H_
