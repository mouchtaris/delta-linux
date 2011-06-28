// usig.h
// Signature extraction templates. Just for the fun of it.
// A. Savidis, May 2007.
//

#ifndef	USIG_H
#define	USIG_H

//---------------------------------------------------------------

#include <string>

// utostring is a trait type -> string
//
template <class T> struct utostring {};

template <> struct utostring<void>
{ static const std::string t2s (void) { return  "void"; } };

template <> struct utostring<std::string> 
{ static const std::string t2s (void) { return  "std::string"; } };

template <> struct utostring<int>
{ static const std::string t2s (void) { return  "int"; } };

template <> struct utostring<bool>
{ static const std::string t2s (void) { return  "bool"; } };

//******************
// sig is a template function -> string signature
// 
template <class T, template <typename C> class TOS, typename Caller> 
struct sig {};

template <class R, class A1, template <typename C> class TOS, typename Caller> 
struct sig<R(*)(A1), TOS, Caller> {
	static const std::string t2s (void) {
		return TOS<R>::t2s() + "(" + TOS<A1>::t2s() + ")";
	}
	R operator()(const std::string& f, A1 a1) {
		return Caller(f).invoke<R, A1>(a1);
	}
};

template <class R, class A1, class A2, template <typename C> class TOS, typename Caller> 
struct sig<R(*)(A1, A2), TOS, Caller> {
	static const std::string t2s (void) {
		return TOS<R>::t2s() + "(" + TOS<A1>::t2s() + "," + TOS<A2>::t2s() + ")";
	}
	R operator()(const std::string& f, A1 a1, A2 a2 ){
		return Caller(f).invoke<R, A1, A2>(a1, a2);
	}
};

//////////////////////////////////////////
//	TEST

/*
struct MyCaller {

	template <typename R, typename A1, typename A2> 
	R invoke(A1, A2)	{ return R(); }

	template <>
	bool invoke<bool, int, std::string> (int, std::string) { return false; }

	template <>
	int invoke<int, bool, int> (bool, int) { return 23; }

	MyCaller (const std::string&){}
};

fprintf(
	stdout, 
		"%s %s.\n", 
		sig<bool(*)(int, std::string), utostring, MyCaller>::t2s().c_str(),
		ubool2conststr(sig<bool(*)(int, std::string), utostring, MyCaller>()("f", 1, ""))
);
fprintf(
	stdout, 
	"%s %d.\n", 
	sig<int(*)(bool, int), utostring, MyCaller>::t2s().c_str(),
	sig<int(*)(bool, int), utostring, MyCaller>()("f", false, 11)
);
*/

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.

