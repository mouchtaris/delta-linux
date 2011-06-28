// ListIterLib.cpp
// Implementation of most STL API for iterators.
// ScriptFighter Project.
// A. Savidis, June 2001.
// Refactored November 2006, adding more facilities
// for iteration.
// Refactored December 2007, encompassed in the 'std' API object. 
//

#include "DeltaLibraryCreators.h"
#include "ListLib.h"
#include "ListLibP.h"
#include <list>

/////////////////////////////////////////////////////

ITERATOR_GETTERS_DEF(ListIteratorLibrary)
ITERATOR_CONSTRUCTOR_DEF(iter_constructor, ListIteratorLibrary)

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ ListNameTraits::iter,			iter_constructor					},
	{ ListNameTraits::setbegin,		ListIteratorLibrary::setbegin		},
	{ ListNameTraits::setend,		ListIteratorLibrary::setend			},
	{ ListNameTraits::checkend,		ListIteratorLibrary::checkend		},
	{ ListNameTraits::checkbegin,	ListIteratorLibrary::checkbegin 	},
	{ ListNameTraits::equal,		ListIteratorLibrary::equal			},
	{ ListNameTraits::assign,		ListIteratorLibrary::assign			},
	{ ListNameTraits::copy,			ListIteratorLibrary::copy			},
	{ ListNameTraits::fwd, 			ListIteratorLibrary::fwd			},
	{ ListNameTraits::bwd, 			ListIteratorLibrary::bwd			},
	{ ListNameTraits::getcontainer, ListIteratorLibrary::getcontainer	},
	{ ListNameTraits::getcounter, 	ListIteratorLibrary::getcounter		},
	{ ListNameTraits::getval, 		ListIteratorLibrary::getval			},
	{ ListNameTraits::setval, 		ListIteratorLibrary::setval			}
};

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, ListNameTraits::setbegin, ListNameTraits::setval)

inline bool VAL_IS_LISTITER (const DeltaValue& val)
	{ return val.Type() == DeltaValue_ExternId && val.GetExternIdTypeString() ==LISTITER_TYPE_STR; }

DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(VAL_IS_LISTITER(val), LISTITER_TYPE_STR)

void ListNameTraits::add_methods (DeltaValue* at) 
	{ DELTALIBFUNC_DELEGATE_METHODS(*at); }

DVM_FUNC void DeltaListIter_Make (DeltaValue* at) 
	{ ListIteratorLibrary::Make(at, ListIteratorLibraryGetter); }

static void Initialise (void) 
	{ ITERATOR_GETTERS_INIT(ListIteratorLibrary);  DELTALIBFUNC_CREATE_METHODS_TABLE(); }

static void CleanUp (void) 
	{ ITERATOR_GETTERS_CLEANUP(ListIteratorLibrary); DELTALIBFUNC_DESTROY_METHODS_TABLE();  }

DELTA_LIBRARY_SUBAPI_INSTALLER_EX(DeltaListIter, "listiter", DELTA_STDLIB_NAMESPACE_PREFIX "listiter_", Initialise(), CleanUp())

/////////////////////////////////////////////////////

const char* ListNameTraits::iter			= "new";
const char* ListNameTraits::setbegin		= "setbegin";
const char* ListNameTraits::setend			= "setend";
const char* ListNameTraits::checkend		= "checkend";
const char* ListNameTraits::checkbegin		= "checkbegin";
const char* ListNameTraits::equal			= "equal";
const char* ListNameTraits::assign			= "assign";
const char* ListNameTraits::copy			= "copy";
const char* ListNameTraits::fwd				= "fwd";
const char* ListNameTraits::bwd				= "bwd";
const char* ListNameTraits::getcontainer	= "getcontainer";
const char* ListNameTraits::getcounter		= "getcounter";
const char* ListNameTraits::setval			= "setval";
const char* ListNameTraits::getval			= "getval";
const char* ListNameTraits::iter2string		= LISTITER_TYPE_STR;
const char* ListNameTraits::itertypestring	= LISTITER_TYPE_STR;
const char*	ListNameTraits::cont2string		= LIST_TYPE_STR;
const char* ListNameTraits::conttypestring	= LIST_TYPE_STR;
			
//////////////////////////////////////////////////////
