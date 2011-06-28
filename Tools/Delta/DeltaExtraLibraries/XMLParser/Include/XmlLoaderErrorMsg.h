#ifndef	XMLLOADERERRORMSG_H
#define	XMLLOADERERRORMSG_H

#include <string>

extern void 				XmlLoaderError (const char* format,...);
extern void 				XmlLoaderMsg (const char* format,...);
extern void					XmlLoaderResetErrors (void);
extern int					XmlLoaderNumErrors (void);
extern void 				XmlLoaderSetErrorCallback (void (*callback)(const char*));
extern void 				XmlLoaderSetMsgCallback (void (*callback)(const char*));
extern const std::string	XmlLoaderGetError (void);

#endif
