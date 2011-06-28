// DeltaString.hh
// Delta string run-time type.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, October 1999.
// Last revision June 2003, July 2008.
//

#ifndef	DELTASTRING_H
#define	DELTASTRING_H

#include "utypes.h"
#include "ustrings.h"
#include "DDebug.h"
#include "DeltaStdDefs.h"
#include <list>
#include <functional>
#include <string>

class DVM_CLASS DeltaString {	// Trivial wrapper for string as used in Delta VM.

	DFRIENDDESTRUCTOR()

	private:
	std::string str;
	util_ui32	maxLen;

	static std::list<DeltaString*>* recycler;

	void				ActualDelete (void) { DDELETE(this); }
	void				Clear (void) { str.clear(); }

	DeltaString (const std::string& s = ""): str(s), maxLen(0){}
	~DeltaString(){}

	//////////////////////////////////////////////////////////////////

	public:
	struct MaxLengthExceededException {};

	void				operator=(const std::string& s) 
							{ str = s; }
	bool				operator==(const DeltaString& s) const
							{ return str == s.str; }
	bool				operator!=(const DeltaString& s) const
							{ return str != s.str; }

	void				Set (const std::string& s)		
							{ str = s; }
	void				Add (const std::string& s);
	const std::string&	Content (void) const	
							{ return str; }
	DeltaString*		Clone (void) const
							{ return New(str); }
	util_ui32			Length (void) const
							{ return (util_ui32) str.length(); }
	void				Crop (util_ui32 maxLen, const std::string& suffix)
							{ str = ucropstr(str, maxLen, suffix); }
	void				SetMaxLength (util_ui32 n) 
								{ maxLen = n; }
	util_ui32			GetMaxLength (void) const 
							{ return maxLen; }
	static bool			IsIdentifier (const std::string& s);

	//////////////////////////////////////////////////////////////////

	static void			SingletonCreate (void);
	static void			SingletonDestroy (void);

	static DeltaString* New (const std::string& s = "");
	static void			Delete (DeltaString*& s);
};

#endif	// Do not add stuff beyond this point.