// usharedresourceholder.h
// Template class for resource sharing based on reference counting.
// String keys are only supported.
// A. Savidis, December 2007.
//

#ifndef	USHAREDRESOURCEHOLDER_H
#define	USHAREDRESOURCEHOLDER_H

#include "DDebug.h"
#include "utypes.h"
#include "ufunctors.h" 

#include <map>
#include <string>
#include <algorithm>

//---------------------------------------------------------------

template <class T> struct uresource_default_traits {
	typedef T type;
	static void destroy_function (T* t) { DDELETE(t); }
	typedef void (*destroy_function_type)(T*);
};

template <class RcTraits> class usharedresourceholder {

	public:
	typedef typename RcTraits::type				rc_type;
	typedef std::map<std::string, rc_type*>		by_key_map;

	private:
	typedef rc_type								T;
	typedef std::pair<std::string, util_ui16>	rc_entry;
	typedef std::map<T*, rc_entry>				by_rc_map;

	by_key_map									byKey;	
	by_rc_map									byRc;

	public:
	by_key_map& getall (void) { return byKey; }
	const by_key_map& getall (void) const { return byKey; }

	util_ui32 total (void) const {
		DASSERT(byRc.size() == byKey.size()); 
		return byKey.size(); 
	}

	T* add (T* rc, const std::string& key) { 
		typename std::map<std::string, T*>::iterator i = byKey.find(key);
		if (i == byKey.end()) {
			DASSERT(byRc.find(rc) == byRc.end());
			byRc[rc]	= rc_entry(key, 1);
			byKey[key]	= rc;
		}
		else {
			DASSERT(byRc.find(rc) != byRc.end());
			++byRc[rc].second;
		}
		return rc;
	}

	bool in (const T* rc) const
		{ return byRc.find(const_cast<T*>(rc)) != byRc.end(); }

	void ignore (T* rc) {
		typename std::map<T*, rc_entry>::iterator i = byRc.find(rc);
		DASSERT(i != byRc.end() && i->second.second);
		
		typename std::map<std::string, T*>::iterator j = byKey.find(i->second.first);
		DASSERT(j != byKey.end());

		byRc.erase(i);
		byKey.erase(j);
	}

	T* getrc (const std::string& key) {
		typename std::map<std::string, T*>::iterator i = byKey.find(key);
		return i != byKey.end() ? i->second : (T*) 0;
	}

	T* getrc (const std::string& key) const {
		typename std::map<std::string, T*>::const_iterator i = byKey.find(key);
		return i != byKey.end() ? i->second : (T*) 0;
	}

	const std::string& getkey (const T* rc) const {
		typename std::map<T*, rc_entry>::const_iterator i = byRc.find(const_cast<T*>(rc));
		DASSERT(i != byRc.end() && i->second.second);
		return i->second.first;
	}

	T* use (T* rc) {
		DASSERT(byRc.find(rc) != byRc.end());
		++byRc[rc].second;
		return rc;
	}

	void unuse (T* rc) { 
		typename std::map<T*, rc_entry>::iterator i = byRc.find(rc);
		DASSERT(i != byRc.end() && i->second.second);
		
		typename std::map<std::string, T*>::iterator j = byKey.find(i->second.first);
		DASSERT(j != byKey.end());

		if (!--i->second.second) {
			byRc.erase(i);
			RcTraits::destroy_function(rc);
			byKey.erase(j);
		}
	}

	void clear (void) {
		std::for_each(
			byRc.begin(),
			byRc.end(),
			ufunctor_first< 
				std::pair<T*, rc_entry>, 
				typename RcTraits::destroy_function_type
			>(RcTraits::destroy_function)
		);
		byRc.clear();
		byKey.clear();
	}
};

//---------------------------------------------------------------
// Simple ref counted, shared, dynamic value holder.

template <typename T> class urefcounted {
	DFRIENDDESTRUCTOR()
	private:
	T						val;
	util_ui32				refCounter;
	urefcounted (const T& _val) : val(_val), refCounter(0){}
	~urefcounted(){}

	public:
	T&						get (void) 
								{ return val; }
	const T&				get (void) const 
								{ return val; }
	void					inc_ref_counter (void)
								{ ++refCounter; }
	void					dec_ref_counter (void)
								{ DASSERT(refCounter); if (!--refCounter) DDELETE(this); }
	static urefcounted*		new_inst (const T& val) 
								{ return DNEWCLASS(urefcounted, (val)); }
};

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
