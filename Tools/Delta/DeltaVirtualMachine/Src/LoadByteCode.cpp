// LoadByteCode.cpp
// Loading virual machines.
// ScriptFighter Project.
// Original version
// Script Fighter Project.
// A. Savidis, October 1999 (orginal version).
// Latest refactoring, February 2008.
//
#include "DDebug.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaVirtualMachine.h"
#include "DeltaRuntimeError.h"
#include "uerrorclass.h"
#include "DeltaVersionDefs.h"

#define	ERROR_HANDLER(what, errclass)			\
	uerror::GetSingleton().post##errclass(		\
		"Loading vm: error reading '%s'!", what \
	); goto FAIL;

//-----------------------------------------------------------------

bool DeltaVirtualMachine::Load (GenericReader& reader) {

	if (GetDebugger())
		GetDebuggerRef().OnStartLoading();

	DeltaDebugExtensionsSuper::LoopLeadingLines loopLines;

	UCHECK_DOMINO_ERROR(DeltaReadVersionInformation(reader), "version information");
	UCHECK_PRIMARY_ERROR(reader.read(srcFile, false), "source file");

	if (userDefinedFuncs) 
		{ DDELARR(userDefinedFuncs); unullify(userDefinedFuncs); totalUserDefinedFuncs = 0; }
	else
		DASSERT(!totalUserDefinedFuncs);
	funcByName.clear();
	funcByAddr.clear();

	userDefinedFuncs = DeltaStdFuncInfo::ReadFunctionTable(reader, &totalUserDefinedFuncs);
	UCHECK_DOMINO_ERROR(userDefinedFuncs || !totalUserDefinedFuncs, "function table");

	if (totalUserDefinedFuncs) {
		DeltaStdFuncInfo* p = userDefinedFuncs;
		for (util_ui16 i = 0; i < totalUserDefinedFuncs; ++i, ++p) {

			if (p->IsNamedFunction() && p->IsGlobal()) { // Only global functions
				DASSERT(funcByName.find(p->GetName()) == funcByName.end());
				DASSERT(!p->HasClosureVars());
				funcByName[p->GetName()] = p;
			}

			DASSERT(funcByAddr.find(p->GetAddress()) == funcByAddr.end());
			funcByAddr[p->GetAddress()]	= p;

			p->SafeClearReturnTypes();	// We do need need this information at runtime.
		}
	}

	UCHECK_PRIMARY_ERROR(reader.read(&totalGlobals), "total globals");
	UCHECK_DOMINO_ERROR(constArrays.Read(reader), "constant arrays");

	// The debug ifnormation is read any way if included, but it is
	// ignored when no debugger linkage is set.
	//
	UCHECK_PRIMARY_ERROR(reader.read(&hasDebugInfo), "has debug info");
	UCHECK_DOMINO_ERROR(!hasDebugInfo || debugInfo.Read(reader), "debug info");

	if (code) 
		{ DDELARR(code);  unullify(code);  }

	UCHECK_PRIMARY_ERROR(reader.read(&codeSize), "code size");
	code = DNEWARR(DeltaInstruction, codeSize);

	if (GetDebugger())
		GetDebuggerRef().OnInitLinesOfCodes(codeSize);

	for (util_ui32 i = 0; i < codeSize; ++i) {

		DeltaInstruction* curr = code + i;

		UCHECK_DOMINO_ERROR(curr->Read(reader), uconstructstr("instruction #%u", i));

		PrebindStringConsts(curr);	// Always preceeded to library funcs.
		PrebindNumericConsts(curr);
		PrebindLibraryFuncs(curr);
		PrebindFunctionsOrMethods(curr);
		
		if (GetDebugger())
			loopLines.Preprocess(*curr, i);
	}

	if (GetDebugger())
		loopLines.Add(GetDebuggerRef());

	GlobalGarbageCollection();
	ResetTotalActualArgs();			

	pc						= 0;				// First instruction.
	executionError			=					// No error yet.
	isRunning				= false;			// And not running yet.

	ClearStaticsTable();
	ProduceStaticsList();

	DASSERT(selfAsExternId);
	DNULLCHECK(GetStdUtilities())->LinkWithLibraryFunctions(this);

	// Notify that this VM has been loaded.
	//
	if (onLoadCallback.f())
		onLoadCallback(this);

	if (GetDebugger())
		GetDebuggerRef().OnFinishedLoading();
	return true;

	// If failed to load, we destroy everything.
	//
	FAIL:	
	if (GetDebugger())
		GetDebuggerRef().OnFinishedLoading();	
	Clear();
	
	return false;
}

//////////////////////////////////////////////////////////////////

#undef ERROR_HANDLER
#define	ERROR_HANDLER(what, errclass) \
	uerror::GetSingleton().post##errclass( "Loading vm: %s!", what ); goto FAIL;

bool DeltaVirtualMachine::Load (const char* file) {

	FILE* fp = ubinaryfileopen(file, "r");
	UCHECK_PRIMARY_ERROR(
		fp,
		uconstructstr("failed to open '%s' for code loading!", file)
	);

	{ // jump to FAIL: crosses initialisation of "result"
		bool result = Load(utempobj(PortableBinFileReader(fp)));
		fclose(fp);

		return result;
	}

	FAIL: return false;
}

//-----------------------------------------------------------------
