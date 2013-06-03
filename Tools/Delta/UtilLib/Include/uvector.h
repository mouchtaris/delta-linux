// uvector.h
// A very simple but very fast vector template class.
// ScriptFighter Project.
// A. Savidis, July 2002.
// Extended July 2009, added iterator.
//

#ifndef	UVECTOR_H
#define	UVECTOR_H

#include "DDebug.h"
#include "ufunctors.h"
#include "utypes.h"
#include <memory.h>

#define	UVECTOR_AUTORESIZE_QUANTITY	256

//---------------------------------------------------------------
// This is a dynamic vector for use with limited flexibility but
// with excellent performance. It is to be used when the size of
// the array is known to be at a fixed size, and when we can ensure that
// we never get out of bounds.
//
template <class T, class TAssign = uassigndefaultfunc<T> > class uvector {
	
	private:
	T**			data;
	util_ui32	total;
	util_ui32	left, right;	// interval is actually [left, right)
	
	bool			invariant (void) const {
						return	(!data == !total)			&&
								(left <= right)				&&
								(right - left <= total)		&&
								(right <= total)			&&
								(data || (!left && !right));
					}

	void			rellocate (util_ui32 newSize) {
						DASSERT(capacity() < newSize);
						T** newData = DNEWARR(T*, newSize);
						memset(newData, 0, newSize);	// Zero memory.
		
						if (data) {						// Assign old user data entries.
							memcpy(newData, data + left, size() * sizeof(T*));
							DDELARR(data);
						}

						data	= newData;
						total	= newSize;
						right	= size();	// Old size, only existing entries.
						left	= 0;
					}

	void			expandright (void) {
						DASSERT(right == total);
						util_ui32	newSize = capacity() + UVECTOR_AUTORESIZE_QUANTITY;
						T**			newData = DNEWARR(T*, newSize);						
						if (data) {						// Assign old user data entries.
							memcpy(newData, data, capacity() * sizeof(T*));
							DDELARR(data);
						}
						memset(newData + total, 0, UVECTOR_AUTORESIZE_QUANTITY * sizeof(T*));	// Nullify empty positions.
						data	= newData;
						total	= newSize;
					}

	void			expandleft (void) {
						DASSERT(!left);
						util_ui32	newSize = capacity() + UVECTOR_AUTORESIZE_QUANTITY;
						T**			newData = DNEWARR(T*, newSize);						
						if (data) {						// Assign old user data entries.
							memcpy(newData + UVECTOR_AUTORESIZE_QUANTITY, data, capacity() * sizeof(T*));
							DDELARR(data);
						}
						memset(newData, 0, UVECTOR_AUTORESIZE_QUANTITY);	// Nullify empty positions.
						data	= newData;
						total	= newSize;
						left = UVECTOR_AUTORESIZE_QUANTITY;
						right += UVECTOR_AUTORESIZE_QUANTITY;
					}

	/////////////////////////////////////////////////////////////////

	template <typename Titer, typename Tval> struct iterator_impl {
		typedef typename uconst_of<Tval>::t Tpure;
		uvector<Tpure>*	v;
		util_ui32		j;
		bool								isvalidforaccess (void) const 
												{ return v && j >= v->left && j < v->right; }
		void								fwd (void) 
												{ DASSERT(v && j < v->right); ++j; }
		void								bwd (void) 
												{ DASSERT(v && j > v->left); --j; }
		util_ui32							getindex (void) const
												{ DASSERT(isvalidforaccess()); return j - v->left; }
		bool								operator==(const Titer& i) const 
												{ return v == i.v && j == i.j; }
		bool								operator!=(const Titer& i) const 
												{ return !operator==(i); }
		Tval&								operator*(void) const
												{ DASSERT(isvalidforaccess()); return *DPTR(v->data[j]); }
		Tval*								operator->(void) const 
												{ DASSERT(isvalidforaccess());  return *DPTR(v->data + v->left + j); }
		const Titer							operator++(int) 
												{ DASSERT(v); Titer i(*this); fwd(); return i; }
		const Titer							operator++(void) 
														{ DASSERT(v); fwd(); return *this; }
		const Titer							operator--(int)
														{ DASSERT(v); Titer i(*this); bwd(); return i; }
		const Titer							operator--(void)
												{ DASSERT(v); bwd(); return *this;}
		const Titer&						operator=(const Titer& i) 
												{ return *new(this) Titer(i); }

		iterator_impl (const iterator_impl& i):				
			v (const_cast<uvector<Tpure>*>(i.v)), j(i.j) { DASSERT(v && j >= v->left && j <= v->right);}
		iterator_impl (const uvector<Tpure>* _v, util_ui32 _j):	
			v (const_cast<uvector<Tpure>*>(_v)), j(_v->left + _j) { DASSERT(v && j >= v->left && j <= v->right); }
		iterator_impl (void):							
			v ((uvector<Tpure>*) 0), j(0){}
	};	

	/////////////////////////////////////////////////////////////////

	public:
	class iterator :	public std::iterator<std::bidirectional_iterator_tag, T>,
						public iterator_impl<iterator, T> {

		public:
		iterator (const iterator_impl& i): iterator_impl(i){}
		iterator (const iterator& i): iterator_impl(i){}
		iterator (const uvector* _v, util_ui32 _j):	iterator_impl(_v, _j){}
		iterator (void){}
	};

	class const_iterator :	public std::iterator<std::bidirectional_iterator_tag, T>,
							public iterator_impl<const_iterator, const T> {

		public:
		const_iterator (const iterator_impl<typename uvector::iterator, T>& i): iterator_impl(i.v, i.j){}
		const_iterator (const iterator_impl& i): iterator_impl(i){}
		const_iterator (const const_iterator& i): iterator_impl(i){}
		const_iterator (const uvector* _v, util_ui32 _j): iterator_impl(_v, _j){}
		const_iterator (void){}
	};

	/////////////////////////////////////////////////////////////////

	UOVERLOADED_ASSIGN_VIA_COPY_CONSTRUCTOR(uvector)

	const iterator	begin (void) const 
						{ DASSERT(invariant()); return iterator(this, 0); }
	const iterator	begin (void)
						{ DASSERT(invariant()); return iterator(this, 0); }
	const iterator	end (void) const 
						{ DASSERT(invariant()); return iterator(this, size()); }
	const iterator	end (void)
						{ DASSERT(invariant()); return iterator(this, size()); }

	bool			valid (void) const 
						{ DASSERT(invariant()); return !!size(); }
	T&				operator[](util_ui32 index) const 
						{ DASSERT(invariant()); DASSERT(validindex(index)); return *DPTR(data[left + index]); }
	bool			validindex (util_ui32 index) const 
						{ DASSERT(invariant()); return index < size(); }
	util_ui32		size (void) const 
						{ DASSERT(invariant()); return right - left; }
	util_ui32		capacity (void) const
						{ DASSERT(invariant()); return total; }
	T&				front (void) const
						{ DASSERT(invariant()); return this->operator[](0); }
	T&				back (void) const
						{ DASSERT(invariant()); return this->operator[](size() - 1); }

	void			push_back (const T& val) {
						DASSERT(invariant());
						if (right == total)	
							expandright();
						DASSERT(right < total);
						data[right++] = DNEWCLASS(T, (val));
					}

	void			pop_back (void) {
						DASSERT(invariant() && size() && right);
						udelete(data[--right]);
					}

	void			push_front (const T& val) {
						DASSERT(invariant());
						if (!left)
							expandleft();
						DASSERT(left);
						data[--left] = DNEWCLASS(T, (val));
					}

	void			pop_front (void) {
						DASSERT(invariant() && size());
						udelete(data[left++]);
					}

	void			resize (util_ui32 newSize) {		// Only expansion is supported.
						
						DASSERT(invariant()); 
						if (capacity() >= newSize)
							return;
					
						util_ui32 oldSize = size();
						rellocate(newSize);

						// Create extra data entries.
						T**			extraStart	= data + oldSize;
						util_ui32	extraSize	= newSize - oldSize;
						while (extraSize--)
							*extraStart++ = DNEW(T);

						right = total;	// Since all entries are filled-in.
						DASSERT(invariant()); 
					}

	void			clear (void) { 
						DASSERT(invariant()); 
						while (left != right)
							udelete(data[left++]);
						if (data)
							DDELARR(data);
						unullify(data);
						total = left = right = 0;
						DASSERT(invariant()); 
					}

	uvector (const uvector& v)  {
		DASSERT(v.invariant()); 
		if (v.size()) {
			left = right = 0;
			data = DNEWARR(T*, total = v.size());
			for (util_ui32 i = 0; i < total; ++i)
				data[right++] = DNEWCLASS(T, (*DPTR(v.data[i])));
			DASSERT(invariant());
		}
		else
			new (this) uvector();
	}
	uvector (void) : data((T**) 0), total(0), left(0), right(0) { DASSERT(invariant()); }
	~uvector() { clear(); }
};

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
