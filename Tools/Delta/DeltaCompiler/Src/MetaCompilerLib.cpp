// MetaCompilerLib.cpp.
// Standard library functions for the MetaCompiler.
// ScriptFighter Project.
// Y. Lilis, July 2011.
//

#include "DeltaLibraryCreators.h"
#include "DeltaMetaCompiler.h"
#include "ASTLib.h"

//------------------------------------------------------------------

static void inline_LibFunc (DeltaVirtualMachine*);
static void context_LibFunc (DeltaVirtualMachine*);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "inline",		inline_LibFunc },
	{ "context",	context_LibFunc }
};

///////////////////////////////////////////////

void Install_DeltaMetaCompiler_Lib (void)
	{ DELTA_LIBRARY_INSTALL_EX(funcs, DELTA_STDLIB_NAMESPACE_PREFIX); }

void CleanUp_DeltaMetaCompiler_Lib (void)
	{ DELTA_LIBRARY_UNINSTALL_EX(funcs, DELTA_STDLIB_NAMESPACE_PREFIX); }

///////////////////////////////////////////////
// inline(val).
// Inline the given ast or const value to the
// next empty inline of the current stage.
//
DLIB_FUNC_START(inline, 1, Empty)
DLIB_ARG(DeltaValue*, val)
	if (DeltaMetaCompiler* compiler = DeltaMetaCompiler::GetCompilerForVM(vm))
		DPTR(compiler)->Inline(DLIB_ARGVAL(val));
	else {
		DPTR(vm)->PrimaryError("Invalid call to %s", CURR_FUNC);
		DLIB_RESET_RETURN;
	}
DLIB_FUNC_END

///////////////////////////////////////////////
// context([tag]).
// Retrieve the next inline context (AST).
// Entry point to access the main AST, but only for read access.
//
DLIB_FUNC_START_OPT_ARGS(context, 0, 1, Nil)
DLIB_OPT_ARG(const char*, tag, "")
	if (DeltaMetaCompiler* compiler = DeltaMetaCompiler::GetCompilerForVM(vm))
		DeltaAST_Make(DLIB_RETVAL_PTR, DPTR(compiler)->GetInlineContext(DLIB_ARGVAL(tag)));
	else {
		DPTR(vm)->PrimaryError("Invalid call to %s", CURR_FUNC);
		DLIB_RESET_RETURN;
	}
DLIB_FUNC_END

//------------------------------------------------------------------
