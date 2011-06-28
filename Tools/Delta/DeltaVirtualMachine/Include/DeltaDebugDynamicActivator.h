// DeltaDebugDynamicActivator.h
// Support for dynamic debugger backend activation (dll version only)
// for applications build without the backend.
// Script Fighter Project.
// A. Savidis, March 2010.
//

#ifndef	DELTADEBUGDYNAMICACTIVATOR_H
#define	DELTADEBUGDYNAMICACTIVATOR_H

#include "DeltaStdDefs.h"
#include "usingleton.h"
#include "ufunctors.h"
#include "ucallbacks.h"
#include "uthreadlib.h"
#include "udynamiclibloader.h"
#include <string>
#include <list>

/////////////////////////////////////////////////////////////////

#define	DELTA_DEBUG_DYNAMIC_ACTIVATOR_NO_INIT		""
#define	DELTA_DEBUG_DYNAMIC_ACTIVATOR_NO_CLEANUP	""

class DVM_CLASS DeltaDebugDynamicActivatorClass {

	public:
	typedef void (*Callback)(void*);

	protected:
	typedef uquadruple<udynamiclibloader*, std::string, std::string, std::string>	DLL;

	std::list<DLL>					dlls;
	ucallbackwithclosure<Callback>	afterLoadingCallback;
	ucallbackwithclosure<Callback>	beforeUnloadingCallback;
	bool							hasLoaded;
	bool							pendingUnload;
	mutable umutex					accessMutex;
	void*							previousDebugExtCtor;

	void		UnloadPrivate (void);
	bool		Load (void);
	bool		ActivationDialogue (const std::string& msg);

	/////////////////////////////////////////////////////////////////

	public:
	void		AddLibrary (const std::string& dll, const std::string& init, const std::string&	cleanup);
	void		SetAfterLoadingCallback (Callback f, void* closure = (void*) 0);
	void		ResetAfterLoadingCallback (void);
	void		SetBeforeUnloadingCallback (Callback f, void* closure = (void*) 0);
	void		ResetBeforeUnloadingCallback (void);
	bool		IsApplicable (void) const;
	bool		HasLoaded (void) const;
	void		LoadOnlyUponUserRequest (void);
	void		SetPendingUnload (void);
	bool		IsUnloadPending (void) const;
	void		Unload (void);

	DeltaDebugDynamicActivatorClass (void);
	virtual ~DeltaDebugDynamicActivatorClass();
};

class DVM_CLASS DeltaDebugDynamicActivator : public usingleton<DeltaDebugDynamicActivatorClass> {};

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
