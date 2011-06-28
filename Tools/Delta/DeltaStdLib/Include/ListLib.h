// ListLib.h
// Garbage collectable list type API for delta, built
// via STL and integrated via the EGC mechanism.
// ScriptFighter Project.
// A. Savidis, June 2001.
//

#ifndef	LISTLIB_H
#define	LISTLIB_H

#include "DeltaLibraryObjectCreator.h"
#include "DeltaValue.h"
#include "DeltaStdClassNames.h"

#include <list>

#define	DEFORMAT_LISTANDITERATORMISMATCH \
			"Iterator / List mismatch in '%s'"

#define	DEFORMAT_ITERATOR_LISTS_MISMATCH \
			"Iterators refer to different lists in '%s'"

#define	DEFORMAT_INVALIDLISTITERATOR \
			"List iterator is invalid in '%s'"

#define	DEFORMAT_LISTENDITERATOR \
			"List iterator reached 'end' in '%s'"

//////////////////////////////////////////////////////////////////////
// If the object is not a list, the caller is responsible to
// generate a run-time error. The list pointer returned is not
// to be verified via DPTR.
//
class DVM_CLASS DeltaValue;

DVM_FUNC std::list<DeltaValue>*	DeltaList_Get (void* val, const std::string& typeStr);
DVM_FUNC std::list<DeltaValue>*	DeltaList_Get (DeltaValue& obj);
DVM_FUNC void*					DeltaList_GetCollectableContainer (DeltaValue& obj);
DVM_FUNC void					DeltaList_Make (DeltaValue& obj);
DVM_FUNC void					DeltaList_MakeForDebuggerOnly (DeltaValue& obj);

//////////////////////////////////////////////////////////////////////

// Variables required are: argObj, val, typeStr.
//
#define	GET_LISTEX(var, func, retvalreset,	argno)									\
	DeltaGetArg(argObj, argno, DeltaValue_ExternId, "ExternId", func, retvalreset); \
	val = argObj->ToExternId(typeStr);												\
	std::list<DeltaValue>* var = DeltaList_Get(val, typeStr);						\
	if (!var) {																		\
		DeltaErrorExternId(argno, LIST_TYPE_STR, typeStr.c_str(), func);			\
		retvalreset																	\
	}

extern void Install_DeltaList_Lib (DeltaObject* mainAPI);
extern void CleanUp_DeltaList_Lib (void);

#endif	// Do not add stuff beyond this point.