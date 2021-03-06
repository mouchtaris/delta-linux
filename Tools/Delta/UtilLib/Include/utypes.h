// utypes.h
// Some common built-in type definitions to
// cater for differences in size accross different
// platforms.
// ScriptFighter Project.
// A. Savidis, October 2002.
// Modified February 2007, removed non-partial template
// specialization tricks.
//

#ifndef	UTYPES_H
#define	UTYPES_H

#include <map>
#include <algorithm>
#include <functional>
#include <list>
#include <string>

//---------------------------------------------------------------
// Generic.

#ifdef	_MSC_VER	// Compiling with visual studio.
#define	UDLL_EXPORT_CPP_CLASS	__declspec(dllexport)	// Full C++ name mangling
#define	UDLL_EXPORT_CPP	__declspec(dllexport)	// Full C++ name mangling
#define	UDLL_EXPORT_C	extern "C" __declspec(dllexport)	// No name mangling
#define	UDLL_IMPORT_CPP_CLASS	__declspec(dllimport)
#define	UDLL_IMPORT_CPP	__declspec(dllimport)
#define	UDLL_IMPORT_C	extern "C" __declspec(dllimport)
#endif

#ifdef	__GNUG__	// Compiling with g++.
#define	UDLL_EXPORT_CPP_CLASS
#define	UDLL_EXPORT_CPP	extern
#define	UDLL_EXPORT_C	extern "C"
#define	UDLL_IMPORT_CPP_CLASS
#define	UDLL_IMPORT_CPP	extern
#define	UDLL_IMPORT_C	extern "C"
#endif

//---------------------------------------------------------------

#if defined(UTILLIB_EXPORT) && defined(UTILLIB_IMPORT)
#error "Can't define both UTILLIB_EXPORT and UTILLIB_IMPORT!"
#endif

#if	defined(UTILLIB_EXPORT)
#define	UTILLIB_CLASS	UDLL_EXPORT_CPP_CLASS
#define	UTILLIB_FUNC	UDLL_EXPORT_CPP
#define	UTILLIB_CFUNC	UDLL_EXPORT_C
#define	UTILEXP_CLASS
#elif defined(UTILLIB_IMPORT)
#define	UTILLIB_CLASS	UDLL_IMPORT_CPP_CLASS
#define	UTILLIB_FUNC	UDLL_IMPORT_CPP
#define	UTILLIB_CFUNC	UDLL_IMPORT_C
#define	UTILEXP_CLASS	extern
#else
#define	UTILLIB_CLASS
#define	UTILEXP_CLASS
#define	UTILLIB_FUNC	extern
#define	UTILLIB_CFUNC	extern "C"
#endif

//---------------------------------------------------------------

#ifdef	_MSC_VER	// Compiling with visual studio.
#pragma warning (disable : 4786)	// (old) name truncation
#pragma warning (disable : 4250)	// 'class1' : inherits 'class2::member' via dominance (diamond case)
#pragma warning (disable : 4996)	// '<std func>': This function or variable may be unsafe.
#pragma warning (disable : 4800)	// 'int' : forcing value to bool 'true' or 'false' (performance warning)
#pragma warning (disable : 4311)	// 'type cast' : pointer truncation from 'T*' to 'util_ui32'
#pragma warning (disable : 4311)	// 'type cast' : pointer truncation from 'T*' to 'util_ui32'
#pragma warning (disable : 4251)	// class '<this one>' needs to have dll-interface to be used by clients
#pragma warning (disable : 4661)	//  no suitable definition provided for explicit template instantiation request
#pragma warning (disable : 4312)	// conversion from '<this>' to '<that>' of greater size
#pragma warning (disable : 4275)	// non dll-interface struct '<this>' used as base for dll-interface struct '<that>'
#pragma warning (disable : 4355)	// 'this' : used in base member initializer list
#pragma warning (disable : 4503)	// 'template class' : decorated name length exceeded, name was truncated
#endif

//---------------------------------------------------------------

#define	UEMPTY

#define	UMB(i)	(((unsigned) i) << 20)
#define	UKB(i)	(((unsigned) i) << 10)
#define	UMb(i)	(((unsigned) i) << 19)
#define	UKb(i)	(((unsigned) i) << 9)


typedef	signed char		util_i8;		// Signed byte.
typedef	unsigned char	util_ui8;		// Unsigned byte.

typedef	unsigned short	util_ui16;		// Two bytes, unsigned.
typedef signed short	util_i16;		// Two bytes, signed.

typedef unsigned int	util_ui32;		// Four bytes, unsigned.
typedef signed int		util_i32;		// Four bytes, signed.

template <class T> struct uptr { typedef T* type; };

#ifdef	UPTR
#error	"Redefinition of UPTR: this may cause severe errors!"
#endif

#define	UPTR(T) uptr<T>::type

#ifdef	_MSC_VER	// Compiling with visual studio.
typedef __int64				util_i64;		// Eight bytes, signed.
typedef unsigned __int64	util_ui64;		// Eight bytes, unsigned.
#endif

#ifdef	__GNUG__	// Compiling with g++.
#include <sys/types.h>
typedef __int64_t			util_i64;		// Eight bytes, signed.
typedef __uint64_t			util_ui64;		// Eight bytes, unsigned.
#endif


// Unused parameters, variables and static functions
#if		defined( _MSC_VER )	// Compiling with visual studio
#	define	POSSIBLY_UNUSED_LOCAL(A)			A
#	define	POSSIBLY_UNUSED_STATIC_FUNCTION(A)	A;
#elif	defined( __GNUG__ )	// Compiling with g++
#	define	POSSIBLY_UNUSED_LOCAL(A)			A __attribute__((unused))
#	define	POSSIBLY_UNUSED_STATIC_FUNCTION(A)	static A __attribute__((unused));
#else
#	error "Unknown compiler"
#endif

typedef void (*uvoidvoid_f)(void);
typedef std::list<std::string> ustrlist;

//---------------------------------------------------------------
// This is a template to control use of the original type T, or a 
// reference to T, through a boolean compile time flag.
//
template <class T, bool useRef> struct ureftyper {};
template <class T>				struct ureftyper<T, true>	{ typedef T& type; };
template <class T>				struct ureftyper<T, false>	{ typedef T type; };

template <typename T> 			struct uconst_of			{ typedef T t; };
template <typename T> 			struct uconst_of<const T>	{ typedef typename uconst_of<T>::t t; };

template <typename T> 			struct uptr_of				{ typedef T t; };
template <typename T> 			struct uptr_of<T*>			{ typedef typename uptr_of<T>::t t; };

template <typename T>			struct uconstref_of				{ typedef const T& t; };
template <typename T>			struct uconstref_of<T&>			{ typedef typename uconstref_of<T>::t t; };
template <typename T>			struct uconstref_of<const T&>	{ typedef typename uconstref_of<T>::t t; };

template <typename T>			struct uref_of				{ typedef T& t; };
template <typename T>			struct uref_of<const T>		{ typedef typename uref_of<T>::t t; };
template <typename T>			struct uref_of<T&>			{ typedef typename uref_of<T>::t t; };
template <typename T>			struct uref_of<const T&>	{ typedef typename uref_of<T>::t t; };

template <typename T> const T*	uaddress_of (const T& x)	{ return &x; }
template <typename T> T*		uaddress_of (T& x)			{ return &x; }
template <typename T> T*		uaddress_of (T* x)			{ return x; }
template <typename T> const T*	uaddress_of (const T* x)	{ return x; }

//---------------------------------------------------------------

template <typename T>			struct utempref				{ T& ref; utempref(const T& val) : ref(const_cast<T&>(val)){} };
template <typename T>			struct utempref<T*>			{ T* ref; utempref(T* val) : ref(val){} };

//---------------------------------------------------------------
// Never T::~T(), it ignores late binding.
template <typename T> void udestructor_invocation (T* t)
	{ t->~T(); } 

template <typename T> void ureset_via_default_ctor (T* t)
	{ udestructor_invocation(t); new (t) T(); } 

template <typename T, typename C> void ureset_via_ctor_arg (T* t, const C& arg)
	{ udestructor_invocation(t); new (t) T(arg); } 

#define	UOVERLOADED_VOID_ASSIGN_VIA_COPY_CONSTRUCTOR(_class)				\
	void operator=(const _class& rvalue) {									\
		if (this != &rvalue) {												\
			udestructor_invocation(this);									\
			new (this) _class(rvalue);										\
		}																	\
	}

#define	UOVERLOADED_ASSIGN_VIA_COPY_CONSTRUCTOR(_class)						\
	const _class& operator=(const _class& rvalue) {							\
		if (this != &rvalue) {												\
			udestructor_invocation(this);									\
			new (this) _class(rvalue);										\
		}																	\
		return *this;														\
	}

#define	UASSIGN_OPERATOR_RETURN_VOID(_class)								\
	UOVERLOADED_VOID_ASSIGN_VIA_COPY_CONSTRUCTOR(_class)

#define	UASSIGN_OPERATOR_RETURN_THIS(_class)								\
	UOVERLOADED_ASSIGN_VIA_COPY_CONSTRUCTOR(_class)

#define	UOVERLOADED_ASSIGN_VIA_CONVERTER_CONSTRUCTOR(_class,_rvalue)		\
	const _class& operator=(const _rvalue& rvalue) {						\
		DASSERT((void*) this != (void*) &rvalue);							\
		udestructor_invocation(this);										\
		new (this) _class(rvalue);											\
		return *this;														\
	}

#define	UCLONE_VIA_COPY_CONSTRUCTOR(_class, _clone)							\
	_class* _clone (void) const {											\
		return DNEWCLASS(_class, (*this));									\
	}

#define	UCLONE_VIRTUAL_VIA_COPY_CONSTRUCTOR(_class, _clone)					\
	virtual _class* _clone (void) const {									\
		return DNEWCLASS(_class, (*this));									\
	}

#define	USUBCLASS_RETAINING_DEFAULT_AND_COPY_CONSTUCTOR(_class, _base)		\
	_class (void){}															\
	_class (const _class& inst) : _base((const _base&) inst){}				\
	~_class(){}

#define	UATTRIBUTEMETHOD_SCALAR(T,_var,_attr)				\
	void Set##_attr(T _var##New) {_var = _var##New; }		\
	T Get##_attr(void) const {return _var;}

#define	UATTRIBUTEMETHOD_AGGREGATE(T,_var,_attr)			\
	void Set##_attr(const T& _var##New) {_var = _var##New;}	\
	const T& Get##_attr(void) const {return _var; }

#define	UATTRIBUTEMETHOD_SCALAR_EX(T,_var,_attr,_check)		\
	void Set##_attr(T _var##New)							\
		{_check(_var##New); _var = _var##New; }				\
	T Get##_attr(void) const { return _var; }

#define	UATTRIBUTEMETHOD_AGGREGATE_EX(T,_var,_attr,_check)	\
	void Set##_attr(const T& _var##New)						\
		{_check(_var##New); _var = _var##New; }				\
	const T& Get##_attr(void) const { return _var; }

#define	UATTRIBUTEMETHOD_SCALAR_INV(T,_var,_attr,_inv)		\
	void Set##_attr(T _var##New)							\
		{_var = _var##New; DASSERT(_inv()); }				\
	T Get##_attr(void) const								\
		{  DASSERT(_inv()); return _var; }

#define	UATTRIBUTEMETHOD_AGGREGATE_INV(T,_var,_attr,_inv)	\
	void Set##_attr(const T& _var##New)						\
		{ _var = _var##New; DASSERT(_inv()); }				\
	const T& Get##_attr(void) const							\
		{ DASSERT(_inv()); return _var; }


#define	UATTRIBUTEMETHOD_SELF_SCALAR(_class, T,_var,_attr)						\
	_class& Set##_attr(T _var##New) {_var = _var##New; return *this; }			\
	T Get##_attr(void) const {return _var;}

#define	UATTRIBUTEMETHOD_SELF_AGGREGATE(_class, T,_var,_attr)					\
	_class& Set##_attr(const T& _var##New) {_var = _var##New; return *this; }	\
	const T& Get##_attr(void) const {return _var; }

#define	UATTRIBUTEMETHOD_SELF_SCALAR_EX(_class, T,_var,_attr,_check)			\
	_class& Set##_attr(T _var##New)												\
		{_check(_var##New); _var = _var##New; return *this; }					\
	T Get##_attr(void) const { return _var; }

#define	UATTRIBUTEMETHOD_SELF_AGGREGATE_EX(_class, T,_var,_attr,_check)			\
	_class& Set##_attr(const T& _var##New)										\
		{_check(_var##New); _var = _var##New; return *this; }					\
	const T& Get##_attr(void) const { return _var; }

#define	UATTRIBUTEMETHOD_SELF_SCALAR_INV(_class, T,_var,_attr,_inv)				\
	_class& Set##_attr(T _var##New)												\
		{_var = _var##New; DASSERT(_inv()); return *this; }						\
	T Get##_attr(void) const													\
		{  DASSERT(_inv()); return _var; }

#define	UATTRIBUTEMETHOD_SELF_AGGREGATE_INV(_class, T,_var,_attr,_inv)			\
	_class& Set##_attr(const T& _var##New)										\
		{ _var = _var##New; DASSERT(_inv()); return *this; }					\
	const T& Get##_attr(void) const												\
		{ DASSERT(_inv()); return _var; }

#define	UCLASSID_STD_ABSTRACT_METHOD						\
	virtual const std::string	GetClassId (void) const = 0;

#define	UCLASSID_STD_DERIVED_METHODS(_classid)				\
	virtual const std::string	GetClassId (void) const		\
									{ return _classid; }	\
	static const std::string	ClassId (void)				\
									{ return _classid; }

//---------------------------------------------------------------

// This template converts a constant integer to a distinct type.
// It allows overloading of functions with simple enumerated
// constants; e.g. uint2type<10> is a different type than uint2type<11>,
// hence, f(uint2type<10>) is a different function than f(uint2type<11>).
// The same is also supplied for booleans (though the uint2type can be
// used with booleans too).

template <const unsigned int N> struct uint2type	{ enum { value = N }; };
template <const bool val>		struct ubool2type	{ enum { value = val ? 1 : 0 }; };
template <class T>				struct utype2type	{ typedef T type; };

//---------------------------------------------------------------
// A quite useful template to distinguish a 'void' type.

template <class T> struct uisvoid		{ enum { value = 0 }; };
template<> struct uisvoid<void>			{ enum { value = 1 }; };
template <class T> struct uisnotvoid	{ enum { value = 1 }; };
template<> struct uisnotvoid<void>		{ enum { value = 0 }; };

//---------------------------------------------------------------
// Some quite useful templates to make certain that no unexpected
// type conversions are going around.
template <typename T1, typename T2>	struct utypeequality		{ static const bool result = false;	};
template <typename T>				struct utypeequality<T,T>	{ static const bool result = true;	};

template <const bool condition>	struct ucompilecheck;
template <>						struct ucompilecheck<true> {};

template <typename Tmust, typename T> static inline
T& utypecheck (T& val)	{ typedef char type_inequality_indicator[sizeof(ucompilecheck<utypeequality<Tmust,T>::result>)]; return val; }

//*****************************
// A useful template to convert the sizeof void to 0,
// while for all other types it is the original sizeof.
//
template <class T> struct usizeof		{ static util_ui32 value (void) { return sizeof(T); } };
template<> struct usizeof<void>			{ static util_ui32 value (void) { return 0; } };

//*****************************

inline bool uisnatural (double val) 
	{ return val >= 0 && ((util_ui32) val) == val; }

inline bool uhasnotfractional (double val) 
	{ return ((util_i32) val) == val; }

template <class T, const util_ui32 N>
util_ui32 uarraysize (T (&P)[N]) { return N; }

template <class T, const util_ui32 N>
void uassignarray (const T (&from)[N],  T (&to)[N]) {
	const T*	p_from	= from;
	T*			p_to	= to;
	util_ui32	i		= N; 
	while (i--)
		*p_to++ = *p_from++;
}

template <typename T, const unsigned N>  struct uptrarray { 
	typedef T (*ptr_type)[N]; 
	typedef T arr_type[N]; 
};

//*****************************

template <typename T> T& utempobj (const T& a)	// Use when a temp object should be passed as non-const ref.
	{ return const_cast<T&>(a); }

template <typename FuncPtrT> static inline void* ufunc2void (FuncPtrT const func_p) // Use when a function pointer needs to be cast to a void pointer.
	{ return *(void**)&func_p; }

template <typename FuncPtrT> static inline void* ufunc2void (FuncPtrT const func_p, void*& void_p) // Use when a function pointer needs to be cast to a void pointer.
	{ return void_p = *(void**)&func_p; }

template <typename FuncPtrT> static inline FuncPtrT const uvoid2func (void const* const void_p, FuncPtrT& func_p) // Use when casting a void pointer to a function pointer.
	{ (*(void const**)&func_p) = void_p; return func_p; }

template <typename FuncPtrT> static inline FuncPtrT const uvoid2func (void const* const void_p) // Use when casting a void pointer to a function pointer.
	{ FuncPtrT func_p; return uvoid2func(void_p, func_p); }

// Also, in macro editions
#if defined( __GNUG__ )
#	define UFUNC2VOID(func_ptr_var)						ufunc2void(func_ptr_var)
#	define UFUNCIN2VOID(func_ptr, void_ptr)				ucastassign(void_ptr, func_ptr)
#	define UVOID2FUNC(FuncPtrT, void_ptr)				uvoid2func<FuncPtrT>(void_ptr)
#	define UVOIDIN2FUNC(void_ptr, FuncPtrT, func_ptr)	uvoid2func<FuncPtrT>(void_ptr, func_ptr) // template argument provided explicitly for better type checking
#else
#	define UFUNC2VOID(func_ptr_var)						((void*) func_ptr_var)
#	define UFUNCIN2VOID(func_ptr, void_ptr)				ucastassign(void_ptr, func_ptr)
#	define UVOID2FUNC(FuncPtrT, void_ptr)				((FuncPtrT) void_ptr)
#	define UVOIDIN2FUNC(void_ptr, FuncPtrT, func_ptr)	ucastassign(func_ptr, void_ptr)
#endif


//*****************************
// Macros doing nothing -- in all flavors for number of arguments
#define UNOTHING0
#define UNOTHING1(A)
#define UNOTHING2(A,B)
#define UNOTHING3(A,B,C)
#define UNOTHING4(A,B,C,D)
#define UNOTHING5(A,B,C,D,E)

//*****************************

template <typename T> struct uvoidptrcaster {};

template <typename T>
struct uvoidptrcaster<T*> {
	void* operator()(T* p) const {
#ifdef _DEBUG
		// make sure that this template is used
		// *only* for casting pointers-to-data
		T* pcopy(p);
		++pcopy;
#endif // _DEBUG
		return p;
	}
	T* operator()(void* p) const
		{ return (T*) p; }
};

template <typename R>
struct uvoidptrcaster<R(*)(void)> {
	typedef R(*Rf)(void);
	void*	operator()(Rf p) const		{ return ufunc2void(p);	}
	Rf		operator()(void* p) const	{ return uvoid2func<Rf>(p); }
};

template <typename R, typename A1>
struct uvoidptrcaster<R(*)(A1)> {
	typedef R(*Rf)(A1);
	void*	operator()(Rf p) const		{ return ufunc2void(p); }  
	Rf		operator()(void* p) const	{ return uvoid2func<Rf>(p); } 
};

template <typename R, typename A1, typename A2>
struct uvoidptrcaster<R(*)(A1,A2)> {
	typedef R(*Rf)(A1,A2);
	void*	operator()(Rf p) const 		{ return ufunc2void(p); }
	Rf		operator()(void* p) const	{ return uvoid2func<Rf>(p); } 
};

template <typename R, typename A1, typename A2, typename A3>
struct uvoidptrcaster<R(*)(A1,A2,A3)> {
	typedef R(*Rf)(A1,A2,A3);
	void*	operator()(Rf p) const		{ return ufunc2void(p); } 
	Rf		operator()(void* p) const	{ return uvoid2func<Rf>(p); } 
};

template <typename R, typename A1, typename A2, typename A3, typename A4>
struct uvoidptrcaster<R(*)(A1,A2,A3,A4)> {
	typedef R(*Rf)(A1,A2,A3,A4);
	void*	operator()(Rf p) const		{ return ufunc2void(p); }
	Rf		operator()(void* p) const	{ return uvoid2func<Rf>(p); } 
};

template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
struct uvoidptrcaster<R(*)(A1,A2,A3,A4,A5)> {
	typedef R(*Rf)(A1,A2,A3,A4,A5);
	void*	operator()(Rf p) const		{ return ufunc2void(p); }
	Rf		operator()(void* p) const	{ return uvoid2func<Rf>(p); } 
};

template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct uvoidptrcaster<R(*)(A1,A2,A3,A4,A5,A6)> {
	typedef R(*Rf)(A1,A2,A3,A4,A5,A6);
	void*	operator()(Rf p) const		{ return ufunc2void(p); }
	Rf		operator()(void* p) const	{ return uvoid2func<Rf>(p); } 
};

template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct uvoidptrcaster<R(*)(A1,A2,A3,A4,A5,A6,A7)> {
	typedef R(*Rf)(A1,A2,A3,A4,A5,A6,A7);
	void*	operator()(Rf p) const		{ return ufunc2void(p); }
	Rf		operator()(void* p) const	{ return uvoid2func<Rf>(p); } 
};

//*****************************

template <typename Tl, typename Tr> struct ucastassigner  {
	Tl operator()(Tl& lv, const Tr& rv) const {
#ifdef _DEBUG
		// make certain that this template is used
		// *only* for casting pointers-to-data.
		Tr rvcopy(rv);
		++rvcopy;
		Tl lvcopy(lv);
		++lvcopy;
#endif // _DEBUG
		return lv = (Tl) rv;
	}
};

template <typename Tr> struct ucastassigner<void*, Tr>  {
	void* operator()(void*& lv, const Tr& rv) const
		{ return lv = uvoidptrcaster<Tr>()(rv); }
};

template <typename Tl> struct ucastassigner<Tl*, void*>  {
	Tl* operator()(Tl*& lv, void* const& rv) const
		{ return lv = uvoidptrcaster<Tl*>()(rv); }
};

template <typename Tl, typename Tr>	// Use when should avoid hard-coding type casting.
Tl ucastassign (Tl& lv,  const Tr& rv)
	{ return ucastassigner<Tl, Tr>()(lv, rv); }

//---------------------------------------------------------------
// std::map adapter as a fast bag (essentially a set).
//
template <class T> class ubag {

	private:
	std::map<T, bool> bag;

	template <typename C>
	struct bag_inserter : public std::binary_function<std::pair<C, bool>, ubag*, void> {
		void operator()(const std::pair<C, bool>& p, ubag* b) const 
			{ b->insert(p.first); }
	};

	template <typename C>
	struct list_inserter : public std::binary_function<C, ubag*, void> {
		void operator()(const C& p, ubag* b) const 
			{ b->insert(p); }
	};
	
	template <typename C>
	struct bag_remover : public std::binary_function<std::pair<C, bool>, ubag*, void> {
		void operator()(const std::pair<C, bool>& p, ubag* b) const 
			{ b->remove(p.first); }
	};

	template <typename C>
	struct list_remover : public std::binary_function<C, ubag*, void> {
		void operator()(const C& p, ubag* b) const 
			{ b->remove(p); }
	};

	public:
	typedef std::pair<const T, bool>					elem_type;
	typedef T											value_type;
	typedef typename std::map<T, bool>::iterator		iterator; 
	typedef typename std::map<T, bool>::const_iterator	const_iterator; 

	const iterator	begin (void)				{ return bag.begin();				}
	const iterator	end (void)					{ return bag.end();					}
	const_iterator	begin (void) const			{ return bag.begin();				}
	const_iterator	end (void) const			{ return bag.end();					}
	void			insert (const T& x)			{ bag[x] = true;					}
	
	template <typename C>
	void			insert (const ubag<C>& b)	{ std::for_each(
													b.begin(), 
													b.end(), 
													std::bind2nd(bag_inserter<C>(), this)
												  );								}
	
	template <typename C>
	void			insert (const std::list<C>& l)		
												{ std::for_each(
													l.begin(), 
													l.end(), 
													std::bind2nd(list_inserter<C>(), this)
												  );								}

	template <typename C>
	void			remove (const ubag<C>& b)	{ std::for_each(
													b.begin(), 
													b.end(), 
													std::bind2nd(bag_remover<C>(), this)
												  );								}
	
	template <typename C>
	void			remove (const std::list<C>& l)		
												{ std::for_each(
													l.begin(), 
													l.end(), 
													std::bind2nd(list_remover<C>(), this)
												  );								}
	void			remove (const T& x)			{ bag.erase(x);						}
	void            remove (const iterator& i)	{ bag.erase(i);                     }
	void            remove (const_iterator& i) 	{ bag.erase(i);                     }
	bool			empty (void) const			{ return bag.empty();				}
	util_ui32		size (void) const			{ return (util_ui32) bag.size();	}
	iterator		find (const T& x)			{ return bag.find(x); 				}
	const_iterator	find (const T& x)	const	{ return bag.find(x); 				}
	bool			in (const T& x) const		{ return bag.find(x) != bag.end();	}
	void			clear(void) 				{ bag.clear();						}

	UOVERLOADED_ASSIGN_VIA_COPY_CONSTRUCTOR(ubag)
	bool operator==(const ubag& b) const
		{ return this == &b || (bag.size() == b.bag.size() && bag == b.bag); }

	ubag (void){}
	ubag (const T& v1) { insert(v1); }
	ubag (const T& v1, const T& v2) { insert(v1); insert(v2); }
	ubag (const T& v1, const T& v2, const T& v3) { insert(v1); insert(v2); insert(v3); }
	ubag (const ubag& b) : bag(b.bag){}
	~ubag(){}
};

typedef ubag<std::string> ustrbag;

//---------------------------------------------------------------

#define	UDISABLE_ASSIGN_OPERATOR_AND_COPY_CONSTRUCTOR_PRIVATE(_class)				\
	private:																		\
	struct uassign_operator_disabled_exception	{};									\
	struct ucopy_constructor_disabled_excpetion	{};									\
	void operator=(const _class&)													\
		{ DASSERT(false); throw uassign_operator_disabled_exception(); }			\
	_class (const _class&)															\
		{ DASSERT(false); throw ucopy_constructor_disabled_excpetion(); }

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.

