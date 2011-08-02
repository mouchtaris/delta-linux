// ufunctors.h
// Some common functor classes for utili library.
// Moved from the central delta util here.
// ScriptFighter Project.
// A. Savidis, July 2002.
//

#ifndef	UFUNCTORS_H
#define	UFUNCTORS_H

#include <string.h>
#include <functional>
#include <utility>

#include "utypes.h"
#include "DDebug.h"
#include "usig.h"

//---------------------------------------------------------------
// The default asignment function relies upon the language
// built-in assignment operator. Assigns t2 to t1. For
// key handlers, one of the following has to be seelcted, or
//  another must be defined.
//
template <class T> struct uassigndefaultfunc : public std::binary_function<T, T, void> {
	void operator()(T& t1, T t2) const { t1 = t2; }
};

/////////////////////////////////////////////////////////////////
// The default comparison function relies upon the language
// built-in equality testing supoprt.
//
template <class T> struct ucmpdefaultfunc : public std::binary_function<T, T, bool>  {
	bool operator()(const T& t1, const T& t2) const { return t1==t2; }
};
		
/////////////////////////////////////////////////////////////////

struct ucmpstringfunc : public std::binary_function<const char*, const char*, bool> {
	bool operator()(const char* s1, const char* s2) const { 
		return !strcmp(s1, s2); 
	}
};

struct ustringless: public std::binary_function<const char*, const char*, bool> {
	bool operator()(const char* s1, const char* s2) const {
		return strcmp(s1,s2) < 0;
	}
};

template <class T>
struct uptrless : public std::binary_function<const T*, const T*, bool> {
	bool operator()(const T* p1, const T* p2) const {
		return ((util_ui32) p1) < ((util_ui32) p2);
	}
};

/////////////////////////////////////////////////////////////////
// The default destructor function uses the language delete operator.
//
template <class T> struct uptrdestructorfunctor : public std::unary_function<T, void> {
	void operator() (T item) const { DDELETE(item); }
};

template <class T> struct uarrdestructorfunctor : public std::unary_function<T, void> {
	void operator() (T item) const { DDELARR(item); }
};

template <class T> struct uptrnullifierfunctor : public std::unary_function<T, void> {
	void operator() (T& p) const { p = (T) 0; }
};

template <class T> 
void uptrnullifier (T*& p)	{ p = (T*) 0;}

template <class T> 
void uptrdestructor (T*& p) { DDELETE(p); p = (T*) 0; }

template <class T> 
void uarrdestructor (T*& p) { DDELARR(p); p = (T*) 0; }

template <typename T, typename Tdestructor> struct udestroyablewrapper {
	T ptr;
	typedef Tdestructor destructor_functor_type;
	const udestroyablewrapper operator=(const udestroyablewrapper& d)
		{ new (this) udestroyablewrapper(d); }
	udestroyablewrapper (T _ptr) : ptr(DPTR(_ptr)){}
	udestroyablewrapper (const udestroyablewrapper& d) : ptr(d.ptr){}
	~udestroyablewrapper(){ Tdestructor()(ptr); }
};

//---------------------------------------------------------------
// Accepts two types that define a pair, and a functor
// for the second type, adapting to a functor that accepts
// a pair reference. The functor can be either a class or simply
// a function pointer type.
//

template <class Ttuple, class Tfun> 
class ufunctor_first : public std::unary_function<Ttuple, void> {

	private:
	Tfun f;

	public:
	void operator()(const Ttuple& t) const {
		f(t.first);
	}

	ufunctor_first (const Tfun& _f) : f(_f) {}
};

template <class Ttuple, class Tfun> 
class ufunctor_second : public std::unary_function<Ttuple, void> {

	private:
	Tfun f;

	public:
	void operator()(const Ttuple& t) const {
		f(t.second);
	}

	ufunctor_second (const Tfun& _f) : f(_f) {}
};

template <class Ttuple, class Tfun> 
class ufunctor_third : public std::unary_function<Ttuple, void> {

	private:
	Tfun f;

	public:
	void operator()(const Ttuple& t) const {
		f(t.third);
	}

	ufunctor_third (const Tfun& _f) : f(_f) {}
};

/////////////////////////////////////////////////////////////////
// Destroyes the second item of a pair, using the pair second functor,
// applying the default destructor. In this case, type T2 is not supplied as
// a pointer.
//

template <typename Ttuple> 
class udestroy_first : public	ufunctor_first<
									Ttuple, 
									uptrdestructorfunctor<typename Ttuple::first_type>
								> {
	private:
	typedef uptrdestructorfunctor<typename Ttuple::first_type> _destructor;

	public:
	udestroy_first (void) : ufunctor_first<Ttuple, _destructor>(_destructor()) {}
};

template <typename Ttuple> 
class udestroy_second : public	ufunctor_second<
									Ttuple, 
									uptrdestructorfunctor<typename Ttuple::second_type>
								> {
	private:
	typedef uptrdestructorfunctor<typename Ttuple::second_type> _destructor;

	public:
	udestroy_second (void) : ufunctor_second<Ttuple, _destructor>(_destructor()) {}
};

template <typename Ttuple> 
class udestroy_third : public	ufunctor_third<
									Ttuple, 
									uptrdestructorfunctor<typename Ttuple::third_type>
								> {
	private:
	typedef uptrdestructorfunctor<typename Ttuple::third_type> _destructor;

	public:
	udestroy_third (void) : ufunctor_third<Ttuple, _destructor>(_destructor()) {}
};

/////////////////////////////////////////////////////////////////
// This is an equality functor for pointer content.
//
template <class T> struct uequal_ptrcontent : public std::binary_function<T*, T*, bool> {
	bool operator()(T* first, T* second) const 
		{ return *first == *second; }
};

/////////////////////////////////////////////////////////////////

template <class T1, class T2, class T3> struct utriple {

	T1	first;
	T2	second;
	T3	third;

	typedef T1 first_type;
	typedef T2 second_type;
	typedef T3 third_type;

	void operator=(const utriple& t) 
		{ new (this) utriple(t); }		// FIXME dangerous: reconstructing members without destructing them.
										// Probably a better idea to delegate assignment to members.

	utriple (const T1& _first, const T2& _second, const T3& _third) :
		first(_first),
		second(_second),
		third(_third) {}

	template<class _T1, class _T2, class _T3> utriple (const utriple<_T1, _T2, _T3> &t) :
		first(t.first),
		second(t.second),
		third(t.third) {}
};

/////////////////////////////////////////////////////////////////

template <class T1, class T2, class T3, class T4> struct uquadruple  {

	T1	first;
	T2	second;
	T3	third;
	T4	fourth;

	typedef T1 first_type;
	typedef T2 second_type;
	typedef T3 third_type;
	typedef T4 fourth_type;

	void operator=(const uquadruple& t)		
		{ new (this) uquadruple(t); }	// FIXME dangerous: reconstructing members without destructing them.
										// Probably a better idea to delegate assignment to members.

	uquadruple (const T1& _first, const T2& _second, const T3& _third, const T4 _fourth) :
		first(_first),
		second(_second),
		third(_third),
		fourth(_fourth) {}

	template<class _T1, class _T2, class _T3, class _T4> 
	uquadruple (const uquadruple<_T1, _T2, _T3, _T4> &t) :
		first(t.first),
		second(t.second),
		third(t.third),
		fourth(t.fourth){}
};

/////////////////////////////////////////////////////////////////

template <class T1, class T2, class T3, class T4, class T5> struct uquintuple  {

	T1	first;
	T2	second;
	T3	third;
	T4	fourth;
	T5	fifth;

	typedef T1 first_type;
	typedef T2 second_type;
	typedef T3 third_type;
	typedef T4 fourth_type;
	typedef T5 fifth_type;

	void operator=(const uquintuple& t) 
		{ new (this) uquintuple(t); }	// FIXME dangerous: reconstructing members without destructing them.
										// Probably a better idea to delegate assignment to members.

	uquintuple (const T1& _first, const T2& _second, const T3& _third, const T4 _fourth, const T5 _fifth) :
		first(_first),
		second(_second),
		third(_third),
		fourth(_fourth),
		fifth(_fifth){}

	template<class _T1, class _T2, class _T3, class _T4, class _T5> 
	uquintuple (const uquintuple<_T1, _T2, _T3, _T4, _T5> &t) :
		first(t.first),
		second(t.second),
		third(t.third),
		fourth(t.fourth),
		fifth(t.fifth){}
};

/////////////////////////////////////////////////////////////////
// Those functor predicates accept tuple types such as std::pair or utriple,
// and allow comparison with a selected member.
//

template <class Ttuple, class Fequal = std::equal_to< typename Ttuple::first_type > >
struct uequal_first : public std::binary_function<Ttuple, typename Ttuple::first_type, bool> {
	bool operator()(const Ttuple& t, typename Ttuple::first_type val) const {
		return Fequal()(t.first, val);
	}
};

template <class Ttuple, class Fequal = std::equal_to< typename Ttuple::second_type > >
struct uequal_second : public std::binary_function<Ttuple, typename Ttuple::second_type, bool> {
	bool operator()(const Ttuple& t, typename Ttuple::second_type val) const {
		return Fequal()(t.second, val);
	}
};

template <class Ttuple, class Fequal = std::equal_to< typename Ttuple::third_type > >
struct uequal_third : public std::binary_function<Ttuple, typename Ttuple::third_type, bool> {
	bool operator()(const Ttuple& t, typename Ttuple::third_type val) const {
		return Fequal()(t.third, val);
	}
};

template <class Ttuple, class Fequal = std::equal_to< typename Ttuple::fourth_type > >
struct uequal_fourth : public std::binary_function<Ttuple, typename Ttuple::fourth_type, bool> {
	bool operator()(const Ttuple& t, typename Ttuple::fourth_type val) const {
		return Fequal()(t.fourth, val);
	}
};

/////////////////////////////////////////////////////////////////
// FUNCTOR ADAPTERS FOR TUPLES.
// FIXME: exact functionality replicated by ufunctor_first, ufunctor_second, etc...
//
template <class T, class F> class utuple_firstfunctor : public std::unary_function<T, void> {
	private:
	F f;

	public:
	void operator()(const T& t) const { f(t.first); }
	utuple_firstfunctor (const F& _f) : f(_f){}
	utuple_firstfunctor (const utuple_firstfunctor<T,F>& p) : f(p.f){}
};

template <class T, class F>
const utuple_firstfunctor<T, F> utuple_firstfunctoriser (const F& f) {
	return utuple_firstfunctor<T, F>(f);
}

//***********************

template <class T, class F> class utuple_secondfunctor : public std::unary_function<T, void> {
	private:
	F f;

	public:
	void operator()(const T& t) const { f(t.second); }
	utuple_secondfunctor (const F& _f) : f(_f){}
	utuple_secondfunctor (const utuple_secondfunctor<T,F>& p) : f(p.f){}
};

template <class T, class F>
const utuple_secondfunctor<T, F> utuple_secondfunctoriser (const F& f) {
	return utuple_secondfunctor<T, F>(f);
}

//***********************

template <class T, class F> class utuple_thirdfunctor : public std::unary_function<T, void> {
	private:
	F f;

	public:
	void operator()(const T& t) const { f(t.third); }
	utuple_thirdfunctor (const F& _f) : f(_f){}
	utuple_thirdfunctor (const utuple_thirdfunctor<T,F>& p) : f(p.f){}
};

template <class T, class F>
const utuple_thirdfunctor<T, F> utuple_thirdfunctoriser (const F& f) {
	return utuple_thirdfunctor<T,F>(f);
}

/////////////////////////////////////////////////////////////////
// A condition class functor that can be supplied as an argument.
//
struct uconditionalfunctor {
	virtual bool operator()(void) const = 0;
};

struct utrueconditionalfunctor : public uconditionalfunctor {
	virtual bool operator()(void) const
		{ return true; }
};

struct ufalseconditionalfunctor : public uconditionalfunctor {
	virtual bool operator()(void) const
		{ return false; }
};

template <class T> void ucalldestructor (T* ptr) {
	ptr->~T();	// Never T::~T(), it ignores late binding.
}

template <class T> void ucalldestructor (T& val) {
	ucalldestructor(&val);
}

/////////////////////////////////////////////////////////////////
// Binder class and function for unary functions.
// The result is a function with no arguments.
//
template <class R, class T, class Tfunc> class unarybinder : public std::unary_function<void,R> {
	private:
	T a;
	Tfunc f;

	public:
	R operator()(void) const { return f(a); }
	unarybinder (const Tfunc& _f, const T& _a) : f(_f), a(_a){}
	unarybinder (const unarybinder<R,T,Tfunc>& b) : f(b.f), a(b.a){}
};

template <class R, class T, class Tfunc> 
const unarybinder<R,T,Tfunc>  unarybind (const Tfunc& f, const T& a) {
	return unarybinder<R,T,Tfunc>(f, a);
}

/////////////////////////////////////////////////////////////////
// A way of converting non-void unary functors to void (ignoring returned result).
// The adapter requires explicitly the argument type.
//
template <class T, class F> 
class uvoid_unaryfunctor : public std::unary_function<T, void> {
	private:
	F f;

	public:
	void operator()(const T& t) const { f(t); }
	uvoid_unaryfunctor (const F& _f) : f(_f){}
	uvoid_unaryfunctor (const uvoid_unaryfunctor<T,F>& p) : f(p.f){}
};

template <class T, class F> 
const uvoid_unaryfunctor<T, F> uvoid_unaryfunctor_adapter (const F& f) {
	return uvoid_unaryfunctor<T,F>(f);
}

/////////////////////////////////////////////////////////////////
// A delegate functor calling its argument as a function.
//
template <class F> struct udelagatefunctor: public std::unary_function<F, void> {
	void operator()(const F& f) const { f(); }
};

/////////////////////////////////////////////////////////////////
// Whenever increment / decrement step functions are needed.
//
template <class T> struct ustep {
	static void	inc (T* i) { ++(*i); }
	static void	dec (T* i) { --(*i); }
};

/////////////////////////////////////////////////////////////////
// Turning anything (non-constant) to a reference.
//
template <class T> T& utoref (T* p) { return *p; }
template <class T> const T& utoref (const T* p) { return *p; }
template <class T> T& utoref (T& p) { return p; }
template <class T> const T& utoref (const T& p) { return p; }

/////////////////////////////////////////////////////////////////

inline bool ubinary_or_operator (bool a, bool b) 
	{ return a || b; }

inline bool ubinary_and_operator (bool a, bool b) 
	{ return a && b; }

template <class F1, class F2> 
struct ubinary_operator_functor : public std::unary_function<
										typename F1::argument_type, 
										typename F1::result_type
									> {
	protected:
	const F1 _fn1; 
	const F2 _fn2;
	bool (*_Op)(bool, bool);

	public:
	typedef typename F1::argument_type	argument_type;
	typedef typename F1::result_type	result_type;

	result_type operator()(argument_type a) 
		{ return (*_Op)(_fn1(a), _fn2(a)); }

	ubinary_operator_functor (const F1& fn1, const F2& fn2, bool (*Op)(bool, bool)) : 
		_fn1(fn1), _fn2(fn2), _Op(Op){}
};

template <class F1, class F2> ubinary_operator_functor<F1, F2> 
ubinary_or (const F1& f1, const F2& f2) 
	{ return ubinary_operator_functor<F1, F2>(f1, f2, &ubinary_or_operator); }

template <class F1, class F2> ubinary_operator_functor<F1, F2> 
ubinary_and (const F1& f1, const F2& f2) 
	{ return ubinary_operator_functor<F1, F2>(f1, f2, &ubinary_and_operator); }

//---------------------------------------------------------------


#endif	// Do not add stuff beyond this point.
