/**
 *	InstanceValidator.h
 *
 *	-- Utility Library --
 *
 *	December 2005
 */

#ifndef UTIL_INSTANCEVALIDATOR_H
#define UTIL_INSTANCEVALIDATOR_H

#include "Common.h"
#include "Protect.h"

#include <boost/noncopyable.hpp>

namespace util {

////////////////////////////////////////////////////////////////////////
// Instance validator class
//
template <class T>
class instance_validator : public boost::noncopyable {
public:
	typedef T													value_type;
	typedef thread::ProtectWithMutex< std::list<const void*> >	instance_list;
	typedef instance_list::MutexType::scoped_lock				scoped_lock;

	instance_validator (void) {}
	~instance_validator (void) {}

	void add (const void* instance) { m_instances.push_back(instance); }
	void remove (const void* instance) { m_instances.remove(instance); }

	bool contains (const void* instance)
	{
		return std::find(
				m_instances.begin(), m_instances.end(), instance
			) != m_instances.end();
	}
	operator instance_list::MutexType& (void) { return m_instances; }

private:
	instance_list m_instances;
};


////////////////////////////////////////////////////////////////////////

} // namespace util

#endif // UTIL_INSTANCEVALIDATOR_H
