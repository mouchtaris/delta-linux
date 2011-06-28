/**
 *	EditorStreams.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef EDITOR_EDITORCOMMON_H
#define EDITOR_EDITORCOMMON_H

#include "Common.h"
#include "BinaryBuffer.h"
#include "LibraryAPIDecls.h"

namespace editor {

////////////////////////////////////////////////////////////////////////
// Editor API convenience defines
//
#ifdef EDITORBASELIBRARY_EXPORTS
#	define _EDITOR_API		UTIL_EXPORT
#	define _EDITOR_C_API	UTIL_C_EXPORT
#else
#	define _EDITOR_API		UTIL_IMPORT
#	define _EDITOR_C_API	UTIL_C_IMPORT
#endif

////////////////////////////////////////////////////////////////////////

} // namespace editor

#endif // EDITOR_EDITORCOMMON_H
