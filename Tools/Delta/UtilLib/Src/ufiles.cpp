// ufiles.cpp
// File specific utilities.
// Moved from the central delta util here.
// ScriptFighter Project.
// A. Savidis, July 2002.
//

#include "ufiles.h"
#include "ustlhelpers.h"
#include "usystem.h"
#include "ustrings.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef	_UNIX_
#include <unistd.h>
#include <pwd.h>
#endif

//------------------------------------------------------------------

void upathmanager::add (const std::string& path, bool prioritised) {
	if (prioritised)
		priorityPaths.push_back(path);
	else
		normalPaths.push_back(path);
}

/////////////////////////////////////////////////////////////////

void upathmanager::add_environment_path (const std::string& env, bool prioritised) {
	std::string var = ugetenvironmentvar(env);
	if (!var.empty()) {
		ustrlist paths = utokenizestr(var, ";,");
		if (!paths.empty())
			ulistcopy(paths, prioritised ? priorityPaths : normalPaths);
	}
}

/////////////////////////////////////////////////////////////////

const std::string upathmanager::make_entire_path (void) {
	std::string result;
	if (!priorityPaths.empty())
		result = ustrconcat(priorityPaths, ";");
	if (!normalPaths.empty()) {
		if (!result.empty())
			result += ";";
		result += ustrconcat(normalPaths, ";");
	}
	return result;
}

/////////////////////////////////////////////////////////////////

const std::string upathmanager::resolve (const std::string& file) {
	std::string resolved;
	if (!priorityPaths.empty())
		resolved = ufileresolvewithpaths(file, priorityPaths, false);
	if (resolved.empty())
		resolved = ufileresolvewithpaths(file, normalPaths, true);
	return resolved;
}

//------------------------------------------------------------------

UTILLIB_FUNC bool ufileexists (const std::string& path) {
	FILE* fp = fopen(path.c_str(), "r");
	if (!fp)
		return false;
	else
		{ fclose(fp); return true; }
}

//------------------------------------------------------------------

UTILLIB_FUNC const std::string ufileresolvewithpaths (const std::string& file, const std::list<std::string>& paths, bool tryFileNameToo) {
	if (tryFileNameToo && ufileexists(file))
		return file;
	else {
		for (std::list<std::string>::const_iterator i = paths.begin(); i != paths.end(); ++i) {
			std::string fullPath = *i + "/" + file;
			if (ufileexists(fullPath))
				return fullPath;
		}
		return "";
	}
}

//------------------------------------------------------------------

#ifdef	_UNIX_
#define	HOME_DIRECTORY_WILDCARD		"~"
#endif

#define	DIR_SEPARATOR_CHAR			'/'
#define	DIR_SEPARATOR_STR			"/"

#define	IS_DIR_SEPARATOR_CHAR(c)	((c) == '/' || (c) == '\\')

UTILLIB_FUNC const std::string ucreatepathstring (const std::list<std::string>& path) {
	DASSERT(!path.empty());
	if (path.size() == 1)
		return path.front();
	else {
		std::string result = path.front();
		std::list<std::string>::const_iterator i = path.begin();
		for (++i; i != path.end(); ++i)
			result += std::string(DIR_SEPARATOR_STR) + *i;
		return result;
	}
}

//------------------------------------------------------------------

UTILLIB_FUNC const std::string unormalisepath (const std::string& path) {
	// In case path starts with . or .. it means it is relative to the cwd so
	// we add it explicitly.
	std::list<std::string> l = utokenizestr((path[0] == '.' ? ugetcwd() : std::string("")) + path, "/\\");
	if (l.empty())
		return path;
	else {

#ifdef	_UNIX_
		if (l.front()[0] == '~') {		// User directory.
			uid_t uid;
			bool userFound = true;
			if (l.front().size() == 1)	// It is current user's home dir.
				uid = getuid();
			else {						// It is another user's home dir.
				std::string name = l.front().substr(1);
				struct stat buffer;
				if (!stat(name.c_str(), &buffer))
					uid = buffer.st_uid;
				else
					userFound = false;
			}

			if (userFound)
				if (struct passwd *pws = getpwuid(uid)) {
					l.pop_front();	// Not popped in case of error so as to make the path invalid.
					std::list<std::string> l_new =  utokenizestr(pws->pw_dir, "/\\");
					for (std::list<std::string>::reverse_iterator i = l_new.rbegin(); i != l_new.rend(); ++i)
						l.push_front(*i);
				}
		}
#endif
		std::list<std::string> l_copy;
		for (std::list<std::string>::iterator i = l.begin(); i != l.end(); ++i)
			if (*i == ".")
				;
			else
			if (*i == "..") {
				if (!l_copy.empty())
					l_copy.pop_back();
			}
			else
				l_copy.push_back(*i);

		std::string result(ucreatepathstring(l_copy));

		if (IS_DIR_SEPARATOR_CHAR(path[0]))
			result = DIR_SEPARATOR_STR + result;
		return result;
	}
}

//------------------------------------------------------------------

UTILLIB_FUNC const ustrlist	ufileresolveallwithpaths (const std::string& file, const std::list<std::string>& paths, bool cwdToo) {

	ustrlist			result, pathsCopied(paths);
	ubag<std::string>	dirs;
	
	if (cwdToo)
		pathsCopied.push_back(ugetcwd());
	
	for (std::list<std::string>::iterator i = pathsCopied.begin(); i != pathsCopied.end(); ++i) {
		*i = unormalisepath(*i);
#ifdef	_WIN32_
		ustrlower(*i);	// Case insensitive.
#endif
		if (!dirs.in(*i)) {	// Intentional assignment in argument.
			std::string filePath = *i + DIR_SEPARATOR_CHAR + file;
			if (ufileexists(filePath)) {
				result.push_back(filePath);
				dirs.insert(*i);
			}
		}
	}

	return result;
}

//------------------------------------------------------------------

UTILLIB_FUNC bool ucreatablefilename (const char* path) {

	FILE* fp = fopen(path,"w");

	if (!fp)
		return false;

	fclose(fp);
	unlink(path);

	return true;
}

//------------------------------------------------------------------

#ifdef	_WIN32_

#include <io.h>
#include <windows.h>

UTILLIB_FUNC bool ufilepathsequal (const std::string& path1, const std::string& path2) 
	{ return ustrlower(unormalisepath(path1)) == ustrlower(unormalisepath(path2)); }

UTILLIB_FUNC const std::string ugetcwd (void) {
	static TCHAR dir[MAX_PATH] = "";
	if(!::GetCurrentDirectory(sizeof(dir) - 1, dir))
		return "";
	else
		return dir;
}

UTILLIB_FUNC util_ui32 ufilesize (const char* path) {
	static _finddata_t finfo;

	if (_findfirst(path, &finfo)!=-1)
		return finfo.size;
	else
		return 0;
}

UTILLIB_FUNC time_t ufilechangetime (const std::string& path) {
	WIN32_FIND_DATA fd;
	if (FindFirstFile(path.c_str(), &fd) == INVALID_HANDLE_VALUE)
		return 0;
	else {
		FILETIME& tm = fd.ftLastWriteTime;
		return (((time_t) tm.dwHighDateTime) << 32) | tm.dwLowDateTime;
	}		
}

#endif	//_WIN32_

/////////////////////////////////////////////////////////////////////

#ifdef	_UNIX_

// Has to open the file for reading byte-by-byte (too slow,
// but too portable).
//
UTILLIB_FUNC util_ui32 ufilesize (const char* path) {

	FILE* fp = fopen(path, "r");

	if (!fp)
		return 0;
	else {
		util_ui32	size;
		util_ui8	byte;
		for (size = 0; !feof(fp) && fread(&byte, 1, 1, fp) == 1; ++size)
			{}

		fclose(fp);
		return size;
	}
}

UTILLIB_FUNC bool ufilepathsequal (const std::string& path1, const std::string& path2) 
	{ return path1 == path2; }

UTILLIB_FUNC const std::string ugetcwd (void) {
	static char dir[1024];
	char* result = getcwd(dir, sizeof(dir));
	return result ? result : "";
}

UTILLIB_FUNC time_t ufilechangetime (const std::string& path) {
	struct stat st;
	if (!stat(path.c_str(), &st))
		return st.st_mtime;
	else
		return 0;
}

#endif	//	_UNIX_

///////////////////////////////////////////////////////////////

UTILLIB_FUNC FILE* ubinaryfileopen (const std::string& path, const char* mode) {

	FILE* fp = fopen(path.c_str(), mode);
	if (!fp)
		return (FILE*) 0;
#ifdef	_MSC_VER
	else
	if (setmode(fileno(fp), O_BINARY) == -1) {
		fclose(fp);
		return false;
	}
#endif
	else
		return fp;
}

///////////////////////////////////////////////////////////////

UTILLIB_FUNC bool ustorebinaryfile (const char* path, void* buffer, util_ui32 size) {

	FILE* fp = ubinaryfileopen(path, "w");
	if (!fp)
		return false;
	fwrite(buffer, size, 1, fp);
	fclose(fp);
	return true;
}

///////////////////////////////////////////////////////////////

UTILLIB_FUNC void* uloadbinaryfile (const char* path, util_ui32* size) {

	*size = ufilesize(path);
	if (!*size)
		return (void*) 0;
	else {
		FILE* fp = ubinaryfileopen(path, "r");
		if (!fp) {
			*size = 0;
			return (void*) 0;
		}
		else {
			void* data = new util_ui8[*size];
			fread(data, *size, 1, fp);
			fclose(fp);
			return data;
		}
	}
}

///////////////////////////////////////////////////////////////

UTILLIB_FUNC bool uloadtextfile (const std::string& path, std::string* at) {
	if (FILE* fp = fopen(path.c_str(), "rt")) {
		for (char c = fgetc(fp); c != EOF; c = fgetc(fp))
			(*at) += c;
		fclose(fp);
		return true;
	}
	else
		return false;
}

UTILLIB_FUNC bool ustoretextfile (const std::string& path, const std::string& text) {
	if (FILE* fp = fopen(path.c_str(), "wt")) {
		fputs(text.c_str(), fp);
		fclose(fp);
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////

UTILLIB_FUNC const std::string ufileext (const char* fname) {
	
	if (!*fname)
		return fname;

	util_ui32	len = strlen(fname);
	const char* end = fname + len - 1;

	while (fname != end && *end != '.')
		--end;

	if (fname == end)
		return "";
	else
		return end + 1;
}

UTILLIB_FUNC const std::string ufileext (const std::string& fname)
		{ return ufileext(fname.c_str()); }

///////////////////////////////////////////////////////////////

UTILLIB_FUNC const std::string ufileprefix (const char* fname) {

	if (!*fname)
		return fname;
	
	util_ui32	len = strlen(fname);
	const char* end = fname + len - 1;

	while (fname != end && *end != '.')
		--end;

	if (fname == end)
		return fname;
	else {
		std::string s;
		while (fname != end)
			s += *fname++;
		return s;
	}
}

UTILLIB_FUNC const std::string ufileprefix (const std::string& fname)
	{ return ufileprefix(fname.c_str()); }

///////////////////////////////////////////////////////////////
