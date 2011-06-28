// DDebug.h
// General purpose debugging facilities.
// New / Delete overloading, as well as special monitorable
// pointer access operations. Handles pointer under-run / over-run,
// invalid pointer use, erroneous deletes and memory leaks.
// A. Savidis, February 2004.


#ifndef	DDEBUG_HH
#define	DDEBUG_HH

#include "utypes.h"
#include "uptr.h"
#include "dassert.h"
#include "dsmallobjectallocator.h"

#define	DDEBUG_USE
#define	DDEBUG_STANDARD_NEWDELETE
//	#define	DDEBUG_SMALLALLOCATOR

template <class T>
T* _DNOTNULL (T* p) {
	DASSERT(p);
	return p;
}

//------------------------------------------------------
// POINTER OPERATIONS.
//
#if	!defined(DDEBUG_USE)

#include <malloc.h>
#include <string.h>

#define	DNEW(a)				_DNOTNULL(new a)
#define	DNEWCLASS(C,Args)	DNEW(C##Args)
#define	DNEWARR(t,n)		DNEW(t[n])
#define	DDELPTR(p)			delete p
#define	DDELARR(p)			delete[] p
#define	DDELETE(p)			delete p
#define	DPTR(ptr)			(ptr)
#define	DSIZE(ptr,n)
#define	DPTRLVALUE(ptr)		(ptr)
#define	DNPTR(ptr)			(ptr)
#define	DASSERTPTR(ptr)		
#define	DNULLCHECK(ptr)		(ptr)
#define	DADDR(addr)			(addr)

#define	DMONITORCONTENT(p, t)	
#define	DMONITORBOUNDS(p, t)	
#define	DMONITOREXPIRATION(p,d)
#define	DUNMONITORCONTENT(p)	
#define	DUNMONITORBOUNDS(p)		
#define	DUNMONITOREXPIRATION(p)	
#define	DLATEDISPOSALON(t)		
#define	DLATEDISPOSALOFF()		

#define	DSTARTLOGGING(path)
#define	DSTOPLOGGING()

#define	DFRIENDDESTRUCTOR()
#define	dseterrorcallback(c)
#define	dinit(c)
#define	dclose
#include <new.h>
#endif

//------------------------------------------------------

#if	defined(DDEBUG_USE)

#include <new.h>	// Need that for placement operator.
#include <stdlib.h>

////////////////////////////////////////////////////////
// Private functions. Do not bother.
//
extern void		dcontext (char* file, util_ui16 line, char* expr = (char*) 0);
extern void		drealign (void* ptr);
extern void		dcheckptr (void* p);
extern void		dchecksize (void* p, util_ui32 size);
extern void		dinit (void (*)(const char*));
extern void		dclose (void);
typedef void	(*dmsgcallback) (const char*);
extern void		dseterrorcallback (dmsgcallback msgFunc);

#define	_DSTR(e)		#e
#define	DNULLCHECK(p)	_DNOTNULL(p)
#define	DPTR(p)			_DPTR(p)
#define	DSIZE(p,n)		dchecksize(p, n)
#define	DASSERTPTR(p)	_DASSERTPTR(p)
#define	DPTRLVALUE(p)	_DPTRLVALUE(p)
#define	DNPTR(p)		(!(p) ? (p) : DPTR(p))


//////////////////////////////////////////////////////////////

#ifdef	DDEBUG_STANDARD_NEWDELETE

#define	DNEW(a)				_DNOTNULL((dcontext(__FILE__, __LINE__, #a), _DREALIGN(new a)))
#define	DNEWCLASS(C,Args)	DNEW(C##Args)
#define	DNEWARR(t,n)		_DNOTNULL((dcontext(__FILE__, __LINE__, _DSTR(t[n])), _DREALIGN(new t[n])))
#define	DDELARR(p)			delete[] (p)
#define	DDELPTR(p)			delete (p)
#define	DDELETE(p)			delete (p)

#define	DFRIENDDESTRUCTOR()

#endif	//DDEBUG_STANDARD_NEWDELETE

//////////////////////////////////////////////////////////////

#ifndef	DDEBUG_STANDARD_NEWDELETE

extern void		operator	delete (void* p);
extern void		operator	delete[] (void* p);
extern void*	operator	new[] (size_t n);
extern void*	operator	new (size_t n);

extern size_t	dprivatememsize (void* ptr);
extern void*	dprivatemalloc (size_t size);
extern void		dprivatefree (void* ptr, bool array = false);

// Some functions may have private destructors.
// The only way to use placement is just to issue
// friendliness.
//
#define	DFRIENDDESTRUCTOR() \
	friend class _DDESTRUCTORS;

//***********************************************************

class _DDESTRUCTORS {
	public:

	template <class T> 	static void simple (T* inst) {	// Destructor of simple elements. 

		// Don't use T::~T() since it does not call virtual destructors.

		void* mem = (void*) inst;
		PINC(mem, -DMEMORY_USERSIZE_BYTES);				// Position at size information.

#ifdef	DDEBUG_SMALLALLOCATOR
		if (dsmallobjectflagraised(*((util_ui8*) mem))) {
			*((util_ui8*) mem) &= DDEBUG_ALLOCATOR_CLEARBIT_MASK;
			DSmallAllocatorSuper* alloc = DSmallAllocatorSuper::GetBest(*((util_ui32*) mem));
			DASSERT(alloc);

			inst->~T();
			alloc->Free(mem);
			return;
		}
#endif
		DASSERT(!*((util_ui32*) mem));					// In case of simple elements, it must be 0.
		DASSERT(dprivatememsize(inst) >= sizeof(T));	// Does not discard all cases, but some very dangerous.
		inst->~T();
		dprivatefree(inst);								// Memory disposal.
	}

	//************************

	template <class T> 	static void array (T* arr) { 

		size_t size = dprivatememsize(arr);			// Total array memory size in bytes.
		size -= sizeof(util_ui32);					// Don't forget those extra four bytes.
		DASSERT((size % sizeof(T))==0);				// Should be dividable with array element size.

		void* mem = (void*) arr;					// Now get the number of array elements.
		PINC(mem, -((util_i16) sizeof(util_ui32)));	// It is in the preceeding four memory bytes.
		util_ui32 n = *((util_ui32*) mem);			// Stored as an unsigned int.

		DASSERT(n== size / sizeof(T));				// Just check it is in symphony with #memory bytes.

		arr += n;									// 'n' is total, 'arr' is passed last item.
		while (n--)	{								// Destroying in reverse order.
			--arr;									// Decrement here, because in scalar types,
			arr->~T();								// the destructor call may be completely ignored!
		}
		
		dprivatefree(mem);							// Now free whole array memory. Use real address.
	}
};

//***********************************************************

template <class T> class _DCONSTRUCTORS {
	public:
	static T* array (size_t n) {

		void* mem = dprivatemalloc(sizeof(util_ui32) + n * sizeof(T));
		*((util_ui32*) mem) = n;
		T* arr = (T*) POFFS(mem, sizeof(util_ui32));

		for (T* p = arr; n; ++p, --n)
			new (p) T;

		drealign(arr);
		return arr;
	}

	/////////////////////////////////////////////////////////////
	// In simple 'new', we reserve 4 extra header bytes
	// in which we write just 0, so that upon destruction
	// we can distinguish among memory created via 'new' and
	// 'new[]' (and assert if a mismatch is observed).
	// Notice that we explicitly perform realignment before returning,
	// so that when objects get constructed, they may use 'this',
	// but the latter can be verified to be a correct pointer.
	//
	void* simple (const util_ui32 n) {							// 'n' comes from sizeof().

	#ifdef	DDEBUG_SMALLALLOCATOR
		DSmallAllocatorSuper* alloc = DSmallAllocatorSuper::GetBest(n);
		if (alloc && alloc->HasMemory())
			return alloc->Allocate(n);
	#endif

		void* mem = dprivatemalloc(sizeof(util_ui32) + n);		// Need 4 extra bytes.
		*((util_ui32*) mem) = 0;								// We put 0 to indicate simple 'new'.
		void* addr = POFFS(mem, sizeof(util_ui32));				// But we return 'mem' after those bytes.

		drealign(addr);											// We re-align here explicitly.
		return addr;											// That's the user-memory.			
	}
};

//***********************************************************
// The operators re-defined. DNEW cannot be used for array
// construction, but it has to be substituted by DNEWARR instead. 
//
#define	DNEW(T)	\
		_DNOTNULL( \
			( \
				dcontext(__FILE__, __LINE__, _DSTR(e)), \
				new ( _DCONSTRUCTORS< T >::simple(sizeof(T)) T \
			) \
		)

#define	DNEWCLASS(C,Args)	\
		_DNOTNULL( \
			( \
				dcontext(__FILE__, __LINE__, _DSTR(C##Args)), \
				new (_DCONSTRUCTORS< C >::simple(sizeof C) ) C##Args \
			) \
		)

#define	DNEWARR(T,n) \
		_DNOTNULL( \
			( \
				dcontext(__FILE__, __LINE__, _DSTR(T[n])), \
				_DCONSTRUCTORS< T >::array(n) \
			) \
		)
#define	DDELPTR(p)		_DDESTRUCTORS::simple(p)
#define	DDELARR(p)		_DDESTRUCTORS::array(p)
#define	DDELETE(p)		_DDESTRUCTORS::simple(p)

#endif	// !DDEBUG_STANDARD_NEWDELETE

//////////////////////////////////////////////////////////////
// Re-align the value returned by new, since this may not
// be the same with the one returned by malloc internally.
//
template <class T>
T*	_DREALIGN(T* p) {
	drealign(p);
	return p;
}

template <class T>
T*	_DPTR (T* p) {
	dcheckptr((void*) p);
	return p;
}

template <class T>
void _DASSERTPTR (T* p) {
	dcheckptr((void*) p);
}

template <class T>
T*&	_DPTRLVALUE (T*& p) {
	dcheckptr((void*) p);
	return p;
}

// Period and expiration delay are provided in msecs.
//
extern void dmonitorcontent (void* p, util_ui32 period);
extern void dmonitorvalidity (void* p, util_ui32 period);
extern void dmonitorexpiration (void* p, util_ui32 delay);
extern void dunmonitorcontent (void* p);
extern void dunmonitorvalidity (void* p);
extern void dunmonitorexpiration (void* p);
extern void dlatedisposalon (util_ui32 delay);
extern void dlatedisposaloff (void);
extern void dstartlogging (const char* path);
extern void dstoplogging (void);

#define	DMONITORCONTENT(p, t)			dmonitorcontent(p,  t)
#define	DMONITORVALIDITY(p, t)			dmonitorvalidity(p, t)
#define	DMONITOREXPIRATION(p,d)			dmonitorexpiration(p, d)
#define	DUNMONITORCONTENT(p)			dunmonitorcontent(p)
#define	DUNMONITORVALIDITY(p)			dunmonitorvalidity(p)
#define	DUNMONITOREXPIRATION(p)			dunmonitorexpiration(p)
#define	DLATEDISPOSALON(t)				dlatedisposalon(t)
#define	DLATEDISPOSALOFF()				dlatedisposaloff()
#define	DSTARTLOGGING(path)				dstartlogging(path)
#define	DSTOPLOGGING()					dstoplogging()

#endif	// DDEBUG_USE

//////////////////////////////////////////////////////////////
// Those macros are now completely obsolete.
//
#define	DADDR(addr)		(addr)
#define	DPARR(p,i)		((p)[i])
#define	DPOFFS(p,i)		((p)+(i))
#define	DPMOVE(p,i)		((p)+=(i))
#define	DVPP(p)			((p)++)
#define	DVMM(p)			((p)--)
#define	DPPV(p)			(++(p))
#define	DMMV(p)			(--(p))

//------------------------------------------------------

#endif	// Do not add stuff beyond this point.