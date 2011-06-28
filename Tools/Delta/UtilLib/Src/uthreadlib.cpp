// uthreadlib.cpp
// Thread library generalisation API for WNT and UNIX.
// Originally from 2WEAR Project.
// NOTE: do not use DDebug in here since the latter already
// uses uthreadlib for mutexes.
// A. Savidis, September 2003.
// Fixed a data race bug on mutex locking if lock and unlock may take
// place from different threads, March 2010.
//

#include "uthreadlib.h"
#include "DDebug.h"
#include "ufunctors.h"
#include "uerrorclass.h"

//--------------------------------------------------------
// DEFINITIONS.

#ifdef	_WIN32_
#include <windows.h>
#include <process.h>

#define	TYPE_SPEC		void __cdecl
#define	THREAD_RETURN	
#define	GET_THREAD		GetCurrentThreadId

#endif	// _WIN32_

//////////////////////////////////////////////////////////

#ifdef	_UNIX_

#include <pthread.h>
#include <errno.h>

#define	TYPE_SPEC		void*
#define	THREAD_RETURN	return (void*) 0
#define	GET_THREAD		pthread_self
#define	_POSIX_

#endif	// _UNIX_

#define	THREAD_CLOSE_SLEEPTIME	500

//--------------------------------------------------------
// THREADS.

static TYPE_SPEC ThreadWrapper (void* args) {

	uthread::ThreadData* data = (uthread::ThreadData*) args;

	LOCK_BLOCK(data->mutex);
	DASSERT(data->mutex.IsLocked());
	data->threadId = (void*) GET_THREAD();
	data->runPending = false;

	(*data->func)(data->userArg);

	THREAD_RETURN;
}

//////////////////////////////////////////////////////////

bool uthread::IsRunning (void) const {
	if (data.mutex.LockIfNotLocked()) {
		data.mutex.UnLock();
		return false;
	}
	else
		return true;
}

//////////////////////////////////////////////////////////

void uthread::WaitToFinish (void) 
	{ LOCK_BLOCK(data.mutex); }

//////////////////////////////////////////////////////////

void uthread::Start (void* arg) {

	DASSERT(!IsRunning());
	
	data.userArg	= arg;
	data.runPending = true;

#ifdef	_WIN32_
	if (_beginthread(ThreadWrapper, 0, &data) == -1)
		DASSERT(false);	// Could not create thread.
	unullify(data.handle);
#endif

#ifdef	_POSIX_
	if (pthread_create((pthread_t *)data.handle, (pthread_attr_t*) 0, ThreadWrapper, &data) != 0)
		DASSERT(false);	// Could not create thread.
#endif

	while (data.runPending)
		;
}

//////////////////////////////////////////////////////////
// Called from within a thread function to cause instant 
// thread termination.
//
void uthread::End (void) {

	DASSERT(IsRunning());

#ifdef	_WIN32_
	_endthread();
#endif

#ifdef	_POSIX_
	pthread_exit((void*) 0);
#endif

	data.threadId = (void*) 0;
}

//////////////////////////////////////////////////////////

void uthread::RequestExit (void) 
	{ data.exitRequested = true; }

//////////////////////////////////////////////////////////

void uthread::RequestExitWaitAndDestroy (uthread* p) {
	DPTR(p)->RequestExit();
	if (DPTR(p)->IsRunning()) {
		DASSERT(!DPTR(p)->IsCallingThread());
		DPTR(p)->WaitToFinish();
		uprocesssleep(THREAD_CLOSE_SLEEPTIME);
	}
	udelete(p);
}

//////////////////////////////////////////////////////////

bool uthread::IsCallingThread (void) const 
	{ return data.threadId == (void*) GET_THREAD(); }

//////////////////////////////////////////////////////////

uthread::uthread (ThreadFuncType _func, const char *name) {

	data.func			= _func;
	data.threadId		= (void*) 0;
	data.exitRequested	= false;
	if (name)
		data.name = name;
#if	defined(_POSIX_)
	data.handle = new pthread_t;
#endif
}

//////////////////////////////////////////////////////////
// A thread function should not try to delete its
// thread instance, since this will lead to a dead lock.
//
uthread::~uthread()
	{ WaitToFinish(); }

//--------------------------------------------------------
// MUTEXES.

bool umutex::LockIfNotLocked (void) {

	DASSERT(handle);

#ifdef	_WIN32_
#define	IF_NON_BLOCKING_LOCK									\
	DWORD result = WaitForSingleObject((HANDLE) handle, 0);		\
	if (result == WAIT_ABANDONED || result == WAIT_OBJECT_0)
#endif

#ifdef	_POSIX_
#define	IF_NON_BLOCKING_LOCK									\
	if (pthread_mutex_trylock((pthread_mutex_t*) handle) != EBUSY)
#endif

	IF_NON_BLOCKING_LOCK
		return ++locked, true;
	else
		return false;	// May be already locked or someone else is trying to lock it first.

#undef	IF_NON_BLOCKING_LOCK
}

//////////////////////////////////////////////////////////

void umutex::WaitToLock (void) {

	DASSERT(IsValid());

#ifdef	_WIN32_
	DWORD result = WaitForSingleObject((HANDLE) handle, INFINITE);
#endif

#ifdef	_POSIX_
	pthread_mutex_lock((pthread_mutex_t*) handle);
#endif

	++locked;
}

//////////////////////////////////////////////////////////

void umutex::UnLock (void) {

	DASSERT(IsValid());
	unlockPending = true;
#ifdef	_WIN32_
	bool result = ReleaseMutex((HANDLE) handle);
	DASSERT(result);
#endif

#ifdef	_POSIX_
	pthread_mutex_unlock((pthread_mutex_t*) handle);
#endif

	--locked;
	unlockPending = false;
}

//////////////////////////////////////////////////////////

umutex::umutex (void) {
	unlockPending = false;
#ifdef	_WIN32_
	handle = (void*) CreateMutex(NULL, FALSE, NULL);
#endif

#ifdef	_POSIX_
	pthread_mutexattr_t attrs;
	pthread_mutexattr_init(&attrs);

	handle = new pthread_mutex_t;
	if (pthread_mutex_init((pthread_mutex_t*) handle, &attrs) != 0) {
		delete (pthread_mutex_t*) handle;
		handle = (void*) 0;
	}
#endif

	locked = 0;
}

//////////////////////////////////////////////////////////

umutex::~umutex(){

	DASSERT(handle);

	while (unlockPending)
			;
	
	if (IsLocked()) {
		DASSERT(handle);
		UnLock();
	}

#ifdef	_WIN32_
	CloseHandle(handle);
#endif

#ifdef	_POSIX_
	pthread_mutex_destroy((pthread_mutex_t*) handle);
	delete (pthread_mutex_t*) handle;
#endif
}

//--------------------------------------------------------
