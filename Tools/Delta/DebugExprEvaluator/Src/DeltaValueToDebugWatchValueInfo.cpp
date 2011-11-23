// DeltaValueToDebugWatchValueInfo.cpp
// Converting a delta object to a watch value information.
// Supports browsing (tree view) of composite expressions.
// ScriptFighter Project.
// Debug expession evaluator.
// A. Savidis, August 2008.
//

#include "DeltaValueToDebugWatchValueInfo.h"
#include "DeltaDebugCommands.h"
#include "ustrings.h"
#include "DeltaObject.h"
#include "DeltaVirtualMachine.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaClosure.h"
#include "ustlhelpers.h"
#include <algorithm>

//////////////////////////////////////////////////////////////////

DeltaValueToDebugWatchValueInfo::ConversionMap* 
	DeltaValueToDebugWatchValueInfo::converters		= (ConversionMap*) 0;

DeltaValueToDebugWatchValueInfo::KeyConversionMap* 
	DeltaValueToDebugWatchValueInfo::keyConverters	= (KeyConversionMap*) 0;

DeltaValueToDebugWatchValueInfo::AbsRefMap*			 
	DeltaValueToDebugWatchValueInfo::absRefFuncs	= (AbsRefMap*) 0;

void DeltaValueToDebugWatchValueInfo::SingletonCreate (void) {
	unew(converters);
	(*DPTR(converters))[DeltaValue_Number]			= &ConvertSimple;
	(*DPTR(converters))[DeltaValue_String]			= &ConvertSimple;		
	(*DPTR(converters))[DeltaValue_Table]			= &ConvertTable;		
	(*DPTR(converters))[DeltaValue_ProgramFunc]		= &ConvertProgramFunc;	
	(*DPTR(converters))[DeltaValue_LibraryFunc]		= &ConvertLibraryFunc;	
	(*DPTR(converters))[DeltaValue_Bool]			= &ConvertSimple;		
	(*DPTR(converters))[DeltaValue_ExternId]		= &ConvertExternId;	
	(*DPTR(converters))[DeltaValue_Nil]				= &ConvertSimple;			
	(*DPTR(converters))[DeltaValue_MethodFunc]		= &ConvertMethodFunc;	
	(*DPTR(converters))[DeltaValue_Undefined]		= &ConvertSimple;	

	unew(keyConverters);
	(*DPTR(keyConverters))[DeltaValue_Table]		= &ConvertTableToKey;		
	(*DPTR(keyConverters))[DeltaValue_ProgramFunc]	= &ConvertProgramFuncToKey;	
	(*DPTR(keyConverters))[DeltaValue_ExternId]		= &ConvertExternIdToKey;	
	(*DPTR(keyConverters))[DeltaValue_MethodFunc]	= &ConvertMethodFuncToKey;	

	unew(absRefFuncs);
	(*DPTR(absRefFuncs))[DeltaValue_Table]			= &Table_AbsoluteRef;		
	(*DPTR(absRefFuncs))[DeltaValue_ExternId]		= &ExternId_AbsoluteRef;	
	(*DPTR(absRefFuncs))[DeltaValue_ProgramFunc]	= &ProgramFunc_AbsoluteRef;	
	(*DPTR(absRefFuncs))[DeltaValue_MethodFunc]		= &MethodFunc_AbsoluteRef;	
	(*DPTR(absRefFuncs))[DeltaValue_LibraryFunc]	= &LibraryFunc_AbsoluteRef;	
}

void DeltaValueToDebugWatchValueInfo::SingletonDestroy (void)
	{ udelete(converters); udelete(keyConverters); udelete(absRefFuncs); }

void DeltaValueToDebugWatchValueInfo::Convert (const DeltaValue& obj, DebugWatchValueInfo* at) {
	DASSERT(converters);
	DASSERT(DPTR(converters)->find(obj.Type()) != DPTR(converters)->end());
	(*(*DPTR(converters))[obj.Type()])(obj, at);
}

//////////////////////////////////////////////////////////////////

void DeltaValueToDebugWatchValueInfo::AddStringKeys (
		const std::vector<std::string>&				keys, 
		std::list<DebugWatchValueInfo::Content>&	contents,
		bool										removeDotPrefix
	){
	for (std::vector<std::string>::const_iterator i = keys.begin(); i != keys.end(); ++i) {
		std::string desc, keyType = DEBUG_KEYTYPE_NONE;
		if (removeDotPrefix) {
			if (ustrprefix(DEBUG_DOUBLE_DOT, i->c_str())) {		// For ident-key object slots (fields or attributes).
				desc = i->c_str() + 2;
				if (desc[0] == DEBUG_ATTR_SUFFIX_START_CHAR)	// Is object attribute?
					keyType = DEBUG_KEYTYPE_OBJECT_ATTRIBUTE_KEY;
			}
			else
			if (*i->begin() == DEBUG_DOT_CHAR) {				// For extern id user keys.
				desc = i->c_str() + 1;
				keyType = DEBUG_KEYTYPE_EXTERNID_USER_PSEUDO_KEY;
			}
			else
			if (ustrprefix("[[", i->c_str())) {					// For all other object slots. 
				util_ui32 n = (util_ui32) i->length(); 
				DASSERT(n > 4 && (*i)[ n - 1] == ']' &&  (*i)[ n - 2] == ']');
				desc = i->substr(1, n - 2);
			}
			else
				desc = *i;
		}
		else
			desc = *i;
		contents.push_back(
			DebugWatchValueInfo::Content(
				*i,
				WATCHVALUE_NODESC(),
				WATCHVALUE_VISIBLE(),
				DebugWatchValueInfo::FieldKey(
					desc, 
					WATCHVALUE_NOKEYREF()
				),
				keyType
			)
		);
	}
}

//////////////////////////////////////////////////////////////////

const std::string DeltaValueToDebugWatchValueInfo::ToString (const DeltaValue& val)
	{ return ToString(&val); }

const std::string DeltaValueToDebugWatchValueInfo::ToString (const DeltaValue* val) {
	std::string s;
	if (val->Type() == DeltaValue_String) {
		util_ui32 maxLen = DeltaValue::GetMaxLengthForConvertToStringBySkippingUserCode();
		DeltaValue::SetMaxLengthForConvertToStringBySkippingUserCode(0);
		s = std::string("'") + val->ConvertToStringBySkippingUserCode() + std::string("'");
		DeltaValue::SetMaxLengthForConvertToStringBySkippingUserCode(maxLen);
	}
	else
		s = val->ConvertToStringBySkippingUserCode();
	return	s + (val->GetTypeTag().empty() ? "" : uconstructstr("<tag:'%s'>", val->GetTypeTag().c_str()));
}

//////////////////////////////////////////////////////////////////

void DeltaValueToDebugWatchValueInfo::ConvertSimple (const DeltaValue& obj, DebugWatchValueInfo* at)
	{ at->Set(ToString(obj)); }		

//////////////////////////////////////////////////////////////////

static const char* protoExtraKeys[] = {
	DEBUG_DOT DEBUG_TABLE_DERIVED_HIDDEN_KEY,		DEBUG_TABLE_DERIVED_VISIBLE_KEY,		
	DEBUG_DOT DEBUG_TABLE_MOSTDERIVED_HIDDEN_KEY,	DEBUG_TABLE_MOSTDERIVED_VISIBLE_KEY,
	DEBUG_DOT DEBUG_TABLE_BASES_HIDDEN_KEY,			DEBUG_TABLE_BASES_VISIBLE_KEY	
};

#define	DELEGATES_KEY	0
#define	DELEGATORS_KEY	2
#define	OVERLOADING_KEY	4
#define	TOTAL_KEY		6
#define	MADEIN_KEY		8

static const char* tableExtraKeys[] = {
	DEBUG_DOT DEBUG_TABLE_DELEGATES_HIDDEN_KEY,				DEBUG_TABLE_DELEGATES_VISIBLE_KEY,		
	DEBUG_DOT DEBUG_TABLE_DELEGATORS_HIDDEN_KEY,			DEBUG_TABLE_DELEGATORS_VISIBLE_KEY,
	DEBUG_DOT DEBUG_TABLE_OVERLOADING_ENABLED_HIDDEN_KEY,	DEBUG_TABLE_OVERLOADING_ENABLED_VISIBLE_KEY,
	DEBUG_DOT DEBUG_TABLE_TOTAL_HIDDEN_KEY,					DEBUG_TABLE_TOTAL_VISIBLE_KEY,
	DEBUG_DOT DEBUG_TABLE_MADEIN_HIDDEN_KEY,				DEBUG_TABLE_MADEIN_VISIBLE_KEY
};

static util_ui32 indicesOfstdPseudoKeys[] = { 
	TOTAL_KEY,
	OVERLOADING_KEY,
	MADEIN_KEY
};

//**************************

static void AddExtraKey (
		std::list<DebugWatchValueInfo::Content>&	contents, 
		const std::string&							subIndex, 
		const std::string&							displayedKey,
		const std::string&							keyType
	) {
	contents.push_back(
		DebugWatchValueInfo::Content(
			subIndex,
			WATCHVALUE_NODESC(),
			WATCHVALUE_VISIBLE(),
			DebugWatchValueInfo::FieldKey(displayedKey, WATCHVALUE_NOKEYREF()), 
			keyType
		)
	);
}

//**************************

void DeltaValueToDebugWatchValueInfo::ConvertTable (const DeltaValue& val, DebugWatchValueInfo* at) {

	DeltaTable* table = const_cast<DeltaValue&>(val).ToTable();
	std::list<DebugWatchValueInfo::Content> contents;

	// Special keys are added only on user-made objects, being
	// always prototypes. The rest, like arguments table or delegates table,
	// are created by the runtime aiming to play like typical numerically indexed tables.

	if (table->IsOfClass(DELTA_OBJECT_CLASS_ID)) {

		DASSERT(GetDeltaObjectValidator()->Validate((DeltaObject*) table));

		// std keys for inheritance (always put first).
		DeltaObject*	obj	= (DeltaObject*) table;
		util_ui32		n	= uarraysize(protoExtraKeys), i = n;

		if (DPTR(obj)->GetBases().size())		i -= 2; else n -= 2;
		if (DPTR(obj)->GetMostDerived() != obj) i = 0;

		for (; i < n; i += 2)
			AddExtraKey(
				contents, 
				protoExtraKeys[i], protoExtraKeys[i+1], 
				DEBUG_KEYTYPE_OBJECT_INHERITANCE_KEY
			);

		// Std keys for delegation.
		if (DPTR(table)->HasDelegates())
			AddExtraKey(
				contents, 
				tableExtraKeys[DELEGATES_KEY], tableExtraKeys[DELEGATES_KEY + 1], 
				DEBUG_KEYTYPE_OBJECT_DELEGATION_KEY
			);

		if (DPTR(table)->HasDelegators())
			AddExtraKey(
				contents, 
				tableExtraKeys[DELEGATORS_KEY], tableExtraKeys[DELEGATORS_KEY + 1], 
				DEBUG_KEYTYPE_OBJECT_DELEGATION_KEY
			);

		for (i = 0, n = uarraysize(indicesOfstdPseudoKeys); i < n; ++i)
			AddExtraKey(
				contents, 
				tableExtraKeys[indicesOfstdPseudoKeys[i]], tableExtraKeys[indicesOfstdPseudoKeys[i] + 1], 
				DEBUG_KEYTYPE_OBJECT_PSEUDO_KEY
			);
	}

	std::vector<std::string> 				stringKeys;
	std::vector<double> 					originalNumericKeys;
	std::vector<std::string> 				boolKeys;			// Convert to [%s] form.
	std::list<DebugWatchValueInfo::Content> referenceableKeys;	// Separate list, as should appear at the end.

	// Extract rest of keys in separate groups (for sorting purposes).
	// Referenceable composite keys are put altogether.
	//
	DeltaValue index;
	for (DeltaTable::const_iterator i = DPTR(table)->begin(); i != DPTR(table)->end(); ++i) {

		i->GetIndex(&index);
		DebugWatchValueInfo::Content content;

		switch (index.Type()) {

			case DeltaValue_String: {
				std::string indexString = index.ToString();
				if (DeltaString::IsIdentifier(indexString))
						stringKeys.push_back(
							std::string(DEBUG_DOUBLE_DOT) + 
							(i->GetType() == DeltaTable::AttributeType ? DEBUG_ATTR_SUFFIX_START : "") +
							indexString
						);
				else {
					std::string s = uconstructstr("[[\"%s\"]]", indexString.c_str());
					if (DPTR(table)->IsOverloadingEnabled() && DeltaTable::IsOverloadableOperator(indexString))
						AddExtraKey(contents, s, indexString, DEBUG_KEYTYPE_OBJECT_OPERATOR_KEY);
					else
						stringKeys.push_back(s); 
				}
				break;
			}

			case DeltaValue_LibraryFunc:
				if (!index.GetLibraryFuncArgsBinder())
					stringKeys.push_back(index.ConvertToStringBySkippingUserCode());
				else {
					ConvertLibraryFuncToKey(index, &content);
					referenceableKeys.push_back(content);
				}
				break;

			case DeltaValue_Number:	
			originalNumericKeys.push_back(index.ToNumber()); break;

			case DeltaValue_Bool:		
			boolKeys.push_back(std::string("[[") + ustrlower(ubool2conststr(index.ToBool())) + "]]"); break;

			case DeltaValue_Table:
			case DeltaValue_ProgramFunc:
			case DeltaValue_MethodFunc:
			case DeltaValue_ExternId:

			DASSERT(DPTR(keyConverters)->find(index.Type()) != DPTR(keyConverters)->end());
			(*(*DPTR(keyConverters))[index.Type()])(index, &content);
			referenceableKeys.push_back(content);
			break;

			default: DASSERT(false);
		}		
	}

	std::sort(stringKeys.begin(),			stringKeys.end());
	std::sort(originalNumericKeys.begin(),	originalNumericKeys.end());

	std::vector<std::string> numericKeys; 
	for (	std::vector<double>::iterator i = originalNumericKeys.begin();
			i != originalNumericKeys.end();
			++i		)
			numericKeys.push_back(
				std::string("[[") +  unum2string(*i) + "]]"
			);

	AddStringKeys(stringKeys,	contents,	true);			// Remove preceeding .. from ident keys.
	AddStringKeys(numericKeys,	contents,	true);			// Turn [[<x>]] to [<x>] for <x> a numeric key.
	AddStringKeys(boolKeys,		contents,	true);			// Same as before for bool keys.
	
	ulistcopy(referenceableKeys, contents);

	at->Set(
		DebugWatchValueInfo::Composite(
			ToString(val), 
			Table_AbsoluteRef(val),
			contents
		)
	);
}

//////////////////////////////////////////////////////////////////

void DeltaValueToDebugWatchValueInfo::ConvertProgramFunc (const DeltaValue& val, DebugWatchValueInfo* at) {

	const DeltaStdFuncInfo*	func;
	DeltaClosure*			closure;
	DeltaVirtualMachine*	vm;
	const_cast<DeltaValue&>(val).ToProgramFunc(&func, &vm, &closure);	// Check function.
	DASSERT(vm && func);

	std::list<DebugWatchValueInfo::Content> contents;
	contents.push_back(
		DebugWatchValueInfo::Content(
			DEBUG_DOT DEBUG_FUNC_OR_METHOD_VM_HIDDEN_KEY,
			WATCHVALUE_NODESC(),
			WATCHVALUE_VISIBLE(),
			DebugWatchValueInfo::FieldKey(DEBUG_FUNC_OR_METHOD_VM_VISIBLE_KEY, WATCHVALUE_NOKEYREF()),
			DEBUG_KEYTYPE_AGGREGATE_PSEUDO_KEY
		)
	);
	if (DPTR(vm)->HasDebugInfo())
		contents.push_back(
			DebugWatchValueInfo::Content(
				DEBUG_DOT DEBUG_FUNC_OR_METHOD_DEFINEDIN_HIDDEN_KEY,
				WATCHVALUE_NODESC(),
				WATCHVALUE_VISIBLE(),
				DebugWatchValueInfo::FieldKey(DEBUG_FUNC_OR_METHOD_DEFINEDIN_VISIBLE_KEY, WATCHVALUE_NOKEYREF()),
				DEBUG_KEYTYPE_AGGREGATE_PSEUDO_KEY
			)
		);

	if (closure && closure->GetTotal() >= (util_ui32) func->GetClosureVarsInfo().size())
		contents.push_back(
			DebugWatchValueInfo::Content(
				DEBUG_DOT DEBUG_FUNC_OR_METHOD_CLOSURE_HIDDEN_KEY,
				WATCHVALUE_NODESC(),
				WATCHVALUE_VISIBLE(),
				DebugWatchValueInfo::FieldKey(DEBUG_FUNC_OR_METHOD_CLOSURE_VISIBLE_KEY, WATCHVALUE_NOKEYREF()),
				DEBUG_KEYTYPE_AGGREGATE_PSEUDO_KEY
			)
		);

	at->Set(
		DebugWatchValueInfo::Composite(
			ToString(val), 
			ProgramFunc_AbsoluteRef(val),
			contents
		)
	);
}	

//////////////////////////////////////////////////////////////////

static const char* externIdReservedKeys[] = {
	DELTA_EXTERNID_TYPE_INDEX,		DEBUG_EXTERNID_TYPE_VISIBLE_KEY,
	DELTA_EXTERNID_USERDATA_INDEX,	DEBUG_EXTERNID_USERDATA_VISIBLE_KEY,
	DELTA_EXTERNID_MADEIN_INDEX,	DEBUG_EXTERNID_MADEIN_VISIBLE_KEY
};

void DeltaValueToDebugWatchValueInfo::ConvertExternId (const DeltaValue& val, DebugWatchValueInfo* at) {
	
	const DeltaExternIdFieldGetter*	fieldGetter = val.GetExternIdFieldGetter();
	void* nativeVal = const_cast<DeltaValue&>(val).ToExternId();

	if (fieldGetter && !DPTR(fieldGetter)->IsValidValue(nativeVal)) 
		{ ConvertSimple(val, at); return; }

	std::list<DebugWatchValueInfo::Content> contents;

	// Add reserved keys, with their appropriate visible keys.
	if (!fieldGetter || DPTR(fieldGetter)->GetIncludeReservedKeys()) {

		std::list<std::string> extraKeys;
		DeltaExternId::AddReservedFieldKeys(&extraKeys);

		for (std::list<std::string>::iterator i = extraKeys.begin(); i != extraKeys.end(); ++i) {

			std::string visibleKey	= *i;

			for (util_ui32 j = 0, n = uarraysize(externIdReservedKeys); j < n; j +=2)
				if (*i == std::string(DNULLCHECK(externIdReservedKeys[j]))) {
					visibleKey = DNULLCHECK(externIdReservedKeys[j + 1]);
					break; 
				}

			AddExtraKey(
				contents, 
				std::string(DEBUG_DOT) + *i, visibleKey, 
				DEBUG_KEYTYPE_EXTERNID_PSEUDO_KEY
			);
		}
	}

	// Add user defined keys through the field getter.

	if (fieldGetter) {

		std::list<std::string> keys;

		DPTR(fieldGetter)->AddStringKeys(keys, nativeVal);
		if (DPTR(fieldGetter)->HasKeyToGetKeys())
			keys.remove(DPTR(fieldGetter)->GetKeyToGetKeys());

		std::vector<std::string> allKeys;
		for (std::list<std::string>::iterator i = keys.begin(); i != keys.end(); ++i)		
			allKeys.push_back(std::string(".") + *i);

		for (util_ui32 i = 0, n = DPTR(fieldGetter)->GetNumericKeys(nativeVal); i < n; ++i)
			allKeys.push_back(uconstructstr("[%u]", i));

		AddStringKeys(allKeys, contents, true);
	}

	at->Set(
		DebugWatchValueInfo::Composite(
			ToString(val), 
			ExternId_AbsoluteRef(val),
			contents
		)
	);
}

//////////////////////////////////////////////////////////////////

void DeltaValueToDebugWatchValueInfo::ConvertLibraryFunc (const DeltaValue& obj, DebugWatchValueInfo* at) {
	
	const DeltaLibraryFuncArgsBinder* binder = obj.GetLibraryFuncArgsBinder();
	if (!binder)
		ConvertSimple(obj, at);
	else
		at->Set(
			DebugWatchValueInfo::Composite(
				ToString(obj), 
				LibraryFunc_AbsoluteRef(obj),
				DebugWatchValueInfo::Content(
					DEBUG_DOT DEBUG_LIBFUNC_BOUNDARGS_HIDDEN_KEY,
					WATCHVALUE_NODESC(),
					WATCHVALUE_VISIBLE(),
					DebugWatchValueInfo::FieldKey(DEBUG_LIBFUNC_BOUNDARGS_VISIBLE_KEY, WATCHVALUE_NOKEYREF()),
					DEBUG_KEYTYPE_AGGREGATE_PSEUDO_KEY
				)
			)
		);
}

//////////////////////////////////////////////////////////////////

void DeltaValueToDebugWatchValueInfo::ConvertMethodFunc (const DeltaValue& val, DebugWatchValueInfo* at) {

	const DeltaStdFuncInfo*	func;
	DeltaClosure*			closure;
	DeltaTable*				self;
	DeltaVirtualMachine*	vm;
	const_cast<DeltaValue&>(val).ToMethodFunc(&func, &self, &vm, &closure);	// Check method.
	DASSERT(vm && func && self);

	std::list<DebugWatchValueInfo::Content> contents;
	contents.push_back(
		DebugWatchValueInfo::Content(
			DEBUG_DOT DEBUG_FUNC_OR_METHOD_VM_HIDDEN_KEY,
			WATCHVALUE_NODESC(),
			WATCHVALUE_VISIBLE(),
			DebugWatchValueInfo::FieldKey(DEBUG_FUNC_OR_METHOD_VM_VISIBLE_KEY, WATCHVALUE_NOKEYREF()),
			DEBUG_KEYTYPE_AGGREGATE_PSEUDO_KEY
		)
	);
	contents.push_back(
		DebugWatchValueInfo::Content(
			DEBUG_DOT DELTA_SELF_POINTER_ID,
			WATCHVALUE_NODESC(),
			WATCHVALUE_VISIBLE(),
			DebugWatchValueInfo::FieldKey(DEBUG_METHOD_SELF_VISIBLE_KEY, WATCHVALUE_NOKEYREF()),
			DEBUG_KEYTYPE_AGGREGATE_PSEUDO_KEY
		)
	);

	if (DPTR(vm)->HasDebugInfo())
		contents.push_back(
			DebugWatchValueInfo::Content(
				DEBUG_DOT DEBUG_FUNC_OR_METHOD_DEFINEDIN_HIDDEN_KEY,
				WATCHVALUE_NODESC(),
				WATCHVALUE_VISIBLE(),
				DebugWatchValueInfo::FieldKey(DEBUG_FUNC_OR_METHOD_DEFINEDIN_VISIBLE_KEY, WATCHVALUE_NOKEYREF()),
				DEBUG_KEYTYPE_AGGREGATE_PSEUDO_KEY
			)
		);

	if (closure && closure->GetTotal() >= (util_ui32) func->GetClosureVarsInfo().size())
		contents.push_back(
			DebugWatchValueInfo::Content(
				DEBUG_DOT DEBUG_FUNC_OR_METHOD_CLOSURE_HIDDEN_KEY,
				WATCHVALUE_NODESC(),
				WATCHVALUE_VISIBLE(),
				DebugWatchValueInfo::FieldKey(DEBUG_FUNC_OR_METHOD_CLOSURE_VISIBLE_KEY, WATCHVALUE_NOKEYREF()),
				DEBUG_KEYTYPE_AGGREGATE_PSEUDO_KEY
			)
		);

	at->Set(
		DebugWatchValueInfo::Composite(
			ToString(val), 
			MethodFunc_AbsoluteRef(val),
			contents
		)
	);
}

//////////////////////////////////////////////////////////////////

void DeltaValueToDebugWatchValueInfo::ConvertTableToKey (const DeltaValue& obj, DebugWatchValueInfo::Content* at) {

	DeltaTable* table = const_cast<DeltaValue&>(obj).ToTable();
	std::string ref(Table_AbsoluteRef(DPTR(table)->GetSerialNo()));

	at->Set(
		std::string("[[") + ref + "]]", // subIndex
		WATCHVALUE_NODESC(),
		WATCHVALUE_VISIBLE(),
		DebugWatchValueInfo::FieldKey(
			uconstructstr("%s(0x%x)", table->GetExtClassString(), table->GetSerialNo()), // displayed key
			WATCHVALUE_KEYREF(ref)
		)
	);
}

//********************************

void DeltaValueToDebugWatchValueInfo::ConvertMethodFuncToKey(const DeltaValue& obj, DebugWatchValueInfo::Content* at) {

	DeltaCodeAddress		addr;
	DeltaTable*				table;
	DeltaVirtualMachine*	vm;
	DeltaClosure*			closure;
	const_cast<DeltaValue&>(obj).ToMethodFunc(&addr, &table, &vm, &closure);

	std::string ref(MethodFunc_AbsoluteRef(addr, table, vm, closure));

	at->Set(
		std::string("[[") + ref + "]]", // subIndex
		WATCHVALUE_NODESC(),
		WATCHVALUE_VISIBLE(), 
		DebugWatchValueInfo::FieldKey(
			uconstructstr(  // displayed key
				"%s(%d, object 0x%x, vm 0x%x)", 
				obj.TypeStr(),
				addr, 
				DPTR(table)->GetSerialNo(), 
				DPTR(vm)->GetSerialNo()
			),
			WATCHVALUE_KEYREF(ref)
		)
	);
}

//********************************

void DeltaValueToDebugWatchValueInfo::ConvertProgramFuncToKey (const DeltaValue& obj, DebugWatchValueInfo::Content* at) {

	DeltaCodeAddress		addr;
	DeltaVirtualMachine*	vm;
	DeltaClosure*			closure;

	const_cast<DeltaValue&>(obj).ToProgramFunc(&addr, &vm, &closure);

	std::string ref(ProgramFunc_AbsoluteRef(addr, vm, closure));

	at->Set(
		std::string("[[") + ref + "]]", // subIndex
		WATCHVALUE_NODESC(),
		WATCHVALUE_VISIBLE(), 
		DebugWatchValueInfo::FieldKey(
			uconstructstr(  // displayed key
				"%s(%u, vm 0x%x)", 
				obj.TypeStr(),
				addr, 
				DPTR(vm)->GetSerialNo()
			),
			WATCHVALUE_KEYREF(ref)
		)
	);
}

//********************************

void DeltaValueToDebugWatchValueInfo::ConvertExternIdToKey (const DeltaValue& obj, DebugWatchValueInfo::Content* at) {
	
	util_ui32	serialNo = obj.ToExternIdSerialNo();
	std::string ref(ExternId_AbsoluteRef(serialNo));

	at->Set(
		std::string("[[") + ref + "]]", // subIndex
		WATCHVALUE_NODESC(),
		WATCHVALUE_VISIBLE(), 
		DebugWatchValueInfo::FieldKey(
			uconstructstr(
				"%s(%s) 0x%x", 
				obj.TypeStr(), 
				obj.GetExternIdTypeString().c_str(), 
				serialNo
			), // displayed key
			WATCHVALUE_KEYREF(ref)
		)
	);
}

void DeltaValueToDebugWatchValueInfo::ConvertLibraryFuncToKey (const DeltaValue& obj, DebugWatchValueInfo::Content* at) {

	std::string ref(LibraryFunc_AbsoluteRef(obj));

	at->Set(
		std::string("[[") + ref + "]]", // subIndex
		WATCHVALUE_NODESC(),
		WATCHVALUE_VISIBLE(), 
		DebugWatchValueInfo::FieldKey(
			obj.ConvertToStringBySkippingUserCode(), // displayed key
			WATCHVALUE_KEYREF(ref)
		)
	);
}

//////////////////////////////////////////////////////////////////

const std::string DeltaValueToDebugWatchValueInfo::Table_AbsoluteRef (const DeltaValue& obj)
	{ return Table_AbsoluteRef(DPTR(const_cast<DeltaValue&>(obj).ToTable())->GetSerialNo()); }

const std::string DeltaValueToDebugWatchValueInfo::Table_AbsoluteRef (util_ui32 serialNo)
	{ return uconstructstr(DEBUG_OBJECT_ABSREF_PREFIX "(0x%x)", serialNo); }

//**************************

const std::string DeltaValueToDebugWatchValueInfo::ExternId_AbsoluteRef (const DeltaValue& obj)
	{ return ExternId_AbsoluteRef(const_cast<DeltaValue&>(obj).ToExternIdSerialNo()); }

const std::string DeltaValueToDebugWatchValueInfo::ExternId_AbsoluteRef (util_ui32 serialNo)
	{ return uconstructstr(DEBUG_EXTERNID_ABSREF_PREFIX "(0x%x)", serialNo); }

//**************************

const std::string DeltaValueToDebugWatchValueInfo::MethodFunc_AbsoluteRef (
		DeltaCodeAddress		addr,
		DeltaTable*				table,
		DeltaVirtualMachine*	vm,
		const DeltaClosure*		closure
	) {
	if (closure)
		return uconstructstr(
				DEBUG_METHODFUNC_ABSREF_PREFIX "(0x%x,0x%x,0x%x,0x%x)", 
				addr,
				DPTR(table)->GetSerialNo(),
				DPTR(vm)->GetSerialNo(),
				DPTR(closure)->GetSerialNo()
			);
	else
		return uconstructstr(
				DEBUG_METHODFUNC_ABSREF_PREFIX "(0x%x,0x%x,0x%x)", 
				addr,
				DPTR(table)->GetSerialNo(),
				DPTR(vm)->GetSerialNo()
			); 
}

//**************************

const std::string DeltaValueToDebugWatchValueInfo::MethodFunc_AbsoluteRef (const DeltaValue& val) {
	DeltaCodeAddress		addr;
	DeltaTable*				table;
	DeltaVirtualMachine*	vm;
	DeltaClosure*			closure;
	const_cast<DeltaValue&>(val).ToMethodFunc(&addr, &table, &vm, &closure);
	return MethodFunc_AbsoluteRef(addr, table, vm, closure);
}

//**************************

const std::string DeltaValueToDebugWatchValueInfo::ProgramFunc_AbsoluteRef (
		DeltaCodeAddress		addr,
		DeltaVirtualMachine*	vm,
		const DeltaClosure*		closure
	) {
		if (closure)
			return uconstructstr(
						DEBUG_PROGRAMFUNC_ABSREF_PREFIX "(0x%x,0x%x,0x%x)", 
						addr, 
						DPTR(vm)->GetSerialNo(),
						DPTR(closure)->GetSerialNo()
					);
		else
			return uconstructstr(
						DEBUG_PROGRAMFUNC_ABSREF_PREFIX "(0x%x,0x%x)", 
						addr, 
						DPTR(vm)->GetSerialNo()
					);
}

//**************************

const std::string DeltaValueToDebugWatchValueInfo::ProgramFunc_AbsoluteRef (const DeltaValue& val) {

	DeltaVirtualMachine*	vm;
	DeltaCodeAddress		addr;
	DeltaClosure*			closure;
	const_cast<DeltaValue&>(val).ToProgramFunc(&addr, &vm, &closure);

	return ProgramFunc_AbsoluteRef(addr, vm, closure);
}

//**************************

const std::string DeltaValueToDebugWatchValueInfo::LibraryFunc_AbsoluteRef (const DeltaValue& obj) 
	{ return LibraryFunc_AbsoluteRef(obj.ToLibraryFunc(), obj.GetLibraryFuncArgsBinder()); }

const std::string DeltaValueToDebugWatchValueInfo::LibraryFunc_AbsoluteRef (
		DeltaLibraryFunc f,
		const DeltaLibraryFuncArgsBinder* binder
	) {
	DASSERT(binder);
	return	uconstructstr(
				DEBUG_LIBRARYFUNC_ABSREF_PREFIX "(%s,0x%x)", 
				DeltaLibFuncBinder::GetNameByAddress(f),
				DPTR(binder)->GetSerialNo()
			);
}

//**************************

const std::string DeltaValueToDebugWatchValueInfo::AbsoluteRef (const DeltaValue& val) {
	DASSERT(absRefFuncs);
	DASSERT(DPTR(absRefFuncs)->find(val.Type()) != DPTR(absRefFuncs)->end());
	return (*(*DPTR(absRefFuncs))[val.Type()])(val);
}

//////////////////////////////////////////////////////////////////
