/**
 *	Script.h
 *
 *	-- IDE Profile Manager Component --
 *
 *	Script component is the component representing
 *	a script files in the Project Manager's tree view.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
 */
#ifndef PROJECT_MANAGER_SCRIPT_H_
#define PROJECT_MANAGER_SCRIPT_H_

#include "Common.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"
#include "ProjectManagerFuncReturnTypes.h"
#include "TextFile.h"

#include <boost/thread/mutex.hpp>
#include <set>
#include <list>

namespace ide
{
	class Script :
		public TextFile
	{
		DECLARE_IDE_COMPONENT(Script);

	public:
		///--- type definitions

		///--- constructors / destructor
		Script(void);
		~Script(void);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));

		DECLARE_EXPORTED_FUNCTION_(virtual bool, Load, (const String& uri),
			_("Load file from disk"));

		DECLARE_EXPORTED_STATIC(const std::string&, GetDebuggerId, (void));

		DECLARE_EXPORTED_FUNCTION(void, Run, (void));
		DECLARE_EXPORTED_FUNCTION(void, Debug, (void));
		DECLARE_EXPORTED_FUNCTION(void, DebugWithConsoleDebugger, (void));

		DECLARE_EXPORTED_FUNCTION(void, BuildCtx, (void));
		DECLARE_EXPORTED_FUNCTION(unsigned long, Build, (const UIntList& workId));
		DECLARE_EXPORTED_FUNCTION(void, CleanCtx, (void));
		DECLARE_EXPORTED_FUNCTION(unsigned long, Clean, (const UIntList& workId));

		DECLARE_EXPORTED_FUNCTION(void, EnableWorkCommands, (void));
		DECLARE_EXPORTED_FUNCTION(void, DisableWorkCommands, (void));

		DECLARE_EXPORTED_FUNCTION(void, SetStartupScript, (void));
		DECLARE_EXPORTED_FUNCTION(bool, Rename, (const String& newName));

		DECLARE_EXPORTED_FUNCTION(const String, GetWorkingDirectory, (void));
		DECLARE_EXPORTED_FUNCTION(const String, GetOutputDirectory, (void));
		
		DECLARE_EXPORTED_FUNCTION(void, OnFileNameChanged, (const Handle& editor, const String& uri));
		DECLARE_EXPORTED_FUNCTION(void, OnCompileFinished, (const std::string& compiler, const Handle& script));
		DECLARE_EXPORTED_FUNCTION(void, OnLibraryDefinitionsChanged, (const std::string& classId, const StringList newDefinitions));

		DECLARE_EXPORTED_FUNCTION(void, OnWorkStarted, (const std::string& caller, const Handle& root, const String& task));
		DECLARE_EXPORTED_FUNCTION(void, OnWorkCanceled, (const std::string& caller, const Handle& root, const String& task));
		DECLARE_EXPORTED_FUNCTION(void, OnWorkCompleted, (const std::string& caller, const Handle& root, const String& task));

		DECLARE_EXPORTED_FUNCTION(const HandleList, FindScriptsOfUsedByteCodeFile, (const String& fileName));

		DECLARE_EXPORTED_FUNCTION(const ScriptDependencies, GetDependencies, (void));

	protected:
		///--- protected API
		virtual void ComponentAppliedChangedProperties(const conf::PropertyTable& old, const conf::PropertyIdVec& changed);
		virtual void ComponentAddedProperty (const std::string& id, conf::Property* prop);

	private:
		///--- private API
		void HandleNewProperties(const conf::PropertyIdVec& changed);
		const String ExtractFuncFilesStr(void);
		const String GetDirectoryProperty(const std::string& property) const;
		const StringList GetFileLibraryFunctions(void) const;
		const conf::Property* GetDeltaVMProperty(const std::string& id) const;
		const String GetLibraryPath(const String& lib) const;

		static void OnResourceWorkStarted(Component *component, const String& task, const UIntList& workId);
		static void OnResourceWorkCompleted(Component *component, const String& task, const UIntList& workId);
		static void OnCompilationMessage(const UIntList& buildId, const String& content, const String& file);

		///--- private members
		UIntList							m_workId;
		static boost::mutex					s_mutex;

		// For handling build dependencies.
		typedef std::list<Script*>					ScriptPtrList;
		typedef std::list<const Script*>			ConstScriptPtrList;
		typedef std::list<unsigned long>			PidList;
		typedef std::pair<Script*, UIntList>		PseudoInitiator;
		typedef std::list<PseudoInitiator>			PseudoInitiators;
		typedef std::map<std::string, std::string>	DeploymentDeps;
		typedef std::map<Script*, bool>				UpToDateMap;
		typedef std::map<Script*, bool>				VisitMap;

		struct MatchScriptPred : public std::binary_function<PseudoInitiator, const Script*, bool> {
			bool operator()(const PseudoInitiator& p, const Script* script) const
				{ return p.first == script; }
		};

		static UpToDateMap*					s_upToDate;
		static VisitMap*					s_visitMap;
		static VisitMap*					s_visitMapProduceCyclicPath;
		static ScriptPtrList*				s_allScripts;
		static unsigned						s_buildNesting;
		ScriptPtrList						m_buildDeps;
		bool								m_buildDepsResolved;
		ScriptPtrList						m_buildDepsRetained;
		StdStringList						m_externalBuildDepsRetained;
		DeploymentDeps						m_deploymentDeps;
		bool								m_buildDepsSucceeded;
		bool								m_someDepsCompiled;
		boost::mutex						m_buildMutex;
		boost::mutex						m_compMutex;
		boost::mutex						m_buildDoneMutex;
		boost::mutex						m_cleanMutex;
		PidList								m_allBuildPids;
		Script*								m_buildInitiator;
		conf::PropertyTable					m_lastBuildProperties;
		bool								m_lastBuildPropertiesInvalid;
		bool								m_beingBuilt;
		bool								m_runAutomatically;
		std::string							m_runFunction;
		PseudoInitiators					m_pseudoInitiators;
		bool								m_isApplication;
		bool								m_isCleaned;
		bool								m_upToDate;
		static const Script*				m_cleanStarter;

		static void							ResetUpToDate (void);
		Script*								GetBuildInitiator (void);
		void								SetBuildInitiator (Script* script);
		bool								HasBuildPseudoInitiators (void) const;
		bool								IsBuildPseudoInitiator (const Script* script) const;
		void								AddBuildPseudoInitiator (Script* script, const UIntList& workdId);
		void								ResetBuildInitiator (void);
		bool								HaveAllDependenciesSucceeded (void) const;
		bool								AreSomeDependenciesCompiled  (void) const;
		bool								IsByteCodeUpToDate (void) const;
		void								SetIsApplication (bool val);
		bool								IsApplication (void) const;
		void								CheckDeploymentPropertyChanged (const conf::PropertyIdVec& changed);
		void								RefreshDeploymentPropertyValue (void);
		const std::string					GetDeploymentPropertyValue (void) const;

		void								SetCompilerPid (unsigned long pid);
		void								AddCompilerPidToBuildInitiator (unsigned long pid);
		void								SetInitiatedBuildIsCompleted (Script* script, unsigned long pid, bool succeeded);
		void								ClearBuildInformation (void);

		bool								IsBeingBuilt (void) const;
		void								ResetIsBeingBuilt (void);
		void								SetIsBeingBuilt (void);
		bool								IsCleaned (void) const 
												{ return m_isCleaned; }

		bool								DependsOn (const Script* script) const;
		bool								DirectlyDependsOn (const Script* script) const;
		static bool							DependsOnRecursion (const Script* from, const Script* to);
		bool								HasAnyCyclicDependencies (const ScriptPtrList& toBuild, ScriptPtrList* putCyclicHere);
		const std::string					ProcuceCyclicDependencyPathString (const Script* target, const Script* start) const;

		const std::string					GetProducedByteCodeFileFullPath (void) const;
		const std::string					GetProducedByteCodeFile (void) const;
		const String						GetProducedBuildInfoFileFullPath (void) const;
		static const String					MakeAbsolutePath (const String& path, const String& dir);
		const String						GetByteCodeLoadingPath (void) const;
		const String						GetByteCodeLoadingPathPropertyValue (void) const;

		const std::string					GetSource (void) const;
		void								GetAllLegalFullPathsForUnknownUsedByteCodeFile (const std::string&	dbc, StdStringList* fullPaths);
		static void							GetAllScriptsProducingByteCodeFileFullPath (const std::string& fullPath, ScriptPtrList* putHere);
		static void							GetAllScriptsProducingUnknownUsedByteCodeFile (
												Script*				caller, 
												const std::string&	dbc, 
												ScriptPtrList*		putHere
											);
		static Script*						GetScriptWithMostRecentSource (const ScriptPtrList& scripts);

		void								UpdateLastBuildProperties (void);
		bool								AreLastBuildPropertiesSameAsCurrent (void) const;
		void								MakeAllProperties (void);
		void								SaveLastBuildProperties (void) const;
		void								LoadLastBuildProperties (void);

		void								LaunchCompiler (void);
		const StringList					ExtractDependencies (void) const;
		bool								ResolveDependencies (
												const StringList&	deps,
												ScriptPtrList*		outDeps,
												StdStringList*		externalDeps,
												bool				outputErrors
											);
		void								CleanDependencies (const UIntList& workId);
		void								BuildWithDependencies (const StringList& deps);
		bool								IsBuildInProgressHandler (const UIntList& workId, Script* initiator);
		bool								IsBuildInProgressQuery (void);
		unsigned long						BuildImpl (const UIntList& workId, Script* initiator = (Script*) 0);
		bool								IsUpToDateCalculation (void);
		bool								IsUpToDateCalculationWithDependencies (const StringList& deps);		
		void								SetBuildCompleted (bool succeeded, bool wasCompiled);
		void								InitialiseNewBuildProcess (const UIntList& workId);
		void								SaveSource (void);

		void								DeleteExternalByteCodeFilesFromWorkingDirectory (const StdStringList& files, bool postMessage = true);
		void								DeleteExternalByteCodeFileFromWorkingDirectory (const std::string& file, bool postMessage = true);
		void								DeleteByteCodeFileOfScriptFromWorkingDirectory (const Script* script, bool postMessage = true);
		void								RecursiveDeleteByteCodeFilesFromWorkingDirectory (const ScriptPtrList& scripts, const StdStringList& externalDeps, bool postMessage = true);

		void								RecursiveClearDependencyInformation (void);
		void								RecursiveCreateDependencyInformation (void);
		void								DeleteCopiedByteCodeFilesFromWorkingDirectory (void);

		bool								CopyExternalByteCodeFilesAtWorkingDirectory (const StdStringList& files);
		bool								CopyExternalByteCodeFileAtWorkingDirectory (const std::string& file);
		bool								CopyByteCodeFileOfScriptAtWorkingDirectory (const Script* script);
		bool								RecursiveCopyByteCodeFilesAtWorkingDirectory (const ScriptPtrList& scripts, const StdStringList& externalDeps);

		void								RunImpl (const std::string& func);
		void								RunCommit (void);
		void								SetRunAutomaticallyAfterBuild (const std::string& func);
		void								ResetRunAutomaticallyAfterBuild (void);
		bool								IsRunAutomaticallyAfterBuild (void);
		bool								IsLegalRunFunction (const std::string& func) const;

		void								TerminateAllLaunchedCompilers (void);

		static void							PostBuildOutput (const UIntList& workId, const String& type, const String& content, const String& file);
		void								PostBuildError (const UIntList& workId, const std::string& error);
		void								PostBuildWarning (const UIntList& workId, const std::string& warning);
		void								PostBuildMessage (const UIntList& workId, const std::string& message);
	};
}

#endif	//PROJECT_MANAGER_SCRIPT
