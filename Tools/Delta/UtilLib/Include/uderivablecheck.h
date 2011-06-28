// uderivablecheck.h
// Method for compile time checking of deriavbility among
// class, especially useful for template classes.
// ScriptFighter Project.
// A. Savidis, December 2002.
//

#ifndef	UDERIVABLECHECK_H
#define	UDERIVABLECHECK_H

//---------------------------------------------------------------
// Template to check if conversion exists at compile time.
//
template <class T1, class T2> class uconversioncheck {

	private:
	typedef char usmall;
	struct  ubig { usmall dummy[2]; };

	static usmall	f (T2)
						{ return (usmall) 0; }
	static ubig		f (...)  
						{ return ubig(); }
	static T1*		g (void) 
						{ return (T1*) 0; }

	public:
	enum { exists = sizeof(f(*g())) == sizeof(usmall) };
};

//////////////////////////////////////////////////////////////////
// Template to check if a const void* pointer (or void*)
// corresponds to a supplied template type.
//
template <class T> struct uisconstvoidpointer {
	enum { result = 0 };
};

template<> struct uisconstvoidpointer<const void*> {
	enum { result = 1 };
};

//////////////////////////////////////////////////////////////////
// Template to cause a compile error when passed
// with false as a template parameter.
//
template <bool> class ucompilesafe;
template<> class ucompilesafe<true>{};

//////////////////////////////////////////////////////////////////
// Macro to cause a compile error if the constant boolean
// expression supplied is false. It is to be used ONLY in
// local scope.
//
#define	UCOMPILECHECK(expr) \
	{ ucompilesafe<expr>(); }

//////////////////////////////////////////////////////////////////
// Macro to ensure a type is a previously defined class.
// Must be used ONLY in local scope.
//
#define	UMUSTBECLASS(T) \
	{ class _Temp : public T {}; }

//////////////////////////////////////////////////////////////////
// Macro to ensure derivation. The two types must be classes,
// and there must be a compiler visible conversion. Also,
// the base type should not be const void*.
//
#define	UBASEDERIVED(Tbase, Tderived)										\
	UMUSTBECLASS(Tbase)														\
	UMUSTBECLASS(Tderived)													\
	UCOMPILECHECK(															\
		(	(uconversioncheck<const Tderived*, const Tbase*>::exists) &&	\
			(!uisconstvoidpointer<const Tbase*>::result)					\
		)																	\
	)

//---------------------------------------------------------------
// This declaration ensures that a class is derived from another
// by performing compile time check. You can use the macros at any
// scope, however, only once (in its respective scope).
//
// UDERIVCHECKSTART()
// UDERIVCHECK(B1,D1)
// ...
// UDERIVCHECK(Bn,Dn)
// UDERIVCHECKEND()
//
#define UDERIVCHECKSTART()				struct uderivationcheck { uderivationcheck (void) {
#define UDERIVCHECK(_base, _derived)	{ UBASEDERIVED(_base, _derived); }
#define UDERIVCHECKEND()				}}; uderivationcheck _uderivationcheck;

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
