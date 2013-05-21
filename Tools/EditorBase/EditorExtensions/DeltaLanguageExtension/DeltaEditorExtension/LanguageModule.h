/**
 *	LanguageModule.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 *	i-sense hooks, Anthony Savidis, August, 2010
 *	Goto Definition additions, A. Savidis, March 2011.
 */

#ifndef DELTA_LANGUAGEMODULE_H
#define DELTA_LANGUAGEMODULE_H

#include "EditorInterfaces/LanguageModuleIface.h"
#include "DeltaParser/DeltaAST.h"
#include "DeltaParser/ProgramDescription.h"
#include <wx/timer.h>
#include "ucallbacks.h"

namespace delta {

////////////////////////////////////////////////////////////////////////

class ASTreeCtrl;
class LanguageModule;

////////////////////////////////////////////////////////////////////////

class ModuleTimer : public wxTimer {
public:
	typedef void (LanguageModule::*ModFunc) (void);
	ModuleTimer (void) : m_module(0), m_func(0) {}

	void SetOwner (LanguageModule* module, ModFunc func)
		{ m_module = module; m_func = func; }

protected:
	void Notify (void) { (m_module->*m_func)(); }

private:
	LanguageModule* m_module;
	ModFunc			m_func;

};

////////////////////////////////////////////////////////////////////////

class LanguageModule : public editor::LanguageModuleIface {

	/////////////////////////////////////////////////////////
	// i-sense required API (callbacks for tree syncing and ui events).

	public:
	typedef void (*OnAfterLanguageModuleCreation)(LanguageModule* langMod, void* closure);
	typedef void (*OnBeforeLanguageModuleDestruction)(LanguageModule* langMod, void* closure);

	typedef void (*OnFocusChanged)(LanguageModule* langMod, DeltaASTNode* focus, void* closure);
	typedef void (*OnReplaced)(LanguageModule* langMod, DeltaASTNode* old, DeltaASTNode* replacement, void* closure);
	typedef void (*OnDeleted)(LanguageModule* langMod, DeltaASTNode* deleted, void* closure);
	typedef void (*OnInserted)(LanguageModule* langMod, DeltaASTNode* parent, DeltaASTNode* after, DeltaASTNode* inserted, void* closure);
	typedef void (*OnFileParsed)(LanguageModule* langMod, DeltaASTNode* tree, void* closure);
	typedef void (*OnMouseHoverStarted)(LanguageModule* langMod, DeltaASTNode* under, void* closure);
	typedef void (*OnMouseHoverEnded)(LanguageModule* langMod, void* closure);
	typedef void (*OnCharacterAdded)(LanguageModule* langMod, int c, void* closure);

	//**************************

	protected:
	static ucallbackwithclosure<OnAfterLanguageModuleCreation>		onAfterCreation;
	static ucallbackwithclosure<OnBeforeLanguageModuleDestruction>	onBeforeDestruction;

	ucallbackwithclosure<OnFocusChanged>							onFocusChanged;
	ucallbackwithclosure<OnReplaced>								onReplaced;
	ucallbackwithclosure<OnDeleted>									onDeleted;
	ucallbackwithclosure<OnInserted>								onInserted;
	ucallbackwithclosure<OnFileParsed>								onFileParsed;
	ucallbackwithclosure<OnMouseHoverStarted>						onMouseHoverStarted;
	ucallbackwithclosure<OnMouseHoverEnded>							onMouseHoverEnded;
	ucallbackwithclosure<OnCharacterAdded>							onCharacterAdded;
	bool															isContentSetup;

	bool															IsContentSetup (void) const
																		{ return isContentSetup; }
	void															SetIsContentSetup (void)
																		{ isContentSetup = true; }
	void															ResetIsContentSetup (void)
																		{ isContentSetup = false; }

	//**************************

	public:
	static void 		SetOnAfterLanguageModuleCreation (OnAfterLanguageModuleCreation f, void* c = (void*) 0)
							{ onAfterCreation.set(f, c); }
	static void 		ResetOnAfterLanguageModuleCreation (void)
							{ onAfterCreation.reset(); }
	static void 		SetOnBeforeLanguageModuleDestruction (OnAfterLanguageModuleCreation f, void* c = (void*) 0)
							{ onBeforeDestruction.set(f, c); }
	static void 		ResetOnBeforeLanguageModuleDestruction (void)
							{ onBeforeDestruction.reset(); }

	void				SetOnFocusChanged (OnFocusChanged f, void* c = (void*) 0)
							{ onFocusChanged.set(f,c); }
	void				ResetOnFocusChanged (void)
							{ onFocusChanged.reset(); } 
	void				SetOnReplaced (OnReplaced f, void* c = (void*) 0)
							{ onReplaced.set(f,c); } 
	void				ResetOnReplaced (void)
							{ onReplaced.reset(); } 
	void				SetOnDeleted (OnDeleted f, void* c = (void*) 0)
							{ onDeleted.set(f,c); } 
	void				ResetOnDeleted (void)
							{ onDeleted.reset(); } 
	void				SetOnInserted (OnInserted f, void* c = (void*) 0)
							{ onInserted.set(f,c); } 
	void				ResetOnInserted (void)
							{ onInserted.reset(); } 
	void				SetOnFileParsed (OnFileParsed f, void* c = (void*) 0)
							{ onFileParsed.set(f,c); } 
	void				ResetOnFileParsed (void)
							{ onFileParsed.reset(); } 
	void				SetOnMouseHoverStarted (OnMouseHoverStarted f, void* c = (void*) 0)
							{ onMouseHoverStarted.set(f,c); } 
	void				ResetOnMouseHoverStarted (void)
							{ onMouseHoverStarted.reset(); } 
	void				SetOnMouseHoverEnded (OnMouseHoverEnded f, void* c = (void*) 0)
							{ onMouseHoverEnded.set(f,c); } 
	void				ResetOnMouseHoverEnded (void)
							{ onMouseHoverEnded.reset(); } 
	void				SetOnCharacterAdded (OnCharacterAdded f, void* c = (void*) 0)
							{ onCharacterAdded.set(f,c); } 
	void				ResetOnCharacterAdded (void)
							{ onCharacterAdded.reset(); } 

	//**************************

	protected:
	static void			NotifyAfterCreation (LanguageModule* langMod)
							{ if (onAfterCreation) onAfterCreation(langMod); }
	static void			NotifyBeforeDestruction (LanguageModule* langMod)
							{ if (onBeforeDestruction) onBeforeDestruction(langMod); }

	void				NotifyFocusChanged (DeltaASTNode* focus)
							{ if (onFocusChanged) onFocusChanged(this, focus); }
	void				NotifyReplaced (DeltaASTNode* old, DeltaASTNode* replacement)
							{ if (onReplaced) onReplaced(this, old, replacement); }
	void				NotifyDeleted (DeltaASTNode* deleted)
							{ if (onDeleted) onDeleted(this, deleted); }
	void				NotifyInserted (DeltaASTNode* parent, DeltaASTNode* after, DeltaASTNode* inserted)
							{ if (onInserted) onInserted(this, parent, after, inserted); }
	void				NotifyFileParsed (DeltaASTNode* tree)
							{ if (onFileParsed) onFileParsed(this, tree); }
	void				NotifyMouseHoverStarted (DeltaASTNode* under)
							{ if (onMouseHoverStarted) onMouseHoverStarted(this, under); }
	void				NotifyMouseHoverEnded(void)
							{ if (onMouseHoverEnded) onMouseHoverEnded(this); }
	void				NotifyCharacterAdded (int c)
							{ if (onCharacterAdded ) onCharacterAdded (this, c); }

	static void			OnFocusChangedCallback (DeltaASTNode* focus, void* langMod)
							{ ((LanguageModule*) langMod)->NotifyFocusChanged(focus); }
	static void			OnReplacedCallback (DeltaASTNode* old, DeltaASTNode* replacement, void* langMod)
							{ ((LanguageModule*) langMod)->NotifyReplaced(old, replacement); }
	static void			OnDeletedCallback (DeltaASTNode* deleted, void* langMod)
							{ ((LanguageModule*) langMod)->NotifyDeleted(deleted); }
	static void			OnInsertedCallback (DeltaASTNode* parent, DeltaASTNode* after, DeltaASTNode* inserted, void* langMod)
							{ ((LanguageModule*) langMod)->NotifyInserted( parent, after, inserted); }

	void				AddCallbacks (void) {
							m_progDesc.SetOnFocusChanged(&OnFocusChangedCallback, this);
							m_progDesc.SetOnReplaced(&OnReplacedCallback, this);
							m_progDesc.SetOnDeleted(&OnDeletedCallback, this);
							m_progDesc.SetOnInserted(&OnInsertedCallback, this);
						}

	// end i-sense API.
	/////////////////////////////////////////////////////////

public:
	LanguageModule (void);
	virtual ~LanguageModule (void);

	static void Initialize (void);
	static void CleanUp (void);
	virtual void ContentSetup (void);
	virtual void ContentCleared (void);

	virtual void	SetEditor (editor::EditorWindow* editor);
	
	virtual void ContentAdded (
			uint		atPos,
			uint		length,
			uint		linesAdded
		);

	virtual void ContentDeleted (
			uint		atPos,
			uint		length,
			uint		linesDeleted
		);

	virtual void ContentOffsetChanged (
			uint		atPos,
			int			offset
		);

	virtual void CursorRepositioned (uint atPos);	// i-sense addition.
	virtual void MouseHoverStarted  (uint atPos);	// i-sense addition.
	virtual void MouseHoverEnded  (void);			// i-sense addition.
	virtual const std::string GetCursorFocusInformation (void) const;	// i-sense addition.

	virtual void CharacterAdded (int keyCode);

	virtual bool GetInfoForPosition (
			uint		pos,
			uint*		startPos,
			uint*		endPos,
			int*		type,
			String*		msg
		);

	virtual void CancelDisplayedInfo (void);

	virtual void ClearIndicators (void);

	virtual void SetExtraLibraryDefinitions (const StringList& keywords);
	virtual void SetByteCodeLoadingPath (const std::string& path);

	virtual const IntList GetHotspots (void) const;
	virtual const SerializedEditorAST GetAST (void) const;
	virtual void  SharedWriteAST (void) const;
	virtual bool  GotoGlobalDefinition (const std::string& id);

	ProgramDescription& GetProgramDesc (void) { return m_progDesc; }

	//******************************************************************
	// Visualization support
	//
	virtual wxWindow*	VisualizeContent (wxWindow* parent);
	virtual void		DestroyContentVisualization (void);

	//******************************************************************
	// Accelerator support
	//
	virtual void GetAccelerators (wxAcceleratorEntry** entry, int* size);
	virtual void DispatchAcceleratorCommand (int command);

private:
	typedef ProgramDescription::Slice Slice;

	void onVisualizerCommand (DeltaASTNode* node, int command);
	void onParseIndicatorTimerExpired (void);

	inline void setInfoIndicator (Range& savedRange, const Range& range, int mask);
	inline void clearInfoIndicator (Range& range, int mask);

	inline void updatePositions (uint atPos, int offset);
	inline void parseSlice (const Slice& slice, int updatePos = -1, int updateLinesAdded = 0);
	inline void expandAffectedSliceToIncludeBlockComments (Slice& slice);

	bool afterOpeningBrace (char brace, int* position = 0) const;
	bool shouldIncreaseIndentAfterNewLine (int line) const;
	int getPreviousOpeningBracketIndentation (char brace) const;
	int getMatchingOpeningBracketIndentation (int pos) const;
	void setIdentLevel (int c);

	void completeSymbol (long flags, int endPos, int startPos = -1);
	void completeSymbol (int endPos);

	inline void deleteText	(int startPos, int endPos);

	void commentLine		(int line, int startPos = -1);
	void uncommentLine		(int line);
	void commentSelection	(void);
	void uncommentSelection	(void);

	// AS, libsense additions.
	int	ParseBack_NamespaceAccess (int pos);
	int	ParseBack_SkipBlanks (int pos);

	void patchBinaryQualifiedId (BinaryOpASTNode* node, bool std);
	bool checkQualifiedFunction	(DeltaASTNode* node, bool *std = (bool *) 0, bool reportError = true);
	void checkNamespaces		(DeltaASTNode *node);

	Range					m_infoAtRange;
	Range					m_parseInfoAtRange;
	ProgramDescription		m_progDesc;
	ProgramDescription		m_progDiff;
	ASTreeCtrl*				m_contentVisualization;

	bool					m_showParseIndicator;
	ModuleTimer				m_parseIndicatorTimer;
	StringList				libfuncs;
	static IntList			hotspots;
};

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_LANGUAGEMODULE_H