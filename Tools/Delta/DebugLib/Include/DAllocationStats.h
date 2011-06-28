// DAllocationStats.h
// Allocation statistics class.
// A. Savidis, February 2003.
//

#ifndef	DALLOCATIONSTATS_H
#define	DALLOCATIONSTATS_H

#include "uarraydoublelist.h"
#include "uhash.h"

//------------------------------------------------------

class AllocationStat : public udoublylinked {

	private:
	util_ui32		size;
	util_ui32		total;
	AllocationStat*	next;

	public:
	void			SetSize (util_ui32 _size) 
						{ DASSERT(!size); DASSERT(_size); size = _size; }
					
	void			IncTotal (void)
						{ ++total; }
					
	util_ui32		GetTotal (void) const
						{ return total; }
					
	util_ui32		GetSize (void) const
						{ return size; }

	AllocationStat*	GetNext (void) const
						{ return next; }

	void			SetNext (AllocationStat* _next) 
						{ next = _next; }

	void			init (void)
						{ size = total = 0; next = (AllocationStat*) 0; }

	void			WriteText (FILE* fp) {
						fprintf(
							fp, 
							"Allocation size '%d', total allocations '%d', total bytes '%u'.\n",
							size,
							total,
							total * size
						);
					}

	const AllocationStat& operator=(const AllocationStat& stat)
			{ new (this) AllocationStat(stat); return *this;  }

	AllocationStat (void) : 
		size(0), 
		total(0), 
		next((AllocationStat*) 0) {}

	AllocationStat (const AllocationStat& stat) : 
		size(stat.size), 
		total(stat.total), 
		next((AllocationStat*) 0) {}

	~AllocationStat(){}
};

/////////////////////////////////////////////////////////////

#define	ALLOCATIONSTAT_MAX	1024

class AllocationStats {

	private:
	typedef uarraydoublelist<AllocationStat, ALLOCATIONSTAT_MAX> StatAllocator;

	StatAllocator*	statAllocator;
	AllocationStat*	statHash[UHASHTABLE_HASH_SIZE];

	public:

	void		Record (util_ui32 size);
	void		Report (const char* file);

	AllocationStats (void);	
	~AllocationStats ();
};

//------------------------------------------------------

#endif	// Do not add stuff beyond this point.