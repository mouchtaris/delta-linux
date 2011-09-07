/**
 *	UndoRedoHelper.h
 *	
 *	Hellper class to suport undo redo.
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	Semptember 2010
 */

#ifndef UNDO_REDO_HELPER_H
#define UNDO_REDO_HELPER_H

#include <list>

namespace iviews {

	template<class UndoRedoData>
	class UndoRedoHelper {
	public:
		UndoRedoHelper() : undoStack(), redoStack() {}	
		virtual ~UndoRedoHelper(){}

		virtual void			Clean (void);
		virtual bool			CanUndo (void) const;
		virtual bool			CanRedo (void) const;
		virtual void			NewUndoAction (void);
		virtual UndoRedoData	Redo (void);
		virtual UndoRedoData	Undo (void);
		

	protected:
		typedef std::list<UndoRedoData> DataStack;
		
		DataStack undoStack;
		DataStack redoStack;

	};

	//-----------------------------------------------------------------------

	template<class UndoRedoData>
	void UndoRedoHelper<UndoRedoData>::Clean(void) {
		undoStack.clear();
		redoStack.clear();
	}

	//-----------------------------------------------------------------------

	template<class UndoRedoData>
	bool UndoRedoHelper<UndoRedoData>::CanUndo(void) const 
		{ return undoStack.size() > 0; }

	//-----------------------------------------------------------------------

	template<class UndoRedoData>
	bool UndoRedoHelper<UndoRedoData>::CanRedo(void) const 	
		{ return redoStack.size() > 0; }

	//-----------------------------------------------------------------------

	template<class UndoRedoData>
	void UndoRedoHelper<UndoRedoData>::NewUndoAction(void) { 
		undoStack.push_back(UndoRedoData()); 
		redoStack.clear();
	}

	//-----------------------------------------------------------------------

	template<class UndoRedoData>
	UndoRedoData UndoRedoHelper<UndoRedoData>::Redo(void){
		assert(CanRedo());
		
		UndoRedoData lastRedoAction = redoStack.back();
		redoStack.pop_back();
		undoStack.push_back(lastRedoAction);
		return lastRedoAction;
	}

	//-----------------------------------------------------------------------

	template<class UndoRedoData>
	UndoRedoData UndoRedoHelper<UndoRedoData>::Undo(void){
		assert(CanUndo());

		UndoRedoData lastUndoAction = undoStack.back();
		undoStack.pop_back();
		redoStack.push_back(lastUndoAction);
		return lastUndoAction;
	}

}	//namespace iviews 

#endif //UNDO_REDO_HELPER_H