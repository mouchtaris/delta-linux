/**
 *	Functional.h
 *
 *	-- Utility Library --
 *	Header file with several objects useful
 *	for functional composition.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2007.
 */
#ifndef UTIL_FUNCTIONAL_H_
#define UTIL_FUNCTIONAL_H_

namespace util
{
	//----------------------------
	//-- function reducer_t

	template <class T, class Op>
	struct reducer_t
	{
		typedef T result_type;
		typedef Op operation;

		reducer_t(operation op)
			: result(result_type()), op(op) {}
		reducer_t(const result_type& t, operation op)
			: result(t), op(op) {}

		reducer_t& operator +=(const result_type& t) {
			result = op(result, t);
			return *this;
		}

		const result_type& operator *(void) const	{ return result; }
		result_type& operator *(void)				{ return result; }

	private:
		result_type result;
		operation op;
	};
}

#endif	//UTIL_FUNCTIONAL_H_
