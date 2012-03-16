// uthreadlib.h
// uthread library generalisation API for WNT and UNIX.
// Uses opaque native types in the class definitions.
// Originally taken from the 2WEAR Project.
// A. Savidis, September 2003.
//

#ifndef	UTHREADLIB_H
#define	UTHREADLIB_H

#include <utility>
#include <string>
#include "ufunctors.h"
#include "utypes.h"
#include "usystem.h"

//---------------------------------------------------------

class UTILLIB_CLASS umutex {

	private:
	void*		handle;
	util_ui16	locked;
	bool		unlockPending;

	//******************************

	public:
	class  FunctionLocker {
		private:
		umutex* mutex;
		public:
		FunctionLocker (umutex* _mutex) : mutex(_mutex){ if (mutex) mutex->WaitToLock(); }
		FunctionLocker (umutex& _mutex) : mutex(&_mutex){ mutex->WaitToLock(); }
		~FunctionLocker() { if (mutex) mutex->UnLock(); }
	};

	static bool Locker (umutex& _mutex)	
		{  _mutex.WaitToLock(); return true; }

	template <typename T>
	static T& UnlockedCopiedRef (T& val, umutex& _mutex) 
		{  _mutex.UnLock(); return val; }

	template <typename T>
	static T UnlockedCopied (T val, umutex& _mutex) 
		{  _mutex.UnLock(); return val; }

	//******************************
	// NOTE: In some platforms, multiple locks on the same mutex by the same
	// thread may be acceptable but in others may block. A safe practice is to assume
	// it blocks (i.e., deadlocks), thus avoiding nested locks in the same thread.

	void	WaitToLock (void);			// Busy waiting until locked.
	bool	LockIfNotLocked (void);		// Locks only if available, returning true, else exits, returning false.
	void	UnLock (void);				// Unlocks the mutex.
	bool	IsLocked (void) const
				{ return locked; }
	bool	IsValid (void) const
				{ return !!handle; }

	umutex (void);
	~umutex();
};

#define	LOCK_BLOCK_OPT(mutexPtr) \
		umutex::FunctionLocker _temp(mutexPtr);

#define	LOCK_BLOCK(mutex) \
		umutex::FunctionLocker _temp(mutex);

#define	LOCK_EXPR_OBJECT(expr, mutex) \
		(umutex::Locker(mutex), umutex::UnlockedCopiedRef((expr), mutex))

#define	LOCK_EXPR_VALUE(expr, mutex) \
		(umutex::Locker(mutex), umutex::UnlockedCopied((expr), mutex))

#define	LOCK_VOID_EXPR(expr, mutex) \
	if (true) { mutex.WaitToLock(); expr; mutex.UnLock(); } else

//////////////////////////////////////////////////////////
	
class UTILLIB_CLASS uthread {

	public:
	typedef void (*ThreadFuncType)(void* arg);	// The std signature of threads.

	struct UTILLIB_CLASS ThreadData {
			ThreadFuncType		func;			// User function.
			mutable umutex		mutex;			// Thread function mutex.
			void*				threadId;		// Therad id, returned.
			void*				handle;			// Thread handle.
			void*				userArg;		// User supplied arguments.
			std::string			name;			// User supplied thread name (optional)
			bool				exitRequested;	// For threads executing with a loop checking this flag.
			volatile bool		runPending;
	};

	private:
	ThreadData	data;

	//******************************

	public:
	void				Start (void* arg = (void*)0 );			// Creates. Checks if running for sure.
	void				End (void);								// Call always from the same thread.
	bool				IsRunning (void) const;
	void				WaitToFinish (void);					// Busy waiting (with a mutex).
	bool				IsCallingThread (void) const;			// Returns true if call is made from the same thread.

	bool				IsExitRequested (void) const			// Can use it as a thread's main loop condition.
							{ return data.exitRequested; }
	void				RequestExit (void);
	static void			RequestExitWaitAndDestroy (uthread* p);	// For p created with DNEWCLASS.

	uthread (ThreadFuncType func, const char *name = (const char*) 0);
	~uthread();
};

//---------------------------------------------------------

#endif	// Do not add stuff beyond this point.

