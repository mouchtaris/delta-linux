/**
 *	DeltaCompiler.h
 *
 *	-- IDE Delta Compiler --
 *
 *	Compiler component of the Delta language.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#ifndef IDE_DELTA_COMPILER_COMPONENT_H_
#define IDE_DELTA_COMPILER_COMPONENT_H_

#include "Common.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"
#include "UndefineAllWin32Shit.h"

#include <boost/tuple/tuple.hpp>

#pragma warning(disable: 4251)

namespace ide
{
	class DeltaCompiler :
		public Component
	{
		DECLARE_IDE_COMPONENT(DeltaCompiler);

	public:
		///--- type definitions

		///--- constructors / destructor
		DeltaCompiler(void);
		~DeltaCompiler(void);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_STATIC_(void, CompileInternal, (const String& uri, const String& options,
			const String& directory), _("Compile an internal Sparrow Delta script file"));
		DECLARE_EXPORTED_STATIC_(unsigned long, Compile, (const String& uri, const String& options,
			const String& directory, const UIntList& buildId, const Handle& script), _("Compile a Delta script file"));
		DECLARE_EXPORTED_STATIC_(void, Clean, (const String& dbc),
			_("Clean targets of a Delta compiled file"));
		DECLARE_EXPORTED_STATIC_(void, SetOutputPath, (const String& path),
			_("Set target files output path"));

	private:
		///--- private API
		friend class OnCompilationMessage;

		typedef boost::tuple<UIntList, String, String, String, uint> Message;
		typedef std::list<Message> MessageList;

		///--- private members
		static std::string output_path;
	};
}

#endif	//IDE_DELTA_COMPILER_COMPONENT_H_
