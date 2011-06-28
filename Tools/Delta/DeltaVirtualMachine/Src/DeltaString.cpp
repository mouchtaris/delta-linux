// DeltaString.cpp
// Implementation of the DeltaString class.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, October 1999.
// Last change July 2008.
//

#include "DeltaString.h"
#include "ufunctors.h"
#include <ctype.h>
#include <algorithm>

std::list<DeltaString*>* DeltaString::recycler = (std::list<DeltaString*>*) 0;

//////////////////////////////////////////////////////////////////

void DeltaString::SingletonCreate (void) {
	DASSERT(!recycler);
	recycler = DNEW(std::list<DeltaString*>);
}

void DeltaString::SingletonDestroy (void) {
	DASSERT(recycler);
	std::for_each(
		DPTR(recycler)->begin(),
		DPTR(recycler)->end(),
		std::mem_fun(&DeltaString::ActualDelete)
	);
	DPTR(recycler)->clear();
	DDELETE(recycler);
	unullify(recycler);
}

//////////////////////////////////////////////////////////////////

bool DeltaString::IsIdentifier (const std::string& s) {

	if (s.empty())
		return false;

	std::string::const_iterator i = s.begin();

	if (!isalpha(*i))
		return false;

	while (++i != s.end())
		if (!isalnum(*i) && *i != '_')
			return false;
	return true;
}

//////////////////////////////////////////////////////////////////

void DeltaString::Add (const std::string& s) {
	if (maxLen) {
		if (str.length() < maxLen) {					// Not exceeds already.
			if (str.length() + s.length() > maxLen) {	// Ecxeeds after concatenation.
				util_ui32 n = maxLen - str.length();	// That's how much we are allowed to add.
				str.append(s, 0, n);					// Adds range [0,n).
				throw MaxLengthExceededException();
			}
			else
				str.append(s);
		}
		else {
			str.erase(maxLen, str.length() - maxLen);	// Crop remaining.
			throw MaxLengthExceededException();
		}
	}
	else
		str.append(s); 
}	

//////////////////////////////////////////////////////////////////

DeltaString* DeltaString::New (const std::string& s) {

	DASSERT(recycler);

	if (!DPTR(recycler)->empty()) {
		DeltaString* p = *DPTR(recycler)->begin();
		DPTR(recycler)->pop_front();
		DASSERT(DPTR(p)->Content().empty());
		return DPTR(p)->str = s, DPTR(p);
	}
	else
		return DNEWCLASS(DeltaString, (s));
}

void DeltaString::Delete (DeltaString*& s) { // No destructor call included

	DASSERT(recycler);
	DPTR(s)->Clear();
	DPTR(recycler)->push_back(DPTR(s));
	unullify(s);
}

//////////////////////////////////////////////////////////////////