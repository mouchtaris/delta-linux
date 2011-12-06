// DDebug.cpp
// Debug library for pointer content defense, checking
// various situations.
// A. Savidis, last update on January 2005.
// Original version November 2000.
//

//----------------------------------------------------------

#include <assert.h>
#include <stdlib.h>
#include "DDebug.h"

#ifdef	DDEBUG_USE

#include "usystem.h"
#include "DMemoryCapsule.h"
#include <map>

//	#define	DDEBUG_DISMISS

//----------------------------------------------------------
// If we dismiss the debug usage, we simply have no
// checks. Only size information.
//

#define	NO_MEMORY_ACTION()												\
	if (true) {															\
		umodalmessage(													\
			"Out of memory!",											\
			"The last allocation caused the system to reach\n"			\
			"its memory limits. The application will exit\n"			\
			"immediately (data in main memory are lost)."				\
		);																\
		exit(0);														\
	} else

#ifdef	DDEBUG_DISMISS

#include <malloc.h>

DDEBUG_FUNC void	dassert (bool cond, const char* = (char*) 0) { assert(cond); }
DDEBUG_FUNC void	dclose(void) {}
DDEBUG_FUNC void	dinit (dmsgcallback){}
DDEBUG_FUNC void	dcontext (const char*, unsigned short, const char*) {}
DDEBUG_FUNC void	dcheckptr (void*){}
DDEBUG_FUNC void	dseterrorcallback (dmsgcallback){}
DDEBUG_FUNC void	dsetassertfunc(dassertercallback){}
DDEBUG_FUNC void	dsetassertcleaner(uvoidvoid_f){}

DDEBUG_FUNC	dassertercallback	dgetassertfunc(void)    { return (dassertercallback) 0; }
DDEBUG_FUNC	uvoidvoid_f         dgetassertcleaner(void) { return (uvoidvoid_f) 0; }

DDEBUG_FUNC void* dprivatemalloc (size_t n) {
	void* mem = malloc(DDEBUG_USERSIZE_BYTES + n);
	if (!mem)
		NO_MEMORY_ACTION();
	*((util_ui32*) mem) = n;
	return POFFS(mem, DDEBUG_USERSIZE_BYTES);
}

DDEBUG_FUNC void dprivatefree (void* p, bool)
	{ free(POFFS(p, -DDEBUG_USERSIZE_BYTES)); }

DDEBUG_FUNC void dchecksize (void* p, util_ui32 size)
	{ dassert(dprivatememsize(p) == size); }

DDEBUG_FUNC util_ui32 dprivatememsize (void* p) 
	{ return *((util_ui32*) POFFS(p, -DDEBUG_USERSIZE_BYTES)); }

#else

//-----------------------------------------------------------

#include "DMemoryManager.h"
#include "DAllocationStats.h"
#include "utypes.h"
#include "ustrings.h"
#include "uptr.h"
#include "uarraydoublelist.h"
#include "uhash.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define	MEMORY_LEAK_CHECK
#define	DRECORD_ALLOCATIONSTATS

//-----------------------------------------------------------

// Variables are initialised in the static initialisation safe
// initializer near the end of the file.
//
static char			file[FILE_CONTEXT_SIZE];
static util_ui32	line					= 0;
static char			expr[EXPR_CONTEXT_SIZE + CONTEXT_SERIAL_SIZE];
static util_ui32	totalAllocedBytes		= 0;
static util_ui32	totalFreedBytes			= 0;
static util_ui32	initCounter				= 0;

//////////////////////////////////////

static void ddefaulterror (const char* msg) 
	{ fprintf(stderr,"%s", msg); fflush(stderr); }

static void	ddefaultassert (bool val, const char* expr, uvoidvoid_f cleaner)
    { (*cleaner)(); assert(val); }

static void ddefaultcleaner (void) 
	{}

static dassertercallback	dasserter		= &ddefaultassert;
static uvoidvoid_f			dcleaner		= &ddefaultcleaner;
static dmsgcallback			derrorreporter	= &ddefaulterror;

DDEBUG_FUNC void dsetassertfunc (dassertercallback f) 
	{ if (f) dasserter = f; else dasserter = &ddefaultassert; }

DDEBUG_FUNC void dsetassertcleaner(uvoidvoid_f f) 
	{ if (f) dcleaner = f; else dcleaner = &ddefaultcleaner; }

DDEBUG_FUNC	dassertercallback dgetassertfunc(void)    
	{ return dasserter; }

DDEBUG_FUNC	uvoidvoid_f dgetassertcleaner(void) 
	{ return dcleaner; }

DDEBUG_FUNC void dassert (bool cond, const char* expr)
	{ (*dasserter)(cond, expr, dcleaner); }

DDEBUG_FUNC void dseterrorcallback (dmsgcallback msgFunc) 
	{ if (msgFunc) derrorreporter = msgFunc; else derrorreporter = &ddefaulterror; }

//////////////////////////////////////

static char*		dfile (void) { return file; }
static char*		dexpr (void) { return expr; }
static util_ui16	dline (void) { return line; }

//////////////////////////////////////
// Copy from src to dest, at most maxLen bytes (including null
// terminator). Hence, pure bytes maxLen-1 written at most.
// Copies from end to start.
//
static void dcopytruncate (char* dest, const char* src, util_ui16 maxLen) {
	util_ui16 len = (util_ui16) strlen(src);
	strcpy(dest, src + (len < maxLen? 0 : len+1 - maxLen));
}

//////////////////////////////////////
// Truncate file name and expr, if it is too large.
//

typedef std::map<std::string, util_ui32> ContextMap;
static ContextMap* contextMap = (ContextMap*) 0;

DDEBUG_FUNC void dcontext (const char* _file, util_ui16 _line, const char* _expr) {

	// The first entry point is here.
	//
	line = _line;
	dcopytruncate(file, _file, FILE_CONTEXT_SIZE);
	if (_expr)
		dcopytruncate(expr, _expr, EXPR_CONTEXT_SIZE);
	else
		expr[0] = '\0';

}

//////////////////////////////////////
// Resets context.
//
static void dresetcontext (void) {
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

#define	DINITALLOCSTATS()	\
		allocationStats = new (DMemoryManager::GetMemory(sizeof(AllocationStats))) AllocationStats

#define	DRECORDALLOCSIZE(size)	\
		DASSERT(allocationStats); \
		allocationStats->Record((util_ui32) size)

#define	DREPORTALLOCSTATS()	\
		DASSERT(allocationStats); \
		allocationStats->Report(ALLOCATIONSTAT_FILE)

#define	DCLEANALLOCSTATS()	\
		DASSERT(allocationStats); \
		allocationStats->~AllocationStats(); \
		DMemoryManager::ReleaseMemory(allocationStats)

#else

#define	DINITALLOCSTATS()
#define	DRECORDALLOCSIZE(size)
#define	DREPORTALLOCSTATS()
#define	DCLEANALLOCSTATS()

#endif	// DRECORD_ALLOCATIONSTATS

//-----------------------------------------------------------
// Check if 'ptr' is a legal un-corrupted user address.
// Small objects are not checked.
//
DDEBUG_FUNC void dcheckptr (void* ptr) {
#ifdef	DDEBUG_SMALLALLOCATOR
		if (dsmallobjectflagraised(mem))
			return;	// Passed test.
#endif
	MemoryCapsule* block = (MemoryCapsule*) MemoryCapsule::GetBlock(ptr);
	block->Validate();
}

//////////////////////////////////
// For small objects we never check their size!.
//
DDEBUG_FUNC size_t dprivatememsize (void* ptr) {

#ifdef	DDEBUG_SMALLALLOCATOR
		if (dsmallobjectflagraised(ptr)) {
			util_ui32 size = *((util_ui32*) ptr;
			dclearsmallobjectflag(&size);
			return size;
		}
#endif

	MemoryCapsule* block = (MemoryCapsule*) MemoryCapsule::GetBlock(ptr);
	block->Validate();

	return block->GetSize();
}

//////////////////////////////////
// Check if 'ptr' is a legal un-corrupted user pointer with
// size 'size'.
//
DDEBUG_FUNC void dchecksize (void* p, util_ui32 size) 
	{ DASSERT(dprivatememsize(p) == size); }

//////////////////////////////////

DDEBUG_FUNC void* dprivatemalloc (size_t size) {

	DASSERT(size <= DDEBUG_MAXALLOCATION_SIZE);

	std::string				context = MemoryCapsule::MakeContextString(dfile(), dexpr(), dline(), (util_ui32) size);
	ContextMap::iterator	i		= contextMap->find(context);
	util_ui32				sn		= i == contextMap->end() ? 0 : i->second + 1;

	if (sn == CONTEXT_SERIAL_MAX)
		sn = 1;
	if (!sn)
		(*contextMap)[context] = 1;
	else
		i->second = sn;

	sprintf(expr, "%s:%x", std::string(dexpr()).c_str(), sn);
	assert(strlen(expr) < sizeof(expr));

	DRECORDALLOCSIZE(size);
	MemoryCapsule* entry = MemoryCapsule::Construct(dfile(), dexpr(), dline(), (util_ui32) size);
	if (!entry) {
		NO_MEMORY_ACTION();
	}

	totalAllocedBytes += (util_ui32) size;

	return entry->GetUserAddr();
}

//////////////////////////////////
// Checks pointer.
//
DDEBUG_FUNC void dprivatefree (void* ptr, bool array) {
	
	dcheckptr(ptr);
	MemoryCapsule* block = (MemoryCapsule*) MemoryCapsule::GetBlock(ptr);
	block->Validate();

	if (!array && *((util_ui32*) ptr) !=0)
		DASSERT(!"Allocated via new[], but not disposed via delete[]!");

	totalFreedBytes += block->GetSize();

	block->Destroy();	// Ask the block to dispose itself.
}

//////////////////////////////////

DDEBUG_FUNC void dinit (dmsgcallback f) {

	derrorreporter	= f;

	if (!initCounter++) {
		totalAllocedBytes	= 0;
		totalFreedBytes		= 0;

		dresetcontext();
		DMemoryManager::Initialise();
		DINITALLOCSTATS();

		MemoryCapsule::Initialise();
		contextMap = new ContextMap;
	}
}

// Close down by simply reporting memory leaks.
// Check if the hast-tables are still valid.
// Also check ptr still registered.
//
DDEBUG_FUNC void dclose (void) {

	DASSERT(initCounter);

	if (!--initCounter) {

		DREPORTALLOCSTATS();
		DCLEANALLOCSTATS();

		(*derrorreporter)(
			uconstructstr("Total '%d' bytes allocated.\nTotal '%d' bytes freed.\n",
				totalAllocedBytes,
				totalFreedBytes
			).c_str()
		);

	#ifdef	MEMORY_LEAK_CHECK
		MemoryCapsule::ReportMemoryLeaks(derrorreporter);
	#endif

		MemoryCapsule::CleanUp();
		DSmallAllocatorSuper::CleanUp();
		DMemoryManager::CleanUp();

		delete contextMap;
		contextMap = (ContextMap*) 0;
	}
} 

//-----------------------------------------------------------

#endif	// DDEBUG_DISMISS

#endif	// DDEBUG_USE
