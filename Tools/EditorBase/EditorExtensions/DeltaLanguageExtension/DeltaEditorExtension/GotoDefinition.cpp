// GotoDefinition.cpp
// Simple and straight goto-definition support for Delta.
// ScriptFigher Project.
// A. Savidis, March 2011.
//

#include "LanguageModule.h"
#include "utypes.h"
#include "DDebug.h"
#include "AutoCompletion.h"
#include "GotoDefinition.h"
#include "usystem.h"
#include "ustrings.h"
#include "ufunctors.h"
#include <ctype.h>
#include <vector>

using namespace editor;
using namespace delta;

#define	SCOPERES_MIN_SIZE		2	// ::
#define	INVALID_POSSIBLE_POS	-1
#define	DELTA_CONST_KWD			"const"
#define	DELTA_LOCAL_KWD			"local"
#define	DELTA_FUNCTION_KWD		"function"
#define	DELTA_BYTECODE_FILE_EXT	".dbc"

//---------------------------------------------------------------------------//

int										DeltaGotoDefinition::possiblePos		= INVALID_POSSIBLE_POS;
int										DeltaGotoDefinition::triedStartPos		= INVALID_POSSIBLE_POS;
util_ui32								DeltaGotoDefinition::refCounter			= 0;
DeltaGotoDefinition::EditorPosVector*	DeltaGotoDefinition::editorPositions	= (EditorPosVector*) 0;	
util_ui32								DeltaGotoDefinition::currPosition		=  0;
int										DeltaGotoDefinition::targetEditorPos	= 0;
void*									DeltaGotoDefinition::targetLangModule	= (void*) 0;

void DeltaGotoDefinition::Initialise (void) 
	{ if (!refCounter++) unew(editorPositions); }

void DeltaGotoDefinition::CleanUp (void)
	{ DASSERT(refCounter); if (!--refCounter) udelete(editorPositions); }

///////////////////////////////////////////////////////////////////////////////

bool DeltaGotoDefinition::GotoDefinitionPrecond (void* langModule, int x, int y) {

	delta::LanguageModule*	mod			= (LanguageModule*) langModule;
	EditorWindow*			editor		= mod->GetEditor(); 
	ProgramDescription&		progDesc	= mod->GetProgramDesc();

	triedStartPos = editor->PositionFromPoint(wxPoint(x, y));
	if (DeltaAutoCompletion::InsideComments(progDesc, triedStartPos, triedStartPos))
		return false;

	int keyCode = editor->GetCharAt(triedStartPos);
	if (!DeltaAutoCompletion::IsIdentChar(keyCode) && triedStartPos)
		keyCode = editor->GetCharAt(--triedStartPos);

	if (!DeltaAutoCompletion::IsIdentChar(keyCode)) {
		triedStartPos = INVALID_POSSIBLE_POS;
		return false;
	}
	else
		return true;
}

///////////////////////////////////////////////////////////////////////////////

int DeltaGotoDefinition::ParseAround_Ident (EditorWindow* editor, int* startPos) {
	
	while (*startPos && DeltaAutoCompletion::IsIdentChar(editor->GetCharAt(*startPos - 1)))
		--*startPos;
	
	int endPos = *startPos;
	while (endPos < editor->GetLength() && DeltaAutoCompletion::IsIdentChar(editor->GetCharAt(endPos + 1)))
		++endPos;

	return endPos + 1;
}

///////////////////////////////////////////////////////////////////////////////
// All find in AST functions should be generic engough so that I can
// reuse them to make a simple quick info implementation.

bool DeltaGotoDefinition::GotoDefinition (void* langModule, int x, int y) {

	DASSERT(GotoDefinitionPrecond(langModule, x, y));

	delta::LanguageModule*	mod			= (LanguageModule*) langModule;
	EditorWindow*			editor		= mod->GetEditor(); 
	ProgramDescription&		progDesc	= mod->GetProgramDesc();

	// Get function name.
	int			startPos = editor->PositionFromPoint(wxPoint(x, y));
	DASSERT(startPos == triedStartPos || startPos - 1 == triedStartPos);
	startPos = triedStartPos;

	int			endPos	 = ParseAround_Ident(editor, &startPos);
	std::string id		 = util::str2std(editor->GetTextRange(startPos, endPos));
	std::string ns;

	// Parse back a namespace prefix as (<id>::)* | ::
	if (startPos >= SCOPERES_MIN_SIZE) {
		int pos = DeltaAutoCompletion::ParseBack_NamespaceAccess(editor, startPos - 1);
		if (!DeltaAutoCompletion::ParseBack_Failed(pos, startPos))
			ns = util::str2std(editor->GetTextRange(pos, startPos));
	}

	if (ns.empty()) {
		if (GotoClosestDefinition(id, editor, progDesc.GetNode(startPos), startPos)) {
			// Check if we really navigated to an earlier definition.
			if (editor->LineFromPosition(editor->GetCurrentPos()) != editor->LineFromPosition(startPos))
				HandleGotoDefinition(langModule, startPos, langModule, editor->GetCurrentPos());
			return true;
		}
		else
			return false;
	}
	else
	if (ns == DELTA_LIBRARYNAMESPACE_SEPARATOR) {
		if (!GotoGlobalDefinition(id, editor, progDesc.GetAST())) {
			umodalmessage(
				"Goto Definition",
				uconstructstr("No global item '%s' was found!", id.c_str())
			);
			return false;
		}
		else {
			if (editor->GetCurrentPos() != startPos)
				HandleGotoDefinition(langModule, startPos, langModule, editor->GetCurrentPos());
			return true;
		}
	}
	else {
		ustrlist path = utokenizestr(ns, ":");
		if (path.size() == 1 && DeltaAutoCompletion::HasByteCodeLibrary(langModule, path.front())) {

			LanguageModuleIface::GotoDefinitionResult result;
			result = mod->GotoGlobalDefinitionOfFile(path.front() + DELTA_BYTECODE_FILE_EXT, id);

			switch (result) {

				case LanguageModuleIface::GotoDefinitionSucceeded			: {
					HandleGotoDefinition(langModule, startPos, targetLangModule, targetEditorPos);
					return true;
				}

				case LanguageModuleIface::GotoDefinitionFailedFileNotFound	: {
					umodalmessage(
						"Goto Definition",
						uconstructstr(
							"Failed to locate project entry producing byte code file '%s.dbc'\n"
							"to lookup for function '%s' (likely a library byte code file created\n"
							"outside this workspace).",
							path.front().c_str(),
							id.c_str()
						)
					);
					return false;
				}

				case LanguageModuleIface::GotoDefinitionFailedMultipleFilesFound	: {
					umodalmessage(
						"Goto Definition",
						uconstructstr(
							"Found multiple project entries producing used byte code file '%s.dbc'\n"
							"while trying to lookup for function '%s' (this will likely cause\n"
							"ambiguities while trying to build current source).",
							path.front().c_str(),
							id.c_str()
						)
					);
					return false;
				}

				case LanguageModuleIface::GotoDefinitionFailedCurrentFileOutsideWorkspace	: {
					umodalmessage(
						"Goto Definition",
						uconstructstr(
							"Cannot search for project entry producing used byte code \n"
							"file '%s.dbc' and lookup for function '%s' since the current\n"
							"source is not part of this workspace.",
							path.front().c_str(),
							id.c_str()
						)
					);
					return false;
				}

				case LanguageModuleIface::GotoDefinitionFailedDefinitionNotFound	: {
					umodalmessage(
						"Goto Definition",
						uconstructstr(
							"No function '%s' was found in the source file of the\n"
							"project entry producing byte code file '%s.dbc'!",
							id.c_str(),
							path.front().c_str()
						)
					);
					return false;
				}

				default: DASSERT(false); return false;
			}
		}
		else {
			return false;
			// TODO: could we open and lead to the position in the lib funcs file (first appearence)?
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

bool DeltaGotoDefinition::HasFoundAndPositionedAtDefinition (
		const std::string&	id, 
		EditorWindow*		editor,
		DeltaASTNode*		node,
		bool				doNotPositionAtVars
	) {

	switch ((node)->GetType()) {

		case UnaryKwdASTNodeType : {
			UnaryKwdASTNode* p = (UnaryKwdASTNode*) node;
			if ((p->GetValueStr() == DELTA_CONST_KWD || p->GetValueStr() == DELTA_LOCAL_KWD) && 
				id == DeltaAutoCompletion::GetText(editor, p->GetChild())) {
				editor->GotoPos(p->GetRange().left);
				return true;
			}
			return false;
		}

		case FunctionASTNodeType : {
			FunctionASTNode* p = (FunctionASTNode*) node;

			// Try with the function name.
			if (p->GetValue() == DELTA_FUNCTION_KWD						&& 
				p->GetChild<0>()										&& 
				p->GetChild<0>()->GetType() == FunctionNameASTNodeType	&&
				id == DeltaAutoCompletion::GetText(editor, p->GetChild<0>())) {
					editor->GotoPos(p->GetRange().left);
					return true;
				}

			// Try with the formal args.
			if (ArgListASTNode* args = (ArgListASTNode*) p->GetChild<1>()) {
				DeltaASTNodeList l;
				args->GetChildren(l);
				for (DeltaASTNodeList::reverse_iterator i = l.rbegin(); i != l.rend(); ++i)
					if (id == DeltaAutoCompletion::GetText(editor, *i)) {
						editor->GotoPos((*i)->GetRange().left);
						return true;
					}
			}

			return false;
		}

		case VariableASTNodeType: {
			VariableASTNode* p = (VariableASTNode*) node;
			if (id == DeltaAutoCompletion::GetText(editor, p))
				if (doNotPositionAtVars)
					possiblePos = p->GetRange().left;
				else {
					editor->GotoPos(p->GetRange().left);
					return true;
				}
			return false;
		}

		default: return false;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Concerns only lookups in used byte code files.

bool DeltaGotoDefinition::HasFoundAndPositionedAtFunction (
		const std::string&	func, 
		EditorWindow*		editor,
		DeltaASTNode*		node
	) {
	if (node->GetType() == FunctionASTNodeType) {
		FunctionASTNode* p = (FunctionASTNode*) node;
		if (p->GetValue() == DELTA_FUNCTION_KWD						&& 
			p->GetChild<0>()										&& 
			p->GetChild<0>()->GetType() == FunctionNameASTNodeType	&&
			func == DeltaAutoCompletion::GetText(editor, p->GetChild<0>())) {
				editor->GotoPos(targetEditorPos = p->GetRange().left);
				targetLangModule = editor->GetLangIface();
				return true;
			}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////

bool DeltaGotoDefinition::IsClosedScope (DeltaASTNode* node) {
	return	node->GetType() == FunctionASTNodeType ||
			node->GetType() == CompoundASTNodeType;  
}

bool DeltaGotoDefinition::IsClosedScopeAbovePosition (
		DeltaASTNode*	node,
		int				startPos
	) { return	IsClosedScope(node) && node->GetRange().IsLeftOf(startPos); }

///////////////////////////////////////////////////////////////////////////////

bool DeltaGotoDefinition::SearchUpDefinition (
		const std::string&	id, 
		EditorWindow*		editor,
		DeltaASTNode*		parent,
		DeltaASTNode*		node,
		int					startPos
	) {
	if (!parent)
		return false;

	if (HasFoundAndPositionedAtDefinition(id, editor, parent))
		return true;

	if (IsClosedScopeAbovePosition(parent, startPos))
		return false;

	DeltaASTNodeList l;
	parent->GetChildren(l);

	bool before = false;
	for (DeltaASTNodeList::reverse_iterator i = l.rbegin(); i != l.rend(); ++i)
		if (*i == node)
			before = true;
		else
		if (before && SearchDownDefinition(id, editor, *i, startPos))
			return true;

	return SearchUpDefinition(id, editor, parent->GetParent(), parent, startPos);
}

///////////////////////////////////////////////////////////////////////////////

bool DeltaGotoDefinition::SearchDownDefinition (
		const std::string&	id, 
		EditorWindow*		editor,
		DeltaASTNode*		node,
		int					startPos
	) {

	if (HasFoundAndPositionedAtDefinition(id, editor, node))
		return true;
	else
	if (IsClosedScopeAbovePosition(node, startPos))
		return false;
	else {
		DeltaASTNodeList l;
		node->GetChildren(l);

		for (DeltaASTNodeList::reverse_iterator i = l.rbegin(); i != l.rend(); ++i)
			if (SearchDownDefinition(id, editor, *i, startPos))
				return true;

		return false;
	}
}

///////////////////////////////////////////////////////////////////////////////

bool DeltaGotoDefinition::GotoGlobalDefinition (
		const std::string&	id, 
		EditorWindow*		editor,
		DeltaASTNode*		node
	) {
	
	if (HasFoundAndPositionedAtDefinition(id, editor, node, false))
		return true;
	else
	if (IsClosedScope(node))
		return false;
	else {
		DeltaASTNodeList l;
		node->GetChildren(l);

		for (DeltaASTNodeList::iterator i = l.begin(); i != l.end(); ++i)
			if (GotoGlobalDefinition(id, editor, *i))	// DFS
				return true;

		return false;		
	}
}

///////////////////////////////////////////////////////////////////////////////

bool DeltaGotoDefinition::GotoGlobalFunction (
		const std::string&	func, 
		EditorWindow*		editor,
		DeltaASTNode*		node
	) {
	
	if (HasFoundAndPositionedAtFunction(func, editor, node))
		return true;
	else
	if (IsClosedScope(node))
		return false;
	else {
		DeltaASTNodeList l;
		node->GetChildren(l);

		for (DeltaASTNodeList::iterator i = l.begin(); i != l.end(); ++i)
			if (GotoGlobalFunction(func, editor, *i))	// DFS
				return true;

		return false;		
	}
}

///////////////////////////////////////////////////////////////////////////////

bool DeltaGotoDefinition::GotoGlobalFunction (void* langModule, const std::string& func) {
	delta::LanguageModule*	mod			= (LanguageModule*) langModule;
	EditorWindow*			editor		= mod->GetEditor(); 
	ProgramDescription&		progDesc	= mod->GetProgramDesc();
	return GotoGlobalFunction(func, editor, progDesc.GetAST());
}

///////////////////////////////////////////////////////////////////////////////

bool DeltaGotoDefinition::GotoClosestDefinition (
		const std::string&	id, 
		EditorWindow*		editor,
		DeltaASTNode*		node,
		int					startPos
	) {
	DASSERT(node);
	possiblePos = INVALID_POSSIBLE_POS;
	if (SearchUpDefinition(id, editor, node->GetParent(), node, startPos))
		return true;
	else
	if (possiblePos != INVALID_POSSIBLE_POS)
		{ editor->GotoPos(possiblePos); return true; }
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////


void DeltaGotoDefinition::ClearEditorPositions (void) {
	DPTR(editorPositions)->clear();
	currPosition = 0;
}

//*****************************

void DeltaGotoDefinition::MakeInitialEditorPositions (void* fromMod, int fromPos, void* toMod, int toPos) {

	DASSERT(!currPosition && !DPTR(editorPositions)->size());

	DPTR(editorPositions)->push_back(EditorPos(fromMod, fromPos));
	DPTR(editorPositions)->push_back(EditorPos(toMod, toPos));
	currPosition = DPTR(editorPositions)->size() - 1;
}

//*****************************

void DeltaGotoDefinition::AppendEditorPosition (void* langMod, int pos) {

	DASSERT(currPosition + 1 == DPTR(editorPositions)->size());

	DPTR(editorPositions)->push_back(EditorPos(langMod, pos));
	currPosition = DPTR(editorPositions)->size() - 1;
}

//*****************************

void DeltaGotoDefinition::FocusAtCurrentEditorPosition (void){
	DASSERT(currPosition < DPTR(editorPositions)->size());
	EditorPos& pos = (*DPTR(editorPositions))[currPosition];
	((LanguageModule*) pos.first)->GotoPosition(pos.second);
}

//*****************************

void DeltaGotoDefinition::DropEditorPositionsAfterCurrent (void) {

	DASSERT(currPosition <= DPTR(editorPositions)->size());

	while (currPosition + 1 < DPTR(editorPositions)->size())
		DPTR(editorPositions)->pop_back();
}

//*****************************

void DeltaGotoDefinition::HandleGotoDefinition (void* fromMod, int fromPos, void* toMod, int toPos) {
	if (!DPTR(editorPositions)->size())
		MakeInitialEditorPositions(fromMod, fromPos, toMod, toPos);
	else {
		DropEditorPositionsAfterCurrent();							// If was navigating, forward items are dropped.
		(*DPTR(editorPositions))[currPosition].second = fromPos;	// Updare position as if we started from here.
		AppendEditorPosition(toMod, toPos);
	}
}

///////////////////////////////////////////////////////////////////////////////

void DeltaGotoDefinition::HandleContentEdited (void* langMod) {
	DASSERT(!DPTR(editorPositions)->size() || (*DPTR(editorPositions))[currPosition].first == langMod);
	if (DPTR(editorPositions)->size())
		ClearEditorPositions();
}

//*****************************

void DeltaGotoDefinition::HandleLanguageModuleDestruction (void* langMod) {
	if (std::find_if(
			DPTR(editorPositions)->begin(), 
			DPTR(editorPositions)->end(),
			std::bind2nd(uequal_first<EditorPos>(), langMod)
		) != DPTR(editorPositions)->end())
		ClearEditorPositions();
}

///////////////////////////////////////////////////////////////////////////////

bool DeltaGotoDefinition::GoBackwardPrecond (void* langModule, int, int) {
	return	currPosition != 0 &&
			langModule == (*DPTR(editorPositions))[currPosition].first;
}

void DeltaGotoDefinition::GoBackward (void* langModule, int, int) { 
	DASSERT(GoBackwardPrecond(langModule, 0, 0));
	--currPosition;
	FocusAtCurrentEditorPosition();
}

//*****************************

bool DeltaGotoDefinition::GoForwardPrecond (void* langModule, int, int) {
	return	currPosition + 1 < DPTR(editorPositions)->size() && 
			langModule == (*DPTR(editorPositions))[currPosition].first;
}

void DeltaGotoDefinition::GoForward (void* langModule, int, int) {
	DASSERT(GoForwardPrecond(langModule, 0, 0));
	++currPosition;
	FocusAtCurrentEditorPosition();
}

//---------------------------------------------------------------------------//

