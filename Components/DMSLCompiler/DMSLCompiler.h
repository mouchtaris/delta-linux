/**
 *	DMSLCompiler.h
 *
 *	-- IDE Delta Virtual Machine Component --
 *
 *	Compiler of the Decision Making Specification
 *	Language (DMSL).
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	July 2007
 */
#ifndef IDE_DMSL_COMPILER_COMPONENT_H_
#define IDE_DMSL_COMPILER_COMPONENT_H_

#include "Common.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"

#include "DecisionMaker.h"

#include "UndefineAllWin32Shit.h"

#pragma warning(disable: 4251)

namespace ide
{
	//----------------------------
	//-- class DMSLCompiler

	class DMSLCompiler :
		public Component
	{
		DECLARE_IDE_COMPONENT(DMSLCompiler);

	public:
		///--- type definitions

		///--- constructors / destructor
		DMSLCompiler(void);
		~DMSLCompiler(void);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));

		DECLARE_EXPORTED_MEMBER_(bool, Compile, (const String& uri),
			_("Compile a DMSL program"));
		DECLARE_EXPORTED_MEMBER_(bool, LoadProfile, (const String& uri),
			_("Load a profile"));
		DECLARE_EXPORTED_MEMBER_(bool, SetAttribute, (const String& attr, const String& value),
			_("Set an attribute to a profile"));
		DECLARE_EXPORTED_MEMBER_(String, GetAttribute, (const String& attr),
			_("Set an attribute to a profile"));
		DECLARE_EXPORTED_MEMBER_(bool, Evaluate, (const String& component),
			_("Evaluate a component"));

		DECLARE_EXPORTED_MEMBER(StringList, GetComponents, (void));
		DECLARE_EXPORTED_MEMBER(StringList, GetStereotypes, (void));
		DECLARE_EXPORTED_MEMBER(String, GetStereotypeDefinition, (const String& id));
		DECLARE_EXPORTED_MEMBER(StringList, GetDecisions, (void));

		DECLARE_EXPORTED_MEMBER(String, GetError, (void));
		DECLARE_EXPORTED_MEMBER(StringList, GetWarnings, (void));

		DECLARE_EXPORTED_MEMBER(bool, IsActive, (const String& component));

	private:
		///--- private API

		///--- private members
		dmsl::DecisionMaker dm;
	};
}

#endif	//IDE_DMSL_COMPILER_COMPONENT_H_
