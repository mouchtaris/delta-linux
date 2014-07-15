// BuildLog.cpp
// Script build filtering supporting logged dependencies.
// George Diakostavrianos, 2014
// Dimploma project under Anthony Savidis (with severe refactoring too)
// 

#ifndef PROJECT_MANAGER_BUILD_LOG_H
#define PROJECT_MANAGER_BUILD_LOG_H

///////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>

#include "Script.h"
#include "Common.h"
#include "VirtualContainer.h"
#include <boost/thread/mutex.hpp>

///////////////////////////////////////////////////////////////////////

struct LoggedScript {	// POD

	typedef	std::map<std::string, bool>	StdStringBag;

	std::string				type;
	std::string				dsc;
	std::string				dbc;
	std::string				name;
	time_t					dscTimeStamp;
	time_t					dbcTimeStamp;
	StdStringBag			uses;
	StdStringBag			usedby;
	StdStringList			external;
	bool					dirty;

	const LoggedScript& operator=(const LoggedScript& s) 
			{ new (this) LoggedScript(s); return *this; }

	LoggedScript (void) :
		dscTimeStamp	(0),
		dbcTimeStamp	(0),
		dirty			(true)
		{}

	LoggedScript (const LoggedScript& s) :
		type			(s.type),
		dsc				(s.dsc),
		dbc				(s.dbc),
		dscTimeStamp	(s.dscTimeStamp),
		dbcTimeStamp	(s.dbcTimeStamp),
		uses			(s.uses),
		usedby			(s.usedby),
		dirty			(s.dirty),
		name			(s.name),
		external		(s.external)
		{}
};

///////////////////////////////////////////////////////////////////////

class BuildLog {	// singleton

	private:
	BuildLog (void);
	~BuildLog(){}

	typedef LoggedScript::StdStringBag				StdStringBag;
	typedef	 std::map<std::string, LoggedScript>	ScriptMap;

	static BuildLog*			inst;
	std::ofstream				debugStream;

	private:
	std::string					currentWorkspaceLogPath;
	std::string					currentWorkspace;
	ScriptMap					logScriptMap;		
	std::vector<std::string>	buildOrder;
	std::string					debugFile;
	std::string					logFile;
	mutable boost::mutex		resourceMutex;
	bool						enabled;

	const std::string			GenerateWorkspaceLogFullFilename	(const String& path, const String& name) const;
	bool						IsScriptUpToDate					(const LoggedScript& sc) const;
	
	void						MarkOutOfDate						(void);
	void						SavePriv							(void) const;
	void						LoadPriv							(void);
	void						Order								(void);
	void						MarkOutOfDateRecursively			(const StdStringBag& children);
	void						AddDependenciesPriv	(
									const std::string&		name, 
									const StdStringList&	deps
								);

	///////////////////////////////////////////////////////////////////////

	public:	
	template <typename T>  BuildLog& operator <<(T const& value) {
		debugStream << value << std::flush;
		return *this;
	}

	static BuildLog&	GetSingleton (void);

	void				Add (
							const std::string&		name, 
							const std::string&		dsc, 
							const std::string&		dbc, 
							const std::string&		type, 
							const StdStringList&	deps, 
							const StdStringList&	external
						);
	void				AddDependencies	(
							const std::string&		name, 
							const StdStringList&	deps
						);
	void				UpdateDirectoryInformation (
							const std::string&		name, 
							const std::string&		sourcePath, 
							const std::string&		bytecodePath
						);
	void				UpdateBytecode				(const std::string& name);

	bool				IsScriptUpToDate			(const std::string& name) const;
	bool				IsEnabled					(void) const;
	bool				LogExists					(const String& path, const String& name) const;

	void				Save						(void) const;
	void				Load						(const String& path, const String& name);

	//----------------------------------
	//for future gui delete log action. arguments same as Read() on file Workspace.cpp
	//----------------------------------

	const std::string&	GetLastWorkspaceLogPath		(void) const;
	void				DeleteBuildLog				(const String& path, const String& name);

	//----------------------------------
};

///////////////////////////////////////////////////////////////////////

#endif	// PROJECT_MANAGER_BUILD_LOG_H