/**
 *	DeltaScriptProxy.cpp
 *
 *	-- IDE Delta Script Component Loader --
 *
 *	DeltaScriptProxy is used for loading and registring
 *	Delta script components into the system.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	July 2007
 */
#include "DeltaScriptProxy.h"

#include "Call.h"
#include "PropertyUtils.h"
#include "ComponentRegistry.h"
#include "ComponentLoader.h"
#include "ComponentFactory.h"
#include "ComponentSignalRegistry.h"
#include "ComponentFunctionCallerSafe.h"

#include "Streams.h"
#include "Adaptors.h"
#include "StringUtils.h"
#include "GenericDialogs.h"

#include "ConstructionAPI.h"
#include "TypeConverter.h"

#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaVMRegistry.h"
#include "DeltaStdLib.h"
#include "VMInit.h"
#include "uinit.h"
#include "uerrorclass.h"

#include "UndefineAllWin32Shit.h"

#include <windows.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/type_traits/is_same.hpp>

#include <algorithm>
#include <iterator>

namespace ide
{
	//-------------------------------------------------------//
	//---- static members -----------------------------------//

	const std::string DeltaScriptProxy::s_classId = "DeltaScriptProxy";
	DeltaScriptProxy::ScriptInstanceHolder DeltaScriptProxy::scriptInstances;
	DeltaScriptProxy::ClassVMMap DeltaScriptProxy::classVMs;

	//-------------------------------------------------------//
	//---- class DeltaScriptProxy --------------------------//

	void DeltaScriptProxy::Initialize(void)
	{
		ComponentLoader::Instance().RegisterLoaders(
			"DeltaScript",
			&DeltaScriptProxy::LoadScriptComponent,
			&DeltaScriptProxy::UnloadScriptComponent
		);
	}

	//-----------------------------------------------------------------------

	void DeltaScriptProxy::CleanUp(void)
	{
		ComponentLoader::Instance().UnregisterLoaders("DeltaScript");
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::LoadScriptComponent(const std::string& uri)	///< supplied uri contains source file
	{
		const size_t slash_index = uri.find_last_of('/') +1;
		const std::string classId = uri.substr(slash_index, uri.find_last_of('.') - slash_index);
		try {
			if (!Call<bool (const std::string&)>(s_classId, "DeltaComponentDirectory", "CheckByteCodeFileExistsAndUpTodate")(classId))
				return false;
		}
		catch(...) { return false; }
		const String id = Call<const String (const String&, const String&, const String&)>(s_classId, "DeltaVM", "LoadAndRun")
			(GenerateBinary(classId), GenerateSource(classId), util::std2str(classId));
		if (id.empty())
			return false;
		DeltaVirtualMachine* vm = VMRegistry().Get(classId.c_str());
		DASSERT(util::str2std(id) == classId && vm && classVMs.find(classId) == classVMs.end());
		classVMs[classId] = ClassVMInfo(vm);
		Call<bool (const String&, const String&)>(s_classId, "DeltaVM", "CallGlobalFunc")(id, _T("ClassLoad"));
		return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::UnloadScriptComponent(const std::string& uri)
	{
		const size_t slash_index = uri.find_last_of('/') +1;
		const std::string classId = uri.substr(slash_index, uri.find_last_of('.') - slash_index);

		ClassVMMap::iterator i = classVMs.find(classId);
		assert(i != classVMs.end());
		const String vmId = util::std2str(DPTR(i->second.vm)->Id());
		if (Call<bool (const String&, const String&)>(s_classId, "DeltaVM", "CallGlobalFunc")(vmId, _T("ClassUnload")))
			Call<bool (const String&)>(s_classId, "DeltaVM", "DeleteVM")(vmId);
		classVMs.erase(i);
		return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::RegisterComponent(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& baseId)
	{
		if (!ClassLoadedCheck(classId, error) || !ClassVMCheck(vm, classId, error))
			return false;
		else if (ComponentRegistry::Instance().HasComponentEntry(classId)) {
			if (error)
				*error = "Component entry for '" + classId + "' already exists";
			return false;
		}
		else {
			if (!baseId.empty() && !ComponentRegistry::Instance().GetComponentEntry(baseId))
				ComponentLoader::Instance().LoadComponent(baseId);

			//-- Register the component entry
			ComponentRegistry::Instance().Register(
				ComponentEntry(
					classId,
					&DeltaScriptProxy::CreateScriptInstance,
					&DeltaScriptProxy::DestroyScriptInstance,
					(ComponentEntry::SetPropertiesFunc) 0,	//Delta scripts invoke their SetProperties manually
					baseId
				)
			);
			return true;
		}
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::UnregisterComponent(std::string* error, DeltaVirtualMachine* vm, const std::string& classId)
	{
		if (!ClassLoadedCheck(classId, error) || !ClassVMCheck(vm, classId, error))
			return false;
		ComponentRegistry::Instance().Unregister(classId);
		return true;
	}

	/////////////////////////////////////////////////////////////////////////

	bool DeltaScriptProxy::ClassImplementStaticFunction(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& funcName, DeltaValue* func, const std::string& signature,
		const std::string& doc)
	{
		return ClassImplementFunction<ComponentStaticCallback>
			(vm, classId, funcName, &DeltaScriptProxy::DispatchStaticFunction, func, signature, false, doc, error);
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::ClassImplementMemberFunction(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& funcName, DeltaValue* func, const std::string& signature,
		const std::string& doc)
	{
		return ClassImplementFunction<ComponentMemberCallback>
			(vm, classId, funcName, &DeltaScriptProxy::DispatchClassMemberFunction, func, signature, false, doc, error);
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::ClassDeclareRequiredMemberFunction(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& funcName, const std::string& signature, const std::string& doc)
	{
		return ClassDeclareFunction<ComponentMemberCallback>
			(vm, classId, funcName, &DeltaScriptProxy::DispatchRequiredMemberFunction, signature, false, doc, error);
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::ClassRemoveStaticFunction(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& funcName)
	{
		if (!ClassLoadedCheck(classId, error)		||
			!ComponentEntryCheck(classId, error)	||
			!ClassVMCheck(vm, classId, error)
		)
			return false;
		ClassRemoveFunction(classId, funcName);
		return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::ClassRemoveMemberFunction(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& funcName)
	{
		if (!ClassLoadedCheck(classId, error)		||
			!ComponentEntryCheck(classId, error)	||
			!ClassVMCheck(vm, classId, error)
		)
			return false;
		ClassRemoveFunction(classId, funcName);
		ClassInstanceRemoveMemberFunctions(classId, funcName);
		return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::InstanceImplementRequiredMemberFunction(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& funcName, DeltaValue* func)
	{
		return InstanceImplementFunction(vm, classId, funcName, func, error);
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::InstanceImplementDynamicMemberFunction(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& funcName, DeltaValue* func, const std::string& signature, 
		const std::string& doc)
	{
		return ImplementDynamicFunction(vm, classId, funcName, func, signature, false, doc, error);
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::InstanceRemoveDynamicMemberFunction(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& funcName)
	{
		return RemoveDynamicFunction(vm, classId, funcName, error);
	}

	/////////////////////////////////////////////////////////////////////////

	bool DeltaScriptProxy::ClassImplementStaticHandler(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& signal, DeltaValue* func, const std::string& doc)
	{
		return ClassImplementHandler<ComponentStaticCallback>
			(vm, classId, func, &DeltaScriptProxy::DispatchStaticFunction, signal, doc, error);
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::ClassImplementMemberHandler(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& signal, DeltaValue* func, const std::string& doc)
	{	
		return ClassImplementHandler<ComponentMemberCallback>
			(vm, classId, func, &DeltaScriptProxy::DispatchClassMemberFunction, signal, doc, error);
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::ClassDeclareRequiredMemberHandler(std::string* error, DeltaVirtualMachine* vm, 
		const std::string& classId, const std::string& signal, const std::string& doc)
	{
		const std::string funcName = signal;	//Single registration, so no need for unique id
		if (!ClassDeclareFunction<ComponentMemberCallback>(
				vm, classId, funcName,
				&DeltaScriptProxy::DispatchRequiredMemberFunction,
				ComponentSignalRegistry::Instance().GetSignal(signal).GetSignature(), true,
				doc, error
			)
		)
			return false;
		ComponentSignalRegistry::Instance().ConnectSlot(signal, ComponentSlot(classId, funcName), false);
		return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::ClassRemoveStaticHandler(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& signal)
	{
		if (!ClassLoadedCheck(classId, error)		||
			!ComponentEntryCheck(classId, error)	||
			!ClassVMCheck(vm, classId, error)
		)
			return false;
		ComponentSignalRegistry::Instance().DisconnectSlot(signal, classId);
		ClassRemoveFunction(classId, signal);
		return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::ClassRemoveMemberHandler(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& signal)
	{
		if (!ClassLoadedCheck(classId, error)		||
			!ComponentEntryCheck(classId, error)	||
			!ClassVMCheck(vm, classId, error)
		)
			return false;
		ComponentSignalRegistry::Instance().DisconnectSlot(signal, classId);
		ClassRemoveFunction(classId, signal);
		ClassInstanceRemoveMemberFunctions(classId, signal);
		return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::InstanceImplementRequiredMemberHandler(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& signal, DeltaValue* func)
	{
		const std::string funcName = signal;	//Single registration, so no need for unique id
		return InstanceImplementFunction(vm, classId, funcName, func, error);
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::InstanceImplementDynamicMemberHandler(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& signal, DeltaValue* func, const std::string& doc)
	{
		const std::string funcName = signal;	//Single registration, so no need for unique id
		if (!ImplementDynamicFunction(
				vm, classId, funcName, func,
				ComponentSignalRegistry::Instance().GetSignal(signal).GetSignature(), true,
				doc, error
			)
		)
			return false;
		ComponentSignalRegistry::Instance().ConnectDynamicSlot(
			signal, DynamicComponentSlot(Handle(GetScriptInstance(vm)), funcName)
		);
		return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::InstanceRemoveDynamicMemberHandler(std::string* error, DeltaVirtualMachine* vm, const std::string& classId, const std::string& signal)
	{
		if (!RemoveDynamicFunction(vm, classId, signal, error))
			return false;
		ComponentSignalRegistry::Instance().DisconnectDynamicSlot(signal, Handle(GetScriptInstance(vm)));
		return true;
	}

	/////////////////////////////////////////////////////////////////////////

	bool DeltaScriptProxy::ClassImplementStaticCommand(std::string* error, DeltaVirtualMachine* vm,
		const UserCommandDesc& cmd, const std::string& command, DeltaValue* func, const std::string& doc)
	{
		const std::string& classId = cmd.GetCallback().first;
		const std::string& funcName = cmd.GetCallback().second;
		if (!ClassImplementFunction<ComponentStaticCallback>
				(vm, classId, funcName, &DeltaScriptProxy::DispatchStaticFunction, func, "void (void)", false, doc, error)
		)
			return false;
		AddComponentEntryUserCommand(command, cmd);
		return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::ClassImplementMemberCommand(std::string* error, DeltaVirtualMachine* vm,
		const UserCommandDesc& cmd, const std::string& command, DeltaValue* func, const std::string& doc)
	{
		const std::string& classId = cmd.GetCallback().first;
		const std::string& funcName = cmd.GetCallback().second;
		if (!ClassImplementFunction<ComponentMemberCallback>
				(vm, classId, funcName, &DeltaScriptProxy::DispatchClassMemberFunction, func, "void (void)", false, doc, error)
		)
			return false;
		AddComponentEntryUserCommand(command, cmd);
		return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::ClassDeclareRequiredMemberCommand(std::string* error, DeltaVirtualMachine* vm,
		const UserCommandDesc& cmd, const std::string& command, const std::string& doc)
	{
		const std::string& classId = cmd.GetCallback().first;
		const std::string& funcName = cmd.GetCallback().second;
		if (!ClassDeclareFunction<ComponentMemberCallback>
			(vm, classId, funcName, &DeltaScriptProxy::DispatchRequiredMemberFunction, "void (void)", false, doc, error)
		)
			return false;
		AddComponentEntryUserCommand(command, cmd);
		return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::ClassRemoveStaticCommand(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& command)
	{
		if (!ClassLoadedCheck(classId, error)		||
			!ComponentEntryCheck(classId, error)	||
			!ClassVMCheck(vm, classId, error)
		)
			return false;
		ComponentEntry& compEntry = ComponentRegistry::Instance().GetComponentEntry(classId);
		String cmd = util::std2str(command);
		const std::string funcName = compEntry.GetUserCommand(cmd).GetAction().GetFunction();
		ClassRemoveFunction(classId, funcName);
		compEntry.RemoveUserCommand(cmd);
		return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::ClassRemoveMemberCommand(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& command)
	{
		if (!ClassLoadedCheck(classId, error)		||
			!ComponentEntryCheck(classId, error)	||
			!ClassVMCheck(vm, classId, error)
		)
			return false;
		ComponentEntry& compEntry = ComponentRegistry::Instance().GetComponentEntry(classId);
		String cmd = util::std2str(command);
		const std::string funcName = compEntry.GetUserCommand(cmd).GetAction().GetFunction();
		ClassRemoveFunction(classId, funcName);
		ClassInstanceRemoveMemberFunctions(classId, funcName);
		compEntry.RemoveUserCommand(cmd);
		return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::InstanceImplementRequiredMemberCommand(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& funcName, DeltaValue* func)
	{
		return InstanceImplementFunction(vm, classId, funcName, func, error);
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::InstanceImplementDynamicMemberCommand(std::string* error, DeltaVirtualMachine* vm,
		const UserCommandDesc& cmd, const std::string& command, DeltaValue* func, const std::string& doc)
	{
		const std::string& classId = cmd.GetCallback().first;
		const std::string& funcName = cmd.GetCallback().second;
		if (!ImplementDynamicFunction(vm, classId, funcName, func, "void (void)", false, doc, error))
			return false;
		//TODO add cmd to dynamic component entry
		if (error)
			*error = "Dynamic commands are not implemented yet";
		return false;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::InstanceRemoveDynamicMemberCommand(std::string* error, DeltaVirtualMachine* vm, const std::string& classId, const std::string& command)
	{
		const std::string funcName;// = compEntry.GetUserCommand(cmd).GetAction().GetFunction();
		if (!RemoveDynamicFunction(vm, classId, funcName, error))
			return false;
		//TODO remove cmd from dynamic component entry
		if (error)
			*error = "Dynamic commands are not implemented yet";
		return false;
	}

	/////////////////////////////////////////////////////////////////////////

	bool DeltaScriptProxy::ClassDeclareStaticSignal(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& signal, const std::string& arglist)
	{
		return AddSignal(vm, classId, signal, arglist, true, error);
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::ClassDeclareMemberSignal(std::string* error, DeltaVirtualMachine* vm,
		const std::string& classId, const std::string& signal, const std::string& arglist)
	{
		return AddSignal(vm, classId, signal, arglist, false, error);
	}

	/////////////////////////////////////////////////////////////////////////

	ScriptInstanceProxy* DeltaScriptProxy::GetScriptInstance(DeltaVirtualMachine* vm)
	{
		ScriptInstanceHolder::index<by_VM>::type::iterator it
			= scriptInstances.get<by_VM>().find(vm);
		return it == scriptInstances.get<by_VM>().end() ? (ScriptInstanceProxy *) 0 : *it;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::IsScriptInstance(Component* comp)
	{
		ScriptInstanceHolder::index<by_Instance>::type::iterator it
			= scriptInstances.get<by_Instance>().find(static_cast<ScriptInstanceProxy *>(comp));
		return it != scriptInstances.get<by_Instance>().end();
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::ComponentEntryCheck(const std::string& classId, std::string* error)
	{
		if (!ComponentRegistry::Instance().HasComponentEntry(classId)) {
			if (error)
				*error = "No component entry '" + classId + "'";
			return false;
		}
		else
			return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::ClassVMCheck(DeltaVirtualMachine* vm, const std::string& classId, std::string* error)
	{
		if (DPTR(vm)->Id() != classId) {
			if (error)
				*error = "'" + std::string(DPTR(vm)->Id()) + "' is not a valid class VM";
			return false;
		}
		else
			return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::ClassLoadedCheck(const std::string& classId, std::string* error)
	{
		ClassVMMap::iterator i = classVMs.find(classId);
		if (i == classVMs.end()) {
			if (error)
				*error = "Class '" + classId + "' is not loaded";
			return false;
		}
		else
			return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::InstanceVMCheck(DeltaVirtualMachine* vm, const std::string& classId, std::string* error)
	{
		const std::string vmId = DPTR(vm)->Id();
		if (!IsInstanceVMId(vmId, classId)){
			if (error)
				*error = "'" + vmId + "' is not a " + classId + " instance VM";
			return false;
		}
		else
			return true;

	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::CallableCheck(DeltaValue* function, std::string* error)
	{
		if (!function->IsCallable()) {
			if (error)
				*error = std::string("Callable value expected as function (") + function->TypeStr() + " given)";
			return false;
		}
		else
			return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::InstanceImplementsRequiredFunctionsCheck(DeltaVirtualMachine* vm,
		const std::string& classId, std::string* error)
	{
		if (!ClassLoadedCheck(classId, error) || !InstanceVMCheck(vm, classId, error))
			return false;

		ScriptInstanceProxy* instance = GetScriptInstance(vm);
		const ComponentEntry::ComponentFuncMap& funcMap =
			ComponentRegistry::Instance().GetComponentEntry(classId).GetFunctionMap();
		
		StdStringList unimplemented;
		BOOST_FOREACH(const ComponentFuncEntry& func, funcMap)
			if (func.IsMemberFunc() && instance->functions.find(func.GetId()) == instance->functions.end())
				unimplemented.push_back(func.GetId());

		//Check for non-required member functions and provide the class implementation for them
		for(StdStringList::iterator i = unimplemented.begin(); i != unimplemented.end(); /*empty*/) {
			const ClassVMInfo::FunctionMapping& functions = classVMs[classId].functions;
			ClassVMInfo::FunctionMapping::const_iterator iter = functions.find(*i);
			if (iter != functions.end()) {
				DASSERT(iter->second.IsCallable());
				instance->functions[*i] = iter->second;
				i = unimplemented.erase(i);
			}
			else
				++i;
		}

		//Any remaining unimplemented functions are required so post an error
		if (!unimplemented.empty()) {
			if (error) {
				std::string ids;
				for(StdStringList::const_iterator i = unimplemented.begin(); i != unimplemented.end(); ++i) {
					if (i != unimplemented.begin())
						ids += ", ";
					ids += *i;
				}
				*error = "Script instance does not implement the following required class members: " + ids;
			}
			return false;
		}
		else
			return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::DispatchRequiredMemberFunction(Component* instance, EXPORTED_FUNCTION_ARGS)
	{
		return InvokeMemberFunction(instance, false, false, EXPORTED_FUNCTION_ARG_NAMES);
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::DispatchClassMemberFunction(Component* instance, EXPORTED_FUNCTION_ARGS)
	{
		return InvokeMemberFunction(instance, true, false, EXPORTED_FUNCTION_ARG_NAMES);
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::DispatchDynamicMemberFunction(Component* instance, EXPORTED_FUNCTION_ARGS)
	{
		return InvokeMemberFunction(instance, false, true, EXPORTED_FUNCTION_ARG_NAMES);
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::InvokeMemberFunction(Component* instance, bool isClassMember, bool isDynamic, EXPORTED_FUNCTION_ARGS)
	{
		//-- construct result message
		result.SetSrc(msg.GetDst());
		result.SetDst(msg.GetSrc());

		//-- Retrieve Delta function
		ScriptInstanceHolder::index<by_Instance>::type::iterator it
			= scriptInstances.get<by_Instance>().find(static_cast<ScriptInstanceProxy*>(instance));
		if (it == scriptInstances.end())
			return false;

		ComponentEntry& compEntry = ComponentRegistry::Instance().GetComponentEntry(instance->GetClassId());
		const std::string funcName = msg.GetDst().function;
		ScriptInstanceProxy::FunctionMapping::iterator fit;
		const ComponentFuncEntry* funcEntry = (const ComponentFuncEntry*) 0;

		if (isDynamic) {
			const ComponentFuncEntry& entry = instance->GetDynamicFuncEntry(funcName);
			if (!entry || (fit = (*it)->dynamicFunctions.find(funcName)) == (*it)->dynamicFunctions.end())
				return false;	//Dynamic functions do not check inheritance
			funcEntry = &entry;
		}
		else {
			if ((fit = (*it)->functions.find(funcName)) == (*it)->functions.end()) {
				bool status = false;
				if ((*it)->base) {	//-- Check if there is a base class with this function
					const ComponentEntry& compEntry = ComponentRegistry::Instance().GetComponentEntry((*it)->base->GetClassId());
					const ComponentFuncEntry& funcEntry = compEntry.GetFuncEntry(funcName);
					if (funcEntry.IsMemberFunc())
						try {
							ComponentFunctionCaller(msg.GetSrc().classId, (*it)->base, funcName).
								Invoke(result, msg.GetData());
							status = true;
						} catch (std::exception&) {}
				}
				return status;
			}
			else
				funcEntry = &compEntry.GetFuncEntry(funcName);
		}
		DeltaValue& function = fit->second;

		//-- Perform Delta function call
		std::list<DeltaValue> deltaArgs;
		std::string retType;
		
		if (!ExtractArguments(*funcEntry, msg, &deltaArgs, &retType))
			return false;

		if (isClassMember) {
			DeltaValue obj;
			to_delta<Handle>().convert(&obj, Handle(instance));
			deltaArgs.push_back(obj);
		}

		DeltaValue retval;
		if (!function.IsCallable())
			return false;

		uint savedSerial = instance->GetSerial();
		function(deltaArgs, &retval);
		if(!ComponentRegistry::Instance().IsValidInstance(instance, savedSerial))
			return true;

		if (DPTR((*it)->vm)->HasProducedError() && DELTA_NO_VM_CALLER_FAILED)
			DeltaVirtualMachine::ResetRunTimeErrors();

		if (UERROR_ISRAISED()) {
			HandleVMError(instance->GetClassId());
			ComponentFactory::Instance().DestroyComponent(*it);
			return true;
		}

		SetReturnValue(retType, &retval, result);
		return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::DispatchStaticFunction(EXPORTED_FUNCTION_ARGS)
	{
		//-- construct result message
		result.SetSrc(msg.GetDst());
		result.SetDst(msg.GetSrc());

		//-- Retrieve Delta function
		const std::string classId = msg.GetDst().classId;
		const std::string funcName = msg.GetDst().function;
		if (!ClassLoadedCheck(classId, 0))
			return false;
		
		ClassVMInfo::FunctionMapping& functions = classVMs[classId].functions;
		ClassVMInfo::FunctionMapping::iterator i = functions.find(funcName);
		if (i == functions.end())	//Cannot invoke base class static functions this way
			return false;
		DeltaValue& function = i->second;
		
		//-- Perform Delta function call
		std::list<DeltaValue> deltaArgs;
		std::string retType;
		ComponentEntry& compEntry = ComponentRegistry::Instance().GetComponentEntry(classId);
		if (!ExtractArguments(compEntry.GetFuncEntry(funcName), msg, &deltaArgs, &retType))
			return false;

		DeltaValue retval;
		if (!function.IsCallable())
			return false;

		function(deltaArgs, &retval);

		if (DPTR(classVMs[classId].vm)->HasProducedError() && DELTA_NO_VM_CALLER_FAILED)
			DeltaVirtualMachine::ResetRunTimeErrors();

		if (UERROR_ISRAISED()) {
			HandleVMError(classId);
			//TODO: unload script class (and kill all instances) or just ignore the error?
			return true;
		}

		SetReturnValue(retType, &retval, result);
		return true;
	}

	//-----------------------------------------------------------------------

	const String DeltaScriptProxy::DocumentationFunction(const String& doc, const ComponentFuncEntry&)
	{
		return doc;
	}

	//-----------------------------------------------------------------------

	const std::string DeltaScriptProxy::GenerateInstanceVMId(const std::string& classId)
	{
		ClassVMMap::const_iterator i = classVMs.find(classId);
		DASSERT(i != classVMs.end());
		const uint id = ++i->second.guid;
		return classId + "::instance::" + boost::lexical_cast<std::string>(id);
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::IsInstanceVMId(const std::string& vmId, const std::string& classId)
	{
		return util::std2str(vmId).StartsWith(util::std2str(classId + "::instance::"));
	}

	//-----------------------------------------------------------------------

	template<typename T>
	bool DeltaScriptProxy::ClassImplementHandler(DeltaVirtualMachine* vm, const std::string& classId,
		DeltaValue* func, T dispatchFunc, const std::string& signal, const std::string& doc, std::string* error)
	{
		const std::string signature = ComponentSignalRegistry::Instance().GetSignal(signal).GetSignature();
		const std::string funcName = signal;	//Single registration, so no need for unique id
		if (!ClassImplementFunction(vm, classId, funcName, dispatchFunc, func, signature, true, doc, error))
			return false;
		ComponentSignalRegistry::Instance().ConnectSlot(
			signal,
			ComponentSlot(classId, funcName),
			boost::is_same<T, ComponentStaticCallback>::value
		);
		return true;
	}

	//-----------------------------------------------------------------------

	template<typename T>
	bool DeltaScriptProxy::ClassDeclareFunction(DeltaVirtualMachine* vm, const std::string& classId,
		const std::string& funcName, T func, const std::string& signature, bool slot, const std::string& doc, std::string* error)
	{
		if (!ClassLoadedCheck(classId, error)		||
			!ComponentEntryCheck(classId, error)	||
			!ClassVMCheck(vm, classId, error)
		)
			return false;
		else if (ComponentRegistry::Instance().GetComponentEntry(classId).HasFunction(funcName, false)) {
			if (error)
				*error = "Duplicate registration of function '" + funcName + "' not allowed";
			return false;
		}
		AddComponentEntryFunc(classId, funcName, func, signature, slot, doc);
		return true;
	}

	//-----------------------------------------------------------------------

	template<typename T>
	bool DeltaScriptProxy::ClassImplementFunction(DeltaVirtualMachine* vm, const std::string& classId, const std::string& funcName,
		T func, DeltaValue* function, const std::string& signature, bool slot, const std::string& doc, std::string* error)
	{
		if (!ClassDeclareFunction(vm, classId, funcName, func, signature, slot, doc, error) ||
			!CallableCheck(function, error)
		)
			return false;
		ClassVMInfo::FunctionMapping& functions = classVMs[classId].functions;
		DASSERT(functions.find(funcName) == functions.end());
		functions[funcName] = *function;
		return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::InstanceImplementFunction(DeltaVirtualMachine* vm, const std::string& classId, 
		const std::string& funcName, DeltaValue* function, std::string* error)
	{
		if (!ClassLoadedCheck(classId, error)		||
			!InstanceVMCheck(vm, classId, error)	||
			!CallableCheck(function, error)
		)
			return false;
		else if (!ComponentRegistry::Instance().GetComponentEntry(classId).HasFunction(funcName, false)) {
			if (error)
				*error = "'" + funcName + "' is not a declared " + classId + " function";
			return false;
		}

		GetScriptInstance(vm)->functions[funcName] = *function;
		return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::ImplementDynamicFunction(DeltaVirtualMachine* vm, const std::string& classId, 
		const std::string& funcName, DeltaValue* function, const std::string& signature, bool slot,
		const std::string& doc, std::string* error)
	{
		if (!ClassLoadedCheck(classId, error)		||
			!InstanceVMCheck(vm, classId, error)	||
			!CallableCheck(function, error)
		)
			return false;

		const size_t spaceIndex = signature.find(" ");
		const size_t argIndex = signature.find("(");
		std::string ret_type = signature.substr(0, min(spaceIndex, argIndex));
		const std::string arglist = signature.substr(argIndex);
		boost::algorithm::trim(ret_type);
		DASSERT(!slot || ret_type == "void");
		ScriptInstanceProxy* instance = GetScriptInstance(vm);
		instance->AddDynamicFunction(
			ComponentFuncEntry(
				(ComponentMemberCallback) &DeltaScriptProxy::DispatchDynamicMemberFunction,
				ret_type, funcName, arglist,
				boost::bind(&DeltaScriptProxy::DocumentationFunction, util::std2str(doc), _1), slot
			)
		);
		instance->dynamicFunctions[funcName] = *function;
		return true;
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::RemoveDynamicFunction(DeltaVirtualMachine* vm, const std::string& classId, const std::string& funcName, std::string* error)
	{
		if (!ClassLoadedCheck(classId, error) || !InstanceVMCheck(vm, classId, error))
			return false;
		ScriptInstanceProxy* instance = GetScriptInstance(vm);
		instance->RemoveDynamicFunction(funcName);
		instance->dynamicFunctions.erase(funcName);
		return true;
	}

	bool DeltaScriptProxy::AddSignal(DeltaVirtualMachine* vm, const std::string& classId, const std::string& signal,
		const std::string& arglist, bool isStatic, std::string* error)
	{
		if (!ClassLoadedCheck(classId, error) || !ClassVMCheck(vm, classId, error))
			return false;
		else if (ComponentRegistry::Instance().GetComponentEntry(classId).HasSignal(signal)) {
			if (error)
				*error = "Duplicate registration of signal '" + signal + "' not allowed";
			return false;
		}
		ComponentSignalRegistry::Instance().AddSignal(signal, arglist, isStatic);
		ComponentRegistry::Instance().GetComponentEntry(classId).AddSignal(signal);
		return true;
	}

	//-----------------------------------------------------------------------

	void DeltaScriptProxy::ClassRemoveFunction(const std::string& classId, const std::string& function)
	{
		ComponentEntry& compEntry = ComponentRegistry::Instance().GetComponentEntry(classId);
		compEntry.RemoveFunction(function);
		classVMs[classId].functions.erase(function);
	}

	//-----------------------------------------------------------------------

	void DeltaScriptProxy::ClassInstanceRemoveMemberFunctions(const std::string& classId, const std::string& function)
	{
		ComponentEntry& compEntry = ComponentRegistry::Instance().GetComponentEntry(classId);
		BOOST_FOREACH(Component* comp, compEntry.GetInstances()) {
			DASSERT(IsScriptInstance(comp));
			static_cast<ScriptInstanceProxy*>(comp)->functions.erase(function);
		}
	}

	//-----------------------------------------------------------------------

	template<typename T>
	void DeltaScriptProxy::AddComponentEntryFunc(const std::string& classId, const std::string& funcName, T func,
		const std::string& signature, bool slot, const std::string& doc)
	{
		const size_t spaceIndex = signature.find(" ");
		const size_t argIndex = signature.find("(");
		std::string ret_type = signature.substr(0, min(spaceIndex, argIndex));
		const std::string arglist = signature.substr(argIndex);
		boost::algorithm::trim(ret_type);
		DASSERT(!slot || ret_type == "void");
		ComponentRegistry::Instance().GetComponentEntry(classId).AddFunction(
			ComponentFuncEntry(
				func, ret_type, funcName, arglist,
				boost::bind(&DeltaScriptProxy::DocumentationFunction, util::std2str(doc), _1), slot
			)
		);
	}

	//-----------------------------------------------------------------------

	void DeltaScriptProxy::AddComponentEntryUserCommand(const std::string& command, const UserCommandDesc& cmd)
	{
		const std::string& classId = cmd.GetCallback().first;
		ComponentRegistry::Instance().GetComponentEntry(classId).AddUserCommand(
			util::std2str(command),
			UserCommand(
				UserCommand::Callback("User", classId, cmd.GetCallback().second),
				UserCommand::UCT_NORMAL, cmd.GetFlags(), cmd.IsStatic(), cmd.IsEnabled(), cmd.GetImage()
			)
		);
	}

	//-----------------------------------------------------------------------

	bool DeltaScriptProxy::ExtractArguments(const ComponentFuncEntry& funcEntry, const Message& msg,
		std::list<DeltaValue>* deltaArgs, std::string* retType)
	{
		const ArgumentVec& formalArgs = funcEntry.GetArgumentVec();
		const Buffer& args = msg.GetData();
		if (deltaArgs && !args.empty())
		{
			comm::decoder decoder(args);
			try {
				BOOST_FOREACH(const ArgumentVec::value_type& formalArg, formalArgs)
				{
					DeltaValue obj;
					TypeConverter::DecodeDeltaValue(decoder, &obj, formalArg.first);
					deltaArgs->push_front(obj);
				}
			}
			catch (std::exception&) { return false; }
		}
		if(retType)
			*retType = funcEntry.GetRetType();
		return true;
	}

	//-----------------------------------------------------------------------

	void DeltaScriptProxy::SetReturnValue(const std::string& retType, DeltaValue* retval, Message& result)
	{
		if (retval->Type() != DeltaValue_Undefined)
		{
			comm::encoder encoder;
			TypeConverter::EncodeDeltaValue(encoder, retval, retType);
			result.SetData(encoder);
		}
	}

	//-----------------------------------------------------------------------

	Component* DeltaScriptProxy::CreateScriptInstance(ComponentEntry& entry)
	{
		const std::string& classId = entry.GetClassId();
		const std::string vmId = GenerateInstanceVMId(classId);
		const String id = Call<const String (const String&, const String&, const String&)>(s_classId, "DeltaVM", "LoadAndRun")
			(GenerateBinary(classId), GenerateSource(classId), util::std2str(vmId));
		if (id.empty())
			return 0;
		DeltaVirtualMachine* vm = VMRegistry().Get(vmId.c_str());
		DASSERT(util::str2std(id) == vmId && vm);

		Component* base = 0;
		if (!entry.GetBaseClassId().empty() && 
			!(base = ComponentFactory::Instance().CreateComponent(entry.GetBaseClassId()))
		) {
			Call<void (const String&), SafeCall>(entry.GetClassId(), "Output", "Append")(
				_("unable to create base class '") + util::std2str(entry.GetBaseClassId()) + _T("'\n"));
			Call<bool (const String&)>(s_classId, "DeltaVM", "DeleteVM")(id);
			return 0;
		}

		//-- Create new Component
		ScriptInstanceProxy* script = new ScriptInstanceProxy(entry.GetClassId(), vm, base);
		scriptInstances.get<by_Instance>().insert(script);

		// Do NOT call the script instance initialization here, as the component doesn't have a handle
		// yet and therefore can't invoke any base class functions. The initialization function is called
		// after the component gets its handle throught the ComponentCreated notification.

		return script;
	}

	//-----------------------------------------------------------------------

	void DeltaScriptProxy::DestroyScriptInstance(Component* instance)
	{
		ScriptInstanceHolder::index<by_Instance>::type::iterator it
			= scriptInstances.get<by_Instance>().find(static_cast<ScriptInstanceProxy*>(instance));

		if (it == scriptInstances.get<by_Instance>().end())
			return;

		//-- Call script instance cleanup
		const char * destructorFunc = "Destructor";
		DeltaVirtualMachine*& vm = (*it)->vm;
		if (!DPTR(vm)->HasProducedError() && DPTR(vm)->GlobalFuncExists(destructorFunc))
			DELTA_VM_CALL_AND_RESET_ERRORS(vm, DPTR(vm)->ExtCallGlobalFunc(destructorFunc), DELTA_NO_VM_CALLER_FAILED);

		//-- Get error condition
		if (UERROR_ISRAISED())
			HandleVMError(instance->GetClassId());

		delete *it;
		scriptInstances.get<by_Instance>().erase(it);
	}

	//-----------------------------------------------------------------------

	const String DeltaScriptProxy::GenerateSource(const std::string& classId)
	{
		const String extensionDir = Call<const String (void)>(s_classId, "DeltaVM", "GetExtensionScriptDirectory")();
		return util::std2str(util::str2std(extensionDir) + classId + ".dsc");
	}

	//-----------------------------------------------------------------------

	const String DeltaScriptProxy::GenerateBinary(const std::string& classId)
	{
		const String extensionDir = Call<const String (void)>(s_classId, "DeltaVM", "GetExtensionScriptDirectory")();
		return util::std2str(util::str2std(extensionDir) + classId + ".dbc");
	}

	//-----------------------------------------------------------------------

	void DeltaScriptProxy::HandleVMError(const std::string& classId) {
		const String error = util::std2str(UERROR_GETREPORT());
		UERROR_CLEAR();
		Call<void (const String&, const String&)>(s_classId, "DeltaVM", "RecordFailedConfigAndExtensionScripts")(
			GenerateSource(classId),
			error
		);
		Call<bool (void)>(s_classId, "Shell", "HandleInternalSparrowError")();
	}

	//-----------------------------------------------------------------------
}