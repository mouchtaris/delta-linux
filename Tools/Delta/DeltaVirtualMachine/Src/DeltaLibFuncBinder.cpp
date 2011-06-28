// DeltaLibFuncBinder.cpp
// Implementation of lib func binder class.
// Additional information regarding library functions,
// keeping two tables mapping indices to names, are
// provided only when debugger is linked. 
// ScriptFigher Project.
// A. Savidis, June 2001.
//

#include "DeltaVirtualMachine.h"
#include "DeltaLibFuncBinder.h"
#include "uerrorclass.h"
#include "DeltaByteCodeTypes.h"

#include <string.h>

DeltaLibFuncBinder::ByName*		DeltaLibFuncBinder::byName			= (ByName*)		0;
DeltaLibFuncBinder::ByAddr*		DeltaLibFuncBinder::byAddr			= (ByAddr*)		0;
DeltaLibFuncBinder::BySerial*	DeltaLibFuncBinder::bySerial		= (BySerial*)	0;
DeltaLibFuncBinder::SerialPool*	DeltaLibFuncBinder::serialPool		= (SerialPool*) 0;
ubag<DeltaLibraryFunc>*			DeltaLibFuncBinder::internalFuncs	= (ubag<DeltaLibraryFunc>*) 0;
util_ui32						DeltaLibFuncBinder::serialNo		= DELTA_LIBFUNC_INTERNAL_SERIALNO + 1;
util_ui32						DeltaLibFuncBinder::totalFuncs		= 0;

////////////////////////////////////////////////////

bool DeltaLibFuncBinder::IsSingletonCreated (void)
	{ return byName && byAddr && bySerial && serialPool && internalFuncs; }

void DeltaLibFuncBinder::SingletonCreate (void) {

	DASSERT(!IsSingletonCreated());
	unew(byName);
	unew(byAddr);
	unew(bySerial);
	unew(serialPool);
	unew(internalFuncs);
	serialNo = totalFuncs = 0;
}

void DeltaLibFuncBinder::SingletonDestroy (void) {
	DASSERT(IsSingletonCreated());
	udelete(byName);
	udelete(byAddr);
	udelete(bySerial);
	udelete(serialPool);
	udelete(internalFuncs);
	serialNo = totalFuncs = 0;
}

////////////////////////////////////////////////////

void DeltaLibFuncBinder::Initialise (void) {
	DASSERT(IsSingletonCreated());
}

void DeltaLibFuncBinder::CleanUp (void) {
	DPTR(byName)->clear();
	DPTR(byAddr)->clear();
	DPTR(bySerial)->clear();
	DPTR(serialPool)->clear();
	serialNo = totalFuncs = 0;
}

////////////////////////////////////////////////////

void DeltaLibFuncBinder::AddInternal (DeltaLibraryFunc addr) {
	DASSERT(IsSingletonCreated());
	DASSERT(!IsInternal(addr));
	DPTR(internalFuncs)->insert(addr);
}

//***************************

void DeltaLibFuncBinder::RemoveInternal (DeltaLibraryFunc addr) {
	DASSERT(IsSingletonCreated());
	ubag<DeltaLibraryFunc>::iterator i = DPTR(internalFuncs)->find(addr);
	DASSERT(i != DPTR(internalFuncs)->end());
	DPTR(internalFuncs)->remove(i);
}

//***************************

bool DeltaLibFuncBinder::IsInternal (DeltaLibraryFunc addr) {
	DASSERT(IsSingletonCreated());
	return DPTR(internalFuncs)->find(addr) != DPTR(internalFuncs)->end();
}

////////////////////////////////////////////////////

util_ui32 DeltaLibFuncBinder::GetSerialNo (const std::string& name) {

	DASSERT(IsSingletonCreated());
	if (name == DELTA_LIBFUNC_INTERNAL_NAME)
		return DELTA_LIBFUNC_INTERNAL_SERIALNO;
	else {
		ByName::iterator i = DPTR(byName)->find(name);
		DASSERT(i != DPTR(byName)->end());
		return i->second.second;
	}
}

////////////////////////////////////////////////////

util_ui32 DeltaLibFuncBinder::GetSerialNo (DeltaLibraryFunc addr) {

	DASSERT(IsSingletonCreated());

	ByAddr::iterator i = DPTR(byAddr)->find(addr);
	if (i != DPTR(byAddr)->end())
		return i->second.second;
	else {
		DASSERT(IsInternal(addr));
		return DELTA_LIBFUNC_INTERNAL_SERIALNO;
	}
}

////////////////////////////////////////////////////

const char* DeltaLibFuncBinder::GetName (util_ui32 serial) {

	DASSERT(IsSingletonCreated());
	if (serial == DELTA_LIBFUNC_INTERNAL_SERIALNO)
		return DELTA_LIBFUNC_INTERNAL_NAME;
	else {
		BySerial::iterator i = DPTR(bySerial)->find(serial);
		DASSERT(i != DPTR(bySerial)->end());
		return i->second.c_str();
	}	
}

////////////////////////////////////////////////////

const char* DeltaLibFuncBinder::GetNameByAddress (DeltaLibraryFunc addr) {

	DASSERT(IsSingletonCreated());

	ByAddr::iterator i = DPTR(byAddr)->find(addr);
	if (i != DPTR(byAddr)->end())
		return i->second.first.c_str();
	else
	if (IsInternal(addr))
		return DELTA_LIBFUNC_INTERNAL_NAME;
	else
		return (char*) 0;
}

////////////////////////////////////////////////////

DeltaLibraryFunc DeltaLibFuncBinder::Get (const std::string& name) {

	DASSERT(IsSingletonCreated());

	ByName::iterator i = DPTR(byName)->find(name);
	if (i != DPTR(byName)->end())
		return i->second.first;
	else
		return (DeltaLibraryFunc) 0;
}

////////////////////////////////////////////////////

bool DeltaLibFuncBinder::Add (const std::string& name, DeltaLibraryFunc addr) {

	DASSERT(IsSingletonCreated());

	if (totalFuncs > DELTA_MAXLIBFUNC_INDEX) {
		UPRIMARYERROR("Adding library function '%s': exceeded maximum!", name.c_str());
		return false;
	}
	
	if (Get(name)) {
		UPRIMARYERROR("Adding library function '%s': already added!", name.c_str());
		return false;
	}

	util_ui32 currSerial;
	++totalFuncs;
	if (!DPTR(serialPool)->empty()) {	// Can we reuse? do it.
		currSerial = DPTR(serialPool)->front(); 
		DPTR(serialPool)->pop_front(); 
	}
	else
		currSerial = ++serialNo;
	DASSERT(serialNo <= totalFuncs);	// No way the serial number get's over it.

	(*DPTR(byName))[name]			= WithAddr(addr, currSerial);
	(*DPTR(byAddr))[addr]			= WithName(name, currSerial);
	(*DPTR(bySerial))[currSerial]	= name;

	return true;
}

////////////////////////////////////////////////////

bool DeltaLibFuncBinder::Remove (const std::string& name, DeltaLibraryFunc addr) {

	ByName::iterator i = DPTR(byName)->find(name);
	if (i == DPTR(byName)->end()) {
		UPRIMARYERROR("Removing library function '%s': not found!", name.c_str());
		return false;
	}

	ByAddr::iterator j = DPTR(byAddr)->find(addr);
	if (j == DPTR(byAddr)->end()) {
		UPRIMARYERROR("Removing library function '%s': address not found!", name.c_str());
		return false;
	}

	DASSERT(i->second.first == addr);					// Addresses match.
	DASSERT(j->second.first == name);					// Names match.
	DASSERT(i->second.second == j->second.second);		// Serial number match.

	util_ui32			serial	= i->second.second;
	BySerial::iterator	k		= DPTR(bySerial)->find(serial);
	DASSERT(k != DPTR(bySerial)->end());

	DPTR(byName)->erase(i);
	DPTR(byAddr)->erase(j);
	DPTR(bySerial)->erase(k);

	DPTR(serialPool)->push_back(serial);
	--totalFuncs;

	return true;
}

////////////////////////////////////////////////////
