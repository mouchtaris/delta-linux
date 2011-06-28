// DAllocationStats.cpp
// Allocation statistics class implementation.
// A. Savidis, February 2003.
//

#include <stdio.h>
#include "DAllocationStats.h"
#include "DMemoryManager.h"

/////////////////////////////////////////////////////////////

void AllocationStats::Record (util_ui32 size) {

	util_ui8 pos = size % UHASHTABLE_HASH_SIZE;
	AllocationStat* stat = statHash[pos];

	// Try to find an entry for 'size'.
	//
	while (stat && stat->GetSize() != size)
		stat = stat->GetNext();

	if (!stat) {
		if (!statAllocator->isfull()) {	// If full, all additions are ingored there after.

			stat = statAllocator->allocate();
			DASSERT(stat);

			stat->SetSize(size);
			stat->IncTotal();
			
			stat->SetNext(statHash[pos]);
			statHash[pos] = stat;
		}
	}
	else
		stat->IncTotal();
}

/////////////////////////////////////////////////////////////

void AllocationStats::Report (const char* file) {

	if (FILE* fp = fopen(file, "wt")) {
		DASSERT(fp);
		
		for (util_ui16 i=0; i < UHASHTABLE_HASH_SIZE; ++i)
			for (AllocationStat* stat = statHash[i]; stat; stat = stat->GetNext())
				stat->WriteText(fp);

		fclose(fp);
	}
}

/////////////////////////////////////////////////////////////

AllocationStats::AllocationStats (void) {

	for (util_ui16 i=0; i < UHASHTABLE_HASH_SIZE; ++i)
		statHash[i] = (AllocationStat*) 0;

	statAllocator = new (DMemoryManager::GetMemory(sizeof(StatAllocator))) StatAllocator;
	statAllocator->init();
}

/////////////////////////////////////////////////////////////

AllocationStats::~AllocationStats() {
	DMemoryManager::ReleaseMemory(statAllocator);
}

/////////////////////////////////////////////////////////////
