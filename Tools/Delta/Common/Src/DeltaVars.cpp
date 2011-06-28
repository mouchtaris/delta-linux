// DeltaVars.cpp
// ScriptFighter Project.
// A. Savidis, August 2005.
// Refactored July 2007, adding error propagation.
//

#include "DeltaVars.h"
#include "uerrorclass.h"
#include "ufiles.h"

///////////////////////////////////////////////////////////////////

DeltaVars::DeltaVars (void) {
	totalBlocks = 0;
	unullify(blocks);
}

void DeltaVars::Clear (void) {

	if (blocks) {
		DDELARR(blocks);
		unullify(blocks);
		totalBlocks = 0;
	}
}

DeltaVars::~DeltaVars() { Clear(); }

///////////////////////////////////////////////////////////////////

void DeltaVars::SetTotalBlocks (util_ui16 _totalBlocks) {

	DASSERT(!blocks && !totalBlocks);
	totalBlocks = _totalBlocks;
	blocks = DNEWARR(DeltaDebugBlock, totalBlocks);
	for (util_ui16 i = 0; i < totalBlocks; ++i)
		DPTR(blocks)[i].SetBlockId(i);
}

///////////////////////////////////////////////////////////////////

const DeltaDebugBlock& DeltaVars::GetBlock (util_ui16 blockId) const {
	DASSERT(blockId < totalBlocks);
	return DPTR(blocks)[blockId];
}

///////////////////////////////////////////////////////////////////
// Compile time only.
//
void DeltaVars::AddVar (const char* name, util_ui16 blockId, util_ui16 offset, util_ui16 line) {
	DASSERT(blockId < totalBlocks);
	DPTR(blocks)[blockId].Add(
		DeltaDebugLocal(name, offset, line)
	);
}

///////////////////////////////////////////////////////////////////

bool DeltaVars::GetVar (const std::string& name, util_ui16 blockId, util_ui16* offset) const {
	DASSERT(blockId < totalBlocks);
	return DPTR(blocks)[blockId].Get(name, offset);
}

bool DeltaVars::GetVar (std::string* name, util_ui16 blockId, util_ui16 offset) const {
	DASSERT(blockId < totalBlocks);
	return DPTR(blocks)[blockId].Get(name, offset);
}

///////////////////////////////////////////////////////////////////

void DeltaVars::Write (GenericWriter& writer) const {
	writer.write(totalBlocks);
	for (util_ui16 i = 0; i < totalBlocks; ++i)
		DPTR(blocks)[i].Write(writer);
}

///////////////////////////////////////////////////////////////////

void DeltaVars::WriteText (FILE* fp) const {
	for (util_ui16 i = 0; i < totalBlocks; ++i)
		DPTR(blocks)[i].WriteText(fp);
}

///////////////////////////////////////////////////////////////////

#define	ERROR_HANDLER(what, errclass) \
	uerror::GetSingleton().post##errclass( \
		"Loading (debug info) vars: error in reading '%s'!", what \
	); goto FAIL; \

bool DeltaVars::Read (GenericReader& reader) {

	UCHECK_PRIMARY_ERROR(reader.read(&totalBlocks), "total blocks");

	blocks = DNEWARR(DeltaDebugBlock, totalBlocks);
	for (util_ui16 i = 0; i < totalBlocks; ++i) {
		blocks[i].SetBlockId(i);
		UCHECK_DOMINO_ERROR(blocks[i].Read(reader), uconstructstr("block #%d", i));
	}

	return true;
	FAIL: return false;
}

///////////////////////////////////////////////////////////////////
