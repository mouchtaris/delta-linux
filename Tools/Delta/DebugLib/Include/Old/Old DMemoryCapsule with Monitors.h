// DMemoryCapsule.h. WORK IN PROGRESS.
// Memory allocation block with defensive information.
// Defensive programming library.
// A. Savidis,
// Updated July 2004.
// TODO: OPTIMISE A LOT. Especially threads.
//

#ifndef	DMemoryCapsule_H
#define	DMemoryCapsule_H

#include "utypes.h"
#ifdef	DHAS_THREADS
#include "DMonitors.h"
#include "uthreadlib.h"
#endif

//------------------------------------------------------
// Context information string lengths.
//
#define	FILE_CONTEXT_SIZE				16
#define	EXPR_CONTEXT_SIZE				16
#define	LINE_INFO_SIZE					5	// At most 9999 lines per file.
#define	SIZEINFO_SIZE					9	// At most 999999999 bytes
#define MAX_SOURCE_LINES 				9999
#define CONTEXT_CHAR_PADDING			'*'
#define USERMEM_ALLOC_PAINTING			'!'

#define	CONTEXT_SIZE \
		FILE_CONTEXT_SIZE + \
		EXPR_CONTEXT_SIZE + \
		LINE_INFO_SIZE + \
		SIZEINFO_SIZE + 1

#define COMPILER_ARRAY_BYTES 	4
#define OFFSETBYTE_OFFSET		-(COMPILER_ARRAY_BYTES + 1)
#define OFFSET_FORARRAYS		9
#define OFFSET_FORNONARRAYS		5

#define HEADER_BYTES 	4
#define TRAILER1_BYTES 	COMPILER_ARRAY_BYTES
#define TRAILER2_BYTES 	4
#define BLOCK_HEADER	170 // 10101010
#define BLOCK_TRAILER1	240 // 11110000
#define BLOCK_TRAILER2	204 // 11001100

////////////////////////////////////////////////////////

#define	IN_DISPOSAL_DISDABLED_STATE_FLAG	0x1
#define	IN_LATE_DISPOSAL_STATE_FLAG			0x2

class MemoryCapsule {
	
	////////////////////////////////////////////////////////

	private:
	void*					userAddr;				// User address.
	char					context[CONTEXT_SIZE];	// File, line and expression.
	util_ui32				allocSize;				// How many bytes allocated.
	util_ui8				stateFlags;				// The memory capsule state flags.

#ifdef	DHAS_THREADS
	DefectMonitors*			monitors;				// The defect monitors.

	static Mutex			blockLinkMutex;
	static MemoryCapsule*	head;					// All memory blocks.
	MemoryCapsule*			next;					// Double linked list.
	MemoryCapsule*			prev;

	//****************************
	// Concurrent logging facilities.
	//
	struct LogEntry {
		util_ui32	timeStamp;		// Exact time.
		util_ui32	n;				// How many bytes.
		bool		isAllocation;	// If false, it is disposal.
		LogEntry*	next;
	};

	static LogEntry*	logHead;
	static LogEntry*	logTail;
	static Thread*		logThread;
	static FILE*		logFile;
	static bool			stopLogging;

	static void			LogFlush (LogEntry* until = (LogEntry*) 0);
	static void			LogThreadFunc (void* unused);
	void				Log (bool isAllocation);

	//****************************
	// Concurrent destruction thread.
	//
	struct DestructionEntry {
		MemoryCapsule*		block;
		DestructionEntry*	next;
	};
	static DestructionEntry* destructionHead;

	static void			DestructionThreadFunc (void* arg);
	static Thread*		destructionThread;
	static Mutex*		destructionMutex;
	static bool			destructionThreadRunning;
	static void			CommitDestructions (void);

	//****************************

	static bool			inLateDisposals;		// If delayed disposal is active.
	static util_ui32	disposalDelay;			// The disposal interval.

#endif	// The above apply only when threads are used.

	//****************************

	bool			VerifyHeader (void) const;
	bool			VerifyTrailer1 (void) const;
	bool			VerifyTrailer2 (void) const;
	bool			VerifyOffsets (void) const;

	void*			GetReplicaAddress (void) const	// At the end of the block.
						{ return POFFS(userAddr, GetSize()) ;}
	void			RefreshReplica (void);
	bool			VerifyReplica (void) const;

	static util_i16	GetOffsetByte (const void* p)
						{ return (util_i16) *((util_ui8*) POFFS(p, OFFSETBYTE_OFFSET)); }

	static void		SetOffsetByte (const void* p, util_ui8 i)
						{ *((util_ui8*) POFFS(p, OFFSETBYTE_OFFSET)) = i; }

	static char		header[HEADER_BYTES];
	static char		trailer1[TRAILER1_BYTES];
	static char		trailer2[TRAILER2_BYTES];
	static char		zeroed[COMPILER_ARRAY_BYTES-1];

	////////////////////////////////////////////////////////

	public:

#ifdef	DHAS_THREADS
	static void				StartLogging (const char* path);
	static void				StopLogging (void);
#endif

	static util_ui32		AllocationSize (util_ui32 userSize);
	static MemoryCapsule*	Construct (const char* file, const char* expr, util_ui16 line, util_ui32 allocSize);

	static void				Initialise (void);
	static void				CleanUp (void);

#ifdef	DHAS_THREADS
	static void				ReportBlocks (void (*f)(const char*));
	DefectMonitors*			GetMonitors (void) { return monitors; }
#endif

	void*			GetUserAddr (void) { return userAddr; }
	bool			KnownAsArray (void) const;
	void			Retarget (void* newUserAddr);
	util_ui32		GetSize (void) const; 
	util_ui32		GetAllocationSize (void) const { return allocSize; }
	const char*		GetContext (void) { return context; }
	void			Destroy (bool directly = false);

	//**********************

	static MemoryCapsule* GetBlock (void* userAddr);

	bool			Validate (void) const;

	void			Dispose (void);

	void			EnableDisposal (void)
						{ stateFlags &= ~IN_DISPOSAL_DISDABLED_STATE_FLAG; RefreshReplica(); }

	void			DisableDisposal (void)
						{ stateFlags |= IN_DISPOSAL_DISDABLED_STATE_FLAG; RefreshReplica(); }

#ifdef	DHAS_THREADS
	bool			IsLateDisposalActive (void) const
						{ return (stateFlags & IN_LATE_DISPOSAL_STATE_FLAG) != 0; }

	static void		EnableLateDisposals (util_ui32 delay) 
						{ inLateDisposals = true; disposalDelay = delay; }
	
	static void		DisableLateDisposals (void) 
						{ inLateDisposals = false; disposalDelay = 0; }
#endif

	MemoryCapsule (
		const char* file, 
		const char* expr, 
		util_ui16	line,
		util_ui32	allocSize,
		void*		userAddr
	);
	~MemoryCapsule();
};
//------------------------------------------------------

#endif	// Do not add stuff beyond this point.