/**
 *	EditorBaseLibrary.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef EDITOR_EDITORBASELIBRARY_H
#define EDITOR_EDITORBASELIBRARY_H

#include "EditorUtils/EditorCommon.h"

namespace editor {

////////////////////////////////////////////////////////////////////////

class _EDITOR_API EditorBaseLibrary {
public:
	static void Initialize (void);
	static void CleanUp (void);

private:
	DISABLE_COPY(EditorBaseLibrary);
};

////////////////////////////////////////////////////////////////////////

} // namespace editor

#endif // EDITOR_EDITORBASELIBRARY_H
