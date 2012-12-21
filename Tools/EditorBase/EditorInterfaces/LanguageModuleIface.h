/**
 *	LanguageModuleIface.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 *	i-sense hooks, Anthony Savidis, August, 2010
 *	Goto Definition additions, A. Savidis, March 2011.
 */

#ifndef EDITOR_LANGUAGEMODULEIFACE_H
#define EDITOR_LANGUAGEMODULEIFACE_H

#include "EditorUtils/EditorCommon.h"

#include <boost/function.hpp>
class wxWindow;
class wxAcceleratorEntry;

namespace editor {

////////////////////////////////////////////////////////////////////////

class EditorWindow;

////////////////////////////////////////////////////////////////////////

class _EDITOR_API LanguageModuleIface {
	
public:
	LanguageModuleIface (void);
	virtual ~LanguageModuleIface();

	EditorWindow*	GetEditor (void) const { return m_editor; }
	virtual void	SetEditor (EditorWindow* editor) { m_editor = editor; }

	//******************************************************************
	// Lang API
	//
	virtual void ContentSetup (void) = 0;
	virtual void ContentCleared (void) = 0;

	virtual void ContentAdded (
			uint		atPos,
			uint		length,
			uint		firstLine,
			uint		lastLine
		) = 0;

	virtual void ContentDeleted (
			uint		atPos,
			uint		length,
			uint		firstLine,
			uint		lastLine
		) = 0;

	virtual void ContentOffsetChanged (
			uint		atPos,
			int			offset
		) = 0;

	// Added for i-sense (AS).
	virtual void CursorRepositioned (uint atPos) = 0;
	virtual void MouseHoverStarted  (uint atPos) = 0;
	virtual void MouseHoverEnded  (void) = 0;
	virtual const std::string GetCursorFocusInformation (void) const = 0;

	virtual void CharacterAdded (int keyCode) = 0;

	virtual bool GetInfoForPosition (
			uint		pos,
			uint*		startPos,
			uint*		endPos,
			int*		type,
			String*		msg
		) = 0;

	virtual void CancelDisplayedInfo (void) = 0;

	virtual void SetExtraLibraryDefinitions (const StringList& keywords) = 0;
	virtual void SetByteCodeLoadingPath (const std::string& path) = 0;

	virtual const IntList GetHotspots (void) const = 0;
	virtual const SerializedEditorAST GetAST (void) const = 0;
	virtual void  SharedWriteAST (void) const = 0;

	//******************************************************************

	enum GotoDefinitionResult { 
			GotoDefinitionSucceeded							= 0,
			GotoDefinitionFailedFileNotFound				= 1,
			GotoDefinitionFailedMultipleFilesFound			= 2,
			GotoDefinitionFailedCurrentFileOutsideWorkspace	= 3,
			GotoDefinitionFailedDefinitionNotFound			= 4
	};

	typedef GotoDefinitionResult	(*GotoGlobalDefinitionFunc)(
										const std::string&	file,
										const std::string&	id,
										uint				stage,
										void*				closure
									);
	typedef void					(*GotoPositionCallback)(
										int					pos,
										void*				closure
									);

	virtual bool    GotoGlobalDefinition (const std::string& id) = 0;
	
	void			SetGotoGlobalDefinitionOfFileCallback (GotoGlobalDefinitionFunc f, void* c) {
						gotoGlobalDefinitionCallback = f; 
						gotoGlobalDefinitionClosure = c; 
					}
	
	GotoDefinitionResult			
					GotoGlobalDefinitionOfFile (const std::string& file, const std::string& id, uint stage) {
						if (gotoGlobalDefinitionCallback) 
							return (*gotoGlobalDefinitionCallback)(file, id, stage, gotoGlobalDefinitionClosure); 
						else
							return GotoDefinitionFailedFileNotFound;
					}

	void			SetGotoPositionCallback (GotoPositionCallback f, void* c) {
						gotoPositionCallback = f; 
						gotoPositionClosure = c; 
					}
	
	void			GotoPosition (int pos) {
						if (gotoPositionCallback) 
							(*gotoPositionCallback)(pos, gotoPositionClosure); 
					}

	//******************************************************************
	// Visualization support
	//
	virtual wxWindow*	VisualizeContent (wxWindow* parent) { return 0; }
	virtual void		DestroyContentVisualization (void) {}

	//******************************************************************
	// Accelerator support
	//
	virtual void GetAccelerators (wxAcceleratorEntry** entry, int* size) { *size = 0; }
	virtual void DispatchAcceleratorCommand (int command) {}

private:
	EditorWindow*			m_editor;
	GotoGlobalDefinitionFunc	gotoGlobalDefinitionCallback; 
	void*						gotoGlobalDefinitionClosure;
	GotoPositionCallback		gotoPositionCallback;
	void*						gotoPositionClosure;
};

////////////////////////////////////////////////////////////////////////

} // namespace editor

#endif // EDITOR_LANGUAGEMODULEIFACE_H
