/**
 *	ControlAPI.cpp
 *
 *	-- Sparrow library for Delta --
 *
 *	Exported Component Control API for Sparrow.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 *	Heavily refactored January 2009 by JL (Lilis).
 */
#include "ControlAPI.h"
#include "TypeConverter.h"
#include "DeltaScriptProxy.h"

#include "Component.h"
#include "ComponentRegistry.h"
#include "ComponentFactory.h"
#include "ComponentFunctionCaller.h"
#include "ComponentSignalRegistry.h"
#include "ComponentSignalEmitter.h"
#include "ComponentUndoRegistrar.h"
#include "ComponentHandle.h"
#include "Call.h"
#include "ComponentFunctionCallerSafe.h"

#include "DeltaByteCodeTypes.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaObject.h"
#include "DeltaStdLibTemplates.h"

#include "SparrowLibFuncCommon.h"

#include "Adaptors.h"
#include "Streams.h"

#include "SimpleTimer.h"

#include <boost/foreach.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <vector>

namespace ide
{
	//-------------------------------------------------------//
	//---- Library Function Functor classes -----------------//

	//-----------------------------------------------------------------------
	// string spw_installationdir().
	//
	class InstallationDirFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 0; }
		const char* ArgCountError	(void) const			{ return "No arguments expected"; }
		const char* Id				(void) const			{ return "installationdir"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
			{ DLIB_RETVAL_REF.FromString(IDECore::GetInstallationDir()); }
	};

	//-----------------------------------------------------------------------
	// bool spw_loadcomponent(class_id).
	// Returns true if component found and successfully registered, false otherwise
	// If class_id is not a string, runtime error and nil is returned.
	//
	class LoadComponentFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 1; }
		const char* ArgCountError	(void) const			{ return "1 argument expected"; }
		const char* Id				(void) const			{ return "loadcomponent"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			std::string classId;
			boost::tuple<std::string> args;
			TypeConverter::DeltaToCpp(args, arguments);
			const ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(ARG(0));
			to_delta<bool>().convert(DLIB_RETVAL_PTR, entry);
		}
	};

	//-----------------------------------------------------------------------
	// handle spw_createcomponent(class_id [, {handle | class_id}]).
	// Returns a handle to the newly constructed component instance.
	// If class_id is not a string, runtime error and nil is returned.
	// the second optional argument is the parent handle.
	//
	class CreateComponentFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 1 || totalArgs == 2; }
		const char* ArgCountError	(void) const			{ return "1 or 2 arguments expected"; }
		const char* Id				(void) const			{ return "createcomponent"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			std::string classId;
			Handle parent;
			if (arguments.size() == 1)
			{
				boost::tuple<std::string> args;
				TypeConverter::DeltaToCpp(args, arguments);
				classId = ARG(0);
			}
			else if (arguments.size() == 2)
			{
				try {
					boost::tuple<std::string, Handle> args;
					TypeConverter::DeltaToCpp(args, arguments);
					classId = ARG(0);
					parent = ARG(1);
				} catch (std::exception&) {
					boost::tuple<std::string, std::string> args;
					TypeConverter::DeltaToCpp(args, arguments);
					classId = ARG(0);
					parent = Handle(ComponentRegistry::Instance().GetFocusedInstance(ARG(1)));
				}
			}

			Component* comp = ComponentFactory::Instance().CreateComponent(classId);
			if (!comp)
				RUNTIME_ERROR("Unknown component requested", Id());

			if (parent)
				comp->SetParent(parent.Resolve());

			to_delta<Handle>().convert(DLIB_RETVAL_PTR, Handle(comp));
		}
	};

	//-----------------------------------------------------------------------
	// void spw_destroycomponent({handle | class_id}).
	// Destroys the given component instance.
	//
	class DestroyComponentFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 1; }
		const char* ArgCountError	(void) const			{ return "1 argument expected"; }
		const char* Id				(void) const			{ return "destroycomponent"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			Handle handle;
			try {
				boost::tuple<Handle> args;
				TypeConverter::DeltaToCpp(args, arguments);
				ComponentFactory::Instance().DestroyComponent(ARG(0).Resolve());
			} catch (std::exception&) {
				boost::tuple<std::string> args;
				TypeConverter::DeltaToCpp(args, arguments);
				if (Component* comp = ComponentRegistry::Instance().GetFocusedInstance(ARG(0)))
					ComponentFactory::Instance().DestroyComponent(comp);
			}
		}
	};

	//-----------------------------------------------------------------------
	// void spw_setparent(handle, {handle | class_id}).
	// Sets the parent of an instance. Second argument is the parent.
	//
	class SetParentFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 2; }
		const char* ArgCountError	(void) const			{ return "2 arguments expected"; }
		const char* Id				(void) const			{ return "setparent"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			try {
				boost::tuple<Handle, Handle> args;
				TypeConverter::DeltaToCpp(args, arguments);
				if (ARG(0).Resolve() && ARG(1).Resolve())
					ARG(0)->SetParent(ARG(1).Resolve());
			} catch (std::exception&) {
				boost::tuple<Handle, std::string> args;
				TypeConverter::DeltaToCpp(args, arguments);
				Component* comp = ComponentRegistry::Instance().GetFocusedInstance(ARG(1));
				if (ARG(0).Resolve() && comp)
					ARG(0)->SetParent(comp);
			}
		}
	};

	//-----------------------------------------------------------------------
	// handle spw_thiscomponent().
	// Returns the calling component handle.
	//
	class ThisComponentFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 0; }
		const char* ArgCountError	(void) const			{ return "0 arguments expected"; }
		const char* Id				(void) const			{ return "thiscomponent"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			Component* comp = DeltaScriptProxy::GetScriptInstance(DPTR(vm));
			to_delta<Handle>().convert(DLIB_RETVAL_PTR, comp ? Handle(comp) : Handle());
		}
	};

	//-----------------------------------------------------------------------
	// handle spw_basecomponent([component]).
	// Returns the base class component handle of the calling component (if any).
	// If no component is specified the calling component is assumed as target.
	//
	class BaseComponentFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 0 || totalArgs == 1; }
		const char* ArgCountError	(void) const			{ return "At most 1 argument expected"; }
		const char* Id				(void) const			{ return "basecomponent"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			ScriptInstanceProxy* script = (ScriptInstanceProxy*) 0;
			if (arguments.empty())
				script = DeltaScriptProxy::GetScriptInstance(DPTR(vm));
			else
			{
				boost::tuple<Handle> args;
				TypeConverter::DeltaToCpp(args, arguments);
				Component* comp = ARG(0).Resolve();
				if (comp && DeltaScriptProxy::IsScriptInstance(comp))
					script = static_cast<ScriptInstanceProxy*>(comp);
			}
			Component* base = 0;
			if (script && script->base)
			{
				if (!script->IsBaseValid())	//Base has been destroyed and so we return nil
				{
					DLIB_RETVAL_REF.FromNil();
					return;
				}
				else
					base = script->base;
			}
			to_delta<Handle>().convert(DLIB_RETVAL_PTR, base ? Handle(base) : Handle());
		}
	};

	//-----------------------------------------------------------------------
	// handle spw_mostbasecomponent([component]).
	// Returns the last non-script base class component handle of the calling component (if any).
	// This is needed to get the actual classId and serial for matching slots targeted for the base class.
	// If no component is specified the calling component is assumed as target.
	//
	class MostBaseComponentFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 0 || totalArgs == 1; }
		const char* ArgCountError	(void) const			{ return "At most 1 argument expected"; }
		const char* Id				(void) const			{ return "mostbasecomponent"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			Component *target;
			if (arguments.empty())
				target = DeltaScriptProxy::GetScriptInstance(DPTR(vm));
			else
			{
				boost::tuple<Handle> args;
				TypeConverter::DeltaToCpp(args, arguments);
				target = ARG(0).Resolve();
			}

			while (DeltaScriptProxy::IsScriptInstance(target))
			{
				ScriptInstanceProxy* script = static_cast<ScriptInstanceProxy *>(target);
				if (script->base && !script->IsBaseValid())	//Some base has been destroyed and so we return nil
				{
					DLIB_RETVAL_REF.FromNil();
					return;
				}
				else
					target = script->base;
			}
			to_delta<Handle>().convert(DLIB_RETVAL_PTR, target ? Handle(target) : Handle());
		}
	};

	//-----------------------------------------------------------------------
	// handle spw_getcomponent(class_id[, serial]).
	// Returns the focused IDE component given a component class id.
	// If class_id is not a string, runtime error and nil is returned.
	//
	class GetComponentFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 1 || totalArgs == 2; }
		const char* ArgCountError	(void) const			{ return "1 or 2 arguments expected"; }
		const char* Id				(void) const			{ return "getcomponent"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			std::string class_id;
			uint serial=0;
			if (arguments.size() == 1)
			{
				boost::tuple<std::string> args;
				TypeConverter::DeltaToCpp(args, arguments);
				class_id = boost::get<0>(args);
			}
			else
			{
				boost::tuple<std::string, uint> args;
				TypeConverter::DeltaToCpp(args, arguments);
				class_id = boost::get<0>(args);
				serial = boost::get<1>(args);
			}

			//-- Retrieve requested component from registry
			const ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(class_id);
			if (!entry)
				RUNTIME_ERROR("Unknown component requested", Id());

			if (!entry.GetFocusedInstance()) {
				//-- Return a component class handle (serial == 0)
				to_delta<Handle>().convert(DLIB_RETVAL_PTR, Handle(class_id, 0));
				return;
			}

			serial = (serial == 0) ? entry.GetFocusedInstance()->GetSerial() : serial;
			Component* comp = entry.GetInstance(serial);
			if (comp == 0)
				RUNTIME_ERROR("Requested component instance could not be found", Id());

			to_delta<Handle>().convert(DLIB_RETVAL_PTR, Handle(comp));
		}
	};

	//-----------------------------------------------------------------------
	// string[] spw_getcomponents().
	// Returns a list with the names of the components in the registry.
	//
	class GetComponentsFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 0; }
		const char* ArgCountError	(void) const			{ return "0 arguments expected"; }
		const char* Id				(void) const			{ return "getcomponents"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			//-- collect all components in registry
			uint count=0;
			DeltaTable* table = DeltaObject::NewObject();
			DeltaValue index, data;

			BOOST_FOREACH(const ComponentEntry& compEntry, ComponentRegistry::Instance().GetComponentMap())
			{
				index.FromNumber(count++);
				data.FromString(compEntry.GetClassId());
				table->Set(index, data);
			}

			DLIB_RETVAL_REF.FromTable(table);
		}
	};

	//-----------------------------------------------------------------------
	// property spw_getclassproperty(class_id, id).
	// Returns the target property given a component class id.
	//
	class GetClassPropertyFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 2; }
		const char* ArgCountError	(void) const			{ return "2 arguments expected"; }
		const char* Id				(void) const			{ return "getclassproperty"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			boost::tuple<std::string, std::string> args;
			TypeConverter::DeltaToCpp(args, arguments);
			const conf::Property* p = ComponentRegistry::Instance().GetComponentEntry(ARG(0)).GetProperty(ARG(1));
			to_delta<const conf::Property*>().convert(DLIB_RETVAL_PTR, p);
		}
	};

	//-----------------------------------------------------------------------
	// property spw_getinstanceproperty({handle | class_id}, id).
	// Returns the target instance property given for the given component.
	//
	class InstancePropertyGetter {
	public:
		const char* Id (void) const { return "getinstanceproperty"; }

		const conf::Property* operator()(Component* component, const std::string& id) const
			{ return component->GetInstanceProperty(id); }
	};

	//-----------------------------------------------------------------------
	// property spw_getproperty({handle | class_id}, id).
	// Returns the target property given (instance or class) for the given component.
	//
	class SimplePropertyGetter {
	public:
		const char* Id (void) const { return "getproperty"; }

		const conf::Property* operator()(Component* component, const std::string& id) const
			{ return component->GetProperty(id); }
	};

	//-----------------------------------------------------------------------
	// Performs get property call using the template getter.
	//
	template<class Getter> class GetPropertyFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 2; }
		const char* ArgCountError	(void) const			{ return "2 arguments expected"; }
		const char* Id				(void) const			{ return Getter().Id(); }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			Component* component = (Component*) 0;
			std::string id;
			try {
				boost::tuple<Handle, std::string> args;
				TypeConverter::DeltaToCpp(args, arguments);
				component = ARG(0).Resolve();
				id = ARG(1);
			} catch (std::exception&) {
				boost::tuple<std::string, std::string> args;
				TypeConverter::DeltaToCpp(args, arguments);
				component = ComponentRegistry::Instance().GetFocusedInstance(ARG(0));
				id = ARG(1);
			}
			if (!component)
				RUNTIME_ERROR("Requested component instance could not be found", Id());
			const conf::Property* p = Getter()(component, id);
			to_delta<const conf::Property*>().convert(DLIB_RETVAL_PTR, p);
		}
	};

	//-----------------------------------------------------------------------
	// bool spw_propertytablesetdirty(class_id, dirty).
	// Sets dirty/clean state to the property table of the class with the given id.
	// If the property table is empty the call is ignored and false is returned.
	//
	class PropertyTableSetDirtyFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 2; }
		const char* ArgCountError	(void) const			{ return "2 arguments expected"; }
		const char* Id				(void) const			{ return "propertytablesetdirty"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			boost::tuple<std::string, bool> args;
			TypeConverter::DeltaToCpp(args, arguments);
			const conf::PropertyTable& table = ComponentRegistry::Instance().GetComponentEntry(ARG(0)).GetPropertyTable();
			bool empty = table.GetPropertyMap().empty();
			if (!empty)
				const_cast<conf::PropertyTable&>(table).SetDirty(ARG(1));
			DLIB_RETVAL_REF.FromBool(empty);
		}
	};

	//-----------------------------------------------------------------------
	// ret_type spw_call({handle | class_id}, function, args...).
	// Calls an exported function from the specified component.
	//
	class CallOp {
	public:
		const char* Id				(void)			 const { return "call"; }

		void SetReturnValue(DeltaVirtualMachine* vm, const Buffer& result, const std::string& funcReturnType)
		{
			//-- Extract return value
			if (!result.empty())
				TypeConverter::DecodeDeltaValue(comm::decoder(result), DLIB_RETVAL_PTR, funcReturnType);
		}

		template<typename Source, typename Dest>
		void operator()(Source source, Dest dest, const std::string& function, Message& result, const Buffer& args) const
			{ ComponentFunctionCaller(source, dest, function).Invoke(result, args); }
	};

	//-----------------------------------------------------------------------
	// ret_type spw_basecall({handle | class_id}, function, args...).
	// Calls an exported function from the specified component.
	//
	class BaseCallOp {
	public:
		const char* Id				(void)			 const { return "basecall"; }

		void SetReturnValue(DeltaVirtualMachine* vm, const Buffer& result, const std::string& funcReturnType)
		{
			//-- Extract return value
			if (!result.empty())
				TypeConverter::DecodeDeltaValue(comm::decoder(result), DLIB_RETVAL_PTR, funcReturnType);
		}

		template<typename Source, typename Dest>
		void operator()(Source source, Dest dest, const std::string& function, Message& result, const Buffer& args) const {
			ComponentFunctionCaller caller(source, dest, function);
			caller.SetBaseCall();
			caller.Invoke(result, args);
		}
	};

	//-----------------------------------------------------------------------
	// void spw_setundo({handle | class_id}, function, args...).
	// Sets the undo action for the specified component.
	//
	class SetUndoOp {
	public:
		const char* Id				(void)			 const { return "setundo"; }

		void SetReturnValue(DeltaVirtualMachine* vm, const Buffer& result, const std::string& funcReturnType) {}

		template<typename T>
		void operator()(const std::string& source, T dest, const std::string& function, Message& result, const Buffer& args) const
			{ ComponentUndoRegistrar(dest, function).Invoke(result, args); }
	};

	//-----------------------------------------------------------------------
	// Wraps a simple call invoker (No other arguments beside the call arguments).
	//
	template<class Op> class SimpleCallInvoker {
	public:
		uint16		MinimumArgs		(void)	const { return 2; }
		const char* ArgCountError	(void)	const { return "At least 2 arguments expected"; }
		const char* Id				(void)	const { return Op().Id(); }

		void SetFunctionInfo(const DeltaArgumentVec& arguments, Handle& handle,
			std::string& class_id, std::string& function) const
		{
			try {
				boost::tuple<Handle, std::string> args;
				TypeConverter::DeltaToCpp(args, arguments);
				handle = ARG(0);
				class_id = handle.GetClassId();
				function = ARG(1);
			} catch (std::exception&) {
				boost::tuple<std::string, std::string> args;
				TypeConverter::DeltaToCpp(args, arguments);
				class_id = ARG(0);
				function = ARG(1);
			}
		}

		void SetReturnValue(DeltaVirtualMachine* vm, const Buffer& result, const std::string& funcReturnType) const
			{ Op().SetReturnValue(vm, result, funcReturnType); }

		template<typename T>
		void operator()(const DeltaArgumentVec& arguments, const std::string& source, T dest,
			const std::string& function, Message& result, const Buffer& args) const
			{ Op()(source, dest, function, result, args); }
	};

	//-----------------------------------------------------------------------
	// int spw_settimer(id, time, continuous, {handle | class_id}, function, args...).
	// Sets the timer data with the given id. If id == -1 it starts a new timer.
	// The return value is the id of the timer.
	//
	class SetTimerInvoker {
	private:
		mutable int32 id;
	public:
		uint16		MinimumArgs		(void)	const { return 5; }
		const char* ArgCountError	(void)	const { return "At least 5 arguments expected"; }
		const char* Id				(void)	const { return "settimer"; }

		void SetFunctionInfo(const DeltaArgumentVec& arguments, Handle& handle, 
			std::string& class_id, std::string& function) const
		{
			try {
				boost::tuple<int32, int32, bool, Handle, std::string> args;
				TypeConverter::DeltaToCpp(args, arguments);
				handle = ARG(3);
				class_id = handle.GetClassId();
				function = ARG(4);
			} catch (std::exception&) {
				boost::tuple<int32, int32, bool, std::string, std::string> args;
				TypeConverter::DeltaToCpp(args, arguments);
				class_id = ARG(3);
				function = ARG(4);
			}
		}

		void SetReturnValue(DeltaVirtualMachine* vm, const Buffer& result, const std::string& funcReturnType) const
		{
			assert(id != -1);
			to_delta<int>().convert(DLIB_RETVAL_PTR, id);
		}

		template<typename T>
		void operator()(const DeltaArgumentVec& arguments, const std::string& source, T dest,
			const std::string& function, Message& result, const Buffer& buffer) const
		{
			assert(id == -1);
			boost::tuple<int32, int32, bool> args;
			TypeConverter::DeltaToCpp(args, arguments);
			id = timer::SimpleTimer::Set(
				ARG(1),
				boost::bind(TimerCallback, _1, ARG(1), ARG(2), ComponentFunctionCaller(source, dest, function), buffer),
				ARG(0)
			);
		}

		static void TimerCallback(int32 id, int32 offset, bool continuous,
			const ComponentFunctionCaller& caller, const Buffer& args)
		{
			Message result;	//not to be used since this is a timer callback
			caller.Invoke(result, args);
			if (continuous)
				timer::SimpleTimer::Set(
					offset,	
					boost::bind(TimerCallback, _1, offset, continuous, caller, args),
					id
				);
		}

		SetTimerInvoker(void) : id(-1) {}
	};

	//-----------------------------------------------------------------------
	// Performs a component call through to the template invoker.
	//
	template<class Invoker> class ComponentCallFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return Invoker().MinimumArgs() <= totalArgs; }
		const char* ArgCountError	(void) const			{ return Invoker().ArgCountError(); }
		const char* Id				(void) const			{ return Invoker().Id(); }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			Handle handle;
			std::string class_id;
			std::string function;

			const Invoker invoker;
			invoker.SetFunctionInfo(arguments, handle, class_id, function);

			//-- Get function entry from Registry
			const ComponentEntry& compEntry = ComponentRegistry::Instance().GetComponentEntry(class_id);
			const ComponentFuncEntry* funcEntry = (const ComponentFuncEntry*) 0;

			Component* comp = handle.GetSerial() != 0 ?
				compEntry.GetInstance(handle.GetSerial()) : compEntry.GetFocusedInstance();
			if (comp)
				if (const ComponentFuncEntry& entry = comp->GetDynamicFuncEntry(function))
					funcEntry = &entry;
			if (!funcEntry)
				funcEntry = &compEntry.GetFuncEntry(function);
			if (!*funcEntry)
				RUNTIME_ERROR("Component does not provide requested function", function.c_str());
			const ArgumentVec& formalArgs = funcEntry->GetArgumentVec();

			comm::encoder args;
			uint16 minArgs = invoker.MinimumArgs();
			for (uint16 i=minArgs; i < DPTR(vm)->TotalActualArgs(); ++i)
			{
				if ((uint16)(i-minArgs) >= formalArgs.size())
					RUNTIME_ERROR("More arguments than expected supplied", function.c_str());

				DeltaValue* arg = DPTR(vm)->GetActualArg(i);
				if (!TypeConverter::EncodeDeltaValue(args, arg, formalArgs[i-minArgs].first))
					RUNTIME_ERROR("Invalid type of argument", function.c_str());
			}

			//-- Call exported function
			Message result;
			try {
				if (funcEntry->IsStaticFunc())	//Static functions not found in derived class are NOT redirected to base
					invoker(arguments, DPTR(vm)->Id(), class_id, function, result, args);
				else if (funcEntry->IsMemberFunc() && handle.GetSerial() == 0 && compEntry.GetFocusedInstance())
				{
					Component* target = compEntry.GetFocusedInstance();
					bool isScriptInvocation = false;
					ScriptInstanceProxy* script;
					while (DeltaScriptProxy::IsScriptInstance(target) &&
						!(script = static_cast<ScriptInstanceProxy *>(target))->HasFunction(function) &&
						script->base && script->IsBaseValid() &&
						ComponentRegistry::Instance().GetComponentEntry(script->base->GetClassId()).GetFuncEntry(function).IsMemberFunc()
					)	//move up base hierarchy until the function is found
					{
						isScriptInvocation = true;
						target = script->base;
					}		
					if (isScriptInvocation)
					{
						if (script->base && !script->IsBaseValid())	//Base has been destroyed so procude runtime error
							RUNTIME_ERROR("Base class component destroyed", function.c_str());
						else
							invoker(arguments, DPTR(vm)->Id(), target, function, result, args);
					}
					else
						invoker(arguments, DPTR(vm)->Id(), class_id, function, result, args);
				}
				else if (handle.GetSerial() != 0)
				{
					Component* target = handle.Resolve();
					ScriptInstanceProxy* script = 0;
					while (DeltaScriptProxy::IsScriptInstance(target) &&
						!(script = static_cast<ScriptInstanceProxy *>(target))->HasFunction(function) &&
						script->base && script->IsBaseValid() &&
						ComponentRegistry::Instance().GetComponentEntry(script->base->GetClassId()).GetFuncEntry(function).IsMemberFunc()
					)	//move up base hierarchy until the function is found
					{
						target = script->base;
						handle = Handle(script->base);
					}
					if (script && script->base && !script->IsBaseValid())	//Base has been destroyed so procude runtime error
						RUNTIME_ERROR("Base class component destroyed", function.c_str());
					else
						invoker(arguments, DPTR(vm)->Id(), handle, function, result, args);
				}
				else
					RUNTIME_ERROR("There is no active instance of component", function.c_str());
			} catch (std::exception&) {
				RUNTIME_ERROR("Component does not provide requested function", function.c_str());
			}

			invoker.SetReturnValue(vm, result.GetData(), funcEntry->GetRetType());
		}
	};

	//-----------------------------------------------------------------------
	// void spw_canceltimer(id).
	// Cancels the timer with the given id.
	//
	class CancelTimerFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 1; }
		const char* ArgCountError	(void) const			{ return "1 argument expected"; }
		const char* Id				(void) const			{ return "canceltimer"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			boost::tuple<int32> args;
			TypeConverter::DeltaToCpp(args, arguments);
			timer::SimpleTimer::Cancel(ARG(0));
		}
	};

	//-----------------------------------------------------------------------
	// void spw_class_signal(signal, classId, args...).
	//
	class StaticSignalInvoker {
	public:
		const char* Id			(void) const { return "class_signal"; }
		bool		IsStatic	(void) const { return true; }

		void SetFunctionInfo(const DeltaArgumentVec& arguments, Handle& handle, 
			std::string& class_id, std::string& signal) const
		{
			boost::tuple<std::string, std::string> args;
			TypeConverter::DeltaToCpp(args, arguments);
			signal = ARG(0);
			class_id = ARG(1);
		}

		void operator()(const std::string& classId, const std::string& signal, Component*, const Buffer& args) const
		{
			ComponentSignalEmitter(classId, signal).Invoke(args);
		}
	};

	//-----------------------------------------------------------------------
	// void spw_inst_signal(signal, handle, args...).
	//
	class MemberSignalInvoker {
	public:
		const char* Id			(void) const { return "inst_signal"; }
		bool		IsStatic	(void) const { return false; }

		void SetFunctionInfo(const DeltaArgumentVec& arguments, Handle& handle, 
			std::string& class_id, std::string& signal) const
		{
			boost::tuple<std::string, Handle> args;
			TypeConverter::DeltaToCpp(args, arguments);
			signal = ARG(0);
			handle = ARG(1);
			class_id = handle.GetClassId();
		}

		void operator()(const std::string& classId, const std::string& signal, Component *comp, const Buffer& args) const
		{
			ComponentSignalEmitter(classId, signal).Invoke(comp, args);
		}
	};

	//-----------------------------------------------------------------------
	// Fires a signal through the template invoker.
	//
	template<class Invoker> class SignalFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs >= 2; }
		const char* ArgCountError	(void) const			{ return "At least 2 arguments expected"; }
		const char* Id				(void) const			{ return Invoker().Id(); }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			Handle handle;
			std::string classId;
			std::string signal;

			const Invoker invoker;
			invoker.SetFunctionInfo(arguments, handle, classId, signal);
			std::string error;
			if (!DeltaScriptProxy::ClassLoadedCheck(classId, &error))
				RUNTIME_ERROR(error, Id());
			const ComponentEntry& compEntry = ComponentRegistry::Instance().GetComponentEntry(classId);
			if (!compEntry.HasSignal(signal))
				RUNTIME_ERROR(classId + " has no signal " + signal, Id());
			const ComponentSignal& cs = ComponentSignalRegistry::Instance().GetSignal(signal);
			if (cs.IsStatic() != invoker.IsStatic())
				RUNTIME_ERROR(signal + " is a " + (cs.IsStatic() ? "static" : "member") + " signal", Id());
			
			const ArgumentVec& formalArgs = cs.GetArgumentVec();
			comm::encoder args;
			const uint16 minArgs = 1;	//signal only, the classId or handle is pushed as argument in the signal call
			for (uint16 i=minArgs; i < DPTR(vm)->TotalActualArgs(); ++i)
			{
				if ((uint16)(i-minArgs) >= formalArgs.size())
					RUNTIME_ERROR("More arguments than expected supplied", "Signal " + signal);
				DeltaValue* arg = DPTR(vm)->GetActualArg(i);
				if (!TypeConverter::EncodeDeltaValue(args, arg, formalArgs[i-minArgs].first))
					RUNTIME_ERROR("Invalid type of argument", "Signal " + signal);
			}
			invoker(classId, signal, invoker.IsStatic() ? 0 : handle.Resolve(), args);
		}
	};

	//-------------------------------------------------------//
	//---- class ControlAPI ---------------------------------//

	void ControlAPI::Install(void)
	{
		DeltaLibFuncBinder::Add(SPW_NAMESPACE_PREFIX "print", &ide::ControlAPI::Print);

		INSTALL_LIBRARY_FUNCTOR(InstallationDirFunctor);
		INSTALL_LIBRARY_FUNCTOR(LoadComponentFunctor);
		INSTALL_LIBRARY_FUNCTOR(CreateComponentFunctor);
		INSTALL_LIBRARY_FUNCTOR(DestroyComponentFunctor);
		INSTALL_LIBRARY_FUNCTOR(SetParentFunctor);

		INSTALL_LIBRARY_FUNCTOR(ThisComponentFunctor);
		INSTALL_LIBRARY_FUNCTOR(BaseComponentFunctor);
		INSTALL_LIBRARY_FUNCTOR(MostBaseComponentFunctor);
		INSTALL_LIBRARY_FUNCTOR(GetComponentFunctor);
		INSTALL_LIBRARY_FUNCTOR(GetComponentsFunctor);

		INSTALL_LIBRARY_FUNCTOR(GetClassPropertyFunctor);
		INSTALL_LIBRARY_FUNCTOR(GetPropertyFunctor<InstancePropertyGetter>);
		INSTALL_LIBRARY_FUNCTOR(GetPropertyFunctor<SimplePropertyGetter>);
		INSTALL_LIBRARY_FUNCTOR(PropertyTableSetDirtyFunctor);
		
		INSTALL_LIBRARY_FUNCTOR(ComponentCallFunctor<SimpleCallInvoker<CallOp>>);
		INSTALL_LIBRARY_FUNCTOR(ComponentCallFunctor<SimpleCallInvoker<BaseCallOp>>);
		INSTALL_LIBRARY_FUNCTOR(ComponentCallFunctor<SimpleCallInvoker<SetUndoOp>>);
		
		INSTALL_LIBRARY_FUNCTOR(ComponentCallFunctor<SetTimerInvoker>);
		INSTALL_LIBRARY_FUNCTOR(CancelTimerFunctor);

		INSTALL_LIBRARY_FUNCTOR(SignalFunctor<StaticSignalInvoker>);
		INSTALL_LIBRARY_FUNCTOR(SignalFunctor<MemberSignalInvoker>);
	}

	//-----------------------------------------------------------------------

	void ControlAPI::Uninstall(void)
	{
	}

	//-----------------------------------------------------------------------
	// string spw_print(x0, ..., xN).
	// Returns string representation of its argument list.
	//
	void ControlAPI::Print(DeltaVirtualMachine* vm)
	{
		std::string result;

		for (unsigned int numOfArgs=DPTR(vm)->TotalActualArgs(), argNo=0; argNo<numOfArgs; argNo++) {
			DeltaValue* arg=DPTR(vm)->GetActualArg(argNo);
			if (!arg) {
				fprintf(stderr,"print: arg %d not found!\n", argNo);
				continue;
			}
			const std::string& s = arg->ConvertToString();
			result += s;
		}
		ide::Call<void (const String&), SafeCall>(DPTR(vm)->Id(), "Output", "Append")(util::std2str(result));
		DBGOUT << result;

		DLIB_RETVAL_REF.FromString(result);
	}

	//-----------------------------------------------------------------------
}