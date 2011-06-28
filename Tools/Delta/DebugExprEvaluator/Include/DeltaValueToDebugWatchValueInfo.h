// DeltaValueToDebugWatchValueInfo.h
// Converting a delta object to a watch value information.
// ScriptFighter Project.
// Debug expession evaluator.
// A. Savidis, August 2008.
//

#ifndef	DELTAVALUETODEBUGWATCHVALUEINFO_H
#define	DELTAVALUETODEBUGWATCHVALUEINFO_H

#include "DebugWatchValueInfo.h"
#include "DeltaValue.h"
#include "DeltaLibraryFuncArgsBinder.h"
#include "DeltaExternId.h"
#include "usingleton.h"
#include <map>

// All extra keys provided by the debugger backend are defined in a way
// they never collide with user-defined keys.

#define	DEBUG_TABLE_DERIVED_VISIBLE_KEY					"{derived}"
#define	DEBUG_TABLE_MOSTDERIVED_VISIBLE_KEY				"{mostderived}"
#define	DEBUG_TABLE_BASES_VISIBLE_KEY					"{bases}"
#define	DEBUG_TABLE_DELEGATES_VISIBLE_KEY				"{delegates}"
#define	DEBUG_TABLE_DELEGATORS_VISIBLE_KEY				"{delegators}"
#define	DEBUG_TABLE_MADEIN_VISIBLE_KEY					"{madein}"
#define	DEBUG_TABLE_OVERLOADING_ENABLED_VISIBLE_KEY		"{overloadingenabled}"
#define	DEBUG_TABLE_TOTAL_VISIBLE_KEY					"{total}"
#define	DEBUG_LIBFUNC_BOUNDARGS_VISIBLE_KEY				"{boundargs}"
#define	DEBUG_ALLFUNCS_FUNC_VISIBLE_KEY					"{func}"
#define	DEBUG_FUNC_OR_METHOD_VM_VISIBLE_KEY				"{vm}"
#define	DEBUG_FUNC_OR_METHOD_DEFINEDIN_VISIBLE_KEY		"{definedin}"
#define	DEBUG_METHOD_SELF_VISIBLE_KEY					"{self}"
#define	DEBUG_EXTERNID_TYPE_VISIBLE_KEY					"{type}"
#define	DEBUG_EXTERNID_USERDATA_VISIBLE_KEY				"{userdata}"
#define	DEBUG_EXTERNID_MADEIN_VISIBLE_KEY				"{madein}"
#define	DEBUG_FUNC_OR_METHOD_CLOSURE_VISIBLE_KEY		"{closure}"

#define	DEBUG_TABLE_DERIVED_HIDDEN_KEY					"_derived_"
#define	DEBUG_TABLE_MOSTDERIVED_HIDDEN_KEY				"_mostderived_"
#define	DEBUG_TABLE_BASES_HIDDEN_KEY					"_bases_"
#define	DEBUG_TABLE_DELEGATES_HIDDEN_KEY				"_delegates_"
#define	DEBUG_TABLE_DELEGATORS_HIDDEN_KEY				"_delegators_"
#define	DEBUG_TABLE_MADEIN_HIDDEN_KEY					"_madein_"
#define	DEBUG_TABLE_OVERLOADING_ENABLED_HIDDEN_KEY		"_overloadingenabled_"
#define	DEBUG_TABLE_TOTAL_HIDDEN_KEY					"_length_"
#define	DEBUG_LIBFUNC_BOUNDARGS_HIDDEN_KEY				"_boundargs_"
#define	DEBUG_FUNC_OR_METHOD_VM_HIDDEN_KEY				"_vm_"
#define	DEBUG_METHOD_SELF_HIDDEN_KEY					"_self_"
#define	DEBUG_FUNC_OR_METHOD_DEFINEDIN_HIDDEN_KEY		"_definedin_"
#define	DEBUG_FUNC_OR_METHOD_CLOSURE_HIDDEN_KEY			"_closure_"
#define	DEBUG_EXTERNID_TYPE_HIDDEN_KEY					"{" DELTA_EXTERNID_TYPE_INDEX "}"
#define	DEBUG_EXTERNID_USERDATA_HIDDEN_KEY				"{" DELTA_EXTERNID_USERDATA_INDEX "}"

#define	DEBUG_DOT										"."
#define	DEBUG_DOT_CHAR									'.'
#define	DEBUG_DOUBLE_DOT								".."
#define	DEBUG_ATTR_SUFFIX_START							"@"
#define	DEBUG_ATTR_SUFFIX_START_CHAR					'@'
#define	DEBUG_CREATION_INTERNAL							"internal"

//////////////////////////////////////////////////////////////////

class DeltaValueToDebugWatchValueInfo {
	
	friend class DebugExprEvaluator;
	friend class DebugExprParseActions;

	private:
	typedef void (*ConvertFunc)(const DeltaValue& obj, DebugWatchValueInfo* at);
	typedef std::map<DeltaValueType, ConvertFunc> ConversionMap;
	static ConversionMap* converters;

	static const std::string ToString (const DeltaValue* obj);
	static const std::string ToString (const DeltaValue& obj);

	static void ConvertSimple (const DeltaValue& obj, DebugWatchValueInfo* at);		
	static void ConvertLibraryFunc (const DeltaValue& obj, DebugWatchValueInfo* at);		
	static void ConvertTable (const DeltaValue& obj, DebugWatchValueInfo* at);		
	static void ConvertProgramFunc (const DeltaValue& obj, DebugWatchValueInfo* at);	
	static void ConvertExternId (const DeltaValue& obj, DebugWatchValueInfo* at);	
	static void ConvertMethodFunc (const DeltaValue& obj, DebugWatchValueInfo* at);

	//******************************

	typedef void (*KeyConvertFunc)(const DeltaValue& obj, DebugWatchValueInfo::Content* at);
	typedef std::map<DeltaValueType, KeyConvertFunc> KeyConversionMap;
	static KeyConversionMap* keyConverters;

	static void ConvertTableToKey (const DeltaValue& obj, DebugWatchValueInfo::Content* at);
	static void ConvertMethodFuncToKey (const DeltaValue& obj, DebugWatchValueInfo::Content* at);
	static void ConvertProgramFuncToKey (const DeltaValue& obj, DebugWatchValueInfo::Content* at);
	static void ConvertExternIdToKey (const DeltaValue& obj, DebugWatchValueInfo::Content* at);
	static void ConvertLibraryFuncToKey (const DeltaValue& obj, DebugWatchValueInfo::Content* at);

	static void	AddStringKeys (
					const std::vector<std::string>&				keys, 
					std::list<DebugWatchValueInfo::Content>&	contents,
					bool										removeDotPrefix = false
				);

	//******************************

	static const std::string	Table_AbsoluteRef (const DeltaValue& obj);
	static const std::string	Table_AbsoluteRef (util_ui32 serialNo);

	static const std::string	LibraryFunc_AbsoluteRef (const DeltaValue& obj);
	static const std::string	LibraryFunc_AbsoluteRef (
									DeltaLibraryFunc f,
									const DeltaLibraryFuncArgsBinder* binder
								);

	static const std::string	ExternId_AbsoluteRef (const DeltaValue& obj);
	static const std::string	ExternId_AbsoluteRef (util_ui32 serialNo);

	static const std::string	MethodFunc_AbsoluteRef (
									DeltaCodeAddress		addr,
									DeltaTable*				table,
									DeltaVirtualMachine*	vm,
									const DeltaClosure*		closure
								);
	static const std::string	MethodFunc_AbsoluteRef (const DeltaValue& val);

	static const std::string	ProgramFunc_AbsoluteRef (
									DeltaCodeAddress		addr, 
									DeltaVirtualMachine*	vm, 
									const DeltaClosure*		closure
								);
	static const std::string	ProgramFunc_AbsoluteRef (const DeltaValue& val);

	typedef const std::string (*AbsRefFunc)(const DeltaValue& obj);
	typedef std::map<DeltaValueType, AbsRefFunc> AbsRefMap;
	static AbsRefMap* absRefFuncs;

	static const std::string	AbsoluteRef (const DeltaValue& obj);

	//////////////////////////////////////////////////////////////////

	public:
	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS
	static void Convert (const DeltaValue& obj, DebugWatchValueInfo* at);
};

//////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
