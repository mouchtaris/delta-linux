// ufiles.h
// File specific utilities.
// Moved from the central delta util here.
// ScriptFighter Project.
// A. Savidis, July 2002.
//

#ifndef	UFILES_H
#define	UFILES_H

#include <stdio.h>
#include "DDebug.h"
#include "ustrings.h"
#include "utypes.h"
#include <list>
#include <string>

//---------------------------------------------------------------

class UTILLIB_CLASS upathmanager {
	private:
	ustrlist normalPaths, priorityPaths;

	public:
	void				add (const std::string& path, bool prioritised);
	void				add_environment_path (const std::string& env, bool prioritised);
	const std::string	make_entire_path (void);
	const std::string	resolve (const std::string& file);
	const ustrlist&		get_normal_paths (void) const 
							{ return normalPaths; }
	const ustrlist&		get_priority_paths (void) const 
							{ return priorityPaths; }
	bool				has_paths (void) const
							{ return !normalPaths.empty() || !priorityPaths.empty(); }
	void				clear_normal_paths (void)
							{ normalPaths.clear(); }
	void				clear_priority_paths (void)
							{ priorityPaths.clear(); }
	void				clear_all_paths (void)
							{ clear_normal_paths(); clear_priority_paths(); }

	upathmanager(void){}
	~upathmanager(){}
};

/////////////////////////////////////////////////////////////////

UTILLIB_FUNC bool					ufileexists (const std::string& path);
UTILLIB_FUNC const std::string		ufileresolvewithpaths (
										const std::string&				file, 
										const std::list<std::string>&	paths,
										bool							tryFileNameToo = true
									);
UTILLIB_FUNC const ustrlist			ufileresolveallwithpaths (
										const std::string&				file, 
										const std::list<std::string>&	paths,
										bool							cwdToo = true
									);
UTILLIB_FUNC const std::string		ucreatepathstring (const std::list<std::string>& path);
UTILLIB_FUNC const std::string		unormalisepath (const std::string& path);
UTILLIB_FUNC bool					ucreatablefilename (const char* path);
UTILLIB_FUNC const std::string		ufileprefix (const char* fname);
UTILLIB_FUNC const std::string		ufileprefix (const std::string& fname);
UTILLIB_FUNC const std::string		ufileext (const char* fname);
UTILLIB_FUNC const std::string		ufileext (const std::string& fname);
UTILLIB_FUNC util_ui32				ufilesize (const char* path);	// Calculates file size (too slow).
UTILLIB_FUNC time_t					ufilechangetime (const std::string& path);
UTILLIB_FUNC bool					ustorebinaryfile (const char* path, void* buffer, util_ui32 size);
UTILLIB_FUNC void*					uloadbinaryfile (const char* path, util_ui32* size);	// Loads a file into memory. Delete via VPDEL().
UTILLIB_FUNC bool					uloadtextfile (const std::string& path, std::string* at);
UTILLIB_FUNC FILE*					ubinaryfileopen (const std::string& path, const char* mode);
UTILLIB_FUNC bool					ufilepathsequal (const std::string& path1, const std::string& path2);
UTILLIB_FUNC const std::string		ugetcwd (void);
inline bool							uisabsolutepath (const std::string& path)
										{ char c = path[0]; return c == '~' || c == '/' || c == '\\'; }

/////////////////////////////////////////////////////////////////

template <class T>
bool ufwrite (const T& var, FILE* fp) {
	return fwrite(&var, sizeof(T), 1, fp) == 1;
}

template <class T>
bool ufread (T* var, FILE* fp) {
	return fread(var, sizeof(T), 1, fp) == 1;
}

template <class T> void* ubufferwrite(void* p, util_ui32* n, const T& j) {
	DASSERT(*n >= sizeof(T));
	memcpy(p, &j, sizeof(T));
	*n -= sizeof(T);
	return POFFS(p, j);
}

template <> 
inline void* ubufferwrite<std::string>(void* p, util_ui32* n, const std::string& s) {
	util_ui32 len = (util_ui32) s.length() + 1;
	p = ubufferwrite(p, n, len);
	DASSERT(*n >= len);

	memcpy(p, s.c_str(), len);
	*n -= len;
	return POFFS(p, len);
}

//***************************

template <class T> bool ustorebinaryobject (T* v, const char* path) {
	if (!v)
		return false;
	FILE* fp = ubinaryfileopen(path, "w");
	if (!fp)
		return false;
	bool result = ufwrite(*v,  fp);
	fclose(fp);
	return result;
}

//***************************

template <class T> bool uloadbinaryobject (T* v, const char* path) {
	if (!v)
		return false;
	FILE* fp = ubinaryfileopen(path, "r");
	if (!fp)
		return false;
	bool result = ufread(v, fp);
	fclose(fp);
	return result;
}

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
