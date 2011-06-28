/**
 *	FriendEntry.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef TALKIE_FRIENDENTRY_H
#define TALKIE_FRIENDENTRY_H

#include "Common.h"
#include "Endpoint.h"
#include "Protect.h"

#include <vector>

namespace talkie {

////////////////////////////////////////////////////////////////////////

class FriendEntry {
public:
	typedef net::Endpoint Endpoint;

	enum Status { ONLINE, BUSY, IDLE, OFFLINE, UNDEFINED /* Leave last */ };

	FriendEntry (void) : m_status(UNDEFINED) {}
	FriendEntry (const String& handle, const String& name, int image, const Endpoint& endpoint) :
		m_handle(handle), m_name(name), m_image(image), m_endpoint(endpoint), m_status(OFFLINE) {}
	~FriendEntry (void) {}

	//******************************************************************

	const String&	GetHandle (void) const { return m_handle; }
	void			SetHandle (const String& handle) { m_handle = handle; }

	const String&	GetName (void) const { return m_name; }
	void			SetName (const String& name) { m_name = name; }

	int				GetImage (void) const { return m_image; }
	void			SetImage (int image) { m_image = image; }

	void			SetEndpoint (const Endpoint& endpoint) { m_endpoint = endpoint; }
	const Endpoint& GetEndpoint (void) const { return m_endpoint; }
	Endpoint&		GetEndpoint (void) { return m_endpoint; }

	void			SetStatus (Status status) { m_status = status; }
	Status			GetStatus (void) const { return m_status; }

	//******************************************************************

	bool operator== (const String& handle) const { return m_handle == handle; }
	bool operator!= (const String& handle) const { return m_handle != handle; }

	bool operator== (const FriendEntry& entry) const { return m_handle == entry.m_handle; }
	bool operator!= (const FriendEntry& entry) const { return m_handle != entry.m_handle; }

	operator bool (void) const { return !m_handle.empty(); }

	////////////////////////////////////////////////////////////////////
	// Commonly used predicates
	//
	struct HasStatusPred : public std::unary_function<FriendEntry, bool> {
		HasStatusPred (Status status) : m_status(status) {}
		bool operator() (const FriendEntry& entry) const { return entry.GetStatus() == m_status; }
	private:
		Status m_status;
	};

	//******************************************************************

	struct CanConnectPred : public std::unary_function<FriendEntry, bool> {
		bool operator() (const FriendEntry& entry) const
			{ return entry.GetStatus() != OFFLINE && entry.GetStatus() != UNDEFINED; }
	};

	////////////////////////////////////////////////////////////////////

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize (Archive& ar, const unsigned int version)
		{ ar & m_handle & m_name & m_image & m_endpoint & m_status; }

	String		m_handle;
	String		m_name;
	int			m_image;
	Endpoint	m_endpoint;
	Status		m_status;
};

////////////////////////////////////////////////////////////////////////

typedef std::vector<FriendEntry> FriendEntryVec;

////////////////////////////////////////////////////////////////////////

} // namespace talkie

////////////////////////////////////////////////////////////////////////

std::ostream& operator<< (std::ostream& stream, const talkie::FriendEntry& entry);

////////////////////////////////////////////////////////////////////////

#endif // TALKIE_FRIENDENTRY_H
