#include "DeltaJSONParserLib.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "JsonLoaderAPI.h"
#include "JsonLoaderErrorMsg.h"

////////////////////////////////////////////////////////////////

static DeltaTable* Load (const std::string& path, bool retainNull) 
	{ return JsonLoaderAPI::Load (path, retainNull); }

static DeltaTable* Parse (const std::string& data, bool retainNull) 
	{ return JsonLoaderAPI::LoadFromString (data, retainNull); }

static const std::string GetError (void) 
	{ return JsonParserLoaderErrorMsg::JsonLoaderGetError(); }

static bool Store (DeltaTable* json, const std::string& path) 
	{ return JsonLoaderAPI::Write(path.c_str(), json); }

static const DeltaValue Null (void)
	{ return JsonLoaderAPI::Null(); }

////////////////////////////////////////////////////////////////

static void jsonload_LibFunc (DeltaVirtualMachine* vm);
static void jsonparse_LibFunc (DeltaVirtualMachine* vm);
static void jsonloadgeterror_LibFunc (DeltaVirtualMachine* vm);
static void jsonstore_LibFunc (DeltaVirtualMachine* vm);
static void jsonnull_LibFunc (DeltaVirtualMachine* vm);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "json::load",			jsonload_LibFunc			},
	{ "json::parse",		jsonparse_LibFunc			},
	{ "json::loadgeterror",	jsonloadgeterror_LibFunc	},
	{ "json::store",		jsonstore_LibFunc			},
	{ "json::null",			jsonnull_LibFunc			}
};

////////////////////////////////////////////////////////////////

#define	LOADER(name, func)													\
DLIB_FUNC_START(name, 2, Nil)												\
DLIB_ARG(const char*,	source)												\
DLIB_ARG(bool,	retainNull)													\
if (DeltaTable* json = func(DLIB_ARGVAL(source), DLIB_ARGVAL(retainNull)))	\
	DLIB_RETVAL_REF.FromTable(json);										\
else																		\
	DLIB_RETVAL_REF.FromNil();												\
DLIB_FUNC_END

LOADER(jsonload, Load)
LOADER(jsonparse, Parse)

DLIB_FUNC_START(jsonloadgeterror, 0, Nil)
DLIB_RETVAL_REF.FromString(GetError());
DLIB_FUNC_END

DLIB_FUNC_START(jsonstore, 2, False)
DLIB_ARG(DeltaValue*,	json)
DLIB_ARG(const char*,	path)
DLIB_ERROR_CHECK(
	DLIB_ARGVAL(json)->Type() != DeltaValue_Table,
	uconstructstr(
		"in '%s' JSON object expected ('%s' supplied)",
		CURR_FUNC, 
		DLIB_ARGVAL(json)->TypeStr()
	)
)
DLIB_RETVAL_REF.FromBool(Store(DLIB_ARGVAL(json)->ToTable(), DLIB_ARGVAL(path)));
DLIB_FUNC_END

DLIB_FUNC_START(jsonnull, 0, Empty)
DLIB_RETVAL_REF.Assign(Null());
DLIB_FUNC_END

////////////////////////////////////////////////////////////////

DJSONPARSER_CFUNC void Install_DeltaJSONParser_Lib (void) {
	JsonLoaderAPI::SingletonCreate();
	DELTA_LIBRARY_INSTALL(funcs);
}

DJSONPARSER_CFUNC void CleanUp_DeltaJSONParser_Lib (void) {
	JsonLoaderAPI::SingletonDestroy();
	DELTA_LIBRARY_UNINSTALL(funcs);
}

////////////////////////////////////////////////////////////////

DJSONPARSER_CFUNC void* Install (void) {
	Install_DeltaJSONParser_Lib();
	return udynamiclibloader::New("CleanUp", true);	 // Explicitly supply a cleaner.
}

DJSONPARSER_CFUNC void* CleanUp (void) {
	CleanUp_DeltaJSONParser_Lib();
	return udynamiclibloader::New("dll cleared");
}

////////////////////////////////////////////////////////////////
