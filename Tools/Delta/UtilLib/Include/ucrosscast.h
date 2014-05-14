// ucrosscast.h
// Cross casting method for multiple inheritance and virtual
// base classes.
// Moved from the central delta util here.
// ScriptFighter Project.
// A. Savidis, July 2002.
//

#ifndef	UCROSSCAST_H
#define	UCROSSCAST_H

#include <string>
#include "utypes.h"

//---------------------------------------------------------------
// This is the safe method for down / cross casting. The key part of the method 
// is the bottom instance pure virtual function, that is to be implemented by every
// derived class.
//
class UTILLIB_CLASS CrossCastable {
	public:
	virtual void* BottomInstance (void) = 0;

	template <class CMostDerived> CMostDerived* DownCast (void) 
		{ return (CMostDerived*) BottomInstance(); }
	
	template <class CTargetBase, class CMostDerived> CTargetBase* CrossCast (void) 
		{ return (CTargetBase*) DownCast<CMostDerived>(); }
	virtual ~CrossCastable(){}
};

#define	CROSS_CASTABLE_WITH_DELETE_SUBCLASS_METHOD_IMPL		\
	virtual void*	BottomInstance (void) { return this; }	\
	virtual void	Delete (void) { DDELETE(this); }

class UTILLIB_CLASS CrossCastableWithDelete : public  CrossCastable {
	public:
	virtual void	Delete (void) = 0;
};

//---------------------------------------------------------------
// Safe dynamic casting (extra to C++) using class names. Can use
// any typed subobject pointer in a polymorphic object. It will not work
// well for repeated inheritance (it will return the first matching
// instance of a repeating class in a multiple inheritance hierarchy).
// It does not need the most derived class for cross casting as before.
//
class UTILLIB_CLASS DynamicCastable {
	public:
	virtual void* CastTo (const std::string& classId) = 0;
	virtual ~DynamicCastable(){}
};

template <typename C, typename T>
C*	DYNAMIC_CAST (T* p, const std::string classId)
		{ return (C*) p->CastTo(classId); }

#define	DYNAMIC_CAST_NOBASE_INLINE_IMPL_EX(_mycid, action)						\
	virtual void* CastTo (const std::string& classId) {							\
		if (classId == _mycid) { { action; } return this; }						\
		return (void*) 0;														\
	}

#define	DYNAMIC_CAST_NOBASE_INLINE_IMPL(_mycid)									\
		DYNAMIC_CAST_NOBASE_INLINE_IMPL_EX(_mycid, UEMPTY)

#define	DYNAMIC_CAST_DUAL_NOBASE_INLINE_IMPL_EX(_mycid, _derivedcid, action)	\
	virtual void* CastTo (const std::string& classId) {							\
		 if (classId == _mycid || classId == _derivedcid)						\
			{ { action; } return this; }										\
		return (void*) 0;														\
	}

#define	DYNAMIC_CAST_DUAL_NOBASE_INLINE_IMPL(_mycid, _derivedcid)				\
		DYNAMIC_CAST_DUAL_NOBASE_INLINE_IMPL_EX(_mycid, _derivedcid, UEMPTY)

#define	DYNAMIC_CAST_SINGLEBASE_INLINE_IMPL_EX(_base, _mycid, action)			\
	virtual void* CastTo (const std::string& classId) {							\
	if (classId == _mycid)	{ { action; } return this; }						\
		return _base::CastTo(classId);											\
	}

#define	DYNAMIC_CAST_SINGLEBASE_INLINE_IMPL(_base, _mycid)						\
		DYNAMIC_CAST_SINGLEBASE_INLINE_IMPL_EX(_base, _mycid, UEMPTY)

#define	DYNAMIC_CAST_DUALBASE_INLINE_IMPL_EX(_b1, _b2, _mycid, action)			\
	virtual void* CastTo (const std::string& classId) {							\
		if (classId == _mycid)	{ { action; } return this; }					\
		void* p;																\
		if (p = _b1::CastTo(classId)) return p;									\
		if (p = _b2::CastTo(classId)) return p;									\
		return (void*) 0;														\
	}

#define	DYNAMIC_CAST_DUALBASE_INLINE_IMPL(_b1, _b2, _mycid)						\
		DYNAMIC_CAST_DUALBASE_INLINE_IMPL_EX(_b1, _b2, _mycid, UEMPTY)

#define	DYNAMIC_CAST_TRIPLEBASE_INLINE_IMPL_EX(_b1, _b2, _b3, _mycid, action)	\
	virtual void* CastTo (const std::string& classId) {							\
		if (classId == _mycid)	{ { action; } return this; }					\
		void* p;																\
		if (p = _b1::CastTo(classId)) return p;									\
		if (p = _b2::CastTo(classId)) return p;									\
		if (p = _b3::CastTo(classId)) return p;									\
		return (void*) 0;														\
	}

#define	DYNAMIC_CAST_TRIPLEBASE_INLINE_IMPL(_b1, _b2, _b3, _mycid)				\
		DYNAMIC_CAST_TRIPLEBASE_INLINE_IMPL_EX(_b1, _b2, _b3, _mycid, UEMPTY)

/////////////////////////////////////////////////////////////////

#if 0 // Example
struct A : public DynamicCastable {
	virtual void* CastTo (const std::string& classId) 
		{ return classId == "A" ? this : (void*) 0; }
};

struct B : public DynamicCastable {
	virtual void* CastTo (const std::string& classId)
		{ return classId == "A" ? this : (void*) 0; }
};

struct C : public A, public B {
	virtual void* CastTo (const std::string& classId) {
		if (classId == "C")	return this;
		void* p;
		if (p = A::CastTo(classId)) return p;
		if (p = B::CastTo(classId)) return p;
		return (void*) 0;
	}
};
#endif // Example

//---------------------------------------------------------------
// This is the method with which we can test if a call for an object
// is made while its construction is taking place. All that is needed is
// to inherit virtually from this class, and then put at the beginning
// of each constructor (for every derived class), the StartConstruction()
// or the macro CONSTRUCTION_AWARE.
//
class ConstructionAware {

	private:
	bool	underConstruction;

	public:
	void	StartConstruction (void)
				{ underConstruction = true; }
	void	FinishConstruction (void)
				{ underConstruction = false; }
	bool	UnderConstruction (void) const
				{ return underConstruction; }

	ConstructionAware (void) { underConstruction = false; }
	~ConstructionAware(){}
};

//***********************************

class ConstructionMonitor {
	private:
	ConstructionAware* caller;

	public:
	ConstructionMonitor (ConstructionAware* _caller) : caller(_caller)
		{ caller->StartConstruction(); }
	~ConstructionMonitor ()
		{ caller->FinishConstruction(); }	
};

//***********************************

#define	CONSTRUCTION_AWARE(_C) \
	ConstructionMonitor _constructionMonitor##_C((ConstructionAware*) this);

//////////////////////////////////////////////////////////////////
// The same facility is available for teh destruction sequence.
//
class DestructionAware {

	private:
	bool	underDestruction;

	public:
	void	StartDestruction (void)
				{ underDestruction = true; }
	void	FinishDestruction (void)
				{ underDestruction = false; }
	bool	UnderDestruction (void) const
				{ return underDestruction; }

	DestructionAware (void) { underDestruction = false; }
	~DestructionAware (){}
};

//***********************************

class DestructionMonitor {
	private:
	DestructionAware* caller;

	public:
	DestructionMonitor (DestructionAware* _caller) : caller(_caller)
		{ caller->StartDestruction(); }
	~DestructionMonitor ()
		{ caller->FinishDestruction(); }	
};

//***********************************

#define	DESTRUCTION_AWARE(_C) \
	DestructionMonitor _destructionMonitor##_C((DestructionAware*) this);

#endif	// Do not add stuff beyond this point.
