/**
 *	PathManager.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "PathManager.h"

namespace fs {

////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(PathManager);

//**********************************************************************

void PathManager::OnInitialize (void)
{
	m_currentDir.AssignCwd();
	m_localDir.AssignHomeDir();
}

//**********************************************************************

void PathManager::OnCleanUp (void)
{

}

//**********************************************************************

void PathManager::AppendLocalDirectory (const String& directory)
{
	m_localDir.AppendDir(directory);
	if (!m_localDir.DirExists())
		m_localDir.Mkdir();
}

//**********************************************************************

const String PathManager::GetLocalDirectory (const String& PORT_UNUSED_PARAM(directory)) const
{
	return wxFileName::GetHomeDir();
}

//**********************************************************************

const String PathManager::GetLocalFile (const String& directory, const String& file) const
{
	wxFileName localFile(m_localDir);
	localFile.AppendDir(directory);

	if (!localFile.DirExists())
		localFile.Mkdir();

	localFile.SetFullName(file);
	return localFile.GetFullPath();
}

//**********************************************************************

const String PathManager::GetTempDirectory (void)
{
	return wxFileName::GetTempDir();
}

////////////////////////////////////////////////////////////////////////

} // namespace timer
