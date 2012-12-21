// VMCompLib.cpp
// Virtual machine compiler related functions.
// ScriptFigher Project.
// A. Savidis, December 1999.
// Moved here from VMRegLib. 
//

#include "DeltaLibraryCreators.h"
#include "VMCompLib.h"
#include "VMRegLib.h"
#include "DeltaStdLibFuncNames.h"
#include "DeltaStdClassNames.h"
#include "BufferLib.h"
#include "ReaderWriterLib.h"
#include "ListLib.h"
#include "ASTLib.h"

/////////////////////////////////////////////////////

BuildDependenciesIFace::ConstructorFunc	BuildDependenciesIFace::ctor = (BuildDependenciesIFace::ConstructorFunc) 0;
BuildDependenciesIFace::DestructorFunc	BuildDependenciesIFace::dtor = (BuildDependenciesIFace::DestructorFunc) 0;

void BuildDependenciesIFace::Install (ConstructorFunc create, DestructorFunc destroy)
	{ ctor = create, dtor = destroy; }

BuildDependenciesIFace* BuildDependenciesIFace::New (void) {
	DASSERT(ctor);
	return DNULLCHECK((*ctor)());
}

void BuildDependenciesIFace::Delete (BuildDependenciesIFace* instance) {
	DASSERT(dtor);
	(*dtor)(instance);
}

/////////////////////////////////////////////////////

CompilerIFace::InitialiseFunc	CompilerIFace::initialiser	= (CompilerIFace::InitialiseFunc) 0;
CompilerIFace::CleanUpFunc		CompilerIFace::cleaner		= (CompilerIFace::CleanUpFunc) 0;
CompilerIFace::ConstructorFunc	CompilerIFace::ctor			= (CompilerIFace::ConstructorFunc) 0;
CompilerIFace::DestructorFunc	CompilerIFace::dtor			= (CompilerIFace::DestructorFunc) 0;

void CompilerIFace::Install (ConstructorFunc create, DestructorFunc destroy, InitialiseFunc init, CleanUpFunc cleanUp) {
	ctor		= create;
	dtor		= destroy;
	initialiser = init;
	cleaner		= cleanUp;
	DASSERT(ctor && dtor);	//init and cleanUp are optional
}

void CompilerIFace::Initialise (void) {
	if (initialiser)
		(*initialiser)();
}

void CompilerIFace::CleanUp (void) {
	if (cleaner)
		(*cleaner)();
}

CompilerIFace* CompilerIFace::New (void) {
	DASSERT(ctor);
	return DNULLCHECK((*ctor)());
}

void CompilerIFace::Delete (CompilerIFace* instance) {
	DASSERT(dtor);
	(*dtor)(instance);
}

/////////////////////////////////////////////////////

static void vmparse_LibFunc (DeltaVirtualMachine* vm);
static void vmparsestring_LibFunc (DeltaVirtualMachine* vm);
static void vmparsequotedelements_LibFunc (DeltaVirtualMachine* vm);
static void vmtranslate_LibFunc (DeltaVirtualMachine* vm);
static void vmtranslatetooutputbuffer_LibFunc (DeltaVirtualMachine* vm);
static void vmtranslateonwriter_LibFunc (DeltaVirtualMachine* vm);
static void vmcomp_LibFunc (DeltaVirtualMachine* vm);
static void vmcomptooutputbuffer_LibFunc (DeltaVirtualMachine* vm);
static void vmcomponwriter_LibFunc (DeltaVirtualMachine* vm);
static void vmcompstring_LibFunc (DeltaVirtualMachine* vm);
static void vmcompstringtooutputbuffer_LibFunc (DeltaVirtualMachine* vm);
static void vmcompstringonwriter_LibFunc (DeltaVirtualMachine* vm);
static void vmextractbuilddeps_LibFunc (DeltaVirtualMachine* vm);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "parse",						vmparse_LibFunc						},
	{ "parsestring",				vmparsestring_LibFunc				},
	{ "parsequotedelements",		vmparsequotedelements_LibFunc		},
	{ "translate",					vmtranslate_LibFunc					},
	{ "translatetooutputbuffer",	vmtranslatetooutputbuffer_LibFunc	},
	{ "translateonwriter",			vmtranslateonwriter_LibFunc			},
	{ "comp",						vmcomp_LibFunc						},
	{ "comptooutputbuffer",			vmcomptooutputbuffer_LibFunc		},
	{ "componwriter",				vmcomponwriter_LibFunc				},
	{ "compstring",					vmcompstring_LibFunc				},
	{ "compstringtooutputbuffer",	vmcompstringtooutputbuffer_LibFunc	},
	{ "compstringonwriter",			vmcompstringonwriter_LibFunc		},
	{ "extractbuilddeps",			vmextractbuilddeps_LibFunc			}
};

/////////////////////////////////////////////////////////////////////////

static void Initialise (void)
	{ CompilerIFace::Initialise(); }

static void CleanUp(void)
	{ CompilerIFace::CleanUp(); }

DELTA_LIBRARY_SUBAPI_INSTALLER_EX(DeltaVMComp, "vm", DELTA_STDLIB_NAMESPACE_PREFIX "vm", Initialise(), CleanUp())

//-----------------------------------------------------------------------
// DYNAMIC COMPILATION.

typedef std::pair<DeltaValue*, bool> OnErrorData;

static void OnErrorWrapper (const char* error, void* closure) {
	OnErrorData* data = (OnErrorData*) closure;
	DASSERT(data && data->first);
	if (data->second)
		data->second = (*data->first)(DeltaValue(error));
}

///////////////////////////////////////////////////////////////////////////

#define	BEFORE_PARSING()															\
	CompilerIFace* compiler = CompilerIFace::New();									\
	DPTR(compiler)->AddExternFuncs(DeltaStdLib_FuncNames());							\
	OnErrorData onErrorData(DLIB_ARGVAL(errorHandler), true);						\
	DPTR(compiler)->SetErrorCallback(umakecallback(&OnErrorWrapper, &onErrorData));	\
	const std::string byteCodePath = DeltaVM_MakeLoadingPath();						\
	if (!byteCodePath.empty())														\
		DPTR(compiler)->SetByteCodePath(byteCodePath)

#define	AFTER_PARSING()																\
	CompilerIFace::Delete(DPTR(compiler));											\
	if (!onErrorData.second) {														\
		DPTR(vm)->PossiblePrimaryError(												\
			"in '%s' while calling error reporting function!",						\
			CURR_FUNC																\
		);																			\
		DLIB_RESET_RETURN;															\
		return;																		\
	} else

#define BEFORE_COMPILATION()														\
	BEFORE_PARSING();																\
	DPTR(compiler)->SetProductionMode(DLIB_ARGVAL(productionMode))

#define AFTER_COMPILATION()															\
	AFTER_PARSING()

#define	DUMP_TEXT_CODE(path)														\
	if (!DLIB_ARGVAL(productionMode)) {												\
		std::string prefix = ufileprefix(DLIB_ARGVAL(path));						\
		DPTR(compiler)->DumpTextCode((prefix + ".txt").c_str());						\
	} else

///////////////////////////////////////////////////////////////////////////

#define	REGISTER_IMPL(name, func)													\
	DLIB_FUNC_START(name, 2, Nil)													\
	DLIB_ARG(const char*,	pathOrText)												\
	DLIB_ARG(DeltaValue*,	errorHandler)											\
	BEFORE_PARSING();																\
	TreeNode* ast = DPTR(compiler)->func(DLIB_ARGVAL(pathOrText));					\
	if (ast && onErrorData.second)													\
		DeltaAST_Make(DLIB_RETVAL_PTR, ast->Clone(TreeNode::DefaultCreator()));		\
	else																			\
		DLIB_RETVAL_REF.FromNil();													\
	AFTER_COMPILATION();															\
	DLIB_FUNC_END

REGISTER_IMPL(vmparse,					Parse)
REGISTER_IMPL(vmparsestring,			ParseText)
REGISTER_IMPL(vmparsequotedelements,	ParseQuotedElements)

///////////////////////////////////////////////////////////////////////////

#define FILE_RESULT()											\
	if (result) {												\
		DPTR(compiler)->DumpBinaryCode(DLIB_ARGVAL(destPath));	\
		DUMP_TEXT_CODE(destPath);								\
	}

#define BUFFER_RESULT()											\
	if (result) {												\
		PortableBufferWriter writer(*DPTR(ob));					\
		DPTR(compiler)->DumpBinaryCode(writer);					\
		DeltaOutputBuffer_Make(DLIB_RETVAL_PTR, ob);			\
	}															\
	else {														\
		DDELETE(ob);											\
		DLIB_RETVAL_REF.FromNil();								\
	}

#define WRITER_RESULT()											\
	if (result)													\
		DPTR(compiler)->DumpBinaryCode(*DPTR(writer));

///////////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(vmtranslate, 4, False)
DLIB_GET_AST
DLIB_ARG(const char*,	destPath)
DLIB_ARG(DeltaValue*,	errorHandler)
DLIB_ARG(bool,			productionMode)

BEFORE_COMPILATION();
bool result = DPTR(compiler)->Translate(DPTR(ast)) && onErrorData.second;
FILE_RESULT();
AFTER_COMPILATION();
DLIB_RETVAL_REF.FromBool(result);
DLIB_FUNC_END

///////////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(vmtranslatetooutputbuffer, 3, Nil)
DLIB_GET_AST
DLIB_ARG(DeltaValue*,	errorHandler)
DLIB_ARG(bool,			productionMode)

BEFORE_COMPILATION();
ubinaryio::OutputBuffer* ob = DNEW(ubinaryio::OutputBuffer);
bool result = DPTR(compiler)->Translate(DPTR(ast)) && onErrorData.second;
BUFFER_RESULT();
AFTER_COMPILATION();
DLIB_FUNC_END

///////////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(vmtranslateonwriter, 4, False)
DLIB_GET_AST
DLIB_GET_WRITER
DLIB_ARG(DeltaValue*,	errorHandler)
DLIB_ARG(bool,			productionMode)

BEFORE_COMPILATION();
bool result = DPTR(compiler)->Translate(DPTR(ast)) && onErrorData.second;
WRITER_RESULT();
AFTER_COMPILATION();
DLIB_RETVAL_REF.FromBool(result);
DLIB_FUNC_END

///////////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(vmcomp, 4, False)
DLIB_ARG(const char*,	srcPath)
DLIB_ARG(const char*,	destPath)
DLIB_ARG(DeltaValue*,	errorHandler)
DLIB_ARG(bool,			productionMode)

BEFORE_COMPILATION();
bool result = DPTR(compiler)->Compile(DLIB_ARGVAL(srcPath)) && onErrorData.second;
FILE_RESULT();
AFTER_COMPILATION();
DLIB_RETVAL_REF.FromBool(result);
DLIB_FUNC_END

///////////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(vmcomptooutputbuffer, 3, Nil)
DLIB_ARG(const char*,	srcPath)
DLIB_ARG(DeltaValue*,	errorHandler)
DLIB_ARG(bool,			productionMode)

BEFORE_COMPILATION();
ubinaryio::OutputBuffer* ob = DNEW(ubinaryio::OutputBuffer);
bool result = DPTR(compiler)->Compile(DLIB_ARGVAL(srcPath)) && onErrorData.second;
BUFFER_RESULT();
AFTER_COMPILATION();
DLIB_FUNC_END

///////////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(vmcomponwriter, 4, False)
DLIB_ARG(const char*,	srcPath)
DLIB_GET_WRITER
DLIB_ARG(DeltaValue*,	errorHandler)
DLIB_ARG(bool,			productionMode)

BEFORE_COMPILATION();
bool result = DPTR(compiler)->Compile(DLIB_ARGVAL(srcPath)) && onErrorData.second;
WRITER_RESULT();
AFTER_COMPILATION();
DLIB_RETVAL_REF.FromBool(result);
DLIB_FUNC_END

///////////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(vmcompstring, 4, False)
DLIB_ARG(const char*,	code)
DLIB_ARG(const char*,	destPath)
DLIB_ARG(DeltaValue*,	errorHandler)
DLIB_ARG(bool,			productionMode)

BEFORE_COMPILATION();
bool result = DPTR(compiler)->CompileText(DLIB_ARGVAL(code)) && onErrorData.second;
FILE_RESULT();
AFTER_COMPILATION();
DLIB_RETVAL_REF.FromBool(result);
DLIB_FUNC_END

///////////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(vmcompstringtooutputbuffer, 3, Nil)
DLIB_ARG(const char*,	code)
DLIB_ARG(DeltaValue*,	errorHandler)
DLIB_ARG(bool,			productionMode)

BEFORE_COMPILATION();
ubinaryio::OutputBuffer* ob = DNEW(ubinaryio::OutputBuffer);
bool result = DPTR(compiler)->CompileText(DLIB_ARGVAL(code)) && onErrorData.second;
BUFFER_RESULT();
AFTER_COMPILATION();
DLIB_FUNC_END

///////////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(vmcompstringonwriter, 4, False)
DLIB_ARG(const char*,	code)
DLIB_GET_WRITER
DLIB_ARG(DeltaValue*,	errorHandler)
DLIB_ARG(bool,			productionMode)

BEFORE_COMPILATION();
bool result = DPTR(compiler)->CompileText(DLIB_ARGVAL(code)) && onErrorData.second;
WRITER_RESULT();
AFTER_COMPILATION();
DLIB_RETVAL_REF.FromBool(result);
DLIB_FUNC_END

///////////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(vmextractbuilddeps, 1, Nil)
DLIB_ARG(const char*, srcPath)
BuildDependenciesIFace* dependencyBuilder = BuildDependenciesIFace::New();
if (!dependencyBuilder->Extract("", DLIB_ARGVAL(srcPath)))
	DLIB_RETVAL_REF.FromNil();
else {
	DeltaList_Make(DLIB_RETVAL_REF);
	std::list<DeltaValue>* l = 	DeltaList_Get(DLIB_RETVAL_REF);
	const BuildDependenciesIFace::Dependencies deps = dependencyBuilder->GetDependencies();
	for (BuildDependenciesIFace::Dependencies::const_iterator i = deps.begin(); i != deps.end(); ++i) {
		l->push_back(i->first);
		switch (i->second) {
			case BuildDependenciesIFace::NotFound	: l->push_back(DELTA_DEPENDENCY_NOT_FOUND);		break;
			case BuildDependenciesIFace::OneFound	: l->push_back(DELTA_DEPENDENCY_ONE_FOUND);		break;
			case BuildDependenciesIFace::ManyFound	: l->push_back(DELTA_DEPENDENCY_MANY_FOUND);		break;
			default : DASSERT(false);
		}
	}
}
BuildDependenciesIFace::Delete(dependencyBuilder);
DLIB_FUNC_END

///////////////////////////////////////////////////////////////////////////
