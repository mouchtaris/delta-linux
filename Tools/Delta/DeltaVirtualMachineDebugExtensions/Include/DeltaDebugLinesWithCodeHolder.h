// DeltaDebugLinesWithCodeHolder.h
// Holds information about lines with code that are legal for bpts.
// Debug support extensions.
// Script Fighter Project.
// A. Savidis, June 2006 (original).
// RF to separate class (July 2008).
//

#ifndef	DELTADEBUGLINESWITHCODEHOLDER_H
#define	DELTADEBUGLINESWITHCODEHOLDER_H

#include "utypes.h"
#include "DeltaByteCodeTypes.h"
#include <list>

/////////////////////////////////////////////////////////////////
// In debug mode, to stop to a break point, we check the line number.
// One important remark is that, while multiple instructions may be given
// a certain line, we can only stop at a break point if this is the
// first line of a block of instructions sharing the line.
//
class DeltaDebugLinesWithCodeHolder {
	
	private:

	class Line {
		private:
		util_ui16			line;
		DeltaCodeAddress	pc;		// First instruction on this line.
		util_ui32			n;		// Number of instructions sharing the line.	

		public:
		util_ui16			GetLine (void) const 
								{ return line; }

		DeltaCodeAddress	GetPC (void) const 
								{ return pc; }

		bool				IsNextInstruction (DeltaCodeAddress addr) const
								{ return addr == pc + n; }

		bool				FallsInInstructions (DeltaCodeAddress addr) const
								{ return addr >= pc && addr < pc + n; }

		void				AssumeOneMoreInstruction (void) 
								{ ++n; }

		struct MatchPred : public std::binary_function<Line, util_ui16, bool> {
			bool operator()(const Line& l, util_ui16 line) const
				{ return line == l.line; }
		};

		Line (const Line& item) : 
			line(item.line), pc(item.pc), n(item.n) {}

		Line (util_ui16 _line, DeltaCodeAddress _pc) : 
			line(_line), pc(_pc), n(1) {}

		Line (void) : line(0), pc(0), n(0) {}

		~Line(){}
	};

	util_ui16				codeSize;
	util_ui32*				instructionLeadingLines;

	typedef std::list<Line>	LineList;
	LineList				linesWithCode;
	ubag<util_ui16>			lineNumbers;

	const std::list<DeltaCodeAddress>	
							GetLeadingInstructionAddresses (util_ui16 line) const;
	LineList::const_iterator			
							GetLineWithCode (util_ui16 line) const;

	///////////////////////////////////////////////////////

	public:
	void					Initialize (util_ui16 _codeSize);
	void					Clear (void);

	void					AddLineWithCode (util_ui16 line, DeltaCodeAddress pc);
	bool					IsLeadingInstructionOfLine(DeltaCodeAddress addr, util_ui16 line) const;
	bool					IsLineWithCode (util_ui16 line) const;

	DeltaDebugLinesWithCodeHolder (void) : 
		codeSize(0), 
		instructionLeadingLines((util_ui32*) 0)
		{}
	~DeltaDebugLinesWithCodeHolder(){}
};

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

