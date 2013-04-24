/**
 *	DeltaScintillaStyler.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "DeltaScintillaStyler.h"
#include "DeltaASTStyler.h"

#include "DeltaParser/DeltaAST.h"
#include "DeltaParser/ProgramDescription.h"
#include "DeltaParser/DeltaLexer.h"
#include "EditorUtils/EditorStreams.h"

#include "EditorMain/EditorWindow.h"
using namespace editor;

#include "Streams.h"

////////////////////////////////////////////////////////////////////////

namespace delta {

////////////////////////////////////////////////////////////////////////

template<class TRangeList>
static void _Style (
		EditorWindow*		editor,
		const TRangeList&	list,
		int					mask,
		int					style,
		uint				start,
		uint				end
	)
{
	TRangeList::const_iterator iter = list.begin();
	for (; iter != list.end(); ++iter) {
		if (iter->left >= end)
			break;
		else if (iter->left >= start) {
			editor->StartStyling(iter->left, mask);
			editor->SetStyling(iter->Length(), style);
		}
	}
}

//**********************************************************************

static int _FindLine(EditorWindow* editor, uint left, int updatePos, int linesAdded) {
	int line = editor->LineFromPosition(left);
	if (updatePos != -1 && editor->LineFromPosition(left) >= editor->LineFromPosition(updatePos))
		line -= linesAdded;
	return line;
}

//**********************************************************************

template<class TRangeList>
static void _ActionAtLine (
		EditorWindow*		editor,
		const TRangeList&	list,
		void (EditorWindow::*func) (int),
		uint				start,
		uint				end,
		int					updatePos = -1,
		int					linesAdded = 0
	)
{
	TRangeList::const_iterator iter = list.begin();
	for (; iter != list.end(); ++iter) {
		if (iter->left >= end)
			break;
		else if (iter->left >= start) {
			int line = _FindLine(editor, iter->left, updatePos, linesAdded);
			(editor->*func)(line);
		}
	}
}

////////////////////////////////////////////////////////////////////////
// ScintillaStyler
//

static void _LexStyler (const Range& range, int token, int type, void* closure)
{
	EditorWindow* editor = static_cast<EditorWindow*>(closure);
	editor->StartStyling(range.left, DELTA_STYLING_MASK);
	editor->SetStyling(range.Length(), type);
}

//**********************************************************************

void ScintillaStyler::LexStyle (EditorWindow* editor, uint start, uint end)
{
	EditorInputStream input(editor);
	DeltaLexer lexer(&_LexStyler, editor);
	input->SetRange(start, end);
	lexer.SetOffset(start);
	lexer.LexStream(input);
}

//**********************************************************************

void ScintillaStyler::StyleProgram (
		EditorWindow* editor, ProgramDescription& progDesc, uint start, uint end
	)
{
	LexStyle(editor, start, end);
	if (progDesc.GetAST()) {
		DeltaASTStyler deltaStyler(editor, start, end);
		progDesc.GetAST()->Accept(&deltaStyler);
	}
	_Style(editor, progDesc.GetCpp(), DELTA_STYLING_MASK, CppASTNodeType, start, end);
	_Style(editor, progDesc.GetComments(), DELTA_STYLING_MASK, CommentsASTNodeType, start, end);
	//Not parsed portions are already styled by initial lex styling.
}

//**********************************************************************

void ScintillaStyler::SetIndicators (
		editor::EditorWindow* editor, ProgramDescription& progDesc, uint start, uint end
	)
{
	_Style(editor, progDesc.GetParseErrors(), wxSCI_INDICS_MASK, wxSCI_INDIC0_MASK, start, end);
	_ActionAtLine(editor, progDesc.GetParseErrors(), &EditorWindow::InsertErrorToLine, start, end);

	_Style(editor, progDesc.GetSemanticErrors(), wxSCI_INDICS_MASK, wxSCI_INDIC0_MASK, start, editor->GetLength());
	_ActionAtLine(editor, progDesc.GetSemanticErrors(), &EditorWindow::InsertErrorToLine, start, editor->GetLength());
}

//**********************************************************************

void ScintillaStyler::ClearIndicators (
		editor::EditorWindow* editor, ProgramDescription& progDesc, uint start, uint end, int updatePos, int linesAdded
	)
{
	// TODO: Temporary ... refactor
	//
	ProgramDescription::ErrorRangeList& parseErrors = progDesc.GetParseErrors();
	ProgramDescription::ErrorRangeList::iterator iter = parseErrors.begin();
	while (iter != parseErrors.end()) {
		if (iter->Empty()) {
			int line = _FindLine(editor, iter->left, updatePos, linesAdded);
			editor->RemoveErrorFromLine(line);
			iter = parseErrors.erase(iter);
		}
		else
			++iter;
	}
	//

	_Style(editor, progDesc.GetParseErrors(), wxSCI_INDICS_MASK, 0, start, end);
	_ActionAtLine(editor, progDesc.GetParseErrors(), &EditorWindow::RemoveErrorFromLine, start, end, updatePos, linesAdded);

	ClearSemanticErrorIndicators(editor, progDesc, start, end);
}

void ScintillaStyler::ClearSemanticErrorIndicators (
		editor::EditorWindow* editor, ProgramDescription& progDesc, uint start, uint end, int updatePos, int linesAdded
	)
{
	ProgramDescription::ErrorRangeList& semanticErrors = progDesc.GetSemanticErrors();
	ProgramDescription::ErrorRangeList::iterator iter = semanticErrors.begin();
	while (iter != semanticErrors.end()) {
		if (iter->Empty()) {
			int line = _FindLine(editor, iter->left, updatePos, linesAdded);
			editor->RemoveErrorFromLine(line);
			iter = semanticErrors.erase(iter);
		}
		else
			++iter;
	}

	_Style(editor, progDesc.GetSemanticErrors(), wxSCI_INDICS_MASK, 0, start, editor->GetLength());
	_ActionAtLine(editor, progDesc.GetSemanticErrors(), &EditorWindow::RemoveErrorFromLine, start, editor->GetLength(), updatePos, linesAdded);

	iter = semanticErrors.begin();
	while (iter != semanticErrors.end()) {
		if (iter->left >= start) {
			int line = _FindLine(editor, iter->left, updatePos, linesAdded);
			editor->RemoveErrorFromLine(line);
			iter = semanticErrors.erase(iter);
		}
		else
			++iter;
	}
}
////////////////////////////////////////////////////////////////////////

} // namespace delta
