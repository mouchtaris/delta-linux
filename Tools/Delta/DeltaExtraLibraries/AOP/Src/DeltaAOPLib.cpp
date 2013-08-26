// DeltaXMLParserLib.cpp
// XML parser library fro Delta.
// ScriptFighter Project.
// A. Savidis, July 2009.
//

#include "DeltaAOPLib.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"

#include "AOPLibrary.h"
#include "ASTLib.h"

////////////////////////////////////////////////////////////////

static void aop_aspect_LibFunc (DeltaVirtualMachine* vm);
static void aop_match_LibFunc (DeltaVirtualMachine* vm);
static void aop_advise_LibFunc (DeltaVirtualMachine* vm);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "aop::aspect",	aop_aspect_LibFunc	},
	{ "aop::match",		aop_match_LibFunc	},
	{ "aop::advise",	aop_advise_LibFunc	}
};

////////////////////////////////////////////////////////////////

#define DLIB_GET_ADVICE_TYPE(name)												\
	DLIB_ARG(const char*, __##name)												\
	if (!AOPLibrary::IsValidAdviceType(__##name.GetValue())) {					\
		DPTR(vm)->PrimaryError(													\
			"invalid advice type '%s' passed to '%s'",							\
			ucstringarg(__##name.GetValue()),									\
			CURR_FUNC															\
		);																		\
		DLIB_RESET_RETURN;														\
	}																			\
	AOPLibrary::AdviceType name = AOPLibrary::ToAdviceType(__##name.GetValue());


DLIB_FUNC_START(aop_aspect, 4, Empty)
DLIB_GET_AST
DLIB_ARG(const char*, pointcut)
DLIB_GET_ADVICE_TYPE(adviceType)
DLIB_GET_AST_EX(advice)
AOPLibrary::Aspect(ast, pointcut.GetValue(), adviceType, advice);
DLIB_FUNC_END

DLIB_FUNC_START(aop_match, 2, Nil)
DLIB_GET_AST
DLIB_ARG(const char*, pointcut)
AOPLibrary::Match(ast, pointcut.GetValue());
DLIB_FUNC_END

DLIB_FUNC_START(aop_advise, 3, Empty)
DLIB_GET_AST
DLIB_GET_ADVICE_TYPE(adviceType)
DLIB_GET_AST_EX(advice)
AOPLibrary::Advise(ast, adviceType, advice);
DLIB_FUNC_END

////////////////////////////////////////////////////////////////

static void Install_DeltaAOP_Lib (void) {
	//AOPLibrary::SingletonCreate();
	DELTA_LIBRARY_INSTALL(funcs);
}

static void CleanUp_DeltaAOP_Lib (void) {
	//AOPLibrary::SingletonDestroy();
	DELTA_LIBRARY_UNINSTALL(funcs);
}

////////////////////////////////////////////////////////////////

DAOPLIB_CFUNC void* Install (void) {
	Install_DeltaAOP_Lib();
	return udynamiclibloader::New("CleanUp", true);	 // Explicitly supply a cleaner.
}

DAOPLIB_CFUNC void* CleanUp (void) {
	CleanUp_DeltaAOP_Lib();
	return udynamiclibloader::New("dll cleared");
}

////////////////////////////////////////////////////////////////
