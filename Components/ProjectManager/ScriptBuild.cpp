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
#include "ComponentFactory.h"
#include "ComponentFunctionCallerSafe.h"
#include "Call.h"
#include "DelayedCaller.h"
#include "ConsoleHost.h"
#include "IDEDialogs.h"

#include <boost/range.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#include <wx/filename.h>
#include <wx/utils.h>

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
#define	STAGE_SOURCES_PROPERTY_TAG			"StageSource"
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
		"' (is up-to-date)!"												\
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

	boost::mutex			Script::s_componentCallMutex;
	boost::mutex			Script::s_allScriptsMutex;
	Script::ScriptPtrList*	Script::s_allScripts					= (ScriptPtrList*) 0;
	Script::UpToDateMap*	Script::s_upToDate						= (UpToDateMap*) 0;

	Script::VisitMap*		Script::s_visitMap						= (VisitMap*) 0;		// Used only on cyclic reference path detection
	Script::VisitMap*		Script::s_visitMapProduceCyclicPath		= (VisitMap*) 0;		// Used only on producing the cyclic path
	Script::VisitMap*		Script::s_upToDateVisitMap				= (VisitMap*) 0;		// Used only on producing the cyclic path

	const Script*			Script::s_cleanStarter		= (Script*) 0;
	unsigned				Script::s_buildNesting		= 0;
	bool					Script::s_compilerActive	= false;

/////////////////////////////////////////////////////////////////////////
	
	EXPORTED_SIGNAL(Script, ScriptSourceAdded, (const Handle& stageSource, const StringList& lineMappings, const String& type, uint index));
	EXPORTED_STATIC_SIGNAL(Script, CompileFinished, (const Handle& script));

/////////////////////////////////////////////////////////////////////////

Script::Script(void) : 
	m_buildInitiator				((Script*) 0), 
	m_buildDepsSucceeded			(false), 
	m_someDepsCompiled				(false),
	m_lastBuildPropertiesInvalid	(false),
	m_beingBuilt					(false),
	m_runAutomatically				(false),
	m_isApplication					(false),
	m_isCleaned						(false),
	m_upToDate						(false),
	m_debugBuild					(false),
	m_buildCanceled					(false),
	m_buildDepsResolved				(false) {

	MakeAllProperties();
	{
		boost::mutex::scoped_lock lock(s_allScriptsMutex);
		s_allScripts->push_back(this);
	}
}

Script::~Script(void) {
	TerminateAllLaunchedCompilers();
	{
		boost::mutex::scoped_lock lock(s_allScriptsMutex);
		DASSERT(std::find(s_allScripts->begin(), s_allScripts->end(), this) != s_allScripts->end());
		s_allScripts->remove(this);
	}
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

static bool FileSystemExists(const std::string& file) {
	try {
		const String path = util::std2str(file);
		return wxFileName::FileExists(path) || wxFileName::DirExists(path);
	}
	catch(...) { return false; }
}

static void RemoveIfExists (const std::string& file) {
	if (FileSystemExists(file))
		try { boost::filesystem::remove(file); }
		catch(...) { /*ignore filesystem errors */ }
}

static bool CreateDirectory (const std::string& dir) {
	if (!FileSystemExists(dir)) {
		try { boost::filesystem::create_directories(dir); }
		catch(...) { /*ignore filesystem errors */ }
		return FileSystemExists(dir);
	}
	else
		return true;
}

static bool	CopyDeploymentByteCodeFile (const std::string& from, const std::string& at) {
	try {
		if (!FileSystemExists(at))
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
	catch(...) { return false; }	//to catch any filesystem errors
}

/////////////////////////////////////////////////////////////////////////

void Script::SaveLastBuildProperties (void) {

	xml::Node root;
	root.Create(_T(SCRIPT_TAG));
	conf::XMLValueSaver propertySaver(conf::MAKE_PATHS_RELATIVE, GetPath());
	propertySaver.SetParent(root);
	m_lastBuildProperties.Accept(LAST_BUILT_PROPERTIES_TAG, &propertySaver);
	
	uint count = 0;
	BOOST_FOREACH(conf::PropertyTable& table, m_stageSources)
		table.Accept(std::string(STAGE_SOURCES_PROPERTY_TAG) + boost::lexical_cast<std::string>(count++), &propertySaver);
	root.SetProperty(_T("StageSources"), boost::lexical_cast<String>(count));

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

	xml::Node root(doc.GetRoot());

	conf::XMLValueLoader propertyLoader;
	propertyLoader.SetParent(root);
	m_lastBuildProperties.Accept(LAST_BUILT_PROPERTIES_TAG, &propertyLoader);
	m_lastBuildPropertiesInvalid = false;

	const uint stageSources = atoi(util::str2std(root.GetProperty(_T("StageSources"))).c_str());
	for (uint i = 0; i < stageSources; ++i) {
		conf::PropertyTable source;
		conf::AddScriptStageSourceProperties(&source);
		source.Accept(std::string(STAGE_SOURCES_PROPERTY_TAG) + boost::lexical_cast<std::string>(i), &propertyLoader);
		const String name = conf::get_prop_value<conf::StringProperty>(source.GetProperty("name"));
		const String type = conf::get_prop_value<conf::StringProperty>(source.GetProperty("type"));
		uint index = conf::get_prop_value<conf::IntProperty>(source.GetProperty("index"));
		bool final = conf::get_prop_value<conf::BoolProperty>(source.GetProperty("final"));

		StringList lineMappings;
		conf::AggregateListProperty* prop = static_cast<conf::AggregateListProperty*>(source.GetProperty("lineMappings"));
		BOOST_FOREACH(conf::AggregateProperty* p, prop->GetPropertyList()) {
			const String original = wxString::Format(_T("%d"), conf::get_prop_value<conf::IntProperty>(p->GetProperty("original")));
			const String mapped = conf::get_prop_value<conf::StringListProperty>(p->GetProperty("mapped"));
			lineMappings.push_back(original + _T(":") + mapped);
		}

		AddSource(name, lineMappings, StringList(), type, index, final); //source refs not stored in prop file
	}
	GenerateFinalLineMappings();
}

/////////////////////////////////////////////////////////////////////////

EXPORTED_FUNCTION(Script, bool, Load, (const String& uri)) {
	bool result = GenericFile::Load(uri);

	conf::Property* output = const_cast<conf::Property*>(GetInstanceProperty("output"));
	if (output && conf::get_prop_value<conf::StringProperty>(output, _T("")).empty()) {
		String name = GetName();
		name = name.substr(0, name.find_last_of(_T(".")));
		conf::set_prop_value<conf::StringProperty>(output, name);
	}

	RefreshDeploymentPropertyValue();
	LoadLastBuildProperties();
	return result;
}

/////////////////////////////////////////////////////////////////////////

EXPORTED_FUNCTION(Script, const std::string, GetType, (void)) { return GetClassId(); }

/////////////////////////////////////////////////////////////////////////

EXPORTED_FUNCTION(Script, const UIntList, GetWorkId, (void))
{
	return m_workId;
}

EXPORTED_FUNCTION(Script, uint, NextWorkSerial, (void))
{
	return ++m_currentWorkSerial;
}

EXPORTED_FUNCTION(Script, const std::string, GetCurrBuildFunc, (void))
{
	return m_debugBuild ? "DebugBuild" : "Build";
}

/////////////////////////////////////////////////////////////////////////

bool Script::DirectlyDependsOn (const Script* script) const {
	DASSERT(m_buildDepsResolved);
	return	this == script || 
			std::find(m_buildDeps.begin(), m_buildDeps.end(), script) !=  m_buildDeps.end();
}

bool Script::DependsOnRecursion (const Script* from, const Script* to) {

	if (from->DirectlyDependsOn(to))
		return true;

	(*s_visitMap)[const_cast<Script*>(from)] = true;

	for (ScriptPtrSet::const_iterator i =  from->m_buildDeps.begin(); i !=  from->m_buildDeps.end(); ++i)
		if (s_visitMap->find(*i) == s_visitMap->end() && to->m_buildDepsResolved && DependsOnRecursion(*i, to))
			return true;
	return false;
}

/////////////////////////////////////////////////////////////////////////

bool Script::DependsOn (const Script* script) const {

	DASSERT(s_visitMap->empty());

	bool result = DependsOnRecursion(this, script);
	s_visitMap->clear();

	return result;
}

/////////////////////////////////////////////////////////////////////////

EXPORTED_FUNCTION(Script, const HandleList, FindScriptsOfUsedByteCodeFile, (const String& fileName, uint stage)) {
	
	String bytecode_path;
	if (!stage)
		bytecode_path = GetByteCodeLoadingPathPropertyValue();
	else {
		using namespace conf;
		const AggregateListProperty* stageSourcesOptions = safe_prop_cast<const AggregateListProperty>(
			GetInstanceProperty("stage_sources_options")
		);
		DASSERT(stageSourcesOptions);

		const AggregateProperty* defaultStage = (const AggregateProperty*) 0;
		bool found = false;
		BOOST_FOREACH(const AggregateProperty* p, stageSourcesOptions->GetPropertyList()) {
			const uint s = get_prop_value<IntProperty>(p->GetProperty("stage"));
			if (!s)
				defaultStage = p;
			else if (s == stage) {
				bytecode_path = get_path_prop_value<const DirectoryListProperty>(
					 p->GetProperty(BYTECODE_PATH_PROPERTY_ID),
					 String()
				);
				found = true;
				break;
			}
		}
		if (!found && defaultStage)
			bytecode_path = get_path_prop_value<const DirectoryListProperty>(
				defaultStage->GetProperty(BYTECODE_PATH_PROPERTY_ID),
				String()
			);
	}

	StringList dirs;
	util::stringtokenizer(dirs, bytecode_path, _T(";"));

	ScriptPtrList scripts;
	BOOST_FOREACH(const String& dir, dirs) {
		const String fullPath = MakeAbsolutePath(dir, GetPath()) + _T("/") + fileName;
		GetAllScriptsProducingByteCodeFileFullPath(util::str2std(fullPath), &scripts);
	}
	
	HandleList handles;
	BOOST_FOREACH(Script* script, scripts)
		handles.push_back(Handle(script));
	return handles;
}

/////////////////////////////////////////////////////////////////////////
// Produces path this -> target when the dep initiates from start.

const std::string Script::ProcuceCyclicDependencyPathString (const Script* target, const Script* start) const {
	VisitMap& vs = *s_visitMapProduceCyclicPath;
	std::string curr = GetSource();
	if (this == target)		// Reach destination.
		if (this == start)	// Cycle is closed.
			return curr;
		else {				// We found path to target, so we now seek for the path to start.
			vs.clear();		// So that we can go back target->start even on nodes over start->target
			return ProcuceCyclicDependencyPathString(start, start);
		}
	else {
		for (ScriptPtrSet::const_iterator i = m_buildDeps.begin(); i != m_buildDeps.end(); ++i)
			if (vs.find(*i) == vs.end() && DependsOnRecursion(*i, target)) {
				vs[*i] = true;
				return curr + "->" + (*i)->ProcuceCyclicDependencyPathString(target, start);
			}
		DASSERT(false);
		return "";
	}
}

/////////////////////////////////////////////////////////////////////////

const String Script::GetByteCodeLoadingPathPropertyValue (void) const 
	{ return conf::get_path_prop_value<const conf::DirectoryListProperty>(GetInstanceProperty(BYTECODE_PATH_PROPERTY_ID), String()); }

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
	const String output = conf::get_prop_value<conf::StringProperty>(GetInstanceProperty("output"), _T(""));
	DASSERT(!output.empty());
	return util::str2std(output + _T(".dbc"));
}

const String Script::GetProducedBuildInfoFileFullPath (void) const {
	const String name = const_cast<Script*>(this)->GetName();
	return const_cast<Script*>(this)->GetOutputDirectory() + name.substr(0, name.find_last_of(_T("."))) + _T(".dbi");
}

EXPORTED_FUNCTION(Script, const std::string, GetProducedByteCodeFileFullPath, (void))
{
	return util::str2std(GetOutputDirectory()) + GetProducedByteCodeFile();
}

EXPORTED_FUNCTION(Script, const std::string, GetTransformedFileFullPath, (uint transformationNo))
{
	String name = GetName();
	name = name.substr(0, name.find_last_of(_T(".")));
	const String path = GetDirectoryProperty("stage_output_path");	//if transformations are placed elsewhere change this
	return util::str2std(path + name + _T("_aspect_") + boost::lexical_cast<String>(transformationNo) + _T(".dsc"));
}

/////////////////////////////////////////////////////////////////////////

EXPORTED_FUNCTION(Script, const String, GetFinalSourceURI, (void))
{
	Script* target = this;
	Component::List children = GetChildren();
	while (!children.empty()) {
		target = static_cast<Script*>(children.back());
		children = target->GetChildren();
	}
	return target->GetURI();
}

/////////////////////////////////////////////////////////////////////////

EXPORTED_FUNCTION(Script, const StringList, GetFinalLineMappings, (void))
{
	return m_finalLineMappings;
}

/////////////////////////////////////////////////////////////////////////
// Produces a full path list taking the dbc file and appending it 
// to every bytecode path (of the script props), the run working dir and the output dir.
// This concerns only the dbc that relate to some script entry.

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
	boost::mutex::scoped_lock lock(s_allScriptsMutex);
	for (ScriptPtrList::iterator i = s_allScripts->begin(); i != s_allScripts->end(); ++i)
		if ((*i)->GetClassId() != "StageSource" && wxFileName(util::std2str((*i)->GetProducedByteCodeFileFullPath())).SameAs(util::std2str(fullPath)))
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
		try {
			if (!result || boost::filesystem::last_write_time(result->GetSource()) < boost::filesystem::last_write_time((*i)->GetSource())) 
				result = *i;
		}
		catch(...) { /*ignore filesystem errors */ }
	return result;
}

/////////////////////////////////////////////////////////////////////////

void Script::ResolveAspectTransformations (void) {
	StringVec projects;
	const conf::Property* p = GetInstanceProperty("aspects");
	if (!p || (projects = static_cast<const conf::StringListProperty*>(p)->GetValues()).empty())
		return;

	std::string func;
	const std::string type = GetType();
	if (type == "Script" || type == "Aspect")	//normal script entry
		func = "GetPreTransformations";
	else if (type == "stage")					//else check for stage sources
		func = "GetInterimTransformations";
	else if (type == "result")
		func = "GetPostTransformations";
	else {
		DASSERT(type == "aspect");
		return;
	}

	const Handle& wsp = Call<const Handle& (void)>(this, treeview, "GetWorkspace")();
	BOOST_FOREACH(const String& project, projects) {
		const Handle proj = Call<Handle (const String&)>(this, wsp, "GetChild")(project);
		if (proj && proj.GetClassId() == "AspectProject") {
			const HandleList aspects = Call<const HandleList (void)>(this, proj, func)();
			BOOST_FOREACH(const Handle& handle, aspects) {
				DASSERT(handle.GetClassId() == "Aspect");
				m_aspectTransformations.insert(static_cast<Script*>(handle.Resolve()));
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////

void Script::BuildSelf (void) {
	bool upToDate = IsByteCodeUpToDate();
	if (upToDate && !m_stageSources.empty())
		BOOST_FOREACH(const Handle& source, CollectChildren(_T("StageSource"))) {
			Script* s = static_cast<Script*>(source.Resolve());
			bool shouldCheck =	s->GetType() != "result" && s->GetType() != "aspect" ||
								conf::get_prop_value<conf::BoolProperty>(s->GetInstanceProperty("final"), false);
			s_upToDate->clear();
			if (shouldCheck && !s->IsUpToDateCalculation()) {
				upToDate = false;
				break;
			}
		}

	if (!upToDate)
		BuildSelfImpl();
	else
	if (AreSomeDependenciesCompiled()) 
		{ BUILD_MESSAGE_DEPENDENCIES_RECOMPILED(GetSource());  BuildSelfImpl(); }
	else
	if (!AreLastBuildPropertiesSameAsCurrent())
		{ BUILD_MESSAGE_PROJECT_SETTINGS_CHANGED(GetSource()); BuildSelfImpl(); }
	else {
		BUILD_MESSAGE_SCRIPT_IS_UPTODATE(GetSource());
		SetBuildCompleted(true, false);
	}
}

/////////////////////////////////////////////////////////////////////////

void Script::BuildSelfImpl (void) {
	if (m_aspectTransformations.empty())
		LaunchCompiler();
	else
		BuildWithTransformations();
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
	options += _T(" --stage_output_path=") + util::quotepath(GetDirectoryProperty("stage_output_path"));

	const String output = conf::get_prop_value<conf::StringProperty>(GetInstanceProperty("output"), _T(""));
	if (!output.empty())
		options += _T(" --output=") + output;

	String bytecodePath = GetByteCodeLoadingPath();

	if (!bytecodePath.empty())
		options += _T(" --bytecode_path=") + util::quotepath(bytecodePath);

	//Select the first one to place stage sources under the original source 
	//and the second one to place them under the transformed aspect files.
	//TODO: for the first to be correct a new original (parent) source is required in the compiler launch
	//Script* original = GetClassId() == "StageSource" && 
	//	conf::get_prop_value<conf::EnumStringProperty>(GetInstanceProperty("treeCtrl_icon"), String()) == _T("aspect") ?
	//	static_cast<Script*>(GetParent().Resolve()) : this;
	Script* original = this;

	options += _T(" --symbolic_names=\"") + original->GetSymbolicURI() + _T("\"");

	const String directory = GetWorkingDirectory();	// Compiler invocation dir is now the working dir of the script
	//The following commented line shows the previous behaviour where the invocation dir was that of the container project
	//const String directory = Call<const String (void)>(this, TreeItemComponent::GetParent(), "GetPath")();

	if (m_workId.empty()) {
		Call<void (const String&, const String&, const String&)>(this, "DeltaCompiler", "CompileInternal")(
			GetURI(), 
			options,
			directory
		);
		timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(OnResourceWorkCompleted, this, BUILD_TASK_ID, m_workId));
	}
	else {
		boost::mutex::scoped_lock callLock(s_componentCallMutex);
		boost::mutex::scoped_lock compLock(m_compMutex);
		
		if (m_debugBuild)
			while (s_compilerActive)
				wxMilliSleep(100);
		s_compilerActive = true;

		std::string comp = m_debugBuild ? "DeltaVM" : "DeltaCompiler";
		std::string func = m_debugBuild ? "DebugCompilerInvocation" : "Compile";
		unsigned long compilerPid = Call<unsigned long (const String&, const String&, const String&, const UIntList&, const Handle&)>
			(this, comp, func)(GetURI(), options, directory, m_workId, this);
		
		if (compilerPid) {
			CleanStageSources();
			SetCompilerPid(compilerPid);
		}
		else
			SetBuildCompleted(false, false);
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
		boost::bind(&Script::PostBuildOutput, this, workId, _T("Error"), util::std2str(error), GetSymbolicURI())
	);
}

//********************************

void Script::PostBuildMessage (const UIntList& workId, const std::string& message) {
	timer::DelayedCaller::Instance().PostDelayedCall(
		boost::bind(&Script::PostBuildOutput, this, workId, _T("Message"), util::std2str(message + "\n"), GetSymbolicURI())
	);
}

//********************************

void Script::PostBuildWarning (const UIntList& workId, const std::string& warning) {
	timer::DelayedCaller::Instance().PostDelayedCall(
		boost::bind(&Script::PostBuildOutput, this, workId, _T("Warning"), util::std2str(warning), GetSymbolicURI())
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
			for (ScriptPtrSet::iterator i = m_buildDeps.begin(); i != m_buildDeps.end(); ++i)
				(*i)->TerminateAllLaunchedCompilers();	// Recursive termination of compiler processes.
		else {											// Script without remaining dependency, thus should kill its compiler thread.
			DASSERT(m_allBuildPids.size() == ONLY_ONE_COMPILER_LAUNCHED_PIDS);
			DASSERT(m_allBuildPids.front() != NO_COMPILER_THREAD_PID);
			util::ConsoleHost::TerminateProcess(m_allBuildPids.front());
		}
	}
	else if (m_beingBuilt)
		ClearBuildInformation();
}

/////////////////////////////////////////////////////////////////////////

EXPORTED_FUNCTION(Script, const ScriptDependencies, GetDependencies, (void)) {
	
	ScriptPtrSet outDeps;
	StdStringList externalDeps;
	bool ok = ResolveDependencies(ExtractDependencies(), &outDeps, &externalDeps, false);

	HandleList scripts;
	StringList external;

	BOOST_FOREACH(Script* script, outDeps)
		scripts.push_back(script);
	BOOST_FOREACH(const std::string str, externalDeps)
		external.push_back(util::std2str(str));

	return ScriptDependencies(ok, scripts, external);
}

/////////////////////////////////////////////////////////////////////////

EXPORTED_FUNCTION(Script, const StringList, ExtractDependencies, (void)) {
	StringList deps;
	{
		boost::mutex::scoped_lock lock(s_componentCallMutex);
		deps = Call<StringList (const String&, const String&)>(const_cast<Script*>(this), "DeltaVM", "ExtractBuildDependencies")(
			const_cast<Script*>(this)->GetURI(), 
			GetByteCodeLoadingPath()
		);
	}
	DASSERT(deps.size() % 2 == 0);
	std::set<String> uris;
	for (StringList::iterator i = deps.begin(); i != deps.end(); /*empty*/)
		if (uris.find(*i) == uris.end()) {
			uris.insert(*i);
			++i;	//skip uri
			++i;	//skip status
		}
		else {
			i = deps.erase(i);	//erase uri
			i = deps.erase(i);	//erase status
		}
	return deps;
}

/////////////////////////////////////////////////////////////////////////

bool Script::ResolveDependencies (
		const StringList&	deps,
		ScriptPtrSet*		outDeps,
		StdStringList*		externalDeps,
		bool				outputErrors
	) {

	DASSERT(deps.size() % 2 == 0 && (outDeps == &m_buildDeps || !outputErrors));
	bool dependencyErrors = false;
	m_buildDepsResolved = true;

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
					outDeps->insert(newDep = GetScriptWithMostRecentSource(producers));
					BUILD_WARNING_MULTIPLE_SOURCES(dbc, newDep->GetSource());
				}
				else {
					outDeps->insert(newDep = producers.front());
					newDep->m_buildDepsResolved = true;
				}

				if (newDep == this)
					BUILD_ERROR_CYCLIC_SELF_DEPENDENCY(GetSource());
				else
				if (newDep->DependsOn(this)) {	// Cyclic dependency!
					DASSERT(s_visitMapProduceCyclicPath->empty());
					std::string path = ProcuceCyclicDependencyPathString(newDep, this);
					s_visitMapProduceCyclicPath->clear();
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
		for (ScriptPtrSet::iterator i = m_buildDepsRetained.begin(); i != m_buildDepsRetained.end(); ++i, ++workSerial)  {
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
		if (GetWorkingDirectory() != GetOutputDirectory())
			DeleteByteCodeFileOfScriptFromWorkingDirectory(this);	// Delete self copy from the wdir.
	}
}

/////////////////////////////////////////////////////////////////////////

void Script::RecursiveCreateDependencyInformation(void) {
	ResolveDependencies(ExtractDependencies(), &m_buildDepsRetained, &m_externalBuildDepsRetained, false);
	for (ScriptPtrSet::iterator i = m_buildDepsRetained.begin(); i != m_buildDepsRetained.end(); ++i)
		(*i)->RecursiveCreateDependencyInformation();
}

void Script::RecursiveClearDependencyInformation (void) {
	for (ScriptPtrSet::iterator i = m_buildDepsRetained.begin(); i != m_buildDepsRetained.end(); ++i)
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

void Script::BuildWithTransformations (void) {

	StringList aspects;
	DASSERT(!m_aspectTransformations.empty());
	BOOST_FOREACH(const Script* s, m_aspectTransformations)
		aspects.push_back(util::std2str(const_cast<Script*>(s)->GetProducedByteCodeFileFullPath()));

	String options;
	//TODO: need to have single invocations per transformation script
	//This means that we can use all aspect script properties in the AspectCompiler invocation
	//(e.g. working dir, bytecode and dllimport paths)
	//
	//const String bytecodePath = GetByteCodeLoadingPath();
	//if (!bytecodePath.empty())
	//	options += _T(" --bytecode_path=") + util::quotepath(bytecodePath);
	//const String dllimportpath = GetDllImportPath();
	//if (!dllimportpath.empty())
	//	options += _T(" --dllimport_path=") + util::quotepath(bytecodePath);

	options += _T(" --symbolic=\"") + GetSymbolicURI() + _T("\"");

	//transformations are stage sources and thus are produced in the sage_output_path
	//if we want them to be produced elsewhere add a new script property and use it here.
	options += _T(" --output_path=") + util::quotepath(GetDirectoryProperty("stage_output_path"));

	const String directory = Call<const String (void)>(this, TreeItemComponent::GetParent(), "GetPath")();

	bool debug = m_debugBuild || GetClassId() == "StageSource" && static_cast<Script*>(GetParent().Resolve())->m_debugBuild;
	const std::string comp = debug ? "DeltaVM" : "DeltaCompiler";
	const std::string func = debug ? "DebugAspectCompilerInvocation" : "AspectTransformation";
	unsigned long pid = Call<unsigned long (const String&, const StringList&, const String&, const String&, const UIntList&, const Handle&)>
		(this, comp, func)(GetURI(), aspects, options, directory, m_workId, this);
	if (pid) {
		CleanStageSources();
		SetCompilerPid(pid);
	}
	else
		SetBuildCompleted(false, false);
}

/////////////////////////////////////////////////////////////////////////

void Script::BuildWithUsingDependencies (const StringList& usingDeps) {
	boost::mutex::scoped_lock lock(m_buildDepsMutex);
	if (!ResolveDependencies(usingDeps, &m_buildDeps, &m_externalBuildDepsRetained, true)) {
		m_externalBuildDepsRetained.clear();
		BUILD_ERROR_FAILED_TO_RESOLVE_ALL_DEPENDENCIES(GetSource());
		SetBuildCompleted(false, false);
		m_upToDate = true;						// We artificially set it up-to-date so that we do not rebuilt in this session.
	}
	else 
		BuildWithScriptDependencies(m_buildDeps);
}

/////////////////////////////////////////////////////////////////////////

void Script::BuildWithScriptDependencies (const ScriptPtrSet& deps) {

	DASSERT(!deps.empty());
	m_buildDepsRetained = m_buildDeps = deps;

	ScriptPtrSet	toBuild(m_buildDeps);	// Should copy since during build may asynchronously remove.
	m_buildDepsSucceeded = true;			// We assume they have all succceeded.

	for (ScriptPtrSet::iterator i = toBuild.begin(); i != toBuild.end(); ++i) {
		UIntList workId = m_workId;
		workId.push_back(NextWorkSerial());
		(*i)->BuildImpl(workId, false, this);	//Do not recursively debug the dependency build
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

EXPORTED_FUNCTION(Script, bool, IsByteCodeUpToDate, (void))
{
	const std::string source = GetSource();
	const std::string binary = GetProducedByteCodeFileFullPath();
	bool retval = FileSystemExists(source) && FileSystemExists(binary);
	try { retval = retval && boost::filesystem::last_write_time(binary) >= boost::filesystem::last_write_time(source); }
	catch(...) { retval = false; }
	return retval;
}

EXPORTED_FUNCTION(Script, bool, AreSourceTransformationsUpToDate, (void))
{
	if (m_aspectTransformations.empty())
		return true;
	else {
		const std::string source = GetSource();
		const std::string lastTransformation = GetTransformedFileFullPath(m_aspectTransformations.size());
		bool retval = FileSystemExists(source) && FileSystemExists(lastTransformation);
		try { retval = retval && boost::filesystem::last_write_time(lastTransformation) >= boost::filesystem::last_write_time(source); }
		catch(...) { retval = false; }
		return retval;
	}
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
	m_debugBuild			= false;
	m_buildDepsResolved		= false;

	m_workId.clear();
	m_allBuildPids.clear();
	m_buildDeps.clear();
	m_pseudoInitiators.clear();
	m_aspectTransformations.clear();

	ResetIsBeingBuilt();	// Always last since it may cause parallelism problems.
}

/////////////////////////////////////////////////////////////////////////

void Script::SetBuildCompleted (bool succeeded, bool wasCompiled) {
	
	boost::mutex::scoped_lock lock(m_buildDoneMutex);

	if (succeeded && GetClassId() == "Script")
		GenerateFinalLineMappings();

	if (wasCompiled)	//even when compilation fails intermediate files may be generated requiring a clean
		m_isCleaned = false;

	bool updatedProperties = false;
	if (succeeded && wasCompiled)
		if (!IsApplication() || (succeeded = CopyByteCodeFileOfScriptAtWorkingDirectory(this)))	{ // Copy self at wdir.
			UpdateLastBuildProperties();
			updatedProperties = true;
		}

	if (!updatedProperties)
		UpdateLastBuildProperties();

	timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(OnResourceWorkCompleted, this, BUILD_TASK_ID, m_workId));

	unsigned long pid = wasCompiled ? m_allBuildPids.front() : NO_COMPILER_THREAD_PID;

	if (Script* initiator = GetBuildInitiator())
		if (initiator->m_beingBuilt)
			initiator->SetInitiatedBuildIsCompleted(this, pid, succeeded);

	for (PseudoInitiators::iterator i = m_pseudoInitiators.begin(); i != m_pseudoInitiators.end(); ++i) {
		timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(OnResourceWorkCompleted, this, BUILD_TASK_ID, i->second));
		if (i->first && i->first->m_beingBuilt)
			i->first->SetInitiatedBuildIsCompleted(this, NO_COMPILER_THREAD_PID, succeeded);
	}

	ClearBuildInformation();
	m_upToDate = succeeded;

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
	m_currentWorkSerial = 0;
	m_workId = workId;
	m_buildDepsRetained.clear();
	m_externalBuildDepsRetained.clear();
	m_deploymentDeps.clear();
	m_buildCanceled = false;
}

/////////////////////////////////////////////////////////////////////////

void Script::UpdateLastBuildProperties (void) {

	for (const char** p = conf::GetScriptLastBuildPropertyIds(); *p; ++p) {
		conf::Property* curr = GetInstancePropertyTable().GetProperty(*p);	// Normal build property.
		DASSERT(curr);
		m_lastBuildProperties.AddProperty(*p, curr->Clone());	// Last build property.
	}
	m_lastBuildPropertiesInvalid = false;
	SaveLastBuildProperties();
}

//********************************
	
bool Script::AreLastBuildPropertiesSameAsCurrent (void) const {

	if (m_lastBuildPropertiesInvalid)
		return false;
	else {
		for (const char** p = conf::GetScriptLastBuildPropertyIds(); *p; ++p) {

			conf::Property* curr = GetInstancePropertyTable().GetProperty(*p);	// Normal build property.
			DASSERT(curr);

			conf::Property* last = m_lastBuildProperties.GetProperty(*p);		// Last build property.
			DASSERT(last);

			if (!curr->Equal(last))
				return false;
		}
		return true;
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

	m_buildDeps.erase(script);

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
				timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(&Script::BuildSelf, this));	//Delayed so as to release any dependency mutexes
		else {
			BUILD_ERROR_FAILED_WHILE_BUILDING_DEPENDENCIES(GetSource());
			SetBuildCompleted(false, false);
		}
	}
}

/////////////////////////////////////////////////////////////////////////

void Script::ResetUpToDate (void) {
	boost::mutex::scoped_lock lock(s_allScriptsMutex);
	for (ScriptPtrList::iterator i = s_allScripts->begin(); i != s_allScripts->end(); ++i)
		(*i)->m_upToDate = false;
}

//*******************************

EXPORTED_SLOT_MEMBER(
		Script, 
		void, 
		OnWorkCanceled, 
		(const std::string& caller, const Handle& root, const String& task), 
		"WorkCanceled"
	) { 
	if (caller == "Workspace") {
		m_buildCanceled = true;
		TerminateAllLaunchedCompilers(); 
		ResetUpToDate();
		m_isCleaned = false;
		s_buildNesting = 0;
	}
}

//*******************************

EXPORTED_SLOT_MEMBER(
		Script, 
		void, 
		OnWorkCompleted, 
		(const std::string& caller, const Handle& root, const String& task), 
		"WorkCompleted"
	){
	if (caller == "Workspace") {
		ResetUpToDate();
		s_buildNesting = 0;
	}
}

//*******************************

EXPORTED_SLOT_MEMBER(
		Script, 
		void, 
		OnWorkStarted, 
		(const std::string& caller, const Handle& root, const String& task), 
		"WorkStarted"
	){
	if (caller == "Workspace") {
		DASSERT(!s_buildNesting);
		ResetUpToDate();	// Just to be sure it is in correct state
	}
}

/////////////////////////////////////////////////////////////////////////

static bool HaveSameWorkId (const UIntList& id1, const UIntList& id2) {
	if (id1.size() != id2.size())
		return false;
	else {
		UIntList::const_iterator i, j;
		for (i = id1.begin(), j = id2.begin(); i != id1.end(); ++i, ++j)
			if (*i != *j)
				return false;
		return true;
	}
}

EXPORTED_SLOT_MEMBER(Script, void, OnResourceWorkCompleted, (const Handle& resource,
	const String& task, const UIntList& workId), "ResourceWorkCompleted")
{
	Component *comp = resource.Resolve();
	if (m_beingBuilt && !m_buildCanceled && comp && task == BUILD_TASK_ID && HaveSameWorkId(workId, m_lastTransformationBuildId)) {
		m_lastTransformationBuildId.clear();
		sigCompileFinished(this);
		SetBuildCompleted(static_cast<Script*>(comp)->IsByteCodeUpToDate(), true);
	}
}

/////////////////////////////////////////////////////////////////////////

EXPORTED_SLOT_MEMBER(Script, void, OnCompileFinished, (const std::string& invoker, const Handle& script), "CompileFinished") {

	if (invoker != s_classId && script.Resolve() == this && !m_buildCanceled) {	//skip notifications from deferred self compilations

		boost::mutex::scoped_lock lock(m_compMutex);
		DASSERT(!m_workId.empty() && m_allBuildPids.size() == ONLY_ONE_COMPILER_LAUNCHED_PIDS && m_buildDeps.empty());

		s_compilerActive = false;
		SetBuildCompleted(
			IsByteCodeUpToDate(),	// When the dbc is up-to-date the compilation finished with no errors.
			true					// And we have always launched the compiler.
		);
	}
}

/////////////////////////////////////////////////////////////////////////

EXPORTED_SLOT_MEMBER(Script, void, OnTransformationFinished, (const std::string& invoker, const Handle& script), "TransformationFinished") {

	if (script.Resolve() == this && !m_buildCanceled) {
		if (AreSourceTransformationsUpToDate()) {
			const Handle lastTransformation = this->GetChildByURI(util::std2str(GetTransformedFileFullPath(m_aspectTransformations.size())));
			DASSERT(lastTransformation);

			m_lastTransformationBuildId = m_workId;		
			m_lastTransformationBuildId.push_back(NextWorkSerial());
			const std::string func = m_debugBuild ? "DebugBuild" : "Build";
			Call<void (const UIntList&)>(this, lastTransformation, func)(m_lastTransformationBuildId);
		}
		else
			SetBuildCompleted(false, false);
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

		if (!s_cleanStarter)
			s_cleanStarter = this;

		CleanDependencies(workId);
		CleanStageSources();

		const std::string dbc = GetProducedByteCodeFileFullPath();
		if (FileSystemExists(dbc)) {
			boost::mutex::scoped_lock lock(s_componentCallMutex);
			Call<void (const String&)>(this, "DeltaCompiler", "Clean")(util::std2str(dbc));
		}

		const std::string dbi = util::str2std(GetProducedBuildInfoFileFullPath());
		RemoveIfExists(dbi);

		if (s_cleanStarter == this) {
			RecursiveClearDependencyInformation();	// Clears across the entire graph.
			s_cleanStarter = (Script*) 0;
		}

		m_isCleaned = true;
	}

	timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(OnResourceWorkCompleted, this, CLEAN_TASK_ID, workId));
	return NO_COMPILER_THREAD_PID;
}

/////////////////////////////////////////////////////////////////////////

static const String AdaptPath(const String& path, const String& originalPath, const String& targetPath) {
	wxFileName filename(path);
	if (filename.IsRelative()) {
		filename.MakeAbsolute(originalPath);
		filename.MakeRelativeTo(targetPath);
	}
	return util::normalizeslashes(filename.GetFullPath());
}

//********************************

template<class T>
void AdaptFileOrDirectoryPath(T* p, const String& originalPath, const String& targetPath) {
	p->SetValue(AdaptPath(p->GetValue(), originalPath, targetPath));
}
template<class T>
void AdaptFileOrDirectoryListPath(T* p, const String& originalPath, const String& targetPath) {
	StringVec paths = p->GetValues();
	for (StringVec::iterator i = paths.begin(); i != paths.end(); ++i)
		*i = AdaptPath(*i, originalPath, targetPath);
	p->SetValues(paths);
}

//********************************

static conf::Property* AdaptFileOrDirectoryProperty(conf::Property* p, const String& originalPath, const String& targetPath) {
	switch(p->GetType()) {
		case conf::FileProperty::Type:
			AdaptFileOrDirectoryPath(static_cast<conf::FileProperty*>(p), originalPath, targetPath);
			break;

		case conf::DirectoryProperty::Type:
			AdaptFileOrDirectoryPath(static_cast<conf::DirectoryProperty*>(p), originalPath, targetPath);
			break;

		case conf::FileListProperty::Type:
			AdaptFileOrDirectoryListPath(static_cast<conf::FileListProperty*>(p), originalPath, targetPath);
			break;

		case conf::DirectoryListProperty::Type:
			AdaptFileOrDirectoryListPath(static_cast<conf::DirectoryListProperty*>(p), originalPath, targetPath);
			break;
	}
	return p;
}

/////////////////////////////////////////////////////////////////////////

EXPORTED_FUNCTION(Script, const Handle, AddSource, (const String& file, const StringList& lineMappings, 
	const StringList& sourceRefs, const String& type, uint index, bool isFinal))
{
	Component* item = ComponentFactory::Instance().CreateComponent("StageSource");
	DASSERT(item);
	item->SetParent(this);

	conf::EnumStringProperty* iconProp = static_cast<conf::EnumStringProperty*>(item->GetClassProperty("treeCtrl_icon")->Clone());
	iconProp->SetValue(type);
	iconProp->SetVisible(false);
	item->AddInstanceProperty("treeCtrl_icon", iconProp);

	conf::StringListProperty* p = new conf::StringListProperty(_T("sourceRefs"), _T("Source References"));
	p->SetValues(StringVec(sourceRefs.begin(), sourceRefs.end()));
	p->SetVisible(false);
	item->AddInstanceProperty("sourceRefs", p);

	p = new conf::StringListProperty(_T("lineMappings"), _T("Line Mappings"));
	p->SetValues(StringVec(lineMappings.begin(), lineMappings.end()));
	p->SetVisible(false);
	item->AddInstanceProperty("lineMappings", p);

	conf::BoolProperty* p2 = new conf::BoolProperty(_T("final"), isFinal);
	p2->SetVisible(false);
	item->AddInstanceProperty("final", p2);

	conf::PropertyTable source;
	conf::AddScriptStageSourceProperties(&source);
	conf::set_prop_value<conf::StringProperty>(source.GetProperty("name"), file);
	conf::set_prop_value<conf::StringProperty>(source.GetProperty("type"), type);
	conf::set_prop_value<conf::IntProperty>(source.GetProperty("index"), index);
	conf::set_prop_value<conf::BoolProperty>(source.GetProperty("final"), isFinal);
	conf::AggregateListProperty* prop = static_cast<conf::AggregateListProperty*>(source.GetProperty("lineMappings"));

	BOOST_FOREACH(const String& mapping, lineMappings) {
		StringVec split;
		util::stringtokenizer(split, mapping, _T(":"));
		DASSERT(split.size() == 2);
		conf::AggregateProperty* p = prop->NewProperty();
		conf::set_prop_value<conf::IntProperty>(p->GetProperty("original"), atoi(util::str2std(split[0]).c_str()));
		conf::set_prop_value<conf::StringListProperty>(p->GetProperty("mapped"), split[1]);
	}
	m_stageSources.push_back(source);

	const String path = GetDirectoryProperty("stage_output_path");

	if (type == _T("stage")) {	//generate stage properties
		using namespace conf;
		const AggregateListProperty* stageSourcesOptions = safe_prop_cast<const AggregateListProperty>(
			GetInstanceProperty("stage_sources_options")
		);
		DASSERT(stageSourcesOptions);
		
		const AggregateProperty* matchedStage = (const AggregateProperty*) 0;
		const AggregateProperty* defaultStage = (const AggregateProperty*) 0;
		BOOST_FOREACH(const AggregateProperty* p, stageSourcesOptions->GetPropertyList()) {
			const uint s = get_prop_value<IntProperty>(p->GetProperty("stage"));
			if (!s)
				defaultStage = p;
			else if (index == s) {
				matchedStage = p;
				break;
			}
		}

		if (const AggregateProperty* targetStage = matchedStage ? matchedStage : defaultStage)
			for (const char** p = conf::GetScriptPropertyIdsForStageSources(); *p; ++p)
				if (const conf::Property* prop = targetStage->GetProperty(*p))
					item->AddInstanceProperty(*p, prop->Clone());

		if (const conf::Property* prop = GetInstanceProperty("aspects"))
			item->AddInstanceProperty("aspects", prop->Clone());
	}
	else {	//for stage results and aspect transformations copy script properties
		StdStringList props;
		for (const char** p = conf::GetProjectBuildPropertyIdsForScripts(); *p; ++p)
			props.push_back(*p);
		for (const char** p = conf::GetScriptExecutionPropertyIds(); *p; ++p)
			props.push_back(*p);
		props.push_back("output");
		props.push_back("aspects");

		for (StdStringList::const_iterator i = props.begin(); i != props.end(); ++i)
			if (const conf::Property* prop = GetInstanceProperty(*i))
				item->AddInstanceProperty(*i, AdaptFileOrDirectoryProperty(prop->Clone(), GetPath(), path));
	}

	assert(treeview);
	Call<bool (const Handle&, const Handle&)>(this, treeview, "AddComponent")(this, item);
	Call<void (const String&)>(this, item, "Load")(path + file);
	Call<void (const String&)>(this, item, "SetSymbolicURI")(file);
	Call<void (const Handle&)>(this, treeview, "SortChildren")(this);

	sigScriptSourceAdded(this, item, lineMappings, type, index);
	return Handle(item);
}

/////////////////////////////////////////////////////////////////////////

Script::ScriptPtrList Script::GetAllIntermediateSources (void) {
	ScriptPtrList result;
	BOOST_FOREACH(const conf::PropertyTable& source, m_stageSources) {
		const String type = conf::get_prop_value<conf::StringProperty>(source.GetProperty("type"));
		if (type == _T("aspect") || type == _T("result")) {
			const String symbolic = conf::get_prop_value<conf::StringProperty>(source.GetProperty("name"));
			Component* child = GetChildBySymbolicURI(symbolic).Resolve();
			DASSERT(child && child->GetClassId() == "StageSource");
			Script *s = static_cast<Script*>(child);
			result.push_back(s);
			if (conf::get_prop_value<conf::BoolProperty>(source.GetProperty("final")))
				result.splice(result.end(), s->GetAllIntermediateSources());
		}
	}
	return result;
}

/////////////////////////////////////////////////////////////////////////

typedef std::map< uint, std::set<uint> > LineMappings;
static const LineMappings StringVecToLineMappings(const StringVec& encodedMappings) {
	LineMappings lineMappings;
	BOOST_FOREACH(const String& str, encodedMappings) {
		StringVec split;
		util::stringtokenizer(split, str, _T(":"));
		DASSERT(split.size() == 2);
		std::set<uint>& s = lineMappings[atoi(util::str2std(split[0]).c_str())];

		const String mapped = split[1];
		split.clear();
		util::stringtokenizer(split, mapped, _T(";"));
		BOOST_FOREACH(const String& line, split)
			s.insert(atoi(util::str2std(line).c_str()));
	}
	return lineMappings;
}

static const StringList LineMappingsToStringList(const LineMappings& lineMappings) {
	StringList result;
	for (LineMappings::const_iterator i = lineMappings.begin(); i != lineMappings.end(); ++i) {
		String str = boost::lexical_cast<String>(i->first);
		for (std::set<uint>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
			str += (j == i->second.begin() ? _T(":") : _T(";")) + boost::lexical_cast<String>(*j);
		result.push_back(str);
	}
	return result;
}

void Script::GenerateFinalLineMappings(void) {
	LineMappings lineMappings;
	BOOST_FOREACH(Script* s, GetAllIntermediateSources()) {
		const conf::StringListProperty* p =
			static_cast<const conf::StringListProperty *>(s->GetInstanceProperty("lineMappings"));
		const LineMappings curr = StringVecToLineMappings(p->GetValues());
		if (lineMappings.empty())
			lineMappings = curr;
		else {
			LineMappings prev = lineMappings;
			lineMappings.clear();
			for (LineMappings::const_iterator i = prev.begin(); i != prev.end(); ++i)
				for (std::set<uint>::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
					LineMappings::const_iterator iter1 = curr.find(*j);
					if (iter1 != curr.end())
						for (std::set<uint>::const_iterator iter2 = iter1->second.begin(); iter2 != iter1->second.end(); ++iter2)
							lineMappings[i->first].insert(*iter2);
				}
		}
	}
	m_finalLineMappings = LineMappingsToStringList(lineMappings);
}

/////////////////////////////////////////////////////////////////////////

void Script::CleanStageSources (void) {
	BOOST_FOREACH(Component* child, GetChildren()) {
		Script* script = static_cast<Script*>(child);
		script->CleanStageSources();
		const std::string dbi = util::str2std(script->GetProducedBuildInfoFileFullPath()); 
		RemoveIfExists(dbi);
	}

	BOOST_FOREACH(const conf::PropertyTable& source, m_stageSources) {
		const String uri = GetDirectoryProperty("stage_output_path") +
			conf::get_prop_value<conf::StringProperty>(source.GetProperty("name"));
		const std::string curi = util::str2std(uri);
		RemoveIfExists(curi);
		if (conf::get_prop_value<conf::StringProperty>(source.GetProperty("type")) == _T("stage")) {
			const std::string binary = curi.substr(0, curi.find_last_of(".")) + ".dbc";
			RemoveIfExists(binary);
		}

		timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(&Script::DestroyStageSource, this, uri));
	}
	m_finalLineMappings.clear();
	m_stageSources.clear();
}

/////////////////////////////////////////////////////////////////////////

void Script::DestroyStageSource (const String& uri) {
	Handle child = GetChildByURI(uri);
	Call<bool (const Handle&)>(this, treeview, "RemoveComponent")(child);
	child->Destroy();
}

/////////////////////////////////////////////////////////////////////////

const Script::ScriptPtrSet Script::ExtractStageDependencies (void) const {
	ScriptPtrSet deps;
	BOOST_FOREACH(const conf::PropertyTable& table, m_stageSources) {
		const String type = conf::get_prop_value<conf::StringProperty>(table.GetProperty("type"));
		if (type == _T("stage")) {
			const String symbolic = conf::get_prop_value<conf::StringProperty>(table.GetProperty("name"));
			const Handle script = const_cast<Script*>(this)->GetDirectChildBySymbolicURI(symbolic);
			DASSERT(script);
			deps.insert(static_cast<Script*>(script.Resolve()));
		}
	}
	return deps;
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
// It saves the source, resolves deps (again) and recursively
// calls that every dep is also up-to-date.

bool Script::IsUpToDateCalculation (void) {

	UpToDateMap::iterator i = s_upToDate->find(this);
	if (i != s_upToDate->end())
		return i->second;
	bool result;
	{
		boost::mutex::scoped_lock lock(m_buildDepsMutex);
		DASSERT(m_buildDeps.empty());

		// If already visited assume it is up-to-date, since the actual 
		// up-to-date calculation will get us the correct results
		if (s_upToDateVisitMap->find(this) != s_upToDateVisitMap->end())
			return true;
		(*s_upToDateVisitMap)[this];

		SaveSource();

		if (!IsByteCodeUpToDate() || !AreLastBuildPropertiesSameAsCurrent())
			result = false;
		else {
			std::time_t lastBinWrite = 0;
			try { lastBinWrite = boost::filesystem::last_write_time(GetProducedByteCodeFileFullPath()); }
			catch(...) { /*ignore filesystem errors */ }

			if (GetType() == "stage" && !AreExternalDependenciesUpToDate(lastBinWrite))
				result = false;
			else {
				ResolveAspectTransformations();

				if (m_aspectTransformations.empty())
					result = IsUpToDateCalculationWithUsingDependencies(ExtractDependencies());
				else {
					result = true;
					BOOST_FOREACH(Script* aspect, m_aspectTransformations)
					 if (!aspect->AreExternalDependenciesUpToDate(lastBinWrite)) {
						result = false;
						break;
					}
				}

				m_aspectTransformations.clear();
				m_buildDeps.clear();
				m_buildDepsResolved = false;
			}
		}
	}
	return (*s_upToDate)[this] = result;
}

/////////////////////////////////////////////////////////////////////////

bool Script::IsUpToDateCalculationWithUsingDependencies (const StringList& deps) {
	if (deps.empty())
		return true;
	else {
		StdStringList externalDeps;
		if (!ResolveDependencies(deps, &m_buildDeps, &externalDeps, false))
			return false;
		return IsUpToDateCalculationWithScriptDependencies(m_buildDeps);
	}
}

/////////////////////////////////////////////////////////////////////////

bool Script::IsUpToDateCalculationWithScriptDependencies (const ScriptPtrSet& deps) {
	for (ScriptPtrSet::const_iterator i = deps.begin(); i != deps.end(); ++i)
		if (!(*i)->IsUpToDateCalculation())
			return false;
	return true;
}

/////////////////////////////////////////////////////////////////////////

bool Script::AreExternalDependenciesUpToDate(std::time_t timestamp) {
	if (const conf::FileListProperty* p = conf::safe_prop_cast<const conf::FileListProperty>(GetInstanceProperty("extra_deps"))) {
		BOOST_FOREACH(const String& dep, p->GetExpandedValues()) {
			const std::string fullPath = util::str2std(MakeAbsolutePath(dep, GetWorkingDirectory()));
			try {
				if (!FileSystemExists(fullPath) || timestamp < boost::filesystem::last_write_time(fullPath))	//bin less recent that dep
					return false;
			}
			catch(...) { /*ignore filesystem errors */ }
		}
	}
	return true;
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
			DASSERT(FileSystemExists(at));														\
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

	const std::string name	= util::str2std(wxFileName(util::std2str(from)).GetFullName());
	const std::string wdir	= util::str2std(GetWorkingDirectory());
	const std::string at	= wdir + "/" + name;

	// Delete only when its original location is not the same as the
	// working dir of the script.

	if (!wxFileName(util::std2str(from)).SameAs(wxFileName(util::std2str(at))) && FileSystemExists(at)) {
		RemoveIfExists(at);
		if (postMessage)
			BUILD_MESSAGE_DELETE_EXTERNAL_DEPEDENCY_FROM_WORKING_DIRECTORY(at);
	}
}

//******************************

bool Script::CopyByteCodeFileOfScriptAtWorkingDirectory (const Script* script) {

	std::string from = const_cast<Script*>(script)->GetProducedByteCodeFileFullPath();
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
	if (FileSystemExists(at)) {
		RemoveIfExists(at);
		if (script != this && postMessage)
			BUILD_MESSAGE_DELETE_DEPEDENCY_FROM_WORKING_DIRECTORY(at);
	}
}

//******************************

bool Script::RecursiveCopyByteCodeFilesAtWorkingDirectory (const ScriptPtrSet& scripts, const StdStringList& externalDeps) {
	if (!CopyExternalByteCodeFilesAtWorkingDirectory(externalDeps))
		return false;
	BOOST_FOREACH(const Script* script, scripts)
		if (!CopyByteCodeFileOfScriptAtWorkingDirectory(script) || 
			!RecursiveCopyByteCodeFilesAtWorkingDirectory(script->m_buildDepsRetained, script->m_externalBuildDepsRetained))
			return false;
	return true;
}

//******************************

void Script::RecursiveDeleteByteCodeFilesFromWorkingDirectory (const ScriptPtrSet& scripts, const StdStringList& externalDeps, bool postMessage) {
	DeleteExternalByteCodeFilesFromWorkingDirectory(externalDeps, postMessage);
	BOOST_FOREACH(const Script* script, scripts) {
		DeleteByteCodeFileOfScriptFromWorkingDirectory(script, postMessage);
		RecursiveDeleteByteCodeFilesFromWorkingDirectory(script->m_buildDepsRetained, script->m_externalBuildDepsRetained, postMessage);
	}
}

/////////////////////////////////////////////////////////////////////////

unsigned long Script::BuildImpl (const UIntList& workId, bool debugBuild, Script* initiator) {

	boost::mutex::scoped_lock buildLock(m_buildMutex);

	timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(OnResourceWorkStarted, this, BUILD_TASK_ID, workId));

	if (m_upToDate) {
		m_workId = workId;
		SetIsBeingBuilt();
		if (initiator)
			SetBuildInitiator(initiator);
		SetBuildCompleted(true, false);
		return NO_COMPILER_THREAD_PID;
	}

	if (IsBuildInProgressHandler(workId, initiator))
		return NO_COMPILER_THREAD_PID;

	//TODO: This should not be called if the script is up-to-date.
	//However, m_upToDate is cleared across builds.
	//Maybe it should be a part of BuildSelfImpl so that it is only invoked upon actuall compilation of THIS script.
	//The following code is to implement the PreBuildEvents.
	//
	//const std::string application = "ECHO hello;ECHO hello;";
	//util::ConsoleHost console(true);
	//console(application, util::str2std(GetPath()), boost::bind(&Script::PostBuildMessage, this, workId, _1));

	//if (!console.Succeeded()) {
	//	SetBuildCompleted(false, false);
	//	return NO_COMPILER_THREAD_PID;
	//}
	//

	std::string type, dir;
	if (!(type = "output",  CreateDirectory(dir = util::str2std(GetOutputDirectory()))) || 
		!(type = "stage output",  CreateDirectory(dir = util::str2std(GetDirectoryProperty("stage_output_path")))) || 
		!(type = "working", CreateDirectory(dir = util::str2std(GetWorkingDirectory())))) {
		BUILD_ERROR_DIRECTORY_CREATION_FAILED(GetSource(), type, dir);
		SetBuildCompleted(false, false);
		return NO_COMPILER_THREAD_PID;
	}

	InitialiseNewBuildProcess(workId);
	m_debugBuild = debugBuild;

	ResolveAspectTransformations();
	if (!m_aspectTransformations.empty())
		BuildWithScriptDependencies(m_aspectTransformations);
	else {
		const StringList usingDeps = ExtractDependencies();
		if (usingDeps.empty())
			BuildSelf();
		else
			BuildWithUsingDependencies(usingDeps);
	}
	return NO_COMPILER_THREAD_PID;
}

/////////////////////////////////////////////////////////////////////////

EXPORTED_FUNCTION(Script, unsigned long, Build, (const UIntList& workId)) 
	{ return BuildImpl(workId); }

/////////////////////////////////////////////////////////////////////////

EXPORTED_FUNCTION(Script, unsigned long, DebugBuild, (const UIntList& workId)) 
{
	boost::thread(boost::bind(&Script::BuildImpl, this, workId, true, (Script*)0));
	return NO_COMPILER_THREAD_PID;
}

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
	if (FileSystemExists(dbc))
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

		s_upToDate->clear();
		s_upToDateVisitMap->clear();

		SetRunAutomaticallyAfterBuild(func);
		if (!IsUpToDateCalculation())
			BuildCtx();
		else
			RunCommit();
	}
}

/////////////////////////////////////////////////////////////////////////

}	// ide

//-----------------------------------------------------------------------
