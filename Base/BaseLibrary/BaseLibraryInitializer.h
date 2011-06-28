/**
 *	BaseLibraryInitializer.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef MAIN_BASELIBRARYINITIALIZER_H
#define MAIN_BASELIBRARYINITIALIZER_H

#include "Common.h"
#include "LibraryAPIDecls.h"

namespace main {

////////////////////////////////////////////////////////////////////////

class _BASE_API BaseLibraryInitializer {
public:
	static void Initialize (void);
	static void CleanUp (void);

private:
	DISABLE_COPY(BaseLibraryInitializer);
};

////////////////////////////////////////////////////////////////////////

} // namespace main

#endif // MAIN_BASELIBRARYINITIALIZER_H
