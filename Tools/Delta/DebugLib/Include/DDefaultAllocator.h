// DDefaultAllocator.h
// The default malloc / free C allocator.
// Defensive programming library.
// A. Savidis,
// Updated July 2004.
//

#ifndef	DDEFAULTALLOCATOR_H
#define	DDEFAULTALLOCATOR_H

//------------------------------------------------------

#include <memory>
#include <stdlib.h>

template <class T> class CAllocator {

	public:
	typedef T			value_type;
	typedef size_t		size_type;
	typedef ptrdiff_t	difference_type;
	typedef T*			pointer;
	typedef const T*	const_pointer;
	typedef T&			reference;
	typedef const T&	const_reference;

	pointer				address (reference r) const	{ return &r; }
	const_pointer		address (const_reference r) const { return &r; }

	CAllocator (void){}
	template <class U> CAllocator(const CAllocator<U>&){}
	~CAllocator() {}

	pointer				allocate (size_type n, const void* hint = 0);
	void				deallocate (pointer p, size_type n);
	void				construct (pointer p, const T& val) { new(p) T(val); }
	void				destroy (pointer p) { p->T::~T(); }
	size_type			max_size(void) const { return 0xffffffff; }

	template <class U> struct rebind { typedef CAllocator<U> other; };
};

template <class T>
CAllocator<T>::pointer CAllocator<T>::allocate (size_type _Count, const void* _Hint) {
	return (pointer) malloc(_Count * sizeof(T));
}

template <class T>
void CAllocator<T>::deallocate(pointer _Ptr, size_type _Count) {
	while (_Count--)
		free((void*)_Ptr++);
}

//------------------------------------------------------

#endif	// Do not add stuff beyond this point.