#ifndef	CORBA_COMMON_LIB_H
#define	CORBA_COMMON_LIB_H

#include "CorbaCommonLibDefs.h"

#define CORBA_NAMESPACE			"corba"
#define CORBA_NAMESPACE_PREFIX	"corba::"

#define ERROR_IF_CORBA_NOT_INITIALIZED														\
	do {																					\
		if (!corba::IsInitialized()) {														\
			DPTR(vm)->PrimaryError(															\
				"corba library is not initialized (must call 'corba_init' before '%s')",	\
				CURR_FUNC																	\
			);																				\
			DLIB_RESET_RETURN;																\
		}																					\
	} while (false)

class DeltaObject;

CORBACOMMON_FUNC void Install_DeltaCorbaCommon_Lib (void);
CORBACOMMON_FUNC void CleanUp_DeltaCorbaCommon_Lib (void);

CORBACOMMON_FUNC DeltaObject * GetCorbaAPI (void);

#endif