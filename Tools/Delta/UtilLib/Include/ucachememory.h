// ucachememory.h
// Simple template class for cases that cached items are needed.
// Supports reference counting scheme and an expiration-based disposal
// after the item becomes unreferenced.
// Generic library.
// A. Savidis, June 2008.
//

#ifndef	UCACHEMEMORY_H
#define	UCACHEMEMORY_H

#include "DDebug.h"
#include "utypes.h"
#include "ufunctors.h"

#include <map>
#include <algorithm>

//---------------------------------------------------------------

template <
		typename	Tkey, 
		typename	Tval,
		const		util_ui32 EXPIRATION_DELAY,
		typename	Fdestructor = uptrdestructorfunctor<Tval>
	> class ucachememory {
	
	private:

		class CacheEntry {
			private:
			enum State { Used, Unused, Expired };
			Tval		val;
			util_ui32	refCounter;
			State		state;
			util_ui32	expiresAt;
			bool		Invariant (void) const 
							{ return !refCounter ^ (state == Used); }

			public:
			Tval&		Get (void)
							{ DASSERT(Invariant() && IsUsed()); return val; }

			bool		HasExpired (util_ui32 currTime) {
							DASSERT(Invariant() && !IsUsed()); 
							if (!expiresAt)
								{ expiresAt = currTime + EXPIRATION_DELAY; return false; }
							else
							if (expiresAt <= currTime)
								{ state = Expired; return true; }
							else
								return false;
						}

			bool		IsUsed (void) const
							{ DASSERT(Invariant()); return state == Used; }

			void		DecRefCounter (void) { 
							DASSERT(Invariant() && refCounter && IsUsed() && !expiresAt);
							if (!--refCounter)
								state = Unused;
						}

			void		IncRefCounter (void) { // Firstly increase, and then Get().
							if (!IsUsed())
								{ state = Used; expiresAt = 0; }
							++refCounter;
						}
			
			// Gets 1 reference upon construction.

			CacheEntry (const Tval& _val) : 
				val(_val), 
				state(Used), 
				refCounter(1), 
				expiresAt(0)
				{}

			~CacheEntry() {
				DASSERT(Invariant() && state == Expired || state == Unused);
				Fdestructor()(val);
			}
		};
	
	////////////////////////////////////////////////////////////////

	std::map<Tkey, CacheEntry*> used;
	std::map<Tkey, CacheEntry*> unused;

	public:
	bool		In (const Tkey& key) const
					{ return used.find(key) != used.end() || unused.find(key) != unused.end(); }

	void		Add (const Tkey& key, const Tval& val) {
					DASSERT(!In(key));
					used[key] = DNEWCLASS(CacheEntry, (val));
				}

	Tval&		Refer (const Tkey& key) { 

					typename std::map<Tkey, CacheEntry*>::iterator i = used.find(key);
					CacheEntry* p = (CacheEntry*) 0;

					if (i == used.end()) { // Not in used? then it should be in unused.
						i = unused.find(key);
						DASSERT(i != unused.end());
						p = i->second;
						used.insert(*i); // Move to used cahced items.
						unused.erase(i);
					}
					else
						p = i->second;

					DPTR(p)->IncRefCounter();
					return DPTR(p)->Get();
				}

	void		Unrefer (const Tkey& key) {

					typename std::map<Tkey, CacheEntry*>::iterator i = used.find(key);
					DASSERT(i != used.end());

					CacheEntry* p = i->second;
					DPTR(p)->DecRefCounter();

					if (!DPTR(p)->IsUsed()) 
						{ unused.insert(*i); used.erase(i); }
				}

	void		HandleExpirations (util_ui32 currTime) {
					typename std::map<Tkey, CacheEntry*>::iterator i = unused.begin();
					while (i != unused.end()) {
						CacheEntry* p = i->second;
						if (DPTR(p)->HasExpired(currTime)) {
							DDELETE(p);
							unused.erase(i++);
						}
						else
							++i;
					}
				}

	void		Initialise (void)	
					{ DASSERT(used.empty() && unused.empty()); }

	// Before clean-up, all referring to cached items should unrefer.

	void		CleanUp (void) {
					std::for_each(
						used.begin(), used.end(), 
						udestroy_second< std::pair<Tkey, CacheEntry*> >()
					);
					used.clear();
					std::for_each(
						unused.begin(), unused.end(), 
						udestroy_second< std::pair<Tkey, CacheEntry*> >()
					);
					unused.clear();
				}

	ucachememory(void){}
	~ucachememory() { CleanUp(); }
};

//---------------------------------------------------------------
// Caching of items using tags that are checked on-demand.
// Appropriate if the frequency of demands is smaller
// compared to the frequency of evens, thus rendering a
// callback mechanism for cache updates less preferrable.
// T: T(void); T(const T&); void operator=(const T&);

template <typename T> class utagged_cached_item {

	protected:
	T			val;
	util_ui32	tag;

	public:
	class proxy {
		protected:
		const utagged_cached_item*	item;
		util_ui32					accessTag;

		public:
		UOVERLOADED_ASSIGN_VIA_COPY_CONSTRUCTOR(proxy)
		bool				IsDirty (void) const
								{ DASSERT(item); return item->IsDirty(accessTag); }
		void				Update (void)
								{ DASSERT(item); accessTag = item->GetTag(); }		
		void				Set (const utagged_cached_item& _item)
								{ item = &_item; accessTag = _item.GetTag(); }
		void				operator=(const utagged_cached_item& _item) 
								{ Set(_item); }
		proxy (void) : 
			item		((const utagged_cached_item*) 0), 
			accessTag	(0)
			{}
		proxy (const utagged_cached_item& _item) : 
			item		(&_item), 
			accessTag	(_item.GetTag())
			{}
		proxy (const proxy& p) : 
			item		(p.item), 
			accessTag	(p.accessTag)
			{}
		virtual ~proxy(){}
	};

	void		Set (const T& _val) 
					{ val = _val; ++tag; }
	const T&	Get (void) const
					{ return val; }
	util_ui32	GetTag (void) const 
					{ return tag; }
	bool		IsDirty (util_ui32 _tag) const
					{ return tag != _tag; }	
	utagged_cached_item (void) : tag(1){}
	virtual ~utagged_cached_item(){}
};

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.

