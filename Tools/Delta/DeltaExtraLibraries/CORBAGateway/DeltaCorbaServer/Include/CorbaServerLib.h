#ifndef	CORBA_SERVER_LIB_H
#define	CORBA_SERVER_LIB_H

#define CORBA_SERVER_NATIVE_INDEX	"@server"

class DeltaObject;

extern void Install_DeltaCorbaServer_Lib (DeltaObject *);
extern void CleanUp_DeltaCorbaServer_Lib (void);

#endif