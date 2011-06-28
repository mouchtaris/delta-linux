/**
 *	ComponentInitializer.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTINITIALIZER_H
#define IDE_COMPONENTINITIALIZER_H

#include "Common.h"
#include "IDECommon.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentInitializer {
public:
	static void InitializePendingComponent (const std::string& compId);
	static void InitializePendingComponents (void);

	static void ClearPendingComponents (void);

	static bool HasPendingComponents (void);
	static void GetPendingComponents (StdStringList& result);

private:
	DISABLE_CONSTRUCTION(ComponentInitializer);
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTINITIALIZER_H
