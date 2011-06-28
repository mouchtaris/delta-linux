/**
 *	StorageService.cpp
 *
 *	-- Talkie Centralized Directory Server --
 *
 *	Centralized directory server of the talkie
 *	application retrieving.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#include "StorageService.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

namespace talkie
{
	//-------------------------------------------------------//
	//---- class StorageService -----------------------------//

	void StorageService::Insert(const DirectoryEntry& entry, InsertResponseHandler handler)
	{
		Key key = DirectoryEntry::GenerateKey(entry.userInfo.GetHandle());

		//-- Insert {key, value} pair
		directory[key] = entry;

		//-- Persistant storage of {key, entry}
		StoreRecord(entry);

		handler(true);
	}

	//-----------------------------------------------------------------------

	void StorageService::Lookup(const String& user, LookupResponseHandler handler)
	{
		Key key = DirectoryEntry::GenerateKey(user);
		
		//-- Lookup {key, value} pair
		Directory::iterator it = directory.find(key);
		if (it != directory.end())
			handler(it->second);
		else
			handler(DirectoryEntry());
	}

	//-----------------------------------------------------------------------

	void StorageService::LoadRecords(void)
	{
		//-- Retrieve keys from persistent storage
		if (boost::filesystem::exists("directory"))
		{
			boost::filesystem::directory_iterator it("directory");
			boost::filesystem::directory_iterator end;
			for (; it != end; ++it)
			{
				if (boost::filesystem::is_directory(*it))
					continue;

				Key key; std::string value; DirectoryEntry entry;
				boost::filesystem::ifstream in(*it);
				std::string strbuf((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
				comm::decoder(strbuf) >> key >> value;
				comm::decoder(value) >> entry;
				directory[key] = entry;
				in.close();
			}
		}
	}

	//-----------------------------------------------------------------------

	void StorageService::StoreRecord(const DirectoryEntry& entry)
	{
		Key key = DirectoryEntry::GenerateKey(entry.userInfo.GetHandle());
		std::string value = comm::encoder() << entry;

		if (!boost::filesystem::exists("directory"))
			boost::filesystem::create_directory("directory");
	    boost::filesystem::ofstream out(
			boost::filesystem::path("directory/" + key.str() + "dat")
		);
		out << (comm::encoder() << key << value).str();
		out.close();
	}

	//-----------------------------------------------------------------------
}
