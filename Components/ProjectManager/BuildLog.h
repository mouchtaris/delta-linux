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


#define __BL bl::buildLogSingleton

using namespace std;
using ide::Script;

namespace bl{

	class BuildLog;
	class script;

	extern BuildLog buildLogSingleton;
	extern string endl;

//----------------------------
	class BuildLog{

	public:

		ofstream debugStream;

		
		BuildLog();
		
		void add(String name,string byte,string,StringList deps);
		void addAspects(string byte,StringList deps);
		
		void save();
		void read();
		void updateBytecode(string);
		bool isScriptUpToDate(string file);

	private:

		void print_map();
		map<string,script> script_map;		
		vector<string> build_order;
		bool isFileUpToDate(string file);
		bool isScriptUpToDate(script sc);
		string debugFile;
		string logText;
		string sanityFile;
		string logFile;
		void getDirty();
		map<int,map<string,bool> > build_map;
		void save_log();
		void save_log_text();
		void read_log();
		void order();
		void propagateDirty(map<string,bool>);

	protected:

	};
//----------------------------
	class script{

	public:
		script();
		string type;
		string dsc;
		string dbc;
		time_t m_dsc;
		time_t m_dbc;
		map<string,bool> uses;
		map<string,bool> usedby;
		bool dirty;
	private:		
		
	};

	template <typename T> BuildLog& operator <<(BuildLog& log, T const& value) {
		log.debugStream << value << std::flush;
		return log;
	}
	string Canonicalize(string str);
}




#endif