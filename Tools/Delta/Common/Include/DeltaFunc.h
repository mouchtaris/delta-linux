// DeltaFunc.h
// Debug information for functions.
// ScriptFighter Project.
// A. Savidis, February 2005.
//

#ifndef	DELTAFUNC_H
#define	DELTAFUNC_H

#include <string.h>
#include "DDebug.h"
#include "utypes.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaBlock.h"
#include "DeltaVars.h"
#include "GenericReader.h"
#include "GenericWriter.h"
#include <string>

class DBYTECODE_CLASS DeltaDebugFuncInfo {

	private:
	std::string			name;			// User-defined name.
	std::string			fullPath;		// In case contained in other functions.
	DeltaCodeAddress	addr;			// Target code address.
	DeltaCodeAddress	ownerAddr;		// Target code address.
	util_ui16			line;			// Source line.
	util_ui16			scope;			// Scope of function.
	bool				isDone;			// If function basic info set.
	util_ui16			totalFormals;	// Size of formal arguments.
	std::string*		formals;		// Dynamic array.
	DeltaVars			locals;

	//**********************************

	public:
	void				SetTotalFormals (util_ui16 _totalFormals); // COMP
	util_ui16			GetTotalFormals (void) const
							{ return totalFormals; }

	void				AddFormal (const std::string& name, util_ui16 offset); // COMP
	const std::string&	GetFormal (util_ui16 argNo) const;
	bool				GetFormal (const std::string& name, util_ui16* argNo) const;

	//**********************************


	DeltaCodeAddress	GetOwnerAddress (void) const	{ return ownerAddr; }
	DeltaCodeAddress	GetAddress (void) const			{ return addr; }
	util_ui16			GetLine (void) const			{ return line; }
	util_ui16			GetScope (void) const			{ return scope; }
	const char*			GetName (void) const			{ return name.c_str(); }
	const char*			GetFullPath (void) const		{ return fullPath.c_str(); }

	//**********************************

	void				SetInformation ( // COMP
							const std::string&	_name, 
							const std::string&	_fullPath, 
							DeltaCodeAddress	_addr, 
							util_ui16			_line, 
							util_ui16			_scope,
							DeltaCodeAddress	_ownerAddr = DELTA_CANTBE_A_FUNCTION_ADDRESS
						);

	DeltaVars&			Locals (void) 
							{ return locals; }
	const DeltaVars&	Locals (void) const
							{ return locals; }

	bool				IsDone (void) const 
							{ return isDone; }

	void				SetDone (void) 
							{ isDone = true; }

	void				Clear (void);

	void				WriteText (FILE* fp) const; // COMP
	bool				Read (GenericReader& reader); // DBG
	void				Write (GenericWriter& writer) const; // COMP

	///////////////////////////////////////////////

	DeltaDebugFuncInfo (void);
	DeltaDebugFuncInfo (GenericReader& reader) { Read(reader); }
	~DeltaDebugFuncInfo() { Clear(); }
};

#endif	// Do not ad stuff beyond this point.
