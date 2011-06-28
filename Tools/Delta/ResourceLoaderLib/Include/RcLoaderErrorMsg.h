// RcLoaderErrorMsg.h
// A simple error reporting API.
// ResourceLoader.
// ScriptFighter Project.
// A. Savidis, October 1999.
//

#ifndef	RCLOADERERRORMSG_H
#define	RCLOADERERRORMSG_H

#include <string>

extern void 				ResourceLoaderError (const char* format,...);
extern void 				ResourceLoaderMsg (const char* format,...);
extern void					ResourceLoaderResetErrors (void);
extern bool					ResourceLoaderErrorsExist (void);
extern void 				ResourceLoaderSetErrorCallback (void (*callback)(const char*));
extern void 				ResourceLoaderSetMsgCallback (void (*callback)(const char*));
extern const std::string	ResourceLoaderGetError (void);

#endif	// Do not add stuff beyond this point.