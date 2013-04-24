/**
 *	DeltaScintillaStyler.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef DELTA_DELTASCINTILLASTYLER_H
#define DELTA_DELTASCINTILLASTYLER_H

#include "Common.h"

namespace editor {
	class EditorWindow;
}

namespace delta {

////////////////////////////////////////////////////////////////////////

class ProgramDescription;

////////////////////////////////////////////////////////////////////////

class ScintillaStyler {
public:
	static void LexStyle (
			editor::EditorWindow*	editor,
			uint					start,
			uint					end
		);

	static void StyleProgram (
			editor::EditorWindow*	editor,
			ProgramDescription&		progDesc,
			uint					start,
			uint					end
		);

	static void SetIndicators (
			editor::EditorWindow*	editor,
			ProgramDescription&		progDesc,
			uint					start,
			uint					end
		);

	static void ClearIndicators (
			editor::EditorWindow*	editor,
			ProgramDescription&		progDesc,
			uint					start,
			uint					end,
			int						updatePos = -1,
			int						addedLines = 0
		);

	static void ClearSemanticErrorIndicators (
			editor::EditorWindow*	editor,
			ProgramDescription&		progDesc,
			uint					start,
			uint					end,
			int						updatePos = -1,
			int						addedLines = 0
		);

private:
	DISABLE_CONSTRUCTION(ScintillaStyler);
};

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_DELTASCINTILLASTYLER_H
