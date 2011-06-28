// DMonitors.cpp. 
// Defect monitors.
// Defensive programming library.
// A. Savidis,
// Updated July 2004.
//

#include <memory.h>
#include "DMonitors.h"
#include "uthreadlib.h"
#include "DMemoryCapsule.h"
#include "usystem.h"

#ifdef	DHAS_THREADS

#define	PAINT_CONTENT_CHANGE

/////////////////////////////////////////////////////////////////

bool DefectMonitors::CheckContentModification (void) {

	bool ok =	!memcmp(
					myBlock->GetUserAddr(), 
					contentCopy, 
					myBlock->GetSize()
				); 

#ifdef	PAINT_CONTENT_CHANGE

	// Paints with a * the matched content, so that
	// the update can be manually inspected.
	//
	if (!ok) {

		util_ui8* c = (util_ui8*) contentCopy;
		util_ui8* b = (util_ui8*) myBlock->GetUserAddr(); 
		
		for (util_ui32 n = myBlock->GetSize(); n; --n, ++c, ++b)
			*c = (*c == *b ? '*' : *b);

		DASSERT(false);
	}
#endif

	return ok;
}

//****************************

void DefectMonitors::ContentThreadFunc (void* arg) {

    DefectMonitors* monitors = (DefectMonitors*) arg;

    while (monitors->IsContentMonitorRunning()) {
	  if (monitors->contentCheckPeriod)
	      uprocesssleep(monitors-> contentCheckPeriod);
	  if (!monitors->CheckContentModification())
		  DASSERT(false);
    }
}

//****************************

void DefectMonitors::ValidityThreadFunc (void* arg) {

    DefectMonitors* monitors = (DefectMonitors*) arg;

    while (monitors->IsValidityMonitorRunning()) {

	  if (monitors->validityCheckPeriod)
	      uprocesssleep(monitors->validityCheckPeriod);

	  if (!monitors->myBlock->Validate())
	      DASSERT(false);	// Raise an explicit failing assertion.
    }
}

//****************************

void DefectMonitors::ExpirationThreadFunc (void* arg) {

    DefectMonitors* monitors = (DefectMonitors*) arg;

    while (monitors->IsExpirationMonitorRunning())
	   if (ugettime() >= monitors->expirationTime)
	       DASSERT(false);	// Raise an explicit failing assertion.
}

//****************************

void DefectMonitors::DisposalThreadFunc (void* arg) {

    DefectMonitors* monitors = (DefectMonitors*) arg;
    uprocesssleep(monitors->disposalDelay);

	monitors->myBlock->Destroy();
}

/////////////////////////////////////////////////////////////////

void DefectMonitors::StartContentMonitor (unsigned period) {

	DASSERT(!contentThread.IsRunning());
	DASSERT(!IsContentMonitorRunning());

	flags |= CONTENT_MONITOR_RUNNING_FLAG;
	contentCheckPeriod = period;
	contentCopy = DMemoryManager::GetMemory(myBlock->GetSize());
	memcpy(contentCopy, myBlock->GetUserAddr(), myBlock->GetSize());
	contentThread.Start(this);
}

//****************************

void DefectMonitors::StopContentMonitor (void) {
	DASSERT(contentThread.IsRunning());
	DASSERT(IsContentMonitorRunning());
	flags &= ~CONTENT_MONITOR_RUNNING_FLAG;
	contentThread.WaitToFinish();
	DMemoryManager::ReleaseMemory(contentCopy);
	contentCopy = (void*) 0;
}

//****************************

void DefectMonitors::StartValidityMonitor (util_ui32 period) {
	DASSERT(!validityThread.IsRunning());
	DASSERT(!IsValidityMonitorRunning());
	flags |= VALIDITY_MONITOR_RUNNING_FLAG;
	validityCheckPeriod = period;
	validityThread.Start(this);
}

//****************************

void DefectMonitors::StopValidityMonitor (void) {
	DASSERT(validityThread.IsRunning());
	flags &= ~VALIDITY_MONITOR_RUNNING_FLAG;
	validityThread.WaitToFinish();
}

/////////////////////////////////////////////////////////////////

void DefectMonitors::StartExpirationMonitor (util_ui32 delay) {
	DASSERT(!expirationThread.IsRunning());
	DASSERT(!IsExpirationMonitorRunning());
	flags |= EXPIRATION_MONITOR_RUNNING_FLAG;
	expirationTime = ugettime() + delay;
	expirationThread.Start(this);
}

//****************************

void DefectMonitors::StopExpirationMonitor (void) {
	DASSERT(expirationThread.IsRunning());
	DASSERT(IsExpirationMonitorRunning());
	flags &= ~EXPIRATION_MONITOR_RUNNING_FLAG;
	expirationThread.WaitToFinish();
}

/////////////////////////////////////////////////////////////////

void DefectMonitors::StartDisposalMonitor (util_ui32 delay) {

	DASSERT(!disposalThread.IsRunning());
	DASSERT(!IsDisposalMonitorRunning());

	flags |= DISPOSAL_MONITOR_RUNNING_FLAG;
	disposalDelay = delay;
	disposalThread.Start(this);

	if (!IsContentMonitorRunning())	
		StartContentMonitor();			// Intensive check with 0 delay.

	if (!IsValidityMonitorRunning())	
		StartValidityMonitor();			// Intensive check with 0 delay.
}

//****************************

void DefectMonitors::WaitDisposalMonitor (void) {
	disposalThread.WaitToFinish();
}

/////////////////////////////////////////////////////////////////

DefectMonitors::DefectMonitors (MemoryCapsule* p): 
	myBlock(p),
	contentThread(ContentThreadFunc),
	validityThread(ValidityThreadFunc),
	expirationThread(ExpirationThreadFunc),
	disposalThread(DisposalThreadFunc),
	flags(0)
	{}

DefectMonitors::~DefectMonitors() {

	if (IsContentMonitorRunning())
		StopContentMonitor();

	if (IsValidityMonitorRunning())
		StopValidityMonitor();

	if (IsExpirationMonitorRunning())
		StopExpirationMonitor();
}

/////////////////////////////////////////////////////////////////

#endif