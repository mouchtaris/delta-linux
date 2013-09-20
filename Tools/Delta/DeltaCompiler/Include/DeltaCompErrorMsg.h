// DeltaCompErrorMsg.h
// Basic error reporting.
// Script Fighter Project.
// A. Savidis, October 1999.
//

#ifndef	DELTACOMPERRORMSG_H
#define	DELTACOMPERRORMSG_H

#include "CompilerComponentDirectory.h"
#include "utypes.h"
#include "ucallbacks.h"
#include "DeltaCompilerDefs.h"
#include "ASTChainOfSourceLineOriginInfo.h"

///////////////////////////////////////////////////////////////

class DCOMPLIB_CLASS DeltaCompilerMessenger : public ucomponentdirectoryclient {

	public:
	typedef ucallbackwithclosure<void (*)(const char*, void*)>	ErrorCallback;

	private:
	std::string							srcFile;
	util_ui32							nodeId;			//0 means no node information
	AST::ChainOfSourceLineOriginInfo	sourceOriginChain;
	ErrorCallback						notifyError;
	util_ui32							errors;
	util_ui32							warnings;

	public:
	void				Error (const char* format,...);
	void				Warning (const char* format,...);
	void				Msg (const char* format,...);

	void				ResetErrors (void);
	void				ResetWarnings (void);

	util_ui32			ErrorsExist (void) const;
	util_ui32			WarningsExist (void) const;

	void				SetErrorCallback (ErrorCallback callback);
	ErrorCallback		GetErrorCallback (void) const;

	void				SetCurrentFile (const std::string& file);
	void				SetChainOfSourceLineOriginInfo (const AST::ChainOfSourceLineOriginInfo& references = AST::ChainOfSourceLineOriginInfo());

	static const std::string OriginChainStr (const AST::ChainOfSourceLineOriginInfo& srcReferences);

	DeltaCompilerMessenger (ucomponentdirectory* directory) : 
		ucomponentdirectoryclient	(directory),
		errors						(0),
		warnings					(0),
		nodeId						(0) {}
	~DeltaCompilerMessenger() {}
};

///////////////////////////////////////////////////////////////

#define COMPMESSENGER_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), DeltaCompilerMessenger)))

#define COMPMESSENGER	COMPMESSENGER_EX(GET_COMPONENT_DIRECTORY())

///////////////////////////////////////////////////////////////

#endif	/* Do not add stuff beyond this point. */