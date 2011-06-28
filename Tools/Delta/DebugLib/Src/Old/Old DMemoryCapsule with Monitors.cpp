// DMemoryCapsule.cpp
// Memory allocation block with defensive information.
// Defensive programming library.
// A. Savidis,
// Updated July 2004.
// TODO: put all block headers in this, making a pure
// defensive memory manager wrapper.
// Small objects will not be put checked here, they will be transparent
// as part of the memory manager.
//

#include <memory.h>

#include "ddebug.h"
#include "ustrings.h"
#include "usystem.h"
#include "DMemoryCapsule.h"
#include "DMonitors.h"

//////////////////////////////////////////////////////////////////

#ifdef	DHAS_THREADS

bool						MemoryCapsule::inLateDisposals = false;
util_ui32					MemoryCapsule::disposalDelay = 0;
MemoryCapsule*				MemoryCapsule::head = (MemoryCapsule*) 0;
Mutex						MemoryCapsule::blockLinkMutex;

#define	LOG_SLEEP
MemoryCapsule::LogEntry*	MemoryCapsule::logHead = (LogEntry*) 0;
MemoryCapsule::LogEntry*	MemoryCapsule::logTail = (LogEntry*) 0;
Thread*						MemoryCapsule::logThread = (Thread*) 0;

MemoryCapsule::DestructionEntry* MemoryCapsule::destructionHead = (DestructionEntry*) 0;
Thread*						MemoryCapsule::destructionThread = (Thread*) 0;
Mutex*						MemoryCapsule::destructionMutex = (Mutex*) 0;
bool						MemoryCapsule::destructionThreadRunning = true;
FILE*						MemoryCapsule::logFile = (FILE*) 0;
bool						MemoryCapsule::stopLogging = false;

#endif

//////////////////////////////////////////////////////////////////

char MemoryCapsule::header[HEADER_BYTES];
char MemoryCapsule::trailer1[TRAILER1_BYTES];
char MemoryCapsule::trailer2[TRAILER2_BYTES];
char MemoryCapsule::zeroed[COMPILER_ARRAY_BYTES-1];

/////////////////////////////////////////////////////////////

MemoryCapsule* MemoryCapsule::GetBlock (void* userAddr)
    { return (MemoryCapsule*) POFFS(POFFS(userAddr, -GetOffsetByte(userAddr)), -sizeof(MemoryCapsule)); }

bool MemoryCapsule::KnownAsArray (void) const 
    { return GetOffsetByte(userAddr) == OFFSET_FORARRAYS; }

/////////////////////////////////////////////////////////////

#ifdef	DHAS_THREADS

void MemoryCapsule::LogThreadFunc (void* unused) {

	while (!stopLogging) {
		uprocesssleep(500);
		LogFlush(logTail);
	}
}

//****************************

void MemoryCapsule::LogFlush (LogEntry* until) {

	while (logHead && logHead != until) {

		fprintf(
			logFile, 
			"%d %s %d\n", 
			logHead->timeStamp, 
			logHead->isAllocation ? "new" : "delete",
			logHead->n
		);

		LogEntry* p = logHead;
		logHead = logHead->next;
		DMemoryManager::ReleaseMemory(p);
	}
}

//****************************

void MemoryCapsule::Log (bool isAllocation) {

	LogEntry* entry		= new (DMemoryManager::GetMemory(sizeof(LogEntry))) LogEntry;
	entry->timeStamp	= ugettime();
	entry->isAllocation = isAllocation;
	entry->n			= allocSize;
	entry->next			= (LogEntry*) 0;

	if (!logTail)
		logHead = logTail = entry;
	else {
		logTail->next = entry;
		logTail = entry;
	}
}

//****************************

void MemoryCapsule::StartLogging (const char* path) {
	logFile = fopen(path, "wt");
	DASSERT(logFile);
	stopLogging = false;
	logThread->Start();
}

//****************************

void MemoryCapsule::StopLogging (void) {

	stopLogging = true;
	logThread->WaitToFinish();
	LogFlush();

	fclose(logFile);
	logFile = (FILE*) 0;
	logHead = logTail = (LogEntry*) 0;
}

#endif

/////////////////////////////////////////////////////////////

util_ui32 MemoryCapsule::GetSize (void) const {
	util_i16 k = GetOffsetByte(userAddr);
	DASSERT(k == OFFSET_FORARRAYS || k == OFFSET_FORNONARRAYS);
	return k == OFFSET_FORARRAYS ? allocSize - COMPILER_ARRAY_BYTES : allocSize;
}

/////////////////////////////////////////////////////////////

#ifdef	DHAS_THREADS

void MemoryCapsule::CommitDestructions (void) {

	LOCK_BLOCK(*destructionMutex);
	while (destructionHead) {

		MemoryCapsule*		block = destructionHead->block;
		DestructionEntry*	del = destructionHead;
		destructionHead = destructionHead->next;
		DMemoryManager::ReleaseMemory(del);

		block->MemoryCapsule::~MemoryCapsule();
	}
}

//***********************

void MemoryCapsule::DestructionThreadFunc (void* arg) {

	while (destructionThreadRunning)
		if (destructionHead)
			CommitDestructions();	
}

#endif

/////////////////////////////////////////////////////////////

MemoryCapsule::~MemoryCapsule() {

#ifdef	DHAS_THREADS

	// We need to ensure the disposal thread
	// has finished, to avoid overwritting disposed
	// memory of its mutex.
	//
	if (monitors->IsDisposalMonitorRunning())
		monitors->WaitDisposalMonitor();

	monitors->DefectMonitors::~DefectMonitors();
	DMemoryManager::ReleaseMemory(monitors);

	// Disconnect from the list of blocks.
	//
	blockLinkMutex.WaitToLock();

	if (this==head)
		head = head->next;

	if (prev) {
		prev->next = next;
		prev->RefreshReplica();
	}

	if (next) {
		next->prev = prev;
		next->RefreshReplica();
	}

	// Now actually release the memory.
	//
	Log(false);	// Destruction.

	blockLinkMutex.UnLock();

#endif

	DMemoryManager::ReleaseMemory(POFFS(this, -HEADER_BYTES));
}

//***********************

void MemoryCapsule::Destroy (bool directly) {

#ifdef	DHAS_THREADS
	if (directly)
		this->MemoryCapsule::~MemoryCapsule();
	else {

		// We have to use a thread to make the destruction
		// to ensure that the destructor returns without
		// disposing its memory. This is necessary since the
		// disposal thread that may have posted this call is
		// not allowed to lead to destruction of its instance.
		//
		destructionMutex->WaitToLock();

		DestructionEntry* entry	= (DestructionEntry*) DMemoryManager::GetMemory(sizeof(DestructionEntry));
		entry->block = this;
		entry->next = destructionHead;
		destructionHead = entry;

		destructionMutex->UnLock();
	}
#else
	this->MemoryCapsule::~MemoryCapsule();
#endif
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

	allocSize = _size;
	userAddr = _userAddr;
	memset(userAddr, USERMEM_ALLOC_PAINTING, allocSize);

	stateFlags = 0;

#ifdef	DHAS_THREADS
	monitors = new (DMemoryManager::GetMemory(sizeof(DefectMonitors))) DefectMonitors(this);
#endif
	memset(context, CONTEXT_CHAR_PADDING, sizeof(context));

	ustrnmaxcpy(
		context, 
		uconstructstr("%s,%d,%s,%d", file, _line, expr, allocSize),
		CONTEXT_SIZE
	);

	DASSERT((void*) this == GetBlock(userAddr));

#ifdef	DHAS_THREADS

	// Now connect in the list of memory blocks.
	//
	prev = (MemoryCapsule*) 0;
	next = head;

	blockLinkMutex.WaitToLock();

	if (head) {
		head->prev = this;
		head->RefreshReplica();
	}
	head = this;

	blockLinkMutex.UnLock();

	Log(true);	// Log the construction.
#endif

	RefreshReplica();
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

bool MemoryCapsule::Validate (void) const {
	return	VerifyHeader()		&&  
			VerifyTrailer1()	&& 
			VerifyTrailer2()	&&
	 		VerifyReplica()		&& 
			VerifyOffsets(); 
}

//*************************

bool MemoryCapsule::VerifyHeader (void) const
    { return !memcmp(POFFS(this, -HEADER_BYTES), header, HEADER_BYTES); }

//*************************

bool MemoryCapsule::VerifyTrailer1 (void) const {
	if (GetOffsetByte(userAddr) == OFFSET_FORARRAYS)
	    return	!memcmp(
					POFFS(userAddr, -COMPILER_ARRAY_BYTES), 
					POFFS(userAddr, GetSize() + sizeof(MemoryCapsule)),
					COMPILER_ARRAY_BYTES
				  );
	else
	    return !memcmp(
					POFFS(userAddr, GetSize() + sizeof(MemoryCapsule)),
					trailer1,
					TRAILER1_BYTES
				);
}

//*************************

bool MemoryCapsule::VerifyTrailer2 (void) const {
    return	!memcmp(
				POFFS(
					userAddr,
					GetSize() + sizeof(MemoryCapsule) + TRAILER1_BYTES
				),
				trailer2,
				TRAILER2_BYTES
			);
}

//*************************

bool MemoryCapsule::VerifyOffsets (void) const {
	util_i16 b = GetOffsetByte(userAddr);
	if (b == OFFSET_FORARRAYS || b == OFFSET_FORNONARRAYS)
		return !memcmp(POFFS(userAddr,-b+1), zeroed,COMPILER_ARRAY_BYTES-1);
    else 
		return false;
}

/////////////////////////////////////////////////////////////

void MemoryCapsule::Dispose (void) {

	DASSERT((stateFlags & IN_DISPOSAL_DISDABLED_STATE_FLAG) == 0);	// Disposal should not be disabled.

#ifdef	DHAS_THREADS
	DASSERT((stateFlags & IN_LATE_DISPOSAL_STATE_FLAG) == 0);		// Should not be in destruction process.
	DASSERT(!monitors->IsDisposalMonitorRunning());					// Should not running disposal monitor.

	if (inLateDisposals) {
		stateFlags |= IN_LATE_DISPOSAL_STATE_FLAG;
		RefreshReplica();
		monitors->StartDisposalMonitor(disposalDelay);
	}
	else
		Destroy(true);
#else
	Destroy(true);
#endif
}

/////////////////////////////////////////////////////////////

util_ui32 MemoryCapsule::AllocationSize (util_ui32 userSize) {
	return	2 * sizeof(MemoryCapsule) + // Includes the defensive copy.
		COMPILER_ARRAY_BYTES+1 +		// Four zeroed bytes plus offset byte. 
		HEADER_BYTES +
		TRAILER1_BYTES +
		TRAILER2_BYTES +
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
	DASSERT(mem);

	memset(mem, BLOCK_HEADER, HEADER_BYTES);
	PINC(mem, HEADER_BYTES);
	void* p = POFFS(mem, sizeof(MemoryCapsule));
	*((util_ui8*) p) = OFFSET_FORNONARRAYS;

	PINC(p, 1);
	memset(p, 0, COMPILER_ARRAY_BYTES);
	PINC(p, COMPILER_ARRAY_BYTES);
	
	new (mem) MemoryCapsule (file, expr, line, size, p);

	PINC(p, size + sizeof(MemoryCapsule));
	memset(p, BLOCK_TRAILER1, TRAILER1_BYTES);
	memset(POFFS(p, TRAILER1_BYTES), BLOCK_TRAILER2, TRAILER2_BYTES);

	return (MemoryCapsule*) mem;
}

/////////////////////////////////////////////////////////////

void MemoryCapsule::Retarget (void* newUserAddr) {

	void* oldUserAddr = userAddr;

	if (newUserAddr != oldUserAddr) {

		// Check that the offset is always the one expected.
		//
		DASSERT(PADDR(newUserAddr) > PADDR(oldUserAddr));
		DASSERT(PDIFF(newUserAddr, oldUserAddr) == COMPILER_ARRAY_BYTES);
		
		// Change the user address to the new one.
		//
		userAddr = newUserAddr;

		// Mark the offset byte for dynamic arrays.
		//
		SetOffsetByte(newUserAddr, OFFSET_FORARRAYS);

		// Preserve the offset byte for non-arrays, zeroing the three bytes that follow it.
		memset(
			POFFS(oldUserAddr, -OFFSET_FORNONARRAYS+1),
			0, 
			COMPILER_ARRAY_BYTES-1
		);

		// Copy the compiler reserved bytes at trailer 1.
		//
		memcpy(
			POFFS(oldUserAddr, allocSize + sizeof(MemoryCapsule)),
			oldUserAddr,
			COMPILER_ARRAY_BYTES
		);

		// Verify that the block is gained correctly from the new user address.
		//
		DASSERT(this == GetBlock(userAddr));

		// Finally refresh the internal copy of block information.
		//
		RefreshReplica();
	}
}

/////////////////////////////////////////////////////////////

#ifdef	DHAS_THREADS

void MemoryCapsule::ReportBlocks (void (*f)(const char*)) {

	while (head) {
		if (!head->IsLateDisposalActive())
			(*f)(uconstructstr("MEMLEAK(%x), %s", head->userAddr, head->context));
		head = head->next;
	}
}

#endif

/////////////////////////////////////////////////////////////

void MemoryCapsule::Initialise (void) {

	memset(header, BLOCK_HEADER, HEADER_BYTES);
	memset(trailer1, BLOCK_TRAILER1, TRAILER1_BYTES);
	memset(trailer2, BLOCK_TRAILER2, TRAILER2_BYTES);
	memset(zeroed, 0, COMPILER_ARRAY_BYTES-1);

#ifdef	DHAS_THREADS
	logThread = new (DMemoryManager::GetMemory(sizeof(Thread))) Thread(LogThreadFunc);
	destructionThread = new (DMemoryManager::GetMemory(sizeof(Thread))) Thread(DestructionThreadFunc);
	destructionMutex = new (DMemoryManager::GetMemory(sizeof(Mutex))) Mutex;
	destructionThreadRunning = true;
	destructionThread->Start((void*) 0);
#endif
}

void MemoryCapsule::CleanUp (void) {

#ifdef	DHAS_THREADS
	destructionThreadRunning = false;
	destructionThread->WaitToFinish();
	DMemoryManager::ReleaseMemory(destructionThread);

	DMemoryManager::ReleaseMemory(destructionMutex);
	DMemoryManager::ReleaseMemory(logThread);
#endif
}

/////////////////////////////////////////////////////////////