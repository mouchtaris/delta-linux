// DeltaDebugUserVarsInspector.cpp
// Debug support extensions.
// ScriptFighter Project.
// A. Savidis, June 2006 (original).
// RF to a separate class (February 2010).
//

#include "DeltaDebugUserVarsInspector.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaStdDefs.h"
#include "DeltaClosure.h"
#include "ugeometry.h"

#define	UNKNOWN_FUNC	"<UnknownFunc>"
#define	UNKNOWN_ARG		"<UnknownActualArg>"
#define	UNKNOWN_LOCAL	"<UnknownLocalVar>"
#define	UNKNOWN_GLOBAL	"<UnknownGlobalVar>"

////////////////////////////////////////////////////////////////////

DeltaDebugUserVarsInspector::DeltaDebugUserVarsInspector (void) :
	stack							((DeltaValue*) 0),
	globals							((DeltaValue*) 0),
	totalGlobals					(0),
	topsp							(0),
	top								(0),
	blocks							((const DeltaBlockIdStacks*) 0),
	currTracedProgramFuncDebugInfo	((const DeltaDebugFuncInfo*) 0),
	currTracedProgramFuncStdInfo	((const DeltaStdFuncInfo*) 0),
	currTracedLibFunc				((const char*) 0),
	closure							((const DeltaClosure*) 0)
	{}

DeltaDebugUserVarsInspector::~DeltaDebugUserVarsInspector()
	{}

////////////////////////////////////////////////////////////////////

void DeltaDebugUserVarsInspector::SetDataPointers (
		DeltaValue*					_stack, 
		util_ui32					_stackSize,
		DeltaValue*					_globals,
		util_ui32					_totalGlobals,
		const DeltaBlockIdStacks&	_blocks
	) {
	stack			= _stack;
	stackSize		= _stackSize;
	globals			= _globals;
	totalGlobals	= _totalGlobals;
	blocks			= &_blocks;
	DASSERT(totalGlobals <= stackSize);
}

////////////////////////////////////////////////////////////////////

void DeltaDebugUserVarsInspector::SetCurrTracedFunction (const char* libFunc) {
	unullify(currTracedProgramFuncDebugInfo); 
	unullify(currTracedProgramFuncStdInfo);
	currTracedLibFunc = DNULLCHECK(libFunc); 
	context = ContextLibraryFunction;
}

//***************************

void DeltaDebugUserVarsInspector::SetCurrTracedFunction (
		const DeltaDebugFuncInfo*	funcDebugInfo, 
		const DeltaStdFuncInfo*		funcStdInfo
	) {
	unullify(currTracedLibFunc); 
	currTracedProgramFuncDebugInfo	= funcDebugInfo;				// Can be null.
	currTracedProgramFuncStdInfo	= DNULLCHECK(funcStdInfo);		// This can't.

	context = ContextProgramFunction;
}

//***************************

void DeltaDebugUserVarsInspector::ResetCurrTracedFunction (void) {
	unullify(currTracedLibFunc); 
	unullify(currTracedProgramFuncDebugInfo); 
	unullify(currTracedProgramFuncStdInfo);
	context = ConextUndef;
}

//***************************

void DeltaDebugUserVarsInspector::SetCurrTracedIsGlobalCode (void) {
	unullify(currTracedLibFunc); 
	unullify(currTracedProgramFuncDebugInfo); 
	unullify(currTracedProgramFuncStdInfo);
	context = ContextGlobalCode;
}

////////////////////////////////////////////////////////////////////
// That simly allows to read global cells. Only when the algorithm
// for assigining global memory cells to global variables is known,
// the association to a single global variable is predictable by users.
//
DeltaValue* DeltaDebugUserVarsInspector::GetGlobalVar (util_ui16 offset) const {
	DASSERT(offset < totalGlobals && offset < stackSize);
	return globals - offset;
}

////////////////////////////////////////////////////////////////////
// Returns the total number of actual args passed in the call of
// the current activation record (n_user) INCLUDING the standard arguments
// passed by the language (n_all). This means in debug mode we provide
// all arguments, not just the user-supplied ones.
//
util_ui16 DeltaDebugUserVarsInspector::TotalActualArgs (void) const {

	DASSERT(InFunction() && topsp);

	DASSERT(topsp + DELTA_TOPSP_OLDTOP_OFFSET		< stackSize);
	DASSERT(topsp + DELTA_TOPSP_TOTALARGS_OFFSET	< stackSize);

	util_ui32 argN_offset	= (util_ui32) stack[topsp + DELTA_TOPSP_OLDTOP_OFFSET].ToNumber();
	util_ui32 arg0_offset	= topsp + DELTA_TOPSP_FIRSTARG_OFFSET;
	util_ui16 n_user		= (util_ui16) stack[topsp + DELTA_TOPSP_TOTALARGS_OFFSET].ToNumber();
	
	DASSERT(
		(arg0_offset <= argN_offset) ||
		(InLibraryFunction() && !n_user && arg0_offset == argN_offset + 1)
	);

	util_ui16 n_all  = (util_ui16) ((argN_offset + 1) - arg0_offset);

	// In lib functions we do not have extra arguments, so
	// n_all = n_user. Else, we have 1 (arguments) or 2 (self)
	// extra arguments.

	DASSERT(
		(n_all >= n_user) &&
		(!InLibraryFunction() || n_all == n_user) &&
		(InLibraryFunction()  || (n_all > n_user &&	(n_all - n_user) <= DELTA_CALL_MAX_STD_ARGUMENTS))
	);

	return n_all;
}

////////////////////////////////////////////////////////////////////
// This returns only the arguments supplied by the user
// in a function call, excluding 'self'and 'arguments'.
//
util_ui16 DeltaDebugUserVarsInspector::TotalUserActualArgs (void) const {
	DASSERT(InFunction() && topsp);
	DASSERT((topsp + DELTA_TOPSP_TOTALARGS_OFFSET) < stackSize);
	return (util_ui16) stack[topsp + DELTA_TOPSP_TOTALARGS_OFFSET].ToNumber();
}

////////////////////////////////////////////////////////////////////
// Returns the local data size of the function called in the
// current activation record (including temporaries). This is 
// by definition topsp - top. In case of a library function, locals is 0.
//
util_ui16 DeltaDebugUserVarsInspector::TotalLocalData (void) const {
	DASSERT(InProgramFunction() && topsp && topsp >= top);
	return (util_ui16) (topsp - top);
}

////////////////////////////////////////////////////////////////////
// Get an actual argument by its relative order from the current
// activation record.
//
DeltaValue* DeltaDebugUserVarsInspector::GetActualArg (util_ui16 argNo) const {
	if (!InFunction())
		return (DeltaValue*) 0;
	else {
		DASSERT(argNo < TotalActualArgs() && topsp);
		DASSERT((topsp + DELTA_TOPSP_FIRSTARG_OFFSET + argNo) < stackSize);
		return stack + (topsp + DELTA_TOPSP_FIRSTARG_OFFSET + argNo);
	}
}

////////////////////////////////////////////////////////////////////
// Get a local var from the current activation record, by its relative
// order in the function definition. Notice that a local variable storage
// may syntactically map to many locally defined variables having non-overlapping
// lifetimes. Hence, viewing local data this way should be done together by analyzing
// the debug information in the text-generated file and always knowing the 
// active blocks->
//
DeltaValue* DeltaDebugUserVarsInspector::GetLocalVar (util_ui16 varNo) const {
	if (!InProgramFunction())
		return (DeltaValue*) 0;
	else {
		DASSERT(varNo < TotalLocalData() && varNo < topsp);
		DASSERT(topsp - varNo < stackSize);
		return stack + (topsp - varNo);
	}
}

////////////////////////////////////////////////////////////////////
// Get an actual argument from current activation record and put name
// information at 'putName'.
//
DeltaValue* DeltaDebugUserVarsInspector::GetActualArg (util_ui16 argNo, std::string& putName) const {
	DASSERT(InFunction() && argNo < TotalActualArgs());
	putName = currTracedProgramFuncDebugInfo ? currTracedProgramFuncDebugInfo->GetFormal(argNo) : UNKNOWN_ARG;
	return DNULLCHECK(GetActualArg(argNo));
}

////////////////////////////////////////////////////////////////////
// Get an actual argument by name. This requires debug information to resolve
// the name to a formal argument index, and use this to extract the object
// instance. Notice that not all actual arguments may be matched to formal 
// argument names, since, upon call, more actuals than formals may be supplied.
// If a formal is not supplied with an actual value then nil is returned.
//
DeltaValue* DeltaDebugUserVarsInspector::GetActualArg (const std::string& name) const {

	DASSERT(InFunction());

	util_ui16 argNo;
	if (InProgramFunctionWithDebugInformation()						&& 
		currTracedProgramFuncDebugInfo->GetFormal(name, &argNo))
		if (argNo < TotalActualArgs())
			return GetActualArg(argNo);
		else 
			return	MakePseudoVar(
						uconstructstr(
							"<formal arg '%s' not given an actual value>",
							name.c_str()
						)
					);
	else
		return (DeltaValue*) 0;
}

////////////////////////////////////////////////////////////////////
// Get the offset of a local variable using the block id stack.
// The block id stack concerns all calls at the call-stack. We know
// we finish with the current call when the main-block (0) is met.
//
bool DeltaDebugUserVarsInspector::GetLocalVarOffset (
		const DeltaVars&	vars,
		const std::string&	name, 
		util_ui16*			offset
	) const {

	DASSERT(InProgramFunction());

	// Get the block id stack of the currently traced 
	// function.
	//
	typedef DeltaBlockIdStacks::BlockList BlockList;
	const BlockList& blockIdStack = blocks->GetCurrBlockIdStack();

	bool	mainBlockMet = false;
	for (	BlockList::const_iterator i = blockIdStack.begin();
			!mainBlockMet;
			++i	) {

			DASSERT(i != blockIdStack.end());
			if (vars.GetVar(name, i->first, offset))
				return true;
			if (i->first == DELTA_MAIN_BLOCK_VALUE)
				mainBlockMet = true;
	}
	
	DASSERT(mainBlockMet);
	return false;
}

////////////////////////////////////////////////////////////////////

bool DeltaDebugUserVarsInspector::GetGlobalVarOffset (
		const DeltaVars&	vars, 
		const std::string&	name, 
		util_ui16*			offset
	) const {

	typedef DeltaBlockIdStacks::BlockList BlockList;
	const BlockList& globalBlocks = blocks->GetGlobalBlockIdStack();

	for (	BlockList::const_iterator i = globalBlocks.begin(); 
			i != globalBlocks.end(); 
			++i	)

			if (vars.GetVar(name, i->first, offset))
				return true;
	
	return false;
}

////////////////////////////////////////////////////////////////////
// Static variables are ensured to have unique names.
//
bool DeltaDebugUserVarsInspector::GetStaticVarOffset (
		const DeltaVars&	vars, 
		const std::string&	name, 
		util_ui16*			offset
	) const { return vars.GetVar(name, DELTA_STATICS_OWNING_BLOCKID, offset); }

////////////////////////////////////////////////////////////////////
// Get a local variable on a specified block.
//
DeltaValue* DeltaDebugUserVarsInspector::GetLocalVar (const std::string& name) const {

	if (!InProgramFunctionWithDebugInformation())
		return (DeltaValue*) 0;
	else {
		util_ui16 offset;
		if (GetLocalVarOffset(currTracedProgramFuncDebugInfo->Locals(), name, &offset)) {
			DASSERT(offset < topsp && (topsp - offset) < stackSize);
			return stack + (topsp - offset);
		}
		else
			return (DeltaValue*) 0;
	}
}

////////////////////////////////////////////////////////////////////
// The same as with local variables.
//
DeltaValue* DeltaDebugUserVarsInspector::GetGlobalVar (
		const DeltaVars&	globalVars,
		const std::string&	name, 
		bool				isStatic
	) const {

	util_ui16 offset;
	if (isStatic)
		if (GetStaticVarOffset(globalVars, name, &offset))
			return GetGlobalVar(offset);
		else
			return (DeltaValue*) 0;
	else
		if (GetGlobalVarOffset(globalVars, name, &offset))
			return GetGlobalVar(offset);
		else
			return (DeltaValue*) 0;
}

////////////////////////////////////////////////////////////////////

DeltaValue* DeltaDebugUserVarsInspector::GetClosureVar (const std::string& name) const {
	if (!closure || !InProgramFunction())
		return (DeltaValue*) 0;
	else {
		DASSERT(currTracedProgramFuncStdInfo);
		typedef DeltaStdFuncInfo::ClosureVarsInfo ClosureVarsInfo;
		const ClosureVarsInfo& info = currTracedProgramFuncStdInfo->GetClosureVarsInfo();
		util_ui32 offset = 0;
		for (ClosureVarsInfo::const_iterator i = info.begin(); i != info.end(); ++i, ++offset)
			if (i->GetName() == name)
				break;
		DASSERT(offset <= (util_ui32) info.size());
		return offset ==(util_ui16) info.size() ? (DeltaValue*) 0 : DPTR(closure)->GetVar(offset);
	}
}

////////////////////////////////////////////////////////////////////
// Vars names are provided with the following format.
// <fully qualified name>:<line>
//

#define	INSERT_GROUP_NAME(name)									\
		vars.push_front(										\
			std::pair<std::string, std::string>(				\
				DBG_GETVARIABLES_NAME_IS_GROUP,					\
				(name)											\
			)													\
		)

#define	IS_DEFINED_STATIC_VAR()									\
	(!val->IsUndefined() &&										\
	 ustrprefix(DELTA_STATICVAR_NAME_PREFIX, k->GetName())

void DeltaDebugUserVarsInspector::GetVariablesAtCurrentContext (
		const DeltaVars&									globalVars,
		std::list< std::pair<std::string, std::string> >&	vars
	) const {

	DASSERT(!InLibraryFunction());
	DASSERT(InGlobalCode() || InProgramFunctionWithDebugInformation());

	typedef DeltaBlockIdStacks::BlockList BlockList;

	const DeltaVars&	varDetails		= InGlobalCode() ? globalVars : currTracedProgramFuncDebugInfo->Locals();
	const BlockList&	activeBlocks	= blocks->GetCurrBlockIdStack();
	DeltaValue*			mem				= InGlobalCode() ?	globals : (stack + topsp);
	ubag<util_ui16>		offsetsUsed;	// Keeps offsets already checked at current context.

	// FUNCTION LOCALS / PROGRAM VARS.
	// Iterate from inner to outer block.

	bool includesStaticVars = false;

	for (BlockList::const_iterator i = activeBlocks.begin(); i != activeBlocks.end(); ++i)

		for (util_ui16 j = 0; j < varDetails.GetTotalBlocks(); ++j)

			if (j == i->first) { // If an active block.

				const std::list<DeltaDebugLocal>& blockVars = varDetails.GetBlock(j).GetLocals();

				// Iterate block vars from bottom to top.

				for (std::list<DeltaDebugLocal>::const_reverse_iterator k = blockVars.rbegin(); k != blockVars.rend(); ++k) {

					if (offsetsUsed.in(k->GetOffset()))					// It is an inactive var.
						continue;

					offsetsUsed.insert(k->GetOffset());

					if (k->GetName()[0] == DELTA_HIDDENVAR_ID_PREFIX)	// Is a hidden variable.
						continue;

					// Add vars in reverse order so that they appear following their
					// order of appearence in the source code.

					DeltaValue* val			= mem - k->GetOffset();
					bool		isStaticVar	= ustrprefix(DELTA_STATICVAR_NAME_PREFIX, k->GetName());

					if (k->GetLine() <= currLine || (!val->IsUndefined() && isStaticVar)) {
						vars.push_front(	
							std::pair<std::string, std::string>(
								uconstructstr(DBG_GETVARIABLES_VAR_FORMAT, k->GetName().c_str(), k->GetLine()),
								val->ConvertToStringBySkippingUserCode()
							)
						);

						if (isStaticVar)
							includesStaticVars = true;
					}
				}
			}

	DASSERT(InGlobalCode() || !includesStaticVars);

	if (vars.size())
		INSERT_GROUP_NAME(
			InGlobalCode()	? 
				includesStaticVars	? 
									"Main program vars (including statics)" 
									: "Main program vars" 
							: "Function locals"
		);

	// CLOSURE.
	// Check if we have also a closure whose vars must be added too.
	// Closure vars are added in front. 

	if (closure && InProgramFunctionWithDebugInformation()) {

		DASSERT(currTracedProgramFuncStdInfo);
		typedef DeltaStdFuncInfo::ClosureVarsInfo ClosureVarsInfo;
		const ClosureVarsInfo& info = currTracedProgramFuncStdInfo->GetClosureVarsInfo();

		util_ui32 offset = (util_ui32) info.size();

		for (ClosureVarsInfo::const_reverse_iterator i = info.rbegin(); i != info.rend(); ++i, --offset) {
			DASSERT(offset);
			DeltaValue* val = DPTR(closure)->GetVar(offset - 1);
			DASSERT(val);
			vars.push_front(
				std::pair<std::string, std::string>(
					uconstructstr(DBG_GETVARIABLES_VAR_FORMAT, i->GetName().c_str(), i->GetLine()), 
					val->ConvertToStringBySkippingUserCode()
				)
			);
		}

		if (info.size())
			INSERT_GROUP_NAME("Closure");
	}

	// ARGUMENTS.
	// Get formal arguments together with their supplied actual values.

	if (InProgramFunctionWithDebugInformation()) {

		util_ui32 k = TotalActualArgs() - TotalUserActualArgs();				// Reserved internal args.
		DASSERT(currTracedProgramFuncStdInfo->GetTotalFormals() >= k);			
		util_ui32 n = currTracedProgramFuncStdInfo->GetTotalFormals() - k;		// User formals only (formals always include reserved).

		for (util_ui32 i = umax((util_ui32) TotalUserActualArgs(), n); i; --i) {

			std::string name;
			DeltaValue* val	= (DeltaValue*) 0;
			util_ui32	j	= i + k - 1;

			if (i > n)	{ // Actual supplied in addition to the defined formals.
				name = uconstructstr(DBG_GETVARIABLES_EXTRA_ACTUAL_FORMAT, i - 1);
				val	= GetActualArg(j);
			}
			else {
				name = currTracedProgramFuncStdInfo->GetFormalName(j);	// Get only user args.
				if (i <= TotalUserActualArgs()) {						// Actual value for formal arg supplied.
					val = DNULLCHECK(GetActualArg(name));
					DASSERT(val == GetActualArg(j));					// Should get the same result this way.
				}
			}

			vars.push_front(
				std::pair<std::string, std::string>(
					uconstructstr(
						DBG_GETVARIABLES_VAR_FORMAT, 
						name.c_str(), 
						currTracedProgramFuncDebugInfo->GetLine()
					), 
					val ? val->ConvertToStringBySkippingUserCode() : DBG_NO_ACTUAL_VALUE_FOR_FORMAL
				)
			);
		}

		if (n)
			INSERT_GROUP_NAME("Arguments");
	}
}

////////////////////////////////////////////////////////////////////

util_ui32 DeltaDebugUserVarsInspector::GetStoredPC (void) const {
	DASSERT(InFunction());
	return (util_ui32) stack[topsp + DELTA_TOPSP_PC_OFFSET].ToNumber();
}

////////////////////////////////////////////////////////////////////
