/**
 *	Adaptors.h
 *
 *	-- Utility Library --
 *	Header file with adaptor objects.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	July 2006.
 */
#ifndef UTIL_ADAPTORS_H_
#define UTIL_ADAPTORS_H_

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <functional>

namespace util
{
	//----------------------------
	//-- function project1st

	template <class Pair, class Fn, class RetT>
	class projector1st :
		public std::unary_function<Pair, RetT>
	{
	public:
		projector1st(Fn& f)
			: f(f) {}

		result_type operator()(const argument_type& entry) const {
			return f(entry.first);
		}

		result_type operator()(argument_type& entry) {
			return f(entry.first);
		}

	private:
		Fn f;
	};

	template <class Pair, class Fn>
	class projector1st<Pair, Fn, void> :
		public std::unary_function<Pair, void>
	{
	public:
		projector1st(Fn& f)
			: f(f) {}

		result_type operator()(const argument_type& entry) const {
			f(entry.first);
		}

		result_type operator()(argument_type& entry) {
			f(entry.first);
		}

	private:
		Fn f;
	};

	template <class Pair, class Fn>
	inline projector1st<Pair, Fn, typename Fn::result_type> project1st(Fn& f) {
		return projector1st<Pair, Fn, typename Fn::result_type>(f);
	}


	//----------------------------
	//-- function projector2nd

	template <class Pair, class Fn, class RetT>
	class projector2nd :
		public std::unary_function<Pair, RetT>
	{
	public:
		projector2nd(Fn f)
			: f(f) {}

		result_type operator()(const argument_type& entry) const {
			return f(entry.second);
		}

		result_type operator()(argument_type& entry) {
			return f(entry.second);
		}

	private:
		Fn f;
	};

	template <class Pair, class Fn>
	class projector2nd<Pair, Fn, void> :
		public std::unary_function<Pair, void>
	{
	public:
		projector2nd(Fn const& f)
			: f(f) {}

		result_type operator()(const argument_type& entry) const {
			f(entry.second);
		}

		result_type operator()(argument_type& entry) {
			f(entry.second);
		}

	private:
		Fn f;
	};

	template <class Pair, class Fn>
	inline projector2nd<Pair, Fn, typename Fn::result_type> project2nd(Fn const& f) {
		return projector2nd<Pair, Fn, typename Fn::result_type>(f);
	}


	//----------------------------
	//-- function dereference

	template <class TPtr, class TFn, class TRet>
	class dereferencer : public std::unary_function<TPtr, TRet> {
	public:
		typedef TPtr pointer_type;

		dereferencer (TFn func)
			: m_func(func) {}

		result_type operator() (pointer_type ptr) const {
			return m_func(*ptr);
		}

	private:
		TFn m_func;
	};

	template <class TPtr, class TFn>
	class dereferencer<TPtr, TFn, void> : public std::unary_function<TPtr, void> {
	public:
		typedef TPtr pointer_type;

		dereferencer (TFn func)
			: m_func(func) {}

		void operator() (pointer_type ptr) const {
			m_func(*ptr);
		}

	private:
		TFn m_func;
	};

	template <class TPtr, class TFn>
	inline dereferencer<TPtr, TFn, typename TFn::result_type>
	dereference (TFn f) {
		return dereferencer<TPtr, TFn, typename TFn::result_type>(f);
	}


	//----------------------------
	//-- function counter_adaptor

	template <class Arg, class Fn, class RetType>
	struct counter_adaptor_t :
		public std::unary_function<Arg&, typename Fn::result_type>
	{
		counter_adaptor_t(Fn& f, size_t count)
			: f(f), count(count) {}

		result_type operator ()(const argument_type useless) const {
			return f(count++);
		}

		result_type operator ()(argument_type useless) {
			return f(count++);
		}

	private:
		Fn f;
		size_t count;
	};

	template <class Arg, class Fn>
	struct counter_adaptor_t<Arg, Fn, void> :
		public std::unary_function<Arg&, typename Fn::result_type>
	{
		counter_adaptor_t(Fn& f, size_t count)
			: f(f), count(count) {}

		result_type operator ()(const argument_type useless) const {
			f(count++);
		}

		result_type operator ()(argument_type useless) {
			f(count++);
		}

	private:
		Fn f;
		size_t count;
	};

	template <class Arg, class Fn>
	inline counter_adaptor_t<Arg, Fn, typename Fn::result_type> counter_adaptor(Fn& f, size_t count=0) {
		return counter_adaptor_t<Arg, Fn, typename Fn::result_type>(f, count);
	}


	//----------------------------
	//-- struct inner_loop_t

	template <class OuterT, class RetT, class InIt, class Fn>
	struct inner_loop_t :
		public std::unary_function<OuterT, RetT>
	{
		typedef boost::function<typename Fn::result_type (typename Fn::second_argument_type)> InFn;
		typedef boost::function<RetT (InIt, InIt, InFn)> algorithm;

		inner_loop_t(algorithm algo, InIt first, InIt last, Fn f)
			: algo(algo), first(first), last(last), f(f) {}

		result_type operator ()(const argument_type& arg) {
			return algo(
				first, last,
				boost::bind(f, arg, _1)
			);
		}

		result_type operator ()(argument_type& arg) {
			return algo(
				first, last,
				boost::bind(f, arg, _1)
			);
		}

	private:
		algorithm algo;
		InIt first;
		InIt last;
		Fn f;
	};


	//----------------------------
	//-- function inner_for_each

	template <class OuterT, class InIt, class Fn>
	inline inner_loop_t<OuterT, boost::function<typename Fn::result_type (typename Fn::second_argument_type)>, InIt, Fn>
	inner_for_each(InIt first, InIt last, Fn f)	{
		return 
			inner_loop_t<OuterT, boost::function<typename Fn::result_type (typename Fn::second_argument_type)>, InIt, Fn>(
				std::for_each<InIt, boost::function<typename Fn::result_type (typename Fn::second_argument_type)> >,
				first, last, f
			);
	}


	//----------------------------
	//-- function inner_reduce

	template <class OuterT, class InIt, class Op, class Fn>
	inline inner_loop_t<OuterT, typename Op::result_type, InIt, Fn>
	inner_reduce(InIt first, InIt last, Op op, Fn f = util::identity<typename Op::result_type>()) {
		return
			inner_loop_t<OuterT, typename Op::result_type, InIt, Fn>(
				boost::bind(
					util::reduce<InIt, Op, boost::function<typename Fn::result_type (typename Fn::second_argument_type)> >,
					_1, _2, op, _3
				),
				first, last, f
			);
	}

	//----------------------------
	//-- function ptr_equal_to

	template <class TPtr, class TValue>
	struct ptr_equal_to_t {
		ptr_equal_to_t (PARAM(TValue) value) : m_value(value) {}
		bool operator() (TPtr ptr) const { return *ptr == m_value; }
	private:
		VALUE(TValue) m_value;
	};

	template <class TPtr, class TValue>
	inline ptr_equal_to_t<TPtr, TValue>
	ptr_equal_to (TValue value) { return ptr_equal_to_t<TPtr, TValue>(value); }

}

#endif //UTIL_ADAPTORS_H_
