// DDebug.h
// General purpose debugging facilities.
// New / Delete overloading, as well as special monitorable
// pointer access operations. Handles pointer under-run / over-run,
// invalid pointer use, erroneous deletes and memory leaks.
// A. Savidis, February 2004.


#ifndef	DDEBUG_H
#define	DDEBUG_H

#include "utypes.h"
#include "uptr.h"
#include "DAssert.h"
#include "DSmallObjectAllocator.h"

//------------------------------------------------------

//	#define	DDEBUG_SMALLALLOCATOR

template <class T>
T* _DNOTNULL (T* p) {
	DASSERT(p);
	return p;
}

//------------------------------------------------------

#if	!defined(DDEBUG_USE)

#include <malloc.h>
#include <string.h>

template <typename T> void udeletesubstitute(T* p) { delete p; }
template <class T, const unsigned N> 
typename void udeletesubstitute (T (*&p)[N]) { delete[] p; }
template <typename T> void udelarraysubstitute(T* p) { delete[] p; }

#define	DNEW(a)				_DNOTNULL(new a)
#define	DNEWCLASS(C,Args)	DNEW(C Args)
#define	DNEWARR(t,n)		DNEW(t[n])
#define	DDELPTR(p)			udeletesubstitute(p)
#define	DDELARR(p)			udelarraysubstitute(p)
#define	DDELETE(p)			udeletesubstitute(p)
#define	DPTR(ptr)			(ptr)
#define	DSIZE(ptr,n)
#define	DPTRLVALUE(ptr)		(ptr)
#define	DNPTR(ptr)			(ptr)
#define	DASSERTPTR(ptr)		
#define	DNULLCHECK(ptr)		(ptr)
#define	DFRIENDDESTRUCTOR()														\
			template <typename _T> friend void ::udeletesubstitute(_T* p);		\
			template <typename _T> friend void ::udelarraysubstitute(_T* p);
#define	dseterrorcallback(c)
#define	dsetassertfunc(f)
#define	dsetassertcleaner(f)
#define	dinit(c)
#define	dclose()

#endif	// !defined(DDEBUG_USE)

//------------------------------------------------------

#if	defined(DDEBUG_USE)

#include <new>
#include <cstdlib>

////////////////////////////////////////////////////////
// Private functions. Do not bother.
//
DDEBUG_FUNC void	dcontext (const char* file, util_ui16 line, const char* expr = (char*) 0);
DDEBUG_FUNC void	dcheckptr (void* p);
DDEBUG_FUNC void	dchecksize (void* p, util_ui32 size);
DDEBUG_FUNC size_t	dprivatememsize (void* ptr);
DDEBUG_FUNC void*	dprivatemalloc (size_t size);
DDEBUG_FUNC void	dprivatefree (void* ptr, bool array = false);

////////////////////////////////////////////////////////
// Public functions. Only those are used by the programmer.
//
typedef void (*dmsgcallback) (const char*);
typedef void (*dassertercallback) (bool, const char*, uvoidvoid_f);

DDEBUG_FUNC	void				dinit (dmsgcallback onError);
DDEBUG_FUNC	void				dclose (void);
DDEBUG_FUNC	void                dseterrorcallback (dmsgcallback msgFunc);
DDEBUG_FUNC	void                dsetassertfunc (dassertercallback asserter);
DDEBUG_FUNC	dassertercallback	dgetassertfunc (void);
DDEBUG_FUNC	void                dsetassertcleaner (uvoidvoid_f cleaner);
DDEBUG_FUNC	uvoidvoid_f         dgetassertcleaner (void);

#define	_DSTR(e)		#e
#define	DNULLCHECK(p)	_DNOTNULL(p)
#define	DPTR(p)			_DPTR(p)
#define	DSIZE(p,n)		dchecksize(POFFS(p,-DDEBUG_USERSIZE_BYTES),  n)
#define	DASSERTPTR(p)	_DASSERTPTR(p)
#define	DPTRLVALUE(p)	_DPTRLVALUE(p)
#define	DNPTR(p)		(!(p) ? (p) : DPTR(p))

////////////////////////////////////////////////////////
// Some classes may have private destructors.
// The only way to use placement is just to issue
// friendliness.
//
#define	DFRIENDDESTRUCTOR() \
	friend class DDEBUG_CLASS _DDESTRUCTORS;

////////////////////////////////////////////////////////

class DDEBUG_CLASS _DDESTRUCTORS {
	public:

	template <class T> 	static void simple (T* inst) {	// Destructor of simple elements. 

		void* mem = POFFS(inst, -DDEBUG_USERSIZE_BYTES);

#ifdef	DDEBUG_SMALLALLOCATOR
		if (dsmallobjectflagraised(mem)) {
			dclearsmallobjectflag(mem);
			DSmallAllocatorSuper* alloc = DSmallAllocatorSuper::GetBest(*((util_ui32*) mem));
			DASSERT(alloc);

			inst->~T();
			alloc->Free(mem);
			return;
		}
#endif
		// In case of simple elements, user array size must be 0.
		// Also, allocated memory size must be at least what is needed for T.
		//
		DASSERT(!*((util_ui32*) mem));		
		DASSERT(dprivatememsize(mem) >= DDEBUG_USERSIZE_BYTES + sizeof(T));

		// Don't use T::~T() since it does not call virtual destructors.
		//
		inst->~T();
		dprivatefree(mem);	// Memory disposal.
	}

	//************************

	template <class T> 	static void array (T* arr) { 

		void* mem = POFFS(arr, -DDEBUG_USERSIZE_BYTES);	// Reposition at originally allocated memory.
		size_t size = dprivatememsize(mem);				// Get real allocation size.

		size -= DDEBUG_USERSIZE_BYTES;					// Don't forget the extra added bytes.
		DASSERT((size % sizeof(T))==0);					// The result must be dividable with array element size.
		
		util_ui32 n = *((util_ui32*) mem);				// Total array elements.

		DASSERT(n== size / sizeof(T));					// Just check it is in symphony with type size.

		arr += n;										// 'n' is total, 'arr' is passed last item.
		while (n--)	{									// Destroying in reverse order.
			--arr;										// Decrement here, because in scalar types,
			arr->~T();									// the destructor call may be completely ignored!
		}
		
		dprivatefree(mem, true);						// Now free whole array memory. Use real address.
	}
};

////////////////////////////////////////////////////////

template <class T> class _DCONSTRUCTORS {
	public:
	static T* array (size_t n) {
		void* mem = dprivatemalloc(DDEBUG_USERSIZE_BYTES + n * sizeof(T));
		*((util_ui32*) mem) = (util_ui32) n;
		T* arr = (T*) POFFS(mem, DDEBUG_USERSIZE_BYTES);

		for (T* p = arr; n; ++p, --n)
			new (p) T;

		return arr;
	}

	/////////////////////////////////////////////////////////////
	// In simple 'new', we reserve 4 extra header bytes
	// in which we write just 0, so that upon destruction
	// we can distinguish among memory created via 'new' and
	// 'new[]' (and assert if a mismatch is observed).
	//
	static void* simple (const util_ui32 n) {					// 'n' comes from sizeof().

	#ifdef	DDEBUG_SMALLALLOCATOR
		DSmallAllocatorSuper* alloc = DSmallAllocatorSuper::GetBest(n);
		if (alloc && alloc->HasMemory())
			return alloc->Allocate(n);
	#endif

		void* mem = dprivatemalloc(DDEBUG_USERSIZE_BYTES + n);	// Need those extra bytes.
		*((util_ui32*) mem) = 0;								// We put 0 to indicate simple 'new'.
		return POFFS(mem, DDEBUG_USERSIZE_BYTES);				// But we return 'mem' after those bytes.
	}
};

////////////////////////////////////////////////////////
// The operators re-defined. DNEW cannot be used for array
// construction, but it has to be substituted by DNEWARR instead. 
//
#define	DNEW(T)	\
		_DNOTNULL( \
			( \
				dcontext(__FILE__, (util_ui16) __LINE__, _DSTR(T)), \
				new (_DCONSTRUCTORS< T >::simple(sizeof(T)) ) T \
			) \
		)

#define	DNEWCLASS(C,Args)	\
		_DNOTNULL( \
			( \
				dcontext(__FILE__, (util_ui16) __LINE__, _DSTR(C) _DSTR(Args)), \
				new (_DCONSTRUCTORS< C >::simple(sizeof(C)) ) C Args \
			) \
		)

#define	DNEWARR(T,n) \
		_DNOTNULL( \
			( \
				dcontext(__FILE__, (util_ui16) __LINE__, _DSTR(T[n])), \
				_DCONSTRUCTORS< T >::array(n) \
			) \
		)
#define	DDELPTR(p)		_DDESTRUCTORS::simple(p)
#define	DDELARR(p)		_DDESTRUCTORS::array(p)
#define	DDELETE(p)		_DDESTRUCTORS::simple(p)

////////////////////////////////////////////////////////
// Here we know that the original pointer value is 
// some bytes before, where the size is actually stored.
//
template <class T>
T*	_DPTR (T* p) {
	dcheckptr(POFFS(p, -DDEBUG_USERSIZE_BYTES));
	return p;
}

template <class T>
void _DASSERTPTR (T* p) {
	dcheckptr(POFFS(p, -DDEBUG_USERSIZE_BYTES));
}

template <class T>
T*&	_DPTRLVALUE (T*& p) {
	dcheckptr(POFFS(p, -DDEBUG_USERSIZE_BYTES));
	return p;
}

#endif	// DDEBUG_USE

template <class T> void udelete (T*& p) 
	{ DASSERT(p); DDELETE((T*) p); unullify(p); }

template <class T> void udeleteasarray (T*& p) 
	{ DASSERT(p); DDELARR((T*) p); unullify(p); }

template <class T> void udeleteunlessnull (T*& p) 
	{ if (p) { DDELETE(p); unullify(p); } }

template <class T> void udeleteasarrayunlessnull (T*& p) 
	{ if (p) { DDELARR((T*) p); unullify(p); } }

template <class T> T* unew (T*& p) 
	{ DASSERT(!p); return p = DNEW(T); }

template <class T, const unsigned N>  typename uptrarray<T,N>::ptr_type unewarray (T (*&p)[N]) 
	{ DASSERT(!p); return p = (typename uptrarray<T,N>::ptr_type) DNEWARR(T, N); }

template <class T, const unsigned N> void udeletearray (T (*&p)[N])
	{ DASSERT(p); DDELARR(((T*) p)); unullify(p); }

template <class T, const unsigned N> void udeletearrayunlessnull (T (*&p)[N])
	{ if (p) { DDELARR(((T*) p)); unullify(p); } }

//------------------------------------------------------

#endif	// Do not add stuff beyond this point.