// ucallbacks.h
// Generic callback holders, with or without a closure.
// Moved from the central delta util here.
// ScriptFighter Project.
// A. Savidis, July 2002.
// Refactored July 2007, supporting multiple signatures.
//
#ifndef	UCALLBACKS_H
#define	UCALLBACKS_H

#include "DDebug.h"
#include "utypes.h"
#include "usig.h"

#include <list>
#include <functional>
#include <algorithm>

//---------------------------------------------------------------
// Represents callbacks that are registered together with
// their closure.
//
template <class Tfun> class ucallbackwithclosure {

	private:
	Tfun	func;
	void*	closure;
	typedef typename usig<Tfun>::result_type R;

	public:
	struct equal : public std::binary_function<ucallbackwithclosure, std::pair<Tfun, void*>, bool> {
		bool operator()(const ucallbackwithclosure& cb, const std::pair<Tfun, void*>& f) const
			{ return cb.f() == f.first && (!f.second || cb.c() == f.second); }
	};

	struct invoke : public std::unary_function<ucallbackwithclosure, void> {
		void operator()(const ucallbackwithclosure& f) const
			{ f(); }
	};

	bool				operator==(const ucallbackwithclosure& cc) const
							{ return f() == cc.f() && (!c() || !cc.c() || c() == cc.c()); }
	operator			bool() const { return !!func; }
	Tfun				f (void) const { return func; }
						
	void*				c (void) const { return closure; }
						
	void				set (Tfun f, void* c = (void*) 0) 
							{ func = f; closure = c; }
	void				reset (void)
							{ func = (Tfun) 0; closure = (void*) 0; }

	R					operator() (void) const 
							{ return (*func)(closure); }
	template <class C1> 
	R					operator()(C1 a1) const  
							{ return (*func)(a1, closure); }
	template <class C1, class C2> 
	R					operator()(C1 a1, C2 a2) const  
							{ return (*func)(a1, a2, closure); }
	template <class C1, class C2, class C3> 
	R					operator()(C1 a1, C2 a2, C3 a3) const  
							{ return (*func)(a1, a2, a3, closure); }
	template <class C1, class C2, class C3, class C4> 
	R					operator()(C1 a1, C2 a2, C3 a3, C4 a4) const  
							{ return (*func)(a1, a2, a3, a4, closure); }
	template <class C1, class C2, class C3, class C4, class C5> 
	R					operator()(C1 a1, C2 a2, C3 a3, C4 a4, C5 a5) const  
							{ return (*func)(a1, a2, a3, a4, a5, closure); }
	template <class C1, class C2, class C3, class C4, class C5, class C6> 
	R					operator()(C1 a1, C2 a2, C3 a3, C4 a4, C5 a5, C6 a6) const  
							{ return (*func)(a1, a2, a3, a4, a5, a6, closure); }

	UOVERLOADED_ASSIGN_VIA_COPY_CONSTRUCTOR(ucallbackwithclosure)

	ucallbackwithclosure (const ucallbackwithclosure& data) : 
		func(data.func), 
		closure(data.closure) {}

	ucallbackwithclosure (Tfun _func, void* _closure = (void*) 0) : 
		func(_func), 
		closure(_closure) {}

	ucallbackwithclosure (void) : 
		func((Tfun) 0), 
		closure((void*) 0) {}

	~ucallbackwithclosure(){}
};

/////////////////////////////////////////////////////////////////
// Represents callbacks that are registered alone with 
// only the function address.
//
template <class Tfun> class ucallbackalone {
	private:
	Tfun func;
	typedef typename usig<Tfun>::result_type R;

	public:
	struct equal : public std::binary_function<ucallbackalone, std::pair<Tfun, void*>, bool> {
		bool operator()(const ucallbackalone& cb, const std::pair<Tfun, void*>& f) const
			{ return cb.f() == f.first; }
	};

	Tfun f(void) const { return func; }
	R operator()(void) const  { return (*func)(); }

	ucallbackalone (const ucallbackalone& data) : func(data.func) {}
	ucallbackalone (Tfun _func) : func(_func) {}
	ucallbackalone (void) : func((Tfun) 0) {}
	~ucallbackalone(){}
};

/////////////////////////////////////////////////////////////////
// Utility callback creators
//
template <class Tfun> const ucallbackwithclosure<Tfun> umakecallback(Tfun func, void* closure)
	{ return ucallbackwithclosure<Tfun>(func, closure); }

/////////////////////////////////////////////////////////////////
// This template class provides a generic callback registration
// and management service. It works for any type of function signature, and for
// any type of closure parameters. The class D represents the data class
// that will hold the function pointer together with any closure data. Two
// such predefined classes are supplied, one taking a single void* closure,
// and another taking no such closure.
//
template <class F, class D = ucallbackwithclosure<F> > class ucallbacklist {

	private:
	std::list<D>	callbacks;
	ubag<D*>		ptrs;

	typename std::list<D>::iterator find (F f, void* c = (void*) 0) {
		return	std::find_if(
					callbacks.begin(),
					callbacks.end(),
					std::bind2nd(
						typename D::equal(),
						std::pair<F, void*>(f, c)
					)
				);
	}

	struct caller0 { void operator()(const D& f) { f(); } };

	template <class C1> struct caller1 { 
		C1& a1;
		void operator()(const D& f) { f(a1); } 
		caller1 (C1& _a1) : a1(_a1){}
		caller1 (const caller1& c) : a1(const_cast<caller1&>(c).a1){}
	};

	template <class C1, class C2> struct caller2 : public caller1<C1> { 
		C2& a2;
		void operator()(const D& f) { f(caller1<C1>::a1, a2); } 
		caller2 (C1& _a1, C2& _a2) : caller1<C1>(_a1), a2(_a2){}
		caller2 (const caller2& c) : caller1<C1>(c), a2(const_cast<caller2&>(c).a2){}
	};

	template <class C1, class C2, class C3> struct caller3 : public caller2<C1, C2> { 
		C3& a3;
		void operator()(const D& f) { f(caller1<C1>::a1,  caller2<C1, C2>::a2, a3); } 
		caller3 (C1& _a1, C2& _a2, C3& _a3) : caller2<C1, C2>(_a1, _a2), a3(_a3){}
		caller3 (const caller3& c) : caller2<C1,C2>(c), a3(const_cast<caller3&>(c).a3){}
	};

	template <class C1, class C2, class C3, class C4> struct caller4 : public caller3<C1, C2, C3> { 
		C4& a4;
		void operator()(const D& f) { f(caller1<C1>::a1,  caller2<C1, C2>::a2, caller3<C1, C2, C3>::a3, a4); } 
		caller4 (C1& _a1, C2& _a2, C3& _a3, C4& _a4) : caller3<C1, C2, C3>(_a1, _a2, _a3), a4(_a4){}
		caller4 (const caller4& c) : caller3<C1,C2,C3>(c), a4(const_cast<caller4&>(c).a4){}
	};

	// We allow a callback call to remove its entry from the list (but only this).
	template <class Caller> void callimpl (Caller& c) {
		if (callbacks.size())
			for (typename std::list<D>::iterator i = callbacks.begin(); i != callbacks.end();)
				c(*i++);
	}

	// We allow a callback call to remove any entry from the list.
	// Callbacks that have been removed are not invoked.
	template <class Caller> void safecallimpl (Caller& c) {
		if (callbacks.size()) {
			std::list<D*> copy;
			for (typename std::list<D>::iterator i = callbacks.begin(); i != callbacks.end(); ++i)
				copy.push_back(&*i);
			for (typename std::list<D*>::iterator i = copy.begin(); i != copy.end(); ++i)
				if (ptrs.in(*i))
					c(**i);
		}
	}

	/////////////////////////////////////////////////////////////////

	public:
	void call (void) 
		{ callimpl(utempobj(caller0())); }

	template <class C1> void call(C1 a1) 
		{ callimpl(utempobj(caller1<C1>(a1))); }

	template <class C1, class C2> void call(C1 a1, C2 a2)
		{ callimpl(utempobj(caller2<C1, C2>(a1, a2))); }

	template <class C1, class C2, class C3> void call(C1 a1, C2 a2, C3 a3)
		{ callimpl(utempobj(caller3<C1, C2, C3>(a1, a2, a3))); }

	void safecall (void) 
		{ safecallimpl(utempobj(caller0())); }

	template <class C1> void safecall(C1 a1) 
		{ safecallimpl(utempobj(caller1<C1>(a1))); }

	template <class C1, class C2> void safecall(C1 a1, C2 a2)
		{ safecallimpl(utempobj(caller2<C1, C2>(a1, a2))); }

	template <class C1, class C2, class C3> void safecall(C1 a1, C2 a2, C3 a3)
		{ safecallimpl(utempobj(caller3<C1, C2, C3>(a1, a2, a3))); }

	void	add (const D& data) {
				DASSERT(find(data.f(), data.c())== callbacks.end());	// Should not be inside.
				callbacks.push_back(data); 
				ptrs.insert(&callbacks.back());
			}

	void	add (F f, void* c = (void*) 0) 
				{ add(D(f,c)); }

	bool	empty (void) const 
				{ return callbacks.empty(); }

	void	remove (F f, void* c = (void*) 0) {
				typename std::list<D>::iterator pos = find(f, c);
				DASSERT(pos != callbacks.end());			// Should definitely be inside.
				ptrs.remove(&*pos);
				callbacks.erase(pos);
			}

	void	clear (void) 
				{ callbacks.clear(); ptrs.clear(); }

	ucallbacklist (void) {}
	ucallbacklist (const ucallbacklist& l) {
		for (typename std::list<D>::const_iterator i = l.callbacks.begin(); i != l.callbacks.end(); ++i)
			add(*i);
	}
	~ucallbacklist() { clear(); }
};

#define	UCALLBACKLIST_ADD_REMOVE_IMPL(_name,_list,_type)				\
	void	AddOn##_name(_type f, void* c = (void*) 0) const			\
				{ _list.add(f, c); }									\
	void	RemoveOn##_name (_type f, void* c = (void*) 0) const		\
				{ _list.remove(f, c); }									\
	void	ClearOn##_name (void)										\
				{ _list.clear(); }

/////////////////////////////////////////////////////////////////
// This class can be used as a base for classes that aim to provide
// change notification. Alternatively it may be used as a field.
// It is up to the caller to supply the appropriate derived instance.

class uchangenotifier {

	public:
	typedef void			(*Callback)(void* context, void* closure);

	protected:
	typedef ucallbacklist<Callback, ucallbackwithclosure<Callback> >	Callbacks;
	mutable Callbacks													onChanged;
	mutable Callbacks													onDeleted;
	mutable util_ui32													locked;
	mutable util_ui32													pending;
	void*																context;
	void*					GetContext (void* _context) const 
								{ return context? context : _context; }

	void					NotifyChangedPriv (void* _context) const
								{ DASSERT(!locked); onChanged.call(GetContext(_context)); pending = 0; }
	void					NotifyDeletedPriv (void* _context = (void*) 0) const
								{ onDeleted.call(GetContext(_context)); }
	void					SafeNotifyChangedPriv (void* _context) const
								{ DASSERT(!locked); onChanged.safecall(GetContext(_context)); pending = 0; }

	public:
	UOVERLOADED_ASSIGN_VIA_COPY_CONSTRUCTOR(uchangenotifier)
	UCALLBACKLIST_ADD_REMOVE_IMPL(Change, onChanged, Callback)
	UCALLBACKLIST_ADD_REMOVE_IMPL(Delete, onDeleted, Callback)

	void					SetContext (void* _context)
								{ context = _context; }
	void*					GetContext (void) const 
								{ return context; }

	void					ClearOnChange (void) const
								{ onChanged.clear(); }
	void					LockNotifyOnChange (void) const
								{ ++locked; }

	void					UnlockNotifyOnChange (void* _context = (void*) 0) const
								{ DASSERT(locked); if (!--locked && pending) NotifyChangedPriv(_context); }
	void					UnlockSafeNotifyOnChange (void* context) const
								{ DASSERT(locked); if (!--locked && pending) SafeNotifyChangedPriv(context); }

	void					NotifyChanged (void* _context = (void*) 0) const
								{ if (!locked) NotifyChangedPriv(_context); else ++pending; }
	void					SafeNotifyChanged (void* _context) const
								{ if (!locked) SafeNotifyChangedPriv(_context); else ++pending; }

	uchangenotifier (const uchangenotifier& n) : locked(0), pending(0), context ((void*) 0) {}	// no state is actually copied!
	uchangenotifier (void) : locked(0), pending(0), context ((void*) 0) {}
	virtual ~uchangenotifier(){  NotifyDeletedPriv();}
};

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
