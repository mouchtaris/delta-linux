// DeltaUnparsedCallsInStmtHolder.h
// Part of program debug information.
// Holder of DeltaUnparsedCallsInStmt instances.
// Defined mainly to support selective step-in.
// ScriptFighter Project.
// A. Savidis, September 2008.
//
#ifndef	DELTAUNPARSEDCALLSINSTMTHOLDER_H
#define	DELTAUNPARSEDCALLSINSTMTHOLDER_H

#include "DDebug.h"
#include "utypes.h"
#include "GenericReader.h"
#include "GenericWriter.h"
#include "DeltaStdDefs.h"
#include "DeltaUnparsedCallsInStmt.h"
#include <map>
#include <list>

///////////////////////////////////////////////////////

class DBYTECODE_CLASS DeltaUnparsedCallsInStmtHolder {

	private:
	typedef std::map<util_ui32, DeltaUnparsedCallsInStmt*> StmtMap;
	StmtMap stmts;

	///////////////////////////////////////////////////////

	public:
	void							WriteText (FILE* fp) const;				// COMP
	bool							Read (GenericReader& reader);			// DBG
	void							Write (GenericWriter& writer) const;	// COMP

	void							GetLines (std::list<util_ui32>& lines) const;
	util_ui16						GetTotal (void) const
										{ return (util_ui16) stmts.size(); }

	bool							Has (util_ui32 line) const
										{ return stmts.find(line) != stmts.end(); }

	void							New (util_ui32 line);
	void							Delete (util_ui32 line);
	DeltaUnparsedCallsInStmt*		Get (util_ui32 line);			// COMP
	const DeltaUnparsedCallsInStmt&	Get (util_ui32 line) const;		// DBG

	void							Clear (void);

	DeltaUnparsedCallsInStmtHolder (void);
	DeltaUnparsedCallsInStmtHolder (const DeltaUnparsedCallsInStmtHolder& holder);
	~DeltaUnparsedCallsInStmtHolder();
};

///////////////////////////////////////////////////////

#endif	// Do not ad stuff beyond this point.


