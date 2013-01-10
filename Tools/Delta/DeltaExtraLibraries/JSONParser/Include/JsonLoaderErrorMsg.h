#ifndef	JSONLOADERERRORMSG_H
#define	JSONLOADERERRORMSG_H

#include <string>

namespace JsonParserLoaderErrorMsg {

extern void 				JsonLoaderError (const char* format,...);
extern void 				JsonLoaderMsg (const char* format,...);
extern void					JsonLoaderResetErrors (void);
extern int					JsonLoaderNumErrors (void);
extern void 				JsonLoaderSetErrorCallback (void (*callback)(const char*));
extern void 				JsonLoaderSetMsgCallback (void (*callback)(const char*));
extern const std::string	JsonLoaderGetError (void);

}

#endif
