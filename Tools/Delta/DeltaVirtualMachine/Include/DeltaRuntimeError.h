// VM error reporting.
// Original version in 1999.
// Re-factored to encapsulate the functionality inside the
// VM class. The macros are needed for compatibility as most of
// the code relies on the use as a C-style API.
// ScriptFighter Project.
// A. Savidis, Februart 2006.
// 

#ifndef	DELTARUNTIMEERROR_H
#define	DELTARUNTIMEERROR_H

#define	DeltaRunTimeError				DeltaVirtualMachine::RunTimeError
#define	DeltaRunTimeWarning				DeltaVirtualMachine::RunTimeWarning
#define	DeltaResetRunTimeErrors			DeltaVirtualMachine::ResetRunTimeErrors
#define	DeltaRunTimeErrorsExist			DeltaVirtualMachine::RunTimeErrorsExist
#define	DeltaSetRunTimeWarningrCallback	DeltaVirtualMachine::SetWarningCallback
#define	DeltaSetRunTimeErrorCallback	DeltaVirtualMachine::SetErrorCallback

#endif	/* Do not add stuff beyond this point. */