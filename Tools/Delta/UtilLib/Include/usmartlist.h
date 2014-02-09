// usmartlist.h
// Smart list template class.
// Moved from the central delta util here.
// ScriptFighter Project.
// A. Savidis, July 2002.
//

#ifndef	USMARTLIST_H
#define	USMARTLIST_H

#include <string.h>
#include <list>
#include <algorithm>

#include "DDebug.h"
#include "ufunctors.h"
#include "utypes.h"

//---------------------------------------------------------------
// usmartlist is  a list template class, supportng iterators
// which are automatically updated when the pointed element gets
// invalidated. Only fwd iterators are supported, and when invalidation
// takes place the iterator moves fwd too. When the list gets destroyed,
// iterators are also invalidated.
//

/////////////////////////////////////////////////////////////////
// The smart list template definition.
//
template <class T,  class TCmp = ucmpdefaultfunc<T> > class usmartlist {


	public:
	class uiterator;

	private:
	friend class uiterator;

	//****************************
	// List item type. Bidirectional list for quick removal.
	//
	class ulistitem {

		public:
		
		T			val;
		ulistitem*	next;
		ulistitem*	prev;

		bool Invariant (void) const {
			return	DPTR(this) &&
					(!next || DPTR(next)->prev == this) &&
					(!prev || DPTR(prev)->next == this);
		}

		T&			value (void) { DASSERT(Invariant()); return val; }
		T*			pointer (void) { DASSERT(Invariant()); return &val; }

		ulistitem (const T& _val): val(_val), prev((ulistitem*) 0), next((ulistitem*) 0) {}
		ulistitem (void): prev((ulistitem*) 0), next((ulistitem*) 0) {}
		~ulistitem() { DASSERT(Invariant()); }
	};

	public:

	//****************************
	// Fwd iterator class for lists.
	//
	class uiterator {

		private:
		friend class usmartlist<T,TCmp>;

		bool				doNotAdvance;		// In next ++, do not move fwd.
		usmartlist<T,TCmp>*	myList;				// My list at which pointing.
		ulistitem*			curr;				// Current element.

		//****************************
		// Setting an iterator explicitly.
		//
		void set (usmartlist<T,TCmp>* _myList, ulistitem* _curr, bool _doNotAdvance=false) {

			if (myList) {
				if (myList!=_myList) {

					myList->removeiterator(this);
					myList = _myList;
					myList->additerator(this);
				}
			}
			else {
					myList = _myList;
					myList->additerator(this);
			}

			curr=_curr;
			doNotAdvance=_doNotAdvance;
		}

		//****************************
		// When invalidating list too, either exlpicit removal is needed afterwards, or the
		// iterator should not be a member of the list.
		// 
		void invalidate (bool listToo) {
				DASSERT(Invariant());
				curr = (ulistitem*) 0; 
				doNotAdvance = false;
				if (listToo && myList)
					myList = (usmartlist*) 0;
		}

		//****************************
		//  This constructor is private.
		//
		uiterator (usmartlist<T,TCmp>* _myList, ulistitem* _curr) :
			myList (_myList), 
			curr (_curr), 
			doNotAdvance (false) {
				if (myList)
					myList->additerator(this);
			}

		/////////////////////////////////////////////////////////////////

		public:

		bool Invariant (void) const {
			return	(((util_ui8) doNotAdvance) <= 1)	&&
					(!curr || curr->Invariant())		&&
					(!myList || myList->ismyiterator(*this));
		}

		//****************************

		uiterator& fwd (void) {

			DASSERT(Invariant());
			if (valid()) {
				if (doNotAdvance)
					doNotAdvance = false;
				else
					curr = curr->next;
			}
			DASSERT(Invariant());

			return *this;
		}

		//****************************

		uiterator& bwd (void) { 
			DASSERT(Invariant());
			if (valid())
				curr = curr->prev;
			DASSERT(Invariant());

			return *this;
		}

		typedef uiterator iterator;

		//****************************

		T&			operator*(void) const { DASSERT(Invariant()); return curr->val; }
		T*			operator->(void) const { DASSERT(Invariant()); return &curr->val; }
		uiterator&	operator++(void) { return fwd(); }
		uiterator&	operator--(void) { return bwd(); }

		bool operator==(const uiterator& _iterator) const 
			{ DASSERT(Invariant()); return myList==_iterator.myList && curr==_iterator.curr; }

		bool operator!=(const uiterator& _iterator) const 
			{ DASSERT(Invariant()); return myList!=_iterator.myList || curr!=_iterator.curr; }

		bool		valid (void) const 
						{ return myList && curr; }

		void		setdonotadvance (bool val) 
						{ DASSERT(Invariant()); doNotAdvance = val; }

		void		setcurr (ulistitem* p) {
						DASSERT(Invariant()); 
						if (curr = p)
							DASSERT(curr->Invariant()); 
					}

		void		setlist (usmartlist<T,TCmp>* l) 
						{ DASSERT(Invariant()); myList = l; }

		//****************************

		uiterator& operator=(const uiterator& _iterator) {
			DASSERT(Invariant());
			if (&_iterator != this)
				set(_iterator.myList, _iterator.curr, _iterator.doNotAdvance);
			return *this;
		}

		//****************************
		// Copy constructor.

		uiterator (const uiterator& _iterator) {
			if (myList = _iterator.myList)
				myList->additerator(this);
			curr = _iterator.curr;
			doNotAdvance = _iterator.doNotAdvance;
		}

		//****************************
		// Default constructor.
		//
		uiterator (void) :	myList ((usmartlist<T,TCmp>*) 0), 
							curr ((ulistitem*) 0), 
							doNotAdvance (false) {}

		~uiterator() { 	
			
			DASSERT(Invariant());

			if (myList) {

				// If the iterator is not a built-in begin / end iterator,
				// then remove the iterator.
				//
				if ((&myList->beginIterator)!=this && (&myList->endIterator)!=this)
					myList->removeiterator(this); 
			}
		}

	};

	/////////////////////////////////////////////////////////////////

	private:
	ulistitem*				head;
	ulistitem*				tail;
	util_ui32				totalItems;
	std::list<uiterator*>	myIterators;
	uiterator				beginIterator;
	uiterator				endIterator;

	const ulistitem* after (ulistitem* p, util_ui32 n) const
		{ return n <=1 ? p : after(p->next, n-1); }

	bool Invariant (void) const {
		return	(totalItems || !head && !tail)		&&
				(totalItems!=1 || head==tail)		&&
				(totalItems <= 1 || head != tail)	&&
				(after(head, totalItems)==tail)		&&
				(beginIterator.Invariant())			&&
				(endIterator.Invariant());
	}

	void removeiterator (uiterator* _iterator) { 
			DASSERT(Invariant()); 
			myIterators.remove(_iterator); 
	}

	void additerator (uiterator* _iterator) { 
			DASSERT(Invariant()); 
			myIterators.push_back(_iterator); 
	}

	bool ismyiterator (const uiterator& _iterator) const { 
			return	_iterator.myList == this &&
					(
					&_iterator == &beginIterator|| 
					&_iterator == &endIterator	||
					std::find(
						myIterators.begin(), 
						myIterators.end(), 
						&_iterator
					) != myIterators.end()
					); 
	}

	//****************************
	// Those iterators which have curr == pos,
	// move fwd automatically and are set to doNotAdvance = true. This
	// call is to be made ONLY when an item is removed, so that
	// affected iteratos can still be valid. The call is made AFTER pos
	// has been disconnected from the main list, i.e. it is isolated.
	//
	void forwardbycurrent (ulistitem* pos) {

		for (	typename std::list<uiterator*>::iterator _iterator = myIterators.begin();
				_iterator != myIterators.end();
				++_iterator	)

				if ((*_iterator)->curr == pos) {

					// We need to enable advancement. The reason is that, if we don't do that,
					// any two consecutive removals of the current element will cause the second 
					// fwd() below to hold a dangling pointer to the removed element.
					// The only way to avoid that is to always move forward upon removal of
					// the current element.
					//
					(*_iterator)->setdonotadvance(false);
					(*_iterator)->fwd();
					(*_iterator)->setdonotadvance(true);
				}
	}

	//****************************
	// Disconnect an item from the list. This is done
	// to remove item, prior to destruction. Check if head or tail
	// are affected.
	//
	void disconnect (ulistitem* p) {
		
		DASSERT(p->Invariant());

		if (p->next) {
			DASSERT(p->next->Invariant());
			p->next->prev = p->prev;
		}
		else
			tail = p->prev;					// Tail removed.

		if (p->prev) {
			DASSERT(p->prev->Invariant());
			p->prev->next = p->next;
		}
		else {
			head = p->next;					// Head removed.
			beginIterator.setcurr(head);	// Update begin iterator.
		}

		p->next = p->prev = (ulistitem*) 0;	// Reset its connectivity anyway.
	}

	public:

	//****************************

	void push_back (const T& val) {

		DASSERT(Invariant()); 

		if (!head)
			push_front(val);
		else {
			ulistitem* add = DNEWCLASS(ulistitem, (val));
			++totalItems;

			if (!tail)
				head = tail = add;
			else {
				DASSERT(tail->Invariant());
				tail->next = add;
				add->prev = tail;
				tail = add;
			}
		}

		DASSERT(Invariant()); 
	}

	//****************************

	void push_front (const T& val) {

		DASSERT(Invariant()); 

		ulistitem* add = DNEWCLASS(ulistitem, (val));
		++totalItems;

		if (!tail)
			head = tail = add;
		else {
			DASSERT(head->Invariant());
			head->prev = add;
			add->next = head;
			head = add;
		}

		beginIterator.setcurr(head);	// We have a new begin item.

		DASSERT(Invariant()); 
	}

	//****************************
	// If invalid iterator pushes at the end.
	// If put after tail, pushes also at the end.
	//
	void insert_after (const uiterator& _iterator, const T& val) {

		DASSERT(Invariant()); 

		if (!_iterator.valid() || !ismyiterator(_iterator) || _iterator.curr==tail)
			push_back(val);
		else {
			ulistitem* pos = _iterator.curr;
			ulistitem* add = DNEWCLASS(ulistitem, (val));
			++totalItems;

			DASSERT(pos->next->Invariant());

			pos->next->prev = add;
			add->next = pos->next;
			add->prev = pos;
			pos->next = add;
		}

		DASSERT(Invariant()); 
	}

	//****************************
	// Inserts before. If invalid iterator pushes
	// at the end. If iterator is head, pushes in front.
	//
	void insert_before (const uiterator& _iterator, const T& val) {

		DASSERT(Invariant()); 

		if (!_iterator.valid() || !ismyiterator(_iterator))
			push_back(val);
		else
		if (_iterator.curr==head)
			push_front(val);
		else {
			ulistitem* pos = _iterator.curr;
			ulistitem* add = DNEWCLASS(ulistitem, (val));
			++totalItems;

			DASSERT(pos->prev->Invariant());

			pos->prev->next = add;
			add->prev = pos->prev;
			add->next = pos;
			pos->prev = add;
		}

		DASSERT(Invariant()); 
	}

	//****************************

	bool remove (const T& val) {

		DASSERT(Invariant()); 

		TCmp cmp;
		for (ulistitem* p=head; p; p=p->next) {

			DASSERT(p->Invariant());

			if (cmp(val, p->val)) {

				disconnect(p);
				forwardbycurrent(p);

				DDELETE(p);
				--totalItems;

				DASSERT(Invariant()); 
				return true;
			}
		}

		DASSERT(Invariant()); 
		return false;
	}

	//****************************
	// Removes only first found.
	//
	bool remove (bool (*filterfunc)(const T&, void*), void* closure=(void*) 0) {

		DASSERT(Invariant()); 
		TCmp cmp;

		for (ulistitem* p=head; p; p=p->next) {

			DASSERT(p->Invariant());

			if ((*filterfunc)(p->val, closure)) {

				disconnect(p);
				forwardbycurrent(p);

				DDELETE(p);
				--totalItems;

				DASSERT(Invariant());
				return true;
			}
		}

		return false;
	}

	//****************************
	// When an item is removed via an iterator, the iterator has
	// to be invalidated, and also to be removed from
	// the list of iterators of this list. Also, the iterators
	// pointing to the current item, must be moved forward.
	//
	bool remove (uiterator& _iterator) {

		DASSERT(Invariant()); 

		if (_iterator.valid() && ismyiterator(_iterator)) {

			ulistitem* item = _iterator.curr;

			forwardbycurrent(item);
			disconnect(item);

			DDELETE(item);
			--totalItems;

			DASSERT(Invariant()); 
			return true;
		}

		DASSERT(Invariant()); 
		return false;
	}

	//****************************

	const uiterator&	begin (void) const { 
							DASSERT(Invariant()); 
							return beginIterator; 
						}

	const uiterator&	end (void) const { 
							DASSERT(Invariant()); 
							return endIterator; 
						}

	util_ui32			total (void) const 
							{ DASSERT(Invariant()); return totalItems; }

	//****************************

	void clear (bool init = true) {

		DASSERT(Invariant()); 

		// We firstly invalidate the iterators.
		//
		for (	typename std::list<uiterator*>::iterator _iterator = myIterators.begin();
				_iterator != myIterators.end();
				++_iterator)

				(*_iterator)->invalidate(true);

		beginIterator.invalidate(false);
		myIterators.clear();

		// Then we destroy the list and the elements.
		//
		for (ulistitem* p=head; p; ) {

			DASSERT(totalItems);
			p->prev = (ulistitem*) 0; // Needed to satisy invariant.
			DASSERT(p->Invariant());

			ulistitem* del = p;
			p = p->next;

			del->next = del->prev = (ulistitem*) 0; // Needed to satisy invariant.
			DDELETE(del);
			--totalItems;
		}

		if (init) {
			head = tail = (ulistitem*) 0;
			beginIterator.curr = (ulistitem*) 0;
			totalItems = 0;
		}
	}

	//****************************
	// Find with a value comparison.
	//
	void find (const uiterator& beginFrom, const T& val, uiterator& result) {

		DASSERT(Invariant()); 

		if (!beginFrom.valid() || !ismyiterator(beginFrom)) {
			result.invalidate(true);
			return;
		}

		TCmp cmp;
		for (ulistitem* p = beginFrom.curr; p; p = p->next) {

			DASSERT(p->Invariant());

			if (cmp(p->val, val)) {
				result.set(this, p);
				return;
			}
		}

		result = end();
	}

	//****************************
	// Find with a filter function.
	//
	void find (const uiterator& beginFrom, bool (*filterfunc)(const T&, void*), uiterator& result, void* closure=(void*) 0) {

		DASSERT(Invariant()); 

		if (!beginFrom.valid() || !ismyiterator(beginFrom)) {
			result.invalidate(true);
			return;
		}

		for (ulistitem* p = beginFrom.curr; p; p = p->next) {

			DASSERT(p->Invariant());

			if ((*filterfunc)(p->val, closure)) {
				result.set(this, p);
				return;
			}
		}

		result = end();
	}

	//****************************
	// Find with a filter comparison class.
	//
	class Tcompare {
		public:
		virtual bool operator() (const T& val, void* closure) const =0 ;
	};

	void find (const uiterator& beginFrom, const Tcompare& _Tcompare, uiterator& result, void* closure=(void*) 0) {

		DASSERT(Invariant()); 

		if (!beginFrom.valid() || !ismyiterator(beginFrom)) {
			result.invalidate(true);
			return;
		}

		for (ulistitem* p = beginFrom.curr; p; p = p->next) {

			DASSERT(p->Invariant());

			if (_Tcompare(p->val, closure)) {
				result.set(this, p);
				return;
			}
		}

		result = end();
	}

	//****************************

	usmartlist (void) { 
			head = tail = (ulistitem*) 0; 
			totalItems = 0;
			beginIterator.myList = endIterator.myList = this;
	}

	~usmartlist() { clear(false); }		
};

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
