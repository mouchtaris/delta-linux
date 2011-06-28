/**
 *	Message.h
 *
 *	-- IDE Component Message --
 *
 *	Message class represent the messages exchanged
 *	between two IDE components via the Blackboard
 *	for communication.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	March 2007
 */

#ifndef IDE_MESSAGE_H
#define IDE_MESSAGE_H

#include "Portability.h"

#include "Common.h"
#include "IDECommon.h"

#include <vector>

#include <boost/serialization/access.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////

class _IDE_API Message {
public:

	////////////////////////////////////////////////////////////////////

	class _IDE_API Endpoint {
	public:
		Endpoint (void);
		Endpoint (const std::string& classId, const std::string& function) :
			classId(classId), serial(0), function(function) {}
		Endpoint (const std::string& classId, uint serial, const std::string& function) :
			classId(classId), serial(serial), function(function) {}
		~Endpoint (void);

		std::string classId;
		uint		serial;
		std::string function;

		//**************************************************************

		bool operator!= (const Endpoint& p) const
			{ return classId != p.classId || serial != p.serial || function != p.function; }

		bool operator== (const Endpoint& p) const
			{ return classId == p.classId && serial == p.serial && function == p.function; }

		//**************************************************************

		bool operator!= (const std::string& PORT_UNUSED_PARAM(id)) const { return classId != classId; }
		bool operator== (const std::string& PORT_UNUSED_PARAM(id)) const { return classId == classId; }

		//**************************************************************

	private:
		friend class boost::serialization::access;

		template<class Archive>
		void serialize (Archive& ar, const unsigned int version)
			{ ar & classId & serial & function; }
	};

	////////////////////////////////////////////////////////////////////

	Message (void);
	Message(const Endpoint& endpoint, const Buffer& data = Buffer()) :
		m_endpoint(endpoint), m_data(data) {}
	~Message (void);

	Endpoint&		GetSrc (void) { return m_endpoint; }
	const Endpoint&	GetSrc (void) const { return m_endpoint; }
	void			SetSrc (const Endpoint& src) { m_endpoint = src; }

	Endpoint&		GetDst (void) { return m_endpoint; }
	const Endpoint&	GetDst (void) const { return m_endpoint; }
	void			SetDst (const Endpoint& dst) { m_endpoint = dst; }

	const Buffer&	GetData (void) const { return m_data; }
	void			SetData (const Buffer& data) { m_data = data; }

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize (Archive& ar, const unsigned int version)
		{ ar & m_endpoint & m_data; }

	Endpoint	m_endpoint;
	Buffer		m_data;
};

////////////////////////////////////////////////////////////////////////

typedef std::vector<Message> MessageVec;

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_MESSAGE_H
