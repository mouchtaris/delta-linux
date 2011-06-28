/**
 *	Algorithms.h
 *
 *	-- Utility Library --
 *	Header file with some new algorithms and
 *	serveral spicy flavors of std algorithms.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2007.
 */
#ifndef UTIL_ALGORITHMS_H_
#define UTIL_ALGORITHMS_H_

#include "Functional.h"

#include <list>
#include <algorithm>

namespace util
{
	//----------------------------
	//-- function for_each_safe

	template <class InIt, class Fn>
	Fn for_each_safe(InIt first, InIt last, Fn f) {
		std::list<typename InIt::value_type> clist(first, last);
		return std::for_each(clist.begin(), clist.end(), f);
	}


	//----------------------------
	//-- function for_each_if

	template <class InIt, class Fn, class Pred>
	Fn for_each_if(InIt first, InIt last, Fn f, Pred pred){
		for (; first != last; ++first)
			if (pred(*first)) f(*first);
		return f;
	}


	//----------------------------
	//-- function for_all

	template <class Seq, class Fn>
	Fn for_all (Seq& seq, Fn f)
		{ return std::for_each(seq.begin(), seq.end(), f); }


	//----------------------------
	//-- function for_all_safe

	template <class Seq, class Fn>
	Fn for_all_safe (Seq& seq, Fn f)
		{ return for_each_safe(seq.begin(), seq.end(), f); }


	//----------------------------
	//-- function for_all_if

	template <class Seq, class Fn, class Pred>
	Fn for_all_if (Seq& seq, Fn f, Pred pred)
		{ return for_each_if(seq.begin(), seq.end(), f, pred); }


	//----------------------------
	//-- function copy_if

	template <class InIt, class OutIt, class Pred>
	OutIt copy_if(InIt first, InIt last, OutIt dest, Pred pred) {
		for (; first != last; ++first)
			if (pred(*first)) *(dest++) = *first;
		return (dest);
	}


	//----------------------------
	//-- function transform_if

	template <class InIt, class OutIt, class Fn1, class Pred>
	OutIt transform_if(InIt first, InIt last, OutIt dest, Fn1 f, Pred pred) {
		for (; first != last; ++first)
			if (pred(*first)) *(dest++) = f(*first);
		return (dest);
	}


	//----------------------------
	//-- function reduce

	template <class InIt, class Op, class Fn>
	typename util::reducer_t<typename Op::result_type, Op>::result_type
	reduce(InIt first, InIt last, Op op, Fn f = util::identity<typename Op::result_type>()) {
		if (first == last) return typename Op::result_type();
		util::reducer_t<typename Op::result_type, Op> c(f(*first), op);
		for (++first; first != last; ++first)
			c += f(*first);
		return *c;
	}


	//----------------------------
	//-- function reduce_if

	template <class InIt, class Op, class Fn, class Pred>
	typename util::reducer_t<typename Op::result_type, Op>::result_type
	reduce_if(InIt first, InIt last, Op op, Pred pred, Fn f = util::identity<typename Op::result_type>()) {
		if (first == last) return typename Op::result_type();
		util::reducer_t<typename Op::result_type, Op> c(f(*first), op);
		for (++first; first != last; ++first)
			if (pred(*first)) c += f(*first);
		return *c;
	}


	//----------------------------
	//-- function exists

	template <class InIt, class ValueType>
	bool exists(InIt first, InIt last, ValueType value) {
		return (std::find(first, last, value) == last) ? false : true;
	}


	//----------------------------
	//-- function exists_if

	template <class InIt, class Pred>
	bool exists_if(InIt first, InIt last, Pred pred) {
		return (std::find_if(first, last, pred) == last) ? false : true;
	}
}

#endif	//UTIL_ALGORITHMS_H_
