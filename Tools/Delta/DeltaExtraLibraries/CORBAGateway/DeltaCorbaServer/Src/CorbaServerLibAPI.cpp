#include "CorbaServerLib.h"
#include "CorbaServerLibDefs.h"
#include "CorbaCommonLib.h"
#include "udynamiclibloader.h"

CORBASERVER_CFUNC void* Install (void) {
	Install_DeltaCorbaCommon_Lib();
	Install_DeltaCorbaServer_Lib(GetCorbaAPI());
	return udynamiclibloader::New("CleanUp", true);	 // Explicitly supply a cleaner.
}

CORBASERVER_CFUNC void* CleanUp (void) {
	CleanUp_DeltaCorbaServer_Lib();
	CleanUp_DeltaCorbaCommon_Lib();
	return udynamiclibloader::New("dll cleared");
}