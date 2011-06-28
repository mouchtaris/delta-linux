/**
 *	ScriptBuild.cpp
 *	Project manager component.
 *	Script build logic with dependencies.
 *	Anthony Savidis, November 2010.
 */
#include "Script.h"

#include "StringUtils.h"
#include "Properties.h"
#include "ExtProperties.h"
#include "PropertyUtils.h"
#include "ProjectManagerCommon.h"
#include "XMLPropertyVisitor.h"

#include "ComponentRegistry.h"
#include "ComponentFunctionCallerSafe.h"
#include "Call.h"
#include "DelayedCaller.h"
#include "ConsoleHost.h"
#include "IDEDialogs.h"

#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>

#include <wx/filename.h>

#include <assert.h>
#include <algorithm>
#include <functional>

//-----------------------------------------------------------------------

#define	DASSERT								assert
#define	NO_COMPILER_THREAD_PID				0
#define	BEFORE_COMPILATION_LINE_NO			0
#define	ONLY_ONE_COMPILER_LAUNCHED_PIDS		1
#define	SCRIPT_BUILD_FOR_THE_FIRST_TIME		1
#define	BUILD_TASK_ID						_T("Build")
#define	CLEAN_TASK_ID						_T("Clean")

#define	USED_SCRIPT_NOT_FOUND				"not_found"
#define	USED_SCRIPT_ONE_FOUND				"one_found"
#define	USED_SCRIPT_MANY_FOUND				"many_found"

#define	SCRIPT_TAG							"Script"
#define	LAST_BUILT_PROPERTIES_TAG			"LastBuildProperties"
#define	BYTECODE_PATH_PROPERTY_ID			conf::GetByteCodePathPropertyId()
#define	EMPTY

//-----------------------------------------------------------------------

#define	BUILD_ERROR_AMBIGUOUS_DEPENDENCY(_src,_dbcs)					\
	if (true) {															\
	if (outputErrors)													\
	PostBuildError(														\
		m_workId,														\
		std::string("Ambiguous dependency for '")	+					\
		_src										+					\
		"'; could be any of '"						+					\
		_dbcs										+					\
		"'!"															\
	); dependencyErrors = true; } else

//********************************

#define	BUILD_WARNING_MULTIPLE_SOURCES(_dbc, _src_used)					\
	if (outputErrors)													\
	PostBuildWarning(													\
		m_workId,														\
		std::string("Multiple sources for target '")	+				\
		_dbc											+				\
		"'; will use most recent '"						+				\
		_src_used										+				\
		"'!"															\
	); else

//********************************

#define	BUILD_ERROR_CYCLIC_SELF_DEPENDENCY(_src)						\
	if (true) {															\
	if (outputErrors)													\
	PostBuildError(														\
		m_workId,														\
		std::string("Cyclic dependency for '")	+						\
		_src									+						\
		"' with itself!"												\
	); dependencyErrors = true; } else

//********************************

#define	BUILD_ERROR_CYCLIC_INDIRECT_DEPENDENCY(_src, _dep_src, _path)	\
	if (true) {															\
	if (outputErrors)													\
	PostBuildError(														\
		m_workId,														\
		std::string("Cyclic dependency for '")	+						\
		_src									+						\
		"' with '"								+						\
		_dep_src								+						\
		"' via "								+						\
		_path									+						\
		"!"																\
	); dependencyErrors = true; } else

//********************************

#define	BUILD_ERROR_UNRESOLVED_DEPENDENCY(_src, _dbc)					\
	if (true) {															\
	if (outputErrors)													\
	PostBuildError(														\
		m_workId,														\
		std::string("Unresolved dependency for '")	+					\
		_src										+					\
		"'; don't know how to build target '"		+					\
		_dbc										+					\
		"'!"															\
	);																	\
	dependencyErrors = true; } else

//********************************

#define	BUILD_WARNING_NOT_PRODUCED_BY_THIS_WORKSPACE(_src, _dbc)		\
	if (outputErrors)													\
	PostBuildWarning(													\
		m_workId,														\
		std::string("External dependency for '")	+					\
		_src										+					\
		"'; file '"									+					\
		_dbc										+					\
		"' not produced by this workspace!"								\
	); else

//********************************

#define	BUILD_ERROR_FAILED_TO_RESOLVE_ALL_DEPENDENCIES(_src)			\
	PostBuildError(														\
		m_workId,														\
		std::string("Failed to build '")	+							\
		_src								+							\
		"' due to erros in resolving dependencies!"						\
	)

#define	BUILD_ERROR_FAILED_WHILE_BUILDING_DEPENDENCIES(_src)			\
	PostBuildError(														\
		m_workId,														\
		std::string("Failed to build '")	+							\
		_src								+							\
		"' due to erros while building dependencies!"					\
	)

#define	BUILD_ERROR_FAILED_WHILE_COPYING_DEPENDENCIES(_src)				\
	PostBuildError(														\
		m_workId,														\
		std::string("Failed to build '")	+							\
		_src								+							\
		"' due to erros while copying dependencies!"					\
	)

#define	BUILD_ERROR_DIRECTORY_CREATION_FAILED(_src, _type, _dir)		\
	PostBuildError(														\
		m_workId,														\
		std::string("Failed to start build for '")	+					\
		_src									+						\
		"' because " + _type + " directory '"	+	_dir +				\
		"' could not be created!"										\
	)

//********************************

#define	BUILD_MESSAGE_SCRIPT_IS_UPTODATE(_src)							\
	PostBuildMessage(													\
		m_workId,														\
		std::string("Skipping script '")		+						\
		_src									+						\
		"' (is up-to-date)!"											\
	)

#define	BUILD_MESSAGE_CURRENTLY_BEING_BUILT(_workId, _src)				\
	PostBuildMessage(													\
		_workId,														\
		std::string("Skipping script '")		+						\
		_src									+						\
		"' (already being built)!"										\
	)

#define	BUILD_MESSAGE_PROJECT_SETTINGS_CHANGED(_src)					\
	PostBuildMessage(													\
		m_workId,														\
		std::string("Recompiling script '")		+						\
		_src									+						\
		"' since the project settings have changed from last build!"	\
	)

#define	BUILD_MESSAGE_DEPENDENCIES_RECOMPILED(_src)						\
	PostBuildMessage(													\
		m_workId,														\
		std::string("Recompiling script '")		+						\
		_src									+						\
		"' since some of its dependencies were recompiled!"				\
	)

#define	BUILD_MESSAGE_CANT_RUN_DUE_TO_ERRORS(_src)						\
	PostBuildMessage(													\
		m_workId,														\
		std::string("Can't run script '")		+						\
		_src									+						\
		"' (there were build errors)!"									\
	)

#define	BUILD_MESSAGE_COPY_DEPEDENCY_AT_WORKING_DIRECTORY(_src, _dbc, _wdir)		\
	PostBuildMessage(																\
		m_workId,																	\
		std::string("Copying depedency '")					+						\
		_dbc												+						\
		"' (byte code file of the workspace) of source '"	+						\
		_src + "' at working dir '" + _wdir + "'!"									\
	)

#define	BUILD_MESSAGE_COPY_EXTERNAL_DEPEDENCY_AT_WORKING_DIRECTORY(_src,_dbc,_wdir)	\
	PostBuildMessage(																\
		m_workId,																	\
		std::string("Copying external depedency '")			+						\
		_dbc												+						\
		"' (byte code not of the workspace) of source '"	+						\
		_src + "' at working dir '" + _wdir + "'!"									\
	)

#define	BUILD_MESSAGE_DELETE_DEPEDENCY_FROM_WORKING_DIRECTORY(_dbc)		\
	PostBuildMessage(													\
		m_workId,														\
		std::string("Deleting depedency copy '")		+				\
		_dbc									+						\
		"' (byte code file of the workspace) from working directory!"	\
	)

#define	BUILD_MESSAGE_DELETE_EXTERNAL_DEPEDENCY_FROM_WORKING_DIRECTORY(_dbc)	\
	PostBuildMessage(															\
		m_workId,																\
		std::string("Deleting external depedency copy '")		+				\
		_dbc									+								\
		"' (byte code not of the workspace) from working directory!"			\
	)

#define	BUILD_ERROR_CONFLICTING_EXTERNAL_DEPEDENCY_AT_WORKING_DIRECTORY(_src, _from, _wdir, _found)	\
	PostBuildError(																					\
		m_workId,																					\
		std::string("Name conflict in copying external depedency '")				+				\
		_from																		+				\
		"' (byte code not of the workspace) of source '"							+				\
		_src + "' at working dir '" + _wdir + "', target was copied earlier from '"	+				\
		_found + "'!"																				\
	)

#define	BUILD_ERROR_CONFLICTING_DEPEDENCY_AT_WORKING_DIRECTORY(_src, _from, _wdir, _found)			\
	PostBuildError(																					\
		m_workId,																					\
		std::string("Name conflict in copying depedency '")							+				\
		_from																		+				\
		"' (byte code of the workspace) of source '"								+				\
		_src + "' at working dir '" + _wdir + "', target was copied earlier from '"	+				\
		_found + "'!"																				\
	)

//-----------------------------------------------------------------------

namespace ide {

	boost::mutex			Script::s_mutex;
	Script::ScriptPtrList*	Script::s_allScripts	= (ScriptPtrList*) 0;
	const Script*			Script::m_cleanStarter	= (Script*) 0;

/////////////////////////////////////////////////////////////////////////
	
Script::Script(void) : 
	m_buildInitiator				((Script*) 0), 
	m_buildDepsSucceeded			(false), 
	m_someDepsCompiled				(false),
	m_lastBuildPropertiesInvalid	(false),
	m_beingBuilt					(false),
	m_runAutomatically				(false),
	m_isApplication					(false),
	m_isCleaned						(false){

	MakeAllProperties();
	s_allScripts->push_back(this);
}

Script::~Script(void) {
	TerminateAllLaunchedCompilers();
	DASSERT(std::find(s_allScripts->begin(), s_allScripts->end(), this) != s_allScripts->end());
	s_allScripts->remove(this);
}

/////////////////////////////////////////////////////////////////////////

void Script::MakeAllProperties (void) {

	// Normal instances properties.
	conf::AddScriptExecutionProperties(this);
	conf::AddScriptBuildProperties(this);
	DASSERT(GetDeploymentPropertyValue() == DELTA_SCRIPT_DEPLOYMENT_LIBRARY); // Default value.

	// Non-instance local properties.
	conf::AddScriptLastBuildProperties(&m_lastBuildProperties);
}

/////////////////////////////////////////////////////////////////////////

static bool CreateDirectory (const std::string& dir) {
	if (!boost::filesystem::exists(dir)) {
		boost::filesystem::create_directory(dir);
		return boost::filesystem::exists(dir);
	}
	else
		return true;
}

static bool	CopyDeploymentByteCodeFile (const std::string& from, const std::string& at) {
	if (!boost::filesystem::exists(at))
		{ boost::filesystem::copy_file(from, at); return true; }
	else
	if (boost::filesystem::last_write_time(from) > boost::filesystem::last_write_time(at)) {
		boost::filesystem::remove(at);
		boost::filesystem::copy_file(from, at);
		return true;
	}
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////

void Script::SaveLastBuildProperties (void) const {

	xml::Node root;
	root.Create(_T(SCRIPT_TAG));
	conf::XMLValueSaver propertySaver(conf::MAKE_PATHS_RELATIVE, const_cast<Script*>(this)->GetPath());
	propertySaver.SetParent(root);
	const_cast<conf::PropertyTable&>(m_lastBuildProperties).Accept(LAST_BUILT_PROPERTIES_TAG, &propertySaver);

	wxXmlDocument doc;
	doc.SetFileEncoding(_T("utf-8"));
	doc.SetRoot(root.NativeType());
	doc.Save(GetProducedBuildInfoFileFullPath(), 2);
}

/////////////////////////////////////////////////////////////////////////

void Script::LoadLastBuildProperties (void) {

	const String file = GetProducedBuildInfoFileFullPath(); 
	wxXmlDocument doc;
	if (!wxFileName::FileExists(file)	|| 
		!doc.Load(file)					|| 
		doc.GetRoot()->GetName() != _T(SCRIPT_TAG))
 		return;

	conf::XMLValueLoader propertyLoader;
	propertyLoader.SetParent(doc.GetRoot());
	m_lastBuildProperties.Accept(LAST_BUILT_PROPERTIES_TAG, &propertyLoader);
	m_lastBuildPropertiesInvalid = false;
}

/////////////////////////////////////////////////////////////////////////

EXPORTED_FUNCTION(Script, bool, Load, (const String& uri)) {
	bool result = GenericFile::Load(uri);
	RefreshDeploymentPropertyValue();
	LoadLastBuildProperties();
	return result;
}

/////////////////////////////////////////////////////////////////////////

bool Script::DependsOn (const Script* script) const {
	if (std::find(m_buildDeps.begin(), m_buildDeps.end(), script) != m_buildDeps.end())
		return true;
	for (ScriptPtrList::const_iterator i = m_buildDeps.begin(); i != m_buildDeps.end(); ++i)
		if ((*i)->DependsOn(script))
			return true;
	return false;
}

/////////////////////////////////////////////////////////////////////////

EXPORTED_FUNCTION(Script, const HandleList, FindScriptsOfUsedByteCodeFile, (const String& fileName)) {
	
	StringList dirs;
	util::stringtokenizer(dirs, GetByteCodeLoadingPath(), _T(";"));

	ScriptPtrList scripts;
	BOOST_FOREACH(const String& dir, dirs)
		GetAllScriptsProducingByteCodeFileFullPath(util::str2std(dir + _T("/") + fileName), &scripts);
	
	HandleList handles;
	BOOST_FOREACH(Script* script, scripts)
		handles.push_back(Handle(script));
	return handles;
}

/////////////////////////////////////////////////////////////////////////

bool Script::HasAnyCyclicDependencies (const ScriptPtrList& toBuild, ScriptPtrList* putCyclicHere) {
	DASSERT(putCyclicHere->empty());
	for (ScriptPtrList::const_iterator i = toBuild.begin(); i != toBuild.end(); ++i)
		if ((*i)->DependsOn(this))
			putCyclicHere->push_back(*i);
	return putCyclicHere->empty();
}

/////////////////////////////////////////////////////////////////////////

const std::string Script::ProcuceCyclicDependencyPathString (const Script* target, const Script* start) const {
	std::string curr = GetSource();
	if (this == target)		// Reach destination.
		if (this == start)	// Cycle is closed.
			return curr;
		else				// We found path to target, so we now seek for the path to start.
			return curr + "->" + ProcuceCyclicDependencyPathString(start, start);
	else {
		for (ScriptPtrList::const_iterator i = m_buildDeps.begin(); i != m_buildDeps.end(); ++i)
			if ((*i)->DependsOn(target))
				return curr + "->" + (*i)->ProcuceCyclicDependencyPathString(target, start);
		DASSERT(false);
		return "";
	}
}

/////////////////////////////////////////////////////////////////////////

const String Script::GetByteCodeLoadingPathPropertyValue (void) const 
	{ return conf::get_prop_value<const conf::DirectoryListProperty>(GetInstanceProperty(BYTECODE_PATH_PROPERTY_ID), String()); }

/////////////////////////////////////////////////////////////////////////

const String Script::MakeAbsolutePath (const String& path, const String& dir) {
	wxFileName filename(path);
	if (filename.IsRelative())
		filename.MakeAbsolute(dir);
	return util::normalizepath(filename.GetFullPath());
}

/////////////////////////////////////////////////////////////////////////

const String Script::GetByteCodeLoadingPath (void) const {

	String path = GetByteCodeLoadingPathPropertyValue();

	if (!path.empty()) {
		StringList dirs;
		util::stringtokenizer(dirs, path, _T(";"));
		path.clear();
		BOOST_FOREACH(const String& dir, dirs)
			path += (path.empty() ? _T("") : _T(";")) + MakeAbsolutePath(dir, const_cast<Script*>(this)->GetPath());
	}

	return path;
}

/////////////////////////////////////////////////////////////////////////

const std::string Script::GetProducedByteCodeFile (void) const {
	const String name = const_cast<Script*>(this)->GetName();
	return util::str2std(name.substr(0, name.find_last_of(_T("."))) + _T(".dbc"));
}

const String Script::GetProducedBuildInfoFileFullPath (void) const {
	const String name = const_cast<Script*>(this)->GetName();
	return const_cast<Script*>(this)->GetOutputDirectory() + name.substr(0, name.find_last_of(_T("."))) + _T(".dbi");
}

const std::string Script::GetProducedByteCodeFileFullPath (void) const 
	{ return util::str2std(const_cast<Script*>(this)->GetOutputDirectory()) + GetProducedByteCodeFile(); }

/////////////////////////////////////////////////////////////////////////
// Produces a full path list taking the dbc file and appending it 
// to every bytecode path, the run working dir and the output dir.

struct PathEqualPredPred : public std::binary_function<String, String, bool> {
	const String& dbc;
	bool operator() (const String& path1, const String& path2) const 
		{ return wxFileName(path1 + _T("/") + dbc).SameAs(path2 + _T("/") +  dbc); }
	PathEqualPredPred (const String& _dbc) : dbc(_dbc){}
	PathEqualPredPred (const PathEqualPredPred& p) : dbc(p.dbc){}
};

void Script::GetAllLegalFullPathsForUnknownUsedByteCodeFile (const std::string& dbc, StdStringList* fullPaths) {

	StringList dirs, dirsFinal;
	String separators = _T("/\\");
	util::stringtokenizer(dirs, GetByteCodeLoadingPath(), _T(";,"));

	for (StringList::iterator i = dirs.begin(); i != dirs.end(); ++i) {
		if (i->Last() == separators[0] || i->Last() == separators[1])
			i->RemoveLast();
		if (std::find_if(dirsFinal.begin(), dirsFinal.end(), std::bind2nd(PathEqualPredPred(util::std2str(dbc)), *i)) == dirsFinal.end())
			dirsFinal.push_back(*i);
	}

	BOOST_FOREACH(const String& dir, dirsFinal)
		fullPaths->push_back(util::str2std(dir + wxFileName::GetPathSeparator()) + dbc);
}

/////////////////////////////////////////////////////////////////////////

void Script::GetAllScriptsProducingUnknownUsedByteCodeFile (
		Script*				caller, 
		const std::string&	dbc, 
		ScriptPtrList*		putHere
	) {
	StdStringList possibleFullPaths;
	caller->GetAllLegalFullPathsForUnknownUsedByteCodeFile(dbc, &possibleFullPaths);

	BOOST_FOREACH(const std::string& fullPath, possibleFullPaths)
		GetAllScriptsProducingByteCodeFileFullPath(fullPath, putHere);
}

/////////////////////////////////////////////////////////////////////////

void Script::GetAllScriptsProducingByteCodeFileFullPath (const std::string& fullPath, ScriptPtrList* putHere) {
	for (ScriptPtrList::iterator i = s_allScripts->begin(); i != s_allScripts->end(); ++i)
		if (wxFileName(util::std2str((*i)->GetProducedByteCodeFileFullPath())).SameAs(util::std2str(fullPath)))
			putHere->push_back(*i);
}

/////////////////////////////////////////////////////////////////////////

const std::string Script::GetSource (void) const
	{ return util::str2std(const_cast<Script*>(this)->GetURI()); }

/////////////////////////////////////////////////////////////////////////

Script* Script::GetScriptWithMostRecentSource (const ScriptPtrList& scripts) {
	DASSERT(!scripts.empty());
	Script* result = (Script*) 0;
	for (ScriptPtrList::const_iterator i = scripts.begin(); i != scripts.end(); ++i)
		if (!result || boost::filesystem::last_write_time(result->GetSource()) < boost::filesystem::last_write_time((*i)->GetSource())) 
			result = *i;
	return result;
}

/////////////////////////////////////////////////////////////////////////

void Script::LaunchCompiler (void) {

	String options;
	if (conf::get_prop_value<conf::BoolProperty>(GetInstanceProperty("debug"), true))
		options += _T(" --debug");
	if (conf::get_prop_value<conf::BoolProperty>(GetInstanceProperty("icode"), true))
		options += _T(" --icode");
	if (conf::get_prop_value<conf::BoolProperty>(GetInstanceProperty("text"), true))
		options += _T(" --text");
	options += ExtractFuncFilesStr();

	const String output_path = GetOutputDirectory();
	options += _T(" --output_path=") + util::quotepath(output_path);

	String bytecodePath = GetByteCodeLoadingPath();

	if (!bytecodePath.empty())
		options += _T(" --bytecode_path=") + util::quotepath(bytecodePath);

	options += _T(" --symbolic_names=\"") + GetSymbolicURI() + _T("\"");

	const String directory = Call<const String (void)>(this, TreeItemComponent::GetParent(), "GetPath")();

	if (m_workId.empty()) {
		Call<void (const String&, const String&, const String&)>(this, "DeltaCompiler", "CompileInternal")(
			GetURI(), 
			options,
			directory
		);
		timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(OnResourceWorkCompleted, this, BUILD_TASK_ID, m_workId));
	}
	else {
		boost::mutex::scoped_lock callLock(s_mutex);
		boost::mutex::scoped_lock compLock(m_compMutex);
		unsigned long compilerPid  = Call<unsigned long (const String&, const String&, const String&, const UIntList&, const Handle&)>
			(this, "DeltaCompiler", "Compile")(GetURI(), options, directory, m_workId, this);

		SetCompilerPid(compilerPid);
	}
}

/////////////////////////////////////////////////////////////////////////

EXPORTED_STATIC_SIGNAL(	//Artificial compilation message
	Script, 
	CompilationMessage, 
	(const UIntList& buildId, const String& type,const String& content, const String& file, uint line)
);	

//********************************

void Script::PostBuildOutput(const UIntList& workId, const String& type, const String& content, const String& file) 
	{ sigCompilationMessage(workId, type, content, file, BEFORE_COMPILATION_LINE_NO); }

//********************************

void Script::PostBuildError (const UIntList& workId, const std::string& error) {
	timer::DelayedCaller::Instance().PostDelayedCall(
		boost::bind(PostBuildOutput, workId, _T("Error"), util::std2str(error), GetSymbolicURI())
	);
}

//********************************

void Script::PostBuildMessage (const UIntList& workId, const std::string& message) {
	timer::DelayedCaller::Instance().PostDelayedCall(
		boost::bind(PostBuildOutput, workId, _T("Message"), util::std2str(message + "\n"), GetSymbolicURI())
	);
}

//********************************

void Script::PostBuildWarning (const UIntList& workId, const std::string& warning) {
	timer::DelayedCaller::Instance().PostDelayedCall(
		boost::bind(PostBuildOutput, workId, _T("Warning"), util::std2str(warning), GetSymbolicURI())
	);
}

/////////////////////////////////////////////////////////////////////////

void Script::TerminateAllLaunchedCompilers (void) {

	boost::mutex::scoped_lock buildLock(m_buildMutex);

	if (!m_allBuildPids.empty()) {

		DASSERT(
			(m_buildDeps.empty() && m_allBuildPids.size() == ONLY_ONE_COMPILER_LAUNCHED_PIDS) ||
			(m_allBuildPids.size() <= m_buildDeps.size())
		);

		if (!m_buildDeps.empty())						// Has dependencies.
			for (ScriptPtrList::iterator i = m_buildDeps.begin(); i != m_buildDeps.end(); ++i)
				(*i)->TerminateAllLaunchedCompilers();	// Recursive termination of compiler processes.
		else {											// Script without remaining dependency, thus should kill its compiler thread.
			DASSERT(m_allBuildPids.size() == ONLY_ONE_COMPILER_LAUNCHED_PIDS);
			DASSERT(m_allBuildPids.front() != NO_COMPILER_THREAD_PID);
			util::ConsoleHost().TerminateProcess(m_allBuildPids.front());
		}

		ClearBuildInformation();
	}
	else
		DASSERT(m_workId.empty() && m_buildDeps.empty() && !GetBuildInitiator());
}

/////////////////////////////////////////////////////////////////////////

bool Script::ResolveDependencies (
		const StringList&	deps,
		ScriptPtrList*		outDeps,
		StdStringList*		externalDeps,
		bool				outputErrors
	) {

	DASSERT(deps.size() % 2 == 0);
	bool dependencyErrors = false;

	for (StringList::const_iterator i = deps.begin(); i != deps.end(); ++i) {

		std::string dbc = util::str2std(*i++), status = util::str2std(*i);

		if (status == USED_SCRIPT_MANY_FOUND)
			BUILD_ERROR_AMBIGUOUS_DEPENDENCY(GetSource(), dbc);
		else {
			// Resolve dbc to list of scripts and find best candidate. If initially found the
			// the dbc is always a full path, else it is as found in the project source (just name).

			ScriptPtrList producers;
			if (status == USED_SCRIPT_ONE_FOUND)
				GetAllScriptsProducingByteCodeFileFullPath(dbc, &producers);
			else
				GetAllScriptsProducingUnknownUsedByteCodeFile(this, dbc, &producers);

			if (!producers.empty()) {
				Script* newDep = (Script*) 0;
				if (producers.size() > 1) {		// Multiple producers? choose most recent source.
					outDeps->push_back(newDep = GetScriptWithMostRecentSource(producers));
					BUILD_WARNING_MULTIPLE_SOURCES(dbc, newDep->GetSource());
				}
				else
					outDeps->push_back(newDep = producers.front());

				if (newDep == this)
					BUILD_ERROR_CYCLIC_SELF_DEPENDENCY(GetSource());
				else
				if (newDep->DependsOn(this)) {	// Cyclic dependency!
					std::string path = ProcuceCyclicDependencyPathString(m_buildDeps.back(), this);
					BUILD_ERROR_CYCLIC_INDIRECT_DEPENDENCY(GetSource(), newDep->GetSource(), path);
				}
			}
			else
			if (status == USED_SCRIPT_NOT_FOUND) // Can't find who can build the dbc and does not exist!
				BUILD_ERROR_UNRESOLVED_DEPENDENCY(GetSource(), dbc);
			else {
				BUILD_WARNING_NOT_PRODUCED_BY_THIS_WORKSPACE(GetSource(), dbc);
				externalDeps->push_back(dbc);
			}
		}
	}
	return !dependencyErrors;
}

/////////////////////////////////////////////////////////////////////////

void Script::CleanDependencies (const UIntList& workId) {

	ResolveDependencies(ExtractDependencies(), &m_buildDepsRetained, &m_externalBuildDepsRetained, false);

	if (!m_buildDepsRetained.empty()) {
		unsigned workSerial = 1;
		for (ScriptPtrList::iterator i = m_buildDepsRetained.begin(); i != m_buildDepsRetained.end(); ++i, ++workSerial)  {
			UIntList newWorkId = workId;
			newWorkId.push_back(workSerial);
			(*i)->Clean(newWorkId);		// Synchronous call.
		}
	}

	// We need to make this call at this point, not earlier, as the recursive invocations to Clean
	// will set the required values of m_buildDepsRetained and m_externalBuildDepsRetained across
	// the entire dependency graph.

	if (IsApplication()) {
		RecursiveDeleteByteCodeFilesFromWorkingDirectory(m_buildDepsRetained, m_externalBuildDepsRetained);
		DeleteByteCodeFileOfScriptFromWorkingDirectory(this);	// Delete self copy from the wdir.
	}
}

/////////////////////////////////////////////////////////////////////////

void Script::RecursiveCreateDependencyInformation(void) {
	ResolveDependencies(ExtractDependencies(), &m_buildDepsRetained, &m_externalBuildDepsRetained, false);
	for (ScriptPtrList::iterator i = m_buildDepsRetained.begin(); i != m_buildDepsRetained.end(); ++i)
		(*i)->RecursiveCreateDependencyInformation();
}

void Script::RecursiveClearDependencyInformation (void) {
	for (ScriptPtrList::iterator i = m_buildDepsRetained.begin(); i != m_buildDepsRetained.end(); ++i)
		(*i)->RecursiveClearDependencyInformation();
	m_buildDepsRetained.clear();
	m_externalBuildDepsRetained.clear();
}

/////////////////////////////////////////////////////////////////////////

void Script::DeleteCopiedByteCodeFilesFromWorkingDirectory (void) {

	DASSERT(IsApplication());

	RecursiveCreateDependencyInformation();

	RecursiveDeleteByteCodeFilesFromWorkingDirectory(m_buildDepsRetained, m_externalBuildDepsRetained, false);
	if (GetWorkingDirectory() != GetOutputDirectory())
		DeleteByteCodeFileOfScriptFromWorkingDirectory(this, false);	// Delete self copy from the wdir.

	RecursiveClearDependencyInformation();
}

/////////////////////////////////////////////////////////////////////////

void Script::BuildWithDependencies (const StringList& deps) {
	
	if (!ResolveDependencies(deps, &m_buildDeps, &m_externalBuildDepsRetained, true)) {
		m_externalBuildDepsRetained.clear();
		BUILD_ERROR_FAILED_TO_RESOLVE_ALL_DEPENDENCIES(GetSource());
		SetBuildCompleted(false, false);
	}
	else {
		
		DASSERT(!m_buildDeps.empty());
		m_buildDepsRetained = m_buildDeps;

		unsigned		workSerial = 1;
		ScriptPtrList	toBuild(m_buildDeps);	// Should copy since during build may asynchronously remove.
		m_buildDepsSucceeded = true;			// We assume they have all succceeded.

		for (ScriptPtrList::iterator i = toBuild.begin(); i != toBuild.end(); ++i, ++workSerial) {
			UIntList workId = m_workId;
			workId.push_back(workSerial);
			(*i)->BuildImpl(workId, this);
		}
	}
}

/////////////////////////////////////////////////////////////////////////

void Script::AddCompilerPidToBuildInitiator (unsigned long pid) 
	{ m_allBuildPids.push_back(pid); }

void Script::SetCompilerPid (unsigned long pid) {

	DASSERT(pid != NO_COMPILER_THREAD_PID);
	DASSERT(m_allBuildPids.empty());
	m_allBuildPids.push_back(pid);

	if (GetBuildInitiator())
		GetBuildInitiator()->AddCompilerPidToBuildInitiator(pid);
}

//********************************

bool Script::HaveAllDependenciesSucceeded (void) const
	{ return m_buildDepsSucceeded; }

bool Script::AreSomeDependenciesCompiled  (void) const
	{ return m_someDepsCompiled; }

bool Script::IsByteCodeUpToDate (void) const {

	const std::string binary = GetProducedByteCodeFileFullPath();
	return	boost::filesystem::exists(binary) &&
			boost::filesystem::last_write_time(binary) >= boost::filesystem::last_write_time(GetSource());
}

//********************************

void Script::SetIsApplication (bool val) {
	if (m_isApplication != val) {
		if (m_isApplication) 	// Was an application? delete deployment files.
			DeleteCopiedByteCodeFilesFromWorkingDirectory();
		m_isApplication = val;
	}
}

bool Script::IsApplication (void) const 
	{ return m_isApplication; }

//********************************

void Script::RefreshDeploymentPropertyValue (void) {
	std::string deployment = GetDeploymentPropertyValue();
	if (deployment == DELTA_SCRIPT_DEPLOYMENT_APPLICATION)
		SetIsApplication(true);
	else
	if (deployment == DELTA_SCRIPT_DEPLOYMENT_LIBRARY)
		SetIsApplication(false);
	else
		DASSERT(false);
}

void Script::CheckDeploymentPropertyChanged (const conf::PropertyIdVec& changed) {
	if (std::find(changed.begin(), changed.end(), conf::GetDeploymentPropertyId()) != changed.end())
		RefreshDeploymentPropertyValue();
}

const std::string Script::GetDeploymentPropertyValue (void) const {
	const conf::Property* p = GetInstanceProperty(conf::GetDeploymentPropertyId());
	DASSERT(p);
	return util::str2std(conf::get_prop_value<conf::EnumStringProperty>(p));
}

//********************************

bool Script::IsBeingBuilt (void) const
	{ return m_beingBuilt; }

void Script::ResetIsBeingBuilt (void)
	{ DASSERT(m_beingBuilt); m_beingBuilt = false; }

void Script::SetIsBeingBuilt (void)
	{ DASSERT(!m_beingBuilt);  m_beingBuilt = true; }

//********************************

Script* Script::GetBuildInitiator (void)
	{ return m_buildInitiator; }

void Script::SetBuildInitiator (Script* script) {
	DASSERT(!m_buildInitiator);
	m_buildInitiator = script;
}

void Script::AddBuildPseudoInitiator (Script* script, const UIntList& workId) 
	{ m_pseudoInitiators.push_back(PseudoInitiator(script, workId)); }

bool Script::HasBuildPseudoInitiators (void) const
	{ return !m_pseudoInitiators.empty(); }

bool Script::IsBuildPseudoInitiator (const Script* script) const {
	return	std::find_if(
				m_pseudoInitiators.begin(), 
				m_pseudoInitiators.end(), 
				std::bind2nd(MatchScriptPred(), script)
			) != m_pseudoInitiators.end();
}

void Script::ResetBuildInitiator (void) {
	DASSERT(m_buildInitiator);
	m_buildInitiator = (Script*) 0;
}

//********************************

void Script::ClearBuildInformation (void) {

	m_buildInitiator		= (Script*) 0;
	m_buildDepsSucceeded	= false;
	m_someDepsCompiled		= false;

	m_workId.clear();
	m_allBuildPids.clear();
	m_buildDeps.clear();
	m_pseudoInitiators.clear();

	ResetIsBeingBuilt();	// Always last since it may cause parallelism problems.
}

/////////////////////////////////////////////////////////////////////////

void Script::SetBuildCompleted (bool succeeded, bool wasCompiled) {
	
	boost::mutex::scoped_lock lock(m_buildDoneMutex);

	if (succeeded && wasCompiled)
		m_isCleaned = false;

	if (succeeded && wasCompiled)
		if (!IsApplication() || (succeeded = CopyByteCodeFileOfScriptAtWorkingDirectory(this)))	// Copy self at wdir.
			UpdateLastBuildProperties();

	timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(OnResourceWorkCompleted, this, BUILD_TASK_ID, m_workId));

	unsigned long pid = wasCompiled ? m_allBuildPids.front() : NO_COMPILER_THREAD_PID;

	if (GetBuildInitiator())
		GetBuildInitiator()->SetInitiatedBuildIsCompleted(this, pid, succeeded);

	for (PseudoInitiators::iterator i = m_pseudoInitiators.begin(); i != m_pseudoInitiators.end(); ++i) {
		timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(OnResourceWorkCompleted, this, BUILD_TASK_ID, i->second));
		if (i->first)
			i->first->SetInitiatedBuildIsCompleted(this, NO_COMPILER_THREAD_PID, succeeded);
	}

	ClearBuildInformation();

	// Is it scheduled to run automatically ?
	if (IsRunAutomaticallyAfterBuild())
		if (succeeded)
			RunCommit();
		else {
			BUILD_MESSAGE_CANT_RUN_DUE_TO_ERRORS(GetSource());
			ResetRunAutomaticallyAfterBuild();
		}
}

/////////////////////////////////////////////////////////////////////////

void Script::InitialiseNewBuildProcess (const UIntList& workId) {
	SaveSource();
	m_workId = workId;
	m_buildDepsRetained.clear();
	m_externalBuildDepsRetained.clear();
	m_deploymentDeps.clear();
}

/////////////////////////////////////////////////////////////////////////

void Script::UpdateLastBuildProperties (void) {

	for (const char** p = conf::GetScriptBuildPropertyIds(); *p; ++p) {
		conf::Property* curr = GetInstancePropertyTable().GetProperty(*p);	// Normal build property.
		DASSERT(curr);
		m_lastBuildProperties.AddProperty(*p, curr->Clone());	// Last build property.
	}

	conf::Property* p = m_lastBuildProperties.GetProperty(BYTECODE_PATH_PROPERTY_ID);
	DASSERT(p);
	static_cast<conf::StringProperty*>(p)->SetValue(GetByteCodeLoadingPath());

	m_lastBuildPropertiesInvalid = false;
	SaveLastBuildProperties();
}

//********************************

bool Script::AreLastBuildPropertiesSameAsCurrent (void) const {

	if (m_lastBuildPropertiesInvalid)
		return false;
	else {
		for (const char** p = conf::GetScriptBuildPropertyIds(); *p; ++p) {

			conf::Property* curr = GetInstancePropertyTable().GetProperty(*p);	// Normal build property.
			DASSERT(curr);

			conf::Property* last = m_lastBuildProperties.GetProperty(*p);		// Last build property.
			DASSERT(last);

			if (!curr->Equal(last))
				return false;
		}

		conf::Property* p = m_lastBuildProperties.GetProperty(BYTECODE_PATH_PROPERTY_ID);
		DASSERT(p);
		return static_cast<conf::StringProperty*>(p)->GetValue() == GetByteCodeLoadingPath();
	}
}

/////////////////////////////////////////////////////////////////////////

void Script::SetInitiatedBuildIsCompleted (Script* script, unsigned long pid, bool succeeded) {

	DASSERT(std::find(m_buildDeps.begin(), m_buildDeps.end(), script) != m_buildDeps.end());

	PidList::iterator i = std::find(m_allBuildPids.begin(), m_allBuildPids.end(), pid);

	DASSERT(
		(pid == NO_COMPILER_THREAD_PID	&& i == m_allBuildPids.end())		|| 
		(!script->IsBuildPseudoInitiator(this) || (i == m_allBuildPids.end()))
	);

	m_buildDeps.remove(script);
	if (pid != NO_COMPILER_THREAD_PID) {	// Dependency was actually compiled.
		if (i != m_allBuildPids.end())
			m_allBuildPids.erase(i);
		m_someDepsCompiled = true;			// At least one dependency was (re)compiled.
	}

	if (!succeeded)
		m_buildDepsSucceeded = false;
	else
	if (m_isCleaned)						// Dep succeeded and assumed cleaned?
		m_isCleaned = script->IsCleaned();	// Only if its dependency is assumed cleaned too.

	if (m_buildDeps.empty()) {				// Done with building all dependencies.
		DASSERT(m_allBuildPids.empty());
		if (HaveAllDependenciesSucceeded())
			if (IsApplication() && !RecursiveCopyByteCodeFilesAtWorkingDirectory(m_buildDepsRetained, m_externalBuildDepsRetained)) {
				BUILD_ERROR_FAILED_WHILE_COPYING_DEPENDENCIES(GetSource());
				SetBuildCompleted(false, false);
			}
			else
			if (!IsByteCodeUpToDate())
				 LaunchCompiler();
			else
			if (AreSomeDependenciesCompiled()) 
				{ BUILD_MESSAGE_DEPENDENCIES_RECOMPILED(GetSource());  LaunchCompiler(); }
			else
			if (!AreLastBuildPropertiesSameAsCurrent()) 
				{ BUILD_MESSAGE_PROJECT_SETTINGS_CHANGED(GetSource());  LaunchCompiler(); }
			else {
				BUILD_MESSAGE_SCRIPT_IS_UPTODATE(GetSource());
				SetBuildCompleted(true, false);
			}
		else {
			BUILD_ERROR_FAILED_WHILE_BUILDING_DEPENDENCIES(GetSource());
			SetBuildCompleted(false, false);
		}
	}
}

/////////////////////////////////////////////////////////////////////////

void Script::OnCompilationMessage(const UIntList& buildId, const String& content, const String& file)
	{ sigCompilationMessage(buildId, _T("Message"), content, file, BEFORE_COMPILATION_LINE_NO); }

/////////////////////////////////////////////////////////////////////////

EXPORTED_SLOT_MEMBER(Script, void, OnCompileFinished, (const std::string& compiler, const Handle& script), "CompileFinished") {

	if (script.Resolve() == this) {

		boost::mutex::scoped_lock lock(m_compMutex);
		DASSERT(!m_workId.empty() && m_allBuildPids.size() == ONLY_ONE_COMPILER_LAUNCHED_PIDS && m_buildDeps.empty());
		
		SetBuildCompleted(
			IsByteCodeUpToDate(),	// When the dbc is up-to-date the compilation finished with no errors.
			true					// And we have always launched the compiler.
		);
	}
}

/////////////////////////////////////////////////////////////////////////

EXPORTED_SLOT_MEMBER(Script, void, OnLibraryDefinitionsChanged,
	(const std::string& classId, const StringList newDefinitions), "LibraryDefinitionsChanged")
{
	using namespace conf;
	if (const Property *property = GetInstanceProperty("libs")) {
		const PropertyTable old = GetInstancePropertyTable();
		MultiChoiceProperty* p = safe_prop_cast<MultiChoiceProperty>(const_cast<Property*>(property));
		BOOST_FOREACH(const String& prevChoice, p->GetAllChoices())	//Remove old choices
			if (std::find(newDefinitions.begin(), newDefinitions.end(), prevChoice) == newDefinitions.end())
				p->RemoveChoice(prevChoice);
		BOOST_FOREACH(const String& def, newDefinitions)			//Add new choices
			p->AddChoice(def);
		this->ComponentAppliedChangedProperties(old, PropertyIdVec(1, "libs"));
	}
}

/////////////////////////////////////////////////////////////////////////

EXPORTED_FUNCTION(Script, unsigned long, Clean, (const UIntList& workId)) {

	boost::mutex::scoped_lock buildLock(m_cleanMutex);
	timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(OnResourceWorkStarted, this, CLEAN_TASK_ID, workId));

	if (!m_isCleaned) {

		if (!m_cleanStarter)
			m_cleanStarter = this;

		CleanDependencies(workId);

		std::string dbc = GetProducedByteCodeFileFullPath();
		if (boost::filesystem::exists(dbc)) {
			boost::mutex::scoped_lock lock(s_mutex);
			Call<void (const String&)>(this, "DeltaCompiler", "Clean")(util::std2str(dbc));
		}

		std::string dbi = util::str2std(GetProducedBuildInfoFileFullPath()); 
		if (boost::filesystem::exists(dbi))
			boost::filesystem::remove(dbi);

		if (m_cleanStarter == this) {
			RecursiveClearDependencyInformation();	// Clears across the entire graph.
			m_cleanStarter = (Script*) 0;
		}

		m_isCleaned = true;
	}

	timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(OnResourceWorkCompleted, this, CLEAN_TASK_ID, workId));
	return NO_COMPILER_THREAD_PID;
}

/////////////////////////////////////////////////////////////////////////

const StringList Script::ExtractDependencies (void) const {
	boost::mutex::scoped_lock lock(s_mutex);
	return	Call<StringList (const String&, const String&)>(const_cast<Script*>(this), "DeltaVM", "ExtractBuildDependencies")(
				const_cast<Script*>(this)->GetURI(), 
				 GetByteCodeLoadingPath()
			);
}

/////////////////////////////////////////////////////////////////////////

bool Script::IsBuildInProgressHandler (const UIntList& workId, Script* initiator) {

	bool isBeingBuilt = false;

	{	// To avoid registering pseudo initiator during build post-completion actions.
		boost::mutex::scoped_lock lock(m_buildDoneMutex);
		if (isBeingBuilt = IsBeingBuilt())
			AddBuildPseudoInitiator(initiator, workId);
	}

	if (isBeingBuilt) {
		// Seperate handling to avoid posting a delayed call while m_buildDoneMutex 
		// is locked (possible deadlock otherwise).

		BUILD_MESSAGE_CURRENTLY_BEING_BUILT(workId, GetSource());
		return true;
	}
	else {
		SetIsBeingBuilt();
		if (initiator)
			SetBuildInitiator(initiator);
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////

bool Script::IsBuildInProgressQuery (void) {
	bool b;
	{ boost::mutex::scoped_lock lock(m_buildDoneMutex); b = IsBeingBuilt(); }
	return b;
}

/////////////////////////////////////////////////////////////////////////

bool Script::IsUpToDateCalculation (void) {

	boost::mutex::scoped_lock buildLock(m_buildMutex);
	DASSERT(!IsBuildInProgressQuery());

	SaveSource();

	return	IsUpToDateCalculationWithDependencies(ExtractDependencies())	&&
			IsByteCodeUpToDate()											&&
			AreLastBuildPropertiesSameAsCurrent();
}

/////////////////////////////////////////////////////////////////////////

bool Script::IsUpToDateCalculationWithDependencies (const StringList& deps) {
	if (deps.empty())
		return true;
	else {
		ScriptPtrList scripts;
		StdStringList externalDeps;
		if (!ResolveDependencies(deps, &scripts, &externalDeps, false))
			return false;
		if (!scripts.empty())
			for (ScriptPtrList::iterator i = scripts.begin(); i != scripts.end(); ++i)
				if (!(*i)->IsUpToDateCalculation())
					return false;
		return true;
	}
}

/////////////////////////////////////////////////////////////////////////
// Ensure that if script is open in an editor it is saved before compilation.

void Script::SaveSource (void) {
	if (ComponentRegistry::Instance().GetFocusedInstance("EditorManager"))
		if (const Handle& editor = Call<const Handle& (const String&)>(this, "EditorManager", "GetEditor")(GetURI())) 
			if (Call<bool (void)>(this, editor, "IsModified")())
				Call<bool (void)>(this, editor, "Save")();
}

/////////////////////////////////////////////////////////////////////////
// AUTOMATIC POST-BUILD ACTIONS FOR DEPLOYMENT READINESS.

bool Script::CopyExternalByteCodeFilesAtWorkingDirectory (const StdStringList& files) {
	BOOST_FOREACH(const std::string& file, files)
		if (!CopyExternalByteCodeFileAtWorkingDirectory(file))
			return false;
	return true;
}

void Script::DeleteExternalByteCodeFilesFromWorkingDirectory (const StdStringList& files, bool postMessage) {
	BOOST_FOREACH(const std::string& file, files)
		DeleteExternalByteCodeFileFromWorkingDirectory(file, postMessage);
}

//******************************
// Requires 'at', 'from', 'wdir'.

#define	COPY_DEPENDENCY_HANDLER(_err, _msg, _cond)												\
	if (m_deploymentDeps.find(at) != m_deploymentDeps.end()) /* Is already copied dep? */		\
		if (m_deploymentDeps[at] != from) {	/* Copied from another dbc at the same target? */	\
			_err(GetSource(), from, wdir, m_deploymentDeps[at]);								\
			return false;																		\
		}																						\
		else {																					\
			DASSERT(boost::filesystem::exists(at));												\
			return true;																		\
		}																						\
	else {																						\
		if (CopyDeploymentByteCodeFile(from, at) _cond)											\
			_msg(GetSource(), from, wdir);														\
		m_deploymentDeps[at] = from;															\
		return true;																			\
	}

bool Script::CopyExternalByteCodeFileAtWorkingDirectory (const std::string& from) {

	std::string name = util::str2std(wxFileName(util::std2str(from)).GetFullName());
	std::string wdir = util::str2std(GetWorkingDirectory());
	std::string at	 = wdir + "/" + name;

	COPY_DEPENDENCY_HANDLER(
		BUILD_ERROR_CONFLICTING_EXTERNAL_DEPEDENCY_AT_WORKING_DIRECTORY,
		BUILD_MESSAGE_COPY_EXTERNAL_DEPEDENCY_AT_WORKING_DIRECTORY,
		EMPTY
	);
}

//******************************

void Script::DeleteExternalByteCodeFileFromWorkingDirectory (const std::string& from, bool postMessage) {

	std::string name = util::str2std(wxFileName(util::std2str(from)).GetFullName());
	std::string wdir = util::str2std(GetWorkingDirectory());
	std::string at	 = wdir + "/" + name;

	// Delete only when its original location is not the same as the
	// working dir of the script.

	if (!wxFileName(util::std2str(from)).SameAs(wxFileName(util::std2str(at))) && boost::filesystem::exists(at)) {
		boost::filesystem::remove(at);
		if (postMessage)
			BUILD_MESSAGE_DELETE_EXTERNAL_DEPEDENCY_FROM_WORKING_DIRECTORY(at);
	}
}

//******************************

bool Script::CopyByteCodeFileOfScriptAtWorkingDirectory (const Script* script) {

	std::string from = script->GetProducedByteCodeFileFullPath();
	std::string wdir = util::str2std(GetWorkingDirectory());
	std::string at	 = wdir + "/" + script->GetProducedByteCodeFile();

	COPY_DEPENDENCY_HANDLER(
		BUILD_ERROR_CONFLICTING_DEPEDENCY_AT_WORKING_DIRECTORY,
		BUILD_MESSAGE_COPY_DEPEDENCY_AT_WORKING_DIRECTORY,
		&& script != this
	);
}

//******************************

void Script::DeleteByteCodeFileOfScriptFromWorkingDirectory (const Script* script, bool postMessage) {
	std::string at = util::str2std(GetWorkingDirectory()) + "/" + script->GetProducedByteCodeFile();
	if (boost::filesystem::exists(at)) {
		boost::filesystem::remove(at);
		if (script != this && postMessage)
			BUILD_MESSAGE_DELETE_DEPEDENCY_FROM_WORKING_DIRECTORY(at);
	}
}

//******************************

bool Script::RecursiveCopyByteCodeFilesAtWorkingDirectory (const ScriptPtrList& scripts, const StdStringList& externalDeps) {
	if (!CopyExternalByteCodeFilesAtWorkingDirectory(externalDeps))
		return false;
	BOOST_FOREACH(const Script* script, scripts)
		if (!CopyByteCodeFileOfScriptAtWorkingDirectory(script) || 
			!RecursiveCopyByteCodeFilesAtWorkingDirectory(script->m_buildDepsRetained, script->m_externalBuildDepsRetained))
			return false;
	return true;
}

//******************************

void Script::RecursiveDeleteByteCodeFilesFromWorkingDirectory (const ScriptPtrList& scripts, const StdStringList& externalDeps, bool postMessage) {
	DeleteExternalByteCodeFilesFromWorkingDirectory(externalDeps, postMessage);
	BOOST_FOREACH(const Script* script, scripts) {
		DeleteByteCodeFileOfScriptFromWorkingDirectory(script, postMessage);
		RecursiveDeleteByteCodeFilesFromWorkingDirectory(script->m_buildDepsRetained, script->m_externalBuildDepsRetained, postMessage);
	}
}

/////////////////////////////////////////////////////////////////////////

unsigned long Script::BuildImpl (const UIntList& workId, Script* initiator) {

	boost::mutex::scoped_lock buildLock(m_buildMutex);

	timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(OnResourceWorkStarted, this, BUILD_TASK_ID, workId));

	if (IsBuildInProgressHandler(workId, initiator))
		return NO_COMPILER_THREAD_PID;

	std::string type, dir;
	if (!(type = "output",  CreateDirectory(dir = util::str2std(GetOutputDirectory()))) || 
		!(type = "working", CreateDirectory(dir = util::str2std(GetWorkingDirectory())))) {
		BUILD_ERROR_DIRECTORY_CREATION_FAILED(GetSource(), type, dir);
		SetBuildCompleted(false, false);
		return NO_COMPILER_THREAD_PID;
	}

	InitialiseNewBuildProcess(workId);

	StringList deps = ExtractDependencies();
	if (!deps.empty())
		BuildWithDependencies(deps);
	else
	if (!IsByteCodeUpToDate())
		 LaunchCompiler();
	else
	if (!AreLastBuildPropertiesSameAsCurrent())
		{ BUILD_MESSAGE_PROJECT_SETTINGS_CHANGED(GetSource()); LaunchCompiler(); }
	else {
		BUILD_MESSAGE_SCRIPT_IS_UPTODATE(GetSource());
		SetBuildCompleted(true, false);
	}

	return NO_COMPILER_THREAD_PID;
}

/////////////////////////////////////////////////////////////////////////

EXPORTED_FUNCTION(Script, unsigned long, Build, (const UIntList& workId)) 
	{ return BuildImpl(workId); }

/////////////////////////////////////////////////////////////////////////

void Script::SetRunAutomaticallyAfterBuild (const std::string& func) 
	{ m_runAutomatically = true, m_runFunction  = func; }

void Script::ResetRunAutomaticallyAfterBuild (void) 
	{ m_runAutomatically = false; m_runFunction.clear(); }

bool Script::IsRunAutomaticallyAfterBuild (void) 
	{ return m_runAutomatically; }

//******************************

bool Script::IsLegalRunFunction (const std::string& func) const {
	return	func == "RunConsole"	||
			func == "DebugConsole"	||
			func == "DebugConsoleWithConsoleDebugger";
}

//******************************

void Script::RunCommit (void) {

	DASSERT(IsRunAutomaticallyAfterBuild() && IsLegalRunFunction(m_runFunction));

	std::string dbc = GetProducedByteCodeFileFullPath();
	if (boost::filesystem::exists(dbc))
		if (m_runFunction == "RunConsole")
			Call<void (const String&, const String&, const String&)>(this, "DeltaVM",  "RunConsole")(
				util::std2str(dbc), 
				_T(""), 
				GetWorkingDirectory()
			);
		else
			Call<void (const String&, const String&, const String&, const String&)>(this, "DeltaVM", m_runFunction)(
				util::std2str(dbc), 
				GetURI(),
				_T(""), 
				GetWorkingDirectory()
			);

	ResetRunAutomaticallyAfterBuild();
}

//******************************

void Script::RunImpl (const std::string& func) {
	if (!IsApplication())
		gui::displayMessage(
			0,													
			_("Error"),											
			String(_("Can't run since deployment type of script is not 'application'."))
		);
	else
	if (!IsRunAutomaticallyAfterBuild()) {

		Call<void (void), SafeCall>(this, "Output", "Clear")();
		Call<void (void), SafeCall>(this, "BuildOrder", "Clear")();

		SaveSource();
		SetRunAutomaticallyAfterBuild(func);

		if (!IsUpToDateCalculation()) 
			Build(UIntList(1, 1));
		else
			RunCommit();
	}
}

/////////////////////////////////////////////////////////////////////////

}	// ide

//-----------------------------------------------------------------------
