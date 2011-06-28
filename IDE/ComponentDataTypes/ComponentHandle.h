/**
 *	ComponentHandle.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTHANDLE_H
#define IDE_COMPONENTHANDLE_H

#include "Common.h"
#include "IDECommon.h"
#include "Message.h"

#include <vector>
#include <list>

namespace ide {

////////////////////////////////////////////////////////////////////////

class Component;

////////////////////////////////////////////////////////////////////////

class _IDE_API Handle {
public:
	Handle (void) : m_serial(0) {}
	Handle (Component* comp);
	Handle (const std::string& classId, uint serial);

	//******************************************************************

	Component* Resolve (void) const;
	Component* operator-> (void) const { return this->Resolve(); }

	//******************************************************************

	const std::string&	GetClassId (void) const { return m_classId; }
	uint				GetSerial (void) const { return m_serial; }

	//******************************************************************

	Handle& operator= (const Handle& handle);

	bool operator== (const Handle& handle) const;
	bool operator!= (const Handle& handle) const;
	bool operator< (const Handle& handle) const;

	bool operator== (const std::string& classId) const { return m_classId == classId; }
	bool operator!= (const std::string& classId) const { return m_classId != classId; }
	bool operator< (const std::string& classId) const { return m_classId < classId; }

	bool operator== (uint serial) const { return m_serial == serial; }
	bool operator!= (uint serial) const { return m_serial != serial; }
	bool operator< (uint serial) const { return m_serial < serial; }

	//******************************************************************

	operator bool (void) const { return !m_classId.empty(); }

	//******************************************************************

	bool IsDangling (void) const { return this->Resolve() == 0; }

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize (Archive& ar, const unsigned int version)
		{ ar & m_classId & m_serial; }

	std::string	m_classId;
	uint		m_serial;
};

////////////////////////////////////////////////////////////////////////

typedef std::vector<Handle> HandleVec;
typedef std::list<Handle> HandleList;

////////////////////////////////////////////////////////////////////////

extern _IDE_API const Handle nullHandle;

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTHANDLE_H
