// DMemoryCapsule.cpp
// Memory allocation block with defensive information.
// Defensive programming library.
// A. Savidis,
// Updated July 2004.
// Small objects will not be put checked here, they will be transparent
// as part of the memory manager.
//

#include "DDebug.h"
#include "ustrings.h"
#include "usystem.h"
#include "DMemoryCapsule.h"

// The following applies massive validation with every 
// pointer validation and is resource demanding.
//	#define	 APPLY_PERIODIC_MASSIVE_VALIDATION

//////////////////////////////////////////////////////////////////

MemoryCapsule*	MemoryCapsule::head = (MemoryCapsule*) 0;
util_ui32		MemoryCapsule::total = 0;
umutex*			MemoryCapsule::blockListMutex = (umutex*) 0;

#define	LISTEDIT_THREAD_SAFE() \
	LOCK_BLOCK((*blockListMutex))

//////////////////////////////////////////////////////////////////

#ifdef	USE_DELAYED_FREE
MemoryCapsule::DelayedFreeEntry	MemoryCapsule::delayedFree[MAX_DELAYED_FREE];
util_ui32						MemoryCapsule::totalDelayedFree = 0;

//*************************

void MemoryCapsule::CommitDelayedFree (void) {
	for (DelayedFreeEntry* p = delayedFree; totalDelayedFree; ++p, --totalDelayedFree) {
		p->CheckPostdisposalOverwrite();
		DMemoryManager::ReleaseMemory(p->block);
	}
}

//*************************

void MemoryCapsule::FreeByDelay (void* block, util_ui32 size) {

	if (++totalDelayedFree ==  MAX_DELAYED_FREE)
		CommitDelayedFree();

	memset(block, DELAYED_FREE_PAINTING, size);
	delayedFree[totalDelayedFree].block = (util_ui8*) block;
	delayedFree[totalDelayedFree].size = size;
}

//*************************

void MemoryCapsule::VerifyDelayed (void) {
	LISTEDIT_THREAD_SAFE();
	util_ui32 n = totalDelayedFree;
	for (DelayedFreeEntry* p = delayedFree; n--; ++p)
		p->CheckPostdisposalOverwrite();
}

#endif	// USE_DELAYED_FREE

/////////////////////////////////////////////////////////////

#ifdef	USE_PERIODIC_MASSIVE_VALIDATION

util_ui32 MemoryCapsule::lastCheckTime = 0;

void DebugMassiveValidation (void) {
	MemoryCapsule::VerifyDelayed();
}

void MemoryCapsule::MassiveValidation (void) {
	LISTEDIT_THREAD_SAFE();
	for (MemoryCapsule* p = head; p; p=p->next)
		p->Validate();
}

//*************************

void MemoryCapsule::VerifyAllPeriodically (void) {

	if (ugettime() - lastCheckTime >= GLOBAL_CHECK_EVERY) {
		MassiveValidation();
		lastCheckTime = ugettime();
	}
}

#endif	// USE_PERIODIC_MASSIVE_VALIDATION

/////////////////////////////////////////////////////////////

MemoryCapsule* MemoryCapsule::GetBlock (void* userAddr)
    { return (MemoryCapsule*) POFFS(userAddr, -((util_i16) sizeof(MemoryCapsule))); }

/////////////////////////////////////////////////////////////
// Disconnect from the block linked list.
//

void MemoryCapsule::DisconnectFromList (void) {

	LISTEDIT_THREAD_SAFE();

	if (this == head)
		head = head->next;

	if (prev) {
		DASSERT(prev->VerifyReplica());
		prev->next = next;
		prev->RefreshReplica();
	}

	if (next) {
		DASSERT(next->VerifyReplica());
		next->prev = prev;
		next->RefreshReplica();
	}

	DASSERT(total);
	--total;
}

//*********************

void MemoryCapsule::ConnectInList (void) {

	LISTEDIT_THREAD_SAFE();

	prev = (MemoryCapsule*) 0;
	next = head;

	if (head) {
		DASSERT(head->VerifyReplica());
		head->prev = this;
		head->RefreshReplica();
	}
	head = this;

	DASSERT(!total || head);
	++total;
	RefreshReplica();
}

/////////////////////////////////////////////////////////////

MemoryCapsule::~MemoryCapsule() {

	DisconnectFromList();

#ifndef	USE_DELAYED_FREE
	DMemoryManager::ReleaseMemory(POFFS(this, -HEADER_BYTES));
#else
	FreeByDelay(POFFS(this, -HEADER_BYTES), AllocationSize(userSize));
#endif
}

//***********************

void MemoryCapsule::Destroy (void) {
	this->MemoryCapsule::~MemoryCapsule();
}

const std::string MemoryCapsule::MakeContextString (
		const char* file, 
		const char* expr, 
		util_ui16	line,
		util_ui32	userSize
	) {
	LISTEDIT_THREAD_SAFE();
	static char buffer[1024];
	sprintf(buffer, "%s:%d,%s:%d", file, line, expr, userSize);
	DASSERT(strlen(buffer) < uarraysize(buffer));
	return buffer;
}

/////////////////////////////////////////////////////////////

MemoryCapsule::MemoryCapsule (	
		const char* file, 
		const char* expr, 
		util_ui16	_line,
		util_ui32	_size,
		void*		_userAddr
	) {

	DASSERT(_line <= MAX_SOURCE_LINES && _size && _userAddr);
	DASSERT(this == GetBlock(_userAddr));

	userSize = _size;
	userAddr = _userAddr;

	// Paint user memory with a standard byte pattern.
	// This will help recognise parts that are written.
	//
	memset(userAddr, USERMEM_ALLOC_PAINTING, userSize);

	// Padd context information with a standard byte pattern to recognise easily
	// where context ends.
	//
	memset(context, CONTEXT_CHAR_PADDING, sizeof(context));
	ustrnmaxcpy(context, MakeContextString(file, expr, _line, userSize).c_str(), CONTEXT_SIZE);

	// Can we still get the memory capsule from user address?
	//
	DASSERT((void*) this == GetBlock(userAddr));

	ConnectInList();
}

/////////////////////////////////////////////////////////////

void MemoryCapsule::RefreshReplica (void) { 
	memcpy(GetReplicaAddress(), this, sizeof(MemoryCapsule));
}

/////////////////////////////////////////////////////////////
// Verifies the copy is the same with the current instance
// content.
//
bool MemoryCapsule::VerifyReplica (void) const {
	return !memcmp(GetReplicaAddress(), this, sizeof(MemoryCapsule));
}

/////////////////////////////////////////////////////////////

void MemoryCapsule::Validate (void) const {

	LISTEDIT_THREAD_SAFE();
	DASSERT(prev != next || (!next && !prev)); // That's to detect post disposal wipe out.
	DASSERT(VerifyHeader() &&  VerifyTrailer() &&  VerifyReplica());
	if (prev)
		DASSERT(prev->next == this);
	if (next)
		DASSERT(next->prev == this);

#if	defined(USE_DELAYED_FREE) && defined(APPLY_PERIODIC_MASSIVE_VALIDATION)
	VerifyDelayed();
#endif

#if	defined(USE_PERIODIC_MASSIVE_VALIDATION) && defined(APPLY_PERIODIC_MASSIVE_VALIDATION)
	VerifyAllPeriodically();
#endif
}

//*************************

bool MemoryCapsule::VerifyHeader (void) const
    { return !memcmp(POFFS(this, -HEADER_BYTES), HEADER_PATTERN, HEADER_BYTES); }

//*************************

bool MemoryCapsule::VerifyTrailer (void) const {
    return	!memcmp(
				POFFS(
					userAddr,
					GetSize() + sizeof(MemoryCapsule)
				),
				TRAILER_PATTERN,
				TRAILER_BYTES
			);
}

/////////////////////////////////////////////////////////////

util_ui32 MemoryCapsule::AllocationSize (util_ui32 userSize) {
	return	2 * sizeof(MemoryCapsule) +		// Includes the defensive copy.
			HEADER_BYTES +
			TRAILER_BYTES +
			userSize;
}

//**************************

MemoryCapsule* MemoryCapsule::Construct (
		const char*	file, 
		const char* expr, 
		util_ui16	line, 
		util_ui32	size
	) {

	void* mem = DMemoryManager::GetMemory(MemoryCapsule::AllocationSize(size));
	if (!mem)
		return (MemoryCapsule*) 0;

	// Write header pattern.
	//
	memcpy(mem, HEADER_PATTERN, HEADER_BYTES);
	PINC(mem, HEADER_BYTES);

	// Move to user memory, 'p', and capsule memory, 'mem'.
	//
	void* p = POFFS(mem, sizeof(MemoryCapsule));
	new (mem) MemoryCapsule (file, expr, line, size, p);

	// Move to trailer.
	//
	PINC(p, size + sizeof(MemoryCapsule));
	memcpy(p, TRAILER_PATTERN, TRAILER_BYTES);

	return (MemoryCapsule*) mem;
}

/////////////////////////////////////////////////////////////

void MemoryCapsule::ReportMemoryLeaks (void (*f)(const char*)) {
	for (MemoryCapsule* p = head; p; p = p->next)
		f(uconstructstr("Memory leak 0x%x, size %d (4 included for arrays), '%s'.\n", p->userAddr, p->userSize, p->context).c_str());
}

void MemoryCapsule::Initialise (void) {
#ifdef	USE_DELAYED_FREE
	memset(delayedFree, 0, sizeof(delayedFree));
	totalDelayedFree = 0;
#endif
	blockListMutex = new (malloc(sizeof(umutex))) umutex;
}

void MemoryCapsule::CleanUp (void) {
#ifdef	USE_DELAYED_FREE
	CommitDelayedFree();
#endif
	free(blockListMutex);
	unullify(blockListMutex);
}

/////////////////////////////////////////////////////////////