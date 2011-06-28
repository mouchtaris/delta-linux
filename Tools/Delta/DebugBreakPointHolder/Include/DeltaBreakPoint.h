// DeltaBreakPoint.h
// Basic breakpoint type for Delta VM.
// A. Savidis, August 2005.
// Refactored to support conditions for breakpoints, November 2006.
//

#ifndef	DELTABREAKPOINT_H
#define	DELTABREAKPOINT_H

#include "DeltaDebuggerCommonDefs.h"
#include "utypes.h"
#include "ustlhelpers.h"
#include "ustrings.h"

#include <functional>
#include <list>
#include <string>

///////////////////////////////////////////////////////

class DVMDEBUGCOMMON_CLASS DeltaBreakPoint {

	private:
	util_ui16	line;
	bool		enabled;
	std::string	cond;	// Optional condition to activate bpt.

	//***************************

	public:
	struct  MatchPred : public std::binary_function<DeltaBreakPoint, util_ui16, bool> {
		bool operator()(const DeltaBreakPoint& bt, util_ui16 line) const
			{ return line == bt.GetLine(); }
	};

	struct SetStateFunctor : public std::binary_function<DeltaBreakPoint, bool, void> {
		void operator()(DeltaBreakPoint& bpt, bool state) const
			{ if (state) bpt.Enable(); else bpt.Disable(); }
	};

	struct WriteTextFunctor : public std::binary_function<DeltaBreakPoint, FILE*, void> {
		void operator()(const DeltaBreakPoint& bpt, FILE* fp) const
			{ bpt.WriteText(fp); }
	};

	//***************************

	void		Disable (void) { enabled = false; }
	void		Enable (void) { enabled = true; }
	bool		IsEnabled (void) const 
					{ return enabled; }

	util_ui16	GetLine (void) const { return line; }
	void		SetLine (util_ui16 l) { line = l; }

	void		SetCondition (const std::string& s)
					{ cond = s; }
	bool		HasCondition (void) const 
					{ return cond != ""; }
	const char*	GetCondition (void) const 
					{ return cond.c_str(); }

	//***************************

	void		operator=(const DeltaBreakPoint& bpt) {
					if (this != &bpt)
						new (this) DeltaBreakPoint(bpt);
				}

	bool		operator==(const DeltaBreakPoint& bpt) const 
					{ return line == bpt.line; }

	bool		operator!=(const DeltaBreakPoint& bpt) const 
					{ return line != bpt.line; }

	bool		Load (FILE* fp);

	void		WriteText (FILE* fp) const;

	//***************************

	DeltaBreakPoint (const DeltaBreakPoint& bpt) : 
			line(bpt.line), 
			enabled(bpt.enabled),
			cond(bpt.cond){}

			DeltaBreakPoint (util_ui16 _line, const std::string& _cond = "") : 
			line(_line), 
			enabled(true),
			cond(_cond){}	// Null means no condition.

	DeltaBreakPoint (void) : 
			line(0), 
			enabled(false),
			cond(""){}

	~DeltaBreakPoint() {}
};

///////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

