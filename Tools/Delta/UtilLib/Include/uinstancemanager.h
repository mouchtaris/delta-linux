// uinstancemanager.h
// This is a template class for managers of dynamic instances
// enabling to store separately created instances via role-based 
// string keys.
// A. Savidis, July 2011.
//

#ifndef	UINSTANCEMANAGER_H
#define	UINSTANCEMANAGER_H

#include "uderivablecheck.h"
#include "uerrorclass.h"
#include "ufunctors.h"
#include "urecyclefactory.h"
#include <functional>
#include <map>

///////////////////////////////////////////////////////////

template <class TObject, class TParms> struct udynamicconstructor {
	virtual TObject* operator()(const TParms&) const = 0;
};

template <typename T> 
struct uvoiddynamicconstructor : public udynamicconstructor<T, uvoidconstructorarguments> {
	T*	operator()(const uvoidconstructorarguments& unused) const 
		{ return DNEW(T); }

};

template <typename T, typename Ta> 
struct udynamicconstructor1 : public udynamicconstructor<T, uconstructorarguments1<Ta> > {
	T*	operator()(const uconstructorarguments1<Ta>& args) const 
		{ return DNEWCLASS(T, (args.arg1)); }
};

template <typename T, typename T1, typename T2> 
struct udynamicconstructor2 : public udynamicconstructor<T, uconstructorarguments2<T1,T2> > {
	T*	operator()(const uconstructorarguments2<T1,T2>& args) const 
		{ return DNEWCLASS(T, (args.arg1, args.arg2)); }
};

template <typename T, typename T1, typename T2, typename T3> 
struct udynamicconstructor3 : public udynamicconstructor<T, uconstructorarguments3<T1,T2,T3> > {
	T*	operator()(const uconstructorarguments3<T1,T2,T3>& args) const 
		{ return DNEWCLASS(T, (args.arg1, args.arg2, args.arg3)); }
};

template <typename T, typename T1, typename T2, typename T3, typename T4> 
struct udynamicconstructor4 : public udynamicconstructor<T, uconstructorarguments4<T1,T2,T3,T4> > {
	T*	operator()(const uconstructorarguments4<T1,T2,T3,T4>& args) const 
		{ return DNEWCLASS(T, (args.arg1, args.arg2, args.arg3, args.arg4)); }
};

template <typename T, typename T1, typename T2, typename T3, typename T4, typename T5> 
struct udynamicconstructor5 : public udynamicconstructor<T, uconstructorarguments5<T1,T2,T3,T4,T5> > {
	T*	operator()(const uconstructorarguments5<T1,T2,T3,T4,T5>& args) const 
		{ return DNEWCLASS(T, (args.arg1, args.arg2, args.arg3, args.arg4, args.arg5)); }
};

template <typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> 
struct udynamicconstructor6 : public udynamicconstructor<T, uconstructorarguments6<T1,T2,T3,T4,T5,T6> > {
	T*	operator()(const uconstructorarguments6<T1,T2,T3,T4,T5,T6>& args) const 
		{ return DNEWCLASS(T, (args.arg1, args.arg2, args.arg3, args.arg4, args.arg5, args.arg6)); }
};

///////////////////////////////////////////////////////////

template <
		typename	TObject,		
		typename	TParms,		
		typename	TConstructor,
		typename	Tkey
		> class uinstancemanager {

	DFRIENDDESTRUCTOR()

	////////////////////////////////////////////////////////////////
	// We prefer to define a struct of the required size, than allocating
	// an array, since the latter is a little slower.
	//
	public:
	typedef TObject			object_type;
	typedef TParms			params_type;
	typedef TConstructor	constructor_type;
	typedef Tkey			key_type;
	
	//******************************

	protected:
	typedef udynamicconstructor<TObject, TParms> _Base;
	typedef TConstructor _Derived;

	UDERIVCHECKSTART()
	UDERIVCHECK(_Base, _Derived)
	UDERIVCHECKEND()

	typedef std::map<key_type, object_type*>	Instances;
	typedef std::map<std::string, object_type*>	Roles;
	typedef std::map<std::string, key_type>		RoleKeys;

	Instances			insts;
	Roles				roles;
	RoleKeys			roleKeys;
	constructor_type	ctor;

	//******************************

	public:
	bool				In (const object_type* inst) const {
							typename Instances::const_iterator i = std::find_if(
															insts.begin(),
															insts.end(),
															std::bind2nd(uequal_second<std::pair<key_type, const object_type*> >(), inst)
														);
							return i != insts.end();
						}
	
	object_type*		New (const key_type& key, object_type* elem) {
							DASSERT(!Get(key));
							insts[key] = elem; 
							return elem;
						}

	object_type*		New (const key_type& key, const params_type& ctorParms) {
							DASSERT(!Get(key));
							return insts[key] = ctor(ctorParms); 
						}

	void				Delete (const key_type& key) {
							typename Instances::iterator i = insts.find(key);
							DASSERT(i != insts.end());
							DDELETE(i->second);
							insts.erase(i);
						}
	object_type*		Get (const key_type& key) const {
							typename Instances::const_iterator i = insts.find(key);
							return i != insts.end() ? i->second : (object_type*) 0;
						}

	object_type*		SetRole (const std::string& role, const key_type& key) {
							object_type* inst = Get(key);
							DASSERT(inst);
							roleKeys[role] = key;
							return roles[role] = inst;
						}
	void				ResetRole (const std::string& role) 
							{ roles.erase(role); roleKeys.erase(role); }
	object_type*		GetRole (const std::string& role) const {
							typename Roles::const_iterator i = roles.find(role);
							return i != roles.end() ? i->second : (object_type*) 0;
						}
	const key_type&		GetRoleKey (const std::string& role) const {
							typename RoleKeys::const_iterator i = roleKeys.find(role);
							DASSERT(i != roleKeys.end());
							return  i->second;
						}

	void				Initialise (void){}
	void				CleanUp (void) {
							std::for_each(
								insts.begin(),
								insts.end(),
								udestroy_second<std::pair<key_type, object_type*> >()
							);
							insts.clear();
							roles.clear();
						}

	uinstancemanager(void){}
	virtual ~uinstancemanager(){}
};

///////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

