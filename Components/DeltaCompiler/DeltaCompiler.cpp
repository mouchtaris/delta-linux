/**
 *	DeltaCompiler.cpp
 *
 *	-- IDE Delta Compiler --
 *
 *	Proxy to the compiler component of the Delta language.
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#include "DeltaCompiler.h"

#include "Call.h"
#include "ComponentFunctionCallerSafe.h"
#include "ComponentRegistry.h"
#include "ComponentFactory.h"
#include "PropertyUtils.h"
#include "DelayedCaller.h"

#include "Streams.h"
#include "StringUtils.h"
#include "ConsoleHost.h"

#include "DeltaCompilerDefs.h"
#include "UndefineAllWin32Shit.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>

#ifndef NO_VLD
#include <vld.h>
#endif

#define	FILENAME(name, ext) \
			(std::string(name) + std::string(ext)).c_str()

#define	SRC(file)		FILENAME(file, ".dsc")
#define	BIN(file)		FILENAME(file, ".dbc")
#define	TXT(file)		FILENAME(file, ".txt")
#define	ICODE(file)		FILENAME(file, ".idc")

namespace ide
{
	//-------------------------------------------------------//
	//---- class DeltaCompiler ------------------------------//

	COMPONENT_METADATA(
		DeltaCompiler,
		_("Delta Compiler Invoker"),
		_("Runs the Delta Compiler to compile scripts"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("beta")
	);
	IMPLEMENT_IDE_COMPONENT(DeltaCompiler);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(DeltaCompiler, table)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC_SIGNAL(DeltaCompiler, CompileStarted, (const Handle& script, const UIntList& buildId));
	EXPORTED_STATIC_SIGNAL(DeltaCompiler, CompileFinished, (const Handle& script));
	EXPORTED_STATIC_SIGNAL(DeltaCompiler, TransformationFinished, (const Handle& script));
	EXPORTED_STATIC_SIGNAL(DeltaCompiler, CompilationMessage, (const UIntList& buildId, const String& type, const String& content, const String& file, uint line));

	//-----------------------------------------------------------------------

	DeltaCompiler::DeltaCompiler(void)
	{
	}

	//-----------------------------------------------------------------------

	DeltaCompiler::~DeltaCompiler(void)
	{
	}

	//-----------------------------------------------------------------------

	class OnCompilationMessage {
	private:
		static const String GetSingleQuotedItem(const String& message) {
			String::size_type firstSingleQuote = message.find_first_of(_T("'"));
			String::size_type secondSingleQuote = message.find_first_of(_T("'"), firstSingleQuote + 1);
			assert(firstSingleQuote != String::npos && secondSingleQuote != String::npos);
			return message.substr(firstSingleQuote + 1, secondSingleQuote - firstSingleQuote - 1);
		}

		void NotifyCompilationMessageThreadSafe(const String& message) {
			String type, content, file;
			uint line = 0;
			if (message.StartsWith(_T(DELTA_COMPILER_ERROR_PREFIX)) || message.StartsWith(_T(DELTA_COMPILER_WARNING_PREFIX)))
			{
				String::size_type firstComma = message.find_first_of(_T(","));
				assert(firstComma != String::npos);
				type = message.substr(0, firstComma);
				String::size_type secondComma = message.find_first_of(_T(","), firstComma + 1);
				assert(secondComma != String::npos);
				file = message.substr(firstComma + 8, secondComma - firstComma - 9);
				String rest = message.substr(secondComma + 1);

				String::size_type colon = rest.find_first_of(_T(":"));
				assert(colon != String::npos);
				content = rest.substr(colon + 2, rest.length() - colon - 3);
				line = atoi(util::str2std(rest.substr(6, colon - 6)).c_str());
			}
			else if (
				message.StartsWith(_T(DELTA_COMPILER_STARTED_PREFIX)) ||
				message.StartsWith(_T(DELTA_COMPILER_FINISHED_PREFIX))||
				message.StartsWith(_T(DELTA_COMPILER_FAILED_PREFIX))
			)
			{
				String::size_type firstSpace = message.find_first_of(_T(" "));
				assert(firstSpace != String::npos);
				type = message.substr(0, firstSpace);
				content = message;
				file = GetSingleQuotedItem(message);
			}
			else if (message.StartsWith(_T(DELTA_COMPILER_REFERENCE_PREFIX))) {
				type = _T(DELTA_COMPILER_REFERENCE_PREFIX);
				file = GetSingleQuotedItem(message);

				String::size_type comma = message.find_first_of(_T(","));
				assert(comma != String::npos);
				comma = message.find_first_of(_T(","), comma + 1);
				assert(comma != String::npos);	//second comma
				String::size_type dot = message.find_last_of(_T("."));
				assert(dot != String::npos);

				line = atoi(util::str2std(message.substr(comma + 7, dot - comma - 7)).c_str());
			}
			else	//not a recognized prefix, just forward message
				content = message;

			const DeltaCompiler::Message msg(buildId, type, content, file, line);
			timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(NotifyCompilationMessage, msg));
		}

		static void NotifyCompilationMessage(const DeltaCompiler::Message& m)
			{ sigCompilationMessage(m.get<0>(), m.get<1>(), m.get<2>(), m.get<3>(), m.get<4>()); }

	public:

		void operator()(const Buffer& buffer) {
			// Assume that the data coming should be in junks ending with '\n' characters.
			// If not (because  i.e. the last line exceeds the buffer size) the remaining
			// data are maintained and appended at the next invocation.
			Buffer::size_type pos = buffer.find_last_of("\n");
			if (pos == String::npos)
				prevRemainingData = buffer;
			else {
				Buffer newBuffer = prevRemainingData + buffer.substr(0, ++pos);
				std::vector<std::string> tokens;
				util::stringtokenizer(tokens, newBuffer, "\n");
				BOOST_FOREACH(const std::string& message, tokens)
					NotifyCompilationMessageThreadSafe(util::std2str(message));
				prevRemainingData = buffer.substr(pos);
			}
		}
		OnCompilationMessage(const UIntList& buildId = UIntList()) : buildId(buildId) {}
		~OnCompilationMessage() {}
	private:
		UIntList	buildId;
		Buffer		prevRemainingData;
	};

	//-----------------------------------------------------------------------
	
	void DeltaCompiler::onFinish(const Handle& script, bool compilation)
	{
		if (compilation)
			sigCompileFinished(script);
		else
			sigTransformationFinished(script);
	}

	void DeltaCompiler::onFinishThreadSafe(const Handle& script, bool compilation, bool success)
	{
		if (success)
			timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(onFinish, script, compilation));
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaCompiler, void, CompileInternal, (const String& uri, const String& options, const String& directory))
	{
#ifndef NDEBUG
		const std::string executable = "DeltaCompiler_d";
#else 
		const std::string executable = "DeltaCompiler";
#endif
		const std::string sparrowDir = IDECore::GetInstallationDir();
		util::ConsoleHost().Execute(
			sparrowDir + executable + " \"" + util::str2std(uri) + "\"" + util::str2std(options),
			util::str2std(directory),
			OnCompilationMessage()
		);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaCompiler, unsigned long, Compile, (const String& uri, const String& options,
		const String& directory, const UIntList& buildId, const Handle& script))
	{
#ifndef NDEBUG
		const std::string executable = "DeltaCompiler_d";
#else 
		const std::string executable = "DeltaCompiler";
#endif
		const std::string sparrowDir = IDECore::GetInstallationDir();
		util::ConsoleHost *compiler = new util::ConsoleHost(false);

		std::string allOptions = util::str2std(options);

		uint port = Call<uint (void)>(s_classId, "BuildSystem", "GetServerPort")();
		allOptions += " --build_server_port=" + boost::lexical_cast<std::string>(port);
		
		sigCompileStarted(script, buildId);
		return compiler->Execute(
			sparrowDir + executable + " \"" + util::str2std(uri) + "\"" + allOptions,
			util::str2std(directory),
			OnCompilationMessage(buildId),
			boost::bind(onFinishThreadSafe, script, true, _1)
		);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaCompiler, unsigned long, AspectTransformation, (const String& uri,
		const StringList& transformations, const String& options, const String& directory,
		const UIntList& buildId, const Handle& script))
	{
#ifndef NDEBUG
	const std::string executable = "AspectCompiler_d";
#else 
	const std::string executable = "AspectCompiler";
#endif

		const std::string sparrowDir = IDECore::GetInstallationDir();
		util::ConsoleHost *compiler = new util::ConsoleHost(false);

		std::string allOptions = util::str2std(options);
		allOptions += " --input=\"" + util::str2std(uri) + "\"";

		String aspects;
		BOOST_FOREACH(const String& aspect, transformations)
			aspects += (aspects.empty() ? _T("\"") : _T(" \"")) + aspect + _T("\"");
		allOptions += " --aspects=" + util::str2std(aspects);

		uint port = Call<uint (void)>(s_classId, "BuildSystem", "GetServerPort")();
		allOptions += " --build_server_port=" + boost::lexical_cast<std::string>(port);
		
		sigCompileStarted(script, buildId);
		return compiler->Execute(
			sparrowDir + executable + allOptions,
			util::str2std(directory),
			OnCompilationMessage(buildId),
			boost::bind(onFinishThreadSafe, script, false, _1)
		);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaCompiler, void, Clean, (const String& dbc))
	{
		std::string out = util::str2std(dbc.substr(0, dbc.find_last_of(_T("."))));

#define REMOVE(file)																			\
	do {																						\
		try { boost::filesystem::remove(boost::filesystem::path(file)); }						\
		catch(...) {																			\
			Call<void (const String&), SafeCall>(s_classId, "Output", "Append")					\
				(wxString::Format(_T("Cannot delete file %s."), util::std2str(file).c_str()));	\
		}																						\
	} while(false)

		REMOVE(BIN(out));
		REMOVE(ICODE(out));
		REMOVE(TXT(out));
	}

	//-----------------------------------------------------------------------
}
