// urecyclefactory.h
// This is a template class for producing factories of
// objects that support recycling. That means, when an object
// is destroyed, it is moved to a recycle bag, while requiring
// a new object will firstly try to get recycled memory and instantiate
// the object via 'new' placement constructor, else with a normal
// construction.
// Advanced object management utilities.
// A. Savidis, December 2002.
// Updates November 2008 (changed list -> ubag).
//

#ifndef	URECYCLEFACTORY_H
#define	URECYCLEFACTORY_H

#include <string.h>
#include <map>
#include <functional>
#include <algorithm>

#include "DDebug.h"
#include "utypes.h"
#include "uderivablecheck.h"
#include "uerrorclass.h"
#include "ufunctors.h"

//-------------------------------------------------------------------
// TObject:	The original recyclable object class.
// TParms:	A class carrying all constructor arguments.
// TConstructor: Should be derived from urecycleconstructor, being a functor
// class that returns pointers to created objects. The derivation check is done
// with a special template function whose declaration will compile normally
// when the constructor block is legal (i.e. the template classes are related
// via derivation).
//

template <class TObject, class TParms> class urecycleconstructor {
	public:
	virtual TObject*	operator()(void* mem, const TParms&) const = 0;
	virtual void		Destroy (TObject* obj) const = 0;
};

//////////////////////////////////////////////////////////////////////

struct uvoidconstructorarguments {
	uvoidconstructorarguments (void){}
	uvoidconstructorarguments (const uvoidconstructorarguments&){}
};

template <typename T1> struct uconstructorarguments1 {
	T1& arg1;
	uconstructorarguments1 (T1& _arg1): 
		arg1(_arg1){}
	uconstructorarguments1 (const uconstructorarguments1& p) : 
		arg1(p.arg1){}
};

template <typename T1, typename T2> struct uconstructorarguments2 {
	T1& arg1; T2& arg2;
	uconstructorarguments2 (T1& _arg1, T2& _arg2): 
		arg1(_arg1), arg2(_arg2){}
	uconstructorarguments2 (const uconstructorarguments2& p) : 
		arg1(p.arg1), arg2(p.arg2){}
};

template <typename T1, typename T2, typename T3> struct uconstructorarguments3 {
	T1& arg1; T2& arg2; T3& arg3;
	uconstructorarguments3 (T1& _arg1, T2& _arg2, T3& _arg3): 
		arg1(_arg1), arg2(_arg2), arg3(_arg3){}
	uconstructorarguments3 (const uconstructorarguments3& p) : 
		arg1(p.arg1), arg2(p.arg2), arg3(p.arg3){}
};

template <typename T1, typename T2, typename T3, typename T4> struct uconstructorarguments4 {
	T1& arg1; T2& arg2; T3& arg3; T4& arg4;
	uconstructorarguments4 (T1& _arg1, T2& _arg2, T3& _arg3, T4& _arg4): 
		arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4){}
	uconstructorarguments4 (const uconstructorarguments4& p) : 
		arg1(p.arg1), arg2(p.arg2), arg3(p.arg3), arg4(p.arg4){}
};

template <typename T1, typename T2, typename T3, typename T4, typename T5> struct uconstructorarguments5 {
	T1& arg1; T2& arg2; T3& arg3; T4& arg4; T5& arg5;
	uconstructorarguments5 (T1& _arg1, T2& _arg2, T3& _arg3, T4& _arg4, T5& _arg5): 
		arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5){}
	uconstructorarguments5 (const uconstructorarguments5& p) : 
		arg1(p.arg1), arg2(p.arg2), arg3(p.arg3), arg4(p.arg4), arg5(p.arg5){}
};

//////////////////////////////////////////////////////////////////////

template <typename T> 
struct uvoidplacementconstructor : public urecycleconstructor<T, uvoidconstructorarguments> {
	T*	operator()(void* mem, const uvoidconstructorarguments& unused) const 
		{ return new (mem) T; }
	void Destroy (T* obj) const { obj->~T(); }

};

template <typename T, typename Ta> 
struct uplacementconstructor1 : public urecycleconstructor<T, uconstructorarguments1<Ta> > {
	T*	operator()(void* mem, const uconstructorarguments1<Ta>& args) const 
		{ return new (mem) T(args.arg1); }
	void Destroy (T* obj) const { obj->~T(); }
};

template <typename T, typename T1, typename T2> 
struct uplacementconstructor2 : public urecycleconstructor<T, uconstructorarguments2<T1,T2> > {
	T*	operator()(void* mem, const uconstructorarguments2<T1,T2>& args) const 
		{ return new (mem) T(args.arg1, args.arg2); }
	void Destroy (T* obj) const { obj->~T(); }
};

template <typename T, typename T1, typename T2, typename T3> 
struct uplacementconstructor3 : public urecycleconstructor<T, uconstructorarguments3<T1,T2,T3> > {
	T*	operator()(void* mem, const uconstructorarguments3<T1,T2,T3>& args) const 
		{ return new (mem) T(args.arg1, args.arg2, args.arg3); }
	void Destroy (T* obj) const { obj->~T(); }
};

template <typename T, typename T1, typename T2, typename T3, typename T4> 
struct uplacementconstructor4 : public urecycleconstructor<T, uconstructorarguments4<T1,T2,T3,T4> > {
	T*	operator()(void* mem, const uconstructorarguments4<T1,T2,T3,T4>& args) const 
		{ return new (mem) T(args.arg1, args.arg2, args.arg3, args.arg4); }
	void Destroy (T* obj) const { obj->~T(); }
};

template <typename T, typename T1, typename T2, typename T3, typename T4, typename T5> 
struct uplacementconstructor5 : public urecycleconstructor<T, uconstructorarguments5<T1,T2,T3,T4,T5> > {
	T*	operator()(void* mem, const uconstructorarguments5<T1,T2,T3,T4,T5>& args) const 
		{ return new (mem) T(args.arg1, args.arg2, args.arg3, args.arg4, args.arg5); }
	void Destroy (T* obj) const { obj->~T(); }
};

//////////////////////////////////////////////////////////////////////

template <
		typename	TObject,		
		typename	TParms,		
		typename	TConstructor
		> class urecyclefactory {

	DFRIENDDESTRUCTOR()

	////////////////////////////////////////////////////////////////
	// We prefer to define a struct of the required size, than allocating
	// an array, since the latter is a little slower.
	//
	struct MemObject { util_ui8 mem[sizeof(TObject)]; };

	public:
	typedef TObject			object_type;
	typedef TParms			params_type;
	typedef TConstructor	constructor_type;
	
	typedef urecycleconstructor<TObject, TParms> _Base;
	typedef TConstructor _Derived;

	UDERIVCHECKSTART()
	UDERIVCHECK(_Base, _Derived)
	UDERIVCHECKEND()

	static void* GetMem (void) {
		void* mem = DNEW(MemObject);
		DASSERT(mem);
		return mem;
	}

	static void DeleteMem (TObject* mem) 
		{ DDELETE((MemObject*) mem); }

	static void WipeOut (TObject* obj)
		{ memset((void*) obj, 0, sizeof(TObject)); }

	////////////////////////////////////////////////////////////////

	private:

	typedef ubag<TObject*> Bag;
	Bag				usedObjects;
	Bag				unUsedObjects;
	TConstructor	constructor;
	TObject*		callingItsDestructor;
	bool			isUnderDestruction;

	//******************************
	
	void DestructorWrapper (TObject* obj) {
		callingItsDestructor = obj;
		constructor.Destroy(obj);
		unullify(callingItsDestructor);	
	}

	protected:
	urecyclefactory(void) : callingItsDestructor((TObject*) 0), isUnderDestruction(false){}
	virtual ~urecyclefactory() { isUnderDestruction = true; CleanUp(); }
	
	////////////////////////////////////////////////////////////////

	public:
	bool		IsUnderDestruction (void) const
					{ return isUnderDestruction; }
	TObject*	Get (const TParms& constructorParms) {

					TObject* obj;
					if (unUsedObjects.empty())
						obj = (TObject*) GetMem();
					else {
						obj = unUsedObjects.begin()->first;
						unUsedObjects.remove(unUsedObjects.begin());
					}

					usedObjects.insert(obj);
					try { 
						obj = constructor((void*) obj, constructorParms); 
					} 
					catch (uerrorclass::udisengageexception) {
						usedObjects.remove(obj);
						unUsedObjects.insert(obj);
						WipeOut(obj);
						return (TObject*) 0;
					}

					return obj;
				}

	//******************************

	bool		HasProduced (TObject* obj) const 
					{ return usedObjects.find(obj) != usedObjects.end(); }

	bool		HasRecycled (TObject* obj) const 
					{ return unUsedObjects.find(obj) != unUsedObjects.end(); }

	//******************************

	void		UnUse (TObject* obj) {

					typename Bag::iterator i = usedObjects.find(obj);
					DASSERT(i != usedObjects.end());
					
					usedObjects.remove(i);
					unUsedObjects.insert(obj);

					DestructorWrapper(obj);
					WipeOut(obj);
				}

	//******************************

	bool		JustCallingDestructor (const TObject* obj) const
					{ return obj && callingItsDestructor == obj; }

	util_ui16	GetTotalUsed (void) const
					{ return (util_ui16) usedObjects.size(); }

	//******************************

	const Bag	GetUsedObjects (void) const 
					{ return usedObjects; }

	void		UnUseAll (void) {

					Bag bag(usedObjects);
					std::for_each(
						bag.begin(), bag.end(),
						utuple_firstfunctoriser< std::pair<TObject*, bool> >(
							std::bind1st(std::mem_fun(&urecyclefactory::UnUse), this)
						)
					);
				}

	void		CleanUp (void) {

					std::for_each(
						usedObjects.begin(), usedObjects.end(),
						utuple_firstfunctoriser< std::pair<TObject*, bool> >(
							std::bind1st(std::mem_fun(&urecyclefactory::DestructorWrapper), this)
						)
					);
					unUsedObjects.insert(usedObjects);
					usedObjects.clear();
					
					std::for_each(
						unUsedObjects.begin(), unUsedObjects.end(),
						utuple_firstfunctoriser< std::pair<TObject*, bool> >(&urecyclefactory::DeleteMem)
					);
					unUsedObjects.clear();
				}
};

//-------------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
