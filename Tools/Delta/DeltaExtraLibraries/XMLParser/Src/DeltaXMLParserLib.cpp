// DeltaXMLParserLib.cpp
// XML parser library fro Delta.
// ScriptFighter Project.
// A. Savidis, July 2009.
//

#include "DeltaXMLParserLib.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "XmlLoaderAPI.h"
#include "XmlLoaderErrorMsg.h"

////////////////////////////////////////////////////////////////

static DeltaTable* Load (const std::string& path) 
	{ return XmlLoaderAPI::Load (path); }

static DeltaTable* Parse (const std::string& data) 
	{ return XmlLoaderAPI::LoadFromString (data); }

static const std::string GetError (void) 
	{ return XmlLoaderGetError(); }

static bool Store (DeltaTable* xml, const std::string& path) 
	{ return XmlLoaderAPI::Write(path.c_str(), xml); }

////////////////////////////////////////////////////////////////

static void xmlload_LibFunc (DeltaVirtualMachine* vm);
static void xmlparse_LibFunc (DeltaVirtualMachine* vm);
static void xmlloadgeterror_LibFunc (DeltaVirtualMachine* vm);
static void xmlstore_LibFunc (DeltaVirtualMachine* vm);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "xml::load",			xmlload_LibFunc			},
	{ "xml::parse",			xmlparse_LibFunc		},
	{ "xml::loadgeterror",	xmlloadgeterror_LibFunc },
	{ "xml::store",			xmlstore_LibFunc		}
};

////////////////////////////////////////////////////////////////

#define	LOADER(name, func)							\
DLIB_FUNC_START(name, 1, Nil)						\
DLIB_ARG(const char*,	source)						\
if (DeltaTable* xml = func(DLIB_ARGVAL(source)))	\
	DLIB_RETVAL_REF.FromTable(xml);					\
else												\
	DLIB_RETVAL_REF.FromNil();						\
DLIB_FUNC_END

LOADER(xmlload,	 Load)
LOADER(xmlparse, Parse)

DLIB_FUNC_START(xmlloadgeterror, 0, Nil)
DLIB_RETVAL_REF.FromString(GetError());
DLIB_FUNC_END

DLIB_FUNC_START(xmlstore, 2, False)
DLIB_ARG(DeltaValue*,	xml)
DLIB_ARG(const char*,	path)
DLIB_ERROR_CHECK(
	DLIB_ARGVAL(xml)->Type() != DeltaValue_Table,
	uconstructstr(
		"in '%s' XML object expected ('%s' supplied)",
		CURR_FUNC, 
		DLIB_ARGVAL(xml)->TypeStr()
	)
)
DLIB_RETVAL_REF.FromBool(Store(DLIB_ARGVAL(xml)->ToTable(), DLIB_ARGVAL(path)));
DLIB_FUNC_END

////////////////////////////////////////////////////////////////

DXMLPARSER_CFUNC void Install_DeltaXMLParser_Lib (void) {
	XmlLoaderAPI::SingletonCreate();
	DELTA_LIBRARY_INSTALL(funcs);
}

DXMLPARSER_CFUNC void CleanUp_DeltaXMLParser_Lib (void) {
	XmlLoaderAPI::SingletonDestroy();
	DELTA_LIBRARY_UNINSTALL(funcs);
}

////////////////////////////////////////////////////////////////

DXMLPARSER_CFUNC void* Install (void) {
	Install_DeltaXMLParser_Lib();
	return udynamiclibloader::New("CleanUp", true);	 // Explicitly supply a cleaner.
}

DXMLPARSER_CFUNC void* CleanUp (void) {
	CleanUp_DeltaXMLParser_Lib();
	return udynamiclibloader::New("dll cleared");
}

////////////////////////////////////////////////////////////////
