// DeltaDebugExtensionsSuper.h
// Contains all functions needed by the VM when a debugger API
// is enabled, that are called inside the basic VM functionality.
// Script Fighter Project.
// A. Savidis, June 2007.
//

#ifndef	DELTADEBUGEXTENSIONSSUPER_H
#define	DELTADEBUGEXTENSIONSSUPER_H

#include "DeltaValueTypes.h"

/////////////////////////////////////////////////////////////////

class DVM_CLASS DeltaDebugExtensionsSuper {

	friend class DeltaVirtualMachine;

	public:
	typedef DeltaDebugExtensionsSuper* (*ConstructorFunc) (DeltaVirtualMachine*);

	private:
	static ConstructorFunc ctor;

	public:
	virtual void	OnStartingExecution (void) = 0;
	virtual void	OnEndingExecution (void) = 0;
	virtual void	OnBlockEnter (bool isMainProgramBlock, util_ui16 blockId) = 0;
	virtual void	OnBlockExit (bool isMainProgramBlock, util_ui16 blockId) = 0;
	virtual bool	OnAssertionFailure (const std::string& expr) = 0;
	virtual void	OnThrow (void) = 0;

	virtual void	OnStackUnWindingProgramFunction (void) = 0;
	virtual void	OnStackUnWindingLibraryFunction (void) = 0;
	virtual void	OnStackUnWindingGlobalCode (void) = 0;

	virtual void	OnCallingLibraryFunction (DeltaLibraryFunc func) = 0;
	virtual void	OnCallingProgramFunction (const std::string& name) = 0;
	virtual void	OnReturnLibraryFunction (DeltaLibraryFunc func) = 0;
	virtual void	OnReturnProgramFunction (const std::string& name) = 0;
	virtual void	OnInitLinesOfCodes (util_ui16 codeSize) = 0;
	virtual void	OnReadingLineWithCode (DeltaCodeAddress addr, util_ui16 line) = 0;
	virtual void	OnExecuteHandleErrorBreak (void) = 0;
	virtual void	OnVirtualMachineDestruction (void) = 0;
	virtual void	OnExecutionError (void) = 0;
	virtual void	OnStartLoading (void) = 0;
	virtual void	OnFinishedLoading (void) = 0;

	enum	CheckBreakPointStatus { 
				ReturnFalse		= 0, 
				ReturnTrue		= 1, 
				FallThrough		= 2
	};

	virtual CheckBreakPointStatus		OnExecuteCheckBreakPoints (void) = 0;
	static DeltaDebugExtensionsSuper*	Construct (DeltaVirtualMachine* vm);

	public:
	static ConstructorFunc				GetConstructor (void);
	static void							SetConstructor (ConstructorFunc f);
	DeltaDebugExtensionsSuper (void) {}
	virtual ~DeltaDebugExtensionsSuper(){}
};

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
