// GotoDefinition.h
// Simple and straight goto-definition support for Delta.
// ScriptFigher Project.
// A. Savidis, March 2011.
//

#ifndef	DELTA_GOTODEFINITION_H
#define	DELTA_GOTODEFINITION_H

#include "StringDefs.h"
#include "Common.h"
#include "EditorMain/EditorWindow.h"
#include "ProgramDescription.h"
#include "DeltaStdDefs.h"
#include "utypes.h"
#include "ustrings.h"
#include "ufunctors.h"
#include "uvector.h"
#include <vector>
#include <functional>

///////////////////////////////////////////////////////////////////////////////

class DeltaGotoDefinition {

	private:

	typedef std::pair<void*, int>	EditorPos;
	typedef uvector<EditorPos>		EditorPosVector;

	static EditorPosVector*		editorPositions;	
	static util_ui32			refCounter;	
	static util_ui32			currPosition;
	static DeltaASTNode::Range	possibleRange;
	static int					triedStartPos;
	static int					targetEditorPos;
	static void*				targetLangModule;
	
	//**************************

	static void					ClearEditorPositions (void);
	static void					MakeInitialEditorPositions (void* fromMod, int fromPos, void* toMod, int toPos);
	static void					AppendEditorPosition (void* langMod, int pos);
	static void					FocusAtCurrentEditorPosition (void);
	static void					DropEditorPositionsAfterCurrent (void);
	static void					HandleGotoDefinition (void* fromMod, int fromPos, void* toMod, int toPos);

	//**************************

	static bool					GotoClosestDefinition (
									const std::string&	id, 
									EditorWindow*		editor,
									DeltaASTNode*		node,
									int					startPos
								);

	static bool					GotoGlobalDefinition (
									const std::string&	id, 
									EditorWindow*		editor,
									DeltaASTNode*		node
								);

	static bool					GotoGlobalFunction (
									const std::string&	func, 
									EditorWindow*		editor,
									DeltaASTNode*		node
								);

	static bool					SearchUpDefinition (
									const std::string&	id, 
									EditorWindow*		editor,
									DeltaASTNode*		parent,
									DeltaASTNode*		node,
									int					startPos,
									bool				searchingInParent = false
								);

	static bool					SearchDownDefinition (
									const std::string&	id, 
									EditorWindow*		editor,
									DeltaASTNode*		node,
									int					startPos
								);

	static bool					HasFoundAndPositionedAtDefinition (
									const std::string&	id, 
									EditorWindow*		editor,
									DeltaASTNode*		node,
									bool				includeFormals = true,
									bool				doNotPositionAtVars = true
								);

	static bool					HasFoundAndPositionedAtFunction (
									const std::string&	func, 
									EditorWindow*		editor,
									DeltaASTNode*		node
								);

	static bool					IsClosedScope(DeltaASTNode* node);
	
	static bool					IsClosedScopeAbovePosition (
									DeltaASTNode*		node,
									int					startPos
								);
	static int					ParseAround_Ident (EditorWindow* editor, int* pos);

	static void					GotoEditorPos(EditorWindow* editor, const DeltaASTNode::Range& range);

	///////////////////////////////////////////////////////////////////////////////

	public:
	static void 				Initialise (void);
	static void 				CleanUp (void);

	static bool					GoBackwardPrecond (void* langModule, int x, int y);
	static void					GoBackward (void* langModule, int x, int y);

	static bool					GoForwardPrecond (void* langModule, int x, int y);
	static void					GoForward (void* langModule, int x, int y);

	static bool					GotoDefinitionPrecond (void* langModule, int x, int y);
	static bool					GotoDefinition (void* langModule, int x, int y);
	static bool					GotoGlobalFunction (void* langModule, const std::string& id);

	static void					HandleContentEdited (void* langMod);
	static void					HandleLanguageModuleDestruction (void* langMod);
};

///////////////////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
