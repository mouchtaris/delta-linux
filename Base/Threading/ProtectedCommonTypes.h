/**
 *	ProtectedCommonTypes.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef UTIL_PROTECTEDCOMMONTYPES_H
#define UTIL_PROTECTEDCOMMONTYPES_H

#include "Common.h"
#include "Protect.h"

////////////////////////////////////////////////////////////////////////

typedef thread::ProtectWithMutex<String>		StringProt;
typedef thread::ProtectWithMutex<StringList>	StringListProt;
typedef thread::ProtectWithMutex<IntList>		IntListProt;
typedef thread::ProtectWithMutex<UIntList>		UIntListProt;

////////////////////////////////////////////////////////////////////////

namespace thread {

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

} // namespace thread

#endif // UTIL_PROTECTEDCOMMONTYPES_H
