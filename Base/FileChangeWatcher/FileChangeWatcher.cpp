/**
 *	FileChangeWatcher.cpp
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	July 2009
 */

#include "FileChangeWatcher.h"
#include <set>
#include <windows.h>
#include <wx/filename.h>

#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/foreach.hpp>

namespace util {

////////////////////////////////////////////////////////////////////////
// DirectoryWatcher

class DirectoryWatcher {
public:
	typedef FileChangeWatcher::Callback Callback;
private:
	DirectoryWatcher (const String& path, HANDLE hFolder, HANDLE hCompPort);
public:
	~DirectoryWatcher ();

	static DirectoryWatcher* New (const String& path);

	//**********************************************************************

	unsigned long long AddFileWatch (const String& filename, const Callback& callback);
	void RemoveFileWatch (unsigned long long watch);
	bool HasFileWatches (void) { return !watches.empty(); }

private:
	void Clear			(void);
	void WatchChanges	(void);

	typedef std::pair<String, Callback> WatchInfo;
	typedef std::list<WatchInfo *> WatchInfoList;

	String					path;
	HANDLE					hFolder;
	HANDLE					hCompPort;
	WatchInfoList			watches;
	boost::thread *			thread;
	boost::recursive_mutex	mutex;
};

//**********************************************************************

DirectoryWatcher::DirectoryWatcher (const String& path, HANDLE hFolder, HANDLE hCompPort) :
	path(path), hFolder(hFolder), hCompPort(hCompPort)
{
	thread = new boost::thread(boost::bind(&DirectoryWatcher::WatchChanges, this));
}

//**********************************************************************

DirectoryWatcher::~DirectoryWatcher () {
	Clear();
	PostQueuedCompletionStatus(hCompPort, 0, 0, NULL);
	thread->join();
	delete thread;
}

//**********************************************************************

unsigned long long DirectoryWatcher::AddFileWatch (const String& filename, const Callback& callback) {
	WatchInfo *info = new WatchInfo(filename, callback);
	{
		boost::recursive_mutex::scoped_lock lock(mutex);
		watches.push_back(info);
	}
	return (unsigned long long) info;
}

//**********************************************************************

void DirectoryWatcher::RemoveFileWatch (unsigned long long watch) {
	WatchInfo *info = (WatchInfo *) watch;
	WatchInfoList::iterator i = std::find(watches.begin(), watches.end(), info);
	assert(i != watches.end());
	{
		boost::recursive_mutex::scoped_lock lock(mutex);
		watches.erase(i);
		delete info;
	}
}

//**********************************************************************

DirectoryWatcher* DirectoryWatcher::New (const String& path) {
	HANDLE hFolder = CreateFile(
		path,
		FILE_LIST_DIRECTORY, 
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL
	);
	if (hFolder == INVALID_HANDLE_VALUE)
		return (DirectoryWatcher *) 0;
	else {
		HANDLE hCompPort = CreateIoCompletionPort(hFolder, NULL, (ULONG_PTR) hFolder, 0);
		DirectoryWatcher* watcher = new DirectoryWatcher(path, hFolder, hCompPort);
		PostQueuedCompletionStatus(hCompPort, sizeof(HANDLE), (ULONG_PTR) hFolder, NULL);
		return watcher;
	}
}

//**********************************************************************

void DirectoryWatcher::Clear (void) {
	boost::recursive_mutex::scoped_lock lock(mutex);
	std::for_each(watches.begin(), watches.end(), util::deleter<WatchInfo>());
	watches.clear();
}

//**********************************************************************

#define BUFFER_SIZE	4096

void DirectoryWatcher::WatchChanges (void)
{
	const DWORD flags = FILE_NOTIFY_CHANGE_FILE_NAME	|
						FILE_NOTIFY_CHANGE_ATTRIBUTES	|
						FILE_NOTIFY_CHANGE_SIZE			|
						FILE_NOTIFY_CHANGE_LAST_WRITE	|
						FILE_NOTIFY_CHANGE_CREATION		;
	while(true) {
		DWORD numBytes;
		LPOVERLAPPED lpOverlapped;
		ULONG_PTR completionKey;

		if (!GetQueuedCompletionStatus(hCompPort, &numBytes, &completionKey, &lpOverlapped, INFINITE))
			break;
		assert(!completionKey || completionKey == (ULONG_PTR) hFolder);
		if (!completionKey)
			break;

		TCHAR buffer[BUFFER_SIZE] = {0}; 
		DWORD dwOffset = 0;
		FILE_NOTIFY_INFORMATION* pInfo = NULL;
		OVERLAPPED overlapped;
		memset(&overlapped, 0, sizeof(overlapped));
		if (!ReadDirectoryChangesW(hFolder, buffer, BUFFER_SIZE, FALSE, flags, NULL, &overlapped, NULL))
			break;
		{
			boost::recursive_mutex::scoped_lock lock(mutex);
			std::set<const WatchInfo*> changed;
			do {
				pInfo = (FILE_NOTIFY_INFORMATION*)((LPBYTE)buffer + dwOffset);
				if(	pInfo->Action == FILE_ACTION_MODIFIED			||
					pInfo->Action == FILE_ACTION_ADDED				||
					pInfo->Action == FILE_ACTION_RENAMED_NEW_NAME
				) {
					WCHAR wcFileName[MAX_PATH + 1] = {0};
					memcpy(wcFileName, pInfo->FileName, min((MAX_PATH * sizeof(WCHAR)), pInfo->FileNameLength));
					const String name(wcFileName);
					BOOST_FOREACH(const WatchInfo* watch, watches)
						if (wxFileName(name).SameAs(watch->first))
							changed.insert(watch);	// multiple notifications may occur so first gather all
													// unique changes and then invoke their callbacks
				}
				dwOffset += pInfo->NextEntryOffset;
			} while (pInfo->NextEntryOffset != 0);
			
			BOOST_FOREACH(const WatchInfo* watch, changed)
				watch->second();
		}
	}
	if (hFolder != INVALID_HANDLE_VALUE)
		CloseHandle(hFolder);
	CloseHandle(hCompPort);
}

////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(FileChangeWatcher);

//**********************************************************************

unsigned long long FileChangeWatcher::Register (const String& uri, const Callback& callback)
{
	wxFileName filename(uri);
	const String path = filename.GetPath();
	if (watchers.find(path) == watchers.end())
		if (DirectoryWatcher* watcher = DirectoryWatcher::New(path))
			watchers[path] = watcher;
		else
			return 0;
	return watchers[path]->AddFileWatch(filename.GetFullName(), callback);
}

//**********************************************************************

void FileChangeWatcher::Cancel (const String& uri, unsigned long long watch)
{
	wxFileName filename(uri);
	WatcherMap::iterator i = watchers.find(filename.GetPath());
	assert(i != watchers.end());
	i->second->RemoveFileWatch(watch);
	if (!i->second->HasFileWatches()) {
		delete i->second;
		watchers.erase(i);
	}
}

////////////////////////////////////////////////////////////////////////

} // namespace util