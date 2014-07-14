// BuildLog.cpp
// Script build filtering supporting logged dependencies.
// George Diakostavrianos, 2014
// Dimploma project under Anthony Savidis (with severe refactoring too)/
// 

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


/*	TODO
	save last enable/disable state somewhere in workspace properties
*/

//********************************************************

#define	IF_NOT_ENABLED_RETURN()						\
	boost::mutex::scoped_lock lock(resourceMutex);	\
	if (!enabled)									\
		return

//********************************************************

typedef LoggedScript::StringBag StringBag;

static const int			GetFileInfo					(const std::string& file, struct stat* info);
static const std::string	GetStringAttribute			(const wxXmlNode* node, const std::string& name);
static const time_t			GetTimeAttribute			(const wxXmlNode* node, const std::string& name);
static StringBag			GetChildNodes				(const wxXmlNode* node, const std::string& name, const std::string& type);
static StdStringList		GetChildExternalNodes		(const wxXmlNode* node, const std::string& name, const std::string& type);
static const std::string	GetDbcFromPath				(const std::string& source);
static xml::Node			CreateChildNodes			(const StdStringList& children, const std::string& type, const std::string& name);
static xml::Node			CreateChildNodes			(const StringBag& children, const std::string& type, const std::string& name);

//********************************************************
/*
**  the build log is singleton, but on each
**  read it empties itself and gets
**  initialized with current workspace information.
*/
//********************************************************

BuildLog& BuildLog::GetSingleton (void)  { 
	if (!inst)
		inst = new BuildLog;
	return *inst;
}

BuildLog* BuildLog::inst = (BuildLog*) 0;

BuildLog::BuildLog (void) {
	debugFile = "debug.txt";
	logFile = ".buildlog";
	debugStream = std::ofstream(debugFile);
	enabled = true;
}

//********************************************************
/*
**	Add dependencies to the *name* entry.
*/
void BuildLog::AddDependencies (const std::string& name, const StdStringList& deps) {

	IF_NOT_ENABLED_RETURN();
	assert(!name.empty());

	for (StdStringList::const_iterator i = deps.begin(); i != deps.end(); ++i) {

		const std::string dep = GetDbcFromPath(*i);
		assert(!dep.empty());

		logScriptMap[dep].usedby[name] = true;
		logScriptMap[name].uses[dep]   = true;
	}
}

//********************************************************
/*
**	Add a script to the log. The key used is its bytecode
**	name. On the dependencies part, we update both the
**	scripts and the depenency's uses/usedby entries along
**	with the script's source file last modification stamp
*/
void BuildLog::Add (
		const std::string&		name, 
		const std::string&		dsc, 
		const std::string&		dbc, 
		const std::string&		type, 
		const StdStringList&	deps, 
		const StdStringList&	external
	) {

	IF_NOT_ENABLED_RETURN();
	assert(!name.empty() && !dbc.empty() && !type.empty());

	// TODO(AS): is it valid to assert below ?
	// assert(logScriptMap.find(name) == logScriptMap.end());

	LoggedScript& script (logScriptMap[name]);
	script.name		= name;
	script.dsc		= dsc;
	script.dbc		= dbc;
	script.type		= type;
	script.external = external;

	for (StdStringList::const_iterator i = deps.begin(); i != deps.end(); ++i){

		const std::string dep = GetDbcFromPath(*i);
		assert(!dep.empty());

		logScriptMap[dep].usedby[name]  = true;
		logScriptMap[name].uses[dep]	= true;
	}

	struct stat info;
	GetFileInfo(logScriptMap[name].dsc, &info);
	logScriptMap[name].dscTimeStamp = info.st_mtime;
}

//********************************************************

bool BuildLog::IsEnabled (void) const {
	IF_NOT_ENABLED_RETURN()	false; else return true;
}

//********************************************************
/*	Returns whether the particural workspace log exists
*/
bool BuildLog::LogExists (const String& path, const String& name) const {
	std::string wpath = GenerateWorkspaceLogFullFilename(path,name);
	boost::filesystem::wpath file(wpath);
	return boost::filesystem::exists(file);	
}

//********************************************************
/*	Proxy script uptodate calculation by name. False is it does
**	not exist or on error.
*/
bool BuildLog::IsScriptUpToDate (const std::string& name) const {

	IF_NOT_ENABLED_RETURN() false;

	assert(!name.empty());

	// intentionally creating entries in maps in case
	// element not inside

	ScriptMap::const_iterator i = logScriptMap.find(name);
	return	i != logScriptMap.end()	&&
			!i->second.dirty		&& 
			IsScriptUpToDate(i->second);
}

//********************************************************
/*	Actual script uptodate calculation by object. False
**	on error.
*/
bool BuildLog::IsScriptUpToDate (const LoggedScript& script) const {

	struct stat info;     
	if (GetFileInfo(script.dbc, &info))
		return false;
	else {
		bool	same	= info.st_mtime == script.dbcTimeStamp;
		int		result	= GetFileInfo(script.dsc, &info);

		return !result && same && info.st_mtime == script.dscTimeStamp;
	}
}

//********************************************************

void BuildLog::Save (void) const {
	boost::mutex::scoped_lock lock(resourceMutex);
	if (enabled)
		SaveLog();
}

//********************************************************
/*
**	Called upon build completion of a script. Updates dbc
**	timestamp or returns on error.
*/
void BuildLog::UpdateBytecode (const std::string& name) {

	boost::mutex::scoped_lock lock(resourceMutex);
	if (!enabled)
		return;

	assert(!name.empty());
	ScriptMap::iterator i = logScriptMap.find(name);
	assert(i != logScriptMap.end());

	LoggedScript&	script (i->second);
	struct stat		info;

	if (!GetFileInfo(script.dbc, &info)) {
		script.dbcTimeStamp = info.st_mtime;
		script.dirty		= false;
	}
}

//********************************************************
/*	Sets scripts *dirty* bit for itself and recursively
**	does the same for anyone using that script.
*/
void BuildLog::MarkOutOfDateRecursively (const StringBag& deps){
	for (StringBag::const_iterator used = deps.begin(); used!=deps.end();++used){
		logScriptMap[used->first].dirty = true;
		MarkOutOfDateRecursively (logScriptMap[used->first].usedby);
	}
}

//********************************************************
/*	Marks clean all the scripts that are uptodate using the
**	above function.
*/
void BuildLog::MarkOutOfDate (void) {

	for (ScriptMap::iterator i = logScriptMap.begin(); i != logScriptMap.end(); ++i){
		LoggedScript& s = i->second;
		if (!IsScriptUpToDate(s)){
			MarkOutOfDateRecursively(s.usedby);
			s.dirty = true;
		}
		else
			s.dirty = false;
	}
}

//********************************************************
/*	Updates dbc and dsc paths of a script in case it moved.
*/
void BuildLog::UpdateDirectoryInformation (
		const std::string& name, 
		const std::string& sourcePath, 
		const std::string& bytecodePath
	) {

	boost::mutex::scoped_lock lock(resourceMutex);
	if (!enabled)
		return;

	assert(!name.empty());
	ScriptMap::iterator i = logScriptMap.find(name);
	assert(i != logScriptMap.end());

	LoggedScript& script (i->second);
	script.dbc = bytecodePath;
	script.dsc = sourcePath;
}

//********************************************************
/*	Constructs workspace path information and calls the
**	reader.
*/
void BuildLog::Read (const String& path, const String& name) {

	boost::mutex::scoped_lock lock(resourceMutex);
	if (!enabled)
		return;

	assert(!name.empty());
	currentWorkspace = util::str2std(name);
	currentWorkspaceLogPath = GenerateWorkspaceLogFullFilename(path,name);
	logScriptMap.clear();

	ReadLog();
	MarkOutOfDate();
}

//********************************************************

std::string BuildLog::GetLastWorkspaceLogPath(void){
	return currentWorkspaceLogPath;
}

//********************************************************
/*	Generates the log path depending on the current
**	workspace name, in the form of *name*.buildlog. If path
**	is not a path, the name generated is general.buildlog
**	in the .exe directory.
*/
std::string BuildLog::GenerateWorkspaceLogFullFilename(const String& path, const String& name) const {

	const std::string	wpath	(util::str2std(path));
	std::string			tmpPath ("general" + logFile);

	if (boost::filesystem::is_directory(wpath)){
		boost::filesystem::path p(wpath);
		p /= boost::filesystem::path(util::str2std(name) + logFile);
		tmpPath = p.string();
	}

	return tmpPath;
}

//********************************************************

void BuildLog::EnableBuildLog (void)
	{ boost::mutex::scoped_lock lock(resourceMutex); enabled = true; }

//********************************************************

void BuildLog::DisableBuildLog (void)
	{ boost::mutex::scoped_lock lock(resourceMutex); enabled = false; }

//********************************************************
/*	Deletes workspace log.
*/
void BuildLog::DeleteBuildLog (const String& path, const String& name){
	std::string tmp = GenerateWorkspaceLogFullFilename(path,name);
	boost::filesystem::wpath file(tmp);
	if(boost::filesystem::exists(file))
        boost::filesystem::remove(file);
}

//********************************************************
/*	Extracts a name of the form name.dbc out of a std::string.
**	Used to get script names out of dependencies.
*/
static const std::string GetDbcFromPath (const std::string& source) {

	std::string s = source;

	if (*s.rbegin() == '/')
		s.pop_back();
	
	size_t n = s.rfind('/');
	return	n != std::string::npos ? s.substr(n + 1, s.length()) : s;
}

//********************************************************
/*	Reads the log. The log is in XML format, and uses
**	wxXML elements and functions. On error it returns
**	with empty maps. Example log of a workspace *foo*.
**
**	<foo>
**		<Script name="bar.dbc">
**			<Information type="Script" dbc="path_to_bar.dbc" dsc="path_to_bar.dsc" dbcTimeStamp="12342150" dscTimeStamp="1235215"/>
**			<UsedBy>
**				<Script name="barbarbar.dbc"/>
**			</UsedBy>
**			<Uses>
**				<Script name="barbar.dbc"/>
**			</Uses>
**			<External/>
**		</Script>
**	</foo>
*/
void BuildLog::ReadLog(){

	wxXmlDocument doc;
	doc.SetFileEncoding(_T("utf-8"));
	wxLogNull disableErrorPopup;

	if (!boost::filesystem::exists(currentWorkspaceLogPath))
		return;

	if (!doc.Load(util::std2str(currentWorkspaceLogPath)))
		return;

	if (doc.GetRoot()->GetName() != util::std2str(currentWorkspace))return;

	wxXmlNode *root = doc.GetRoot();
	String name = root->GetName();
	wxXmlNode *child = root->GetChildren();
	while(child){
		std::string name =  util::str2std(child->GetAttribute(_T("name"),_T("")));
		assert(!name.empty());
		if (name.empty()){
			logScriptMap.clear();
			return;
		}
		logScriptMap[name].name = name;

		wxXmlNode* info = child->GetChildren();

		while (info){
			if (info->GetName() == util::std2str("Information")){
				logScriptMap[name].dbc = GetStringAttribute(info,"dbc");
				logScriptMap[name].dsc = GetStringAttribute(info,"dsc");
				logScriptMap[name].type = GetStringAttribute(info,"type");

				logScriptMap[name].dbcTimeStamp = GetTimeAttribute(info,"dbcTimeStamp");
				logScriptMap[name].dscTimeStamp = GetTimeAttribute(info,"dscTimeStamp");
				break;
			}
			info = info->GetNext();
		}

		logScriptMap[name].usedby = GetChildNodes(child,"UsedBy","name");
		logScriptMap[name].uses = GetChildNodes(child,"Uses","name");
		logScriptMap[name].external = GetChildExternalNodes(child,"External","name");
		child = child->GetNext();
	}
	
}

//********************************************************
/*	Constructs a node containing children with *name*
**	attributes. Used to fill usedby/uses nodes.
*/
static xml::Node CreateChildNodes(const StringBag& children, const std::string& type, const std::string& name){
	xml::Node parent;
	parent.Create(util::std2str(name));
	for (StringBag::const_iterator tt = children.begin(); tt!=children.end();++tt){
		xml::Node tmp;
		tmp.Create(_T("Script"));
		tmp.SetProperty(util::std2str(type),util::std2str(tt->first));
		parent.InsertChild(tmp);
	}
	return parent;
}

//********************************************************
/*	Constructs a node containing children with *name*
**	attributes. Used to fill external nodes.
*/
static xml::Node CreateChildNodes(const StdStringList& children, const std::string& type, const std::string& name){
	xml::Node parent;
	parent.Create(util::std2str(name));
	for (StdStringList::const_iterator tt = children.begin(); tt!=children.end();++tt){
		xml::Node tmp;
		tmp.Create(_T("Script"));
		tmp.SetProperty(util::std2str(type),util::std2str(*tt));
		parent.InsertChild(tmp);
	}
	return parent;
}

//********************************************************
/*	Saves the log in the form given above.
*/

void BuildLog::SaveLog (void) const {

	xml::Node		root;
	wxXmlDocument	doc;

	root.Create(util::std2str(currentWorkspace));
	doc.SetRoot(root.NativeType());

	for (ScriptMap::const_iterator i = logScriptMap.begin(); i != logScriptMap.end(); ++i){

		const LoggedScript& s = i->second;

		xml::Node child;
		child.Create(_T("Script"));
		child.SetProperty(_T("name"),util::std2str(s.name));

		xml::Node info;
		info.Create(_T("Information"));
		info.SetProperty(_T("type"), util::std2str(s.type));
		info.SetProperty(_T("dbc"), util::std2str(s.dbc));
		info.SetProperty(_T("dsc"), util::std2str(s.dsc));			
		info.SetProperty(_T("dbcTimeStamp"), util::std2str(boost::lexical_cast<std::string>(s.dbcTimeStamp)));
		info.SetProperty(_T("dscTimeStamp"), util::std2str(boost::lexical_cast<std::string>(s.dscTimeStamp)));

		child.InsertChild(info);
		child.InsertChild(CreateChildNodes (s.usedby, "name", "UsedBy"));
		child.InsertChild(CreateChildNodes (s.uses, "name", "Uses"));
		child.InsertChild(CreateChildNodes (s.external, "name", "External"));

		root.InsertChild(child);
	}

	doc.SetFileEncoding(_T("utf-8"));
	bool succeeded = doc.Save(util::std2str(currentWorkspaceLogPath), 2);
	assert(succeeded);
}

//********************************************************
/*	Constructs and returns a stat struct containing given
**	file information. stat is (i think) cross-platform
**	and thus suitable for this task. Could have used boost
**	too. Maybe later.
*/
static const int GetFileInfo (const std::string& file, struct stat* fileinfo){	
	memset(fileinfo, 0, sizeof(struct stat));
	return stat(file.c_str(), fileinfo);
}

//********************************************************
/*	Gets attribute *name* of an xml node.
*/
static const std::string GetStringAttribute(const wxXmlNode* node, const std::string& name){
	String tmp = (node->GetAttribute(util::std2str(name),_T("")));
	assert(!tmp.empty());
	return util::str2std(tmp);
}

//********************************************************
/*	Same as above returning time_t timestamp data.
*/
static const time_t GetTimeAttribute(const wxXmlNode* node, const std::string& name){
	String tmp = (node->GetAttribute(util::std2str(name),_T("0")));
	assert(!tmp.empty());
	time_t mod_time =  boost::lexical_cast<time_t>(tmp);
	return mod_time;
}

//********************************************************
/*	Gets usedby/uses data out of an xml node.
*/
static StringBag GetChildNodes(const wxXmlNode* node, const std::string& name, const std::string& type){

	StringBag	result;
	wxXmlNode*	children = node->GetChildren();

	while (children){
		if (children->GetName() == util::std2str(name)){
			children = children->GetChildren();
			while(children){
				String element = children->GetAttribute(util::std2str(type),_T(""));
				if (element.empty())continue;	// FIXME(AS): likely endless loop
				result[util::str2std(element)] = true;
				children = children->GetNext();
			}
			break;
		}
		children = children->GetNext();
	}
	return result;
}

//********************************************************
/*	Gets external data out of an xml node.
*/
static StdStringList GetChildExternalNodes(const wxXmlNode* node, const std::string& name, const std::string& type){
	wxXmlNode* children = node->GetChildren();
	StdStringList tmp;
	while (children){
		if (children->GetName() == util::std2str(name)){
			children = children->GetChildren();
			while(children){
				String element = children->GetAttribute(util::std2str(type),_T(""));
				if (element.empty())continue;	// FIXME(AS): likely endless loop
				tmp.push_back(util::str2std(element));
				children = children->GetNext();
			}
			break;
		}
		children = children->GetNext();
	}
	return tmp;
}

//********************************************************
