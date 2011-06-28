// TableLib.cpp.
// Standard library functions for tables.
// ScriptFighter Project.
// A. Savidis, November 1999.
// Moved to a separate file, November 2006.
// Updated July 2007, changing the handling of nomadic methods.
// Refactored December 2007, encompassed in the 'std' API object. 
// Refactored November 2008 to use the new templates for crafting
// library functions.
//

#include "DeltaLibraryCreators.h"
#include <stdio.h>
#include "TableLib.h"
#include "DeltaStdLibFuncNames.h"
#include "DeltaTableSerialiser.h"
#include "BufferLib.h"

//------------------------------------------------------------------

static void tablength_LibFunc (DeltaVirtualMachine* vm);
static void tabnewattribute_LibFunc (DeltaVirtualMachine* vm);
static void tabredefineattribute_LibFunc (DeltaVirtualMachine* vm);
static void tabsetattribute_LibFunc (DeltaVirtualMachine* vm);
static void tabgetattribute_LibFunc (DeltaVirtualMachine* vm);
static void tabset_LibFunc (DeltaVirtualMachine* vm);
static void tabget_LibFunc (DeltaVirtualMachine* vm);
static void tabindices_LibFunc (DeltaVirtualMachine* vm);
static void tabcopy_LibFunc (DeltaVirtualMachine* vm);
static void tabclear_LibFunc (DeltaVirtualMachine* vm);
static void tabenableoverloading_LibFunc (DeltaVirtualMachine* vm);
static void tabdisableoverloading_LibFunc (DeltaVirtualMachine* vm);
static void tabisoverloadingenabled_LibFunc (DeltaVirtualMachine* vm);
static void tabsetmethod_LibFunc (DeltaVirtualMachine* vm);
static void tabmethodonme_LibFunc (DeltaVirtualMachine* vm);
static void tabextend_LibFunc (DeltaVirtualMachine* vm);
static void tabserialise_LibFunc (DeltaVirtualMachine* vm);
static void tabdeserialise_LibFunc (DeltaVirtualMachine* vm);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "length",					tablength_LibFunc				},
	{ "set",					tabset_LibFunc					},
	{ "newattribute",			tabnewattribute_LibFunc			},
	{ "redefineattribute",		tabredefineattribute_LibFunc	},
	{ "setattribute",			tabsetattribute_LibFunc			},
	{ "getattribute",			tabgetattribute_LibFunc			},
	{ "get",					tabget_LibFunc					},
	{ "indices",				tabindices_LibFunc				},
	{ "copy",					tabcopy_LibFunc					},
	{ "enableoverloading",		tabenableoverloading_LibFunc	},
	{ "disableoverloading",		tabdisableoverloading_LibFunc	},
	{ "isoverloadingenabled",	tabisoverloadingenabled_LibFunc },
	{ "clear",					tabclear_LibFunc				},
	{ "setmethod",				tabsetmethod_LibFunc			},
	{ "methodonme",				tabmethodonme_LibFunc			},
	{ "extend",					tabextend_LibFunc				},
	{ "serialise",				tabserialise_LibFunc			},
	{ "deserialise",			tabdeserialise_LibFunc			}
};

DELTA_LIBRARY_SUBAPI_INSTALLER(DeltaTable, "table", DELTA_STDLIB_NAMESPACE_PREFIX "tab")

//------------------------------------------------------------------
// tablength(x).
// Returns total number of entries in a table.
//
DLIB_FUNC_START(tablength, 1, Zero)
DLIB_ARG(DeltaTable*, t)
DLIB_CALL_RETURN(Number, DLIB_ARGVAL(t)->Total) 
DLIB_CALL_CLOSE
DLIB_FUNC_END

//------------------------------------------------------------------
// tabset(t,i,c).
// It is the native table bounded set operator.
//
DLIB_FUNC_START(tabset, 3, Empty)
DLIB_ARG(DeltaTable*,	table)
DLIB_ARG(DeltaValue*,	index)
DLIB_ARG(DeltaValue*,	content)
DLIB_CALL_CHECK_OPEN(DLIB_ARGVAL(table)->DeltaTable::BasicSet)
	*DLIB_ARGVAL(index),
	*DLIB_ARGVAL(content)
DLIB_CALL_CHECK_CLOSE_POSSIBLE_PRIMARY	// due to nested attribute calls
DLIB_FUNC_END

//------------------------------------------------------------------
// tabget(t,i).
// It is the native table bounded get operator.
//
DLIB_FUNC_START(tabget, 2, Nil)
DLIB_ARG(DeltaTable*,	table)
DLIB_ARG(DeltaValue*,	index)
DLIB_CALL_CHECK_OPEN(DLIB_ARGVAL(table)->DeltaTable::BasicGet)
	*DLIB_ARGVAL(index),
	DLIB_RETVAL_PTR
DLIB_CALL_CHECK_CLOSE_POSSIBLE_PRIMARY	// due to nested attribute calls
DLIB_FUNC_END

//------------------------------------------------------------------
// tabnewattribute(t, id, set, get).
//
DLIB_FUNC_START(tabnewattribute, 4, Empty)
DLIB_ARG(DeltaTable*,	table)
DLIB_ARG(const char*,	id)
DLIB_ARG(DeltaValue*,	set)
DLIB_ARG(DeltaValue*,	get)
DLIB_CALL_CHECK_OPEN(DLIB_ARGVAL(table)->DeltaTable::NewAttribute)
	DLIB_ARGVAL(id),
	*DLIB_ARGVAL(set),
	*DLIB_ARGVAL(get)
DLIB_CALL_CHECK_CLOSE
DLIB_FUNC_END

//------------------------------------------------------------------
// tabredefineattribute(t, id, set, get).
//
DLIB_FUNC_START(tabredefineattribute, 4, Empty)
DLIB_ARG(DeltaTable*,	table)
DLIB_ARG(const char*,	id)
DLIB_ARG(DeltaValue*,	set)
DLIB_ARG(DeltaValue*,	get)
DeltaTable::Element* attr = DLIB_ARGVAL(table)->GetAttribute(DLIB_ARGVAL(id));
DLIB_ERROR_CHECK(
	!attr, 
	uconstructstr(
		"in call ('%s') no attribute '%s' found", 
		DLIB_CALLSIG_CROPPED, 
		DLIB_ARGVAL(id)
	)
)
DLIB_ARGVAL(table)->Remove(DeltaValue(DLIB_ARGVAL(id)));
DLIB_CALL_CHECK_OPEN(DLIB_ARGVAL(table)->DeltaTable::NewAttribute)
	DLIB_ARGVAL(id),
	*DLIB_ARGVAL(set),
	*DLIB_ARGVAL(get)
DLIB_CALL_CHECK_CLOSE

DLIB_FUNC_END

//------------------------------------------------------------------
// tabgetattribute(t,id).
// It is the native table bounded get operator.
//
DLIB_FUNC_START(tabgetattribute, 2, Nil)
DLIB_ARG(DeltaTable*,	table)
DLIB_ARG(const char*,	id)
if (DeltaTable::Element* attr = DLIB_ARGVAL(table)->GetAttribute(DLIB_ARGVAL(id)))
	DPTR(attr)->GetContentDirectly(DLIB_RETVAL_PTR);
else
	DLIB_RETVAL_REF.FromNil();
DLIB_FUNC_END

//------------------------------------------------------------------
// tabsetattribute(t, id, val).
// It is the native table bounded get operator.
//
DLIB_FUNC_START(tabsetattribute, 3, Empty)
DLIB_ARG(DeltaTable*,	table)
DLIB_ARG(const char*,	id)
DLIB_ARG(DeltaValue*,	val)
if (DLIB_ARGVAL(val)->Type() == DeltaValue_Nil) {
	DLIB_ERROR_CHECK(
		!DLIB_ARGVAL(table)->Remove(DLIB_ARGVAL(id)), 
		uconstructstr(
			"in call ('%s') failed to remove '%s' attribute",
			DLIB_CALLSIG_CROPPED, 
			DLIB_ARGVAL(id)
		)
	)
}
DeltaTable::Element* attr = DLIB_ARGVAL(table)->GetAttribute(DLIB_ARGVAL(id));
DLIB_ERROR_CHECK(
	!attr, 
	uconstructstr(
		"in call ('%s') no attribute '%s' found", 
		DLIB_CALLSIG_CROPPED, 
		DLIB_ARGVAL(id)
	)
)
DPTR(attr)->SetContentDirectly(DLIB_ARGVAL(val));
DLIB_FUNC_END

//------------------------------------------------------------------
// tabindices(x).
// Makes a table  with all indices of x, and returns it.
//
DLIB_FUNC_START(tabindices,1,Nil)
DLIB_ARG(DeltaTable*, t)
DLIB_CALL_RETURN(Table, DLIB_ARGVAL(t)->GetIndices)
DLIB_CALL_CLOSE
DLIB_FUNC_END

//------------------------------------------------------------------
// tabcopy(x).
// Makes a copy of a table as a new table with the same
// elements.
//
DLIB_FUNC_START(tabcopy, 1, Nil)
DLIB_ARG(DeltaTable*, t)
DeltaTable* copy = DLIB_ARGVAL(t)->Copy();
DLIB_ERROR_CHECK(
	!copy, 
	uconstructstr(
		"in call ('%s') there were errors while copying", 
		DLIB_CALLSIG_CROPPED
	)
)
DLIB_RETVAL_REF.FromTable(copy);
DPTR(copy)->GetCreationInfo().SetCreatorInformation(
	vm->Source(),
	vm->Line(),
	CURR_FUNC
);
DLIB_FUNC_END

//------------------------------------------------------------------
// tabclear(t).
// Clears a table.
//
DLIB_FUNC_START(tabclear, 1, Empty)
DLIB_ARG(DeltaTable*, t)
DLIB_ARGVAL(t)->Clear();
DLIB_FUNC_END

//------------------------------------------------------------------
// tabenableoverloading(t).
// Enables overloading.
//
DLIB_FUNC_START(tabenableoverloading, 1, Empty)
DLIB_ARG(DeltaTable*, t)
DLIB_ARGVAL(t)->EnableOverloading();
DLIB_FUNC_END

// tabdisableoverloading(t).
// Disables overloading.
//
DLIB_FUNC_START(tabdisableoverloading, 1, Empty)
DLIB_ARG(DeltaTable*, t)
DLIB_ARGVAL(t)->DisableOverloading();
DLIB_FUNC_END

// Bool tabisoverloadingenabled(t).
// Returns if overloading is enabled.
//
DLIB_FUNC_START(tabisoverloadingenabled, 1, False)
DLIB_ARG(DeltaTable*, t)
DLIB_RETVAL_REF.FromBool(DLIB_ARGVAL(t)->IsOverloadingEnabled());
DLIB_FUNC_END

//------------------------------------------------------------------
// tabsetmethod(t1, t2, i, j).
// t1[i] = t2[j]; t1[i].self = t1;
// t2[j] should be a method.
//
DLIB_FUNC_START(tabsetmethod, 4, Empty)
	DLIB_ARG(DeltaTable*,	t1)
	DLIB_ARG(DeltaTable*,	t2)
	DLIB_ARG(DeltaValue*,	i)
	DLIB_ARG(DeltaValue*,	j)

	DeltaValue method;
	bool result = DLIB_ARGVAL(t2)->Get(*DLIB_ARGVAL(j), &method);
	DLIB_ERROR_CHECK(
		!result, 
		uconstructstr("in call ('%s') getting method t2[j]", DLIB_CALLSIG_CROPPED)
	)
	DLIB_ERROR_CHECK(
		method.Type() == DeltaValue_Nil, 
		uconstructstr("in call ('%s') t2[j] not found", DLIB_CALLSIG_CROPPED)
	)
	DLIB_ERROR_CHECK(
		method.Type() != DeltaValue_MethodFunc, 
		uconstructstr(
			"in call ('%s') t2[j] is '%s' not a method", 
			DLIB_CALLSIG_CROPPED, 
			method.TypeStr()
		)
	)
	DeltaCodeAddress		addr;
	DeltaVirtualMachine*	methodVM;
	DeltaTable*				self;
	method.ToMethodFunc(&addr, &self, &methodVM);
	DPTR(DLIB_ARGVAL(t1))->Set(
		*DLIB_ARGVAL(i), 
		DeltaValue(addr, DLIB_ARGVAL(t1), methodVM)
	);	
DLIB_FUNC_END

//------------------------------------------------------------------
// tabmethodforme(t,m).
// Returns an method value with self being t.
//
DLIB_FUNC_START(tabmethodonme, 2, Empty)
	DLIB_ARG(DeltaTable*,	t)
	DLIB_ARG(DeltaValue*,	m)
	DLIB_ERROR_CHECK(
		DLIB_ARGVAL(m)->Type() != DeltaValue_MethodFunc, 
		uconstructstr(
			"in call ('%s') method expected ('%s' passed)", 
			DLIB_CALLSIG_CROPPED, 
			DLIB_ARGVAL(m)->TypeStr()
		)
	)
	DLIB_RETVAL_REF.Assign(DLIB_ARGVAL(m));
	DLIB_RETVAL_REF.ChangeMethodFuncSelf(DLIB_ARGVAL(t));
DLIB_FUNC_END

//------------------------------------------------------------------
// tabextend(t1,t2).
// t1 += t1-t2, i.e. adds elements of t2 not in t1 to t1, setting as their 
// owner (member funcs only) t1.
//
DLIB_FUNC_START(tabextend, 2, Empty)
DLIB_ARG(DeltaTable*,	dest)
DLIB_ARG(DeltaTable*,	src)
DPTR(DLIB_ARGVAL(dest))->Extend(DLIB_ARGVAL(src));
DLIB_FUNC_END

//------------------------------------------------------------------
// outputbuffer tabserialise(t).
// Serialises a table to an output buffer.
//
DLIB_FUNC_START(tabserialise, 1, Nil)
DLIB_ARG(DeltaTable*, t)
ubinaryio::OutputBuffer* ob = DeltaTableSerialiser().Serialise(DLIB_ARGVAL(t));
DLIB_ERROR_CHECK(
		!ob, 
		uconstructstr(
			"in call ('%s') serialisation failed", 
			DLIB_CALLSIG_CROPPED
		)
	)
DeltaOutputBuffer_Make(DLIB_RETVAL_PTR, ob);
DLIB_FUNC_END

//------------------------------------------------------------------
// table tabdeserialise(ib).
// Deserialises a table from an input buffer.
//
DLIB_FUNC_START(tabdeserialise, 1, Nil)
DLIB_GET_INPUTBUFFER
if (DeltaTable* t = DeltaTableSerialiser().Deserialise(ib))
	{ DLIB_RETVAL_REF.FromTable(t); DeltaObject::NativeCodeHelpers::Unrefer(t); }
else
	DLIB_RESET_RETURN;
DLIB_FUNC_END

//------------------------------------------------------------------

