// DeltaFunc.cpp
// ScriptFighter Project.
// A. Savidis, Feburary 2005.
// Refactored August 2005, moving locals in a separate class.
// Refactored July 2007, adding error propagation.
//

#include "DeltaFunc.h"
#include "uerrorclass.h"
#include "ufiles.h"

////////////////////////////////////////////////////////

DeltaDebugFuncInfo::DeltaDebugFuncInfo(void) {

	totalFormals	= 0;
	formals			= (std::string*) 0;
	name			= "";

	line 			= 
	addr 			= 
	scope			= 0;
	isDone			= false;
}

////////////////////////////////////////////////////////

void DeltaDebugFuncInfo::Clear (void) {

	locals.Clear();

	if (formals) {
		DASSERT(totalFormals);
		DDELARR(formals);
		formals = (std::string*) 0;
		totalFormals = 0;
	}

	isDone = false;
}

////////////////////////////////////////////////////////

void DeltaDebugFuncInfo::SetTotalFormals (util_ui16 _totalFormals) {
	DASSERT(!formals && !totalFormals);
	DASSERT(_totalFormals);
	totalFormals = _totalFormals;
	formals = DNEWARR(std::string, _totalFormals);
}

////////////////////////////////////////////////////////

void DeltaDebugFuncInfo::SetInformation (
		const std::string&	_name, 
		const std::string&	_fullPath, 
		DeltaCodeAddress	_addr, 
		util_ui16			_line, 
		util_ui16			_scope,
		DeltaCodeAddress	_ownerAddr
	) {
	name		=	_name;
	fullPath	= _fullPath;
	addr		= _addr;
	line		= _line;
	scope		= _scope;
	ownerAddr	= _ownerAddr;
}

////////////////////////////////////////////////////////
// The offset of a formal argument is always positive.
//
void DeltaDebugFuncInfo::AddFormal (const std::string& name, util_ui16 offset) {

	DASSERT(offset < totalFormals);
	DPTR(formals)[offset] = name;
}

////////////////////////////////////////////////////////

const std::string& DeltaDebugFuncInfo::GetFormal (util_ui16 argNo) const {
	DASSERT(argNo < totalFormals);
	return DPTR(formals)[argNo];
}

bool DeltaDebugFuncInfo::GetFormal (const std::string& name, util_ui16* argNo) const {

	for (util_ui16 i = 0; i < totalFormals; ++i)
		if (DPTR(formals)[i] ==  name) {
			*argNo = i;
			return true;
		}

	return false;
}

////////////////////////////////////////////////////////

void DeltaDebugFuncInfo::Write (GenericWriter& writer) const {
	writer.write(name);
	writer.write(fullPath);
	writer.write(addr);
	writer.write(line);
	writer.write(scope);
	writer.write(ownerAddr);

	writer.write(totalFormals);
	for (util_ui16 i = 0; i < totalFormals; ++i)
		writer.write(DPTR(formals)[i]);
	locals.Write(writer);
}

////////////////////////////////////////////////////////

void DeltaDebugFuncInfo::WriteText (FILE* fp) const {

	fprintf(
		fp,
		"FUNC(%s), FULLPATH(%s), AT %u, SCOPE %u, OWNER(%u).\n", 
		GetName(), 
		GetFullPath(), 
		GetAddress(), 
		GetScope(),
		GetOwnerAddress()
	);

	for (util_ui16 i = 0; i < totalFormals; ++i)
		fprintf(fp,"\tFORMAL[%d]->%s.\n", i, DPTR(formals)[i].c_str());

	locals.WriteText(fp);
}

////////////////////////////////////////////////////////

#define	ERROR_HANDLER(what, errclass) \
	uerror::GetSingleton().post##errclass( \
		"Loading (debug info) function: error in reading '%s'!", what \
	); goto FAIL; \

bool DeltaDebugFuncInfo::Read (GenericReader& reader) {

	Clear();

	UCHECK_PRIMARY_ERROR(reader.read(name, false), "name");
	UCHECK_PRIMARY_ERROR(reader.read(fullPath, false), "full path");
	UCHECK_PRIMARY_ERROR(reader.read(&addr), "addr");
	UCHECK_PRIMARY_ERROR(reader.read(&line), "line");
	UCHECK_PRIMARY_ERROR(reader.read(&scope), "scope");
	UCHECK_PRIMARY_ERROR(reader.read(&ownerAddr), "owner address");
	UCHECK_PRIMARY_ERROR(reader.read(&totalFormals), "total formals");

	if (totalFormals) {
		formals = DNEWARR(std::string, totalFormals);
		for (util_ui16 i = 0; i < totalFormals; ++i){
			UCHECK_DOMINO_ERROR(
				reader.read(DPTR(formals)[i], false), 
				uconstructstr("formal #%d", i)
			); 
		}
	}

	UCHECK_DOMINO_ERROR(locals.Read(reader), "locals");
	return true;

	FAIL: Clear(); return false;
}

////////////////////////////////////////////////////////
