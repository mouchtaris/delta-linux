// usingleton.h
// A mixin class for singletons (turning a class to singleton).
// ScriptFighter Project.
// A. Savidis, January 2007 (extracting here).
//

#ifndef	USINGLETON_H
#define	USINGLETON_H

#include "DDebug.h"
#include "uptr.h"
#include "utypes.h"

//---------------------------------------------------------------

#define	USINGLETON_APISTYLE_DECLARE_PRIVATESTDMETHODS								\
	static bool	IsSingletonCreated(void);

#define	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS								\
	static void SingletonCreate (void);												\
	static void SingletonDestroy (void);

#define	USINGLETON_APISTYLE_DECLARE_PRIVATEINSTANCE(_class)							\
	friend _class* ::unew_with_context<_class> (_class*&, const char*, util_ui32);	\
	static _class* singletonPtr;

#define	USINGLETON_APISTYLE_DEFINE_PRIVATEINSTANCE(_class)							\
	_class* _class::singletonPtr = (_class*) 0;

#define	USINGLETON_APISTYLE_IMPL_PUBLICSTDMETHODS(_class)							\
	void _class::SingletonCreate (void)												\
		{ unew(singletonPtr); }														\
	void _class::SingletonDestroy (void)											\
		{ udelete(singletonPtr); }													\

#define	USINGLETON_APISTYLE_DECLARE_GETTER(_class)									\
	static _class & GetSingleton (void);											\
	static _class*	GetSingletonPtr (void);											\

#define	USINGLETON_INLINE_ACCESS_HELPER(_class)										\
	inline	_class& _class##Get (void) { return (_class&) _class::GetSingleton(); }

#define	USINGLETON_APISTYLE_IMPL_GETTER(_class)										\
	_class & _class::GetSingleton (void)											\
		{ DASSERT(singletonPtr); return *DPTR(singletonPtr); }						\
	_class*	_class::GetSingletonPtr (void)											\
		{ DASSERT(singletonPtr); return DPTR(singletonPtr); }

template <typename T> class usingleton : public T {

	DFRIENDDESTRUCTOR()

	protected:
	static usingleton* singletonPtr;
	virtual ~usingleton(){}

	public:
#ifdef	_MSC_VER
	static usingleton&	GetSingleton (void);
	static usingleton*	GetSingletonPtr (void);
	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS
#else
	static usingleton&	GetSingleton (void)
							{ DASSERT(singletonPtr); return *DPTR(singletonPtr); }
	static usingleton*	GetSingletonPtr (void)
							{ DASSERT(singletonPtr); return DPTR(singletonPtr); }
	static void SingletonCreate (void)
							{ unew(singletonPtr); }
	static void SingletonDestroy (void)
							{ udelete(singletonPtr); }
#endif
};


#ifdef	_MSC_VER
#define	USINGLETON_INSTANCE(T)									\
	template<> T*	T::singletonPtr = (T*) 0;					\
	template<> T&	T::GetSingleton (void)						\
		{ DASSERT(singletonPtr); return *DPTR(singletonPtr); }	\
	template<> T*	T::GetSingletonPtr (void)					\
		{ DASSERT(singletonPtr); return DPTR(singletonPtr); }	\
	template<> void T::SingletonCreate (void)					\
		{ unew(singletonPtr); }									\
	template<> void T::SingletonDestroy (void)					\
		{ udelete(singletonPtr); }
#else
#define	USINGLETON_INSTANCE(T)									\
	template<> T*	T::singletonPtr = (T*) 0;
#endif

/////////////////////////////////////////////////////////////////
//	Use as follows:
//	.h		class MyClass {...};
//	.h		class MyClassSingleton: public usingleton<MyClass>{};
//	.cpp	USINGLETON_INSTANCE(usingleton<MyClass>)
//---------------------------------------------------------------
// The following cannot become a template since if we need two or more global instances
// of the same class the static object will result in replicate definitions at link time.
//
#define	UGLOBALINSTANCE_CLASS_DEF(_class, T)	\
	class _class {								\
		private:								\
		static T* inst;							\
		public:									\
		static void Create (void);				\
		static void	Destroy (void);				\
		static T*	GetPtr (void);				\
		static T&	Get (void);					\
	};

#define	UGLOBALINSTANCE_CLASS_IMPL_EX(_class, T, _oncreate, _ondestroy)					\
		void	_class::Create (void)		{ unew(inst); _oncreate }					\
		void	_class::Destroy (void)		{ _ondestroy udelete(inst);  }				\
		T*		_class::GetPtr (void)		{ return inst; }							\
		T&		_class::Get (void)			{ DASSERT(inst); return *DPTR(inst); }		\
		T*		_class::inst = (T*) 0;

#define	UGLOBALINSTANCE_CLASS_IMPL(_class, T)											\
		UGLOBALINSTANCE_CLASS_IMPL_EX(_class, T, UEMPTY, UEMPTY)

#define	UGLOBALINSTANCE_INLINE_ACCESS_HELPER(_class)									\
	inline	_class& _class##Get (void) { return (_class&) _class##Instance::Get(); }

/////////////////////////////////////////////////////////////////
//	Use as follows:
//	Let MyClass be the class for which you need a global instance MyClassInst.
//	.h		UGLOBALINSTANCE_CLASS_DEF(MyClassInst, MyClass)
//	.cpp	UGLOBALINSTANCE_CLASS_IMPL(MyClassInst, MyClass)

/////////////////////////////////////////////////////////////////
// A singleton to register constructors of the same produce instance type (normally a superclass).
// Registration and runtime resolution is done with a unique class id.

#define	UCONSTRUCTOR_REGISTRY_SINGLETON_DEF(																		\
	_class,				/* of signleton */																			\
	_inst_class,		/* of produced insts */																		\
	_ctor_args,			/* of the ctor function signature */ 														\
	_new_args,			/* in New() method */																		\
	_args_pass,			/* from New() method args to register ctor function */										\
	_resolve_class_id	/* expression to resolve class id, normally from New() method args */ 						\
	)																												\
class _class {																										\
	DFRIENDDESTRUCTOR()																								\
	public:																											\
	typedef _inst_class*	(*Constructor)(_ctor_args);																\
	private:																										\
	USINGLETON_APISTYLE_DECLARE_PRIVATEINSTANCE(_class)																\
	typedef std::map<std::string, Constructor>	Constructors;														\
	Constructors			ctors;																					\
	_class (void){}																									\
	public:																											\
	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS																	\
	USINGLETON_APISTYLE_DECLARE_GETTER(_class)																		\
	void				Initialise (void)																			\
							{ DASSERT(ctors.empty()); }																\
	void				CleanUp (void)																				\
							{ ctors.clear(); }																		\
	void				Install (Constructor ctor, const std::string& classId) {									\
							DASSERT(ctors.find(classId) == ctors.end());											\
							ctors[classId] = ctor;																	\
						}																							\
	_inst_class*		New (_new_args) const {																		\
							Constructors::const_iterator i = ctors.find(_resolve_class_id);							\
							DASSERT(i != ctors.end());																\
							return DNULLCHECK((*i->second)(_args_pass));											\
						}																							\
};																													\
USINGLETON_INLINE_ACCESS_HELPER(_class)

#define	UCONSTRUCTOR_REGISTRY_SINGLETON_IMPL(_class)																\
USINGLETON_APISTYLE_DEFINE_PRIVATEINSTANCE(_class)																	\
USINGLETON_APISTYLE_IMPL_PUBLICSTDMETHODS(_class)																	\
USINGLETON_APISTYLE_IMPL_GETTER(_class)

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.

