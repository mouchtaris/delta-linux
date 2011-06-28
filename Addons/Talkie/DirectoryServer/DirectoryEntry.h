/**
 *	DirectoryEntry.h
 *
 *	-- Talkie Directory Server --
 *
 *	Directory server of the talkie application retrieving
 *	user information from the Chord DHT.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2007
 */
#ifndef TALKIE_DIRECTORY_ENTRY_H_
#define TALKIE_DIRECTORY_ENTRY_H_

#include "Common.h"
#include "Hashing.h"
#include "Algorithms.h"

#include "ChordCommon.h"

#include "FriendEntry.h"
#include "TalkieNetwork.h"

#include <list>
#include <utility>

namespace talkie
{
	//----------------------------
	//-- struct DirectoryEntry

	struct DirectoryEntry
	{
	public:
		///--- type definitions
		typedef std::list<String> FriendList;

		///--- constructors / destructor
		DirectoryEntry(void) {}
		DirectoryEntry(const FriendEntry& userInfo, const String& passwd)
			: userInfo(userInfo), passwd(passwd) {}
		~DirectoryEntry(void) {}

		///--- public API
		static const chord::Key GenerateKey(const void* data, uint32 len) {
			return chord::Key(
				util::SuperFastHash((const char*)data, len)
			);
		}
		static const chord::Key GenerateKey(const String& str) {
			return chord::Key(
				util::SuperFastHash((const char*)str.c_str(), str.length() * sizeof(wxChar))
			);
		}

		operator bool(void) const { return !userInfo.GetHandle().empty(); }

		///--- public members
		FriendEntry userInfo;
		String passwd;

		FriendList friends;
		PendingOperationList pending;

	private:
		friend class boost::serialization::access;

		template<class Archive>
		void serialize (Archive& ar, const unsigned int version)
			{ ar & userInfo & passwd & friends & pending; }
	};
}

#endif // TALKIE_DIRECTORY_ENTRY_H_
