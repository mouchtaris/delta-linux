// uarraydoublelist.h
// A template class for making doubly linked lists
// stored in arrays, without requiring dynamic memory allocation.
// A. Savidis, November 2002.
//

#ifndef	UARRAYDOUBLELIST_H
#define	UARRAYDOUBLELIST_H

#include "DDebug.h"
#include "utypes.h"

//---------------------------------------------------------------
// This is a super class of a doubly linked item that can be put
// in an array-stored double linked list.
//
class udoublylinked {

	/////////////////////////////////////////////////////////////

	private:
	udoublylinked* next;
	udoublylinked* prev;

	/////////////////////////////////////////////////////////////

	public:
	udoublylinked*	getnext (void) const 
						{ return next; }
	udoublylinked*	getprev (void) const 
						{ return prev; }
	void			reset (void) 
						{ next = prev = (udoublylinked*) 0; }

	virtual void	init (void) = 0;

	void			linkbefore (udoublylinked* item) {
						if (!item) 
							reset();
						else {
							prev = item->prev;
							next = item;
							item->prev = this;
						}
					}
					
	void			linkafter (udoublylinked* item) {
						if (!item)
							reset();
						else {
							next = item->next;
							prev = item;
							item->next = this;
						}
					}
					
	void			disconnect (void) {
						if (prev)
							prev->next = next;
						if (next)
							next->prev = prev;
						reset();
					}

	/////////////////////////////////////////////////////////////

	udoublylinked (void) :
			next((udoublylinked*) 0),
			prev((udoublylinked*) 0)
			{}

	~udoublylinked(){}
};

//---------------------------------------------------------------

template <class TItem, const util_ui32 N> class uarraydoublelist {

	private:

	/////////////////////////////////////////////////////////////

	TItem		array[N];
	util_ui32	totalFree;
	util_ui32	totalUsed;
	TItem*		usedList;
	TItem*		freeList;


	public:
	
	/////////////////////////////////////////////////////////////

	bool	isfull (void) const 
				{ return !freeList; }

	TItem*	allocate (void) {

				// Get a free item and disconnect it. Always
				// the head free list element is taken.
				//
				DASSERT(!isfull());
				TItem* item = freeList;
				freeList = (TItem*) freeList->getnext();
				item->disconnect();
				--totalFree;
				
				// Now add it in used list.
				//
				item->linkbefore(usedList);
				++totalUsed;
				usedList = item;

				return item;
			}
		
	//****************************

	void	release (TItem* item) {
				
				// If removing used list head, move it forward.
				//
				DASSERT(item);
				if (item==usedList)
					usedList = (TItem*) usedList->getnext();

				// Disconnect it from used list, and connect
				// it to the free list.
				//
				item->disconnect();
				--totalUsed;

				item->linkbefore(freeList);

				if (freeList) {
					DASSERT(freeList->getprev()==item);
					DASSERT(item->getnext()==freeList);
				}

				++totalFree;
				freeList = item;
			}

	//****************************

	TItem*	head (void) 
				{ return usedList; }

	//****************************

	void	init (void) {
				
				array[0].reset();
				array[0].init();
				
				for (util_ui32 i = 1; i < N; ++i) {

					array[i].reset();
					array[i].init();
					array[i].linkafter(&array[i-1]);

					DASSERT(array[i].getprev()==&array[i-1]);
					DASSERT(array[i-1].getnext()==&array[i]);
				}

				freeList = &array[0];
				totalUsed = 0;
				totalFree = N;
				usedList = (TItem*) 0;
			}

	//****************************

	void	clear (void) 
				{ init(); }

	/////////////////////////////////////////////////////////////

	uarraydoublelist (void) { init(); }
	~uarraydoublelist(){}
};

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
