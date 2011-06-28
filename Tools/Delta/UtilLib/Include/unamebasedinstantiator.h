// unamebasedinstantiator.h
// Template class for factory wrappers to produce instances
// of classes through names, and to destroy created instances
// using class-name information via traits.
// ScriptFighter Project.
// A. Savidis, May 2008.
//

#ifndef	UNAMEBASEDINSTANTIATOR_H
#define	UNAMEBASEDINSTANTIATOR_H

//---------------------------------------------------------------

#include "DDebug.h"
#include <map>
#include <algorithm>

template <class T> struct uclassnametraits {
	static const char* ClassId (void) 
		{ DASSERT(false); return (char*) 0; }
};

#define	UINSTANTIATORNAMETRAITS(_class, _name) \
template<> struct uclassnametraits<_class> \
	{  static const char* ClassId (void) { return _name; }  };

//////////////////////////////////////////////////////////////////

template <class T> class unamebasedinstantiator {

	public:
	typedef T*		(*Ctor) (void* args);
	typedef bool	(*Dtor) (T* inst);
	
	private:
	typedef std::map<std::string, Ctor> Ctors;
	Ctors constructors;
	typedef std::map<std::string, Dtor> Dtors;
	Dtors destructors;

	public:
	bool	Add (const char* classId, Ctor ctor, Dtor dtor) {
				if (constructors.find(classId)!= constructors.end())
					return false;
				else {
					DASSERT(destructors.find(classId) == destructors.end());
					constructors[classId] = ctor;
					destructors[classId] = dtor;
					return true;
				}
			}

	bool	Remove (const char* classId) {
				Ctors::iterator i = constructors.find(classId);
				DASSERT(i != constructors.end());
				Dtors::iterator j = destructors.find(classId);
				DASSERT(j != destructors.end());
				constructors.erase(i);
				destructors.erase(j);
				return true;
			}

	T*		New (const char* classId, void* args = (void*) 0) {
				Ctors::iterator i = constructors.find(classId);
				DASSERT(i != constructors.end());
				return (*i->second)(args);
			}

	bool	HasClass (const char* classId ) const 
				{ return constructors.find(classId) != constructors.end(); }

	template <class C>
	bool	Delete (C* inst) {
				Dtors::iterator i = destructors.find(uclassnametraits<C>::ClassId());
				DASSERT(i != destructors.end());
				return (*i->second)(inst); // Should auto up-cast.
			}

	bool	Delete (T* inst, const char* classId) {
				Dtors::iterator i = destructors.find(classId);
				DASSERT(i != destructors.end());
				return (*i->second)(inst); // Should auto up-cast.
			}

	unamebasedinstantiator(void){}
	~unamebasedinstantiator(){}
};

//////////////////////////////////////////////////////////////////

template <class T> 
class unamebasedinstantiatorsingleton : public usingleton< unamebasedinstantiator<T> > {
	public:
	typedef typename unamebasedinstantiator<T>::Ctor Ctor;
	typedef typename unamebasedinstantiator<T>::Dtor Dtor;

	static bool	Add (const char* classId, Ctor ctor, Dtor dtor)
		{ return GetSingleton().Add(classId, ctor, dtor); }

	static bool	Remove (const char* classId)
			{ return GetSingleton().Remove(classId); }				

	static T* New (const char* classId, void* args = (void*) 0) 
			{ return GetSingleton().New(classId, args); }

	static bool HasClass (const char* classId)
			{ return GetSingleton().HasClass(classId); }

	template <class C>
	static bool	Delete (C* inst) 
		{ return GetSingleton().Delete(inst); }

	static bool	Delete (T* inst, const char* classId)
		{ return GetSingleton().Delete(inst, classId); }
};

//////////////////////////////////////////////////////////////////
// _Superclass is the super-class of instantiated objects.
// _Name is the identifier of the instantiator singleton.
// You may prefix the name with an export / import qualifier if needed.
// The Initialise() and CleanUp() are user-defined methods. 
// Usually, ctors / dtors are explicitly added here, while implemented 
// as needed by the application.
//
#define	UNAMEBASED_INSTANTIATOR_DEF(_Superclass, _Name)						\
	class _Name : public unamebasedinstantiatorsingleton<_Superclass> {		\
		public:																\
		static void Initialise (void);										\
		static void CleanUp (void);											\
	};

#define	UNAMEBASED_INSTANTIATOR_IMPL(_Superclass) \
	USINGLETON_INSTANCE(usingleton< unamebasedinstantiator<_Superclass> >)

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.

