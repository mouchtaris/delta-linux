// AlgorithmsLib.cpp
// Implementation of algorithms for containers that uses the STD library API.
// ScriptFighter Project.
// API Construction: A. Savidis, February 2011.
// Implementation: A. Maragoudakis, February 2011.

// FIXME (AS->AM):
// 1) All error messages to be reworked to be meaningfull and consistent.
// 2) All (Primary)Error calls must be followed by a proper 'return'.
// 3) Make the code inlining macros crystal clear.
// 4) In all Get calls the return value should be checked (if false an error
// occured, thus shoukld either post a domino error or 'return' as necessary).
//

#include "DeltaLibraryCreators.h"
#include "AlgorithmLib.h"
#include "DeltaStdLib.h"
#include "OperatorOverloading.h"
#include "ListLib.h"
#include "TableIterLib.h"
#include <assert.h>

//------------------------------------------------------------------

static void find_LibFunc (DeltaVirtualMachine*);
static void find_all_LibFunc (DeltaVirtualMachine*);
static void find_if_LibFunc (DeltaVirtualMachine*);
static void find_all_if_LibFunc (DeltaVirtualMachine*);
static void apply_LibFunc (DeltaVirtualMachine*);
static void remove_LibFunc (DeltaVirtualMachine*);
static void remove_all_LibFunc (DeltaVirtualMachine*);
static void remove_if_LibFunc (DeltaVirtualMachine*);
static void remove_all_if_LibFunc (DeltaVirtualMachine*);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "find",					find_LibFunc					},
	{ "find_all",				find_all_LibFunc				},
	{ "find_if",				find_if_LibFunc					},
	{ "find_all_if",			find_all_if_LibFunc				},
	{ "apply",					apply_LibFunc					},
	{ "remove",					remove_LibFunc					},
	{ "remove_all",				remove_all_LibFunc				},
	{ "remove_if",				remove_if_LibFunc				},
	{ "remove_all_if",			remove_all_if_LibFunc			}
};

DELTA_LIBRARY_SUBAPI_INSTALLER(DeltaAlgorithm, "algorithm", DELTA_STDLIB_NAMESPACE_PREFIX)

///////////////////////////////////////////////

struct LibFunctionData {
	const char *CURR_FUNC;
	DeltaVirtualMachine *vm;
	util_ui32 totalActualArgs, argNo;
	LibFunctionData(const char *name, DeltaVirtualMachine *vm, util_ui32 args):
	CURR_FUNC(name), vm(vm), totalActualArgs(args), argNo(0) {}
};

///////////////////////////////////////////////

DeltaValue CallDeltaFunction(DeltaValue &function, DeltaValue &caller) {
	DeltaValue retval;
	if (function.Type() == DeltaValue_MethodFunc) {
		function(&retval);
	} else {
		function(caller, &retval);
	}
	return retval;
}

DeltaValue CallDeltaFunction(DeltaValue &function, DeltaValue &caller, DeltaValue &argument) {
	DeltaValue retval;
	if (function.Type() == DeltaValue_MethodFunc) {
		function(argument, &retval);
	} else {
		function(caller, argument, &retval);
	}
	return retval;
}

///////////////////////////////////////////////

bool ConvertValueToTable(
		DeltaValue&			val,
		DeltaTable**		table
	){
	if (val.Type() == DeltaValue_Table) { *table = val.ToTable(); }
	else if (val.Type() == DeltaValue_ExternId) { *table = val.GetExternIdUserData(); }
	else { return false; }
	return true;
}

bool GetTableOrExternIdFromArgument(
		LibFunctionData*	data,
		DeltaValue*			val,
		DeltaTable**		table
	){
	*val = *(DPTR(data->vm)->GetActualArg(data->argNo++));
	if (!ConvertValueToTable(*val, &(*table))) {
		DPTR(data->vm)->PrimaryError(
			"arg #%d of type 'Object / ExternId' expected (%s passed) to '%s'",
			data->argNo - 1, val->TypeStr(), data->CURR_FUNC
		);
		return false;
	}
	return true;
}

bool GetFunctionFromTable(
		LibFunctionData*	data,
		DeltaTable*			table,
		const char*			index,
		DeltaValue*			function
	){
	if (!(table->Get(DeltaValue(index), &(*function)) && function->IsCallable())) {
		DPTR(data->vm)->PrimaryError(
			"slot '%s' for %s 0x%x not a function / functor but %s at %s",
			index, DPTR(table)->GetExtClassString(), DPTR(table)->GetSerialNo(),
			function->TypeStr(), data->CURR_FUNC
		);
		return false;
	}
	return true;
}

bool GetCallableArgument(
		LibFunctionData*	data,
		DeltaValue*			func
	){
	*func = *(DPTR(data->vm)->GetActualArg(data->argNo++));
	if (!func->IsCallable()) {
		DPTR(data->vm)->PrimaryError("arg #%d expected callable (%s passed) to %s",
			data->argNo - 1, func->TypeStr(), data->CURR_FUNC);
			return false;
	}
	return true;
}

bool GetTableOrExternId(
		LibFunctionData*	data,
		DeltaValue&			val,
		DeltaValue*			it,
		DeltaTable**		table,
		const char*			funcName
	){
	DeltaValue function;
	DeltaTable *valTable = NULL;
	if (!(ConvertValueToTable(val, &valTable) &&
		  GetFunctionFromTable(data, valTable, funcName, &function)))
		return false;
	*it = CallDeltaFunction(function, val);
	if (!ConvertValueToTable(*it, &(*table))) {
		DPTR(data->vm)->PrimaryError(
			"method '%s' implicitly called on '%s' should return '%s', but returned '%s'",
			funcName, data->CURR_FUNC, "Object / ExternId", it->TypeStr()
		);
		return false;
	}
	return true;
}

bool GetTableOrExternIdFromContainer(
		LibFunctionData*	data,
		DeltaValue&			cont,
		DeltaValue*			it,
		DeltaTable**		table,
		const char*			methodName
	){
	DeltaValue iter_function, function;
	DeltaTable *contTable = NULL;
	
	if (ConvertValueToTable(cont, &contTable)					&&
		contTable->Get(DeltaValue("iterator"), &iter_function)	&&
		iter_function.IsNil()) {
		DASSERT(cont.Type() == DeltaValue_Table);
		DeltaTableIter_Make(&(*it));
		bool result = ConvertValueToTable(*it, &(*table));
		DASSERT(result);
		if (!GetFunctionFromTable(data, *table, methodName, &function))
			return false;
	} else
	if (!(GetTableOrExternId(data, cont, &(*it), &(*table), "iterator") &&
		  GetFunctionFromTable(data, *table, methodName, &function)))
		return false;
	CallDeltaFunction(function, *it, cont);
	return true;
}

bool GetTableOrExternIdFromIterator(LibFunctionData *data, DeltaValue& start, DeltaValue *it, DeltaTable **table)
{ return GetTableOrExternId(data, start, &(*it), &(*table), "copy"); }

bool GetIteratorArgument(
		LibFunctionData*	data,
		DeltaValue&			cont,
		DeltaValue*			it,
		const char*			methodName
	){
	DeltaTable *table = NULL;
	if (data->totalActualArgs > data->argNo) {
		return GetTableOrExternIdFromArgument(data, &(*it), &table);
	}
	return GetTableOrExternIdFromContainer(data, cont, &(*it), &table, methodName);
}

bool GetEraserArgument(
		LibFunctionData*	data,
		DeltaValue&			cont,
		DeltaValue*			eraser
	){
	if (data->totalActualArgs > data->argNo){
		return GetCallableArgument(data, &(*eraser));
	} else {
		DeltaTable *contTable = NULL;
		return	ConvertValueToTable(cont, &contTable) &&
				GetFunctionFromTable(data, contTable, "erase", &(*eraser));
	}
}

bool GetBeginAndEndIterator(
		LibFunctionData*	data,
		DeltaValue*			begin,
		DeltaValue*			end,
		DeltaTable**		beginTable
	){
	DeltaValue superclass, arg;
	DeltaTable *argTable = NULL;
	if (!(GetTableOrExternIdFromArgument(data, &arg, &argTable) &&
		  argTable->Get(DELTA_STD_ALGORITHMS_SUPERCLASS, &superclass)))
		return false;
	if (arg.IsTable() && superclass.IsNil())
		superclass.FromString(DELTA_STD_ALGORITHMS_CONTAINER);
	if (superclass.IsNil()) {
		DPTR(data->vm)->PrimaryError(
			"no 'superclass' slot in %s arg #%u at %s",
			arg.GetExternIdTypeString().c_str(), data->argNo - 1, data->CURR_FUNC
		);
		return false;
	}
	else if (superclass.Type() != DeltaValue_String) {
		DPTR(data->vm)->PrimaryError(
			"no String 'superclass' slot (was %s) in arg #%u at %s",
			superclass.TypeStr(), data->argNo - 1, data->CURR_FUNC
		);
		return false;
	}
	else if (superclass.ToString() != DELTA_STD_ALGORITHMS_ITERATOR &&
			superclass.ToString() != DELTA_STD_ALGORITHMS_CONTAINER) {
		DPTR(data->vm)->PrimaryError(
			"unknown '%s' value ('iterator' / 'container' expected) of 'superclass' slot in arg #%u at %s",
			superclass.ToString().c_str(), data->argNo - 1, data->CURR_FUNC
		);
		return false;
	}
	if (superclass.ToString() == DELTA_STD_ALGORITHMS_ITERATOR) {
		DeltaTable *endTable = NULL;
		if (!GetTableOrExternIdFromIterator(data, arg, &(*begin), &(*beginTable)))
			return false;
		if (data->argNo >= data->totalActualArgs) {
			DPTR(data->vm)->PrimaryError(
				"arg #%d expected end iterator, because start iterator passed in arg #%d at %s",
				data->argNo, data->argNo - 1, data->CURR_FUNC
			);
			return false;
		}
		return GetTableOrExternIdFromArgument(data, &(*end), &endTable);
	} else if (superclass.ToString() == DELTA_STD_ALGORITHMS_CONTAINER) {
		return	GetTableOrExternIdFromContainer(data, arg, &(*begin), &(*beginTable), "setbegin") &&
				GetIteratorArgument(data, arg, &(*end), "setend");
	}
	return false;
}

bool GetContainerAndIterators(
		LibFunctionData*	data,
		DeltaValue*			container,
		DeltaValue*			begin,
		DeltaValue*			end,
		DeltaTable**		beginTable
	){
	DeltaValue superclass, getcontainer;
	DeltaValue arg = *(DPTR(data->vm)->GetActualArg(data->argNo));
	DeltaTable *argTable = NULL;
	if (!(GetBeginAndEndIterator(data, &(*begin), &(*end), &(*beginTable))	&&
		  ConvertValueToTable(arg, &argTable)								&&
		  argTable->Get(DELTA_STD_ALGORITHMS_SUPERCLASS, &superclass)))
		return false;

	if (arg.IsTable() && superclass.IsNil())
		superclass.FromString(DELTA_STD_ALGORITHMS_CONTAINER);
	if (superclass.ToString() == DELTA_STD_ALGORITHMS_ITERATOR) {
		if (!GetFunctionFromTable(data, *beginTable, "getcontainer", &getcontainer))
			return false;
		*container = CallDeltaFunction(getcontainer, *begin);
	} else if (superclass.ToString() == DELTA_STD_ALGORITHMS_CONTAINER) {
		*container = arg;
	}
	return true;
}

DeltaValue GetNextArgument(LibFunctionData* data)
{ return DPTR(data->vm)->GetActualArg(data->argNo++); }

///////////////////////////////////////////////

#define DLIB_FUNC_ARGRANGE_START(name, min, max, reset)					\
	static void name##_LibFunc (DeltaVirtualMachine* vm) {				\
		ISSUE_FUNC(#name);												\
		util_ui32 POSSIBLY_UNUSED_LOCAL(_argNo) = 0;					\
		Delta##reset##Reset reseter(vm);								\
		util_ui16 totalActualArgs = DPTR(vm)->TotalActualArgs();		\
		if ((totalActualArgs < (min)) || (totalActualArgs > (max))) {	\
			DPTR(vm)->PrimaryError(										\
				"expected %d to %d args (%d passed) to '%s'",			\
				(min),													\
				(max),													\
				totalActualArgs,										\
				CURR_FUNC												\
			);															\
			DLIB_RESET_RETURN;											\
		}																\
		std::string _sig1, _sig2;										\
		LibFunctionData libFuncData(CURR_FUNC, vm, totalActualArgs);

///////////////////////////////////////////////

bool HandleEqualMethod(DeltaValue *it1, DeltaValue *it2, bool* retval) {
	DeltaValue equal;
	DeltaTable *itTable = NULL;
	ConvertValueToTable(*it1, &itTable);
	if (itTable && itTable->Get(DeltaValue("equal"), &equal) && equal.IsCallable()) {
		DeltaValue isEqual = CallDeltaFunction(equal, *it1, *it2);
		if (isEqual.Type() == DeltaValue_Bool) {
			*retval = isEqual.ToBool();
			return true;
		}
	}
	return false;
}

bool HandleOperatorEqual(DeltaValue *it, DeltaValue *endIt, DeltaVirtualMachine *vm, const char* functionName, bool *isEqual) {
	bool retval = true;
	if (HandleEqualMethod(it, endIt, &retval)) { *isEqual = retval; return true; }
	if (HandleEqualMethod(endIt, it, &retval)) { *isEqual = retval; return true; }
	
	OperatorOverloading::RelationalResult result = OperatorOverloading::HandleRelational("==", false, it, endIt, vm);
	switch (result) {
		case OperatorOverloading::Relational_True : *isEqual = true; return true;
		case OperatorOverloading::Relational_False : *isEqual = false; return true;
		case OperatorOverloading::Relational_NotOverloaded :
		case OperatorOverloading::Relational_OverloadedButRequestedRetry :
			result = OperatorOverloading::HandleRelational("==", true, endIt, it, vm);
			switch (result) {
				case OperatorOverloading::Relational_True : *isEqual = true; return true;
				case OperatorOverloading::Relational_False : *isEqual = false; return true;
				case OperatorOverloading::Relational_NotOverloaded :
				case OperatorOverloading::Relational_OverloadedButRequestedRetry :
					if (OperatorOverloading::HasGlobalRelational(DeltaRelOp_EQ)) {
						result = OperatorOverloading::HandleGlobalRelational(DeltaRelOp_EQ, it, endIt, vm);
						switch (result) {
							case OperatorOverloading::Relational_True : *isEqual = true; return true;
							case OperatorOverloading::Relational_False : *isEqual = false; return true;
							case OperatorOverloading::Relational_NotOverloaded :
							case OperatorOverloading::Relational_OverloadedButRequestedRetry :
								DPTR(vm)->PrimaryError("no 'equal' slot or overloaded '==' operator on passed iterator at '%s'", functionName);
								break;
							case OperatorOverloading::Relational_ExecutionError :
								DPTR(vm)->Error("Execution error during overloaded operator '==' at '%s'", functionName);
								break;
							case OperatorOverloading::Relational_Exception : return false;
						}
					} else
						DPTR(vm)->PrimaryError("no 'equal' slot or overloaded '==' operator on passed iterator at '%s'", functionName);
					break;
				case OperatorOverloading::Relational_ExecutionError :
					DPTR(vm)->Error("Execution error during overloaded operator '==' at '%s'", functionName);
					break;
				case OperatorOverloading::Relational_Exception : return false;
			}
			break;
		case OperatorOverloading::Relational_ExecutionError :
			DPTR(vm)->Error("Execution error during overloaded operator '==' at '%s'", functionName);
			break;
		case OperatorOverloading::Relational_Exception : return false;
	}
	return false;
}

//------------------------------------------------------------------
//find (element, Container[, end iterator])
//find (element, start iterator, end iterator)
DLIB_FUNC_ARGRANGE_START(find, 2, 3, Nil)
	DeltaValue beginIt, endIt, fwd, getval;
	DeltaTable *itTable = NULL;
	DeltaValue element = GetNextArgument(&libFuncData);
	if (!(GetBeginAndEndIterator(&libFuncData, &beginIt, &endIt, &itTable)	&&
		  GetFunctionFromTable(&libFuncData, itTable, "fwd", &fwd)			&&
		  GetFunctionFromTable(&libFuncData, itTable, "getval", &getval)))
		DLIB_RESET_RETURN;
	else {}

	bool reachedEnd, equal_result;
	while ((equal_result = HandleOperatorEqual(&beginIt, &endIt, vm, CURR_FUNC, &reachedEnd)) && !reachedEnd) {
		DeltaValue getval_retval = CallDeltaFunction(getval, beginIt);
		if (getval_retval.Equal(element)) { break; }
		CallDeltaFunction(fwd, beginIt);
	}
	if (equal_result) DLIB_RETVAL_REF.Assign(beginIt);
}

//------------------------------------------------------------------
//find_all (element, Container[, end iterator])
//find_all (element, start iterator, end iterator)
DLIB_FUNC_ARGRANGE_START(find_all, 2, 3, Nil)
	DeltaValue listValue, beginIt, endIt, fwd, getval, copy;
	DeltaTable *itTable = NULL;
	DeltaList_Make(listValue);
	std::list<DeltaValue>* iteratorList = DeltaList_Get(listValue);

	DeltaValue element = GetNextArgument(&libFuncData);
	if (!(GetBeginAndEndIterator(&libFuncData, &beginIt, &endIt, &itTable)	&&
		  GetFunctionFromTable(&libFuncData, itTable, "fwd", &fwd)			&&
		  GetFunctionFromTable(&libFuncData, itTable, "getval", &getval)	&&
		  GetFunctionFromTable(&libFuncData, itTable, "copy", &copy)))
		DLIB_RESET_RETURN;
	else {}

	bool reachedEnd, equal_result;
	while ((equal_result = HandleOperatorEqual(&beginIt, &endIt, vm, CURR_FUNC, &reachedEnd)) && !reachedEnd) {
		DeltaValue getval_retval = CallDeltaFunction(getval, beginIt);
		if (getval_retval.Equal(element))
			iteratorList->push_back(CallDeltaFunction(copy, beginIt));
		CallDeltaFunction(fwd, beginIt);
	}
	if (equal_result) DLIB_RETVAL_REF.Assign(listValue);
}

//------------------------------------------------------------------
//find_if (predicate, Container[, end iterator])
//find_if (predicate, start iterator, end iterator)
DLIB_FUNC_ARGRANGE_START(find_if, 2, 3, Nil)
	DeltaValue predicate, beginIt, endIt, fwd, copy;
	DeltaTable *itTable = NULL;
	if (!(GetCallableArgument(&libFuncData, &predicate)						&&
		  GetBeginAndEndIterator(&libFuncData, &beginIt, &endIt, &itTable)	&&
		  GetFunctionFromTable(&libFuncData, itTable, "fwd", &fwd)			&&
		  GetFunctionFromTable(&libFuncData, itTable, "copy", &copy)))
		DLIB_RESET_RETURN;
	else {}

	bool reachedEnd, equal_result;
	while ((equal_result = HandleOperatorEqual(&beginIt, &endIt, vm, CURR_FUNC, &reachedEnd)) && !reachedEnd) {
		DeltaValue predicateValue;
		if (predicate(CallDeltaFunction(copy, beginIt), &predicateValue) &&
			predicateValue.Type() == DeltaValue_Bool &&
			predicateValue.ToBool()) { break; }
		CallDeltaFunction(fwd, beginIt);
	}
	if (equal_result) DLIB_RETVAL_REF.Assign(beginIt);
}

//------------------------------------------------------------------
//find_all_if (predicate, Container[, end iterator])
//find_all_if (predicate, start iterator, end iterator)
DLIB_FUNC_ARGRANGE_START(find_all_if, 2, 3, Nil)
	DeltaValue listValue, predicate, beginIt, endIt, fwd, copy;
	DeltaTable *itTable;
	DeltaList_Make(listValue);
	std::list<DeltaValue>* iteratorList = DeltaList_Get(listValue);

	if (!(GetCallableArgument(&libFuncData, &predicate)						&&
		  GetBeginAndEndIterator(&libFuncData, &beginIt, &endIt, &itTable)	&&
		  GetFunctionFromTable(&libFuncData, itTable, "fwd", &fwd)			&&
		  GetFunctionFromTable(&libFuncData, itTable, "copy", &copy)))
		DLIB_RESET_RETURN;
	else {}

	bool reachedEnd, equal_result;
	while ((equal_result = HandleOperatorEqual(&beginIt, &endIt, vm, CURR_FUNC, &reachedEnd)) && !reachedEnd) {
		DeltaValue predicateValue;
		if (predicate(CallDeltaFunction(copy, beginIt), &predicateValue) &&
			predicateValue.Type() == DeltaValue_Bool &&
			predicateValue.ToBool())
			iteratorList->push_back(CallDeltaFunction(copy, beginIt));
		CallDeltaFunction(fwd, beginIt);
	}
	if (equal_result) DLIB_RETVAL_REF.Assign(listValue);
}

//------------------------------------------------------------------
//apply (function, Container[, end iterator])
//apply (function, start iterator, end iterator)
DLIB_FUNC_ARGRANGE_START(apply, 2, 3, Nil)
	DeltaValue function, beginIt, endIt, fwd, copy;
	DeltaTable *itTable = NULL;
	if (!(GetCallableArgument(&libFuncData, &function)						&&
		  GetBeginAndEndIterator(&libFuncData, &beginIt, &endIt, &itTable)	&&
		  GetFunctionFromTable(&libFuncData, itTable, "fwd", &fwd)			&&
		  GetFunctionFromTable(&libFuncData, itTable, "copy", &copy)))
		DLIB_RESET_RETURN;
	else {}

	bool reachedEnd, equal_result;
	while ((equal_result = HandleOperatorEqual(&beginIt, &endIt, vm, CURR_FUNC, &reachedEnd)) && !reachedEnd) {
		function(CallDeltaFunction(copy, beginIt));
		CallDeltaFunction(fwd, beginIt);
	}
}

//------------------------------------------------------------------
//remove (element, Container[, end iterator, eraser function])
//remove (element, start iterator, end iterator[, eraser function])
//
DLIB_FUNC_ARGRANGE_START(remove, 2, 4, Nil)
	DeltaValue container, beginIt, endIt, erase, fwd, getval, copy;
	DeltaTable *itTable = NULL;
	DeltaValue element = GetNextArgument(&libFuncData);
	if (!(GetContainerAndIterators(&libFuncData, &container, &beginIt, &endIt, &itTable)	&&
		  GetEraserArgument(&libFuncData, container, &erase)								&&
		  GetFunctionFromTable(&libFuncData, itTable, "fwd", &fwd)							&&
		  GetFunctionFromTable(&libFuncData, itTable, "getval", &getval)					&&
		  GetFunctionFromTable(&libFuncData, itTable, "copy", &copy)))
		DLIB_RESET_RETURN;
	else {}

	bool reachedEnd, equal_result;
	while ((equal_result = HandleOperatorEqual(&beginIt, &endIt, vm, CURR_FUNC, &reachedEnd)) && !reachedEnd) {
		DeltaValue getvalValue = CallDeltaFunction(getval, beginIt);
		if (getvalValue.Equal(element)) {
			DeltaValue removeIteratorValue = CallDeltaFunction(copy, beginIt);
			CallDeltaFunction(fwd, beginIt);
			CallDeltaFunction(erase, container, removeIteratorValue);
			break;
		}
		CallDeltaFunction(fwd, beginIt);
	}
	if (equal_result) DLIB_RETVAL_REF.Assign(beginIt);
}

//------------------------------------------------------------------
//remove_all (element, Container[, end iterator, eraser function])
//remove_all (element, start iterator, end iterator[, eraser function])
//
DLIB_FUNC_ARGRANGE_START(remove_all, 2, 4, Nil)
	DeltaValue container, beginIt, endIt, erase, fwd, getval, copy;
	DeltaTable *itTable = NULL;
	DeltaValue element = GetNextArgument(&libFuncData);
	if (!(GetContainerAndIterators(&libFuncData, &container, &beginIt, &endIt, &itTable)	&&
		  GetEraserArgument(&libFuncData, container, &erase)								&&
		  GetFunctionFromTable(&libFuncData, itTable, "fwd", &fwd)							&&
		  GetFunctionFromTable(&libFuncData, itTable, "getval", &getval)					&&
		  GetFunctionFromTable(&libFuncData, itTable, "copy", &copy)))
		DLIB_RESET_RETURN;
	else {}

	bool reachedEnd, equal_result;
	while ((equal_result = HandleOperatorEqual(&beginIt, &endIt, vm, CURR_FUNC, &reachedEnd)) && !reachedEnd) {
		DeltaValue removeIteratorValue;
		DeltaValue getvalValue = CallDeltaFunction(getval, beginIt);
		if (getvalValue.Equal(element))
			removeIteratorValue = CallDeltaFunction(copy, beginIt);
		CallDeltaFunction(fwd, beginIt);
		if (getvalValue.Equal(element))
			CallDeltaFunction(erase, container, removeIteratorValue);
	}
}

//------------------------------------------------------------------
//remove_if (predicate, Container[, end iterator, eraser function])
//remove_if (predicate, start iterator, end iterator[, eraser function])
//
DLIB_FUNC_ARGRANGE_START(remove_if, 2, 4, Nil)
	DeltaValue predicate, container, beginIt, endIt, erase, fwd, copy;
	DeltaTable *itTable = NULL;
	if (!(GetCallableArgument(&libFuncData, &predicate)										&&
		  GetContainerAndIterators(&libFuncData, &container, &beginIt, &endIt, &itTable)	&&
		  GetEraserArgument(&libFuncData, container, &erase)								&&
		  GetFunctionFromTable(&libFuncData, itTable, "fwd", &fwd)							&&
		  GetFunctionFromTable(&libFuncData, itTable, "copy", &copy)))
		DLIB_RESET_RETURN;
	else {}

	bool reachedEnd, equal_result;
	while ((equal_result = HandleOperatorEqual(&beginIt, &endIt, vm, CURR_FUNC, &reachedEnd)) && !reachedEnd) {
		DeltaValue predicateValue;
		if (predicate(CallDeltaFunction(copy, beginIt), &predicateValue) &&
			predicateValue.Type() == DeltaValue_Bool &&
			predicateValue.ToBool()) {
				DeltaValue removeIteratorValue = CallDeltaFunction(copy, beginIt);
				CallDeltaFunction(fwd, beginIt);
				CallDeltaFunction(erase, container, removeIteratorValue);
				break;
		}
		CallDeltaFunction(fwd, beginIt);
	}
	if (equal_result) DLIB_RETVAL_REF.Assign(beginIt);
}

//------------------------------------------------------------------
//remove_all_if (predicate, Container[, end iterator, eraser function])
//remove_all_if (predicate, start iterator, end iterator[, eraser function])
//
DLIB_FUNC_ARGRANGE_START(remove_all_if, 2, 4, Nil)
	DeltaValue predicate, container, beginIt, endIt, erase, fwd, copy;
	DeltaTable *itTable = NULL;
	if (!(GetCallableArgument(&libFuncData, &predicate)										&&
		  GetContainerAndIterators(&libFuncData, &container, &beginIt, &endIt, &itTable)	&&
		  GetEraserArgument(&libFuncData, container, &erase)								&&
		  GetFunctionFromTable(&libFuncData, itTable, "fwd", &fwd)							&&
		  GetFunctionFromTable(&libFuncData, itTable, "copy", &copy)))
		DLIB_RESET_RETURN;
	else {}

	bool reachedEnd, equal_result;
	while ((equal_result = HandleOperatorEqual(&beginIt, &endIt, vm, CURR_FUNC, &reachedEnd)) && !reachedEnd) {
		DeltaValue predicateValue, removeIteratorValue;
		if (predicate(CallDeltaFunction(copy, beginIt), &predicateValue) &&
			predicateValue.Type() == DeltaValue_Bool && predicateValue.ToBool())
			removeIteratorValue = CallDeltaFunction(copy, beginIt);
		CallDeltaFunction(fwd, beginIt);
		if (predicateValue.Type() == DeltaValue_Bool && predicateValue.ToBool())
			CallDeltaFunction(erase, container, removeIteratorValue);
	}
}

//------------------------------------------------------------------
