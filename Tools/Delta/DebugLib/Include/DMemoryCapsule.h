// DMemoryCapsule.h.
// Memory allocation block with defensive information.
// Defensive programming library.
// A. Savidis,
// Updated: January 2005.
//

#ifndef	DMEMORYCAPSULE_H
#define	DMEMORYCAPSULE_H

#include "utypes.h"
#include "uthreadlib.h"

//	#define	USE_DELAYED_FREE					// To free memory with a delay.
//	#define	USE_PERIODIC_MASSIVE_VALIDATION		// To periodically verify all pointers.

#ifdef	USE_PERIODIC_MASSIVE_VALIDATION
#define	GLOBAL_CHECK_EVERY	150				// Change this value as needed.
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
#define	DELAYED_FREE_PAINTING			'7'

#define	CONTEXT_SIZE \
		FILE_CONTEXT_SIZE + \
		EXPR_CONTEXT_SIZE + \
		LINE_INFO_SIZE + \
		SIZEINFO_SIZE + 1

#define HEADER_BYTES 		8
#define TRAILER_BYTES 		8
#define	HEADER_PATTERN		"12345678"
#define	TRAILER_PATTERN		"87654321"

#define	MAX_DELAYED_FREE	1024

////////////////////////////////////////////////////////
// The layout of a memory capsule is as follows:
// |HEADER|MemoryCapsule|user memory|MemoryCapsule copy|TRAILER|

class MemoryCapsule {
	
	////////////////////////////////////////////////////////

	private:
	void*					userAddr;				// User address.
	char					context[CONTEXT_SIZE];	// File, line and expression.
	util_ui32				userSize;				// How many bytes allocated.

	static umutex*			blockListMutex;
	MemoryCapsule*			next;					// Double linked list.
	MemoryCapsule*			prev;

	static MemoryCapsule*	head;					// Block list head.
	static util_ui32		total;					// Total allocated blocks.

#ifdef	USE_DELAYED_FREE

	struct DelayedFreeEntry {

		util_ui8*		block;
		util_ui32		size;

		void CheckPostdisposalOverwrite (void) {
			util_ui8*	p = block;
			util_ui32	n = size;
			while (n--)
				DASSERT(*p++ == DELAYED_FREE_PAINTING);
		}
	};

	static DelayedFreeEntry	delayedFree[MAX_DELAYED_FREE];
	static util_ui32		totalDelayedFree;
	static void				FreeByDelay (void* p, util_ui32 n);
	static void				CommitDelayedFree (void);
	static void				VerifyDelayed (void);

#endif

	//****************************

#ifdef	USE_PERIODIC_MASSIVE_VALIDATION
	friend void DebugMassiveValidation (void);
	static unsigned lastCheckTime;
	static void		MassiveValidation (void);
	static void		VerifyAllPeriodically (void);
#endif

	//****************************

	bool			VerifyHeader (void) const;
	bool			VerifyTrailer (void) const;

	void*			GetReplicaAddress (void) const	// At the end of the block.
						{ return POFFS(userAddr, GetSize()) ;}

	void			RefreshReplica (void);
	bool			VerifyReplica (void) const;

	void			DisconnectFromList (void);
	void			ConnectInList (void);

	////////////////////////////////////////////////////////

	public:
	static util_ui32		AllocationSize (util_ui32 userSize);
	static MemoryCapsule*	Construct (const char* file, const char* expr, util_ui16 line, util_ui32 userSize);
	static void				Initialise (void);
	static void				CleanUp (void);

	void*					GetUserAddr (void) { return userAddr; }
	util_ui32				GetSize (void) const { return userSize; }
	const char*				GetContext (void) { return context; }
	void					Destroy (void);
	static void				ReportMemoryLeaks (void (*f)(const char*));

	//**********************

	static MemoryCapsule*	GetBlock (void* userAddr);
	void					Validate (void) const;

	MemoryCapsule (
		const char* file, 
		const char* expr, 
		util_ui16	line,
		util_ui32	userSize,
		void*		userAddr
	);

	~MemoryCapsule();
};
//------------------------------------------------------

#endif	// Do not add stuff beyond this point.