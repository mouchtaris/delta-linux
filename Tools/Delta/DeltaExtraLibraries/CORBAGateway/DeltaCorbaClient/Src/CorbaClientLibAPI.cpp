#include "CorbaClientLib.h"
#include "CorbaClientLibDefs.h"
#include "CorbaCommonLib.h"
#include "udynamiclibloader.h"

CORBACLIENT_CFUNC void* Install (void) {
	Install_DeltaCorbaCommon_Lib();
	Install_DeltaCorbaClient_Lib(GetCorbaAPI());
	return udynamiclibloader::New("CleanUp", true);	 // Explicitly supply a cleaner.
}

CORBACLIENT_CFUNC void* CleanUp (void) {
	CleanUp_DeltaCorbaClient_Lib();
	CleanUp_DeltaCorbaCommon_Lib();
	return udynamiclibloader::New("dll cleared");
}