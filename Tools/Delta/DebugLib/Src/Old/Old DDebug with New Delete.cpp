// DDebug.cpp
// Debug library using the new memory manager.
// A. Savidis, November 2002.
//

#include <assert.h>
#include "ddebug.h"
#include "uthreadlib.h"
#include "usystem.h"
#include "DMemoryCapsule.h"

//	#define	DDEBUG_DISMISS

#ifdef	DDEBUG_DISMISS

#include "DMemoryManager.h"

void nop (const char* unused);

void dclose(void){
	DSmallAllocatorSuper::CleanUp();
	DMemoryManager::CleanUp();
}

void dinit (dmsgcallback){
	DMemoryManager::Initialise();
}

void dcontext (char*, unsigned short, char*) {}
void dstartlogging (const char*){}
void dstoplogging (void){}
void drealign (void*) {}
void dassert (bool){}
void dlatedisposalon (util_ui32){}
void dlatedisposaloff (void){}
void dcheckptr (void*){}
void* dprivatemalloc (size_t n){ return DMemoryManager::GetMemory(n); }
void dprivatefree (void* p) { DMemoryManager::ReleaseMemory(p); }
void dseterrorcallback (dmsgcallback){}

#ifdef	DDEBUG_NEWDELETE 
void*	operator	new (size_t n)		{ return DMemoryManager::GetMemory(n); }
void	operator	delete (void* p)	{ DMemoryManager::ReleaseMemory(p); }
void*	operator	new[] (size_t n)	{ return DMemoryManager::GetMemory(n); }
void	operator	delete[] (void* p)	{	DMemoryManager::ReleaseMemory(p); }
#endif

#else

////////////////////////////////////////////////////////////

#ifdef	ALADIN_LINKAGE
#undef	ALADIN_LINKAGE
#endif

#ifdef	ALADIN_LINKAGE

#include "aladin_api.h"

void dassert (bool cond) {

	if (ALADIN_GetVideoMode()==VideoMode_FullScreen)
		ALADIN_Assert(cond);
	else
		assert(cond);
}

#else

#include <assert.h>
void dassert (bool cond) { 
	if (!cond)
		assert(false);
	assert(cond); 
}

#endif

////////////////////////////////////////////////////////////

#ifdef	DDEBUG_USE

//-----------------------------------------------------------

#include "dmemorymanager.h"
#include "dallocationstats.h"
#include "utypes.h"
#include "ustrings.h"
#include "uptr.h"
#include "uarraydoublelist.h"
#include "uhash.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define	MEMORY_LEAK_CHECK
//	#define	DRECORD_ALLOCATIONSTATS

//-----------------------------------------------------------
// Context information for 'new' and 'delete' operations.
//

#define	MSG_BUFFER_SIZE				256
#define	EXPR_UNKNOWN				"<unknown>"

static void ddefaulterror (const char* msg) {
	fprintf(stderr,"%s", msg);
	fflush(stderr);
}

// Variables are initialised in the static initialisation safe
// initializer near the end of th file.
//
static dmsgcallback msgCallback;
static char			errorMsg[MSG_BUFFER_SIZE];
static char			buffer[MSG_BUFFER_SIZE];
static char			file[FILE_CONTEXT_SIZE];
static util_ui32	line;
static char			expr[EXPR_CONTEXT_SIZE];
static util_ui32	totalAllocedBytes;
static util_ui32	totalFreedBytes;

static void dcheckinit (void);

//////////////////////////////////////

char*		dfile (void) { return file; }

char*		dexpr (void) { return expr; }

util_ui16	dline (void) { return line; }

//////////////////////////////////////
// Set the error reporting callback.
//
void dseterrorcallback (dmsgcallback msgFunc) {
	msgCallback=msgFunc;
}

//////////////////////////////////////
// Copy from src to dest, at most maxLen bytes (including null
// terminator). Hence, pure bytes maxLen-1 written at most.
// Copies from end to start.
//
void dcopytruncate (char* dest, char* src, util_ui16 maxLen) {

	util_ui16 len = strlen(src);
	strcpy(dest, src + (len < maxLen? 0 : len+1 - maxLen));
}

//////////////////////////////////////
// Truncate file name, if it is to large.
//

void dcontext (char* _file, util_ui16 _line, char* _expr) {

	// The first entry point is here.
	//
	line = _line;
	dcopytruncate(file, _file, FILE_CONTEXT_SIZE);
	if (_expr)
		dcopytruncate(expr, _expr, EXPR_CONTEXT_SIZE);
	else
		expr[0]='\0';
}

//////////////////////////////////////

bool dcontextnotempty (void) {
	return file[0] || expr[0] || line;
}

//////////////////////////////////////
// Resets context.
//
void dresetcontext (void) {
	file[0] = expr[0] = '\0';
	line = 0;
}

//-----------------------------------------------------------
// Maintainance of block size allocation statistics.
//

#ifdef	DRECORD_ALLOCATIONSTATS

#define	ALLOCATIONSTAT_MAX	1024
#define	ALLOCATIONSTAT_FILE	"dallocstats.txt"

static AllocationStats* allocationStats;

#define	DINITALLOCSTATS()			allocationStats = new (DMemoryManager::GetMemory(sizeof(AllocationStats))) AllocationStats
#define	DRECORDALLOCSIZE(size)		DASSERT(allocationStats); allocationStats->Record(size)
#define	DREPORTALLOCSTATS()			DASSERT(allocationStats); allocationStats->Report(ALLOCATIONSTAT_FILE)
#define	DCLEANALLOCSTATS()			DASSERT(allocationStats); allocationStats->~AllocationStats(); DMemoryManager::ReleaseMemory(allocationStats)

#else

#define	DINITALLOCSTATS()
#define	DRECORDALLOCSIZE(size)
#define	DREPORTALLOCSTATS()
#define	DCLEANALLOCSTATS()

#endif	// DRECORD_ALLOCATIONSTATS

void dstartlogging (const char* path) {
#ifdef	DHAS_THREADS
	MemoryCapsule::StartLogging(path);
#endif
}

void dstoplogging (void) {
#ifdef	DHAS_THREADS
	MemoryCapsule::StopLogging();
#endif
}

//-----------------------------------------------------------
// Bug defense monitors.
//

void dmonitorcontent (void* p, util_ui32 period) {

#ifdef	DHAS_THREADS
	MemoryCapsule* block = (MemoryCapsule*) MemoryCapsule::GetBlock(p);
	DASSERT(block->Validate());
	block->GetMonitors()->StartContentMonitor(period);
#endif
}

void dunmonitorcontent (void* p) {

#ifdef	DHAS_THREADS
	MemoryCapsule* block = (MemoryCapsule*) MemoryCapsule::GetBlock(p);
	DASSERT(block->Validate());
	block->GetMonitors()->StopContentMonitor();
#endif
}

//****************************

void dmonitorvalidity (void* p, util_ui32 period) {

#ifdef	DHAS_THREADS
	MemoryCapsule* block = (MemoryCapsule*) MemoryCapsule::GetBlock(p);
	DASSERT(block->Validate());
	block->GetMonitors()->StartValidityMonitor(period);
#endif
}

void dunmonitorvalidity (void* p) {

#ifdef	DHAS_THREADS
	MemoryCapsule* block = (MemoryCapsule*) MemoryCapsule::GetBlock(p);
	DASSERT(block->Validate());
	block->GetMonitors()->StopValidityMonitor();
#endif
}

//****************************

void dmonitorexpiration (void* p, util_ui32 delay) {

#ifdef	DHAS_THREADS
	MemoryCapsule* block = (MemoryCapsule*) MemoryCapsule::GetBlock(p);
	DASSERT(block->Validate());
	block->GetMonitors()->StartExpirationMonitor(delay);
#endif
}

void dunmonitorexpiration (void* p) {

#ifdef	DHAS_THREADS
	MemoryCapsule* block = (MemoryCapsule*) MemoryCapsule::GetBlock(p);
	DASSERT(block->Validate());
	block->GetMonitors()->StopExpirationMonitor();
#endif
}

void dlatedisposalon (util_ui32 delay) {
#ifdef	DHAS_THREADS
	MemoryCapsule::EnableLateDisposals(delay);
#endif
}

void dlatedisposaloff (void) {
#ifdef	DHAS_THREADS
	MemoryCapsule::DisableLateDisposals();
#endif
}

//-----------------------------------------------------------

// Check if 'ptr' is a legal user address.
//
void dcheckptr (void* ptr) {

	MemoryCapsule* block = (MemoryCapsule*) MemoryCapsule::GetBlock(ptr);
	DASSERT(block->Validate());
#ifdef	DHAS_THREADS
	DASSERT(!block->IsLateDisposalActive());
#endif
}

//////////////////////////////////

size_t dprivatememsize (void* ptr) {

	MemoryCapsule* block = (MemoryCapsule*) MemoryCapsule::GetBlock(ptr);
	DASSERT(block->Validate());
	return block->GetSize();
}

//////////////////////////////////
// Check if 'ptr' is a legal user pointer with
// size 'size'.
//
void dchecksize (void* p, util_ui32 size) {
	DASSERT(dprivatememsize(p)==size);
}

//////////////////////////////////
// We keep 'new' memory blocks in a stack. The stack
// is to be overflowed if the max number of open 'new'
// due to a single call goes above this. 
//
#define NEW_STACK_SIZE	512
static	MemoryCapsule*	newStack[NEW_STACK_SIZE];
static	util_ui16		newTop = 0;

// Here, the last 'new'  is checked for offset change
// of the user returned pointer. If something like this
// is detected, pointer memory becomes:
// |memoryblock|1byte(nooffset)||3bytes(0)|(1byte(hasoffset)|4bytes(C++)|usermemory|.
//
void drealign (void* uptrNew) {

	DASSERT(newTop);
	MemoryCapsule* lastNew = newStack[--newTop];
	DASSERT(lastNew);

	lastNew->Retarget(uptrNew);
}

//////////////////////////////////

// This is the main memory allocation function. 
// The pointer is created as follows:
// |memoryblock|1byte(nooffset)|4bytes(0)|usermemory|
//

void* dprivatemalloc (size_t size) {

	DRECORDALLOCSIZE(size);
	MemoryCapsule* entry = MemoryCapsule::Construct(dfile(), dexpr(), dline(), size);

	// Stack this block for offset change, only if the current
	// context is not empty, meaning it has been called via a DNEW.
	//
	if (dcontextnotempty()) {

		newStack[newTop++] = entry;
		DASSERT(newTop < NEW_STACK_SIZE);

		dresetcontext();
	}

	totalAllocedBytes += size;
	return entry->GetUserAddr();
}

//////////////////////////////////

static void dcheckdeleteinsidenew (MemoryCapsule* block) {

	util_ui16 i = newTop;

	while (i)
		if (newStack[--i] == block)
			DASSERT(!"'delete' called, while 'new' not returned!");
}

//////////////////////////////////
// Checks pointer. Removes from the list of allocated
// blocks. Frees allocated memory. Cleans-up the headers.
//
#if	!defined(DDEBUG_NEWDELETE) && defined(DDEBUG_DEFENSE)
void dprivatefree (void* ptr, bool array)
#else
void dprivatefree (void* ptr, bool array = false)
#endif
 {
	
	dcheckptr(ptr);
	MemoryCapsule* block = (MemoryCapsule*) MemoryCapsule::GetBlock(ptr);
	DASSERT(block->Validate());

	if (!array && block->KnownAsArray())
		DASSERT(!"Allocated via new[], but not disposed via delete[]!");

	dcheckdeleteinsidenew(block);				// We delete while new not returned ?
	totalFreedBytes += block->GetAllocationSize();

	block->Dispose();							// Ask the block to dispose itself.
}

//////////////////////////////////
// Memory leak reporting.
//

static void dmemoryleak (const char* msg) 
	{ (*msgCallback)(errorMsg); }

//-----------------------------------------------------------

// We want to distinguish among a DNEW, that records context, a 
// a simple 'new'. In the latter case, the dprivatemalloc should not 
// stack the allocated block. To do this, we take advantage of the following:
//
// A 'new' is performed by the compiler by firstly calling
// the allocation function (i.e. the one overloaded), and then
// calling the constructors of the items created via new.
// As a result, whatever the caller, after a 'new' we reset
// the context, to ensure that subsequent calls, due to
// constructors, unless using the header file macros,
// will find context empty.
//

//////////////////////////////////

#ifdef	DDEBUG_NEWDELETE 

// The only way that a dcheckinit() may be executed
// is that a non DNEW() allocation is entered. Else,
// the dcontext() initialisation will precede.
//
void*  operator	new (size_t n) {
	return dprivatemalloc(n);
}

//********************

void operator delete (void* p) {

	DASSERT(!dcontextnotempty());
	dprivatefree(p);
}

//********************

void*  operator	new[] (size_t n) {
	return dprivatemalloc(n);
}

//********************

void  operator delete[] (void* p) {

	DASSERT(!dcontextnotempty());
	dprivatefree(p, true);
}

#endif	// DDEBUG_NEWDELETE

//-----------------------------------------------------------

void dinit (dmsgcallback f) {

	totalAllocedBytes	= 0;
	totalFreedBytes		= 0;
	msgCallback			= f;

	dresetcontext();
	DMemoryManager::Initialise();
	DINITALLOCSTATS();

	MemoryCapsule::Initialise();
}

// Close down by simply reporting memory leaks.
// Check if the hast-tables are still valid.
// Also check ptr still registered.
//
void dclose (void) {

	DREPORTALLOCSTATS();
	DCLEANALLOCSTATS();

	sprintf(
		errorMsg,
		"Total '%d' bytes allocated.\nTotal '%d' bytes freed.\n",
		totalAllocedBytes,
		totalFreedBytes
	);

	(*msgCallback)(errorMsg);
#ifdef	DHAS_THREADS
	MemoryCapsule::ReportBlocks(msgCallback);
#endif
	MemoryCapsule::CleanUp();
	DSmallAllocatorSuper::CleanUp();
	DMemoryManager::CleanUp();
} 

//-----------------------------------------------------------

#endif	// DDEBUG_USE

#endif	// DDEBUG_DISMISS