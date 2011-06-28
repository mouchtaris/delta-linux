// DeltaDebugLinesWithCodeHolder.cpp
// Holds information about lines with code that are legal for bpts.
// Debug support extensions.
// Script Fighter Project.
// A. Savidis, June 2006 (original).
// RF to separate class (July 2008).
//

#include "DeltaDebugLinesWithCodeHolder.h"

///////////////////////////////////////////////////////////////////

void DeltaDebugLinesWithCodeHolder::AddLineWithCode (util_ui16 line, DeltaCodeAddress pc) {

	DASSERT(instructionLeadingLines && pc < codeSize);

	for (	std::list<Line>::iterator i = linesWithCode.begin();
			i != linesWithCode.end();
			++i	) 
			if (line == i->GetLine()) { // Lines match.
				if (i->IsNextInstruction(pc)) { // One more instruction of a block at the same line.
					DASSERT(!i->FallsInInstructions(pc));
					i->AssumeOneMoreInstruction(); 
					DASSERT(i->FallsInInstructions(pc)); 
					return;
				}
				else
					DASSERT(!i->FallsInInstructions(pc)); // Must be independent of the line's block.
			}

	linesWithCode.push_back(			// A new block of instructions for this line.
		Line(line, pc)
	);
	instructionLeadingLines[pc] = line;	// This instruction leads the block starting from this line.
}

///////////////////////////////////////////////////////////////////

DeltaDebugLinesWithCodeHolder::LineList::const_iterator DeltaDebugLinesWithCodeHolder::GetLineWithCode (util_ui16 line) const {

	return	std::find_if(
				linesWithCode.begin(),
				linesWithCode.end(),
				std::bind2nd(Line::MatchPred(), line)
			);
}

///////////////////////////////////////////////////////////////////
// Returns if at 'line' we have a block of instructions lead by 'addr.
//
bool DeltaDebugLinesWithCodeHolder::IsLeadingInstructionOfLine (DeltaCodeAddress addr, util_ui16 line) const {
	DASSERT(instructionLeadingLines && addr < codeSize);
	return instructionLeadingLines[addr] == line;
}

///////////////////////////////////////////////////////////////////
// At least one instruction block starts with the line.
//
bool DeltaDebugLinesWithCodeHolder::IsLineWithCode (util_ui16 line) const
	{ return GetLineWithCode(line) != linesWithCode.end(); }

///////////////////////////////////////////////////////////////////

void DeltaDebugLinesWithCodeHolder::Clear (void) {
	linesWithCode.clear();
	if (instructionLeadingLines)
		{ DDELARR(instructionLeadingLines); unullify(instructionLeadingLines); }
}

///////////////////////////////////////////////////////////////////

void DeltaDebugLinesWithCodeHolder::Initialize (util_ui16 _codeSize) {
	
	Clear();
	
	codeSize = _codeSize;
	instructionLeadingLines = DNEWARR(util_ui32, codeSize);

	for (util_ui32* p = instructionLeadingLines; _codeSize; *p++ = 0, --_codeSize)
		;	// Intentionally empty stmt
}

///////////////////////////////////////////////////////////////////
