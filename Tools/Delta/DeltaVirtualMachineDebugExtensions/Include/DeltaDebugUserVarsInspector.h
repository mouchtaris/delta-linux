// DeltaDebugUserVarsInspector.h
// Supports inspection of user variables for the debugger backend.
// ScriptFighter Project.
// A. Savidis, June 2006 (original).
// RF to a separate class (February 2010).
//

#ifndef	DELTADEBUGUSERVARSINSPECTOR_H
#define	DELTADEBUGUSERVARSINSPECTOR_H

#include "DeltaDebugExtensionsStdDefs.h"
#include "DeltaValue.h"
#include "DeltaFunc.h"
#include "BlockIdStacks.h"
#include "DDebug.h"
#include "utypes.h"

/////////////////////////////////////////////////////////////////

class DeltaDebugUserVarsInspector {

	DFRIENDDESTRUCTOR()

	private:
	enum Context {
		ContextProgramFunction	= 0,
		ContextLibraryFunction	= 1,
		ContextGlobalCode		= 2,
		ConextUndef				= 3
	};

	DeltaValue* 				stack;
	util_ui32					stackSize;
	DeltaValue*					globals;
	util_ui32					totalGlobals;
	util_ui32					topsp, top;
	const DeltaBlockIdStacks*	blocks;
	const DeltaDebugFuncInfo*	currTracedProgramFuncDebugInfo;
	const DeltaStdFuncInfo*		currTracedProgramFuncStdInfo;
	const char*					currTracedLibFunc;
	const DeltaClosure*			closure;
	Context						context;
	util_ui32					currLine;	// Source line at current context.
	mutable DeltaValue			pseudoVar;	// Just to return string messages.

	bool			InFunction (void) const
						{ return context == ContextProgramFunction || context == ContextLibraryFunction; }
	bool			InProgramFunction (void) const
						{ return context == ContextProgramFunction; }
	bool			InLibraryFunction (void) const
						{ return context == ContextLibraryFunction; }
	bool			InGlobalCode (void) const
						{ return context == ContextGlobalCode; }
	bool			InProgramFunctionWithDebugInformation (void) const
						{ return InProgramFunction() && currTracedProgramFuncDebugInfo; }
	DeltaValue*		MakePseudoVar (const std::string& s) const
						{ pseudoVar.FromString(s); return &pseudoVar; }

	/////////////////////////////////////////////////////////////////

	public:
	util_ui16		TotalActualArgs (void) const;
	util_ui16		TotalUserActualArgs (void) const;
	util_ui16		TotalLocalData (void) const;
	util_ui16		TotalGlobalData (void) const
						{ return totalGlobals; }

	bool			GetLocalVarOffset (
						const DeltaVars&	vars,		// DBG INFO
						const std::string&	name, 
						util_ui16*			offset
					) const;

	bool			GetGlobalVarOffset (
						const DeltaVars&	vars,		// DBG INFO
						const std::string&	name, 
						util_ui16*			offset
					) const;
			
	bool			GetStaticVarOffset (
						const DeltaVars&	vars,		// DBG INFO
						const std::string&	name, 
						util_ui16*			offset
					) const;

	DeltaValue*		GetActualArg (util_ui16 argNo) const;
	DeltaValue*		GetLocalVar (util_ui16 varNo) const;
	DeltaValue*		GetGlobalVar (util_ui16 offset) const;

	DeltaValue*		GetActualArg (util_ui16 argNo, std::string& putName) const;
	DeltaValue*		GetActualArg (const std::string& name) const;
	DeltaValue*		GetLocalVar (const std::string& name) const;
	DeltaValue*		GetGlobalVar (
						const DeltaVars&	globalVars, // DBG INFO
						const std::string&	name, 
						bool				isStatic = false
					)const;
	DeltaValue*		GetClosureVar (const std::string& name) const;
	void			GetVariablesAtCurrentContext (
						const DeltaVars&									globalVars,	// DBG INFO
						std::list< std::pair<std::string, std::string> >&	vars
					) const;
	util_ui32		GetStoredPC (void) const;

	// Setting stack pointers, closure, context and traced function must be done
	// appropriately in sync to the backend.

	void			SetCurrTracedFunction (const char* libFunc);
	void			SetCurrTracedFunction (
						const DeltaDebugFuncInfo*	funcDebugInfo, 
						const DeltaStdFuncInfo*		funcStdInfo
					);
	void			SetCurrTracedIsGlobalCode (void);
	void			ResetCurrTracedFunction (void);

	void			SetStackPointers (util_ui32 _top, util_ui32 _topsp, const DeltaClosure* _closure)
						{ top = _top, topsp = _topsp, closure = _closure; }
	void			SetCurrLine (util_ui32 line)
						{ currLine = line; }
	void			SetDataPointers (
						DeltaValue*					_stack, 
						util_ui32					_stackSize,
						DeltaValue*					_globals,
						util_ui32					_totalGlobals,
						const DeltaBlockIdStacks&	_blocks
					);

	util_ui32		GetTop (void) const 
						{ return top; }
	util_ui32		GetTopSp (void) const 
						{ return topsp; }

	DeltaDebugUserVarsInspector (void);
	~DeltaDebugUserVarsInspector();
};

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
