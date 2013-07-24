/*
 *	ProjectManagerCommon.h
 *	Common stuff across project manager files.
 *	Anthony Savidis, November 2010
 *	Functionality moved here from original code from Yannis Lilis.
 *	Additions may be thematically incorporated depending on what they extend
 *	to appropriate existing or new namespaces. 
 */
#ifndef PROJECT_MANAGER_COMMON_H
#define PROJECT_MANAGER_COMMON_H

#include "Common.h"
#include "Properties.h"
#include "ExtProperties.h"
#include "PropertyUtils.h"
#include "Component.h"

#include <string>

///////////////////////////////////////////////////////////
// DEFINE DLL EXPORT //////////////////////////////////////

#ifdef PROJECTMANAGER_EXPORTS
#	define _PROJECT_MANAGER_API		UTIL_EXPORT
#	define _PROJECT_MANAGER_C_API	UTIL_C_EXPORT
#else
#	define _PROJECT_MANAGER_API		UTIL_IMPORT
#	define _PROJECT_MANAGER_C_API	UTIL_C_IMPORT
#endif

#pragma warning(disable: 4251)

///////////////////////////////////////////////////////////

#define DELTA_APPLICATION							_T("Delta Application")
#define DELTA_LIBRARY								_T("Delta Library")
#define HOST_APPLICATION_ACTIVATOR					_T("Host Application Activator")
#define	DELTA_SCRIPT_DEPLOYMENT_APPLICATION			"application"
#define	DELTA_SCRIPT_DEPLOYMENT_LIBRARY				"library"
#define	DELTA_SCRIPT_DEPLOYMENT_DEFAULT_LIBRARY		1

namespace conf {
	_PROJECT_MANAGER_API Property*				GenerateLibraryProperty (void);
	_PROJECT_MANAGER_API EnumStringProperty*	GenerateScriptDeploymentProperty (void);
	_PROJECT_MANAGER_API void					AddProjectBuildProperties (ide::Component* comp); 
	_PROJECT_MANAGER_API void					AddScriptBuildProperties (ide::Component* comp); 
	_PROJECT_MANAGER_API void					AddScriptLastBuildProperties (conf::PropertyTable* props);
	_PROJECT_MANAGER_API void					AddScriptStageSourceProperties (conf::PropertyTable* props);
	_PROJECT_MANAGER_API void					AddScriptExecutionProperties (ide::Component* comp); 
	_PROJECT_MANAGER_API const char **			GetScriptPropertyIdsForStageSources (void);
	_PROJECT_MANAGER_API const char **			GetProjectBuildPropertyIdsForScripts (void);
	_PROJECT_MANAGER_API const char **			GetScriptBuildPropertyIds (void);
	_PROJECT_MANAGER_API const char **			GetScriptLastBuildPropertyIds (void);
	_PROJECT_MANAGER_API const char **			GetScriptExecutionPropertyIds (void);
	_PROJECT_MANAGER_API const char *			GetByteCodePathPropertyId (void);
	_PROJECT_MANAGER_API const char *			GetDllImportPathPropertyId (void);
	_PROJECT_MANAGER_API const char *			GetDeploymentPropertyId (void);
}

///////////////////////////////////////////////////////////

#endif	//PROJECT_MANAGER_COMMON_H