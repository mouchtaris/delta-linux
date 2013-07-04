/**
 *	ConstructionAPI.cpp
 *
 *	-- Sparrow library for Delta --
 *
 *	Exported Component Construction API for Sparrow.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	August 2007
 *	Heavily refactored January 2009 by JL (Lilis).
 */
#include "ConstructionAPI.h"
#include "TypeConverter.h"
#include "DeltaScriptProxy.h"

#include "Component.h"
#include "ComponentRegistry.h"
#include "ComponentFactory.h"
#include "ComponentFunctionCaller.h"
#include "ComponentHandle.h"
#include "ComponentFunctionCallerSafe.h"
#include "Call.h"
#include "BitmapRegistry.h"

#include "DeltaByteCodeTypes.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaObject.h"

#include "SparrowLibFuncCommon.h"

#include "Adaptors.h"

#include <wx/image.h>

#include <boost/foreach.hpp>

#include <iostream>
#include <vector>

namespace ide
{
	//-------------------------------------------------------//
	//---- Library Function Functor classes -----------------//

	//-----------------------------------------------------------------------
	// void spw_setcomponentmetadata(class_id, name, description, author, version).
	//
	class SetComponentMetadataFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 5; }
		const char* ArgCountError	(void) const			{ return "Five arguments expected";	 }
		const char* Id				(void) const			{ return "setcomponentmetadata"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			boost::tuple<std::string, String, String, String, String> args;
			TypeConverter::DeltaToCpp(args, arguments);
			ComponentEntry::Metadata metadata = { ARG(1), ARG(2), ARG(3), ARG(4) };
			ComponentRegistry::Instance().GetComponentEntry(ARG(0)).SetMetadata(metadata);
		}
	};

	//-----------------------------------------------------------------------
	// Class and Instance registration Functions using templates and macros
	//
	template<typename T0>
	void DefaultLastArgFunctionCaller(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments, 
		bool (*func)(std::string *, DeltaVirtualMachine*, PARAM(T0), const std::string&),
		bool useDefaultLastArg, const char *id)
	{
		if (useDefaultLastArg) {
			boost::tuple<T0> args;
			TypeConverter::DeltaToCpp(args, arguments);
			std::string error;
			if (!func(&error, vm, ARG(0), ""))
				RUNTIME_ERROR(error, id);
		}
		else {
			boost::tuple<T0, std::string> args;
			TypeConverter::DeltaToCpp(args, arguments);
			std::string error;
			if (!func(&error, vm, ARG(0), ARG(1)))
				RUNTIME_ERROR(error, id);
		}
	}

	template<typename T0, typename T1>
	void DefaultLastArgFunctionCaller(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments, 
		bool (*func)(std::string *, DeltaVirtualMachine*, PARAM(T0), PARAM(T1), const std::string&),
		bool useDefaultLastArg, const char *id)
	{
		if (useDefaultLastArg) {
			boost::tuple<T0, T1> args;
			TypeConverter::DeltaToCpp(args, arguments);
			std::string error;
			if (!func(&error, vm, ARG(0), ARG(1), ""))
				RUNTIME_ERROR(error, id);
		}
		else {
			boost::tuple<T0, T1, std::string> args;
			TypeConverter::DeltaToCpp(args, arguments);
			std::string error;
			if (!func(&error, vm, ARG(0), ARG(1), ARG(2)))
				RUNTIME_ERROR(error, id);
		}
	}

	template<typename T0, typename T1, typename T2>
	void DefaultLastArgFunctionCaller(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments, 
		bool (*func)(std::string *, DeltaVirtualMachine*, PARAM(T0), PARAM(T1), PARAM(T2), const std::string&),
		bool useDefaultLastArg, const char *id)
	{
		if (useDefaultLastArg) {
			boost::tuple<T0, T1, T2> args;
			TypeConverter::DeltaToCpp(args, arguments);
			std::string error;
			if (!func(&error, vm, ARG(0), ARG(1), ARG(2), ""))
				RUNTIME_ERROR(error, id);
		}
		else {
			boost::tuple<T0, T1, T2, std::string> args;
			TypeConverter::DeltaToCpp(args, arguments);
			std::string error;
			if (!func(&error, vm, ARG(0), ARG(1), ARG(2), ARG(3)))
				RUNTIME_ERROR(error, id);
		}
	}

	template<typename T0, typename T1, typename T2, typename T3>
	void DefaultLastArgFunctionCaller(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments, 
		bool (*func)(std::string *, DeltaVirtualMachine*, PARAM(T0), PARAM(T1), PARAM(T2), PARAM(T3), const std::string&),
		bool useDefaultLastArg, const char *id)
	{
		if (useDefaultLastArg) {
			boost::tuple<T0, T1, T2, T3> args;
			TypeConverter::DeltaToCpp(args, arguments);
			std::string error;
			if (!func(&error, vm, ARG(0), ARG(1), ARG(2), ARG(3), ""))
				RUNTIME_ERROR(error, id);
		}
		else {
			boost::tuple<T0, T1, T2, T3, std::string> args;
			TypeConverter::DeltaToCpp(args, arguments);
			std::string error;
			if (!func(&error, vm, ARG(0), ARG(1), ARG(2), ARG(3), ARG(4)))
				RUNTIME_ERROR(error, id);
		}
	}


	template<typename T0>
	void FunctionCaller(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments, 
		bool (*func)(std::string *, DeltaVirtualMachine*, PARAM(T0)), const char *id)
	{
		boost::tuple<T0> args;
		TypeConverter::DeltaToCpp(args, arguments);
		std::string error;
		if (!func(&error, vm, ARG(0)))
			RUNTIME_ERROR(error, id);
	}

	template<typename T0, typename T1>
	void FunctionCaller(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments, 
		bool (*func)(std::string *, DeltaVirtualMachine*, PARAM(T0), PARAM(T1)), const char *id)
	{
		boost::tuple<T0, T1> args;
		TypeConverter::DeltaToCpp(args, arguments);
		std::string error;
		if (!func(&error, vm, ARG(0), ARG(1)))
			RUNTIME_ERROR(error, id);
	}

	template<typename T0, typename T1, typename T2>
	void FunctionCaller(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments, 
		bool (*func)(std::string *, DeltaVirtualMachine*, PARAM(T0), PARAM(T1), PARAM(T2)), const char *id)
	{
		boost::tuple<T0, T1, T2> args;
		TypeConverter::DeltaToCpp(args, arguments);
		std::string error;
		if (!func(&error, vm, ARG(0), ARG(1), ARG(2)))
			RUNTIME_ERROR(error, id);
	}

#define IMPLEMENT_FUNCTOR_WITH_DEFAULT_LAST_ARG(_name, _argMin, _argMax, _id)									\
	BOOST_STATIC_ASSERT(_argMin + 1 == _argMax);																\
	class _name##Functor {																						\
	public:																										\
		bool HasValidArgCount		(uint n) const	{ return n == _argMin || n == _argMax; }					\
		const char* ArgCountError	(void) const	{ return #_argMin " to " #_argMax " arguments expected"; }	\
		const char* Id				(void) const	{ return _id; }												\
		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& args) const {							\
			_name##Caller(vm, args, DeltaScriptProxy::_name, DPTR(vm)->TotalActualArgs() == _argMin, Id());		\
		}																										\
	}


#define IMPLEMENT_FUNCTOR(_name, _argNo,_id)													\
	class _name##Functor {																		\
	public:																						\
		bool HasValidArgCount		(uint n) const	{ return n == _argNo; }						\
		const char* ArgCountError	(void) const	{ return #_argNo " arguments expected"; }	\
		const char* Id				(void) const	{ return _id; }								\
		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& args) const {			\
			_name##Caller(vm, args, DeltaScriptProxy::_name, Id());								\
		}																						\
	}

//--------------------------------------------------------------------------------------------------------------------

// void spw_registercomponent(class_id[, base_class_id]).
#define RegisterComponentCaller DefaultLastArgFunctionCaller<std::string>
IMPLEMENT_FUNCTOR_WITH_DEFAULT_LAST_ARG(RegisterComponent, 1, 2, "registercomponent");

// void spw_unregistercomponent(class_id).
#define UnregisterComponentCaller FunctionCaller<std::string>
IMPLEMENT_FUNCTOR(UnregisterComponent, 1, "unregistercomponent");

//--------------------------------------------------------------------------------------------------------------------

// void spw_class_impl_static_function(class_id, function_name, function, signature[, doc_string]).
#define ClassImplementStaticFunctionCaller DefaultLastArgFunctionCaller<std::string, std::string, DeltaValue*, std::string>
IMPLEMENT_FUNCTOR_WITH_DEFAULT_LAST_ARG(ClassImplementStaticFunction, 4, 5, "class_impl_static_function");

// void spw_class_impl_member_function(class_id, function_name, function, signature[, doc_string]).
#define ClassImplementMemberFunctionCaller DefaultLastArgFunctionCaller<std::string, std::string, DeltaValue*, std::string>
IMPLEMENT_FUNCTOR_WITH_DEFAULT_LAST_ARG(ClassImplementMemberFunction, 4, 5, "class_impl_member_function");

// void spw_class_decl_required_member_function(class_id, function_name, signature[, doc_string]).
#define ClassDeclareRequiredMemberFunctionCaller DefaultLastArgFunctionCaller<std::string, std::string, std::string>
IMPLEMENT_FUNCTOR_WITH_DEFAULT_LAST_ARG(ClassDeclareRequiredMemberFunction, 3, 4, "class_decl_required_member_function");

// void spw_class_remove_static_function(class_id, function_name).
#define ClassRemoveStaticFunctionCaller FunctionCaller<std::string, std::string>
IMPLEMENT_FUNCTOR(ClassRemoveStaticFunction, 2, "class_remove_static_function");

// void spw_class_remove_member_function(class_id, function_name).
#define ClassRemoveMemberFunctionCaller FunctionCaller<std::string, std::string>
IMPLEMENT_FUNCTOR(ClassRemoveMemberFunction, 2, "class_remove_member_function");


// void spw_inst_impl_required_member_function(class_id, function_name, function).
#define InstanceImplementRequiredMemberFunctionCaller FunctionCaller<std::string, std::string, DeltaValue*>
IMPLEMENT_FUNCTOR(InstanceImplementRequiredMemberFunction, 3, "inst_impl_required_member_function");

// void spw_inst_impl_dynamic_member_function(handle, function_name, function, signature[, doc_string]).
#define InstanceImplementDynamicMemberFunctionCaller DefaultLastArgFunctionCaller<Component*, std::string, DeltaValue*, std::string>
IMPLEMENT_FUNCTOR_WITH_DEFAULT_LAST_ARG(InstanceImplementDynamicMemberFunction, 4, 5, "inst_impl_dynamic_member_function");

// void spw_inst_remove_dynamic_member_function(handle, function_name).
#define InstanceRemoveDynamicMemberFunctionCaller FunctionCaller<Component*, std::string>
IMPLEMENT_FUNCTOR(InstanceRemoveDynamicMemberFunction, 2, "inst_remove_dynamic_member_function");

//--------------------------------------------------------------------------------------------------------------------

// void spw_class_impl_static_handler(class_id, function, signal[, doc_string]).
#define ClassImplementStaticHandlerCaller DefaultLastArgFunctionCaller<std::string, std::string, DeltaValue*>
IMPLEMENT_FUNCTOR_WITH_DEFAULT_LAST_ARG(ClassImplementStaticHandler, 3, 4, "class_impl_static_handler");

// void spw_class_impl_member_handler(class_id, function, signal[, doc_string]).
#define ClassImplementMemberHandlerCaller DefaultLastArgFunctionCaller<std::string, std::string, DeltaValue*>
IMPLEMENT_FUNCTOR_WITH_DEFAULT_LAST_ARG(ClassImplementMemberHandler, 3, 4, "class_impl_member_handler");

// void spw_class_decl_required_member_handler(class_id, signal[, doc_string]).
#define ClassDeclareRequiredMemberHandlerCaller DefaultLastArgFunctionCaller<std::string, std::string>
IMPLEMENT_FUNCTOR_WITH_DEFAULT_LAST_ARG(ClassDeclareRequiredMemberHandler, 2, 3, "class_decl_required_member_handler");

// void spw_class_remove_static_handler(class_id, signal).
#define ClassRemoveStaticHandlerCaller FunctionCaller<std::string, std::string>
IMPLEMENT_FUNCTOR(ClassRemoveStaticHandler, 2, "class_remove_static_handler");

// void spw_class_remove_member_handler(class_id, signal).
#define ClassRemoveMemberHandlerCaller FunctionCaller<std::string, std::string>
IMPLEMENT_FUNCTOR(ClassRemoveMemberHandler, 2, "class_remove_member_handler");


// void spw_inst_impl_required_member_handler(class_id, signal, function).
#define InstanceImplementRequiredMemberHandlerCaller FunctionCaller<std::string, std::string, DeltaValue*>
IMPLEMENT_FUNCTOR(InstanceImplementRequiredMemberHandler, 3, "inst_impl_required_member_handler");

// void spw_inst_impl_dynamic_member_handler(handle, signal, function[, doc_string]).
#define InstanceImplementDynamicMemberHandlerCaller DefaultLastArgFunctionCaller<Component*, std::string, DeltaValue*>
IMPLEMENT_FUNCTOR_WITH_DEFAULT_LAST_ARG(InstanceImplementDynamicMemberHandler, 3, 4, "inst_impl_dynamic_member_handler");

// void spw_inst_remove_dynamic_member_handler(handle, handler).
#define InstanceRemoveDynamicMemberHandlerCaller FunctionCaller<Component*, std::string>
IMPLEMENT_FUNCTOR(InstanceRemoveDynamicMemberHandler, 2, "inst_remove_dynamic_member_handler");

//--------------------------------------------------------------------------------------------------------------------

// void spw_class_impl_static_command(user_cmd, command, function[, doc_string]).
#define ClassImplementStaticCommandCaller DefaultLastArgFunctionCaller<UserCommandDesc, std::string, DeltaValue*>
IMPLEMENT_FUNCTOR_WITH_DEFAULT_LAST_ARG(ClassImplementStaticCommand, 3, 4, "class_impl_static_command");

// void spw_class_impl_member_command(user_cmd, command, function[, doc_string]).
#define ClassImplementMemberCommandCaller DefaultLastArgFunctionCaller<UserCommandDesc, std::string, DeltaValue*>
IMPLEMENT_FUNCTOR_WITH_DEFAULT_LAST_ARG(ClassImplementMemberCommand, 3, 4, "class_impl_member_command");

// void spw_class_decl_required_member_command(user_cmd, command[, doc_string]).
#define ClassDeclareRequiredMemberCommandCaller DefaultLastArgFunctionCaller<UserCommandDesc, std::string>
IMPLEMENT_FUNCTOR_WITH_DEFAULT_LAST_ARG(ClassDeclareRequiredMemberCommand, 2, 3, "class_decl_required_member_command");

// void spw_class_remove_static_command(class_id, command).
#define ClassRemoveStaticCommandCaller FunctionCaller<std::string, std::string>
IMPLEMENT_FUNCTOR(ClassRemoveStaticCommand, 2, "class_remove_static_command");

// void spw_class_remove_member_command(class_id, command).
#define ClassRemoveMemberCommandCaller FunctionCaller<std::string, std::string>
IMPLEMENT_FUNCTOR(ClassRemoveMemberCommand, 2, "class_remove_member_command");


// void spw_inst_impl_required_member_command(class_id, function_name, function).
#define InstanceImplementRequiredMemberCommandCaller FunctionCaller<std::string, std::string, DeltaValue*>
IMPLEMENT_FUNCTOR(InstanceImplementRequiredMemberCommand, 3, "inst_impl_required_member_command");

// void spw_inst_impl_dynamic_member_command(handle, user_cmd, command, function[, doc_string]).
#define InstanceImplementDynamicMemberCommandCaller DefaultLastArgFunctionCaller<Component*, UserCommandDesc, std::string, DeltaValue*>
IMPLEMENT_FUNCTOR_WITH_DEFAULT_LAST_ARG(InstanceImplementDynamicMemberCommand, 4, 5, "inst_impl_dynamic_member_command");

// void spw_inst_remove_dynamic_member_command(handle, command).
#define InstanceRemoveDynamicMemberCommandCaller FunctionCaller<Component*, std::string>
IMPLEMENT_FUNCTOR(InstanceRemoveDynamicMemberCommand, 2, "inst_remove_dynamic_member_command");

//--------------------------------------------------------------------------------------------------------------------

// void spw_class_decl_static_signal(class_id, signal, arglist).
#define ClassDeclareStaticSignalCaller FunctionCaller<std::string, std::string, std::string>
IMPLEMENT_FUNCTOR(ClassDeclareStaticSignal, 3, "class_decl_static_signal");

// void spw_class_decl_member_signal(class_id, signal, arglist).
#define ClassDeclareMemberSignalCaller FunctionCaller<std::string, std::string, std::string>
IMPLEMENT_FUNCTOR(ClassDeclareMemberSignal, 3, "class_decl_member_signal");

//--------------------------------------------------------------------------------------------------------------------

	//-----------------------------------------------------------------------
	// void spw_addclassproperty(class_id, id, property).
	//
	class AddClassPropertyFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 3; }
		const char* ArgCountError	(void) const			{ return "Three arguments expected"; }
		const char* Id				(void) const			{ return "addclassproperty"; }
		
		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			boost::tuple<std::string, std::string, conf::Property*> args;
			TypeConverter::DeltaToCpp(args, arguments);
			ComponentRegistry::Instance().GetComponentEntry(ARG(0)).AddProperty(ARG(1), ARG(2));
		}
	};

	//-----------------------------------------------------------------------
	// void spw_removeclassproperty(class_id, id).
	//
	class RemoveClassPropertyFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 2; }
		const char* ArgCountError	(void) const			{ return "Two arguments expected";	}
		const char* Id				(void) const			{ return "removeclassproperty"; }
		
		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			boost::tuple<std::string, std::string> args;
			TypeConverter::DeltaToCpp(args, arguments);
			ComponentRegistry::Instance().GetComponentEntry(ARG(0)).RemoveProperty(ARG(1));
		}
	};

	//-----------------------------------------------------------------------
	// void spw_addinstanceproperty({handle | class_id}, id, property).
	//
	class AddInstancePropertyFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 3; }
		const char* ArgCountError	(void) const			{ return "Three arguments expected"; }
		const char* Id				(void) const			{ return "addinstanceproperty"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			try {
				boost::tuple<Handle, std::string, conf::Property*> args;
				TypeConverter::DeltaToCpp(args, arguments);
				ARG(0)->AddInstanceProperty(ARG(1), ARG(2));
			} catch(std::exception&) {
				boost::tuple<std::string, std::string, conf::Property*> args;
				TypeConverter::DeltaToCpp(args, arguments);
				ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(ARG(0));
				entry.GetFocusedInstance()->AddInstanceProperty(ARG(1), ARG(2));
			}
		}
	};

	//-----------------------------------------------------------------------
	// void spw_removeinstanceproperty({handle | class_id}, id).
	//
	class RemoveInstancePropertyFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 2; }
		const char* ArgCountError	(void) const			{ return "Two arguments expected"; }
		const char* Id				(void) const			{ return "removeinstanceproperty"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			try {
				boost::tuple<Handle, std::string> args;
				TypeConverter::DeltaToCpp(args, arguments);
				ARG(0)->RemoveInstanceProperty(ARG(1));
			} catch(std::exception&) {
				boost::tuple<std::string, std::string> args;
				TypeConverter::DeltaToCpp(args, arguments);
				ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(ARG(0));
				entry.GetFocusedInstance()->RemoveInstanceProperty(ARG(1));
			}
		}
	};

	//-----------------------------------------------------------------------
	// void spw_registerimage(image_id, source_file).
	//
	class RegisterImageFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 2; }
		const char* ArgCountError	(void) const			{ return "Two arguments expected"; }
		const char* Id				(void) const			{ return "registerimage"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			boost::tuple<String, String> args;
			TypeConverter::DeltaToCpp(args, arguments);
			BitmapRegistry::Instance().Insert(ARG(0), wxImage(ARG(1), wxBITMAP_TYPE_ANY));
		}
	};

	//-----------------------------------------------------------------------
	// void spw_unregisterimage(image_id).
	//
	class UnregisterImageFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 1; }
		const char* ArgCountError	(void) const			{ return "One argument expected"; }
		const char* Id				(void) const			{ return "unregisterimage"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			boost::tuple<String> args;
			TypeConverter::DeltaToCpp(args, arguments);
			BitmapRegistry::Instance().Remove(ARG(0));
		}
	};

	//-----------------------------------------------------------------------
	// {wxWindow|Nil} spw_basecreatewindow(parent).
	//
	class BaseCreateWindowFunctor {
	public:
		bool HasValidArgCount		(uint totalArgs) const	{ return totalArgs == 1; }
		const char* ArgCountError	(void) const			{ return "One argument expected"; }
		const char* Id				(void) const			{ return "basecreatewindow"; }

		void operator()(DeltaVirtualMachine* vm, const DeltaArgumentVec& arguments) const
		{
			boost::tuple<wxWindow*> args;
			TypeConverter::DeltaToCpp(args, arguments);
			ScriptInstanceProxy* script = DeltaScriptProxy::GetScriptInstance(DPTR(vm));
			wxWindow* result = (wxWindow*) 0;
			if (script && script->base && script->IsBaseValid())
				result = script->base->GenerateWindow(ARG(0));
			to_delta<wxWindow*>().convert(DLIB_RETVAL_PTR, result);
		}
	};


	//-------------------------------------------------------//
	//---- class ConstructionAPI ----------------------------//

	void ConstructionAPI::Install(void)
	{
		INSTALL_LIBRARY_FUNCTOR(RegisterComponentFunctor);
		INSTALL_LIBRARY_FUNCTOR(UnregisterComponentFunctor);
		INSTALL_LIBRARY_FUNCTOR(SetComponentMetadataFunctor);

#define INSTALL_FUNCTION_ENTRY_LIBRARY_FUNCTORS(type)							\
	INSTALL_LIBRARY_FUNCTOR(ClassImplementStatic##type##Functor);				\
	INSTALL_LIBRARY_FUNCTOR(ClassImplementMember##type##Functor);				\
	INSTALL_LIBRARY_FUNCTOR(ClassDeclareRequiredMember##type##Functor);			\
	INSTALL_LIBRARY_FUNCTOR(ClassRemoveStatic##type##Functor);					\
	INSTALL_LIBRARY_FUNCTOR(ClassRemoveMember##type##Functor);					\
	INSTALL_LIBRARY_FUNCTOR(InstanceImplementRequiredMember##type##Functor);	\
	INSTALL_LIBRARY_FUNCTOR(InstanceImplementDynamicMember##type##Functor);		\
	INSTALL_LIBRARY_FUNCTOR(InstanceRemoveDynamicMember##type##Functor)

		INSTALL_FUNCTION_ENTRY_LIBRARY_FUNCTORS(Function);
		INSTALL_FUNCTION_ENTRY_LIBRARY_FUNCTORS(Handler);
		INSTALL_FUNCTION_ENTRY_LIBRARY_FUNCTORS(Command);

#undef INSTALL_FUNCTION_ENTRY_LIBRARY_FUNCTORS

		INSTALL_LIBRARY_FUNCTOR(ClassDeclareStaticSignalFunctor);
		INSTALL_LIBRARY_FUNCTOR(ClassDeclareMemberSignalFunctor);

		INSTALL_LIBRARY_FUNCTOR(AddClassPropertyFunctor);
		INSTALL_LIBRARY_FUNCTOR(RemoveClassPropertyFunctor);
		INSTALL_LIBRARY_FUNCTOR(AddInstancePropertyFunctor);
		INSTALL_LIBRARY_FUNCTOR(RemoveInstancePropertyFunctor);
		
		INSTALL_LIBRARY_FUNCTOR(RegisterImageFunctor);
		INSTALL_LIBRARY_FUNCTOR(UnregisterImageFunctor);

		INSTALL_LIBRARY_FUNCTOR(BaseCreateWindowFunctor);

		TypeConverter::Initialize();
		DeltaScriptProxy::Initialize();
	}

	//-----------------------------------------------------------------------

	void ConstructionAPI::Uninstall(void)
	{
		DeltaScriptProxy::CleanUp();
		TypeConverter::CleanUp();
	}

	//-----------------------------------------------------------------------
}
