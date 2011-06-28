// ualgorithms.h
// Some algorithms that work on either STL or hand-made templates.
// A. Savidis, March 2003.
//

#ifndef	UALGORITHMS_H
#define	UALGORITHMS_H

//---------------------------------------------------------------

template <
			class _II, 
			class _OI, 
			class _Pred
		>

void ucopy_if (
		_II		first, 
		_II		last,
		_OI		at, 
		_Pred	pred 
	) {

	for (; first != last; ++first)
		if (pred(*first)) {
			*at = *first;
		++at;
		}
}

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.

