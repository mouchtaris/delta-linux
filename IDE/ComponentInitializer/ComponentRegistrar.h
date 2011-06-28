/**
 *	ComponentRegistrar.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTREGISTRAR_H
#define IDE_COMPONENTREGISTRAR_H

#include "Common.h"
#include "ComponentEntry.h"

#include <boost/tuple/tuple.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////
// USE THE MACROS Luke
//
#define DELAYED_REGISTER_COMPONENT(className, create, destroy, prop, baseClassStr)	\
	static ide::ComponentEntryCreator::StaticInitializer							\
		s_create_component_entry_##className										\
		(ide::ComponentEntryCreator::TupleType(										\
			STRINGIFY(className), create, destroy, prop, baseClassStr				\
		))

//**********************************************************************

#define DELAYED_SET_COMPONENT_METADATA(className, metadata)	\
	static ide::ComponentMetadataSetter::StaticInitializer	\
		s_set_metadata_##className							\
		(ide::ComponentMetadataSetter::TupleType(			\
			STRINGIFY(className), &metadata					\
		))

//**********************************************************************

#define DELAYED_POST_COMPONENT_INITIALIZATION(className)	\
	static ide::ComponentPostInitializer::StaticInitializer	\
		s_post_initializer_##className						\
		(ide::ComponentPostInitializer::TupleType(STRINGIFY(className)))

//**********************************************************************

#define DELAYED_REGISTER_FUNCTION(											\
		className, funcName, memberFunc, staticFunc, retType, argList, doc	\
	)																		\
	static ide::ComponentEntryFunctionRegistrar::StaticInitializer			\
		s_register_function_##className##_##funcName						\
		(ide::ComponentEntryFunctionRegistrar::TupleType(					\
			STRINGIFY(className), STRINGIFY(funcName),						\
			memberFunc, staticFunc, retType, argList, doc					\
		))

//**********************************************************************

#define DELAYED_REGISTER_CMD_FUNCTION(										\
		className, funcName, memberFunc, staticFunc,						\
		retType, argList, isStatic, doc, path, isCheck, flags, image		\
	)																		\
	static ide::ComponentEntryUserCmdFunctionRegistrar::StaticInitializer	\
		s_register_function_##className##_##funcName						\
		(ide::ComponentEntryUserCmdFunctionRegistrar::TupleType(			\
			STRINGIFY(className), STRINGIFY(funcName),						\
			memberFunc, staticFunc, retType, argList, isStatic, doc,		\
			ComponentEntryUserCmdFunctionRegistrar::CmdTupleType(			\
				path, isCheck, flags, image									\
			)																\
		))

//**********************************************************************

#define DELAYED_REGISTER_SIGNAL(className, signalIdName, argList, isStatic)		\
	static ide::ComponentEntrySignalRegistrar::StaticInitializer				\
		s_register_signal_##className##_##signalIdName							\
		(ide::ComponentEntrySignalRegistrar::TupleType(							\
			STRINGIFY(className), STRINGIFY(signalIdName),						\
			STRINGIFY(argList),	isStatic										\
		))

//**********************************************************************

#define DELAYED_REGISTER_SLOT(												\
		className, funcName, signalId, memberFunc, staticFunc,				\
		retType, argList, doc												\
	)																		\
	static ide::ComponentEntrySlotRegistrar::StaticInitializer				\
		s_register_function_##className##_##funcName						\
		(ide::ComponentEntrySlotRegistrar::TupleType(						\
			STRINGIFY(className), STRINGIFY(funcName), signalId,			\
			memberFunc, staticFunc, retType, argList, doc					\
		))

//**********************************************************************

#define DELAYED_REGISTER_IMAGE(className, id, image)		\
	static ide::ComponentImageRegistrar::StaticInitializer	\
		s_register_image_##image							\
		(ide::ComponentImageRegistrar::TupleType(			\
			STRINGIFY(className), id, image					\
		))

////////////////////////////////////////////////////////////////////////
// Automatic component initialization support (fucking dlls)
// Note that TupleType<0> must be the component's ID
//
#define DECLARE_AUTOINITIALIZER(className)					\
	struct StaticInitializer {								\
	public:													\
		StaticInitializer (const TupleType& tuple)			\
			{ className::DelayedInit(tuple); }				\
	};														\
															\
	className (const TupleType& tuple) : m_tuple(tuple) {}	\
															\
	static void DelayedInit (const TupleType& tuple);		\
	static void Commit (void);								\
	static void Commit (const std::string& compId);			\
	static void Clear (void);								\
															\
	private:												\
		typedef std::list<className> ListType;				\
															\
		void init (void);									\
		TupleType			m_tuple;						\
		static ListType*	s_list

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentEntryCreator {
public:
	typedef boost::tuple<
		std::string,
		ComponentEntry::CreationFunc,
		ComponentEntry::DestructionFunc,
		ComponentEntry::SetPropertiesFunc,
		std::string
	> TupleType;

	DECLARE_AUTOINITIALIZER(ComponentEntryCreator);

public:
	static bool HasPendingComponents (void);
	static void GetPendingComponents (StdStringList& result);
};

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentMetadataSetter {
public:
	typedef boost::tuple<
		std::string,
		const ComponentEntry::Metadata*
	> TupleType;

	DECLARE_AUTOINITIALIZER(ComponentMetadataSetter);
};

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentPostInitializer {
public:
	typedef boost::tuple<std::string> TupleType;

	DECLARE_AUTOINITIALIZER(ComponentPostInitializer);
};

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentEntryFunctionRegistrar {
public:
	typedef boost::tuple<
		std::string,
		std::string,
		ComponentEntry::MemberFunc,
		ComponentEntry::StaticFunc,
		std::string,
		std::string,
		ComponentFuncEntry::FuncDocFunc
	> TupleType;

	DECLARE_AUTOINITIALIZER(ComponentEntryFunctionRegistrar);
};

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentEntrySignalRegistrar {
public:
	typedef boost::tuple<
		std::string,
		std::string,
		std::string,
		bool
	> TupleType;

	DECLARE_AUTOINITIALIZER(ComponentEntrySignalRegistrar);
};

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentEntrySlotRegistrar {
public:
	typedef boost::tuple<
		std::string,
		std::string,
		std::string,
		ComponentEntry::MemberFunc,
		ComponentEntry::StaticFunc,
		std::string,
		std::string,
		ComponentFuncEntry::FuncDocFunc
	> TupleType;

	DECLARE_AUTOINITIALIZER(ComponentEntrySlotRegistrar);
};

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentEntryUserCmdFunctionRegistrar {
public:
	typedef boost::tuple<
		String,
		bool,
		long,
		String
	> CmdTupleType;

	typedef boost::tuple<
		std::string,
		std::string,
		ComponentEntry::MemberFunc,
		ComponentEntry::StaticFunc,
		std::string,
		std::string,
		bool,
		ComponentFuncEntry::FuncDocFunc,
		CmdTupleType
	> TupleType;

	DECLARE_AUTOINITIALIZER(ComponentEntryUserCmdFunctionRegistrar);
};

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentImageRegistrar {
public:
	typedef boost::tuple<
		std::string,
		String,
		char**
	> TupleType;

	DECLARE_AUTOINITIALIZER(ComponentImageRegistrar);
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTREGISTRAR_H
