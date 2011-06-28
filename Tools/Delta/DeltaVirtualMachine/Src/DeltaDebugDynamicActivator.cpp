// DeltaDebugDynamicActivator.cpp
// Script Fighter Project.
// A. Savidis, March 2010.
//

#include "DeltaDebugDynamicActivator.h"
#include "DeltaDebugExtensionsSuper.h"
#include "uerrorclass.h"

#ifdef	_MSC_VER
#include <windows.h>
#include <winbase.h>
#endif

USINGLETON_INSTANCE(usingleton<DeltaDebugDynamicActivatorClass>)

#define	ERROR_HANDLER(what, errclass)									\
	uerror::GetSingleton().post##errclass(								\
		"Dynamic debugger backend activation: %s!", what				\
	); goto FAIL;

#define	USER_MESSAGE													\
	"A runtime error occurred but the executable was not linked\n"		\
	"with the debugger modules. Do you wish the system to try load\n"	\
	"them dynamically so that you can initiate a debug session?"		\

/////////////////////////////////////////////////////////////////

DeltaDebugDynamicActivatorClass::DeltaDebugDynamicActivatorClass (void) : 
	hasLoaded(false),
	pendingUnload(false),
	previousDebugExtCtor((void*) 0)
	{}

DeltaDebugDynamicActivatorClass::~DeltaDebugDynamicActivatorClass() 
	{ DASSERT(!HasLoaded()); }

/////////////////////////////////////////////////////////////////
// The following is a necessary wrap around to ensure none
// intercepts the message box to apply other type of processing.

#ifdef	_MSC_VER
static LRESULT CALLBACK MessageProc(__in  int code, WPARAM wParam, __in LPARAM lParam) 
	{ return code == HC_ACTION ? 1 : CallNextHookEx(NULL, code, wParam, lParam); }
#endif

/////////////////////////////////////////////////////////////////

bool DeltaDebugDynamicActivatorClass::ActivationDialogue (const std::string& msg) {

#ifdef	_MSC_VER

	HHOOK hook = SetWindowsHookEx(WH_GETMESSAGE, &MessageProc, NULL, GetCurrentThreadId());

	bool result =	MessageBox(
						GetForegroundWindow(),
						msg.c_str(),
						"Runtime error!",
						MB_APPLMODAL | MB_YESNO
					) == IDYES;

	UnhookWindowsHookEx(hook);
	return result;

#else
	fprintf(stderr, "Runtime error!\n%s\n1.Yes\t2.No\n", msg.c_str());
	util_ui32 i, j;
	do
		{ printf(">"); j = scanf("%u", &i); }
	while (j != 1 || (i != 1 && i != 2));
	return i == 1;
#endif
}

/////////////////////////////////////////////////////////////////

void DeltaDebugDynamicActivatorClass::AddLibrary (
		const std::string&	dll,
		const std::string&	init,
		const std::string&	cleanup
	) {
	LOCK_BLOCK(accessMutex);
	dlls.push_back(DLL((udynamiclibloader*) 0, dll, init, cleanup));
}

/////////////////////////////////////////////////////////////////

void DeltaDebugDynamicActivatorClass::LoadOnlyUponUserRequest (void) {
	if (ActivationDialogue(USER_MESSAGE))
		if (!Load()) {
			DASSERT(UERROR_ISRAISED());
			umodalmessage("Loading error", UERROR_GETREPORT());		
	};
}

/////////////////////////////////////////////////////////////////

bool DeltaDebugDynamicActivatorClass::Load (void) {

	LOCK_BLOCK(accessMutex);
	DASSERT(!HasLoaded());

	UFUNCIN2VOID(DeltaDebugExtensionsSuper::GetConstructor(), previousDebugExtCtor);

	for (std::list<DLL>::iterator i = dlls.begin(); i != dlls.end(); ++i) {

		DASSERT(!i->first);
		udynamiclibloader* p = i->first = DNEWCLASS(udynamiclibloader, (i->second));

		UCHECK_PRIMARY_ERROR(
			*DPTR(p),
			uconstructstr("failed to load dll '%s'", DPTR(p)->GetError().c_str())
		);

		if (!i->third.empty()) { // Has init function

			udynamiclibloader::ResultType result = DPTR(p)->Call(i->third); // Call it
				
			UCHECK_PRIMARY_ERROR(
				result.first != UTIL_DLLFUNC_NOTFOUND,
				uconstructstr(
					"in '%s' invocation of init function '%s' failed (%s)",
					i->second.c_str(),
					i->third.c_str(),
					p->GetError().c_str()
				)
			);

			UCHECK_PRIMARY_ERROR(
				result.first == UTIL_DLLFUNC_TRUSTED,
				uconstructstr(
					"in '%s' invocation of init function '%s' failed (%s)",
					i->second.c_str(),
					i->third.c_str(),
					p->GetErrorString(result.first)
				)
			);
		}

		if (!i->fourth.empty())	// Register cleanup function if any
			p->SetCleanUp(i->fourth);
	}

	hasLoaded = true;
	if (afterLoadingCallback)
		afterLoadingCallback();
	return true;

	FAIL: 
	UnloadPrivate(); 
	return false;
}

/////////////////////////////////////////////////////////////////

void DeltaDebugDynamicActivatorClass::UnloadPrivate (void) {
	pendingUnload = false;
	for (std::list<DLL>::iterator i = dlls.begin(); i != dlls.end(); ++i)
		udeleteunlessnull(i->first);
	
	UCHECK_DOMINO_ERROR(!UERROR_ISRAISED(), "there have been errors during unloading");

	FAIL: ;
}

/////////////////////////////////////////////////////////////////

void DeltaDebugDynamicActivatorClass::SetPendingUnload (void) {
	LOCK_BLOCK(accessMutex);
	DASSERT(HasLoaded());
	pendingUnload = true;
}

bool DeltaDebugDynamicActivatorClass::IsUnloadPending (void) const
	{ return pendingUnload; }

/////////////////////////////////////////////////////////////////

void DeltaDebugDynamicActivatorClass::Unload (void) {

	LOCK_BLOCK(accessMutex);
	DASSERT(HasLoaded());

	if (beforeUnloadingCallback)
		beforeUnloadingCallback();
	UnloadPrivate();

	DeltaDebugExtensionsSuper::SetConstructor(
		UVOID2FUNC(DeltaDebugExtensionsSuper::ConstructorFunc, DNULLCHECK(previousDebugExtCtor))
	);
	hasLoaded = false;
}

/////////////////////////////////////////////////////////////////

bool DeltaDebugDynamicActivatorClass::HasLoaded (void) const 
	{ return hasLoaded; }

/////////////////////////////////////////////////////////////////

bool DeltaDebugDynamicActivatorClass::IsApplicable (void) const {
#ifdef	DVM_EXPORT	// DLL build
	LOCK_BLOCK(accessMutex);
	return !dlls.empty();
#else
	return false;
#endif
}

/////////////////////////////////////////////////////////////////

void DeltaDebugDynamicActivatorClass::SetAfterLoadingCallback (Callback f, void* closure) 
	{ afterLoadingCallback.set(f, closure); }

void DeltaDebugDynamicActivatorClass::ResetAfterLoadingCallback (void)
	{ afterLoadingCallback.reset(); }

void DeltaDebugDynamicActivatorClass::SetBeforeUnloadingCallback (Callback f, void* closure) 
	{ beforeUnloadingCallback.set(f, closure); }

void DeltaDebugDynamicActivatorClass::ResetBeforeUnloadingCallback (void)
	{ beforeUnloadingCallback.reset(); }

/////////////////////////////////////////////////////////////////

