/**
 *	Predicates.h
 *
 *	-- Utility Library --
 *	Header file with some commonly used predicates.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *  Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	May 2007.
 */
#ifndef UTIL_PREDICATES_H_
#define UTIL_PREDICATES_H_

#include <functional>

namespace util
{
	//----------------------------
	//-- struct contains

	template <class InIt>
	struct contains :
		public std::unary_function<PARAM(typename InIt::value_type), bool>
	{
		contains(InIt first, InIt last)
			: first(first), last(last) {}

		result_type operator ()(argument_type value) {
			return std::find(first, last, value) != last;
		}

	private:
		InIt first, last;
	};


	//----------------------------
	//-- struct isValidPtr

	template <class Ptr>
	struct isValidPtr :
		public std::unary_function<Ptr, bool>
	{
		result_type operator() (argument_type value) {
			return (value != (Ptr)0);
		}
	};


	//----------------------------
	//-- struct const_pred

	template <class TArg, bool TresultVal>
	struct const_pred :
		public std::unary_function<TArg, bool>
	{
		result_type operator() (argument_type value) {
			return TresultVal;
		}
	};
}

#endif	//UTIL_PREDICATES_H_
