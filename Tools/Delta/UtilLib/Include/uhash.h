// uhash.h
// Hash utilities and templates.
// Moved from the central delta util here.
// ScriptFighter Project.
// A. Savidis, July 2002.
//

#ifndef	UHASH_H
#define	UHASH_H

#include <string.h>
#include "DDebug.h"
#include "ufunctors.h"
#include "ustrings.h"
#include "utypes.h"
#include "usmartlist.h"

//---------------------------------------------------------------

UTILLIB_FUNC util_ui32 uhash (const std::string& s, util_ui32 hashSize);

/////////////////////////////////////////////////////////////////
// The following is a general purpose hash map 
// which uses local copying of keys. 
//
#define	UHASHTABLE_HASH_SIZE	211
		
/////////////////////////////////////////////////////////////////
// Hash key functors.
//

template <class T> class ukeynumber  {

	public:
	typedef const T CRef;
	typedef T Ref;

	bool		cmp (CRef t1, CRef t2) { return t1==t2; }
	const T		copy (CRef t) { return t; }
	void		clear (Ref t) {}
	util_ui8	hash (CRef t) { return ((util_ui32) t) % UHASHTABLE_HASH_SIZE; }
};

//***********************

template <class T> class ukeypointer  {

	public:
	typedef const T CRef;
	typedef T Ref;

	bool		cmp (CRef t1, CRef t2) { return ((const void*) t1) == ((const void*) t2); }
	const T		copy (CRef t) { return t; }
	void		clear (Ref t) {}
	util_ui8	hash (CRef t) { return ((util_ui32) t) % UHASHTABLE_HASH_SIZE; }
};

//***********************

class ukeyuint	: public ukeynumber<util_ui32> {};
class ukeyushort: public ukeynumber<util_ui16> {};
class ukeyuchar	: public ukeynumber<util_ui8> {};

//***********************

class UTILLIB_CLASS ukeystring {

	public:
	typedef const std::string& CRef;
	typedef std::string& Ref;

	bool				cmp (CRef t1, CRef t2) { return t1 == t2; }
	const std::string&	copy (CRef t) { return t; }
	void				clear (Ref t) {}
	util_ui8			hash (CRef t) { return (util_ui8) uhash(t.c_str(), UHASHTABLE_HASH_SIZE); }
};

/////////////////////////////////////////////////////////////////

template <class K, class T, class TKey, class TAssign = uassigndefaultfunc<T> > class uhashmap {
	
	/////////////////////////////////////////////////////////////////

	private:
	
	//***********************
	// The hash item implements a doubly linked list,
	// while each item holds its bucket order. We keep a smart list
	// that holds all created items, to speed-up the iteration process.
	//
	struct uhashitem {

		T			val;	// Value.
		K			key;	// Key.
		util_ui8	pos;	// Bucket order in hash table.
		uhashitem*	next;
		uhashitem*	prev;

		bool Invariant (void) const {
			return	DPTR(this) &&
					(pos < UHASHTABLE_HASH_SIZE) &&
					(!next || next->prev == this) &&
					(!prev || prev->next == this);
		}

		//***************************

		void disconnect (bool checkNeighbors = true) {

			if (next) {
				DASSERT(!checkNeighbors || next->Invariant());
				next->prev = prev;
			}

			if (prev) {
				DASSERT(!checkNeighbors || prev->Invariant());
				prev->next = next;
			}
			next = prev = (uhashitem*) 0;
		}

		//***************************

		uhashitem (typename TKey::CRef _key, T _val, util_ui8 _pos) :
			key(TKey().copy(_key)),
			pos(_pos),
			next((uhashitem*) 0),
			prev((uhashitem*) 0) { TAssign()(val, _val); }

		~uhashitem() { DASSERT(Invariant()); TKey().clear(key); }
	};

	//***********************

	uhashitem*				table[UHASHTABLE_HASH_SIZE];
	usmartlist<uhashitem*>	iterationList;
	util_ui32				totalItems;

	bool Invariant (void) const 
		{ return totalItems == iterationList.total(); }

	/////////////////////////////////////////////////////////////////

	public:

	class uiterator;
	friend class uiterator;

	//***********************
	// Iterator class for hash maps. If items are removed while
	// iterating, not very safe.
	//
	class uiterator {

		private:
		friend class uhashmap<K, T, TKey, TAssign>;

		uhashmap<K, T, TKey, TAssign>*				myMap;			// My map at which pointing.
		typename usmartlist<uhashitem*>::uiterator	iter;			// The list iterator.

		//****************************
		// Setting an iterator explicitly.
		//
		void		set (
						const uhashmap<K, T, TKey, TAssign>*				_myMap, 
						const typename usmartlist<uhashitem*>::uiterator&	_iter
					) {

						myMap = const_cast< uhashmap<K, T, TKey, TAssign>* >(_myMap);
						iter = _iter;
					}

		//****************************
		//  This constructor is private.
		//
		uiterator (
				const uhashmap<K, T, TKey, TAssign>*				_myMap, 
				const typename usmartlist<uhashitem*>::uiterator&	_iter
				) :

			myMap(const_cast< uhashmap<K, T, TKey, TAssign>* >(_myMap)), iter(_iter){}

		public:
		
		//****************************

		T&					val (void) const { return DPTR(*iter)->val; }
		typename TKey::Ref	key (void) const { return DPTR(*iter)->key; }

		uiterator&	operator++(void) { ++iter; return *this; }

		bool operator==(const uiterator& _iterator) const { 
				return	myMap	== _iterator.myMap	&& 
						iter	== _iterator.iter;
		}

		bool operator!=(const uiterator& _iterator) const { 
				return	myMap	!= _iterator.myMap	|| 
						iter	!=	_iterator.iter;
		}

		bool valid (void) const { return myMap && iter.valid(); }

		//****************************

		uiterator& operator=(const uiterator& _iterator) {

			if (&_iterator != this)
				set(_iterator.myMap, _iterator.iter);

			return *this;
		}

		//****************************

		void invalidate (void) 
			{ iter.invalidate(); }

		//****************************
		// Copy constructor.

		uiterator (const uiterator& _iterator) {
			myMap = _iterator.myMap;
			iter = _iterator.iter;
		}

		//****************************
		// Default constructor.
		//
		uiterator (void) :	myMap ((uhashmap<K, T, TKey, TAssign>*) 0) {}
		~uiterator() {}

	};

	typedef uiterator iterator;

	/////////////////////////////////////////////////////////////////

	private:

	uiterator			endIterator;

	//***********************

	uhashitem*			lookupinternal (typename TKey::CRef key) {
						
							uhashitem* bucket = table[TKey().hash(key)];
						
							while (bucket && !TKey().cmp(DPTR(bucket)->key, key)) {
								DASSERT(bucket->Invariant());
								bucket = bucket->next;
							}
						
							return bucket;
						}

	//***********************

	void				handleheadremoval (uhashitem* p) {
							if (!p->prev) {		// First element.
								DASSERT(table[p->pos] == p);
								table[p->pos] = p->next;
							}
						}

	//***********************

	void				clearbucket (uhashitem* bucket) {

							while (bucket) {

								uhashitem* item = bucket;
								DASSERT(item->Invariant());

								bucket = DPTR(bucket)->next;
								item->disconnect(false);

								DDELETE(item);
								--totalItems;
							}
						}
				
	/////////////////////////////////////////////////////////////////

	public:

	const uiterator		begin (void) const { 
							DASSERT(Invariant());
							return uiterator(this, iterationList.begin());
						}

	const uiterator&	end (void) const { 
							DASSERT(Invariant());
							return endIterator; 
						}

	//***********************
	// When an item already exists with that name, 
	// installation fails, unless 'changeIfExists' is true.
	// Newlly created items are always put in front of the
	// hash bucket list.
	//
	bool	install (typename TKey::CRef key, T val, bool changeIfExists = false) {

		DASSERT(Invariant());
		uhashitem* bucket;
		if (bucket = lookupinternal(key)) {
			if (changeIfExists) {
				TAssign()(DPTR(bucket)->val, val);
				return true;
			}
			else
				return false;
        }

		util_ui8 pos;
		bucket = table[pos = TKey().hash(key)];
		uhashitem* item = DNEWCLASS(uhashitem, (key, val, pos));
		DPTR(item)->next = bucket;

		if (bucket)
			DPTR(bucket)->prev = item;

		table[pos] = item;
		iterationList.push_back(item);
		++totalItems;

		return true;
	}

	//***********************
	// Returns internal ptr to stored item.
	//
	T*	lookup (typename TKey::CRef key) {
		DASSERT(Invariant());
		uhashitem* p = lookupinternal(key);

		if (p) {
			DASSERT(p->Invariant());
			return &DPTR(p)->val;
		}
		else
			return (T*) 0;
	}

	//***********************
	// Returns end() if not found.
	//
	uiterator	lookupiter (typename TKey::CRef key) {
		DASSERT(Invariant());
		return uiterator(this, lookupinternal(key));
	}

	//***********************

	bool lookup (typename TKey::CRef key, T* val) {

		DASSERT(Invariant());
		uhashitem* p = lookupinternal(key);

		if (p) {
			DASSERT(p->Invariant());
			TAssign()(*val, DPTR(p)->val);
			return true;
		}
		else
			return false;
	}

	//***********************
	// When removing, we disconnect the element in both directions
	// since the list is double linked list. Also, when removing the
	// head element of a bucket, we update the bucket head. Finally,
	// we remove from the iteration list this is why removal takes
	// some time).
	//
	bool	remove (typename TKey::CRef key) {

		DASSERT(Invariant());
		if (!totalItems)		// No items in the hash table.
			return false;

		uhashitem* p = lookupinternal(key);

		if (p) {

			handleheadremoval(p);
			p->disconnect();

			iterationList.remove(p);
			--totalItems;
			DDELETE(p);

			return true;
		}
		else
			return false;
	}

	//***********************
	// When an item is removed via an iterator, the iterator
	// is invalidated.
	//
	void	erase (uiterator& _iterator) {

		DASSERT(Invariant());
		DASSERT(totalItems);
		DASSERT(_iterator.valid());

		uhashitem* p = *(_iterator.iter);
		DASSERT(p->Invariant());

		handleheadremoval(p);
		p->disconnect();

		iterationList.remove(_iterator.iter);
		--totalItems;

		DDELETE(p);
	}

	//***********************
	// Clear the entire hash table.
	//
	void	clear (void) {

		DASSERT(Invariant());

		if (totalItems) {
			for (util_ui8 i = 0; i < UHASHTABLE_HASH_SIZE; ++i) {
				clearbucket(table[i]);
				table[i] = (uhashitem*) 0;
			}

			iterationList.clear();
			DASSERT(!totalItems);
		}
	}

	//***********************
	// This implementation of iterate, allows the iteration callback
	// to affect freely the table (the reason being that a smart list is
	// used for iteration).
	//
	void	iterate (bool (*callback)(T& val, typename TKey::Ref key, void* closure), void* closure ) {

		DASSERT(Invariant());
		for (	typename usmartlist<uhashitem*>::uiterator pos = iterationList.begin();
				pos.valid() && pos != iterationList.end();
				++pos) {
			
				DASSERT((*pos)->Invariant());

				if (!(*callback)((*pos)->val, (*pos)->key, closure))
					return;
		}
	}

	//***********************

	util_ui32 total (void) const 
		{ return totalItems; }

	//***********************

	uhashmap (void) {

		totalItems = 0;
		for (util_ui8 i=0; i < UHASHTABLE_HASH_SIZE; ++i)
			table[i] = (uhashitem*) 0;

		endIterator.set(this, iterationList.end());
	}

	~uhashmap() { clear(); }

};

/////////////////////////////////////////////////////////////////
// This is a helper class defined for string hash tables.
//
template <class T, class TAssign = uassigndefaultfunc<T> > 
class uhashtable : public uhashmap<std::string, T, ukeystring, TAssign> {};

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.

