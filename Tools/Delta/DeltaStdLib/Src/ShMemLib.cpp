// ShMemLib.cpp
// Simple shared memory facilities for virtual machines running
// IN THE SAME executable, i.e., not really inter-process shared memory.
// Access should be serialised (i.e. no parallel read / writes).
// ScriptFigher Project.
// A. Savidis, December 1999.
// Refactored December 2007, encompassed in the 'std' API object. 
//

#include "DeltaLibraryCreators.h"
#include "ShMemLib.h"
#include "ShMemLibP.h"
#include "DeltaStdLibFuncNames.h"
#include <string.h>

//-----------------------------------------------------------------------

static void shexists_LibFunc (DeltaVirtualMachine*);
static void shwrite_LibFunc (DeltaVirtualMachine*);
static void shread_LibFunc (DeltaVirtualMachine*);
static void shdelete_LibFunc (DeltaVirtualMachine*);
static void shobject_LibFunc (DeltaVirtualMachine*);

//-----------------------------------------------------------------------

DeltaSharedMemory::Memory* DeltaSharedMemory::mem = (Memory*) 0;
static DELTA_OBJECT* sharedTable = (DELTA_OBJECT*) 0;

static void Initialise (void) {
	DASSERT(!sharedTable);
	sharedTable = DeltaObject::NewObject();
	DPTR(sharedTable)->IncRefCounter((DeltaValue*) 0);
	DPTR(sharedTable)->GetCreationInfo().SetCreatorInformation(
		DELTA_STDLIB_NAMESPACE  "(library)",
		DELTA_CREATION_LINE_HIDDEN,
		"is " DELTA_STD_SHARED_OBJECT_DESC
	);
	DeltaSharedMemory::Initialise();
}

static void CleanUp (void) {
	DASSERT(sharedTable);
	DPTR(sharedTable)->DecRefCounter((DeltaValue*) 0);
	sharedTable = (DELTA_OBJECT*) 0;
	DeltaSharedMemory::CleanUp();
}

//-----------------------------------------------------------------------

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "exists",		shexists_LibFunc	},
	{ "write",		shwrite_LibFunc		},
	{ "read",		shread_LibFunc		},
	{ "delete",		shdelete_LibFunc	},
	{ "object",		shobject_LibFunc	}
};

DELTA_LIBRARY_SUBAPI_INSTALLER_EX(
		DeltaShMem, 
		"shmem", 
		DELTA_STDLIB_NAMESPACE_PREFIX "sh",
		Initialise(), 
		CleanUp()
	)

//////////////////////////////

void DeltaSharedMemory::Initialise (void) { unew(mem); }
void DeltaSharedMemory::CleanUp (void) { udelete(mem); }

void DeltaSharedMemory::Write (const std::string& name, const DeltaValue& val) {

	DASSERT(mem);
	
	Memory::iterator i = DPTR(mem)->find(name);
	if (i != DPTR(mem)->end())
		i->second.Assign(val);
	else
		(*DPTR(mem))[name].Assign(val);
}

//////////////////////////////

bool DeltaSharedMemory::Read (const std::string& name, DeltaValue* val) {

	DASSERT(mem);

	Memory::iterator i = DPTR(mem)->find(name);
	if (i != DPTR(mem)->end())
		{ val->Assign(i->second); return true; }
	else
		return false;
}

//////////////////////////////

bool DeltaSharedMemory::Remove (const std::string& name) {

	DASSERT(mem);
	Memory::iterator i = DPTR(mem)->find(name);
	if (i != DPTR(mem)->end()) 
		{ DPTR(mem)->erase(i); return true; }
	else
		return false;
}

//////////////////////////////

bool DeltaSharedMemory::Exists (const std::string& name) {
	DASSERT(mem);
	return DPTR(mem)->find(name) != DPTR(mem)->end();
}

//-----------------------------------------------------------------------
// shexists(s).
// s should be a name.
//
void shexists_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("shexists");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_FALSE_RETURNVALUE);

	DeltaValue* argObj;
	GET_STRING(str, CURR_FUNC, RESET_FALSE_RETURNVALUE, 0);

	DPTR(vm)->GetReturnValue().FromBool(DeltaSharedMemory::Exists(str));
}

//-----------------------------------------------------------------------
// shwrite(s,v).
// s should be a shared object name.
// v an object whose value is written.
//
void shwrite_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("shwrite");
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY);

	DeltaValue* argObj;
	GET_STRING(str, CURR_FUNC, RESET_EMPTY, 0);

	DeltaSharedMemory::Write(str, *DPTR(vm)->GetActualArg(1));
}

//-----------------------------------------------------------------------
// shread(s).
// Returns the content of shared object 's'. Should
// check existence with shexists prior to this, if
// not sure.
//
void shread_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("shread");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DeltaValue* argObj;
	GET_STRING(str, CURR_FUNC, RESET_NIL_RETURNVALUE, 0);

	DeltaSharedMemory::Read(
		str, 
		&DPTR(vm)->GetReturnValue()
	);
}

//-----------------------------------------------------------------------
// Bool shdelete(s).
// Returns true if succeeded, else false.
//
void shdelete_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("shdelete");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_FALSE_RETURNVALUE);

	DeltaValue* argObj;
	GET_STRING(str, CURR_FUNC, RESET_FALSE_RETURNVALUE, 0);

	DPTR(vm)->GetReturnValue().FromBool(
		DeltaSharedMemory::Remove(str)
	);
}

//-----------------------------------------------------------------------

// Table shobject().
// Returns shared table.
//
void shobject_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("shobject");
	DeltaTotalArgsCheck(0, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DPTR(vm)->GetReturnValue().FromTable(sharedTable);
}

//-----------------------------------------------------------------------
