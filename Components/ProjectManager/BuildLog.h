#ifndef SCRIPT_UTILS_H
#define SCRIPT_UTILS_H
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include "Script.h"
#include <windows.h>
#include "Common.h"
#include "VirtualContainer.h"


#define __BL bl::buildLog

using namespace std;

namespace bl{

	class BuildLog;
	class script;

	typedef map<string,bool> KeyMap;
	typedef map<string,script> ScriptMap;


	extern BuildLog buildLog;
	extern string endl;

//----------------------------
	class BuildLog{

	public:

		ofstream debugStream;

		
		BuildLog();
		
		void				add							(const string &name, const String &dsc, const string &dbc, const string &type, const StringList &deps);
		void				addAspects					(const string &byte, const StringList &deps);
		void				updateDirectoryInformation	(const string &name, const String &sourcePath, const string &bytecodePath);
		void				save						(void);
		void				read						(const String &path, const String &name);
		void				updateBytecode				(const string &name);
		bool				isScriptUpToDate			(const string &name);

	private:

		string				currentWorkspaceLogPath;
		string				currentWorkspace;
		ScriptMap			script_map;		
		vector<string>		build_order;
		string				debugFile;
		string				logFile;


		void				print_map							(void);
		
		bool				isFileUpToDate						(const string &file);
		bool				isScriptUpToDate					(const script &sc);
		
		void				markOutOfDate						(void);
		void				saveLog								(void);
		void				readLog								(void);
		void				order								(void);
		void				markOutOfDateRecursively			(const KeyMap &children);

	protected:

	};
//----------------------------
	class script{

	public:

		string				type;
		string				dsc;
		string				dbc;
		string				outputDirectory;
		string				workingDirectory;
		string				name;
		string				test_dsc;
		string				test_dbc;
		time_t				m_dsc;
		time_t				m_dbc;
		KeyMap				uses;
		KeyMap				usedby;
		bool				dirty;

		const script& operator=(const script& s) 
				{ new (this) script(s); return *this; }

		script (void) :
			m_dsc	(0),
			m_dbc	(0),
			dirty	(true)
			{}

		script (const script& s) :
			type				(s.type),
			dsc					(s.dsc),
			dbc					(s.dbc),
			m_dsc				(s.m_dsc),
			m_dbc				(s.m_dbc),
			uses				(s.uses),
			usedby				(s.usedby),
			dirty				(s.dirty),
			outputDirectory		(s.outputDirectory),
			workingDirectory	(s.workingDirectory),
			name				(s.name),
			test_dsc			(s.test_dsc),
			test_dbc			(s.test_dbc)
			{}
				
	};

	template <typename T> BuildLog& operator <<(BuildLog& log, T const& value) {
		log.debugStream << value << std::flush;
		return log;
	}
	
}

#endif