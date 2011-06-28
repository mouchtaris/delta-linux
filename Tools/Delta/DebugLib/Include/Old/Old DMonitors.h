// DMonitors.h. WORK IN PROGRESS.
// Defect monitors.
// Defensive programming library.
// A. Savidis,
// Updated July 2004.
//

#ifndef	DMONITORS_H
#define	DMONITORS_H

#include "utypes.h"
#include "uthreadlib.h"

//------------------------------------------------------

#define	CONTENT_MONITOR_RUNNING_FLAG	0x1
#define	VALIDITY_MONITOR_RUNNING_FLAG	0x2
#define	EXPIRATION_MONITOR_RUNNING_FLAG	0x4
#define	DISPOSAL_MONITOR_RUNNING_FLAG	0x8

/////////////////////////////////////////////////////////

class MemoryCapsule;

class DefectMonitors {

	private:
	MemoryCapsule*		myBlock;
	util_ui8			flags;

	Thread				contentThread;
	void*				contentCopy;	
	util_ui32			contentCheckPeriod;
	bool				CheckContentModification (void);
	static void			ContentThreadFunc (void* arg);

	Thread				validityThread;
	util_ui32			validityCheckPeriod;
	static void			ValidityThreadFunc (void* arg);

	Thread				expirationThread;
	util_ui32			expirationTime;
	static void			ExpirationThreadFunc (void* arg);

	Thread				disposalThread;
	util_ui32			disposalDelay;
	static void			DisposalThreadFunc (void* arg);

	public:
	bool	IsContentMonitorRunning (void) const
		    { return (flags & CONTENT_MONITOR_RUNNING_FLAG) != 0; }

	bool	IsValidityMonitorRunning (void) const
		    { return (flags & VALIDITY_MONITOR_RUNNING_FLAG) != 0; }

	bool	IsExpirationMonitorRunning (void) const
		    { return (flags & EXPIRATION_MONITOR_RUNNING_FLAG) != 0; }

	bool	IsDisposalMonitorRunning (void) const
		    { return (flags & DISPOSAL_MONITOR_RUNNING_FLAG) != 0; }

	void	StartContentMonitor (util_ui32 checkPeriod = 0);
	void	StopContentMonitor (void);

	void	StartValidityMonitor (util_ui32 checkPeriod = 0);
	void	StopValidityMonitor (void);

	void	StartExpirationMonitor (util_ui32 delay);
	void	StopExpirationMonitor (void);

	void	StartDisposalMonitor (util_ui32 delay);
	void	WaitDisposalMonitor (void);

	DefectMonitors (MemoryCapsule* block);
	~DefectMonitors ();
};

//------------------------------------------------------

#endif	// Do not add stuff beyond this point.