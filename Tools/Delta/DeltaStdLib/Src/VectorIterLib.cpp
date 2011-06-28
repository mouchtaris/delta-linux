// VectorIterLib.cpp
// Implementation of vector iterator.
// ScriptFighter Project.
// A. Savidis, July 2009.
//

#include "DeltaLibraryCreators.h"
#include "VectorLib.h"
#include "VectorIterLib.h"
#include "VectorLibP.h"
#include "DeltaStdLibFuncNames.h"

/////////////////////////////////////////////////////
// The following are required by the GET_ITERATOR, so we prepare
// its context.

#define	NameTraits			VectorNameTraits
#define	TIadapted			VectorIteratorAdapter
#define	OriginalIterator	VectorIteratorAdapter::Original

static void Reset(DeltaVirtualMachine* vm)	
	{ RESET_NIL_RETURNVALUE }

static void vectoriter_getindex_LibFunc(DeltaVirtualMachine* vm) {

	ISSUE_FUNC("tableiter_getindex");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

	GET_ITERATOR(0, Reset);
	CHECK_INVALID_ITERATOR(RESET_NIL_RETURNVALUE);
	CHECK_END_ITERATOR(RESET_NIL_RETURNVALUE);

	DPTR(vm)->GetReturnValue().FromNumber(nativeIter.getindex());
}

#undef	NameTraits			
#undef	TIadapted			
#undef	OriginalIterator	

/////////////////////////////////////////////////////

ITERATOR_GETTERS_DEF(VectorIteratorLibrary)
ITERATOR_CONSTRUCTOR_DEF(iter_constructor, VectorIteratorLibrary)

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ VectorNameTraits::iter,			iter_constructor					},
	{ VectorNameTraits::setbegin,		VectorIteratorLibrary::setbegin		},
	{ VectorNameTraits::setend,			VectorIteratorLibrary::setend		},
	{ VectorNameTraits::checkend,		VectorIteratorLibrary::checkend		},
	{ VectorNameTraits::checkbegin,		VectorIteratorLibrary::checkbegin	},
	{ VectorNameTraits::equal,			VectorIteratorLibrary::equal		},
	{ VectorNameTraits::assign,			VectorIteratorLibrary::assign		},
	{ VectorNameTraits::copy,			VectorIteratorLibrary::copy			},
	{ VectorNameTraits::fwd, 			VectorIteratorLibrary::fwd			},
	{ VectorNameTraits::bwd, 			VectorIteratorLibrary::bwd			},
	{ VectorNameTraits::getcontainer, 	VectorIteratorLibrary::getcontainer	},
	{ VectorNameTraits::getcounter, 	VectorIteratorLibrary::getcounter	},
	{ VectorNameTraits::getval, 		VectorIteratorLibrary::getval		},
	{ VectorNameTraits::setval, 		VectorIteratorLibrary::setval		},
	{ VectorNameTraits::getindex, 		vectoriter_getindex_LibFunc			}
};

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, VectorNameTraits::setbegin, VectorNameTraits::getindex)

inline bool VAL_IS_VECTORITER (const DeltaValue& val)
	{ return val.Type() == DeltaValue_ExternId && val.GetExternIdTypeString() == VECTORITER_TYPE_STR; }

DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(VAL_IS_VECTORITER(val), VECTORITER_TYPE_STR)

void VectorNameTraits::add_methods (DeltaValue* at) 
	{  DELTALIBFUNC_DELEGATE_METHODS(*at); }

DVM_FUNC void DeltaVectorIter_Make (DeltaValue* at) 
	{ VectorIteratorLibrary::Make(at, VectorIteratorLibraryGetter); }

static void Initialise (void) 
	{ ITERATOR_GETTERS_INIT(VectorIteratorLibrary); DELTALIBFUNC_CREATE_METHODS_TABLE(); }

static void CleanUp (void) 
	{ ITERATOR_GETTERS_CLEANUP(VectorIteratorLibrary); DELTALIBFUNC_DESTROY_METHODS_TABLE(); }

DELTA_LIBRARY_SUBAPI_INSTALLER_EX(DeltaVectorIter, "vectoriter", DELTA_STDLIB_NAMESPACE_PREFIX "vectoriter_", Initialise(), CleanUp())

/////////////////////////////////////////////////////

const char* VectorNameTraits::iter				= "new";
const char* VectorNameTraits::setbegin			= "setbegin";
const char* VectorNameTraits::setend			= "setend";
const char* VectorNameTraits::checkend			= "checkend";
const char* VectorNameTraits::checkbegin		= "checkbegin";
const char* VectorNameTraits::equal				= "equal";
const char* VectorNameTraits::assign			= "assign";
const char* VectorNameTraits::copy				= "copy";
const char* VectorNameTraits::fwd				= "fwd";
const char* VectorNameTraits::bwd				= "bwd";
const char* VectorNameTraits::getcontainer		= "getcontainer";
const char* VectorNameTraits::getcounter		= "getcounter";
const char* VectorNameTraits::setval			= "setval";
const char* VectorNameTraits::getval			= "getval";
const char* VectorNameTraits::getindex			= "getindex";
const char* VectorNameTraits::iter2string		= VECTORITER_TYPE_STR;
const char* VectorNameTraits::itertypestring	= VECTORITER_TYPE_STR;
const char*	VectorNameTraits::cont2string		= VECTOR_TYPE_STR;
const char* VectorNameTraits::conttypestring	= VECTOR_TYPE_STR;
			
//////////////////////////////////////////////////////
