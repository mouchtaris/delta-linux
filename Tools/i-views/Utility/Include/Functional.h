/**
 *	Functional.h
 *
 *	-- Utility Library --
 *	Header file with several objects useful
 *	for functional composition.
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	June 2010.
 */

#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#include <list>
#include <utility>
#include <assert.h>
#include <functional>


namespace utility {

	//
	template<class T>
	struct ComparatorFunctor : public std::binary_function<const T *, const T *, bool>{
		bool operator() (const T * obj1, const T * obj2) const
			{	return obj1->Id() < obj2->Id();	}
	};
	
	//-----------------------------------------------------------------------

	template<class T>
	struct EqualityFunctor : public std::unary_function<const T *, bool> {
		const T & object;
		EqualityFunctor(const T & obj) : object(obj) {}

		bool operator() (const T * obj) const {
			assert (obj != static_cast<T *>(0));
			return object == *obj;
		}
	};

	//-----------------------------------------------------------------------

	template<class T>
	struct DeletePointersFromList : public std::unary_function<T *, void> {
		void operator() (T * obj) const { 
			delete obj; 
			obj = static_cast<T *>(0);
		}
	};

	//-----------------------------------------------------------------------
	// erase each element matching Val
	template<class T>
	void RemovePointerFromList(std::list<T *> & l, const T & Val) {	
		typedef std::list<T *> List;
		for (List::iterator first = l.begin(); first != l.end(); )
			if (*first == Val) {
				delete (*first);
				first = l.erase(first);
			}
			else
				++first;
	}

}	//namespace utility


#endif	//FUNCTIONAL_H
