// DebugBreakPointHolder.h
// Singleton class for holding  break points for virtual machines.
// Those are organised as distinct lists per VM id.
// ScriptFighter Project.
// A. Savidis, February 2002, addition for new debugger.
// Refactored to be a separate project, September 2005.
// Refactored to support conditions for breakpoints, November 2006.
// Changed July 2007, the name 'vmId' to 'source' as the breakpoint holder now
// organizes breakpoints by source file names.
// Changed Decemeber 2008 froms ingleton to normal class.
//

#ifndef	DEBUGBREAKPOINTHOLDER_H
#define	DEBUGBREAKPOINTHOLDER_H

#include "DeltaBreakPoint.h"
#include "DeltaDebuggerCommonDefs.h"
#include "uthreadlib.h"
#include "ufiles.h"

#include <algorithm>
#include <functional>
#include <list>
#include <map>

class DVMDEBUGCOMMON_CLASS DebugBreakPointHolder {
	
	private:
	typedef std::list<DeltaBreakPoint>					BreakPointsList;
	typedef std::map<std::string, BreakPointsList*>		BreakPointsMap;
	BreakPointsMap										breakPoints;
	mutable umutex										breakPointMutex;

	void		AddPriv (const std::string& source, const DeltaBreakPoint& pt);									
	bool		SetBreakPointStatePriv (const std::string& source, util_ui16 line, bool state);
	bool		SetBreakPointStatePriv (const std::string& source, bool state);
	bool		HasBreakPointsWithStatePriv (bool isEnabled) const;
	void		CleanUpPriv (void);

	BreakPointsList::iterator 
				Find (BreakPointsList&	bpts, util_ui16 line);
	BreakPointsList::const_iterator 
				Find (BreakPointsList&	bpts, util_ui16 line) const;
	BreakPointsMap::iterator
				Find (const std::string& src);
	BreakPointsMap::const_iterator
				Find (const std::string& src) const;

	mutable bool				isDirty;
	mutable std::string			sourceDirty;
	mutable BreakPointsList*	breakPointsDirty;

	bool						IsSourceDirty (const std::string& source) const
									{ return isDirty && sourceDirty == source; }
	void						SetSourceDirty (const std::string& source, BreakPointsList* breakPoints) const
									{ isDirty = true; sourceDirty = source; breakPointsDirty = DNULLCHECK(breakPoints); }
	void						ResetSourceDirty (void) const
									{ isDirty = false; sourceDirty.clear(); unullify(breakPointsDirty); }

	struct MatchSource : public std::binary_function<std::pair<std::string, BreakPointsList*>, std::string, bool> { 
		bool operator()(const std::pair<std::string, BreakPointsList*>& e, const std::string& s) const
			{ return ufilepathsequal(e.first, s); }
	};

	struct WriteTextFunctor : 
	public	std::binary_function<std::pair<std::string, BreakPointsList*>, FILE*, void> {
		void operator()(const std::pair<std::string, BreakPointsList*>& p, FILE* fp) const {
			fprintf(fp,"\"%s\" %d\n", p.first.c_str(), DPTR(p.second)->size());
			std::for_each(
				DPTR(p.second)->begin(), 
				DPTR(p.second)->end(),
				std::bind2nd(DeltaBreakPoint::WriteTextFunctor(), fp)
			);
		}
	};

	///////////////////////////////////////////////////////

	public:
	void								Add (const std::string& source, util_ui16 line, const std::string& cond = "");
	bool								Remove (const std::string& source, util_ui16 line);									
	bool								RemoveAll (const std::string& source);
							
	void								Change (const std::string& source, util_ui16 line, util_ui16 newLine);
	void								Change (const std::string& source, util_ui16 line, const std::string& newCond = "");

	bool								Enable (const std::string& source, util_ui16 line);									
	bool								EnableAll (const std::string& source);
	bool								HasEnabledBreakPoints (void) const;
							
	bool								Disable (const std::string& source, util_ui16 line);									
	bool								DisableAll (const std::string& source);
	bool								HasDisabledBreakPoints (void) const;

	util_ui16							Total (void) const;
	const std::string&					GetSource (util_ui16 order) const;
	const std::list<DeltaBreakPoint>&	Get (const std::string& source) const;
	const DeltaBreakPoint&				Get (const std::string& source, util_ui16 line) const;
	bool								HasAnyBreakPoints (const std::string& source) const;
	bool								HasBreakPoint (const std::string& source, util_ui16 line) const;

	////////////////////////////////////////////////

	bool								LoadText (const char* path);							
	bool								StoreText (const char* path) const;
	void								CleanUp (void);

	DebugBreakPointHolder (void);
	~DebugBreakPointHolder();
};

#endif	// Do not add stuff beyond this point.