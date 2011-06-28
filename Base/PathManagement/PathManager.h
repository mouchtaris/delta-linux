/**
 *	PathManager.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#ifndef FS_PATHMANAGER_H
#define FS_PATHMANAGER_H

#include "Common.h"
#include "LibraryAPIDecls.h"
#include "Singleton.h"

#include <wx/filename.h>

namespace fs {

////////////////////////////////////////////////////////////////////////

class _BASE_API PathManager : public util::Singleton {
	DECLARE_SINGLETON(PathManager);

public:
	void AppendLocalDirectory (const String& directory);

	const String GetLocalDirectory (const String& directory) const;
	const String GetLocalFile (const String& directory, const String& file) const;
	const String GetTempDirectory (void);

protected:
	void OnInitialize (void);
	void OnCleanUp (void);

private:
	PathManager (void) {}
	~PathManager (void) {}

	wxFileName m_currentDir;
	wxFileName m_localDir;
};

////////////////////////////////////////////////////////////////////////

} // namespace fs

#endif // FS_PATHMANAGER_H
