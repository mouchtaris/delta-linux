/**
 *	Protect.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef THREAD_PROTECT_H
#define THREAD_PROTECT_H

#include "Common.h"

#include <boost/thread/mutex.hpp>
#include <boost/serialization/access.hpp>

namespace thread {

////////////////////////////////////////////////////////////////////////

template <class T, class TMutex = boost::mutex>
class ProtectWithMutex : public T {
public:
	typedef TMutex MutexType;

	MutexType mutex; // mutex is public

	//******************************************************************
	// Constructors
	//
	ProtectWithMutex (void) {}

	template <class A1>
	ProtectWithMutex (A1 a1) : T(a1) {}

	template <class A1, class A2>
	ProtectWithMutex (A1 a1, A2 a2) : T(a1, a2) {}

	template <class A1, class A2, class A3>
	ProtectWithMutex (A1 a1, A2 a2, A3 a3) : T(a1, a2, a3) {}

	template <class A1, class A2, class A3, class A4>
	ProtectWithMutex (A1 a1, A2 a2, A3 a3, A4 a4) : T(a1, a2, a3, a4) {}

	template <class A1, class A2, class A3, class A4, class A5>
	ProtectWithMutex (A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) : T(a1, a2, a3, a4, a5) {}

	//******************************************************************

	operator MutexType& (void) { return mutex; }

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize (Archive& ar, const unsigned int version)
		{ ar & boost::serialization::base_object<T>(*this); }
};

////////////////////////////////////////////////////////////////////////

} // namespace thread

#endif // THREAD_PROTECT_H
