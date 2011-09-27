/**
 *	ComponentBuilder.h
 *
 * All the macros that begin with _ are private and must not be used
 * outside this file. All the others are public
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTBUILDER_H
#define IDE_COMPONENTBUILDER_H

#include "Common.h"
#include "ComponentRegistrar.h"
#include "ComponentMemberCallback.h"
#include "ExportedFunctionSkeleton.h"
#include "ComponentSignal.h"

#include <boost/preprocessor/control/if.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/type_traits/is_base_and_derived.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////
// Component namespace definition
//
#ifndef COMPNS
#	define COMPNS ide
#endif

////////////////////////////////////////////////////////////////////////
// Component declaration
//
#define DECLARE_COMPONENT(compClass)										\
	public:																	\
		const std::string& GetClassId (void) const { return s_classId; }	\
	private:																\
		static const std::string s_classId

////////////////////////////////////////////////////////////////////////
// Component implementation the variants with _ suffix, support a baseClass
//
#define _IMPLEMENT_CONSTRUCTION_AND_INITIALIZATION_WITH_ARGS(compClass, args)	\
	static COMPNS::Component* _Create_##compClass (COMPNS::ComponentEntry&) {	\
		COMPNS::Component* comp = new compClass args;							\
		return comp;															\
	}																			\

//**********************************************************************

#define _CHECK_COMPONENT_BASE(compClass, hasBase, baseClass)	\
	BOOST_MPL_ASSERT((boost::is_base_and_derived<				\
		BOOST_PP_IF(hasBase, baseClass, COMPNS::Component), compClass>))

#define _IMPLEMENT_COMPONENT_WITH_FUNCTIONS_(compClass, create, destroy, hasBase, baseClass)	\
	static void _SetProp_##compClass (COMPNS::ComponentEntry&);									\
	_CHECK_COMPONENT_BASE(compClass, hasBase, baseClass);										\
	DELAYED_REGISTER_COMPONENT(compClass, create, destroy, _SetProp_##compClass,				\
		BOOST_PP_IF(hasBase, STRINGIFY(baseClass), ""));										\
	DELAYED_POST_COMPONENT_INITIALIZATION(compClass);											\
	const std::string compClass::s_classId = STRINGIFY(compClass)

#define IMPLEMENT_COMPONENT_WITH_FUNCTIONS_(compClass, create, destroy, baseClass) \
	_IMPLEMENT_COMPONENT_WITH_FUNCTIONS_(compClass, create, destroy, 1, baseClass)

#define IMPLEMENT_COMPONENT_WITH_FUNCTIONS(compClass, create, destroy) \
	_IMPLEMENT_COMPONENT_WITH_FUNCTIONS_(compClass, create, destroy, 0, None)

//**********************************************************************

#define _IMPLEMENT_COMPONENT_WITH_ARGS_(compClass, args, hasBase, baseClass)	\
	_IMPLEMENT_CONSTRUCTION_AND_INITIALIZATION_WITH_ARGS(compClass, args)		\
	static void _Destroy_##compClass (COMPNS::Component* comp) { delete comp; }	\
	_IMPLEMENT_COMPONENT_WITH_FUNCTIONS_(compClass, _Create_##compClass,		\
		_Destroy_##compClass, hasBase, baseClass)

#define IMPLEMENT_COMPONENT_WITH_ARGS(compClass, args) \
	_IMPLEMENT_COMPONENT_WITH_ARGS_(compClass, args, 0, None)

#define IMPLEMENT_COMPONENT_WITH_ARGS_(compClass, args, baseClass) \
	_IMPLEMENT_COMPONENT_WITH_ARGS_(compClass, args, 1, baseClass)

#define IMPLEMENT_COMPONENT_(compClass, baseClass) \
	_IMPLEMENT_COMPONENT_WITH_ARGS_(compClass, (), 1, baseClass)

#define IMPLEMENT_COMPONENT(compClass) \
	_IMPLEMENT_COMPONENT_WITH_ARGS_(compClass, (), 0, None)

//**********************************************************************

#define _IMPLEMENT_WX_COMPONENT_WITH_ARGS_(compClass, wxClass, args, hasBase, baseClass)	\
	_IMPLEMENT_CONSTRUCTION_AND_INITIALIZATION_WITH_ARGS(compClass, args)					\
	static void _Destroy_##compClass (COMPNS::Component* comp)								\
		{ static_cast<wxClass*>(static_cast<compClass*>(comp))->Destroy(); }				\
	_IMPLEMENT_COMPONENT_WITH_FUNCTIONS_(compClass, _Create_##compClass,					\
		_Destroy_##compClass, hasBase, baseClass)

#define IMPLEMENT_WX_COMPONENT_WITH_ARGS(compClass, wxClass, args) \
	_IMPLEMENT_WX_COMPONENT_WITH_ARGS_(compClass, wxClass, args, 0, None)

#define IMPLEMENT_WX_COMPONENT_WITH_ARGS_(compClass, wxClass, args, baseClass) \
	_IMPLEMENT_WX_COMPONENT_WITH_ARGS_(compClass, wxClass, args, 1, baseClass)

#define IMPLEMENT_WX_COMPONENT(compClass, wxClass) \
	_IMPLEMENT_WX_COMPONENT_WITH_ARGS_(compClass, wxClass, (), 0, None)

#define IMPLEMENT_WX_COMPONENT_(compClass, wxClass, baseClass) \
	_IMPLEMENT_WX_COMPONENT_WITH_ARGS_(compClass, wxClass, (), 1, baseClass)

////////////////////////////////////////////////////////////////////////
// Metadata support
//
#define COMPONENT_METADATA(compClass, name, desc, author, version)		\
	static COMPNS::ComponentEntry::Metadata s_##compClass##_metadata =	\
		{ name, desc, author, version };								\
	DELAYED_SET_COMPONENT_METADATA(compClass, s_##compClass##_metadata)

////////////////////////////////////////////////////////////////////////
// Class properties
//
#define COMPONENT_SET_PROPERTIES_FUNCTION(compClass, entryName)	\
	static void _SetProp_##compClass (COMPNS::ComponentEntry& entryName)

////////////////////////////////////////////////////////////////////////
// Exported functions declaration
//
#define DECLARE_EXPORTED_MEMBER_(retType, funcName, argList, doc)							\
	static bool funcName##_CALLER (COMPNS::Component* comp, EXPORTED_FUNCTION_ARGS);		\
	static const String funcName##_DOC (const COMPNS::ComponentFuncEntry&) { return doc; }	\
	retType funcName argList

#define DECLARE_EXPORTED_MEMBER(retType, funcName, argList)		\
	DECLARE_EXPORTED_MEMBER_(retType, funcName, argList,		\
		_("This function has no documentation. The programmer was probably lazy!"))

//**********************************************************************

#define DECLARE_EXPORTED_STATIC_(retType, funcName, argList, doc)							\
	static bool funcName##_CALLER (EXPORTED_FUNCTION_ARGS);									\
	static const String funcName##_DOC (const COMPNS::ComponentFuncEntry&) { return doc; }	\
	static retType funcName argList

#define DECLARE_EXPORTED_STATIC(retType, funcName, argList)		\
	DECLARE_EXPORTED_STATIC_(retType, funcName, argList,		\
		_("This function has no documentation. The programmer was probably lazy!"))

////////////////////////////////////////////////////////////////////////
// Exported functions & commands implemetation utility macros
//
#define _IMPLEMENT_EXPORTED_MEMBER(className, retType, funcName, argList)					\
	bool className::funcName##_CALLER (COMPNS::Component* comp, EXPORTED_FUNCTION_ARGS) {	\
		className* self = static_cast<className*>(comp);									\
		COMPNS::ExportedFunctionSkeleton<retType argList>()									\
			(self, &className::funcName, EXPORTED_FUNCTION_ARG_NAMES);						\
		return true;																		\
	}																						\
	retType className::funcName argList

//**********************************************************************

#define _IMPLEMENT_EXPORTED_STATIC(className, retType, funcName, argList)	\
	bool className::funcName##_CALLER (EXPORTED_FUNCTION_ARGS)	{			\
		COMPNS::ExportedFunctionSkeleton<retType argList>()					\
			(&className::funcName, EXPORTED_FUNCTION_ARG_NAMES);			\
		return true;														\
	}																		\
	retType className::funcName argList

////////////////////////////////////////////////////////////////////////
// Exported functions implementation
//
#define _EXPORTED_FUNCTION(className, retType, funcName, argList, isStatic)			\
	DELAYED_REGISTER_FUNCTION(														\
		className, funcName,														\
		BOOST_PP_IF(isStatic, 0, &className::funcName##_CALLER),					\
		BOOST_PP_IF(isStatic, &className::funcName##_CALLER, 0),					\
		STRINGIFY(retType), STRINGIFY(argList),										\
		&className::funcName##_DOC													\
	);																				\
	BOOST_PP_IF(isStatic, _IMPLEMENT_EXPORTED_STATIC, _IMPLEMENT_EXPORTED_MEMBER)	\
		(className, retType, funcName, argList)

//**********************************************************************

#define EXPORTED_MEMBER(className, retType, funcName, argList) \
	_EXPORTED_FUNCTION(className, retType, funcName, argList, 0)

#define EXPORTED_STATIC(className, retType, funcName, argList) \
	_EXPORTED_FUNCTION(className, retType, funcName, argList, 1)

////////////////////////////////////////////////////////////////////////
// Exported command functions implementation
//
#define _EXPORTED_CMD_FUNCTION(className, funcName, path, flags, image, isChk, isStatic)	\
	DELAYED_REGISTER_CMD_FUNCTION(															\
		className, funcName,																\
		BOOST_PP_IF(isStatic, 0, &className::funcName##_CALLER),							\
		BOOST_PP_IF(isStatic, &className::funcName##_CALLER, 0),							\
		"void", BOOST_PP_IF(isChk, "(bool active)", "(void)"),								\
		isStatic, &className::funcName##_DOC,												\
		path, bool(isChk), flags, _T(image)													\
	);																						\
	BOOST_PP_IF(isStatic, _IMPLEMENT_EXPORTED_STATIC, _IMPLEMENT_EXPORTED_MEMBER)			\
		(className, void, funcName, BOOST_PP_IF(isChk, (bool active), (void)))

//**********************************************************************

#define EXPORTED_CMD_MEMBER(className, funcName, path, flags, image) \
	_EXPORTED_CMD_FUNCTION(className, funcName, path, flags, image, 0, 0)

#define EXPORTED_CMD_STATIC(className, funcName, path, flags, image) \
	_EXPORTED_CMD_FUNCTION(className, funcName, path, flags, image, 0, 1)

//**********************************************************************

#define EXPORTED_CHK_MEMBER(className, funcName, path, flags, image) \
	_EXPORTED_CMD_FUNCTION(className, funcName, path, flags, image, 1, 0)

#define EXPORTED_CHK_STATIC(className, funcName, path, flags, image) \
	_EXPORTED_CMD_FUNCTION(className, funcName, path, flags, image, 1, 1)

////////////////////////////////////////////////////////////////////////
// Exported signals implementation
//
#define _IMPLEMENT_EXPORTED_SIGNAL(className, sigIdName, argList, isStatic)							\
	static COMPNS::Signal<void argList> sig##sigIdName(STRINGIFY(className), STRINGIFY(sigIdName));	\
	DELAYED_REGISTER_SIGNAL(className, sigIdName, argList, isStatic)

#define EXPORTED_SIGNAL(className, sigIdName, argList) \
	_IMPLEMENT_EXPORTED_SIGNAL(className, sigIdName, argList, false)

#define EXPORTED_STATIC_SIGNAL(className, sigIdName, argList) \
	_IMPLEMENT_EXPORTED_SIGNAL(className, sigIdName, argList, true)

////////////////////////////////////////////////////////////////////////
// Exported slots implementation
//
#define _EXPORTED_SLOT(className, retType, funcName, argList, sigId, isStatic)		\
	DELAYED_REGISTER_SLOT(															\
		className, funcName, sigId,													\
		BOOST_PP_IF(isStatic, 0, &className::funcName##_CALLER),					\
		BOOST_PP_IF(isStatic, &className::funcName##_CALLER, 0),					\
		STRINGIFY(retType), STRINGIFY(argList),										\
		&className::funcName##_DOC													\
	);																				\
	BOOST_PP_IF(isStatic, _IMPLEMENT_EXPORTED_STATIC, _IMPLEMENT_EXPORTED_MEMBER)	\
		(className, retType, funcName, argList)

//**********************************************************************

#define EXPORTED_SLOT_MEMBER(className, retType, funcName, argList, sigId) \
	_EXPORTED_SLOT(className, retType, funcName, argList, sigId, 0)

#define EXPORTED_SLOT_STATIC(className, retType, funcName, argList, sigId) \
	_EXPORTED_SLOT(className, retType, funcName, argList, sigId, 1)

////////////////////////////////////////////////////////////////////////
// Image registration support
//
#define EXPORTED_IMAGE(className, id, image) \
	DELAYED_REGISTER_IMAGE(className, _T(id), image)

////////////////////////////////////////////////////////////////////////
// Compatibility defines
//
#define DECLARE_IDE_COMPONENT			DECLARE_COMPONENT
#define IMPLEMENT_IDE_COMPONENT			IMPLEMENT_COMPONENT
#define IMPLEMENT_WX_IDE_COMPONENT		IMPLEMENT_WX_COMPONENT

//**********************************************************************

#define DECLARE_EXPORTED_FUNCTION_		DECLARE_EXPORTED_MEMBER_
#define DECLARE_EXPORTED_FUNCTION		DECLARE_EXPORTED_MEMBER

#define EXPORTED_FUNCTION				EXPORTED_MEMBER
#define EXPORTED_CMD_FUNCTION			EXPORTED_CMD_MEMBER
#define EXPORTED_CHK_FUNCTION			EXPORTED_CHK_MEMBER

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTBUILDER_H
