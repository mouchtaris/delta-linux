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

///////////////////////////////////////////////////////////////

class DCOMPLIB_CLASS DeltaCompilerMessenger {

	USE_COMPILER_COMPONENT_DIRECTORY();

	public:
	typedef ucallbackwithclosure<void (*)(const char*, void*)>	ErrorCallback;
	typedef std::pair<std::string, util_ui32>					SourceReference;
	typedef std::list<SourceReference>							SourceReferenceList;

	private:
	std::string			srcFile;
	util_ui32			nodeId;			//0 means no node information
	SourceReferenceList	srcReferences;
	ErrorCallback		notifyError;
	util_ui32			errors;
	util_ui32			warnings;

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
	void				SetSourceReferences (const SourceReferenceList& references = SourceReferenceList());

	static const std::string ReferencesStr (const SourceReferenceList& srcReferences);

	DeltaCompilerMessenger (void) : 
		errors		(0),
		warnings	(0),
		nodeId		(0) {}
	~DeltaCompilerMessenger() {}
};

///////////////////////////////////////////////////////////////

#define COMPMESSENGER_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), DeltaCompilerMessenger)))

#define COMPMESSENGER	COMPMESSENGER_EX(COMPONENT_DIRECTORY())

///////////////////////////////////////////////////////////////

#endif	/* Do not add stuff beyond this point. */