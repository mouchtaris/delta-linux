/**
 *	FileChangeWatcher.h
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	July 2009
 */

#ifndef FILE_CHANGE_WATCHER_H
#define FILE_CHANGE_WATCHER_H

#include "Common.h"
#include "LibraryAPIDecls.h"
#include "Singleton.h"

#include <map>
#include <boost/function.hpp>

namespace util {

class DirectoryWatcher;

////////////////////////////////////////////////////////////////////////

class _BASE_API FileChangeWatcher : public util::Singleton {
	DECLARE_SINGLETON(FileChangeWatcher);

public:
	typedef boost::function<void (void)> Callback;

	unsigned long long	Register(const String& uri, const Callback& callback);
	void				Cancel	(const String& uri, unsigned long long watch);

private:
	typedef std::map<String, DirectoryWatcher*> WatcherMap;
	WatcherMap watchers;
};

////////////////////////////////////////////////////////////////////////

} // namespace util

#endif // FILE_CHANGE_WATCHER_H