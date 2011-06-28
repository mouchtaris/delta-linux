/**
 *	DirectoryEntry.h
 *
 *	-- Talkie Centralized Directory Server --
 *
 *	Centralized directory server of the talkie
 *	application storing user information.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#ifndef TALKIE_CENTRALIZED_DIRECTORY_ENTRY_H_
#define TALKIE_CENTRALIZED_DIRECTORY_ENTRY_H_

#include "Common.h"
#include "BigFixedInt.h"
#include "Hashing.h"

#include "FriendEntry.h"
#include "TalkieNetwork.h"

#include <list>
#include <utility>

namespace talkie
{
	typedef boost::asio::ip::tcp::endpoint tcp_endpoint;
	typedef ide::BigFixedInt<4> Key;

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
		static const Key GenerateKey(const void* data, uint32 len) {
			return Key(
				util::SuperFastHash((const char*)data, len)
			);
		}
		static const Key GenerateKey(const String& str) {
			return Key(
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

#endif	//TALKIE_CENTRALIZED_DIRECTORY_ENTRY_H_
