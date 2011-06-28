/**
 *	DMSLCompiler.h
 *
 *	-- IDE Delta Virtual Machine Component --
 *
 *	Compiler of the Decision Making Specification
 *	Language (DMSL).
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	Updated November 2008 to have JL as the compiler author.
 *	July 2007
 */
#include "DMSLCompiler.h"

#include "Call.h"
#include "ComponentFunctionCallerSafe.h"
#include "ComponentRegistry.h"
#include "ComponentFactory.h"
#include "ComponentUserCommandDesc.h"

#include "StringUtils.h"
#include "PropertyUtils.h"
#include "Streams.h"

#include "UndefineAllWin32Shit.h"

#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#ifndef NO_VLD
#include <vld.h>
#endif

namespace ide
{
	//-------------------------------------------------------//
	//---- static members -----------------------------------//


	//-------------------------------------------------------//
	//---- class DMSLCompiler -------------------------------//

	COMPONENT_METADATA(
		DMSLCompiler,
		_("DMSL Compiler"),
		_("Used to compile decision rules"),
		_T("Yannis Lilis <lilis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_IDE_COMPONENT(DMSLCompiler);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(DMSLCompiler, table)
	{
	}

	//-----------------------------------------------------------------------

	DMSLCompiler::DMSLCompiler(void)
	{
	}

	//-----------------------------------------------------------------------

	DMSLCompiler::~DMSLCompiler(void)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DMSLCompiler, void, Initialize, (void))
	{
		const std::string sparrowDir = util::normalizepath(IDECore::GetInstallationDir()) + "/";
		dmsl::DecisionMaker::Initialize(sparrowDir + "dmsl.ini");
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DMSLCompiler, void, CleanUp, (void))
	{
		dmsl::DecisionMaker::Cleanup();
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DMSLCompiler, bool, Compile, (const String& uri))
	{
		bool success = dm.Compile(util::str2std(uri));

		BOOST_FOREACH(const std::string& warning, dm.GetWarnings())
			TRACEOUT << "DMSL Warning: " << warning << std::endl;

		const std::string& error = dm.GetError();
		if (!error.empty())
			TRACEOUT << "DMSL Error: " << error << std::endl;

		return success;
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DMSLCompiler, bool, LoadProfile, (const String& uri))
	{
		bool success = dm.LoadProfile(util::str2std(uri));

		BOOST_FOREACH(const std::string& warning, dm.GetWarnings())
			TRACEOUT << "DMSL Warning: " << warning << std::endl;

		const std::string& error = dm.GetError();
		if (!error.empty())
			TRACEOUT << "DMSL Error: " << error << std::endl;

		return success;
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DMSLCompiler, bool, SetAttribute, (const String& attr, const String& value))
	{
		return dm.SetAttribute(util::str2std(attr), util::str2std(value));
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DMSLCompiler, String, GetAttribute, (const String& attr))
	{
		const dmsl::DecisionMaker::ProfileList& profiles = dm.GetProfileList();
		assert(!profiles.empty());
		dmsl::ExprValue *value = (dmsl::ExprValue *) 0;
		const std::string name = util::str2std(attr);
		//traverse profile list in reverse order (newer to older)
		for(dmsl::DecisionMaker::ProfileList::const_reverse_iterator i = profiles.rbegin(); i != profiles.rend(); ++i)
			if(value = (*i)->GetAttribute(name))
				break;
		return value ? util::std2str(value->toString()) : _("");
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DMSLCompiler, bool, Evaluate, (const String& component))
	{
		bool success = dm.Evaluate(util::str2std(component));

		BOOST_FOREACH(const std::string& warning, dm.GetWarnings())
			TRACEOUT << "DMSL Warning: " << warning << std::endl;

		const std::string& error = dm.GetError();
		if (!error.empty())
			TRACEOUT << "DMSL Error: " << error << std::endl;

		return success;
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DMSLCompiler, StringList, GetComponents, (void))
	{
		dmsl::DecisionMaker::StringVector vec = dm.GetComponents();
		StringList list;

		std::transform(
			vec.begin(),
			vec.end(),
			std::back_inserter(list),
			&util::std2str
		);
		return list;
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DMSLCompiler, StringList, GetStereotypes, (void))
	{
		dmsl::DecisionMaker::StringVector vec = dm.GetStereotypes();
		StringList list;

		std::transform(
			vec.begin(),
			vec.end(),
			std::back_inserter(list),
			&util::std2str
		);
		return list;
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DMSLCompiler, String, GetStereotypeDefinition, (const String& id))
	{
		return util::std2str(dm.GetStereotypeDefinition(util::str2std(id)));
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DMSLCompiler, StringList, GetDecisions, (void))
	{
		dmsl::DecisionMaker::StringVector vec = dm.GetDecisions();
		StringList list;

		std::transform(
			vec.begin(),
			vec.end(),
			std::back_inserter(list),
			&util::std2str
		);
		return list;
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DMSLCompiler, String, GetError, (void))
	{
		return util::std2str(dm.GetError());
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DMSLCompiler, StringList, GetWarnings, (void))
	{
		dmsl::DecisionMaker::StringVector vec = dm.GetWarnings();
		StringList list;

		std::transform(
			vec.begin(),
			vec.end(),
			std::back_inserter(list),
			&util::std2str
		);
		return list;
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(DMSLCompiler, bool, IsActive, (const String& component))
	{
		return dm.IsActiveComponent(util::str2std(component));
	}

	//-----------------------------------------------------------------------
}
