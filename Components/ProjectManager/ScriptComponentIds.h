/**
 *	ScriptComponentIds.h
 *  Script-specific component ids.
 *	Anthony Savidis
 *	April 2007
 */
#ifndef PROJECT_MANAGER_SCRIPT_COMPONENT_IDS_H
#define PROJECT_MANAGER_SCRIPT_COMPONENT_IDS_H

#include <string>

#define	SPW_COMPONENT_ID_SCRIPT_SOURCE	"Script"
#define	SPW_COMPONENT_ID_STAGE_SOURCE	"StageSource"
#define	SPW_COMPONENT_ID_STAGE_RESULT	"StageResult"
#define	SPW_COMPONENT_ID_ASPECT			"Aspect"
#define	SPW_COMPONENT_ID_ASPECT_RESULT	"AspectResult"

// is a script having a source file explicitly associated
inline bool IS_EXPLICIT_SCRIPT_SOURCE_COMPONENT_ID (const std::string& id) {
	return id == SPW_COMPONENT_ID_SCRIPT_SOURCE	||	
		   id == SPW_COMPONENT_ID_ASPECT;
}

// is a script having whose source file is produced as a result
inline bool IS_RESULTING_SCRIPT_SOURCE_COMPONENT_ID (const std::string& id) {
	return id == SPW_COMPONENT_ID_STAGE_RESULT	||	
		   id == SPW_COMPONENT_ID_ASPECT_RESULT;
}

// is any sort of script
inline bool IS_SCRIPT_COMPONENT_ID (const std::string& id) {
	return id == SPW_COMPONENT_ID_SCRIPT_SOURCE	||	
		   id == SPW_COMPONENT_ID_STAGE_SOURCE	||	
		   id == SPW_COMPONENT_ID_STAGE_RESULT	||	
		   id == SPW_COMPONENT_ID_ASPECT		||
		   id == SPW_COMPONENT_ID_ASPECT_RESULT;
}

#endif	//PROJECT_MANAGER_SCRIPT_COMPONENT_IDS_H

