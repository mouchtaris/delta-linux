// ustlhelpers.h
// Some helper functions for the STL library.
// Moved from the central delta util here.
// ScriptFighter Project.
// A. Savidis, July 2002.
//

#ifndef	USTLHELPERS_H
#define	USTLHELPERS_H


#include <list>
#include <vector>
#include <algorithm>

#include "DDebug.h"
#include "ufunctors.h"
#include "utypes.h"

//---------------------------------------------------------------
// This gets a list iterator  by its index. It should be used only
// after checking that there are at least 'index'+1 items in the list.
//

template <typename T>
typename std::list<T>::iterator ulistgetiterbyindex (std::list<T>& items, util_ui32 index) {

	DASSERT(index < items.size());

	typename std::list<T>::iterator pos;
	for (pos = items.begin(); index;--index, ++pos) 
		{}

	return pos;
}

template <typename T>
typename std::list<T>::const_iterator ulistgetiterbyindex (const std::list<T>& items, util_ui32 index) {

	DASSERT(index < items.size());

	typename std::list<T>::const_iterator pos;
	for (pos = items.begin(); index;--index, ++pos) 
		{}

	return pos;
}

//*******************************

template <typename T>
void ulistcopy (const std::list<T>& from, std::list<T>& to) {

	for (	typename std::list<T>::const_iterator item = from.begin(); 
			item != from.end();
			++item)

			to.push_back(*item);
}

//*******************************

template <typename T>
void ulistrangecopy (std::list<T>& from, util_ui32 start, util_ui32 n, std::list<T>& to) {

	DASSERT(start + n <= from.size());
	
	for (	typename std::list<T>::const_iterator item = from.begin(); 
			item != from.end();
			++item)

			if (start)
				--start;
			else
			if (n) {
				to.push_back(*item);
				--n;
			}
			else
				break;
}

//*******************************

template <typename T>
T& ulistgetbyindex (std::list<T>& items, util_ui32 index) {
	return *ulistgetiterbyindex(items, index);
}

template <class T>
const T& ulistgetbyindex (const std::list<T>& items, util_ui32 index) {
	return *ulistgetiterbyindex(items, index);
}

template <class T>
void ulist2vector (const std::list<T>& l, std::vector<T>& v) {
	v.reserve(l.size());
	for (typename std::list<T>::const_iterator i = l.begin(); i != l.end(); ++i)
		v.push_back(*i);
}

/////////////////////////////////////////////////////////////////
// Clear a list with dynamic pointer arguments.
// The destructor functor class can be supplied as a template
// argument (the default uses the delete language operator).
//
template <class T, class Tdestroy = uptrdestructorfunctor<T*> > class ulistclear {

	public:
	void operator() (std::list<T*>& items) {
		std::for_each(items.begin(), items.end(), Tdestroy());
		items.clear();
	}
};

/////////////////////////////////////////////////////////////////
// Getting a vector iterator, instead of an item directly with the
// overloaded [] operator. Asserts that index is not out of bounds.
//
template <class T>
typename std::vector<T>::iterator uvectorget (std::vector<T>& array, util_ui32 index) {

	DASSERT(index < array.size());
	typename std::vector<T>::iterator pos;
	for (pos= array.begin(); index; --index, ++pos) 
		{}
	return pos;
}

/////////////////////////////////////////////////////////////////
// Getting a vector iterator, instead of an item directly with the
// overloaded [] operator. Asserts that index is not out of bounds.
//
template <class T>
bool ulistiteratback (const typename std::list<T>::iterator& i, const std::list<T>& l)
	{ return &*i == &l.back(); }

template <class T>
bool ulistiteratback (const typename std::list<T>::const_iterator& i, const std::list<T>& l)
	{ return &*i == &l.back(); }

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
