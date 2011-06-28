/**
 *	DeltaScriptProxy.h
 *
 *	-- IDE Delta Script Component Loader --
 *
 *	DeltaScriptProxy is used for loading and registring
 *	Delta script components into the system.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	July 2007
 */
#ifndef IDE_DELTA_SCRIPT_PROXY_COMPONENT_H_
#define IDE_DELTA_SCRIPT_PROXY_COMPONENT_H_

#pragma warning (disable:4251)

#include "Common.h"

#include "Component.h"
#include "ComponentUserCommandDesc.h"
#include "ComponentMemberCallback.h"
#include "ScriptInstanceProxy.h"

#include "UndefineAllWin32Shit.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

#include <map>

using namespace boost::multi_index;

class DeltaVirtualMachine;
class DeltaValue;

namespace ide
{
	class DeltaScriptProxy
	{
	public:
		///--- type definitions

		///--- public API
		static void Initialize(void);
		static void CleanUp(void);

		static bool LoadScriptComponent(const std::string& uri);
		static bool UnloadScriptComponent(const std::string& uri);

		static bool RegisterComponent(std::string* error, DeltaVirtualMachine* vm, const std::string& classId,
			const std::string& baseId = std::string());
		static bool UnregisterComponent(std::string* error, DeltaVirtualMachine* vm, const std::string& classId);

		///////////////////////////
		//--- Function registration
		//--- Class function registration
		static bool ClassImplementStaticFunction(std::string* error, DeltaVirtualMachine* vm, 
			const std::string& classId, const std::string& funcName, DeltaValue* func,
			const std::string& signature, const std::string& doc = std::string());
		static bool ClassImplementMemberFunction(std::string* error, DeltaVirtualMachine* vm,
			const std::string& classId,	const std::string& funcName, DeltaValue* func,
			const std::string& signature, const std::string& doc = std::string());
		static bool ClassDeclareRequiredMemberFunction(std::string* error, DeltaVirtualMachine* vm,
			const std::string& classId, const std::string& funcName, const std::string& signature, 
			const std::string& doc = std::string());

		static bool ClassRemoveStaticFunction(std::string* error, DeltaVirtualMachine* vm,
			const std::string& classId, const std::string& funcName);
		static bool ClassRemoveMemberFunction(std::string* error, DeltaVirtualMachine* vm,
			const std::string& classId, const std::string& funcName);

		//--- Instance function registration
		static bool InstanceImplementRequiredMemberFunction(std::string* error, DeltaVirtualMachine* vm,
			const std::string& classId, const std::string& funcName, DeltaValue* func);
		static bool InstanceImplementDynamicMemberFunction(std::string* error, DeltaVirtualMachine* vm, 
			const std::string& classId, const std::string& funcName, DeltaValue* func, 
			const std::string& signature, const std::string& doc = std::string());

		static bool InstanceRemoveDynamicMemberFunction(std::string* error, DeltaVirtualMachine* vm, const std::string& classId, const std::string& funcName);

		///////////////////////////
		//--- Handler registration
		//--- Class function registration
		static bool ClassImplementStaticHandler(std::string* error, DeltaVirtualMachine* vm,
			const std::string& classId, const std::string& signal,DeltaValue* func,
			const std::string& doc = std::string());
		static bool ClassImplementMemberHandler(std::string* error, DeltaVirtualMachine* vm,
			const std::string& classId, const std::string& signal,DeltaValue* func, 
			const std::string& doc = std::string());
		static bool ClassDeclareRequiredMemberHandler(std::string* error, DeltaVirtualMachine* vm,
			const std::string& classId, const std::string& signal, const std::string& doc = std::string());

		static bool ClassRemoveStaticHandler(std::string* error, DeltaVirtualMachine* vm,
			const std::string& classId, const std::string& signal);
		static bool ClassRemoveMemberHandler(std::string* error, DeltaVirtualMachine* vm,
			const std::string& classId, const std::string& signal);

		//--- Instance function registration
		static bool InstanceImplementRequiredMemberHandler(std::string* error, DeltaVirtualMachine* vm,
			const std::string& classId, const std::string& signal, DeltaValue* func);
		static bool InstanceImplementDynamicMemberHandler(std::string* error, DeltaVirtualMachine* vm,
			const std::string& classId, const std::string& signal, DeltaValue* func,
			const std::string& doc = std::string());

		static bool InstanceRemoveDynamicMemberHandler(std::string* error, DeltaVirtualMachine* vm, const std::string& classId, const std::string& signal);

		///////////////////////////
		//--- Command registration
		//--- Class function registration
		static bool ClassImplementStaticCommand(std::string* error, DeltaVirtualMachine* vm,
			const UserCommandDesc& cmd, const std::string& command, DeltaValue* func,
			const std::string& doc = std::string());
		static bool ClassImplementMemberCommand(std::string* error, DeltaVirtualMachine* vm,
			const UserCommandDesc& cmd, const std::string& command, DeltaValue* func,
			const std::string& doc = std::string());
		static bool ClassDeclareRequiredMemberCommand(std::string* error, DeltaVirtualMachine* vm,
			const UserCommandDesc& cmd, const std::string& command, const std::string& doc = std::string());

		static bool ClassRemoveStaticCommand(std::string* error, DeltaVirtualMachine* vm, 
			const std::string& classId, const std::string& command);
		static bool ClassRemoveMemberCommand(std::string* error, DeltaVirtualMachine* vm, 
			const std::string& classId, const std::string& command);

		//--- Instance function registration
		static bool InstanceImplementRequiredMemberCommand(std::string* error, DeltaVirtualMachine* vm,
			const std::string& classId, const std::string& funcName, DeltaValue* func);
		static bool InstanceImplementDynamicMemberCommand(std::string* error, DeltaVirtualMachine* vm, 
			const UserCommandDesc& cmd, const std::string& command, DeltaValue* func, 
			const std::string& doc = std::string());

		static bool InstanceRemoveDynamicMemberCommand(std::string* error, DeltaVirtualMachine* vm, const std::string& classId, const std::string& command);

		///////////////////////////
		//--- Signal
		static bool ClassDeclareStaticSignal(std::string* error, DeltaVirtualMachine* vm,
			const std::string& classId, const std::string& signal, const std::string& arglist);
		static bool ClassDeclareMemberSignal(std::string* error, DeltaVirtualMachine* vm,
			const std::string& classId, const std::string& signal, const std::string& arglist);

		///////////////////////////
		//--- Scipt instance funcs
		static ScriptInstanceProxy* GetScriptInstance(DeltaVirtualMachine* vm);
		static bool IsScriptInstance(Component* comp);

		static bool ComponentEntryCheck(const std::string& classId, std::string* error);
		static bool ClassVMCheck(DeltaVirtualMachine* vm, const std::string& classId, std::string* error);
		static bool ClassLoadedCheck(const std::string& classId, std::string* error);
		static bool InstanceVMCheck(DeltaVirtualMachine* vm, const std::string& classId, std::string* error);
		static bool CallableCheck(DeltaValue* function, std::string* error);
		static bool InstanceImplementsRequiredFunctionsCheck(DeltaVirtualMachine* vm, const std::string& classId, 
			std::string* error);

	private:
		///--- constructors / destructor
		DeltaScriptProxy(void) {}
		~DeltaScriptProxy(void) {}

		///--- private API
		static bool DispatchClassMemberFunction(Component* instance, EXPORTED_FUNCTION_ARGS);
		static bool DispatchRequiredMemberFunction(Component* instance, EXPORTED_FUNCTION_ARGS);
		static bool DispatchDynamicMemberFunction(Component* instance, EXPORTED_FUNCTION_ARGS);
		static bool InvokeMemberFunction(Component* instance, bool isClassMember, bool isDynamic, EXPORTED_FUNCTION_ARGS);
		static bool DispatchStaticFunction(EXPORTED_FUNCTION_ARGS);

		static const String DocumentationFunction(const String& doc, const ComponentFuncEntry& funcEntry);

		static const std::string GenerateInstanceVMId(const std::string& classId);
		static bool IsInstanceVMId(const std::string& vmId, const std::string& classId);

		template<typename T> static bool ClassImplementHandler(DeltaVirtualMachine* vm, const std::string& classId,
			DeltaValue* func, T dispatchFunc, const std::string& signal, const std::string& doc, std::string* error);
		template<typename T> static bool ClassDeclareFunction(DeltaVirtualMachine* vm, const std::string& classId,
			const std::string& funcName, T func, const std::string& signature, bool slot, const std::string& doc,
			std::string* error);
		template<typename T> static bool ClassImplementFunction(DeltaVirtualMachine* vm, const std::string& classId,
			const std::string& funcName, T func, DeltaValue* function, const std::string& signature, bool slot,
			const std::string& doc, std::string* error);
		static bool InstanceImplementFunction(DeltaVirtualMachine* vm, const std::string& classId,
			const std::string& funcName, DeltaValue* function, std::string* error);
		static bool ImplementDynamicFunction(DeltaVirtualMachine* vm, const std::string& classId, 
			const std::string& funcName, DeltaValue* function, const std::string& signature, bool slot,
			const std::string& doc, std::string* error);
		static bool RemoveDynamicFunction(DeltaVirtualMachine* vm, const std::string& classId,
			const std::string& funcName, std::string* error);
		static bool AddSignal(DeltaVirtualMachine* vm, const std::string& classId, const std::string& signal,
		const std::string& arglist, bool isStatic, std::string* error);

		static void ClassRemoveFunction(const std::string& classId, const std::string& function);
		static void ClassInstanceRemoveMemberFunctions(const std::string& classId, const std::string& function);

		template<typename T> static void AddComponentEntryFunc(const std::string& classId,
			const std::string& funcName, T func, const std::string& signature, bool slot, const std::string& doc);
		static void AddComponentEntryUserCommand(const std::string& command, const UserCommandDesc& cmd);

		static bool ExtractArguments(const ComponentFuncEntry& funcEntry, const Message& msg,
			std::list<DeltaValue>* deltaArgs, std::string* retType);
		static void SetReturnValue(const std::string& retType, DeltaValue* retval, Message& result);

		static Component* CreateScriptInstance(ComponentEntry& entry);
		static void DestroyScriptInstance(Component* comp);

		static const String GenerateSource(const std::string& classId);
		static const String GenerateBinary(const std::string& classId);

		static void HandleVMError(const std::string& classId);

		///--- multi-index structure of script instances
		struct by_Instance {};
		struct by_VM {};

		typedef multi_index_container<
			ScriptInstanceProxy*,
			indexed_by<
				ordered_unique<		///< sort by classId
					tag<by_Instance>, identity<ScriptInstanceProxy*>
				>,
				ordered_unique<		///< sort by uri
					tag<by_VM>, member<ScriptInstanceProxy, DeltaVirtualMachine*, &ScriptInstanceProxy::vm>
				>
			>
		> ScriptInstanceHolder;

		struct ClassVMInfo {
			DeltaVirtualMachine* vm;
			mutable uint guid;
			typedef std::map<std::string, DeltaValue> FunctionMapping;
			FunctionMapping functions;
			ClassVMInfo(DeltaVirtualMachine* vm = (DeltaVirtualMachine *) 0) : vm(vm), guid(0) {}
		};
		typedef std::map<std::string, ClassVMInfo> ClassVMMap;

		///--- private members
		static const std::string s_classId;
		static ScriptInstanceHolder scriptInstances;
		static ClassVMMap classVMs;
	};
}

#endif	//IDE_DELTA_SCRIPT_PROXY_COMPONENT_H_
