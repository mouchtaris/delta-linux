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

#include <wx/log.h>

#define LOG_ENABLED true

#define _SS util::str2std
#define _ss util::std2str

namespace bl{


	//////////////////////////////////////////////////////////////////

	static const int			GetFileInfo			(const string &file, struct stat &fileinfo);
	static const std::string	GetStringAttribute	(const wxXmlNode* node, const string name);
	static const time_t			GetTimeAttribute	(const wxXmlNode* node, const string name);
	static KeyMap				GetChildNodes		(const wxXmlNode* node, const string name, const string type);
	static const string			GetDbcFromPath		(const string &source);
	//////////////////////////////////////////////////////////////////

	BuildLog buildLog;
	string endl = "\n";

	//////////////////////////////////////////////////////////////////

	BuildLog::BuildLog(){
		debugFile = "debug.txt";
		logFile = "_BuildLog.xml";
		debugStream = ofstream(debugFile);
	}

	//////////////////////////////////////////////////////////////////

	void BuildLog::AddAspects (const string &name ,const StringList &deps){
		assert(!name.empty());

		for (StringList::const_iterator it = deps.begin(); it!=deps.end();++it){		
			const string dep = GetDbcFromPath(util::str2std(*it));
			assert(!dep.empty());
			logScriptMap[dep].usedby[name]=true;
			logScriptMap[name].uses[dep]=true;
		}
	}

	//////////////////////////////////////////////////////////////////

	void BuildLog::Add (const string &name,const String &dsc, const string &byte, const string &type, const StringList &deps){
		assert(!byte.empty());
		assert(!type.empty());
		assert(!name.empty());

		logScriptMap[name].name = name;
		logScriptMap[name].dsc =  _SS(dsc);
		logScriptMap[name].dbc = byte;
		logScriptMap[name].type = type;

		for (StringList::const_iterator it = deps.begin(); it!=deps.end(); ++it){
			const string dep = GetDbcFromPath(util::str2std(*it));
			logScriptMap[dep].usedby[name]=true;
			logScriptMap[name].uses[dep]=true;
		}

		struct stat info;
		GetFileInfo(logScriptMap[name].dsc,info);
		logScriptMap[name].m_dsc = info.st_mtime;
	}

	//////////////////////////////////////////////////////////////////

	bool BuildLog::IsScriptUpToDate(const string &name){


		assert(!str.empty());
		string path = name;
		if (path.empty())return false;
		if (logScriptMap.count(path)==0)return false;

		return LOG_ENABLED && !logScriptMap[path].dirty && IsScriptUpToDate(logScriptMap[path]);
	}

	//////////////////////////////////////////////////////////////////

	bool BuildLog::IsScriptUpToDate(const LogScript &sc){

		string path = sc.dbc;
		assert(!path.empty());

		struct stat at;     
		int err = GetFileInfo(sc.dbc, at);

		if (err!=0)return false;
		bool res = (at.st_mtime==sc.m_dbc);

		err = GetFileInfo(sc.dsc, at);
		if (err!=0)return false;
		res = res && (at.st_mtime==sc.m_dsc);
		return LOG_ENABLED && res;
	}

	//////////////////////////////////////////////////////////////////

	void BuildLog::Save(){
		SaveLog();
	}

	//////////////////////////////////////////////////////////////////

	void BuildLog::UpdateBytecode(const string &name){
		assert(!n.empty());
		assert(!name.empty());
		assert(logScriptMap.count(name));
		if (!logScriptMap.count(name))return;

		string path = logScriptMap[name].dbc;
		struct stat info;     
		int res = GetFileInfo(path, info);

		if (res==0){
			logScriptMap[name].m_dbc = info.st_mtime;
			logScriptMap[name].dirty = false;
		}
	}

	//////////////////////////////////////////////////////////////////

	void BuildLog::MarkOutOfDateRecursively (const KeyMap &deps){
		for (KeyMap::const_iterator used = deps.begin(); used!=deps.end();++used){
			logScriptMap[used->first].dirty = true;
			MarkOutOfDateRecursively (logScriptMap[used->first].usedby);
		}
	}

	//////////////////////////////////////////////////////////////////

	void BuildLog::MarkOutOfDate(){
		for (ScriptMap::iterator it=logScriptMap.begin(); it!=logScriptMap.end(); ++it){
			LogScript& s = it->second;
			if (!IsScriptUpToDate(s)){
				MarkOutOfDateRecursively(s.usedby);
				s.dirty = true;
			}
			else{
				s.dirty = false;
			}
		}
	}

	//////////////////////////////////////////////////////////////////

	void BuildLog::UpdateDirectoryInformation(const string &name, const String &sourcePath, const string &bytecodePath){
		assert(!name.empty());
		if (logScriptMap.count(name)==0)return;
		logScriptMap[name].dbc = bytecodePath;
		logScriptMap[name].dsc = _SS(sourcePath);
	}

	//////////////////////////////////////////////////////////////////

	void BuildLog::Read(const String &path, const String &name){
		const string wpath= _SS(path);
		currentWorkspaceLogPath = logFile;
		currentWorkspace = _SS(name);
		currentWorkspaceLogPath = logFile;
		if (boost::filesystem::is_directory(wpath)){
			boost::filesystem::path p(wpath);
			p /= boost::filesystem::path(_SS(name)+logFile);
			currentWorkspaceLogPath = p.string();
		}
		debugStream << currentWorkspaceLogPath << std::endl;
		buildOrder.clear();
		logScriptMap.clear();
		ReadLog();
		MarkOutOfDate();
		//order();
	}

	///////////////////////////////////////////////////////////////////

	string BuildLog::GetLastWorkspaceLogPath(void){
		return currentWorkspaceLogPath;
	}

	///////////////////////////////////////////////////////////////////

	void BuildLog::DeleteBuildLog(const String &path, const String &name){
		const string wpath= _SS(path);
		currentWorkspaceLogPath = logFile;
		currentWorkspace = _SS(name);
		currentWorkspaceLogPath = logFile;
		if (boost::filesystem::is_directory(wpath)){
			boost::filesystem::path p(wpath);
			p /= boost::filesystem::path(_SS(name)+logFile);
			currentWorkspaceLogPath = p.string();
		}

		boost::filesystem::wpath file(currentWorkspaceLogPath);
		if(boost::filesystem::exists(file))
            boost::filesystem::remove(file);
	}
	//////////////////////////////////////////////////////////////////

	static const string GetDbcFromPath(const string &source){
		string str = source;
		size_t found = str.rfind('/');
		if (found!=std::string::npos){
			return str.substr(found+1,str.length());
		}
		return str;
	}
	//////////////////////////////////////////////////////////////////

	void BuildLog::ReadLog(){
		wxXmlDocument doc;
		doc.SetFileEncoding(_T("utf-8"));
		wxLogNull disableErrorPopup;
		if ( !boost::filesystem::exists(currentWorkspaceLogPath))return;
		if (!doc.Load(_ss(currentWorkspaceLogPath)))return;

		if (doc.GetRoot()->GetName() != _ss(currentWorkspace))return;

		wxXmlNode *root = doc.GetRoot();
		String name = root->GetName();
		wxXmlNode *child = root->GetChildren();
		while(child){
			string name =  _SS(child->GetAttribute(_T("name"),_T("")));
			assert(!name.empty());
			//if (name.empty())return;
			logScriptMap[name].name = name;

			wxXmlNode* info = child->GetChildren();

			while (info){
				if (info->GetName() == _ss("Information")){
					logScriptMap[name].dbc = GetStringAttribute(info,"dbc");
					logScriptMap[name].dsc = GetStringAttribute(info,"dsc");
					logScriptMap[name].type = GetStringAttribute(info,"type");

					logScriptMap[name].m_dbc = GetTimeAttribute(info,"m_dbc");
					logScriptMap[name].m_dsc = GetTimeAttribute(info,"m_dsc");
					break;
				}
				info = info->GetNext();
			}

			logScriptMap[name].usedby = GetChildNodes(child,"UsedBy","name");
			logScriptMap[name].uses = GetChildNodes(child,"Uses","name");

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

	void BuildLog::SaveLog(){
		wxXmlDocument doc;
		xml::Node	root;
		root.Create(_ss(currentWorkspace));
		doc.SetRoot(root.NativeType());
		xml::Node	child;
		for (ScriptMap::const_iterator it=logScriptMap.begin();it!=logScriptMap.end();++it){
			xml::Node child;
			const LogScript& s = it->second;
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

	static const int GetFileInfo(const string &file, struct stat &fileinfo){	
		memset(&fileinfo, 0, sizeof(struct stat));
		return stat(file.c_str(), &fileinfo);
	}

	//////////////////////////////////////////////////////////////////

	static const std::string GetStringAttribute(const wxXmlNode* node,const std::string name){
		String tmp = (node->GetAttribute(_ss(name),_T("")));
		assert(!tmp.empty());
		return _SS(tmp);
	}

	//////////////////////////////////////////////////////////////////

	static const time_t GetTimeAttribute(const wxXmlNode* node,const std::string name){
			String tmp = (node->GetAttribute(_ss(name),_T("0")));
			assert(!tmp.empty());
			time_t mod_time =  boost::lexical_cast<time_t>(tmp);
			return mod_time;
	}

	//////////////////////////////////////////////////////////////////

	static KeyMap GetChildNodes(const wxXmlNode* node, const string name, const string type){
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