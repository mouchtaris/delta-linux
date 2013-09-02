// DeltaAOPLib.cpp
// AOP library for Delta.
// ScriptFighter Project.
// Y. Lilis, August 2013.
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
#include "ListLib.h"

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
const AOPLibrary::ASTList matches = AOPLibrary::Match(ast, pointcut.GetValue());

DeltaList_Make(DLIB_RETVAL_REF);
DLIB_SET_CREATOR_INFORMATION();
std::list<DeltaValue>*	nativeList	= DeltaList_Get(DLIB_RETVAL_REF);
void*					listCont	= DeltaList_GetCollectableContainer(DLIB_RETVAL_REF);
for (AOPLibrary::ASTList::const_iterator i = matches.begin(); i != matches.end(); ++i) {
	DeltaValue val;
	DeltaAST_Make(&val, *i);
	nativeList->push_back(val);
	nativeList->back().SetResidencyType(
		DeltaValue::Contained,
		listCont
	);
}
DLIB_FUNC_END

DLIB_FUNC_START(aop_advise, 3, Empty)
DLIB_GET_AST
DLIB_GET_ADVICE_TYPE(adviceType)
DLIB_GET_AST_EX(advice)
AOPLibrary::Advise(ast, adviceType, advice);
DLIB_FUNC_END

////////////////////////////////////////////////////////////////

static void Install_DeltaAOP_Lib (void) {
	AOPLibrary::Initialise();
	DELTA_LIBRARY_INSTALL(funcs);
}

static void CleanUp_DeltaAOP_Lib (void) {
	AOPLibrary::CleanUp();
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
