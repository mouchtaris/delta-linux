// TableIterLib.cpp
// ScriptFighter Project.
// A. Savidis, July 2009.
//

#include "DeltaLibraryCreators.h"
#include "TableIterLibP.h"
#include "TableIterLib.h"
#include "DeltaStdLibFuncNames.h"

/////////////////////////////////////////////////////
// The following are required by the GET_ITERATOR, so we prepare
// its context.

#define	NameTraits			TableNameTraits
#define	TIadapted			TableIteratorAdapter
#define	OriginalIterator	TableIteratorAdapter::Original

static void Reset(DeltaVirtualMachine* vm)	
	{ RESET_NIL_RETURNVALUE }

static void tableiter_getindex_LibFunc(DeltaVirtualMachine* vm) {

	ISSUE_FUNC("tableiter_getindex");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

	GET_ITERATOR(0, Reset);
	CHECK_INVALID_ITERATOR(RESET_NIL_RETURNVALUE);
	CHECK_END_ITERATOR(RESET_NIL_RETURNVALUE);

	nativeIter->GetIndex(&vm->GetReturnValue());
}

#undef	NameTraits			
#undef	TIadapted			
#undef	OriginalIterator	

/////////////////////////////////////////////////////

class TableIteratorHandlerClass : public DeltaVirtualMachine::TableIteratorHandler {
	public:
	virtual void Make (DeltaValue* at) const { DeltaTableIter_Make(at); }
};

class TableIteratorHandler : public usingleton<TableIteratorHandlerClass>{};
USINGLETON_INSTANCE(usingleton<TableIteratorHandlerClass>)

/////////////////////////////////////////////////////

ITERATOR_GETTERS_DEF(TableIteratorLibrary)
ITERATOR_CONSTRUCTOR_DEF(iter_constructor, TableIteratorLibrary)

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ TableNameTraits::iter,		iter_constructor					},
	{ TableNameTraits::setbegin,	TableIteratorLibrary::setbegin		},
	{ TableNameTraits::setend,		TableIteratorLibrary::setend		},
	{ TableNameTraits::checkend,	TableIteratorLibrary::checkend		},
	{ TableNameTraits::checkbegin,	TableIteratorLibrary::checkbegin	},
	{ TableNameTraits::equal,		TableIteratorLibrary::equal			},
	{ TableNameTraits::assign,		TableIteratorLibrary::assign		},
	{ TableNameTraits::copy,		TableIteratorLibrary::copy			},
	{ TableNameTraits::fwd, 		TableIteratorLibrary::fwd			},
	{ TableNameTraits::getcontainer, TableIteratorLibrary::getcontainer	},
	{ TableNameTraits::getcounter, 	TableIteratorLibrary::getcounter	},
	{ TableNameTraits::getval, 		TableIteratorLibrary::getval		},
	{ TableNameTraits::setval, 		TableIteratorLibrary::setval		},
	{ TableNameTraits::getindex, 	tableiter_getindex_LibFunc			}
};

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, TableNameTraits::setbegin, TableNameTraits::getindex)

inline bool VAL_IS_TABLEITER (const DeltaValue& val)
	{ return val.Type() == DeltaValue_ExternId && val.GetExternIdTypeString() == TABLEITER_TYPE_STR; }

DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(VAL_IS_TABLEITER(val), TABLEITER_TYPE_STR)

DVM_FUNC void DeltaTableIter_Make (DeltaValue* at) 
	{ TableIteratorLibrary::Make(at, TableIteratorLibraryGetter); }

void TableNameTraits::add_methods (DeltaValue* at) 
	{  DELTALIBFUNC_DELEGATE_METHODS(*at); }

TableContainerAdapter* GET_CONTAINER_TEMPLATE<TableContainerAdapter>::adapter = (TableContainerAdapter*) 0;

static void Initialise (void) {
	ITERATOR_GETTERS_INIT(TableIteratorLibrary); 
	unew(GET_CONTAINER_TEMPLATE<TableContainerAdapter>::adapter);
	TableIteratorHandler::SingletonCreate();
	DeltaVirtualMachine::SetTableIteratorHandler(TableIteratorHandler::GetSingletonPtr());
	DELTALIBFUNC_CREATE_METHODS_TABLE();
}
static void CleanUp (void) {
	ITERATOR_GETTERS_CLEANUP(TableIteratorLibrary); 
	udelete(GET_CONTAINER_TEMPLATE<TableContainerAdapter>::adapter);
	TableIteratorHandler::SingletonDestroy();
	DELTALIBFUNC_DESTROY_METHODS_TABLE();
}

DELTA_LIBRARY_SUBAPI_INSTALLER_EX(DeltaTableIter, "tableiter", DELTA_STDLIB_NAMESPACE_PREFIX "tableiter_", Initialise(), CleanUp())

/////////////////////////////////////////////////////

const char* TableNameTraits::iter			= "new";
const char* TableNameTraits::setbegin		= "setbegin";
const char* TableNameTraits::setend			= "setend";
const char* TableNameTraits::checkend		= "checkend";
const char* TableNameTraits::checkbegin		= "checkbegin";
const char* TableNameTraits::equal			= "equal";
const char* TableNameTraits::assign			= "assign";
const char* TableNameTraits::copy			= "copy";
const char* TableNameTraits::fwd			= "fwd";
const char* TableNameTraits::getcontainer	= "getcontainer";
const char* TableNameTraits::getcounter		= "getcounter";
const char* TableNameTraits::setval			= "setval";
const char* TableNameTraits::getval			= "getval";
const char* TableNameTraits::getindex		= "getindex";
const char* TableNameTraits::iter2string	= TABLEITER_TYPE_STR;
const char* TableNameTraits::itertypestring	= TABLEITER_TYPE_STR;
const char*	TableNameTraits::cont2string	= "Table / Object";
const char* TableNameTraits::conttypestring	= "Table / Object";
			
//////////////////////////////////////////////////////
