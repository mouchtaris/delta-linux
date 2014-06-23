#ifndef BUILD_LOG_H
#define BUILD_LOG_H
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include "Script.h"
#include "Common.h"
#include "VirtualContainer.h"
#include <boost/thread/mutex.hpp>

#define __BL bl::buildLog

using namespace std;

namespace bl{

	class		BuildLog;
	class		LogScript;

	typedef		map<string,bool> KeyMap;
	typedef		map<string,LogScript> ScriptMap;


	extern		BuildLog buildLog;
	extern		string endl;

//----------------------------
	class BuildLog{

	public:

		ofstream debugStream;

		
		BuildLog();
		
		void				Add							(const string &name, const string &dsc, const string &dbc, const string &type, const StdStringList &deps, const StdStringList &external);
		void				AddDependencies					(const string &name, const StdStringList &deps);
		void				UpdateDirectoryInformation	(const string &name, const string &sourcePath, const string &bytecodePath);
		void				Save						(void);
		void				Read						(const String &path, const String &name);
		void				UpdateBytecode				(const string &name);
		bool				IsScriptUpToDate			(const string &name);
		bool				IsEnabled					(void);
		bool				LogExists					(const String &path, const String &name);

		//----------------------------------
		//for future gui delete log action. arguments same as Read() on file Workspace.cpp
		//----------------------------------
		string				GetLastWorkspaceLogPath		(void);
		void				DeleteBuildLog				(const String &path, const String &name);
		void				EnableBuildLog				(void);
		void				DisableBuildLog				(void);
		//----------------------------------

	private:

		
		string				currentWorkspaceLogPath;
		string				currentWorkspace;
		ScriptMap			logScriptMap;		
		vector<string>		buildOrder;
		string				debugFile;
		string				logFile;
		boost::mutex		resourceMutex;
		bool				enabled;

		void				PrintMap							(void);
		string				GenerateWorkspaceLogFullFilename	(const String &path, const String &name);
		bool				IsFileUpToDate						(const string &file);
		bool				IsScriptUpToDate					(const LogScript &sc);
		
		void				MarkOutOfDate						(void);
		void				SaveLog								(void);
		void				ReadLog								(void);
		void				Order								(void);
		void				MarkOutOfDateRecursively			(const KeyMap &children);

	protected:

	};
//----------------------------
	class LogScript{

	public:

		string				type;
		string				dsc;
		string				dbc;
		string				name;
		time_t				m_dsc;
		time_t				m_dbc;
		KeyMap				uses;
		KeyMap				usedby;
		StdStringList		external;
		bool				dirty;

		const LogScript& operator=(const LogScript& s) 
				{ new (this) LogScript(s); return *this; }

		LogScript (void) :
			m_dsc	(0),
			m_dbc	(0),
			dirty	(true)
			{}

		LogScript (const LogScript& s) :
			type				(s.type),
			dsc					(s.dsc),
			dbc					(s.dbc),
			m_dsc				(s.m_dsc),
			m_dbc				(s.m_dbc),
			uses				(s.uses),
			usedby				(s.usedby),
			dirty				(s.dirty),
			name				(s.name),
			external			(s.external)
			{}
				
	};

	template <typename T> BuildLog& operator <<(BuildLog& log, T const& value) {
		log.debugStream << value << std::flush;
		return log;
	}
	
}

#endif