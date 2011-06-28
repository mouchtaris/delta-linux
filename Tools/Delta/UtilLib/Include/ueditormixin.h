// ueditormixin.h
// A generic editor mixin layer supporting undo / redo
// and the clipboard functionality.
// It is to be refined by derived editor classes as needed.
// Aims to address many different editors' needs (e.g. paint canvas,
// graphic primitive editor, text editor).
// A. Savidis, October 2006.
//

#ifndef	UEDITORMIXIN_H
#define	UEDITORMIXIN_H

#include <stack>
#include <algorithm>
#include <string>
#include "ustlhelpers.h"
#include "utypes.h"
#include "DDebug.h"

//---------------------------------------------------------------

class UTILLIB_CLASS ueditor {

	protected:
	static ueditor* editor;

	///////////////////////////////////////////////////////
	// MIXIN LAYER REFINABLE CLASSES (Abstract).
	//

	public:

	//**************************
	// It represents the unambiguous identification of an
	// editing context, affecting a single item. See it as the topological
	// analogy of the up-left corner of a bounding-box.
	//
	class UTILLIB_CLASS uposition {	// Abstract, editing position.
		public:
		virtual util_ui16	classid (void) const = 0;
		virtual uposition*	clone (void) const = 0;

		virtual const std::string description (void) const = 0;
	
		virtual ~uposition(){}
	};

	//**************************
	// It represents a collective editing context, meaning derived
	// classes should refine so that it can cover whole blocks, selected items,
	// or list of isolated items (not necessarily blocked).
	//
	class UTILLIB_CLASS uselection {
		protected:
		mutable uposition* pos;					// Major position of the collection.
		virtual void makepos (void) const = 0;	// Production of the position when needed.
		bool isActive;

		public:
		
		virtual util_ui16	classid (void) const = 0;
		virtual void		clear (void){}
		virtual uselection* clone (void) const = 0;

		const uposition& toposition (void) const 
				{ makepos(); DASSERT(pos); return *pos; }

		virtual uselection* offsetby (const uposition&) const = 0;	// Dynamic copy.

		bool isactive (void) const 
			{ return isActive; }

		void inactivate (void) 
			{ isActive = false; clear(); }

		virtual void activate (void) 
			{ isActive = true; }

		virtual const std::string description (void) const = 0;

		uselection (void) : pos((uposition*) 0), isActive(false){}
		virtual ~uselection(){ clear(); if (pos) DDELETE(pos); }
	};

	//**************************
	// Editing internal data for all editing operations.
	// It ranges from pure editing items to item attributes.
	// Clearly refined versions should distinguish as needed.
	// It holds internally its collective context and starting
	// context position. Derived classes should allow data to 
	// be aware of their original editing area.
	//
	class UTILLIB_CLASS udata {

		protected:
		bool isNormalized;
		mutable uselection* sel;

		virtual void makesel (void) const = 0;	// Production of the selection when needed.
		virtual void normalizeprivate (void){}	// Should be refined as needed.

		public:
		virtual util_ui16	classid (void) const = 0;
		virtual udata*		clone (void) const = 0;
		const uposition&	toposition (void) const 
								{ return toselection().toposition(); }
		const uselection&	toselection (void) const 
								{ makesel(); return *sel; }
		bool				isnormalized (void) const 
								{ return isNormalized; }
		void				setisnormalized (bool val) 
								{ isNormalized = val; }
		void				normalize (void);

		virtual const std::string description (void) const = 0;

		udata (void) : isNormalized(false), sel((uselection*) 0){}
		virtual ~udata(){ if (sel) DDELETE(sel); }
	};

	//**************************
	// Editing actions. Strangely, all classes, but the
	// the super one, are concrete. Specialization is defined
	// at the level of low-level editor operations latter.
	//

	class UTILLIB_CLASS uaction {	// Abstract
		public:
		virtual uaction*	opposite(void) const = 0;	// The -A generator.
		virtual void		apply(void) const = 0;		// The A() functor.

		struct validator { virtual bool operator()(udata*) = 0; };
		virtual bool isvalid(validator& f) = 0;

		virtual const std::string description (void) const = 0;

		virtual ~uaction(){}
	};

	//**************************

	class UTILLIB_CLASS udeleteaction;

	class UTILLIB_CLASS uinsertaction : public uaction {	// Concrete.

		friend class udeleteaction;
		private:
		uposition*	pos;
		udata*		curr;
		udata*		old;

		uinsertaction (	// Direct assign, parameterized constructor.
			const uposition&	_pos,
			const udata&		_curr,
			const udata*		_old
		):	pos(_pos.clone()), 
			curr(_curr.clone()), 
			old(_old ? _old->clone() : (udata*) 0){}

		public:
		uaction*	clone (void) const;
		uaction*	opposite(void) const;
		void		apply(void) const;

		bool		isvalid (validator& f) 
						{ return f(curr) && (!old || f(old)); }

		const std::string description (void) const;

		uinsertaction (const uinsertaction&);
		uinsertaction (const uposition&, const udata& data);
		~uinsertaction();
	};
	
	friend class UTILLIB_CLASS uinsertaction;

	//**************************

	class UTILLIB_CLASS udeleteaction : public uaction {	// Concrete.

		friend class uinsertaction;
		private:
		uselection*	area;
		udata*		old;

		udeleteaction (	// Explicit parameterized when opposite of insert.
			const uselection&	area,
			const udata&		old
		);

		public:
		uaction*	clone (void) const;
		uaction*	opposite(void) const;
		void		apply(void) const;

		bool		isvalid(validator& f) 
						{ return f(old); }

		const std::string description (void) const;

		udeleteaction (const udeleteaction&);
		udeleteaction (const uselection& area);
		~udeleteaction();
	};
	
	friend class udeleteaction;

	//**************************

	class UTILLIB_CLASS uattrs {	// Abstract.
		public:
		virtual util_ui16	classid (void) const = 0;
		virtual uattrs*		clone (void) const = 0;

		virtual const std::string	description (void) const = 0;

		virtual ~uattrs(){}
	};

	//**************************
	// Represents setting of attributes to editor elements.
	//
	class UTILLIB_CLASS usetattrsaction : public uaction {	// Concrete.

		private:
		uselection*	area;
		uattrs*		attrs;
		udata*		old;
		bool		sw;	// T: apply 'attrs', F: apply insert 'old'.

		usetattrsaction (	// Explicit parameterized ctor.
			const uselection&	area,	 
			const uattrs&		attrs,
			const udata&		old,
			bool				sw
		);

		public:
		uaction*	clone (void) const;
		uaction*	opposite(void) const;
		void		apply(void) const;

		bool		isvalid(validator& f) 
						{ return f(old); }

		const std::string description (void) const;

		usetattrsaction (
				const uselection&	area,	 
				const uattrs&		attrs
			);
		usetattrsaction (const usetattrsaction&);
		~usetattrsaction();
	};
	
	friend class UTILLIB_CLASS usetattrsaction;

	///////////////////////////////////////////////////////
	// UNDO / REDO PATTERN IMPLEMENTATION.
	//
	protected:

	friend class UTILLIB_CLASS uundoredo;

	class UTILLIB_CLASS uundoredo {
		private:
		std::list<uaction*>	undoStack;
		std::list<uaction*>	redoStack;
		uaction::validator& fv;

		// Removes all actions with invalid data at this point.
		//
		void removeinvalidactions(std::list<uaction*>& l) {
			std::list<uaction*>::iterator i = l.begin();
			while (i != l.end()) {
				if (!(*i)->isvalid(fv)){
					std::list<uaction*>::iterator del = i;
					DDELETE(*del);	// Destroy action.
					++i;
					l.erase(del);	// And remove it.
				}
				else
					++i;
			}
		}		
		
		// The basic symmetric undo / redo functionality.
		//
		void undoredo(std::list<uaction*>& undo, std::list<uaction*>& redo) {

			removeinvalidactions(undo);
			removeinvalidactions(redo);

			if (!undo.empty()) {
				uaction* action = *(undo.begin());
				undo.pop_front();							// Pop -A (+A) from undo (redo) stack.
				redo.push_front(DPTR(action)->opposite());	// Push +A (-A) on redo (undo) stack.
				DPTR(action)->apply();						// Call -A (+A).
				DDELETE(action);
			}
		}

		const std::string description(const char* id, const std::list<uaction*>& st) const;

		public:
		void onedit (const uaction& action) { 
			ulistclear<uaction>()(redoStack);				// Clear redo stack.
			undoStack.push_front(action.opposite());		// Push -A on undo stack.
			action.apply();									// Call +A.
		}

		void undo (void) 
			{ undoredo(undoStack, redoStack); }

		void redo (void)
			{ undoredo(redoStack, undoStack); }
	
		void clear (void) {
			ulistclear<uaction>()(undoStack);
			ulistclear<uaction>()(redoStack);
		}

		const std::string description (void) const;

		uundoredo (uaction::validator& f) : fv(f){}
		~uundoredo() { clear(); }
	};

	uundoredo undoRedo;	// The main embedded handler.
	
	///////////////////////////////////////////////////////
	// Next are editor functions to be defined by derived editors.
	//

	// The 'insertaffected' returns data that are affected 
	// after an insert. For typical text editor insert mode this
	// may be null, but for overwrite mode as in paint editors,
	// it may be the copied affected area below the inserted.
	//
	virtual	udata*	insertaffected (const uselection&) = 0;
	virtual	udata*	deepcopy (const uselection&) = 0;	// Pure clipbaord copy.
	virtual	udata*	undocopy (const uselection&) = 0;	// For cut / remove / settatrs (undo purpose).
	virtual bool	validate (udata* data) = 0;			// Validates copied data.

	virtual void	setattrs (const uselection&, const uattrs&) = 0;
	virtual void	insert (const uposition&, const udata&) = 0;
	virtual void	remove (const uselection&) = 0;

	///////////////////////////////////////////////////////
	// Clipboard staff. Refinable, but conrete.
	//
	private:
	static udata*	clipboard;		// Shared clipboard among all editors.
	uselection*		currSelection;	// If editing data are selected.
	uposition*		focusPosition;	// The position of the editing focus.

	///////////////////////////////////////////////////////
	// Validation of stored action data.
	//
	struct actionvalidator;
	friend struct actionvalidator;

	struct UTILLIB_CLASS actionvalidator : public uaction::validator {
		ueditor* e;
		bool operator()(udata* data) 
			{ return e->validate(data); }
		actionvalidator (ueditor* _e) : e(_e){}
	};
	actionvalidator av;

	///////////////////////////////////////////////////////

	protected:
	void setselection (uselection* p) 
		{ currSelection = p; }

	void setfocusposition (uposition* p)
		{ focusPosition = p; }

	udata* getclipboard (void) const
			{ return clipboard; }

	virtual void cut (void);
	virtual void copy (void);
	virtual void paste (void);
	virtual void remove (void);
	virtual void undo (void) { undoRedo.undo(); }
	virtual void redo (void) { undoRedo.redo(); }
	virtual void edit (const uaction& action) { undoRedo.onedit(action); }

	///////////////////////////////////////////////////////

	virtual void setcurrenteditor (void) { editor = this; }	

	ueditor (void);
	virtual ~ueditor();
};

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.

