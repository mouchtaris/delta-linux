// DeltaStdLib.cpp
// Installing Delta standard library.
// ScriptFighter Project.
// A. Savidis, December 1999.
//

#include "DeltaLibraryCreators.h"
#include "DeltaStdLib.h"
#include "MiscLib.h"
#include "TableLib.h"
#include "StringLib.h"
#include "MathLib.h"
#include "FileIOLib.h"
#include "ShMemLib.h"
#include "BitOpLib.h"
#include "VMRegLib.h"
#include "VMCompLib.h"
#include "ListLib.h"
#include "ListIterLib.h"
#include "TableIterLib.h"
#include "VectorLib.h"
#include "VectorIterLib.h"
#include "ObjectLib.h"
#include "BufferLib.h"
#include "ReaderWriterLib.h"
#include "SocketsLib.h"
#include "AlgorithmLib.h"
#include "ASTLib.h"
#include "ASTVisitorLib.h"

static DeltaObject* api = (DeltaObject*) 0;
DELTA_LIBRARY_GETAPI_IMPL(std_getapi,api)

class StdUtilities : public usingleton<DeltaStdUtilities>{};
USINGLETON_INSTANCE(usingleton<DeltaStdUtilities>)

//////////////////////////////////////

void Install_StdLib (void) {

	api = DeltaObject::NewObject();
	DPTR(api)->IncRefCounter((DeltaValue*) 0);
	DeltaLibFuncBinder::Add("std_getapi", std_getapi_LibFunc);
	StdUtilities::SingletonCreate();
	DeltaVirtualMachine::SetStdUtilities(StdUtilities::GetSingletonPtr());

	Install_DeltaList_Lib(api);
	Install_DeltaMisc_Lib(api);
	Install_DeltaTable_Lib(api);
	Install_DeltaString_Lib(api);
	Install_DeltaMath_Lib(api);
	Install_DeltaFile_Lib(api);
	Install_DeltaShMem_Lib(api);
	Install_DeltaBitOp_Lib(api);
	Install_DeltaVM_Lib(api);
	Install_DeltaVMComp_Lib(api);
	Install_DeltaListIter_Lib(api);	
	Install_DeltaTableIter_Lib(api);	
	Install_DeltaVector_Lib(api);
	Install_DeltaVectorIter_Lib(api);
	Install_DeltaObject_Lib(api);
	Install_DeltaInputBuffer_Lib(api);
	Install_DeltaOutputBuffer_Lib(api);
	Install_DeltaReaderWriter_Lib(api);
	Install_DeltaSockets_Lib(api);
	Install_DeltaAlgorithm_Lib(api);
	Install_DeltaAST_Lib(api);
	Install_DeltaASTVisitor_Lib(api);
}

//////////////////////////////////////

void CleanUp_StdLib (void) {

	DELTA_COLLECTOR_ENTER();

		CleanUp_DeltaList_Lib();
		CleanUp_DeltaMisc_Lib();
		CleanUp_DeltaTable_Lib();
		CleanUp_DeltaString_Lib();
		CleanUp_DeltaMath_Lib();
		CleanUp_DeltaFile_Lib();
		CleanUp_DeltaShMem_Lib();
		CleanUp_DeltaBitOp_Lib();
		CleanUp_DeltaVM_Lib();
		CleanUp_DeltaVMComp_Lib();
		CleanUp_DeltaListIter_Lib();
		CleanUp_DeltaTableIter_Lib();
		CleanUp_DeltaVector_Lib();
		CleanUp_DeltaVectorIter_Lib();
		CleanUp_DeltaObject_Lib();
		CleanUp_DeltaInputBuffer_Lib();
		CleanUp_DeltaOutputBuffer_Lib();
		CleanUp_DeltaReaderWriter_Lib();
		CleanUp_DeltaSockets_Lib();
		CleanUp_DeltaAlgorithm_Lib();
		CleanUp_DeltaAST_Lib();
		CleanUp_DeltaASTVisitor_Lib();

		DeltaVirtualMachine::ResetStdUtilities();
		StdUtilities::SingletonDestroy();

		DeltaLibraryObjectCreator::DestroyAPI(api);

	DELTA_COLLECTOR_EXIT();
}

//////////////////////////////////////

static void SetPath (const std::string& path, void (*set)(const std::string&, bool), bool prioritised) {
	std::list<std::string> paths(utokenizestr(path, ";,"));
	for (std::list<std::string>::iterator i = paths.begin(); i != paths.end(); ++i) 
		(*set)(*i, prioritised);
}

DVM_FUNC void SetByteCodeLoadingPath_StdLib (const std::string& path, bool prioritised) 
	{ SetPath(path, &DeltaVM_AddLoadingPath, prioritised); }

DVM_FUNC void SetDllImportPath_StdLib (const std::string& path, bool prioritised)
	{ SetPath(path, &DeltaAdd_dllimport_Path, prioritised); }

//////////////////////////////////////

#define	TUPLE_MAKE()								\
	DeltaTable* t = DeltaTableFactory::New();		\
	DeltaValue index

#define	ADD_FIELD(i)								\
	if (!val##i.IsNil())							\
		DPTR(t)->Set(DeltaValue(id##i), val##i);	\
	else

DVM_FUNC DeltaTable* DeltaMakeTupple (const DeltaTupleFields& fields) {
	TUPLE_MAKE();
	for (DeltaTupleFields::const_iterator i = fields.begin(); i!= fields.end(); ++i)
		if (!i->second.IsNil())
			DPTR(t)->Set(DeltaValue(i->first), i->second);
	return t;
}

DVM_FUNC DeltaTable* DeltaMakeTupple (DELTATUPLE_FIELD(1)) {
	TUPLE_MAKE();
	ADD_FIELD(1);
	return t;
}

DVM_FUNC DeltaTable* DeltaMakeTupple (DELTATUPLE_FIELD(1), DELTATUPLE_FIELD(2)) {
	TUPLE_MAKE();
	ADD_FIELD(1); ADD_FIELD(2);
	return t;
}

DVM_FUNC DeltaTable* DeltaMakeTupple (DELTATUPLE_FIELD(1), DELTATUPLE_FIELD(2), DELTATUPLE_FIELD(3)) {
	TUPLE_MAKE();
	ADD_FIELD(1); ADD_FIELD(2); ADD_FIELD(3);
	return t;
}

DVM_FUNC DeltaTable* DeltaMakeTupple (DELTATUPLE_FIELD(1), DELTATUPLE_FIELD(2), DELTATUPLE_FIELD(3), DELTATUPLE_FIELD(4)) {
	TUPLE_MAKE();
	ADD_FIELD(1); ADD_FIELD(2); ADD_FIELD(3); ADD_FIELD(4);
	return t;
}

//////////////////////////////////////
