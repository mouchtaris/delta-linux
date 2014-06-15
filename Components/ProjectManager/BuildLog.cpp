#include "BuildLog.h"
#include <boost/range.hpp>
#include <boost/foreach.hpp>
#include <wx/utils.h>
#include "VirtualContainer.h"
#include <sys/stat.h>
#include <assert.h>
	
#include <time.h>
#include "Script.h"

#define LOG_ENABLED false

using namespace std;
using ide::Script;

namespace bl{

	BuildLog buildLogSingleton;
	string endl = "\n";

	BuildLog::BuildLog(){
		debugFile = "debug.txt";
		logFile = "log.bin";
		logText = "log.txt";
		sanityFile = "logSanity";
		debugStream = ofstream(debugFile);
	}

	void BuildLog::addAspects(string byte,StringList deps){
		assert(!byte.empty());

		string bt = Canonicalize(byte);
		script_map[bt];
		for (StringList::iterator it = deps.begin(); it!=deps.end();++it){		
			string dep = Canonicalize(util::str2std(*it));
			script_map[dep].usedby[bt]=true;
			script_map[bt].uses[dep]=true;
		}
	}

	void BuildLog::add(String name,string byte,string type,StringList deps){
		assert(!byte.empty());
		assert(!type.empty());
		assert(!name.empty());

		string bt = Canonicalize(byte);
		script_map[bt].dsc =  Canonicalize(util::str2std(name));
		script_map[bt].dbc = bt;
		script_map[bt].type = type;
		for (StringList::iterator it = deps.begin(); it!=deps.end();++it){
			
			string dep = Canonicalize(util::str2std(*it));
			if (dep=="one_found" || dep=="many_found" || dep=="not_found")continue;
			script_map[dep].usedby[bt]=true;
			script_map[bt].uses[dep]=true;
		}
		struct stat fileinfo;			// create a file attribute structure  
		memset(&fileinfo, 0, sizeof(struct stat));
		stat(script_map[bt].dsc.c_str(), &fileinfo);	
		script_map[bt].m_dsc = fileinfo.st_mtime;
	}

	void BuildLog::order(){
		build_order = vector<string>();
		debugStream << "-----ORDER-------" << std::endl;
		for (map<string,script>::iterator it = script_map.begin();it!=script_map.end();++it){
			if(it->second.dirty==false)continue;
			int max=-1;
			for (map<string,bool>::iterator used = it->second.uses.begin();used!=it->second.uses.end();++used){
				vector<string>::iterator index = std::find(build_order.begin(), build_order.end(), used->first);
				if (index!=build_order.end()){
					int at = std::distance(build_order.begin(),index);
					max = at>max?at:max;
				}
			}
			if (max>-1){
				//debugStream << it->first << " is needed at pos " << max << std::endl;
				build_order.insert(build_order.begin()+max,it->first);
			}
			else{
				build_order.insert(build_order.begin(),it->first);
			}
		}
		int i=0;
		for (vector<string>::iterator it = build_order.begin(); it != build_order.end(); ++it,++i){
			debugStream << i << ": " << (*it) << std::endl;
		}
	}

	void BuildLog::save_log_text(){
		ofstream txt(logText);
		for (map<string,script>::iterator it=script_map.begin();it!=script_map.end();++it){
			script s = it->second;
			txt << s.dbc << std::endl;
			txt << "\t" <<  s.dsc << std::endl;
			txt << "\t" <<  s.type << std::endl;
			txt << "\tm_source: " <<  s.m_dsc << std::endl;
			txt << "\tm_byte: " <<  s.m_dbc << std::endl;
			txt << "\tused by: " <<  endl;
			for (map<string,bool>::iterator tt = s.usedby.begin(); tt!=s.usedby.end();++tt){
				txt << "\t\t" <<  tt->first << std::endl;
			}
			txt << "\tuses: " << std::endl;
			for (map<string,bool>::iterator tt = s.uses.begin(); tt!=s.uses.end();++tt){
				txt << "\t\t" <<  tt->first << std::endl;
			}
		}
		txt.flush();
		txt.close();
	}

	void BuildLog::print_map(){
		for (map<string,script>::iterator it=script_map.begin();it!=script_map.end();++it){
			script s = it->second;
			debugStream << s.dbc << std::endl;
			debugStream << "\t" <<  s.dsc << std::endl;
			debugStream << "\t" <<  s.type << std::endl;
			debugStream << "\tm_source: " <<  s.m_dsc << std::endl;
			debugStream << "\tm_byte: " <<  s.m_dbc << std::endl;
			debugStream << "\tused by: " <<  endl;
			for (map<string,bool>::iterator tt = s.usedby.begin(); tt!=s.usedby.end();++tt){
				debugStream << "\t\t" <<  tt->first << std::endl;
			}
			debugStream << "\tuses: " << std::endl;
			for (map<string,bool>::iterator tt = s.uses.begin(); tt!=s.uses.end();++tt){
				debugStream << "\t\t" <<  tt->first << std::endl;
			}
		}
		debugStream.flush();
	}

	bool BuildLog::isScriptUpToDate(string str){
		assert(!str.empty());
		string tmp =  Canonicalize(str);
		if (tmp.empty())return false;
		if (script_map.count(tmp)==0)return false;
		return LOG_ENABLED && !script_map[tmp].dirty && isScriptUpToDate(script_map[tmp]);
	}

	bool BuildLog::isScriptUpToDate(script sc){
		string name = sc.dbc;
		//debugStream << name << std::endl;
		struct stat at;			// create a file attribute structure       
		int err = stat(sc.dbc.c_str(), &at);
		if (err!=0)return false;
		bool res = (at.st_mtime==script_map[name].m_dbc);
	//	debugStream << at.st_mtime << "---" << script_map[name].m_dbc << std::endl;
		err = stat(sc.dsc.c_str(), &at);
		if (err!=0)return false;
		res = res && (at.st_mtime==script_map[name].m_dsc);
		return LOG_ENABLED && res;
	}

	void BuildLog::save(){
		debugStream << "-----SAVING-----" << std::endl;
		save_log();
		save_log_text();
	//	print_map();
		debugStream.flush();

	}

	void BuildLog::updateBytecode(string n){
		assert(!n.empty());
		string name = Canonicalize(n);
		struct stat attrib;			// create a file attribute structure       
		int res = stat(name.c_str(), &attrib);
		script_map[name];
		if (res==0){
			script_map[name].m_dbc = attrib.st_mtime;
			script_map[name].dirty = false;
		}
	}

	void BuildLog::propagateDirty(map<string,bool> deps){
		for (map<string,bool>::iterator used = deps.begin(); used!=deps.end();++used){
			script_map[used->first].dirty = true;
			propagateDirty(script_map[used->first].usedby);
		}
	}


	void BuildLog::getDirty(){
		for (map<string,script>::iterator it=script_map.begin();it!=script_map.end();++it){
			script s = it->second;
			if (!isScriptUpToDate(s)){
				propagateDirty(s.usedby);
				it->second.dirty = true;
			}
			else{
				it->second.dirty = false;
			}
		}
		for (map<string,script>::iterator it=script_map.begin();it!=script_map.end();++it){
			if ( it->second.dirty)debugStream << "x: " << it->second.dbc << std::endl;
		}
	}

	void BuildLog::read(){
		debugStream << "-----READING-----" << std::endl;
		script_map.clear();
		build_map.clear();
		read_log();
		getDirty();
		order();
		print_map();		
	}

	script::script(){
		m_dbc = 0;
		m_dsc = 0;
		dbc = "";
		dsc = "";
		type = "";
		dirty = true;
		uses = map<string,bool>();
		usedby = map<string,bool>();
	}

	string Canonicalize(string str){
		while (true){
			size_t index = str.find("/./");
			if (index==string::npos)break;		
			str.replace(index,3,"/");
		}
		transform(str.begin(), str.end(), str.begin(), ::tolower);
		return str;
	}

	void BuildLog::save_log(){
		ofstream bin;
		bin.open(logFile, ios_base::out | ios_base::binary);
		size_t x;
		char* ss;
		x=script_map.size();
		bin.write((char*)&x,sizeof(size_t));///total scripts;

		for (map<string,script>::iterator it=script_map.begin();it!=script_map.end();++it){
			script s = it->second;
			x = (s.dbc.size()+1);
			bin.write((char*)(&x),sizeof(uint));
			ss = (char*)s.dbc.c_str();
			bin.write((char*)ss,sizeof(char)*s.dbc.size()+1);
			time_t xx;
			xx=s.m_dbc;
			bin.write((char*)&xx,sizeof(time_t));
			xx=s.m_dsc;
			bin.write((char*)&xx,sizeof(time_t));

			x=(s.type.size()+1);
			bin.write((char*)&x,sizeof(size_t));
			ss = (char*)s.type.c_str();
			bin.write((char*)ss,sizeof(char)*s.type.size()+1);

			
			x = (s.dsc.size()+1);
			bin.write((char*)(&x),sizeof(size_t));
			ss = (char*)s.dsc.c_str();
			bin.write((char*)ss,sizeof(char)*s.dsc.size()+1);

			x=s.usedby.size();
			bin.write((char*)&x,sizeof(size_t));///total scripts;

			for (map<string,bool>::iterator used = s.usedby.begin();used!=s.usedby.end();++used){
				x=used->first.size()+1;
				bin.write((char*)&x,sizeof(size_t));
				ss = (char*)used->first.c_str();
				bin.write((char*)ss,sizeof(char)*used->first.size()+1);
			}

			size_t ffs  = s.uses.size();
			bin.write((char*)&ffs,sizeof(size_t));

			for (map<string,bool>::iterator uses = s.uses.begin();uses!=s.uses.end();++uses){
				ffs = uses->first.size()+1;
				bin.write((char*)&ffs,sizeof(size_t));
				char* omg = (char*)uses->first.c_str();
				bin.write(omg,ffs*sizeof(char));
			}
		}
		bin.close();

		ofstream sanity;

		sanity.open(sanityFile, ios_base::out | ios_base::binary);

		struct stat fileinfo;
		memset(&fileinfo, 0, sizeof(struct stat));
		
		stat(logFile.c_str(), &fileinfo);
		size_t log_size = fileinfo.st_size;
		sanity.write((char*)&log_size,sizeof(size_t));
		sanity.close();
	}

	void BuildLog::read_log(){

		ifstream sanity;
		size_t log_size=0;
		sanity.open(sanityFile, ios_base::in || ios_base::binary);
		sanity.read((char*)&log_size,sizeof(size_t));
		struct stat fileinfo;
		memset(&fileinfo, 0, sizeof(struct stat));
		stat(logFile.c_str(), &fileinfo);
		assert(log_size == fileinfo.st_size);
		if (log_size != fileinfo.st_size){
			debugStream << "Log file is corrupt: " << log_size << " ---- " << fileinfo.st_size << std::endl;
			return;
		}
		sanity.close();

		ifstream bin;
		bin.open(logFile, ios_base::in || ios_base::binary);
		size_t no_scripts = 0;
		bin.read((char*)&no_scripts,sizeof(size_t));
		char* s;
		for (size_t i=0;i<no_scripts;i++){

			string tmp;

			size_t l=0;
			bin.read((char*)&l,sizeof(size_t));
			s = (char*)malloc(sizeof(char)*l);
			bin.read((char*)s,sizeof(char)*l);
			string name = string(s);
			script_map[name];
			script_map[name].dbc = name;

			bin.read((char*)&script_map[name].m_dbc,sizeof(time_t));
			bin.read((char*)&script_map[name].m_dsc,sizeof(time_t));

			bin.read((char*)&l,sizeof(size_t));
			s = (char*)malloc(sizeof(char)*l);
			bin.read((char*)s,sizeof(char)*l);
			tmp = string(s);
			script_map[name].type = tmp;

			bin.read((char*)&l,sizeof(size_t));
			s = (char*)malloc(sizeof(char)*l);
			bin.read((char*)s,sizeof(char)*l);
			tmp = string(s);
			script_map[name].dsc = tmp;

			size_t used=0;
			bin.read((char*)&used,sizeof(size_t));

			for (size_t j=0;j<used;j++){
				bin.read((char*)&l,sizeof(size_t));
				s = (char*)malloc(sizeof(char)*l);
				bin.read((char*)s,sizeof(char)*l);
				tmp = string(s);
				script_map[name].usedby[tmp]=true;
			}

			
			size_t uses=0;
			bin.read((char*)&uses,sizeof(size_t));

			for (size_t f=0;f<uses;f++){
				bin.read((char*)&l,sizeof(size_t));
				s = (char*)malloc(sizeof(char)*l);
				bin.read((char*)s,sizeof(char)*l);
				tmp = string(s);
				script_map[name].uses[tmp]=true;
			}
		}
		bin.close();
	}
}