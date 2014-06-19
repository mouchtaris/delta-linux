#include "BuildLog.h"
#include <boost/range.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <wx/utils.h>
#include "VirtualContainer.h"
#include <sys/stat.h>
#include <assert.h>
	
#include <time.h>
#include "Script.h"

#define LOG_ENABLED true

#define _SS util::str2std
#define _ss util::std2str

namespace bl{


	//////////////////////////////////////////////////////////////////

	static const int			getFileInfo			(const string &file, struct stat &fileinfo);
	static const std::string	getStringAttribute	(const wxXmlNode* node, const string name);
	static const time_t			getTimeAttribute	(const wxXmlNode* node, const string name);
	static KeyMap				getChildNodes		(const wxXmlNode* node, const string name, const string type);
	static const string			getDbcFromPath		(const string &source);
	//////////////////////////////////////////////////////////////////

	BuildLog buildLog;
	string endl = "\n";

	//////////////////////////////////////////////////////////////////

	BuildLog::BuildLog(){
		debugFile = "debug.txt";
		logFile = "buildLog.xml";
		debugStream = ofstream(debugFile);
	}

	//////////////////////////////////////////////////////////////////

	void BuildLog::addAspects (const string &name ,const StringList &deps){
		assert(!name.empty());

		for (StringList::const_iterator it = deps.begin(); it!=deps.end();++it){		
			const string dep = getDbcFromPath(util::str2std(*it));
			assert(!dep.empty());
			script_map[dep].usedby[name]=true;
			script_map[name].uses[dep]=true;
		}
	}

	//////////////////////////////////////////////////////////////////

	void BuildLog::add (const string &name,const String &dsc, const string &byte, const string &type, const StringList &deps){
		assert(!byte.empty());
		assert(!type.empty());
		assert(!name.empty());

		script_map[name].name = name;
		script_map[name].dsc =  _SS(dsc);
		script_map[name].dbc = byte;
		script_map[name].type = type;

		for (StringList::const_iterator it = deps.begin(); it!=deps.end(); ++it){
			const string dep = getDbcFromPath(util::str2std(*it));
			script_map[dep].usedby[name]=true;
			script_map[name].uses[dep]=true;
		}

		struct stat info;
		getFileInfo(script_map[name].dsc,info);
		script_map[name].m_dsc = info.st_mtime;
	}

	//////////////////////////////////////////////////////////////////

	bool BuildLog::isScriptUpToDate(const string &name){


		assert(!str.empty());
		string path = name;
		if (path.empty())return false;
		if (script_map.count(path)==0)return false;

		return LOG_ENABLED && !script_map[path].dirty && isScriptUpToDate(script_map[path]);
	}

	//////////////////////////////////////////////////////////////////

	bool BuildLog::isScriptUpToDate(const script &sc){

		string path = sc.dbc;
		assert(!path.empty());

		struct stat at;     
		int err = getFileInfo(sc.dbc, at);

		if (err!=0)return false;
		bool res = (at.st_mtime==sc.m_dbc);

		err = getFileInfo(sc.dsc, at);
		if (err!=0)return false;
		res = res && (at.st_mtime==sc.m_dsc);
		return LOG_ENABLED && res;
	}

	//////////////////////////////////////////////////////////////////

	void BuildLog::save(){
		saveLog();
	}

	//////////////////////////////////////////////////////////////////

	void BuildLog::updateBytecode(const string &name){
		assert(!n.empty());
		assert(!name.empty());
		assert(script_map.count(name));
		if (!script_map.count(name))return;

		string path = script_map[name].dbc;
		struct stat info;     
		int res = getFileInfo(path, info);

		if (res==0){
			script_map[name].m_dbc = info.st_mtime;
			script_map[name].dirty = false;
		}
	}

	//////////////////////////////////////////////////////////////////

	void BuildLog::markOutOfDateRecursively (const KeyMap &deps){
		for (KeyMap::const_iterator used = deps.begin(); used!=deps.end();++used){
			script_map[used->first].dirty = true;
			markOutOfDateRecursively (script_map[used->first].usedby);
		}
	}

	//////////////////////////////////////////////////////////////////

	void BuildLog::markOutOfDate(){
		for (ScriptMap::iterator it=script_map.begin();it!=script_map.end();++it){
			const script s = it->second;
			if (!isScriptUpToDate(s)){
				markOutOfDateRecursively(s.usedby);
				it->second.dirty = true;
			}
			else{
				it->second.dirty = false;
			}
		}
	}

	//////////////////////////////////////////////////////////////////

	void BuildLog::updateDirectoryInformation(const string &name, const String &sourcePath, const string &bytecodePath){
		assert(!name.empty());
		if (script_map.count(name)==0)return;
		script_map[name].dbc = bytecodePath;
		script_map[name].dsc = _SS(sourcePath);
	}

	//////////////////////////////////////////////////////////////////

	void BuildLog::read(const String &path, const String &name){
		const string wpath= _SS(path);
		currentWorkspaceLogPath = logFile;
		currentWorkspace = _SS(name);
		currentWorkspaceLogPath = logFile;
		if (boost::filesystem::is_directory(wpath)){
			boost::filesystem::path p(wpath);
			p /= boost::filesystem::path(logFile);
			currentWorkspaceLogPath = p.string();
		}
		debugStream << currentWorkspaceLogPath << std::endl;
		build_order.clear();
		script_map.clear();
		readLog();
		markOutOfDate();
		//order();
	}

	//////////////////////////////////////////////////////////////////

	static const string getDbcFromPath(const string &source){
		string str = source;
		size_t found = str.rfind('/');
		if (found!=std::string::npos){
			return str.substr(found+1,str.length());
		}
		return str;
	}
	//////////////////////////////////////////////////////////////////

	void BuildLog::readLog(){
		wxXmlDocument doc;

		if ( !boost::filesystem::exists(currentWorkspaceLogPath))return;
		if (!doc.Load(_ss(currentWorkspaceLogPath)))return;

		if (doc.GetRoot()->GetName() != _ss(currentWorkspace))return;

		wxXmlNode *root = doc.GetRoot();
		String name = root->GetName();
		wxXmlNode *child = root->GetChildren();
		while(child){
			string name =  _SS(child->GetAttribute(_T("name"),_T("")));
			assert(!name.empty());
			if (name.empty())return;
			script_map[name].name = name;

			wxXmlNode* info = child->GetChildren();

			while (info){
				if (info->GetName() == _ss("Information")){
					script_map[name].dbc = getStringAttribute(info,"dbc");
					script_map[name].dsc = getStringAttribute(info,"dsc");
					script_map[name].type = getStringAttribute(info,"type");

					script_map[name].m_dbc = getTimeAttribute(info,"m_dbc");
					script_map[name].m_dsc = getTimeAttribute(info,"m_dsc");
					break;
				}
				info = info->GetNext();
			}

			script_map[name].usedby = getChildNodes(child,"UsedBy","name");
			script_map[name].uses = getChildNodes(child,"Uses","name");

			child = child->GetNext();
		}
		
	}

	static xml::Node createChildNodes(const KeyMap &children, const string type,const string name){
		xml::Node parent;
		parent.Create(_ss(name));
		for (KeyMap::const_iterator tt = children.begin(); tt!=children.end();++tt){
			xml::Node tmp;
			tmp.Create(_T("Script"));
			tmp.SetProperty(_ss(type),_ss(tt->first));
			parent.InsertChild(tmp);
		}
		return parent;
	}

	//////////////////////////////////////////////////////////////////

	void BuildLog::saveLog(){
		wxXmlDocument doc;
		xml::Node	root;
		root.Create(_ss(currentWorkspace));
		doc.SetRoot(root.NativeType());
		xml::Node	child;
		for (ScriptMap::const_iterator it=script_map.begin();it!=script_map.end();++it){
			xml::Node child;
			const script& s = it->second;
			child.Create(_T("Script"));
			child.SetProperty(_T("name"),_ss(s.name));
			xml::Node info;
			info.Create(_T("Information"));
			info.SetProperty(_T("type"),_ss(s.type));
			info.SetProperty(_T("dbc"),_ss(s.dbc));
			info.SetProperty(_T("dsc"),_ss(s.dsc));			
			info.SetProperty(_T("m_dbc"),_ss(boost::lexical_cast<std::string>(s.m_dbc)));
			info.SetProperty(_T("m_dsc"),_ss(boost::lexical_cast<std::string>(s.m_dsc)));
			child.InsertChild(info);
			child.InsertChild(createChildNodes (s.usedby,"name","UsedBy"));
			child.InsertChild(createChildNodes (s.uses,"name","Uses"));
			root.InsertChild(child);
		}
		doc.SetFileEncoding(_T("utf-8"));
		bool ok = doc.Save(_ss(currentWorkspaceLogPath),2);
		assert(ok);
	}

	//////////////////////////////////////////////////////////////////

	static const int getFileInfo(const string &file, struct stat &fileinfo){	
		memset(&fileinfo, 0, sizeof(struct stat));
		return stat(file.c_str(), &fileinfo);
	}

	//////////////////////////////////////////////////////////////////

	static const std::string getStringAttribute(const wxXmlNode* node,const std::string name){
		String tmp = (node->GetAttribute(_ss(name),_T("")));
		assert(!tmp.empty());
		return _SS(tmp);
	}

	//////////////////////////////////////////////////////////////////

	static const time_t getTimeAttribute(const wxXmlNode* node,const std::string name){
			String tmp = (node->GetAttribute(_ss(name),_T("0")));
			assert(!tmp.empty());
			time_t mod_time =  boost::lexical_cast<time_t>(tmp);
			return mod_time;
	}

	//////////////////////////////////////////////////////////////////

	static KeyMap getChildNodes(const wxXmlNode* node, const string name, const string type){
		wxXmlNode* children = node->GetChildren();
		KeyMap tmp;
		while (children){
			if (children->GetName() == _ss(name)){
				children = children->GetChildren();
				while(children){
					String element = children->GetAttribute(_ss(type),_T(""));
					if (element.empty())continue;
					tmp[_SS(element)] = true;
					children = children->GetNext();
				}
				break;
			}
			children = children->GetNext();
		}
		return tmp;
	}
}