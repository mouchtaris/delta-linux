// DebugExprParserAPI.cpp
// The API implementation.
// ScriptFighter Project.
// A. Savidis, February 2002.
// Refactored June 2008, disabling any calls to overloaded operators.
//

#include "DebugExprParserAPI.h"
#include "DebugExprLexAn.h"
#include "DebugExprParseActions.h"
#include "DeltaTable.h"
#include "DeltaExternId.h"
#include "DebugEvaluatedExpr.h"
#include "DeltaObject.h"
#include "DeltaValueToDebugWatchValueInfo.h"
#include "DebugWatchValueTextEncoders.h"
#include "DebugWatchValueRcEncoder.h"
#include "DebugWatchValueXmlEncoder.h"
#include "DebugWatchValueJsonEncoder.h"
#include "DebugAttributeInformation.h"
#include "DebugClosureInformation.h"
#include "ulexutil.h"

USTRINGDESTRUCTOR_DECLARE(UCLASSQUALIFIER_EMPTY, DebugExprEvalStringDestructor)
USTRINGDESTRUCTOR_IMPL(DebugExprEvalStringDestructor)

char* DebugEvaluatedExpr::StringLateDestroy (char* s) {
	DebugExprEvalStringDestructor::GetSingleton().add(DNEWCLASS(udestroyablestring, (s))); 
	return s;
}

bool DebugExprEvaluator::expandLibraryFuncs	= false;
bool DebugExprEvaluator::expandMethodFuncs	= false;
bool DebugExprEvaluator::expandProgramFuncs = false;
bool DebugExprEvaluator::expandFuncVMs		= false;

////////////////////////////////////////////////////

extern int DebugExprParser_yyparse (void);

void DebugExprEvaluator::SingletonCreate (void) {
	DebugExprParseActions::SingletonCreate(); 
	DebugEvaluatedExpr::SingletonCreate(); 
	DebugExprEvalStringDestructor::SingletonCreate();
	DeltaValueToDebugWatchValueInfo::SingletonCreate();
	DebugAttributeInformation::SingletonCreate();
	DebugClosureInformation::SingletonCreate();

	DebugWatchValueTextEncoders::SingletonCreate();
	DebugWatchValueTextEncoders::Install(
		DebugWatchValueRcEncoder::GetFormatId(),
		&DebugWatchValueRcEncoder::Encode
	);
	DebugWatchValueTextEncoders::Install(
		DebugWatchValueXmlEncoder::GetFormatId(),
		&DebugWatchValueXmlEncoder::Encode
	);
	DebugWatchValueTextEncoders::Install(
		DebugWatchValueJsonEncoder::GetFormatId(),
		&DebugWatchValueJsonEncoder::Encode
	);
}

void DebugExprEvaluator::SingletonDestroy (void) {
	DebugExprParseActions::SingletonDestroy();
	DebugEvaluatedExpr::SingletonDestroy();
	DebugExprEvalStringDestructor::SingletonDestroy();
	DeltaValueToDebugWatchValueInfo::SingletonDestroy();
	DebugWatchValueTextEncoders::SingletonDestroy();
	DebugAttributeInformation::SingletonDestroy();
	DebugClosureInformation::SingletonDestroy();
}

////////////////////////////////////////////////////

static void ParseExpression (const char* expr, DeltaVirtualMachine* vm) {

	DebugExprLexAn::Initialise(expr);

	DebugExprParseActions::Initialise(vm);
	DebugExprParser_yyparse();

	DebugExprLexAn::CleanUp();
	DebugExprEvalStringDestructor::GetSingleton().commit();
}

////////////////////////////////////////////////////

void DebugExprEvaluator::CleanUpEvaluation (void) {
	DebugExprParseActions::CleanUp();	
	DebugEvaluatedExpr::CleanUp();
	DebugAttributeInformation::CleanUp();
}

////////////////////////////////////////////////////

const std::string DebugExprEvaluator::ToString (const DeltaValue* obj) 
	{ return DeltaValueToDebugWatchValueInfo::ToString(obj); }

const std::pair<const std::string, bool> DebugExprEvaluator::Evaluate (
		DeltaVirtualMachine*	vm,
		const char*				expr
	) {

	ParseExpression(expr, vm);

	std::string value;
	bool		noError = true;

	if (DebugExprParseActions::HasError())
		{ value = DebugExprParseActions::GetLastError(); noError = false; }
	else
		value = ToString(DPTR(DebugExprParseActions::GetResultExpression())->GetResult());

	CleanUpEvaluation();
	return std::pair<const std::string, bool>(value, noError);
}

////////////////////////////////////////////////////

const std::pair<const std::string, bool> DebugExprEvaluator::EvaluateToTypeData (
		DeltaVirtualMachine*	vm,
		const char*				formatId,
		const char*				expr
	) {

	DebugWatchValueTextEncoders::Encoder encoder = DebugWatchValueTextEncoders::Get(formatId);
	if (!encoder)
		return	std::pair<const std::string, bool>(
					uconstructstr("unsupported format '%s'", formatId),
					false
				);

	ParseExpression(expr, vm);

	std::string value;
	bool		noError = true;

	if (DebugExprParseActions::HasError())
		{ value = DebugExprParseActions::GetLastError(); noError = false; }
	else {
		DebugWatchValueInfo info;
		DeltaValueToDebugWatchValueInfo::Convert(
			DPTR(DebugExprParseActions::GetResultExpression())->GetResult(), 
			&info
		);
		value = (*encoder)(info);
	}
	
	CleanUpEvaluation();
	return std::pair<const std::string, bool>(value, noError);
}

////////////////////////////////////////////////////

#define	CHECK_DEPTH_EXCEEDED_OR_ALREADY_EXPANDED(type)										\
	if (!depth.More())																		\
		return;																				\
	std::string absRef = DeltaValueToDebugWatchValueInfo::type##_AbsoluteRef(*val);			\
	if (graph.find(absRef) != graph.end())													\
		return;																				\
	
#define	MAKE_NEW_NODE()																		\
	ObjectNode& node = graph[absRef];  /* Inserts in graph too */							\
	if (values)																				\
		(*values)[absRef].Assign(val);

#define	ADD_START_NODE()																	\
	if (true) {																				\
		DASSERT(graph.find(DBG_GRAPH_START_NODE) == graph.end());							\
		graph[DBG_GRAPH_START_NODE].push_back(												\
			ObjectSlot(																		\
				DBG_GRAPH_START_NODE_ABSREF,												\
				false,																		\
				DeltaValueToDebugWatchValueInfo::AbsoluteRef(*val),							\
				false,																		\
				DEBUG_KEYTYPE_NONE															\
			)																				\
		);																					\
	} else

#define	EXPAND_SLOT(key, keyType)															\
	if (true) {																				\
	std::pair<std::string, bool> result = ExpandValueSlot(graph, values, &field, depth);	\
	node.push_back(																			\
		ObjectSlot(																			\
			key,																			\
			false,																			\
			result.first,																	\
			result.second,																	\
			keyType																			\
		)																					\
	); } else

#define	EXPAND_OBJECT_TRUE(obj)	true

#define	EXPAND_OBJECT_PSEUDO_CONTAINER(obj, objtype, get, type, index, access, keyType, pre)\
	if (true) {																				\
		const type<objtype*>& cont(obj->get());												\
		util_ui32 n = 0;																	\
		for (type<objtype*>::const_iterator i = cont.begin(); i != cont.end(); ++i, ++n) {	\
			if (pre(*i)) {																	\
				std::string key(uconstructstr("%s[%u]", index, n));							\
				DeltaValue field(access);													\
				EXPAND_SLOT(key, keyType);													\
			}																				\
		}																					\
	} else

//*****************************

void DebugExprEvaluator::SetObjectGraphConfiguration(
		bool _expandLibraryFuncs,
		bool _expandMethodFuncs,
		bool _expandProgramFuncs,
		bool _expandFuncVMs
	) {
	expandLibraryFuncs	= _expandLibraryFuncs;
	expandMethodFuncs	= _expandMethodFuncs;
	expandProgramFuncs	= _expandProgramFuncs;
	expandFuncVMs		= _expandFuncVMs;
}

//*****************************

const std::pair<std::string, bool> DebugExprEvaluator::ExpandValueSlot (
		ObjectGraph&		graph,
		ObjectValues*		values, 
		const DeltaValue*	obj,
		const DepthCounter&	depth
	) {

	if (obj->Type() == DeltaValue_Table) {
		ExpandTable(graph, values, obj, depth.Decrease());
		return	std::pair<std::string, bool>(
					DeltaValueToDebugWatchValueInfo::Table_AbsoluteRef(*obj),
					true
				);
	}
	else
	if (obj->Type() == DeltaValue_ExternId) {
		ExpandExternId(graph, values, obj, depth.Decrease());
		return	std::pair<std::string, bool>(
					DeltaValueToDebugWatchValueInfo::ExternId_AbsoluteRef(*obj),
					true
				);
	}
	else
	if (obj->Type() == DeltaValue_ProgramFunc && expandProgramFuncs && IsUsefulToExpandProgramFunc(graph, *obj)) {
		ExpandProgramFunc(graph, values, obj, depth.Decrease());
		return	std::pair<std::string, bool>(
					DeltaValueToDebugWatchValueInfo::ProgramFunc_AbsoluteRef(*obj),
					true
				);
	}
	else
	if (obj->Type() == DeltaValue_MethodFunc && expandMethodFuncs && IsUsefulToExpandMethodFunc(graph, *obj)) {
		ExpandMethodFunc(graph, values, obj, depth.Decrease());
		return	std::pair<std::string, bool>(
					DeltaValueToDebugWatchValueInfo::MethodFunc_AbsoluteRef(*obj),
					true
				);
	}
	else
	if (obj->Type() == DeltaValue_LibraryFunc  && expandLibraryFuncs && obj->GetLibraryFuncArgsBinder()) {
		ExpandLibraryFunc(graph, values, obj, depth.Decrease());
		return	std::pair<std::string, bool>(
					DeltaValueToDebugWatchValueInfo::LibraryFunc_AbsoluteRef(*obj),
					true
				);
	}
	else 
		return	std::pair<std::string, bool>(
				ToString(obj),
				false
			);
}

//*****************************

void DebugExprEvaluator::ExternIdExcludeKeys (std::list<std::string>* keys, const std::string& typeString) {
	if (typeString == DELTA_VM_TYPE_STR) {
		std::list<std::string>::iterator i = std::find(keys->begin(), keys->end(), "statics");
		DASSERT(i != keys->end());
		keys->erase(i);
	}
}

void DebugExprEvaluator::ExpandExternId (ObjectGraph& graph, ObjectValues* values, const DeltaValue* val, const DepthCounter& depth) {

	CHECK_DEPTH_EXCEEDED_OR_ALREADY_EXPANDED(ExternId);
	MAKE_NEW_NODE();

	// If the extern id is invalid we disable expansion.
	const DeltaExternIdFieldGetter*	fieldGetter = val->GetExternIdFieldGetter();
	void* nativeVal = const_cast<DeltaValue*>(val)->ToExternId();
	if (fieldGetter && !DPTR(fieldGetter)->IsValidValue(nativeVal)) // Was destroyed.
		unullify(fieldGetter);

	// Add all reserved fields.
	std::list<std::string> keys;
	DPTR(val->GetExternId())->AddReservedFieldKeys(&keys);

	DeltaValue field;
	for (std::list<std::string>::iterator i = keys.begin(); i != keys.end(); ++i) {
		DeltaExternId::GetReservedField(*i, *const_cast<DeltaValue*>(val), &field);
		std::string displayedKey;
		if (*i == DELTA_EXTERNID_TYPE_INDEX)
			displayedKey = DEBUG_EXTERNID_TYPE_VISIBLE_KEY;
		else
		if (*i == DELTA_EXTERNID_USERDATA_INDEX)
			displayedKey = DEBUG_EXTERNID_USERDATA_VISIBLE_KEY;
		else
		if (*i == DELTA_EXTERNID_MADEIN_INDEX)
			displayedKey = DEBUG_EXTERNID_MADEIN_VISIBLE_KEY;
		else
			DASSERT(false);	// Added more reserved keys, need to update this function.
		EXPAND_SLOT(displayedKey, DEBUG_KEYTYPE_EXTERNID_PSEUDO_KEY);
	}
	keys.clear();

	// Add the fields by the field getter.
	if (fieldGetter) {
		
		// String keys.
		DPTR(fieldGetter)->AddStringKeys(keys, nativeVal);
		if (DPTR(fieldGetter)->HasKeyToGetKeys())
			keys.remove(DPTR(fieldGetter)->GetKeyToGetKeys());

		ExternIdExcludeKeys(&keys, val->GetExternIdTypeString());

		for (std::list<std::string>::iterator i = keys.begin(); i != keys.end(); ++i) {
			fieldGetter->Get(nativeVal, *i, &field);
			EXPAND_SLOT(*i, DEBUG_KEYTYPE_EXTERNID_USER_PSEUDO_KEY);
		}

		// Numeric keys.
		for (util_ui32 i = 0, n = DPTR(fieldGetter)->GetNumericKeys(nativeVal); i < n; ++i) {
			fieldGetter->Get(nativeVal, i, &field);
			EXPAND_SLOT(uconstructstr("[%d]", i), DEBUG_KEYTYPE_EXTERNID_USER_PSEUDO_KEY);
		}
	}
}

//*****************************
// For attributes we generate a sequence of slots with special attribute keys.

void DebugExprEvaluator::ExpandAttribute (ObjectGraph& graph, ObjectNode& node, ObjectValues* values, const DeltaTable::Attribute* attr, const DepthCounter& depth) {

	DeltaValue index(DPTR(attr)->GetIndex());
	DASSERT(index.Type() == DeltaValue_String);

	std::string primaryKey = index.ToString();
	DASSERT(DeltaString::IsIdentifier(primaryKey));
	primaryKey = std::string(DEBUG_ATTR_SUFFIX_START) + primaryKey + ":";

	DeltaValue content;

	content.Assign(DPTR(attr)->GetGetter());
	std::pair<std::string, bool> contentResult = ExpandValueSlot(graph, values, &content, depth);	

	node.push_back(
		ObjectSlot(
			primaryKey + "get",
			false,
			contentResult.first,
			contentResult.second,
			DEBUG_KEYTYPE_OBJECT_ATTRIBUTE_KEY
		)
	);

	content.Assign(DPTR(attr)->GetSetter());
	contentResult = ExpandValueSlot(graph, values, &content, depth);	
	node.push_back(
		ObjectSlot(
			primaryKey + "set",
			false,
			contentResult.first,
			contentResult.second,
			DEBUG_KEYTYPE_OBJECT_ATTRIBUTE_KEY
		)
	);

	DPTR(attr)->GetContentBySkippingUserCode(&content);
	contentResult = ExpandValueSlot(graph, values, &content, depth);	
	node.push_back(
		ObjectSlot(
			primaryKey + "value",
			false,
			contentResult.first,
			contentResult.second,
			DEBUG_KEYTYPE_OBJECT_ATTRIBUTE_KEY
		)
	);
}

//*****************************

void DebugExprEvaluator::ExpandTable (ObjectGraph& graph, ObjectValues* values, const DeltaValue* val, const DepthCounter& depth) {

	CHECK_DEPTH_EXCEEDED_OR_ALREADY_EXPANDED(Table);
	MAKE_NEW_NODE();

	const DeltaTable* table = const_cast<DeltaValue*>(val)->ToTable();

	if (GetDeltaObjectValidator()->Validate((DeltaObject*) table)) {

		// Inheritance fields.

		DeltaObject* proto = (DeltaObject*) table;

		if (DPTR(proto)->GetBases().size()) {
			EXPAND_OBJECT_PSEUDO_CONTAINER(
				proto, 
				DeltaObject, 
				GetBases, 
				std::list, 
				DEBUG_TABLE_BASES_VISIBLE_KEY, 
				*i,
				DEBUG_KEYTYPE_OBJECT_INHERITANCE_KEY,
				EXPAND_OBJECT_TRUE
			);
		}

		if (DPTR(proto)->GetDerived()) {

			DeltaValue field(DPTR(proto)->GetDerived());
			EXPAND_SLOT(DEBUG_TABLE_DERIVED_VISIBLE_KEY, DEBUG_KEYTYPE_OBJECT_INHERITANCE_KEY);

			if (DPTR(proto)->GetMostDerived() != proto) {
				field.FromTable(DPTR(proto)->GetMostDerived());
				EXPAND_SLOT(DEBUG_TABLE_MOSTDERIVED_VISIBLE_KEY, DEBUG_KEYTYPE_OBJECT_INHERITANCE_KEY);
			}
		}

		// Delegates and delegators fields.

#define	DELEGATE_PRECOND(p)		\
			(expandLibraryFuncs || !DPTR(table)->IsInternalDelegate(p))

		if (DPTR(table)->HasDelegates()) {
			EXPAND_OBJECT_PSEUDO_CONTAINER(
				table, 
				DeltaTable, 
				GetDelegates, 
				std::list, 
				DEBUG_TABLE_DELEGATES_VISIBLE_KEY, 
				*i,
				DEBUG_KEYTYPE_OBJECT_DELEGATION_KEY,
				DELEGATE_PRECOND
			);
		}
#undef	DELEGATE_PRECOND

#define	DELEGATOR_PRECOND(p)	\
			(expandLibraryFuncs || !DPTR((p).first)->IsInternalDelegate(table))

		if (DPTR(table)->HasDelegators()) {
			EXPAND_OBJECT_PSEUDO_CONTAINER(
				table, 
				DeltaTable, 
				GetDelegators, 
				ubag, 
				DEBUG_TABLE_DELEGATORS_VISIBLE_KEY, 
				i->first,
				DEBUG_KEYTYPE_OBJECT_DELEGATION_KEY,
				DELEGATOR_PRECOND
			);
		}
#undef	DELEGATOR_PRECOND

	}

	DeltaValue field;
	field.FromNumber(table->Total());
	EXPAND_SLOT(DEBUG_TABLE_TOTAL_VISIBLE_KEY, DEBUG_KEYTYPE_OBJECT_PSEUDO_KEY);

	field.FromBool(table->IsOverloadingEnabled());
	EXPAND_SLOT(DEBUG_TABLE_OVERLOADING_ENABLED_VISIBLE_KEY, DEBUG_KEYTYPE_OBJECT_PSEUDO_KEY);

	field.FromString(table->GetCreationInfo().GetDesc());
	EXPAND_SLOT(DEBUG_TABLE_MADEIN_VISIBLE_KEY, DEBUG_KEYTYPE_OBJECT_PSEUDO_KEY);

	// Rest of normal user-defined fields.
	for (DeltaTable::const_iterator i = DPTR(table)->begin(); i != DPTR(table)->end(); ++i) {

		std::string keyType = DEBUG_KEYTYPE_NONE;

		DeltaValue index, content;
		i->GetIndex(&index);
		i->GetContentBySkippingUserCode(&content);

		if (i->GetType() == DeltaTable::AttributeType)
			ExpandAttribute(graph, node, values, (DeltaTable::Attribute*) &*i, depth);
		else {

			std::pair<std::string, bool> indexResult	= ExpandValueSlot(graph, values, &index, depth);	
			std::pair<std::string, bool> contentResult	= ExpandValueSlot(graph, values, &content, depth);	

			// We leave aggregate fields with their absolute reference as it is.
			// We surround numeric / bool / non-identifier / non-operator keys with [].

			if (index.Type() == DeltaValue_Number || index.Type() == DeltaValue_Bool)
				indexResult.first = std::string("[") + indexResult.first + "]";
			else 
			if (index.Type() == DeltaValue_String) {
				
				std::string& s	 = indexResult.first;
				util_ui32	 len =  s.length();

				DASSERT(s[0] == '\'' && s[len - 1] == '\'');
				s = s.substr(1, len - 2);	// Strip off quotes from indices.
			
				if (!DeltaString::IsIdentifier(s)) {
					if (DPTR(table)->IsOverloadingEnabled() && DeltaTable::IsOverloadableOperator(s))
						keyType = DEBUG_KEYTYPE_OBJECT_OPERATOR_KEY;
					else
						s = std::string("[\"") + s + "\"]";
				}
			}

			node.push_back(
				ObjectSlot(
					indexResult.first,
					indexResult.second,
					contentResult.first,
					contentResult.second,
					keyType
				)
			);
		}
	}
}

//*****************************

void DebugExprEvaluator::ExpandLibraryFunc (ObjectGraph& graph, ObjectValues* values, const DeltaValue* val, const DepthCounter& depth) {
	
	DASSERT(expandLibraryFuncs);
	CHECK_DEPTH_EXCEEDED_OR_ALREADY_EXPANDED(LibraryFunc);
	MAKE_NEW_NODE();

	AddFunctionSlot(node, val);

	const DeltaLibraryFuncArgsBinder*	binder	=	DNULLCHECK(val->GetLibraryFuncArgsBinder());
	std::pair<std::string, bool>		slot	=	ExpandValueSlot(
														graph, 
														values, 
														&DPTR(binder)->GetListValue(), 
														depth
													);	
	DASSERT(slot.second);	// Definitely aggregate.

	node.push_back(
			ObjectSlot(
				DEBUG_LIBFUNC_BOUNDARGS_VISIBLE_KEY,	// Key
				false,
				slot.first,								// Value
				true,
				DEBUG_KEYTYPE_AGGREGATE_PSEUDO_KEY
			)
		);
}

//*****************************

void DebugExprEvaluator::AddFunctionSlot (ObjectNode& node, const DeltaValue* val) {
	DASSERT(
		val->Type()	== DeltaValue_ProgramFunc	||
		val->Type()	== DeltaValue_MethodFunc	||
		val->Type()	== DeltaValue_LibraryFunc
	);
	node.push_back(
		ObjectSlot(
			DEBUG_ALLFUNCS_FUNC_VISIBLE_KEY,
			false,
			val->ConvertToStringBySkippingUserCode(),
			false,
			DEBUG_KEYTYPE_AGGREGATE_PSEUDO_KEY
		)
	);
}

//*****************************

const std::string DebugExprEvaluator::ExpandVMValue (
		ObjectGraph&				graph, 
		ObjectValues*				values, 
		const DeltaVirtualMachine*	vm, 
		const DepthCounter&			depth
	) {

	DeltaValue val;
	val.FromExternIdBySerialNo(DPTR(DPTR(vm)->GetExternId())->GetSerialNo());

	if (expandFuncVMs) {
		std::pair<std::string, bool> slot = ExpandValueSlot(graph, values, &val, depth);
		DASSERT(slot.second);	// Definitely aggregate.
		return slot.first;
	}
	else
		return val.ConvertToStringBySkippingUserCode();
}

//*****************************

const std::string DebugExprEvaluator::ExpandSelfValue (
		ObjectGraph&		graph, 
		ObjectValues*		values, 
		const DeltaTable*	self, 
		const DepthCounter& depth
	) {

	DeltaValue val(const_cast<DeltaTable*>(self));
	DASSERT(val.Type() == DeltaValue_Table);

	std::pair<std::string, bool> slot = ExpandValueSlot(graph, values, &val, depth);
	DASSERT(slot.second);	// Definitely aggregate.

	return slot.first;
}

//*****************************

bool DebugExprEvaluator::IsMethodOrProgramFuncVMNotYetExpanded (ObjectGraph& graph, const DeltaValue& mf) {
	if (expandFuncVMs) {
		DeltaVirtualMachine* vm = mf.Type() == DeltaValue_MethodFunc ? mf.GetMethodFuncVM() : mf.GetProgramFuncVM();
		return	graph.find(
					DeltaValueToDebugWatchValueInfo::ExternId_AbsoluteRef(
						DPTR(vm)->GetExternId()->GetSerialNo()
					)
				) == graph.end();
	} 
	else
		return false;
}

//*****************************

bool DebugExprEvaluator::IsMethodSelfNotYetExpanded (ObjectGraph& graph, const DeltaValue& m) {
	return	graph.find(
				DeltaValueToDebugWatchValueInfo::Table_AbsoluteRef(
					DPTR(m.GetMethodFuncSelf())->GetSerialNo()
				)
			) == graph.end();
}

//*****************************

bool DebugExprEvaluator::IsMethodOrProgramFuncContainerNotYetExpanded (ObjectGraph& graph, const DeltaValue& mf) {
	if (mf.GetResidencyType() != DeltaValue::Contained)
		return false;
	else {
		DeltaValue cont;
		((CollectableContainer*) mf.GetResidencyContext())->ToValue(&cont);
		return graph.find(DeltaValueToDebugWatchValueInfo::AbsoluteRef(cont)) == graph.end();
	}
}

//*****************************

bool DebugExprEvaluator::IsUsefulToExpandProgramFunc (ObjectGraph& graph, const DeltaValue& f) {
	return	IsMethodOrProgramFuncVMNotYetExpanded(graph, f)			||
			IsMethodOrProgramFuncContainerNotYetExpanded(graph, f)	||
			f.GetProgramFuncClosure();
}

bool DebugExprEvaluator::IsUsefulToExpandMethodFunc (ObjectGraph& graph, const DeltaValue& m) {
	return	IsMethodOrProgramFuncVMNotYetExpanded(graph, m)			||
			IsMethodSelfNotYetExpanded(graph, m)					||
			IsMethodOrProgramFuncContainerNotYetExpanded(graph, m)	||
			m.GetMethodFuncClosure();
}

//*****************************

void DebugExprEvaluator::ExpandProgramFunc	(
		ObjectGraph&		graph, 
		ObjectValues*		values, 
		const DeltaValue*	val, 
		const DepthCounter& depth
	) {

	DASSERT(expandProgramFuncs);
	CHECK_DEPTH_EXCEEDED_OR_ALREADY_EXPANDED(ProgramFunc);
	MAKE_NEW_NODE();
	
	AddFunctionSlot(node, val);

	const DeltaStdFuncInfo*	func;
	DeltaVirtualMachine*	vm;
	const_cast<DeltaValue*>(val)->ToProgramFunc(&func, &vm);

	node.push_back(
			ObjectSlot(
				DEBUG_FUNC_OR_METHOD_VM_VISIBLE_KEY,
				false,
				ExpandVMValue(graph, values, vm, depth),
				true,
				DEBUG_KEYTYPE_AGGREGATE_PSEUDO_KEY
			)
		);

	if (DeltaClosure* closure = val->GetProgramFuncClosure())
		ExpandClosure(graph, node, values, closure, func, depth);
}

//*****************************

void DebugExprEvaluator::ExpandMethodFunc (
		ObjectGraph&		graph, 
		ObjectValues*		values, 
		const DeltaValue*	val, 
		const DepthCounter& depth
	) {

	DASSERT(expandMethodFuncs);
	CHECK_DEPTH_EXCEEDED_OR_ALREADY_EXPANDED(MethodFunc);
	MAKE_NEW_NODE();
	
	AddFunctionSlot(node, val);

	const DeltaStdFuncInfo*	func;
	DeltaTable*				self;
	DeltaVirtualMachine*	vm;
	const_cast<DeltaValue*>(val)->ToMethodFunc(&func, &self, &vm);

	node.push_back(
			ObjectSlot(
				DEBUG_METHOD_SELF_VISIBLE_KEY,
				false,
				ExpandSelfValue(graph, values, self, depth),
				true,
				DEBUG_KEYTYPE_AGGREGATE_PSEUDO_KEY
			)
		);
	node.push_back(
			ObjectSlot(
				DEBUG_FUNC_OR_METHOD_VM_VISIBLE_KEY,
				false,
				ExpandVMValue(graph, values, vm, depth),
				true,
				DEBUG_KEYTYPE_AGGREGATE_PSEUDO_KEY
			)
		);

	if (DeltaClosure* closure = val->GetMethodFuncClosure())
		ExpandClosure(graph, node, values, closure, func, depth);
}

//*****************************

void DebugExprEvaluator::ExpandClosure (
		ObjectGraph&			graph, 
		ObjectNode&				node, 
		ObjectValues*			values, 
		const DeltaClosure*		closure, 
		const DeltaStdFuncInfo* func, 
		const DepthCounter&		depth
	) {

	typedef DeltaStdFuncInfo::ClosureVarsInfo ClosureVarsInfo;
	const ClosureVarsInfo& info = func->GetClosureVarsInfo();

	util_ui32 offset = 0;
	for (ClosureVarsInfo::const_iterator i = info.begin(); i != info.end(); ++i, ++offset) {

		DeltaValue* val = DPTR(closure)->GetVar(offset);
		DASSERT(val);

		std::pair<std::string, bool> content = ExpandValueSlot(graph, values, val, depth);	

		node.push_back(
			ObjectSlot(
				i->GetName(),
				false,
				content.first,
				content.second,
				DEBUG_KEYTYPE_EXTERNID_PSEUDO_KEY
			)
		);
	}	
}

//*****************************

void DebugExprEvaluator::ExpandBackClosure (
		ObjectGraph&			graph,
		ObjectValues*			values,
		CollectableContainer*	cont
	) {

	DASSERT(expandMethodFuncs || expandProgramFuncs);
	DASSERT(!strcmp(cont->GetExtClassString(), DELTA_CLOSURE_CLASSID));

	// Add each referrer value (always of program / method func type) directly into the subgraph.

	ubag<DeltaValue*> referrers = cont->GetReferrers();
	for (ubag<DeltaValue*>::iterator i = referrers.begin(); i != referrers.end(); ++i) {
		DASSERT(
			i->first->Type() == DeltaValue_ProgramFunc ||
			i->first->Type() == DeltaValue_MethodFunc
		);
		if (i->first->Type() == DeltaValue_ProgramFunc && expandProgramFuncs)
			ExpandProgramFunc(graph, values, i->first, DepthCounter(1));
		else
		if (i->first->Type() == DeltaValue_MethodFunc && expandMethodFuncs)
			ExpandMethodFunc(graph, values, i->first, DepthCounter(1));
	}
}

//*****************************

bool DebugExprEvaluator::EvaluateToObjectGraph (
		DeltaVirtualMachine*	vm,
		const char*				expr,
		ObjectGraph&			graph,
		util_ui32				depth,	// In terms of edges
		std::string&			simpleOrError
	) {
	simpleOrError.clear();

	if (depth)	// The algorithm uses depth in terms of nodes, so we increase it to imply edges.
		++depth;
	DeltaValue* val = Evaluate(vm, expr, simpleOrError);
	if (!val)
		return false;
	else
	if (val->Type() == DeltaValue_Table) {
		ExpandTable(graph, (ObjectValues*) 0, val, depth ? DepthCounter(depth) : DepthCounter());
		goto DONE_COMPOSITE;
	}
	else
	if (val->Type() == DeltaValue_ExternId) {
		ExpandExternId(graph, (ObjectValues*) 0, val, depth ? DepthCounter(depth) : DepthCounter());
		goto DONE_COMPOSITE;
	}
	else
	if (val->Type() == DeltaValue_ProgramFunc && expandProgramFuncs) {
		ExpandProgramFunc(graph, (ObjectValues*) 0, val, depth ? DepthCounter(depth) : DepthCounter());
		goto DONE_COMPOSITE;
	}
	else
	if (val->Type() == DeltaValue_MethodFunc && expandMethodFuncs) {
		ExpandMethodFunc(graph, (ObjectValues*) 0, val, depth ? DepthCounter(depth) : DepthCounter());
		goto DONE_COMPOSITE;
	}
	else
	if (val->Type() == DeltaValue_LibraryFunc && expandLibraryFuncs && val->GetLibraryFuncArgsBinder()) {
		ExpandLibraryFunc(graph, (ObjectValues*) 0, val, depth ? DepthCounter(depth) : DepthCounter());
		goto DONE_COMPOSITE;
	}
	else {	// Not a composite one.
		simpleOrError = ToString(val);
		return true;
	}

	DONE_COMPOSITE:
	ADD_START_NODE();
	return true;
}

////////////////////////////////////////////////////

void DebugExprEvaluator::ExpandUndirectedNeighbours (
		ObjectGraph&		graph, 
		ObjectList&			neighbours, 
		const DepthCounter& depth
	) {
	if (depth.More()) {

		ObjectList nextNeighbours;
		for (ObjectList::iterator i = neighbours.begin(); i != neighbours.end(); ++i)
			IdentifyAndCreateNodesForUndirectedNeighbours(graph, nextNeighbours, &*i);

		if (nextNeighbours.size())
			ExpandUndirectedNeighbours(graph, nextNeighbours, depth.Decrease());
	}
}

//*****************************

#define	CASE_OF(type)														\
	case DeltaValue_##type: {												\
		graph[DeltaValueToDebugWatchValueInfo::type##_AbsoluteRef(*val)];	\
		break;																\
	}

void DebugExprEvaluator::AddNewNode (ObjectGraph& graph, const DeltaValue* val) {
	switch (val->Type()) {
		CASE_OF(Table);
		CASE_OF(ExternId);	
		CASE_OF(ProgramFunc);	
		CASE_OF(MethodFunc);	
		CASE_OF(LibraryFunc);
		default: DASSERT(false);
	}
}

#undef	CASE_OF

//*****************************

#define	EXPAND_UNDIRECTED(type)												\
	if (true) {																\
		Expand##type(graph,	   &values, val, DepthCounter(1));				\
		Expand##type(subGraph, &values, val, DepthCounter(2));				\
	} else

void DebugExprEvaluator::IdentifyAndCreateNodesForUndirectedNeighbours (
		ObjectGraph&		graph, 
		ObjectList&			neighbours, 
		const DeltaValue*	val
	) {

	ObjectGraph			subGraph;	// Subgraph from the current node (val parameter).
	ObjectValues		values;		// Holds values for absolute references of subgraph objects.
	ubag<DeltaValue*>	referrers;	// Its referrers are also neighbours.

	// The first expansion just adds the object in the main graph. The second
	// simply adds it in the subGraph together with the direct referrents.

	if (val->Type() == DeltaValue_Table) 
		{ EXPAND_UNDIRECTED(Table); referrers = val->ToTable()->GetReferrers();	}
	else
	if (val->Type() == DeltaValue_ExternId) 
		{ EXPAND_UNDIRECTED(ExternId); referrers = val->GetExternId()->GetReferrers(); }
	else
	if (val->Type() == DeltaValue_ProgramFunc && expandProgramFuncs && IsUsefulToExpandProgramFunc(graph, *val))
		EXPAND_UNDIRECTED(ProgramFunc);
	else
	if (val->Type() == DeltaValue_MethodFunc && expandMethodFuncs && IsUsefulToExpandMethodFunc(graph, *val))
		EXPAND_UNDIRECTED(MethodFunc);
	else
	if (val->Type() == DeltaValue_LibraryFunc && expandLibraryFuncs && val->GetLibraryFuncArgsBinder())
		EXPAND_UNDIRECTED(LibraryFunc);
	else
		return;	// Nothing to expand here.

	// All containers of referrer values must be also added in the subgraph.

	for (ubag<DeltaValue*>::iterator i = referrers.begin(); i != referrers.end(); ++i)
		if (i->first->GetResidencyType() == DeltaValue::Contained) {

			CollectableContainer* cont = (CollectableContainer*) i->first->GetResidencyContext();
			DeltaValue contVal;
			cont->ToValue(&contVal);

			if (!strcmp(cont->GetExtClassString(), DELTA_CLOSUREVAR_CLASSID)) {	// The closures vars are treated specially.

				DASSERT(contVal.Type() == DeltaValue_Undefined);

				if (expandMethodFuncs || expandProgramFuncs) {

					// Expand every referrer value (always of closure type).
					ubag<DeltaValue*> referrers = cont->GetReferrers();

					for (ubag<DeltaValue*>::iterator i = referrers.begin(); i != referrers.end(); ++i) {

						DASSERT(i->first->Type() == DeltaValue_ExternId);	// Pseudo externid 
						DASSERT(DELTA_CLOSURE_PSEUDO_EXTERNID_VALUE == i->first->ToExternId());

						CollectableContainer* cont = (CollectableContainer*) i->first->GetResidencyContext();
						DASSERT(cont && !strcmp(cont->GetExtClassString(), DELTA_CLOSURE_CLASSID));

						DeltaValue contVal;
						cont->ToValue(&contVal);
						DASSERT(contVal.Type() == DeltaValue_Undefined);

						ExpandBackClosure(subGraph, &values, cont);
					}
				}
			}
			else
			if (!strcmp(cont->GetExtClassString(), DELTA_CLOSURE_CLASSID)) {	// The closures are treated specially.
				DASSERT(contVal.Type() == DeltaValue_Undefined);
				if (expandMethodFuncs || expandProgramFuncs)
					ExpandBackClosure(subGraph, &values, cont);
			}
			else	// The libfunc args binder is treated specially.
			if (!strcmp(cont->GetExtClassString(), DELTALIBFUNCARGSBINDER_CLASSID)) {

				DASSERT(contVal.Type() == DeltaValue_Undefined);

				// Add each referrer value (always of libfunc type) directly into the subgraph.
				if (expandLibraryFuncs) {
					ubag<DeltaValue*> referrers = cont->GetReferrers();
					for (ubag<DeltaValue*>::iterator i = referrers.begin(); i != referrers.end(); ++i) {
						DASSERT(i->first->Type() == DeltaValue_LibraryFunc);
						ExpandLibraryFunc(subGraph, &values, i->first, DepthCounter(1));
					}
				}
			}
			else	// Add directly into the subgraph (can be only table or externid).
				if (contVal.Type() == DeltaValue_ExternId)
					ExpandExternId(subGraph, &values, &contVal, DepthCounter(1));
				else
				if (contVal.Type() == DeltaValue_Table)
					ExpandTable(subGraph, &values, &contVal, DepthCounter(1));
				else
					DASSERT(false);
		}

	for (ObjectGraph::iterator i = subGraph.begin(); i != subGraph.end(); ++i)
		if (graph.find(i->first) == graph.end()) {	// Not in the graph?
			graph[i->first] = i->second;			// Its a new node.
			neighbours.push_back(values[i->first]);	// Add it to neighbours.
		}
}

#undef	EXPAND_UNDIRECTED

//*****************************

bool DebugExprEvaluator::EvaluateToUndirectedObjectGraph (
		DeltaVirtualMachine*	vm,
		const char*				expr,
		ObjectGraph&			graph,
		util_ui32				depth,
		std::string&			simpleOrError
	) {
	simpleOrError.clear();
	std::string absRef;
	DeltaValue* val = Evaluate(vm, expr, simpleOrError);
	if (!val)
		return false;
	else
	if (val->Type() == DeltaValue_Table									||
		val->Type() == DeltaValue_ExternId								||
		(val->Type() == DeltaValue_ProgramFunc && expandProgramFuncs)	||
		(val->Type() == DeltaValue_MethodFunc  && expandMethodFuncs)	||
		(val->Type() == DeltaValue_LibraryFunc && expandLibraryFuncs && val->GetLibraryFuncArgsBinder())) {	

		std::list<DeltaValue> nodes;
		nodes.push_back(*val);

		// Here the algorithm counts edges, so depth value is fine.
		ExpandUndirectedNeighbours(graph, nodes, depth ? DepthCounter(depth) : DepthCounter());
		if (graph.empty())
			simpleOrError = ToString(val);
		else
			ADD_START_NODE();
		return true;
	}
	else {	// Not a composite one.
		simpleOrError = ToString(val);
		return true;
	}
}

////////////////////////////////////////////////////

DeltaValue* DebugExprEvaluator::Evaluate (
		DeltaVirtualMachine*	vm,
		const char*				expr,
		std::string&			putErrorString
	) {

	DASSERT(expr && strlen(expr));
	ParseExpression(expr, vm);
	DeltaValue* result = (DeltaValue*) 0;

	if (DebugExprParseActions::HasError())
		putErrorString = DebugExprParseActions::GetLastError();
	else {
		putErrorString = "";
		DebugEvaluatedExpr* resultExpr = DebugExprParseActions::GetResultExpression();
		DASSERT(resultExpr);

		result = DNEW(DeltaValue);
		DPTR(result)->Assign(DPTR(resultExpr)->GetResult());		
	}

	CleanUpEvaluation();
	return result;		
}

////////////////////////////////////////////////////

const std::string DebugExprEvaluator::Assign (
		DeltaVirtualMachine*	vm,
		const char*				lvalue,
		const char*				rvalue
	) {

	ParseExpression(lvalue, vm);

	std::string result = "Assigned";
	if (DebugExprParseActions::HasError())
		result = DebugExprParseActions::GetLastError();
	else {

		DebugEvaluatedExpr* lvalueExpr = DebugExprParseActions::GetResultExpression();
		DASSERT(lvalueExpr);

		if (!DPTR(lvalueExpr)->IsLvalue())
			result = uconstructstr("'%s' not an lvalue", lvalue);
		else {
			CleanUpEvaluation();
			ParseExpression(rvalue, vm);

			if (DebugExprParseActions::HasError())
				result = DebugExprParseActions::GetLastError();
			else {
				DebugEvaluatedExpr* rvalueExpr = DebugExprParseActions::GetResultExpression();

				if (DPTR(lvalueExpr)->IsField()) {

					std::pair<DeltaValue*, DeltaValue*>& field = DPTR(lvalueExpr)->GetField();
					DeltaTable* table = field.first->ToTable();
					DPTR(table)->SetBySkippingUserCode(*field.second, *DPTR(rvalueExpr)->GetResult());
				}
				else
					DPTR(lvalueExpr)->GetResult()->Assign(DPTR(rvalueExpr)->GetResult());
			}
		}
	}

	CleanUpEvaluation();
	return result;		
}

////////////////////////////////////////////////////

bool DebugExprEvaluator::HasError (void) 
	{ return DebugExprParseActions::HasError(); }

////////////////////////////////////////////////////
