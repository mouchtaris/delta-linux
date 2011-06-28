// DeltaUnparsedCallsInStmt.h
// Part of program debug information.
// Information for all function calls of a sttmt in unparsed form.
// Defined mainly to support selective step-in.
// ScriptFighter Project.
// A. Savidis, September 2008.
//

#ifndef	DELTAUNPARSEDCALLSINSTMT_H
#define	DELTAUNPARSEDCALLSINSTMT_H

#include "DDebug.h"
#include "utypes.h"
#include "GenericReader.h"
#include "GenericWriter.h"
#include "DeltaStdDefs.h"
#include "DeltaUnparsedCall.h"
#include <list>

///////////////////////////////////////////////////////

class DBYTECODE_CLASS DeltaUnparsedCallsInStmt {
	
	private:
	std::list<DeltaUnparsedCall>	calls;
	util_ui32						line;
	
	///////////////////////////////////////////////////////
	public:
	void								WriteText (FILE* fp) const;				// COMP
	bool								Read (GenericReader& reader);			// DBG
	void								Write (GenericWriter& writer) const;	// COMP

	std::list<DeltaUnparsedCall>&		GetAllCalls (void)
											{ return calls; }

	void								GetCallsAfterInstruction (
											DeltaCodeAddress				addr,
											std::list<DeltaUnparsedCall>&	output
										) const;

	util_ui32							GetLine (void) const
											{ return line; }
	void								SetLine (util_ui32 l)
											{ line = l; }

	util_ui16							GetTotal (void) const
											{ return (util_ui16) calls.size(); }

	void								Add (const DeltaUnparsedCall& call)
											{ calls.push_back(call); }

	void								Clear (void)	
											{ calls.clear(); line = 0; }

	DeltaUnparsedCallsInStmt (util_ui32 _line = 0): line(_line) {}
	DeltaUnparsedCallsInStmt (const DeltaUnparsedCallsInStmt& stmt);
	~DeltaUnparsedCallsInStmt(){}
};

///////////////////////////////////////////////////////

#endif	// Do not ad stuff beyond this point.


