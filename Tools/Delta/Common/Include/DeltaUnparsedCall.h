// DeltaUnparsedCall.h
// Part of program debug information.
// Information for function calls in fully unparsed form, with the exception
// of introducing the target code leading instruction label.
// Defined mainly to support selective step-in.
// ScriptFighter Project.
// A. Savidis, September 2008.
//

#ifndef	DELTAUNPARSEDCALL_H
#define	DELTAUNPARSEDCALL_H

#include "DDebug.h"
#include "utypes.h"
#include "GenericReader.h"
#include "GenericWriter.h"
#include "DeltaStdDefs.h"
#include <string>

///////////////////////////////////////////////////////

class DBYTECODE_CLASS DeltaUnparsedCall {

	private:
	std::string			func;
	std::string			args;
	DeltaCodeAddress	label;
	
	///////////////////////////////////////////////////////

	public:
	void				WriteText (FILE* fp) const;		// COMP
	bool				Read (GenericReader& reader);	// DBG
	void				Write (GenericWriter& writer) const; // COMP

	const std::string&	GetFunction (void) const
							{ return func; }
	void				SetFunction (const std::string& s) 
							{ func = s; }

	const std::string&	GetArguments (void) const
							{ return args; }
	void				SetArguments (const std::string& s) 
							{ args = s; }
	
	DeltaCodeAddress	GetLabel (void) const
							{ return label; }
	void				SetLabel (DeltaCodeAddress& _label)
							{ label = _label; }

	DeltaUnparsedCall (void){}
	DeltaUnparsedCall (const std::string& _func, const std::string& _args, DeltaCodeAddress _label) : 
		func(_func), 
		args(_args),
		label(_label){}
	~DeltaUnparsedCall(){}
};

///////////////////////////////////////////////////////

#endif	// Do not ad stuff beyond this point.


