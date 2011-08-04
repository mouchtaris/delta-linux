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
template <typename T> struct uassigndefaultfunc : public std::binary_function<T, T, void> {
	void operator()(T& t1, T t2) const { t1 = t2; }
};

/////////////////////////////////////////////////////////////////
// The default comparison function relies upon the language
// built-in equality testing supoprt.
//
template <typename T> struct ucmpdefaultfunc : public std::binary_function<T, T, bool>  {
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

template <typename T>
struct uptrless : public std::binary_function<const T*, const T*, bool> {
	bool operator()(const T* p1, const T* p2) const {
		return ((util_ui32) p1) < ((util_ui32) p2);
	}
};

/////////////////////////////////////////////////////////////////
// The default destructor function uses the language delete operator.
//
template <typename T> struct uptrdestructorfunctor : public std::unary_function<T, void> {
	void operator() (T item) const { DDELETE(item); }
};

template <typename T> struct uarrdestructorfunctor : public std::unary_function<T, void> {
	void operator() (T item) const { DDELARR(item); }
};

template <typename T> struct uptrnullifierfunctor : public std::unary_function<T, void> {
	void operator() (T& p) const { p = (T) 0; }
};

template <typename T> 
void uptrnullifier (T*& p)	{ p = (T*) 0;}

template <typename T> 
void uptrdestructor (T*& p) { DDELETE(p); p = (T*) 0; }		// FIXME: duplicated functionality of udelete()

template <typename T> 
void uarrdestructor (T*& p) { DDELARR(p); p = (T*) 0; }

template <typename T, typename Tdestructor> struct udestroyablewrapper {
	T ptr;
	typedef Tdestructor destructor_functor_type;
	const udestroyablewrapper& operator=(const udestroyablewrapper& d) {
		if (this != &d && ptr != d.ptr) {
			udestructor_invocation(this);
			new (this) udestroyablewrapper(d);
		}
		return *this;
	}
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

template <typename Ttuple, typename Tfun> 
class ufunctor_first : public std::unary_function<Ttuple, void> {

	private:
	Tfun f;

	public:
	typename Tfun::result_type operator()(typename uref_of<Ttuple>::t t) const {
		return f(t.first);
	}
	typename Tfun::result_type operator()(typename uconstref_of<Ttuple>::t t) const {
		return f(t.first);
	}

	ufunctor_first (const Tfun& _f) : f(_f) {}
};

template <typename Ttuple, typename Tfun> 
class ufunctor_second : public std::unary_function<Ttuple, void> {

	private:
	Tfun f;

	public:
	typename Tfun::result_type operator()(typename uref_of<Ttuple>::t t) const {
		return f(t.second);
	}
	typename Tfun::result_type operator()(typename uconstref_of<Ttuple>::t t) const {
		return f(t.second);
	}

	ufunctor_second (const Tfun& _f) : f(_f) {}
};

template <typename Ttuple, typename Tfun> 
class ufunctor_third : public std::unary_function<Ttuple, void> {

	private:
	Tfun f;

	public:
	typename Tfun::result_type operator()(typename uref_of<Ttuple>::t t) const {
		return f(t.third);
	}
	typename Tfun::result_type operator()(typename uconstref_of<Ttuple>::t t) const {
		return f(t.third);
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
template <typename T> struct uequal_ptrcontent : public std::binary_function<T*, T*, bool> {
	bool operator()(T* first, T* second) const 
		{ return *first == *second; }
};

/////////////////////////////////////////////////////////////////

template <typename T1, typename T2, typename T3> struct utriple {

	T1	first;
	T2	second;
	T3	third;

	typedef T1 first_type;
	typedef T2 second_type;
	typedef T3 third_type;

	UOVERLOADED_VOID_ASSIGN_VIA_COPY_CONSTRUCTOR(utriple)

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

template <typename T1, typename T2, typename T3, typename T4> struct uquadruple  {

	T1	first;
	T2	second;
	T3	third;
	T4	fourth;

	typedef T1 first_type;
	typedef T2 second_type;
	typedef T3 third_type;
	typedef T4 fourth_type;

	UOVERLOADED_VOID_ASSIGN_VIA_COPY_CONSTRUCTOR(uquadruple)

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

template <typename T1, typename T2, typename T3, typename T4, typename T5> struct uquintuple  {

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

	UOVERLOADED_VOID_ASSIGN_VIA_COPY_CONSTRUCTOR(uquintuple)

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

template <typename Ttuple, typename Fequal = std::equal_to< typename Ttuple::first_type > >
struct uequal_first : public std::binary_function<Ttuple, typename Ttuple::first_type, bool> {
	bool operator()(const Ttuple& t, typename Ttuple::first_type val) const {
		return Fequal()(t.first, val);
	}
};

template <typename Ttuple, typename Fequal = std::equal_to< typename Ttuple::second_type > >
struct uequal_second : public std::binary_function<Ttuple, typename Ttuple::second_type, bool> {
	bool operator()(const Ttuple& t, typename Ttuple::second_type val) const {
		return Fequal()(t.second, val);
	}
};

template <typename Ttuple, typename Fequal = std::equal_to< typename Ttuple::third_type > >
struct uequal_third : public std::binary_function<Ttuple, typename Ttuple::third_type, bool> {
	bool operator()(const Ttuple& t, typename Ttuple::third_type val) const {
		return Fequal()(t.third, val);
	}
};

template <typename Ttuple, typename Fequal = std::equal_to< typename Ttuple::fourth_type > >
struct uequal_fourth : public std::binary_function<Ttuple, typename Ttuple::fourth_type, bool> {
	bool operator()(const Ttuple& t, typename Ttuple::fourth_type val) const {
		return Fequal()(t.fourth, val);
	}
};

/////////////////////////////////////////////////////////////////
// FUNCTOR ADAPTERS FOR TUPLES.
// FIXME: exact functionality replicated by ufunctor_first, ufunctor_second, etc...
//
template <typename T, typename F> class utuple_firstfunctor : public std::unary_function<T, void> {
	private:
	F f;

	public:
	void operator()(typename uref_of<T>::t t) const { f(t.first); }
	void operator()(typename uconstref_of<T>::t t) const { f(t.first); }
	utuple_firstfunctor (const F& _f) : f(_f){}
	utuple_firstfunctor (const utuple_firstfunctor<T,F>& p) : f(p.f){}
};

template <typename T, typename F>
const utuple_firstfunctor<T, F> utuple_firstfunctoriser (const F& f) {
	return utuple_firstfunctor<T, F>(f);
}

//***********************

template <typename T, typename F> class utuple_secondfunctor : public std::unary_function<T, void> {
	private:
	F f;

	public:
	void operator()(typename uref_of<T>::t t) const { f(t.second); }
	void operator()(typename uconstref_of<T>::t t) const { f(t.second); }
	utuple_secondfunctor (const F& _f) : f(_f){}
	utuple_secondfunctor (const utuple_secondfunctor<T,F>& p) : f(p.f){}
};

template <typename T, typename F>
const utuple_secondfunctor<T, F> utuple_secondfunctoriser (const F& f) {
	return utuple_secondfunctor<T, F>(f);
}

//***********************

template <typename T, typename F> class utuple_thirdfunctor : public std::unary_function<T, void> {
	private:
	F f;

	public:
	void operator()(typename uref_of<T>::t t) const { f(t.third); }
	void operator()(typename uconstref_of<T>::t t) const { f(t.third); }
	utuple_thirdfunctor (const F& _f) : f(_f){}
	utuple_thirdfunctor (const utuple_thirdfunctor<T,F>& p) : f(p.f){}
};

template <typename T, typename F>
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
template <typename R, typename T, typename Tfunc> class unarybinder : public std::unary_function<void,R> {
	private:
	T a;
	Tfunc f;

	public:
	R operator()(void) const { return f(a); }
	unarybinder (const Tfunc& _f, const T& _a) : f(_f), a(_a){}
	unarybinder (const unarybinder<R,T,Tfunc>& b) : f(b.f), a(b.a){}
};

template <typename R, typename T, typename Tfunc> 
const unarybinder<R,T,Tfunc>  unarybind (const Tfunc& f, const T& a) {
	return unarybinder<R,T,Tfunc>(f, a);
}

/////////////////////////////////////////////////////////////////
// A way of converting non-void unary functors to void (ignoring returned result).
// The adapter requires explicitly the argument type.
//
template <typename T, typename F> 
class uvoid_unaryfunctor : public std::unary_function<T, void> {
	private:
	F f;

	public:
	void operator()(typename uref_of<T>::t t) const { f(t); }
	void operator()(typename uconstref_of<T>::t t) const { f(t); }
	uvoid_unaryfunctor (const F& _f) : f(_f){}
	uvoid_unaryfunctor (const uvoid_unaryfunctor<T,F>& p) : f(p.f){}
};

template <typename T, typename F> 
const uvoid_unaryfunctor<T, F> uvoid_unaryfunctor_adapter (const F& f) {
	return uvoid_unaryfunctor<T,F>(f);
}

/////////////////////////////////////////////////////////////////
// A delegate functor calling its argument as a function.
//
template <typename F> struct udelagatefunctor: public std::unary_function<F, void> {
	void operator()(const F& f) const { f(); }
};

/////////////////////////////////////////////////////////////////
// Whenever increment / decrement step functions are needed.
//
template <typename T> struct ustep {
	static void	inc (T* i) { ++(*i); }
	static void	dec (T* i) { --(*i); }
};

/////////////////////////////////////////////////////////////////
// Turning anything (non-constant) to a reference.
//
template <typename T> T& utoref (T* p) { return *p; }
template <typename T> const T& utoref (const T* p) { return *p; }
template <typename T> T& utoref (T& p) { return p; }
template <typename T> const T& utoref (const T& p) { return p; }

/////////////////////////////////////////////////////////////////

inline bool ubinary_or_operator (bool a, bool b) 
	{ return a || b; }

inline bool ubinary_and_operator (bool a, bool b) 
	{ return a && b; }

template <typename F1, typename F2> 
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

template <typename F1, typename F2> ubinary_operator_functor<F1, F2> 
ubinary_or (const F1& f1, const F2& f2) 
	{ return ubinary_operator_functor<F1, F2>(f1, f2, &ubinary_or_operator); }

template <typename F1, typename F2> ubinary_operator_functor<F1, F2> 
ubinary_and (const F1& f1, const F2& f2) 
	{ return ubinary_operator_functor<F1, F2>(f1, f2, &ubinary_and_operator); }

/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Additional by Nikos Mouchtaris <muhtaris@ics.forth.gr>
// Added on the 1st of August, 2011.
/////////////////////////////////////////////////////////////////
// The following templates are defined:
//
// --- Functor supertypes (according to std::unary_function,
//     std::binary_function, ...)
// + uvoid_function		<Result>
// + uunary_function	<Result, Argument1>
// + ubinary_function	<Result, Argument1, Argument2>
// + uternary_function	<Result, Argument1, Argument2, Argument3>
//
//
// --- Specialisations of usig<> for functors' supertypes
//
//
// --- Pointers to functions
//   Each upointer_to_{*}_function type IS-A u{*}_function type.
// + upointer_to_function (supertype)
// + upointer_to_void_function		<Result>
// + upointer_to_unary_function		<Result, Argument1>
// + upointer_to_binary_function	<Result, Argument1, Argument2>
// + upointer_to_ternary_function	<Result, Argument1, Argument2, Argument3>
//
// + uptr_fun(&funcaddr) -> upointer_to_{*}_function obj
//
//
// --- Pointers to specific functions
//   Each uspecific_pointer_to_{*}_function IS-A u{*}_function type.
// + uspecific_pointer_to_void_function		<Result, &Func>
// + uspecific_pointer_to_unary_function    <Result, Argument1, &Func>
// + uspecific_pointer_to_binary_function   <Result, Argument1, Argument2, &Func>
// + uspecific_pointer_to_ternary_function	<Result, Argument1, Argument2, Argument3, &Func>
//
//
// --- Member function types
//   Types for member functions of arity up to 1 already
//   exist in namespace std::*. The alternatives provided
//   here additionally offer (through their contract)
//   type definitions for the Container-Type (of the
//   member function), as well as uniformity of argument
//   handling.
//   Specifically, arguments are handled as much as possible
//   as references, as they are passed to the original function.
//
//   Extras:
//   struct *mem_fun* {
//		typedef Container;
//		typedef Func;
//		...
//   };
// + uconst_mem_fun		<Result, Container>
// + umem_fun			<Result, Container>
// + uconst_mem_fun1	<Result, Container, Argument1>
// + umem_fun1			<Result, Container, Argument1>
// + uconst_mem_fun2	<Result, Container, Argument1, Argument2>
// + umem_fun2			<Result, Container, Argument1, Argument2>
//
// + umemberfunctionpointer(funcptr) -> [const_]mem_fun{*} obj
//
//
// --- Pointers to specific member functions
//   Specific pointers to member functions provided for
//   compile time optimisations.
//   There is no way to get these types or objects
//   "easily" from an adapter function. It would require
//   template templating (which may exist, TODO check out).
// + uspecific_const_mem_fun		<Result, Container, &Func>
// + uspecific_mem_fun              <Result, Container, &Func>
// + uspecific_const_mem_fun1       <Result, Container, Argument1, &Func>
// + uspecific_mem_fun1             <Result, Container, Argument1, &Func>
// + uspecific_const_mem_fun2       <Result, Container, Argument1, Argument2, &Func>
// + uspecific_mem_fun2             <Result, Container, Argument1, Argument2, &Func>
//
//
// --- Binders
// Binders provided for functions with arity of two (2) or
// three (3).
// All binders are constructed by receiving the value to be bound
// and optionally the operation to which the value is bound.
// If the operation is not provided, it is default constructed.
//
// No binder for functions of arity 0 or 1 is provided. Use
// unarybinder{} and unarybind() for those cases.
//
// + ubinder2_1st
// + ubinder2_2nd
// + ubinder3_1st
// + ubinder3_2nd
// + ubinder3_3rd
// + ubind1st(val, op)
// + ubind2nd(val, op)
// + ubind3rd(val, op)
//
//
// --- Traits for ubind*().
// Custom functors should specialise the ubind_traits<Operation>
// template if need to be bindable.
// For functors that inherit from the u-functor-supertypes
// (ubinary_function, etc), there are convencience default
// traits:
//		ubind2_traits<Operation>	for binary functors
//		ubind3_traits<Operation>	for ternary functors.
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
// Functor supertypes (according to std::unary_function, std::binary_function, ...)
// + uvoid_function
// + uunary_function
// + ubinary_function
// + uternary_function
/////////////////////////////////////////////////////////////////

template <typename _Result>
struct uvoid_function {
	typedef _Result			result_type;
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _Argument1>
struct uunary_function {
	typedef _Result			result_type;
	typedef _Argument1		argument_type;
	typedef _Argument1		first_argument_type;
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _Argument1, typename _Argument2>
struct ubinary_function {
	typedef _Result			result_type;
	typedef _Argument1		first_argument_type;
	typedef _Argument2		second_argument_type;
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _Argument1, typename _Argument2, typename _Argument3>
struct uternary_function {
	typedef _Result			result_type;
	typedef _Argument1		first_argument_type;
	typedef _Argument2		second_argument_type;
	typedef _Argument3		third_argument_type;
};

/////////////////////////////////////////////////////////////////
// Specialisations of usig<> for functors' supertypes
/////////////////////////////////////////////////////////////////

template <typename _Result>
struct usig<uvoid_function<_Result> > {
	typedef uvoid_function<_Result> Operation;

	typedef typename Operation::result_type		result_type;
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _Argument1>
struct usig<uunary_function<_Result, _Argument1> > {
	typedef uunary_function<_Result, _Argument1> Operation;

	typedef typename Operation::result_type				result_type;
	typedef typename Operation::first_argument_type		first_argument_type;
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _Argument1, typename _Argument2>
struct usig<ubinary_function<_Result, _Argument1, _Argument2> > {
	typedef ubinary_function<_Result, _Argument1, _Argument2> Operation;

	typedef typename Operation::result_type				result_type;
	typedef typename Operation::first_argument_type		first_argument_type;
	typedef typename Operation::second_argument_type	second_argument_type;
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _Argument1, typename _Argument2, typename _Argument3>
struct usig<uternary_function<_Result, _Argument1, _Argument2, _Argument3> > {
	typedef uternary_function<_Result, _Argument1, _Argument2,_Argument3> Operation;

	typedef typename Operation::result_type				result_type;
	typedef typename Operation::first_argument_type		first_argument_type;
	typedef typename Operation::second_argument_type	second_argument_type;
	typedef typename Operation::third_argument_type		third_argument_type;
};

/////////////////////////////////////////////////////////////////
// Pointers to functions
// + upointer_to_function (supertype)
// + upointer_to_void_function
// + upointer_to_unary_function
// + upointer_to_binary_function
// + upointer_to_ternary_function
/////////////////////////////////////////////////////////////////

template <typename T> struct urefto					{ typedef const T&	t; };
template <typename T> struct urefto<T&>				{ typedef T&		t; };
template <typename T> struct urefto<const T&>		{ typedef const T&	t; };

template <typename T> struct uref_or_scal			{ typedef T		t; };
template <typename T> struct uref_or_scal<T&>		{ typedef T&	t; };
template <typename T> struct uref_or_scal<const T&>	{ typedef typename uref_or_scal<T>::t	t; };


template <typename _Func>
struct upointer_to_function {
	typedef _Func		function_type;

	function_type 		get_function (void) const
							{ return f; }
protected:
	explicit			upointer_to_function (function_type const& _f): f(_f) { }
	//	no virtual destructor!
	function_type		f;
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _Func = _Result (*) (void)>
struct upointer_to_void_function: public upointer_to_function<_Func>, public uvoid_function<_Result> {
	typedef upointer_to_function<_Func>		Base0;
	typedef uvoid_function<_Result>			Base1;
	typedef typename Base0::function_type	Func;
	typedef typename Base1::result_type		Result;

	Result		operator () (void) const
					{ return (*f)(); }
	explicit	upointer_to_void_function (Func const _f): Base0(_f), Base1() {}
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _Arg1, typename _Func = _Result (*) (_Arg1)>
struct upointer_to_unary_function: public upointer_to_function<_Func>, public uunary_function<_Result, _Arg1> {
	typedef upointer_to_function<_Func>					Base0;
	typedef uunary_function<_Result, _Arg1>				Base1;
	typedef typename Base0::function_type				Func;
	typedef typename Base1::result_type					Result;
	typedef typename Base1::first_argument_type			Argument1;
	typedef typename urefto<Argument1>::t				Ref1;

	Result		operator () (Ref1 a) const
					{ return (*f)(a); }
	explicit	upointer_to_unary_function (Func const _f): Base0(_f), Base1() {}
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _Arg1, typename _Arg2, typename _Func = _Result (*) (_Arg1, _Arg2)>
struct upointer_to_binary_function: public upointer_to_function<_Func>, public ubinary_function<_Result, _Arg1, _Arg2> {
	typedef upointer_to_function<_Func>					Base0;
	typedef ubinary_function<_Result, _Arg1, _Arg2>		Base1;
	typedef typename Base0::function_type				Func;
	typedef typename Base1::result_type					Result;
	typedef typename Base1::first_argument_type			Argument1;
	typedef typename Base1::second_argument_type		Argument2;
	typedef typename urefto<Argument1>::t				Ref1;
	typedef typename urefto<Argument2>::t				Ref2;

	Result		operator () (Ref1 a, Ref2 b) const
					{ return (*f)(a, b); }
	explicit	upointer_to_binary_function (Func const _f): Base0(_f), Base1() {}
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _Arg1, typename _Arg2, typename _Arg3, typename _Func = _Result (*) (_Arg1, _Arg2, _Arg3)>
struct upointer_to_ternary_function: public upointer_to_function<_Func>, public uternary_function<_Result, _Arg1, _Arg2, _Arg3> {
	typedef upointer_to_function<_Func>						Base0;
	typedef uternary_function<_Result, _Arg1, _Arg2, _Arg3>	Base1;
	typedef typename Base0::function_type					Func;
	typedef typename Base1::result_type						Result;
	typedef typename Base1::first_argument_type				Argument1;
	typedef typename Base1::second_argument_type			Argument2;
	typedef typename Base1::third_argument_type				Argument3;
	typedef typename urefto<Argument1>::t					Ref1;
	typedef typename urefto<Argument2>::t					Ref2;
	typedef typename urefto<Argument3>::t					Ref3;

	Result		operator () (Ref1 a, Ref2 b, Ref3 c) const
					{ return (*f)(a, b, c); }
	explicit	upointer_to_ternary_function (Func const _f): Base0(_f), Base1() {}
};

/////////////////////////////////////////////////////////////////

template <typename _Result>
upointer_to_void_function<_Result> uptr_fun (_Result (*const func) (void))
	{ return upointer_to_void_function<_Result>(func); }

template <typename _Result, typename _Arg1>
upointer_to_unary_function<_Result, _Arg1> uptr_fun (_Result (*const func) (_Arg1))
	{ return upointer_to_unary_function<_Result, _Arg1>(func); }

template <typename _Result, typename _Arg1, typename _Arg2>
upointer_to_binary_function<_Result, _Arg1, _Arg2> uptr_fun (_Result (*const func) (_Arg1, _Arg2))
	{ return upointer_to_binary_function<_Result, _Arg1, _Arg2>(func); }

template <typename _Result, typename _Arg1, typename _Arg2, typename _Arg3>
upointer_to_ternary_function<_Result, _Arg1, _Arg2, _Arg3> uptr_fun (_Result (*const func) (_Arg1, _Arg2, _Arg3))
	{ return upointer_to_ternary_function<_Result, _Arg1, _Arg2, _Arg3>(func); }

/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Pointers to specific functions
// + uspecific_pointer_to_void_function
// + uspecific_pointer_to_unary_function
// + uspecific_pointer_to_binary_function
// + uspecific_pointer_to_ternary_function
/////////////////////////////////////////////////////////////////

template <typename _Result, _Result (*const _f) (void)>
struct uspecific_pointer_to_void_function: public uvoid_function<_Result> {
	typedef uspecific_pointer_to_void_function<_Result, _f>	Self;
	typedef typename Self::result_type						Result;

	Result				operator () (void) const
							{ return (*_f)(); }

	typedef _Result		(*function_type) (void);
	function_type 		get_function (void) const
							{ return _f; }
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _Arg1, _Result (*const _f) (_Arg1)>
struct uspecific_pointer_to_unary_function: public uunary_function<_Result, _Arg1> {
	typedef uspecific_pointer_to_unary_function<_Result, _Arg1, _f>	Self;
	typedef typename Self::result_type				Result;
	typedef typename Self::first_argument_type		Argument1;
	typedef typename urefto<Argument1>::t			Ref1;

	Result				operator () (Ref1 a) const
							{ return (*_f)(a); }

	typedef _Result		(*function_type) (_Arg1);
	function_type		get_function (void) const
							{ return _f; }
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _Arg1, typename _Arg2, _Result (*const _f) (_Arg1, _Arg2)>
struct uspecific_pointer_to_binary_function: public ubinary_function<_Result, _Arg1, _Arg2> {
	typedef uspecific_pointer_to_binary_function<_Result, _Arg1, _Arg2, _f>		Self;
	typedef typename Self::result_type				Result;
	typedef typename Self::first_argument_type		Argument1;
	typedef typename Self::second_argument_type		Argument2;
	typedef typename urefto<Argument1>::t			Ref1;
	typedef typename urefto<Argument2>::t			Ref2;

	Result				operator () (Ref1 a, Ref2 b) const
							{ return (*_f)(a, b); }

	typedef _Result		(*function_type) (_Arg1, _Arg2);
	function_type		get_function (void) const
							{ return _f; }
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _Arg1, typename _Arg2, typename _Arg3, _Result (*const _f) (_Arg1, _Arg2, _Arg3)>
struct uspecific_pointer_to_ternary_function: public uternary_function<_Result, _Arg1, _Arg2, _Arg3> {
	typedef uspecific_pointer_to_ternary_function<_Result, _Arg1, _Arg2, _Arg3, _f>		Self;
	typedef typename Self::result_type				Result;
	typedef typename Self::first_argument_type		Argument1;
	typedef typename Self::second_argument_type		Argument2;
	typedef typename Self::third_argument_type		Argument3;
	typedef typename urefto<Argument1>::t			Ref1;
	typedef typename urefto<Argument2>::t			Ref2;
	typedef typename urefto<Argument3>::t			Ref3;

	Result				operator () (Ref1 a, Ref2 b, Ref3 c) const
							{ return (*_f)(a, b, c); }

	typedef _Result		(*function_type) (_Arg1, _Arg2, _Arg3);
	function_type		get_function (void) const
							{ return _f; }
};

/////////////////////////////////////////////////////////////////
// Member function types
//   Types for member functions of arity up to 1 already
//   exist in namespace std::*. The alternatives provided
//   here additionally offer (through their contract)
//   type definitions for the Container-Type (of the
//   member function), as well as uniformity of argument
//   handling.
//   Specifically, arguments are handled as much as possible
//   as references, as they are passed to the original function.
//
//   Extras:
//   struct *mem_fun* {
//		typedef Container;
//		typedef Func;
//		...
//   };
// + uconst_mem_fun
// + umem_fun
// + uconst_mem_fun1
// + umem_fun1
// + uconst_mem_fun2
// + umem_fun2
// + umemberfunctionpointer(funcptr) -> [const_]mem_fun{*} obj
/////////////////////////////////////////////////////////////////

template <typename _Result, typename _T>
struct uconst_mem_fun: public uunary_function<_Result, _T const*> {
public:
	typedef uconst_mem_fun<_Result, _T>					Self;
	typedef typename Self::first_argument_type			Container;
	typedef typename Self::result_type					Result;

	typedef _Result			(_T::* Func) (void) const;

	explicit				uconst_mem_fun (Func const _f): f(_f) {}
	Result					operator () (Container const inst) const
								{ DASSERT(inst); return (inst->*f)(); }
protected:
	Func 					f;
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _T>
struct umem_fun: public uunary_function<_Result, _T*> {
public:
	typedef umem_fun<_Result, _T>						Self;
	typedef typename Self::first_argument_type			Container;
	typedef typename Self::result_type					Result;

	typedef _Result			(_T::* Func) (void);

	explicit				umem_fun (Func const _f): f(_f) {}
	Result					operator () (Container const inst) const
								{ DASSERT(inst); return (inst->*f)(); }
protected:
	Func 					f;
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _T, typename _Arg1>
struct uconst_mem_fun1: public ubinary_function<_Result, _T const*, _Arg1> {
public:
	typedef uconst_mem_fun1<_Result, _T, _Arg1>			Self;
	typedef typename Self::first_argument_type			Container;
	typedef typename Self::result_type					Result;
	typedef typename Self::second_argument_type			Argument1;
	typedef typename urefto<Argument1>::t				Ref1;

	typedef _Result			(_T::* Func) (_Arg1) const;

	explicit				uconst_mem_fun1 (Func const _f): f(_f) {}
	Result					operator () (Container const inst, Ref1 a) const
								{ DASSERT(inst); return (inst->*f)(a); }
protected:
	Func 					f;
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _T, typename _Arg1>
struct umem_fun1: public ubinary_function<_Result, _T*, _Arg1> {
public:
	typedef umem_fun1<_Result, _T, _Arg1>				Self;
	typedef typename Self::first_argument_type			Container;
	typedef typename Self::result_type					Result;
	typedef typename Self::second_argument_type			Argument1;
	typedef typename urefto<Argument1>::t				Ref1;

	typedef _Result			(_T::* Func) (_Arg1);

	explicit				umem_fun1 (Func const _f): f(_f) {}
	Result					operator () (Container const inst, Ref1 a) const
								{ DASSERT(inst); return (inst->*f)(a); }
protected:
	Func 					f;
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _T, typename _Arg1, typename _Arg2>
struct uconst_mem_fun2: public uternary_function<_Result, _T const*, _Arg1, _Arg2> {
public:
	typedef uconst_mem_fun2<_Result, _T, _Arg1, _Arg2>	Self;
	typedef typename Self::first_argument_type			Container;
	typedef typename Self::result_type					Result;
	typedef typename Self::second_argument_type			Argument1;
	typedef typename Self::third_argument_type			Argument2;
	typedef typename urefto<Argument1>::t				Ref1;
	typedef typename urefto<Argument2>::t				Ref2;

	typedef _Result			(_T::* Func) (_Arg1, _Arg2) const;

	explicit				uconst_mem_fun2 (Func const _f): f(_f) {}
	Result					operator () (Container const inst, Ref1 a, Ref2 b) const
								{ DASSERT(inst); return (inst->*f)(a, b); }
protected:
	Func 					f;
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _T, typename _Arg1, typename _Arg2>
struct umem_fun2: public uternary_function<_Result, _T*, _Arg1, _Arg2> {
public:
	typedef umem_fun2<_Result, _T, _Arg1, _Arg2>		Self;
	typedef typename Self::first_argument_type			Container;
	typedef typename Self::result_type					Result;
	typedef typename Self::second_argument_type			Argument1;
	typedef typename Self::third_argument_type			Argument2;
	typedef typename urefto<Argument1>::t				Ref1;
	typedef typename urefto<Argument2>::t				Ref2;

	typedef _Result			(_T::* Func) (_Arg1, _Arg2);

	explicit				umem_fun2 (Func const _f): f(_f) {}
	Result					operator () (Container const inst, Ref1 a, Ref2 b) const
								{ DASSERT(inst); return (inst->*f)(a, b); }
protected:
	Func 					f;
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _Container>
umem_fun<_Result, _Container>						umemberfunctionpointer (_Result (_Container::* const func) (void))
														{ return umem_fun<_Result, _Container>(func); }

template <typename _Result, typename _Container>
uconst_mem_fun<_Result, _Container>					umemberfunctionpointer (_Result (_Container::* const func) (void) const)
														{ return uconst_mem_fun<_Result, _Container>(func); }

template <typename _Result, typename _Container, typename _Arg1>
umem_fun1<_Result, _Container, _Arg1>				umemberfunctionpointer (_Result (_Container::* const func) (_Arg1))
														{ return umem_fun1<_Result, _Container, _Arg1>(func); }

template <typename _Result, typename _Container, typename _Arg1>
uconst_mem_fun1<_Result, _Container, _Arg1>			umemberfunctionpointer (_Result (_Container::* const func) (_Arg1) const)
														{ return uconst_mem_fun1<_Result, _Container, _Arg1>(func); }

template <typename _Result, typename _Container, typename _Arg1, typename _Arg2>
umem_fun2<_Result, _Container, _Arg1, _Arg2>		umemberfunctionpointer (_Result (_Container::* const func) (_Arg1, _Arg2))
														{ return umem_fun2<_Result, _Container, _Arg1, _Arg2>(func); }

template <typename _Result, typename _Container, typename _Arg1, typename _Arg2>
uconst_mem_fun2<_Result, _Container, _Arg1, _Arg2>	umemberfunctionpointer (_Result (_Container::* const func) (_Arg1, _Arg2) const)
														{ return uconst_mem_fun2<_Result, _Container, _Arg1, _Arg2>(func); }

/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
// Pointers to specific member functions
//   Specific pointers to member functions provided for
//   compile time optimisations.
//   There is no way to get these types or objects
//   "easily" from an adapter function. It would require
//   template templating (which may exist, TODO check out).
// + uspecific_const_mem_fun
// + uspecific_mem_fun
// + uspecific_const_mem_fun1
// + uspecific_mem_fun1
// + uspecific_const_mem_fun2
// + uspecific_mem_fun2
/////////////////////////////////////////////////////////////////

template <typename _Result, typename _T, _Result (_T::*const _f) (void) const>
struct uspecific_const_mem_fun: public uunary_function<_Result, _T const*> {
	typedef uspecific_const_mem_fun<_Result, _T, _f>	Self;
	typedef typename Self::result_type				Result;
	typedef typename Self::first_argument_type		Container;

	Result		operator () (Container const inst) const
					{ return (inst->*_f)(); }
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _T, _Result (_T::*const _f) (void)>
struct uspecific_mem_fun: public uunary_function<_Result, _T*> {
	typedef uspecific_mem_fun<_Result, _T, _f>	Self;
	typedef typename Self::result_type				Result;
	typedef typename Self::first_argument_type		Container;

	Result		operator () (Container const inst) const
					{ return (inst->*_f)(); }
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _T, typename _Arg1, _Result (_T::*const _f) (_Arg1) const>
struct uspecific_const_mem_fun1: public ubinary_function<_Result, _T const*, _Arg1> {
	typedef uspecific_const_mem_fun1<_Result, _T, _Arg1, _f>	Self;
	typedef typename Self::result_type				Result;
	typedef typename Self::first_argument_type		Container;
	typedef typename Self::second_argument_type		Argument1;
	typedef typename urefto<Argument1>::t			Ref1;

	Result		operator () (Container const inst, Ref1 a) const
					{ return (inst->*_f)(a); }
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _T, typename _Arg1, _Result (_T::*const _f) (_Arg1)>
struct uspecific_mem_fun1: public ubinary_function<_Result, _T*, _Arg1> {
	typedef uspecific_mem_fun1<_Result, _T, _Arg1, _f>	Self;
	typedef typename Self::result_type				Result;
	typedef typename Self::first_argument_type		Container;
	typedef typename Self::second_argument_type		Argument1;
	typedef typename urefto<Argument1>::t			Ref1;

	Result		operator () (Container const inst, Ref1 a) const
					{ return (inst->*_f)(a); }
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _T, typename _Arg1, typename _Arg2, _Result (_T::*const _f) (_Arg1, _Arg2) const>
struct uspecific_const_mem_fun2: public uternary_function<_Result, _T const*, _Arg1, _Arg2> {
	typedef uspecific_const_mem_fun2<_Result, _T, _Arg1, _Arg2, _f>	Self;
	typedef typename Self::result_type				Result;
	typedef typename Self::first_argument_type		Container;
	typedef typename Self::second_argument_type		Argument1;
	typedef typename Self::third_argument_type		Argument2;
	typedef typename urefto<Argument1>::t			Ref1;
	typedef typename urefto<Argument2>::t			Ref2;

	Result		operator () (Container const inst, Ref1 a, Ref2 b) const
					{ return (inst->*_f)(a, b); }
};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _T, typename _Arg1, typename _Arg2, _Result (_T::*const _f) (_Arg1, _Arg2)>
struct uspecific_mem_fun2: public uternary_function<_Result, _T*, _Arg1, _Arg2> {
	typedef uspecific_mem_fun2<_Result, _T, _Arg1, _Arg2, _f>	Self;
	typedef typename Self::result_type				Result;
	typedef typename Self::first_argument_type		Container;
	typedef typename Self::second_argument_type		Argument1;
	typedef typename Self::third_argument_type		Argument2;
	typedef typename urefto<Argument1>::t			Ref1;
	typedef typename urefto<Argument2>::t			Ref2;

	Result		operator () (Container const inst, Ref1 a, Ref2 b) const
					{ return (inst->*_f)(a, b); }
};

/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Binders
// Binders provided for functions with arity of two (2) or
// three (3).
// All binders are constructed by receiving the value to be bound
// and optionally the operation to which the value is bound.
// If the operation is not provided, it is default constructed.
//
// No binder for functions of arity 0 or 1 is provided. Use
// unarybinder{} and unarybind() for those cases.
//
// Bindings to scalar types and simple references use the
// same type to store the binding.
// Bindings to const references do not store the binding
// as a const reference, but instead create a copy of the
// binding, through its copy constructor.
//
// Example of cool usage:
//		bool f (int, int, int);
//		ubind1st(ubind3rd(uptr_fun(&f), 3), 1)(2);
//
// + ubinder2_1st
// + ubinder2_2nd
// + ubinder3_1st
// + ubinder3_2nd
// + ubinder3_3rd
// + ubind1st(op, val)
// + ubind2nd(op, val)
// + ubind3rd(op, val)
/////////////////////////////////////////////////////////////////

template <typename _Operation, typename _Value = typename _Operation::first_argument_type>
struct ubinder2_1st: public uunary_function<typename _Operation::result_type,
											typename _Operation::second_argument_type> {
private:
	typedef	_Operation								Operation;
	typedef ubinder2_1st<Operation>					Self;
	typedef typename Self::result_type				Result;
	typedef typename Self::first_argument_type		Argument1;
	typedef typename urefto<Argument1>::t			Ref1;
	typedef typename uref_or_scal<_Value>::t		Value;
					// WARNING: binding to const-references is VERY dangerous.
					// It usually implies that temporary objects are being bound,
					// which will be destroyed after the call of ubind*() is over.
					// Therefore, const& bindings are prohibited.
public:
	typedef typename urefto<Value>::t				ValRef;	// public

	Result		operator () (Ref1 b) const
					{ return op(typename Operation::first_argument_type(value), b); }

	explicit	ubinder2_1st (ValRef _value, Operation const& _op = Operation()):
					op(_op), value(_value) { }
				ubinder2_1st (Self const& other):
					op(other.op), value(other.value) { }
protected:
	Operation	op;
	Value		value;
private:
	// no assignments -- we can't know that Operation or Value will not be reference types.
	void		operator = (Self const&);
};

/////////////////////////////////////////////////////////////////

template <typename _Operation, typename _Value = typename _Operation::second_argument_type>
struct ubinder2_2nd: public uunary_function<typename _Operation::result_type,
											typename _Operation::first_argument_type> {
private:
	typedef	_Operation								Operation;
	typedef ubinder2_2nd<Operation>					Self;
	typedef typename Self::result_type				Result;
	typedef typename Self::first_argument_type		Argument1;
	typedef typename urefto<Argument1>::t			Ref1;
	typedef typename uref_or_scal<_Value>::t		Value;
					// WARNING: binding to const-references is VERY dangerous.
					// It usually implies that temporary objects are being bound,
					// which will be destroyed after the call of ubind*() is over.
					// Therefore, const& bindings are prohibited.
public:
	typedef typename urefto<Value>::t				ValRef;	// public

	Result		operator () (Ref1 a) const
					{ return op(a, typename Operation::second_argument_type(value)); }

	explicit	ubinder2_2nd (ValRef _value, Operation const& _op = Operation()):
					op(_op), value(_value) { }
				ubinder2_2nd (Self const& other):
					op(other.op), value(other.value) { }
protected:
	Operation	op;
	Value		value;
private:
	// no assignments -- we can't know that Operation or Value will not be reference types.
	void		operator = (Self const&);
};

/////////////////////////////////////////////////////////////////

template <typename _Operation, typename _Value = typename _Operation::first_argument_type>
struct ubinder3_1st: public ubinary_function<	typename _Operation::result_type,
												typename _Operation::second_argument_type,
												typename _Operation::third_argument_type> {
private:
	typedef	_Operation								Operation;
	typedef ubinder3_1st<Operation>					Self;
	typedef typename Self::result_type				Result;
	typedef typename Self::first_argument_type		Argument1;
	typedef typename Self::second_argument_type		Argument2;
	typedef typename urefto<Argument1>::t			Ref1;
	typedef typename urefto<Argument2>::t			Ref2;
	typedef typename uref_or_scal<_Value>::t		Value;	
					// WARNING: binding to const-references is VERY dangerous.
					// It usually implies that temporary objects are being bound,
					// which will be destroyed after the call of ubind*() is over.
					// Therefore, const& bindings are prohibited.
public:
	typedef typename urefto<Value>::t				ValRef;	// public

	Result		operator () (Ref1 b, Ref2 c) const
					{ return op(typename Operation::first_argument_type(value), b, c); }

	explicit	ubinder3_1st (ValRef _value, Operation const& _op = Operation()):
					op(_op), value(_value) { }
				ubinder3_1st (Self const& other):
					op(other.op), value(other.value) { }
protected:
	Operation	op;
	Value		value;
private:
	// no assignments -- we can't know that Operation or Value will not be reference types.
	void		operator = (Self const&);
};

/////////////////////////////////////////////////////////////////

template <typename _Operation, typename _Value = typename _Operation::second_argument_type>
struct ubinder3_2nd: public ubinary_function<	typename _Operation::result_type,
												typename _Operation::first_argument_type,
												typename _Operation::third_argument_type> {
private:
	typedef	_Operation								Operation;
	typedef ubinder3_2nd<Operation>					Self;
	typedef typename Self::result_type				Result;
	typedef typename Self::first_argument_type		Argument1;
	typedef typename Self::second_argument_type		Argument2;
	typedef typename urefto<Argument1>::t			Ref1;
	typedef typename urefto<Argument2>::t			Ref2;
	typedef typename uref_or_scal<_Value>::t		Value;
public:
	typedef typename urefto<Value>::t				ValRef;
					// WARNING: binding to const-references is VERY dangerous.
					// It usually implies that temporary objects are being bound,
					// which will be destroyed after the call of ubind*() is over.
					// Therefore, const& bindings are prohibited.

	Result		operator () (Ref1 a, Ref2 c) const
					{ return op(a, typename Operation::second_argument_type(value), c); }

	explicit	ubinder3_2nd (ValRef _value, Operation const& _op = Operation()):
					op(_op), value(_value) { }
				ubinder3_2nd (Self const& other):
					op(other.op), value(other.value) { }
protected:
	Operation	op;
	Value		value;
private:
	// no assignments -- we can't know that Operation or Value will not be reference types.
	void		operator = (Self const&);
};

/////////////////////////////////////////////////////////////////

template <typename _Operation, typename _Value = typename _Operation::third_argument_type>
struct ubinder3_3rd: public ubinary_function<	typename _Operation::result_type,
												typename _Operation::first_argument_type,
												typename _Operation::second_argument_type> {
private:
	typedef	_Operation								Operation;
	typedef ubinder3_3rd<Operation>					Self;
	typedef typename Self::result_type				Result;
	typedef typename Self::first_argument_type		Argument1;
	typedef typename Self::second_argument_type		Argument2;
	typedef typename urefto<Argument1>::t			Ref1;
	typedef typename urefto<Argument2>::t			Ref2;
	typedef typename uref_or_scal<_Value>::t		Value;
					// WARNING: binding to const-references is VERY dangerous.
					// It usually implies that temporary objects are being bound,
					// which will be destroyed after the call of ubind*() is over.
					// Therefore, const& bindings are prohibited.
public:
	typedef typename urefto<Value>::t				ValRef;

	Result		operator () (Ref1 a, Ref2 b) const
					{ return op(a, b, typename Operation::third_argument_type(value)); }

	explicit	ubinder3_3rd (ValRef _value, Operation const& _op = Operation()):
					op(_op), value(_value) { }
				ubinder3_3rd (Self const& other):
					op(other.op), value(other.value) { }
protected:
	Operation	op;
	Value		value;
private:
	// no assignments -- we can't know that Operation or Value will not be reference types.
	void		operator = (Self const&);
};

/////////////////////////////////////////////////////////////////
// Traits for ubind*().
// Custom functors should specialise the ubind_traits<Operation>
// template if need to be bindable.
// For functors that inherit from the u-functor-supertypes
// (ubinary_function, etc), there are convencience default
// traits:
//		ubind2_traits<Operation>	for binary functors
//		ubind3_traits<Operation>	for ternary functors.

template <typename _Operation>
struct ubind3_traits {
private:
	typedef	_Operation									Operation;
public:
	typedef ubinder3_1st<Operation>						binder1st_type;
	typedef ubinder3_2nd<Operation>						binder2nd_type;
	typedef ubinder3_3rd<Operation>						binder3rd_type;
};

template <typename _Operation>
struct ubind2_traits {
private:
	typedef	_Operation									Operation;
public:
	typedef ubinder2_1st<Operation>						binder1st_type;
	typedef ubinder2_2nd<Operation>						binder2nd_type;
};

/////////////////////////////////////////////////////////////////

template <typename _Operation>
struct ubind_traits {};

/////////////////////////////////////////////////////////////////

template <typename _Result, typename _Arg1, typename _Arg2, typename _Arg3, typename _Func>
struct ubind_traits<upointer_to_ternary_function<_Result,_Arg1,_Arg2,_Arg3,_Func> >:
	public ubind3_traits<upointer_to_ternary_function<_Result,_Arg1,_Arg2,_Arg3,_Func> >
	{ };

template <typename _Result, typename _Arg1, typename _Arg2, typename _Func>
struct ubind_traits<upointer_to_binary_function<_Result,_Arg1,_Arg2,_Func> >:
	public ubind2_traits<upointer_to_binary_function<_Result,_Arg1,_Arg2,_Func> >
	{ };

template <typename _Operation, typename _Value>
struct ubind_traits<ubinder3_1st<_Operation, _Value> >:
	public ubind2_traits<ubinder3_1st<_Operation, _Value> >
	{ };

template <typename _Operation, typename _Value>
struct ubind_traits<ubinder3_2nd<_Operation, _Value> >:
	public ubind2_traits<ubinder3_2nd<_Operation, _Value> >
	{ };

template <typename _Operation, typename _Value>
struct ubind_traits<ubinder3_3rd<_Operation, _Value> >:
	public ubind2_traits<ubinder3_3rd<_Operation, _Value> >
	{ };

template <typename _Result, typename _Arg1, typename _Arg2, _Result (*const _f) (_Arg1, _Arg2)>
struct ubind_traits<uspecific_pointer_to_binary_function<_Result,_Arg1,_Arg2,_f> >:
	public ubind2_traits<uspecific_pointer_to_binary_function<_Result,_Arg1,_Arg2,_f> >
	{ };

template <typename _Result, typename _Arg1, typename _Arg2, typename _Arg3, _Result (*const _f) (_Arg1, _Arg2, _Arg3)>
struct ubind_traits<uspecific_pointer_to_ternary_function<_Result,_Arg1,_Arg2,_Arg3,_f> >:
	public ubind3_traits<uspecific_pointer_to_ternary_function<_Result,_Arg1,_Arg2,_Arg3,_f> >
	{ };

template <typename _Result, typename _T, typename _Arg1>
struct ubind_traits<umem_fun1<_Result,_T,_Arg1> >:
	public ubind2_traits<umem_fun1<_Result,_T,_Arg1> >
	{ };

template <typename _Result, typename _T, typename _Arg1>
struct ubind_traits<uconst_mem_fun1<_Result,_T,_Arg1> >:
	public ubind2_traits<uconst_mem_fun1<_Result,_T,_Arg1> >
	{ };

template <typename _Result, typename _T, typename _Arg1, typename _Arg2>
struct ubind_traits<uconst_mem_fun2<_Result,_T,_Arg1,_Arg2> >:
	public ubind3_traits<uconst_mem_fun2<_Result,_T,_Arg1,_Arg2> >
	{ };

template <typename _Result, typename _T, typename _Arg1, typename _Arg2>
struct ubind_traits<umem_fun2<_Result,_T,_Arg1,_Arg2> >:
	public ubind3_traits<umem_fun2<_Result,_T,_Arg1,_Arg2> >
	{ };

template <typename _Result, typename _T, typename _Arg1, typename _Arg2, _Result (_T::*const _f) (_Arg1, _Arg2) const>
struct ubind_traits<uspecific_const_mem_fun2<_Result,_T,_Arg1,_Arg2,_f> >:
	public ubind3_traits<uspecific_const_mem_fun2<_Result,_T,_Arg1,_Arg2,_f> >
	{ };

template <typename _Result, typename _T, typename _Arg1, typename _Arg2, _Result (_T::*const _f) (_Arg1, _Arg2)>
struct ubind_traits<uspecific_mem_fun2<_Result,_T,_Arg1,_Arg2,_f> >:
	public ubind3_traits<uspecific_mem_fun2<_Result,_T,_Arg1,_Arg2,_f> >
	{ };

template <typename _Result, typename _T, typename _Arg1, _Result (_T::*const _f) (_Arg1) const>
struct ubind_traits<uspecific_const_mem_fun1<_Result,_T,_Arg1,_f> >:
	public ubind2_traits<uspecific_const_mem_fun1<_Result,_T,_Arg1,_f> >
	{ };

template <typename _Result, typename _T, typename _Arg1, _Result (_T::*const _f) (_Arg1)>
struct ubind_traits<uspecific_mem_fun1<_Result,_T,_Arg1,_f> >:
	public ubind2_traits<uspecific_mem_fun1<_Result,_T,_Arg1,_f> >
	{ };

/////////////////////////////////////////////////////////////////

// WARNING: this template version in which the second parameter is an arbitrary type _Value
// is problematic. Explicit casting of _Value to ubind_traits<_Operation>::binder1st_type::ValRef
// does not report invalid pointer casts in VS2010 (03-aug-2011).
// Also, it is not deduced as a volatile-reference type if an argument of such type is passed,
// but instead the argument is copy-constructed and _Value is deduced as the original type.
// If it is necessary to use _Value for flexibility, refactor with care for the issues
// mentioned above.

template <typename _Operation>
typename ubind_traits<_Operation>::binder1st_type ubind1st (_Operation const& op, typename ubind_traits<_Operation>::binder1st_type::ValRef val) {
	return typename ubind_traits<_Operation>::binder1st_type(val, op);
}

template <typename _Operation>
typename ubind_traits<_Operation>::binder2nd_type ubind2nd (_Operation const& op, typename ubind_traits<_Operation>::binder2nd_type::ValRef val) {
	return typename ubind_traits<_Operation>::binder2nd_type(val, op);
}

template <typename _Operation>
typename ubind_traits<_Operation>::binder3rd_type ubind3rd (_Operation const& op, typename ubind_traits<_Operation>::binder3rd_type::ValRef val) {
	return typename ubind_traits<_Operation>::binder3rd_type(val, op);
}

/////////////////////////////////////////////////////////////////

//---------------------------------------------------------------


#endif	// Do not add stuff beyond this point.
